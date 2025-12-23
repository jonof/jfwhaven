#import <Cocoa/Cocoa.h>

#include "compat.h"
#include "baselayer.h"
#include "startwin.h"
#include "build.h"
// #include "tekwar.h"

static struct soundQuality_t {
    int frequency;
    int samplesize;
    int channels;
} soundQualities[] = {
    { 44100, 16, 2 },
    { 22050, 16, 2 },
    { 11025, 16, 2 },
    { 0, 0, 0 },    // May be overwritten by custom sound settings.
    { 0, 0, 0 },
};

#include <stdlib.h>

// Callbacks to glue the C in datascan.c with the Objective-C here.
static void importmeta_progress(void *data, const char *path);
static int importmeta_cancelled(void *data);

@interface StartupWinController : NSWindowController <NSWindowDelegate>
{
    BOOL quiteventonclose;
    NSThread *importthread;
    BOOL inmodal;
    struct startwin_settings *settings;

    IBOutlet NSButton *alwaysShowButton;
    IBOutlet NSButton *fullscreenButton;
    IBOutlet NSTextView *messagesView;
    IBOutlet NSTabView *tabView;
    IBOutlet NSTabViewItem *tabConfig;
    IBOutlet NSTabViewItem *tabMessages;
    IBOutlet NSPopUpButton *videoMode3DPUButton;
    IBOutlet NSPopUpButton *displayPUButton;

    IBOutlet NSPopUpButton *soundQualityPUButton;
    IBOutlet NSButton *useMouseButton;
    IBOutlet NSButton *useJoystickButton;

    IBOutlet NSButton *chooseImportButton;
    IBOutlet NSButton *importInfoButton;

    IBOutlet NSButton *cancelButton;
    IBOutlet NSButton *startButton;

    IBOutlet NSWindow *importStatusWindow;
    IBOutlet NSTextField *importStatusText;
    IBOutlet NSButton *importStatusCancel;
}

- (int)modalRun;
- (void)closeQuietly;
- (void)populateVideoModes:(BOOL)firstTime;
- (void)populateSoundQuality:(BOOL)firstTime;

- (IBAction)fullscreenOrDisplayClicked:(id)sender;

- (IBAction)chooseImportClicked:(id)sender;
- (IBAction)importInfoClicked:(id)sender;
- (IBAction)importStatusCancelClicked:(id)sender;
- (void)updateImportStatusText:(NSString *)text;
- (void)doImport:(NSString *)path;
- (void)doneImport:(NSNumber *)result;
- (BOOL)isImportCancelled;

- (IBAction)cancel:(id)sender;
- (IBAction)start:(id)sender;

- (void)setupConfigMode;
- (void)setupMessagesMode:(BOOL)allowCancel;
- (void)putsMessage:(NSString *)str;
- (void)setTitle:(NSString *)str;
- (void)setSettings:(struct startwin_settings *)theSettings;

@end

@implementation StartupWinController

- (void)windowDidLoad
{
    quiteventonclose = TRUE;
}

- (BOOL)windowShouldClose:(id)sender
{
    if (inmodal) {
        [NSApp stopModalWithCode:STARTWIN_CANCEL];
    }
    quitevent = quitevent || quiteventonclose;
    return NO;
}

- (int)modalRun
{
    int retval;

    inmodal = YES;
    switch ([NSApp runModalForWindow:[self window]]) {
        case STARTWIN_RUN: retval = STARTWIN_RUN; break;
        case STARTWIN_CANCEL: retval = STARTWIN_CANCEL; break;
        default: retval = -1;
    }
    inmodal = NO;

    return retval;
}

// Close the window, but don't cause the quitevent flag to be set
// as though this is a cancel operation.
- (void)closeQuietly
{
    quiteventonclose = FALSE;
    [self close];
}

