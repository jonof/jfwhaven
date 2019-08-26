/***************************************************************************
 *   ICORP.H   - include file for Whaven game                              *
 *                                                                         *
 *                                                     12/15/94 Les Bird   *
 ***************************************************************************/

#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <setjmp.h>
#include <stdarg.h>
#include <process.h>

#include "build.h"
#include "names.h"
#include "sndmod.h"
#include "les.h"
#include "jeff.h"
#include "raf.h"
#include "whdefs.h"
#include "extern.h"
#include "keydefs.h"

#ifndef _PROTOTYPING_
     #include "whaven.def"
     #include "whinp.def"
     #include "whtag.def"
     #include "whobj.def"
     #include "whplr.def"
     #include "whfx.def"
     #include "whsndmod.def"
     #include "whani.def"
     #include "whnet.def"
     #include "cdrom.def"
     #include "whctm.def"
#endif


extern int  swingcnt;

struct swingdoor {
     int  wall[8];
     int  sector;
     int  angopen;
     int  angclosed;
     int  angopendir;
     int  ang;
     int  anginc;
     long x[8];
     long y[8];
};

extern
struct swingdoor swingdoor[MAXSWINGDOORS];

//
// plr->flags defines
//

//
// plr->equipment flag values
//


struct player {
     long x,y,z;
     long ang;
     long horiz;
     long zoom;
     long height;
     long hvel;
     short sector,oldsector;
     short screensize;
     short spritenum;
     char dimension;
     unsigned long flags;
     int  weapon[MAXWEAPONS];
     int  ammo[MAXWEAPONS];
     int  orbammo[MAXNUMORBS];
     int  treasure[MAXTREASURES];
     int  orbactive[MAXNUMORBS];
     int  orb[MAXNUMORBS];
     int  potion[MAXPOTIONS];
     int  lvl;
     long score;
     int  health;
     int  maxhealth;
     int  armor;
     int  armortype;
     char onsomething;
     long fallz;
};

extern struct player player[MAXPLAYERS];

struct delayitem {
     void (*func)(int);
     int  item;
     int  timer;
};

extern
struct delayitem delayitem[MAXSECTORS];

struct sectoreffect {
     unsigned long sectorflags;
     long animate;
     int  hi,lo;
     int  delay,delayreset;
};

extern
int  secnt,
     sexref[];

extern
struct sectoreffect sectoreffect[MAXSECTORS];

extern
int  pyrn;

extern
long angvel,
     svel,
     vel;

extern
long *animateptr[],
     animategoal[],
     animatevel[],
     animatecnt,
     neartagdist,
     neartaghitdist;

extern
unsigned long flags32[];

extern
short neartagsector,
     neartagwall,
     neartagsprite;

extern
long lockclock;

void lb_printf(int,int,char *,...);

