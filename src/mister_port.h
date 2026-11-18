#ifndef MISTER_PORT_H
#define MISTER_PORT_H

// MiSTer hybrid core (the jfSW core): the game runs on the ARM, the FPGA
// core shows the picture at 15kHz and provides audio and input. SDL's
// "mister" drivers (hybrid/sdl2) and the engine's sdlayer2.c (video, the
// clock, the joystick) do most of the work; this is what is specific to the
// game. Everything is a no-op without the core.

// OSD options (CONF_STR of core/jfSW.sv): Resolution, 320 or 640 wide, 200 or
// 240 lines
int MiSTer_ScreenWidth(void);
int MiSTer_ScreenHeight(void);
// In app_main(), before the engine is started
void MiSTer_Init(void);
// The core opened before the game's files are looked for
void MiSTer_Loading(void);
// The game's data check and logo (MH_PickGame()): quits after a message on
// the core's screen when the files are not there
void MiSTer_PickGame(void);
// Problems the player can fix: on the core's purple screen until the player
// leaves the core
void MiSTer_ShowError(const char *message);
// Exit code of the process after the player quit
int MiSTer_ExitCode(void);

#endif