- (void)populateVideoModes:(BOOL)firstTime
{
    int i, mode3d = -1, idx3d = -1;
    int xdim = 0, ydim = 0, bitspp = 0, display = 0, fullsc = 0;
    int cd[] = { 32, 24, 16, 15, 8, 0 };
    NSMenu *menu3d = nil;
    NSMenuItem *menuitem = nil;

    if (firstTime) {
        getvalidmodes();
        xdim = settings->xdim3d;
        ydim = settings->ydim3d;
        bitspp = settings->bpp3d;
        fullsc = settings->fullscreen & 255;
        display = min(displaycnt-1, max(0, (settings->fullscreen >> 8)));

        NSMenu *menu = [displayPUButton menu];
        [menu removeAllItems];
        for (int i = 0; i < displaycnt; i++) {
            menuitem = [menu addItemWithTitle:[NSString stringWithFormat:@"Display %d \u2013 %s",
                                               i, getdisplayname(i)]
                                       action:nil
                                keyEquivalent:@""];
            [menuitem setTag:i];
        }
        if (displaycnt < 2) [displayPUButton setHidden:YES];
    } else {
        fullsc = ([fullscreenButton state] == NSControlStateValueOn);
        if (fullsc) display = max(0, (int)[displayPUButton selectedTag]);
        mode3d = (int)[videoMode3DPUButton selectedTag];
        if (mode3d >= 0) {
            xdim = validmode[mode3d].xdim;
            ydim = validmode[mode3d].ydim;
            bitspp = validmode[mode3d].bpp;
        }
    }

    // Find an ideal match.
    mode3d = checkvideomode(&xdim, &ydim, bitspp, SETGAMEMODE_FULLSCREEN(display, fullsc), 1);
    for (i=0; mode3d < 0 && cd[i]; i++) {
        mode3d = checkvideomode(&xdim, &ydim, cd[i], SETGAMEMODE_FULLSCREEN(display, fullsc), 1);
    }
    if (mode3d < 0) mode3d = 0;
    fullsc = validmode[mode3d].fs;
    display = validmode[mode3d].display;

    // Repopulate the mode lists.
    menu3d = [videoMode3DPUButton menu];
    [menu3d removeAllItems];

    for (i = 0; i < validmodecnt; i++) {
        if (validmode[i].fs != fullsc) continue;
        if (validmode[i].display != display) continue;

        if (i == mode3d || idx3d < 0) idx3d = i;
        menuitem = [menu3d addItemWithTitle:[NSString stringWithFormat:@"%d \u00d7 %d %d-bpp",
                                             validmode[i].xdim, validmode[i].ydim,
                                             validmode[i].bpp]
                                     action:nil
                              keyEquivalent:@""];
        [menuitem setTag:i];
    }
    if (idx3d >= 0) [videoMode3DPUButton selectItemWithTag:idx3d];

    [displayPUButton selectItemWithTag:validmode[mode3d].display];
    [displayPUButton setEnabled: validmode[mode3d].fs ? YES : NO];
    [fullscreenButton setState: validmode[mode3d].fs ? NSControlStateValueOn : NSControlStateValueOff];
}

- (void)populateSoundQuality:(BOOL)firstTime
{
    int i, curidx = -1;
    NSMenu *menu = nil;
    NSMenuItem *menuitem = nil;

    if (firstTime) {
        for (i = 0; soundQualities[i].frequency > 0; i++) {
            if (soundQualities[i].frequency == settings->samplerate &&
                soundQualities[i].samplesize == settings->bitspersample &&
                soundQualities[i].channels == settings->channels) {
                curidx = i;
                break;
            }
        }
        if (curidx < 0) {
            soundQualities[i].frequency = settings->samplerate;
            soundQualities[i].samplesize = settings->bitspersample;
            soundQualities[i].channels = settings->channels;
        }
    }

    menu = [soundQualityPUButton menu];
    [menu removeAllItems];

    for (i = 0; soundQualities[i].frequency > 0; i++) {
        menuitem = [menu addItemWithTitle:[NSString stringWithFormat:@"%d kHz, %d-bit, %s",
                                          soundQualities[i].frequency / 1000,
                                          soundQualities[i].samplesize,
                                          soundQualities[i].channels == 1 ? "Mono" : "Stereo"]
                                   action:nil
                            keyEquivalent:@""];
        [menuitem setTag:i];
    }

    if (curidx >= 0) [soundQualityPUButton selectItemAtIndex:curidx];
}

- (IBAction)fullscreenOrDisplayClicked:(id)sender
{
    [self populateVideoModes:NO];
}

- (IBAction)chooseImportClicked:(id)sender
{
    @autoreleasepool {
        NSOpenPanel *panel = [NSOpenPanel openPanel];

        [panel setTitle:@"Import game data"];
        [panel setPrompt:@"Import"];
        [panel setMessage:@"Select a folder to search."];
        [panel setCanChooseFiles:FALSE];
        [panel setCanChooseDirectories:TRUE];
        [panel setShowsHiddenFiles:TRUE];
        [panel beginSheetModalForWindow:[self window]
                      completionHandler:^void (NSModalResponse resp) {
            if (resp == NSModalResponseOK) {
                NSURL *file = [panel URL];
                if ([file isFileURL]) {
                    [self doImport:[file path]];
                }
            }
        }];
    }
}

