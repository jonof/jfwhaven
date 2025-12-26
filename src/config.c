// Evil and Nasty Configuration File Reader for KenBuild
// Repurposed for JFWitchaven
// by Jonathon Fowler

#include "compat.h"
#include "build.h"
#include "osd.h"
#include "scriptfile.h"
#include "icorp.h"
#include "baselayer.h"

enum {
    type_bool = 0,    //int
    type_double = 1,
    type_int = 2,
    type_hex = 3,
};

#if USE_POLYMOST
static int tmprenderer = -1;
#endif
static unsigned tmpmaxrefreshfreq = 0;
static int tmpfullscreen = -1, tmpdisplay = -1;
static int tmpbrightness = -1;
static int tmpsamplerate = -1;
static int tmpmusic = -1;
static int tmpmouse = -1;
static int tmpjoystick = -1;

static struct {
    const char *name;
    int type;
    void *store;
    const char *doc;
} configspec[] = {
    { "forcesetup", type_bool, &forcesetup,
        "; Always show configuration options on startup\n"
        ";   0 - No\n"
        ";   1 - Yes\n"
    },
    { "fullscreen", type_bool, &tmpfullscreen,
        "; Video mode selection\n"
        ";   0 - Windowed\n"
        ";   1 - Fullscreen\n"
    },
    { "display", type_int, &tmpdisplay,
        "; Video display number\n"
        ";   0 - Primary display\n"
    },
    { "xdim", type_int, &xdimgame,
        "; Video resolution\n"
    },
    { "ydim", type_int, &ydimgame, NULL },
    { "bpp",  type_int, &bppgame,
        "; Video colour depth\n"
    },
#if USE_POLYMOST
    { "renderer", type_int, &tmprenderer,
        "; Renderer type\n"
        ";   0  - classic\n"
        ";   3  - OpenGL Polymost\n"
    },
#endif
    { "brightness", type_int, &tmpbrightness,
        "; 3D mode brightness setting\n"
        ";   0 - lowest\n"
        ";   8 - highest\n"
    },
    { "usegammabrightness", type_bool, &usegammabrightness,
        "; Brightness setting method\n"
        ";   0 - palette\n"
#if USE_OPENGL
        ";   1 - shader gamma\n"
#endif
        ";   2 - system gamma\n"
    },
#if USE_POLYMOST && USE_OPENGL
    { "glusetexcache", type_bool, &glusetexcache,
        "; OpenGL mode options\n"
    },
#endif
    { "maxrefreshfreq", type_int, &tmpmaxrefreshfreq,
        "; Maximum fullscreen mode refresh rate (in Hertz, 0 indicates no limit)\n"
    },
    { "samplerate", type_int, &tmpsamplerate,
        "; Sound sample frequency\n"
        ";   0 - 6 KHz\n"
        ";   1 - 8 KHz\n"
        ";   2 - 11.025 KHz\n"
        ";   3 - 16 KHz\n"
        ";   4 - 22.05 KHz\n"
        ";   5 - 32 KHz\n"
        ";   6 - 44.1 KHz\n"
        ";   7 - 48 KHz\n"
    },
    { "digilevel", type_int, &digilevel,
        "; Sound volume level\n"
        ";   0 - Silent\n"
        ";  16 - Loud\n"
    },
    { "music", type_bool, &tmpmusic,
        "; Music playback\n"
        ";   0 - Off\n"
        ";   1 - On\n"
    },
    { "musiclevel", type_int, &musiclevel,
        "; Music volume level\n"
        ";   0 - Silent\n"
        ";  16 - Loud\n"
    },
    { "mouse", type_bool, &tmpmouse,
        "; Enable mouse\n"
        ";   0 - No\n"
        ";   1 - Yes\n"
    },
    { "joystick", type_bool, &tmpjoystick,
        "; Enable joystick\n"
        ";   0 - No\n"
        ";   1 - Yes\n"
    },
    { "keyforward", type_hex, &keys[KEYFWD],
        "; Key Settings\n"
        ";  Here's a map of all the keyboard scan codes: NOTE: values are listed in hex!\n"
        "; +---------------------------------------------------------------------------------------------+\n"
        "; | 01   3B  3C  3D  3E   3F  40  41  42   43  44  57  58          46                           |\n"
        "; |ESC   F1  F2  F3  F4   F5  F6  F7  F8   F9 F10 F11 F12        SCROLL                         |\n"
        "; |                                                                                             |\n"
        "; |29  02  03  04  05  06  07  08  09  0A  0B  0C  0D   0E     D2  C7  C9      45  B5  37  4A   |\n"
        "; | ` '1' '2' '3' '4' '5' '6' '7' '8' '9' '0'  -   =  BACK    INS HOME PGUP  NUMLK KP/ KP* KP-  |\n"
        "; |                                                                                             |\n"
        "; | 0F  10  11  12  13  14  15  16  17  18  19  1A  1B  2B     D3  CF  D1      47  48  49  4E   |\n"
        "; |TAB  Q   W   E   R   T   Y   U   I   O   P   [   ]    \\    DEL END PGDN    KP7 KP8 KP9 KP+   |\n"
        "; |                                                                                             |\n"
        "; | 3A   1E  1F  20  21  22  23  24  25  26  27  28     1C                     4B  4C  4D       |\n"
        "; |CAPS  A   S   D   F   G   H   J   K   L   ;   '   ENTER                    KP4 KP5 KP6    9C |\n"
        "; |                                                                                      KPENTER|\n"
        "; |  2A    2C  2D  2E  2F  30  31  32  33  34  35    36            C8          4F  50  51       |\n"
        "; |LSHIFT  Z   X   C   V   B   N   M   ,   .   /   RSHIFT          UP         KP1 KP2 KP3       |\n"
        "; |                                                                                             |\n"
        "; | 1D     38              39                  B8     9D       CB  D0   CD      52    53        |\n"
        "; |LCTRL  LALT           SPACE                RALT   RCTRL   LEFT DOWN RIGHT    KP0    KP.      |\n"
        "; +---------------------------------------------------------------------------------------------+\n"
    },
    { "keybackward", type_hex, &keys[KEYBACK], NULL },
    { "keyturnleft", type_hex, &keys[KEYLEFT], NULL },
    { "keyturnright", type_hex, &keys[KEYRIGHT], NULL },
    { "keyrun", type_hex, &keys[KEYRUN], NULL },
    { "keystrafe", type_hex, &keys[KEYSTRAFE], NULL },
    { "keyfire", type_hex, &keys[KEYFIRE], NULL },
    { "keyuse", type_hex, &keys[KEYUSE], NULL },
    { "keyjump", type_hex, &keys[KEYJUMP], NULL },
    { "keycrouch", type_hex, &keys[KEYCROUCH], NULL },
    { "keylookup", type_hex, &keys[KEYLKUP], NULL },
    { "keylookdown", type_hex, &keys[KEYLKDN], NULL },
    { "keycentre", type_hex, &keys[KEYCNTR], NULL },
    { "keystrafeleft", type_hex, &keys[KEYSTFL], NULL },
    { "keystraferight", type_hex, &keys[KEYSTFR], NULL },
    { "keymap", type_hex, &keys[KEYMAP], NULL },
    { "keyzoomin", type_hex, &keys[KEYZOOMI], NULL },
    { "keyzoomout", type_hex, &keys[KEYZOOMO], NULL },
    { "keyusepotion", type_hex, &keys[KEYUSEP], NULL },
    { "keycast", type_hex, &keys[KEYCAST], NULL },
    { "keyfly", type_hex, &keys[KEYFLY], NULL },
    { "keylooking", type_hex, &keys[KEYLOOKING], NULL },
    { "keyuncast", type_hex, &keys[KEYUNCAST], NULL },
    { "keyflyup", type_hex, &keys[KEYFLYUP], NULL },
    { "keyflydown", type_hex, &keys[KEYFLYDN], NULL },
    { "keyconsole", type_hex, &keys[KEYCONSOLE], NULL },

    { "mousebutton1", type_int, &mousekeys[0],
        "; Mouse Button Actions\n"
        ";   4 - Run\n"
        ";   5 - Strafe\n"
        ";   6 - Fire\n"
        ";   7 - Use\n"
        ";   8 - Jump\n"
        ";   9 - Crouch\n"
        ";  20 - Use potion\n"
    },
    { "mousebutton2", type_int, &mousekeys[1], NULL },
    { "mousexspeed", type_int, &mousxspeed,
        "; Mouse speed (1-16)\n" },
    { "mouseyspeed", type_int, &mousyspeed, NULL },
    { "mouselookmode", type_bool, &mouselookmode,
        "; Mouse look mode\n"
        ";   0 - Momentary\n"
        ";   1 - Toggle\n"
    },
    { "mouselook", type_bool, &mouselook,
        "; Mouse look enabled (if Toggle mode)\n"
    },

    { "joystickbutton1", type_int, &joykeys[0],
        "; Joystick Button Actions (see Mouse Buttons Actions above)\n"
    },
    { "joystickbutton2", type_int, &joykeys[1], NULL },
    { "joystickbutton3", type_int, &joykeys[2], NULL },
    { "joystickbutton4", type_int, &joykeys[3], NULL },

    { "gore", type_bool, &goreon,
        "; Gore preference\n"
    },
    { "difficulty", type_int, &difficulty,
        "; Difficulty preference\n"
    },

    { NULL, 0, NULL, NULL }
};

