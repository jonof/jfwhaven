/***************************************************************************
 *   ICORP.H   - include file for Whaven game                              *
 *                                                                         *
 *                                                     12/15/94 Les Bird   *
 ***************************************************************************/

#include "build.h"
#include "names.h"
#include "sndmod.h"
#include "les.h"
#include "jeff.h"
#include "raf.h"
#include "whdefs.h"
#include "extern.h"
#include "keydefs.h"


extern int  swingcnt;

struct swingdoor {
     int  wall[8];
     int  sector;
     int  angopen;
     int  angclosed;
     int  angopendir;
     int  ang;
     int  anginc;
     int x[8];
     int y[8];
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
     int x,y,z;
     int ang;
     int horiz;
     int zoom;
     int height;
     int hvel;
     short sector,oldsector;
     short screensize;
     short spritenum;
     char dimension;
     unsigned int flags;
     int  weapon[MAXWEAPONS];
     int  ammo[MAXWEAPONS];
     int  orbammo[MAXNUMORBS];
     int  treasure[MAXTREASURES];
     int  orbactive[MAXNUMORBS];
     int  orb[MAXNUMORBS];
     int  potion[MAXPOTIONS];
     int  lvl;
     int score;
     int  health;
     int  maxhealth;
     int  armor;
     int  armortype;
     char onsomething;
     int fallz;
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
     unsigned int sectorflags;
     int animate;
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
int angvel,
     svel,
     vel;

extern
int *animateptr[],
     animategoal[],
     animatevel[],
     animatecnt,
     neartagdist,
     neartaghitdist;

extern
unsigned int flags32[];

extern
short neartagsector,
     neartagwall,
     neartagsprite;

extern
int lockclock;

void lb_printf(int,int,char *,...);

// b5compat.c
void permanentwritesprite(int thex, int they, short tilenum, signed char shade,
        int cx1, int cy1, int cx2, int cy2, unsigned char dapalnum);
void permanentwritespritetile(int UNUSED(thex), int UNUSED(they), short tilenum, signed char shade,
        int cx1, int cy1, int cx2, int cy2, unsigned char dapalnum);
void overwritesprite(int thex, int they, short tilenum, signed char shade,
        char stat, unsigned char dapalnum);

// whaven.c
int setanimation(int *animptr,int thegoal,int thevel);
void setdelayfunc(void (*func)(),int item,int delay);
int getanimationgoal(int *animptr);

// whmenu.c
void fancyfont(int x,int y,short tilenum,char *string,char pal);
void help(void);
void loadsave(struct player *plr);

// whobj.c
int movesprite(short spritenum,int dx,int dy,int dz,int ceildist,int flordist,char cliptype);
void newstatus(short sn,int seq);
void trowajavlin(int s);

// whsndmod.c
int SND_PlaySound(unsigned int sound,int x,int y,unsigned int Pan,unsigned int loopcount);
