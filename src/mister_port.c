// MiSTer hybrid core support, see mister_port.h. After jfSW_MiSTer's
// jfsw/src/mister_port.c (ItsDanik), cut down to one game without GRP files:
// Witchaven's data are loose files in the game's folder.

#include "mister_port.h"

#include <mister_hybrid.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EXIT_TO_MENU 0
#define EXIT_RESTART 42 // danik_hybrid_launch.sh starts the game again
#define EXIT_RELOADED 43 // the same, after the core has settled

static const char *const TITLE = "jfSW";

// The engine of the MiSTer branch has no voxels; the game still refers to this
int nextvoxid = 0;

// Resolution: the video mode of the core (OSD status bits 24..25). One the
// core does not have is the one of 200 lines; 320x200 without a core.
static int ScreenMode(void)
{
    static const int option[4] = { MH_MODE_320x200, MH_MODE_640x200, MH_MODE_320x240, MH_MODE_640x240 };
    int mode;

    if (!MH_IsOpen())
        return MH_MODE_320x200;
    mode = option[MH_OSD_GAME_BITS(MH_OSDStatus(), 24, 2)];
    if (!MH_ModeAvailable(mode))
        mode = MH_ModeWidth(mode) == 640 ? MH_MODE_640x200 : MH_MODE_320x200;
    return mode;
}

int MiSTer_ScreenWidth(void)
{
    return MH_ModeWidth(ScreenMode());
}

int MiSTer_ScreenHeight(void)
{
    return MH_ModeHeight(ScreenMode());
}

// The player came through the list: quitting a game goes back to it
static int PickedFromList = 0;

void MiSTer_ShowError(const char *message)
{
    if (MH_Open())
        MH_UI_Message(TITLE, message, MH_UI_WAIT | MH_UI_ERROR);
}

void MiSTer_Loading(void)
{
    MH_Open();
}

// The game of the menu entry (set name jfSW_WHAVEN): known by the files of
// the game's folder the port reads
static const char *const GameFiles[] = { "TILES000.ART", "PALETTE.DAT", "LOOKUP.DAT", "LEVEL1.MAP", NULL };
#define NO_GAME "Witchaven was not found. Copy everything in the game's folder (the one with TILES000.ART, the LEVEL*.MAP files, JOESND and SONGS) to %s/."

void MiSTer_PickGame(void)
{
    mh_game games[1];
    int listed = 0, pick;

    games[0].id = "WHAVEN";
    games[0].name = "Witchaven";
    games[0].files = GameFiles;
    games[0].shareware_of = NULL;
    games[0].missing = NO_GAME;
    pick = MH_PickGame(games, 1, NULL, NO_GAME, &listed);
    if (pick < 0)
        exit(0);
    PickedFromList = listed;
}

// Runs after everything else of exit(), whichever of the game's ways out was
// taken
static void ExitWithCode(void)
{
    _exit(MiSTer_ExitCode());
}

void MiSTer_Init(void)
{
    atexit(ExitWithCode);
}

int MiSTer_ExitCode(void)
{
    if (MH_CoreReloaded())
        return EXIT_RELOADED;
    if (PickedFromList && MH_Open())
        return EXIT_RESTART;
    return EXIT_TO_MENU;
}