int loadsetup(const char *fn)
{
    scriptfile *cfg;
    char *token;
    int item;

    cfg = scriptfile_fromfile(fn);
    if (!cfg) {
        return -1;
    }

    scriptfile_clearsymbols();

    while (1) {
        token = scriptfile_gettoken(cfg);
        if (!token) break;    //EOF

        for (item = 0; configspec[item].name; item++) {
            if (!Bstrcasecmp(token, configspec[item].name)) {
                // Seek past any = symbol.
                token = scriptfile_peektoken(cfg);
                if (!Bstrcasecmp("=", token)) {
                    scriptfile_gettoken(cfg);
                }

                switch (configspec[item].type) {
                    case type_bool: {
                        int value = 0;
                        if (scriptfile_getnumber(cfg, &value)) break;
                        *(int*)configspec[item].store = (value != 0);
                        break;
                    }
                    case type_int: {
                        int value = 0;
                        if (scriptfile_getnumber(cfg, &value)) break;
                        *(int*)configspec[item].store = value;
                        break;
                    }
                    case type_hex: {
                        int value = 0;
                        if (scriptfile_gethex(cfg, &value)) break;
                        *(int*)configspec[item].store = value;
                        break;
                    }
                    case type_double: {
                        double value = 0.0;
                        if (scriptfile_getdouble(cfg, &value)) break;
                        *(double*)configspec[item].store = value;
                        break;
                    }
                    default: {
                        buildputs("loadsetup: unhandled value type\n");
                        break;
                    }
                }
                break;
            }
        }
        if (!configspec[item].name) {
            buildprintf("loadsetup: error on line %d\n", scriptfile_getlinum(cfg, cfg->ltextptr));
            continue;
        }
    }

#if USE_POLYMOST
    if (tmprenderer >= 0) {
        setrendermode(tmprenderer);
    }
#endif
    setmaxrefreshfreq(tmpmaxrefreshfreq);
    if (tmpbrightness >= 0) {
        gbrightness = brightness = min(max(tmpbrightness,0),15);
    }
    fullscreen = 0;
    if (tmpfullscreen >= 0) {
        fullscreen = tmpfullscreen;
    }
    if (tmpdisplay >= 0) {
        fullscreen |= tmpdisplay<<8;
    }
    if (tmpsamplerate >= 0) {
        option[7] = (tmpsamplerate & 0x0f) << 4;
        option[7] |= 1<<1;  // 16-bit
        option[7] |= 1<<2;  // stereo
    }
    digilevel = min(16, max(0, digilevel));
    if (tmpmusic >= 0) {
        option[2] = !!tmpmusic;
    }
    musiclevel = min(16, max(0, musiclevel));
    if (tmpmouse == 0) {
        option[3] &= ~1;
    } else if (tmpmouse > 0) {
        option[3] |= 1;
    }
    mousxspeed = max(1,min(16,mousxspeed));
    mousyspeed = max(1,min(16,mousyspeed));
    if (tmpjoystick == 0) {
        option[3] &= ~2;
    } else if (tmpjoystick > 0) {
        option[3] |= 2;
    }
    OSD_CaptureKey(keys[KEYCONSOLE]);

    scriptfile_close(cfg);
    scriptfile_clearsymbols();

    return 0;
}