- (IBAction)importStatusCancelClicked:(id)sender
{
    [importthread cancel];
}

- (void)updateImportStatusText:(NSString *)text
{
    [importStatusText setStringValue:text];
}

- (void)doImport:(NSString *)path
{
    if ([importthread isExecuting]) {
        NSLog(@"import thread is already executing");
        return;
    }

    // Put up the status sheet which becomes modal.
    [[self window] beginSheet:importStatusWindow
            completionHandler:nil];

    // Spawn a thread to do the scan.
    importthread = [[NSThread alloc] initWithBlock:^void(void) {
        struct importdatameta meta = {
            (void *)self,
            importmeta_progress,
            importmeta_cancelled
        };
        int result = ImportDataFromPath([path UTF8String], &meta);
        [self performSelectorOnMainThread:@selector(doneImport:)
                               withObject:[NSNumber numberWithInt:result]
                            waitUntilDone:FALSE];
    }];
    [importthread start];
}

// Finish up after the import thread returns.
- (void)doneImport:(NSNumber *)result
{
    [importStatusWindow orderOut:nil];
    [[self window] endSheet:importStatusWindow
                 returnCode:1];
}

// Report on whether the import thread has been been cancelled early.
- (BOOL)isImportCancelled
{
    return [importthread isCancelled];
}

- (IBAction)importInfoClicked:(id)sender
{
    @autoreleasepool {
        NSAlert *alert = [[NSAlert alloc] init];
        NSURL *sharewareurl = [NSURL URLWithString:@"https://www.jonof.id.au/files/jfwhaven/whcdpreview.zip"];

        [alert setAlertStyle:NSAlertStyleInformational];
        [alert setMessageText:@"JFWitchaven can scan locations of your choosing for Witchaven game data"];
        [alert setInformativeText:@"Click the 'Choose a location...' button, then locate "
            @"a folder to scan.\n\n"
            @"Common locations to check include:\n"
            @" • CD/DVD drives\n"
            @" • Unzipped data from copies of the full DOS game\n\n"
            @"To play the 3-level preview version, download the preview data (whcdpreview.zip), "
            @"unzip the file, then select the WHAVEN folder with the 'Choose a location...' option."];
        [alert addButtonWithTitle:@"OK"];
        [alert addButtonWithTitle:@"Download Preview"];
        switch ([alert runModal]) {
            case NSAlertFirstButtonReturn:
                break;
            case NSAlertSecondButtonReturn:
                LSOpenCFURLRef((CFURLRef)sharewareurl, nil);
                break;
        }
    }
}

- (IBAction)cancel:(id)sender
{
    if (inmodal) {
        [NSApp stopModalWithCode:STARTWIN_CANCEL];
    }
    quitevent = quitevent || quiteventonclose;
}

- (IBAction)start:(id)sender
{
    int mode = -1;

    mode = (int)[videoMode3DPUButton selectedTag];
    if (mode >= 0) {
        settings->xdim3d = validmode[mode].xdim;
        settings->ydim3d = validmode[mode].ydim;
        settings->bpp3d = validmode[mode].bpp;
        settings->fullscreen = SETGAMEMODE_FULLSCREEN(validmode[mode].display, validmode[mode].fs);
    }

    settings->usemouse = [useMouseButton state] == NSControlStateValueOn;
    settings->usejoy = [useJoystickButton state] == NSControlStateValueOn;

    mode = (int)[[soundQualityPUButton selectedItem] tag];
    if (mode >= 0) {
        settings->samplerate = soundQualities[mode].frequency;
        settings->bitspersample = soundQualities[mode].samplesize;
        settings->channels = soundQualities[mode].channels;
    }

    settings->forcesetup = [alwaysShowButton state] == NSControlStateValueOn;

    if (inmodal) {
        [NSApp stopModalWithCode:STARTWIN_RUN];
    }
}

