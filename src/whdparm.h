//**
//** Application specific defines for INSTALL.C
//**

#ifndef _CONTROLS_H
#include "whdcntrl.h"
#endif

#define   EXEFILENAME         "WHDEMO.EXE"
#define   MININSTALLBYTES     22000000

#define   SETUPNAME           "WITCHAVEN SETUP UTILITY V1.0"
#define   SETUPVERSION        "COPYRIGHT (C) 1996 INTRACORP, INC."

struct smk {
     int  Active;                  // smack is currently playing flag
     int  Loop;                    // loop this smack continuously
     int  State;                   // link this smack to a I_? state (below)
     int  Frame;                   // current frame being played
     int  Flag;                    // 1=load in memory, -1=never play
     char *File;                   // smack filename (in INSTALLDATAPATH)
     Smack *Smack;                 // pointer to this Smack's structure
};

//** Sample to keep in memory
enum {
     WAV_CLICK,
     WAV_MAXSAMPLES
};

char *sampleDataFile[WAV_MAXSAMPLES]={
     "CLICK.WAV"
};

//** Main install program states
enum {
     I_DETECTAUDIO,
     I_DETECTMIDI,
     I_GETDESTINATIONPATH,
     I_COPYFILES,
     I_GETCONTROLCONFIG,
     I_GETVIDEOMODE,
     I_MAXSTATES
} installState;

//** Register all playable smacks here
struct smk smk[]={
     { 1, 1, -1,                   0, 1, "penta.smk", NULL},
     { 0, 0, 0,                    0, 0, NULL, NULL} // terminator
};

//** Heres a list of files to copy from the CD to the destination path
char defPath[]="\\CAPSTONE\\WHDEMO\\",
     srcPath[]="\\WHAVEN\\DEMO\\RUN\\",
     *fileList[]={
          "CAPSTONE.SMK",
          "DOS4GW.EXE",
          "DRUM.BNK",
          "HMIDET.386",
          "HMIDRV.386",
          "HMIMDRV.386",
          "JOESND",
          "LEVEL1.MAP",
          "LEVEL2.MAP",
          "LEVEL3.MAP",
          "LOOKUP.DAT",
          "LOOPS",
          "MELODIC.BNK",
          "PALETTE.DAT",
          "SETUP.EXE",
          "SETUP.INI",
          "SMACKPLY.EXE",
          "SONGS",
          "TABLES.DAT",
          "TEST.HMP",
          "TEST.RAW",
          "TILES000.ART",
          "TILES001.ART",
          "TILES002.ART",
          "TILES003.ART",
          "TILES004.ART",
          "TILES005.ART",
          "TILES006.ART",
          "TILES007.ART",
          "TILES008.ART",
          "WHDEMO.EXE",
          NULL
     };

//** Randomly played songs (after MIDI selection is made)
#define   MAXSONGS            3

char *songs[MAXSONGS]={
     "SONG1.HMP",
     "SONG2.HMP",
     "SONG3.HMP"
};