int writesetup(const char *fn)
{
    BFILE *fp;
    int item;

    fp = Bfopen(fn,"wt");
    if (!fp) return -1;

    tmpbrightness = gbrightness;
    tmpfullscreen = !!(fullscreen&255);
    tmpdisplay = fullscreen>>8;
#if USE_POLYMOST
    tmprenderer = getrendermode();
#endif
    tmpmaxrefreshfreq = getmaxrefreshfreq();
    tmpsamplerate = option[7]>>4;
    tmpmusic = option[2];
    tmpmouse = !!(option[3]&1);
    tmpjoystick = !!(option[3]&2);

    for (item = 0; configspec[item].name; item++) {
        if (configspec[item].doc) {
            if (item > 0) fputs("\n", fp);
            fputs(configspec[item].doc, fp);
        }
        fputs(configspec[item].name, fp);
        fputs(" = ", fp);
        switch (configspec[item].type) {
            case type_bool: {
                fprintf(fp, "%d\n", (*(int*)configspec[item].store != 0));
                break;
            }
            case type_int: {
                fprintf(fp, "%d\n", *(int*)configspec[item].store);
                break;
            }
            case type_hex: {
                fprintf(fp, "%X\n", *(int*)configspec[item].store);
                break;
            }
            case type_double: {
                fprintf(fp, "%g\n", *(double*)configspec[item].store);
                break;
            }
            default: {
                fputs("?\n", fp);
                break;
            }
        }
    }

    Bfclose(fp);

    return 0;
}