- (void)setupConfigMode
{
    [alwaysShowButton setState: (settings->forcesetup ? NSControlStateValueOn : NSControlStateValueOff)];
    [alwaysShowButton setEnabled:YES];

    [videoMode3DPUButton setEnabled:YES];
    [fullscreenButton setEnabled:YES];
    [displayPUButton setEnabled:YES];
    [self populateVideoModes:YES];

    [soundQualityPUButton setEnabled:YES];
    [self populateSoundQuality:YES];
    [useMouseButton setEnabled:YES];
    [useMouseButton setState: (settings->usemouse ? NSControlStateValueOn : NSControlStateValueOff)];
    [useJoystickButton setEnabled:YES];
    [useJoystickButton setState: (settings->usejoy ? NSControlStateValueOn : NSControlStateValueOff)];

    [cancelButton setEnabled:YES];
    [startButton setEnabled:YES];

    [chooseImportButton setEnabled:YES];
    [importInfoButton setEnabled:YES];

    [tabView selectTabViewItem:tabConfig];
    [NSCursor unhide];  // Why should I need to do this?
}

- (void)setupMessagesMode:(BOOL)allowCancel
{
    [tabView selectTabViewItem:tabMessages];

    // disable all the controls on the Configuration page
    NSEnumerator *enumerator = [[[tabConfig view] subviews] objectEnumerator];
    NSControl *control;
    while (control = [enumerator nextObject]) {
        [control setEnabled:false];
    }

    [chooseImportButton setEnabled:NO];
    [importInfoButton setEnabled:NO];

    [alwaysShowButton setEnabled:NO];

    [cancelButton setEnabled:allowCancel];
    [startButton setEnabled:false];
}

- (void)putsMessage:(NSString *)str
{
    NSRange end;
    NSTextStorage *text = [messagesView textStorage];
    BOOL shouldAutoScroll;

    shouldAutoScroll = ((int)NSMaxY([messagesView bounds]) == (int)NSMaxY([messagesView visibleRect]));

    end.location = [text length];
    end.length = 0;

    [text beginEditing];
    [messagesView replaceCharactersInRange:end withString:str];
    [text endEditing];

    if (shouldAutoScroll) {
        end.location = [text length];
        end.length = 0;
        [messagesView scrollRangeToVisible:end];
    }
}

- (void)setTitle:(NSString *)str
{
    [[self window] setTitle:str];
}

- (void)setSettings:(struct startwin_settings *)theSettings
{
    settings = theSettings;
}

@end

static StartupWinController *startwin = nil;

int startwin_open(void)
{
    if (startwin != nil) return 1;

    @autoreleasepool {
        startwin = [[StartupWinController alloc] initWithWindowNibName:@"startwin.game"];
        if (startwin == nil) return -1;

        NSWindow *win = [startwin window];  // Forces the window controls on the controller to be initialised.
        if (win == nil) {
            [startwin closeQuietly];
            [startwin release];
            startwin = nil;
            return -1;
        }
        [startwin setupMessagesMode:YES];
        [startwin showWindow:nil];

        return 0;
    }
}

int startwin_close(void)
{
    if (startwin == nil) return 1;

    @autoreleasepool {
        [startwin closeQuietly];
        [startwin release];
        startwin = nil;

        return 0;
    }
}

int startwin_puts(const char *s)
{
    if (!s) return -1;
    if (startwin == nil) return 1;

    @autoreleasepool {
        NSString *str = [NSString stringWithUTF8String:s];
        if ([NSThread isMainThread]) {
            [startwin putsMessage:str];
        } else {
            [startwin performSelectorOnMainThread:@selector(putsMessage:)
                                       withObject:str
                                    waitUntilDone:TRUE];
        }

        return 0;
    }
}

int startwin_settitle(const char *s)
{
    if (!s) return -1;
    if (startwin == nil) return 1;

    @autoreleasepool {
        [startwin setTitle:[NSString stringWithUTF8String:s]];

        return 0;
    }
}

int startwin_idle(void *v)
{
    (void)v;
    return 0;
}

int startwin_run(struct startwin_settings *settings)
{
    int retval;

    if (startwin == nil) return STARTWIN_RUN;

    @autoreleasepool {
        [startwin setSettings:settings];

        [startwin setupConfigMode];
        retval = [startwin modalRun];
        [startwin setupMessagesMode: FALSE];

        return retval;
    }
}

// Callback for the C-universe ImportGroupsFrom*() to notify the UI in Obj-C land.
static void importmeta_progress(void *data, const char *path)
{
    StartupWinController *control = (StartupWinController *)data;

    @autoreleasepool {
        [control performSelectorOnMainThread:@selector(updateImportStatusText:)
                                  withObject:[NSString stringWithUTF8String:path]
                               waitUntilDone:FALSE];
    }
}

// Callback for the C-universe ImportGroupsFrom*() to discover they've been cancelled by the UI.
static int importmeta_cancelled(void *data)
{
    StartupWinController *control = (StartupWinController *)data;
    return [control isImportCancelled];
}
