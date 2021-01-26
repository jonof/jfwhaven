/***************************************************************************
 *   ICORP.H   - include file for Whaven game                              *
 *                                                                         *
 *                                                     12/15/94 Les Bird   *
 ***************************************************************************/

#include "build.h"
#include "pragmas.h"
#include "cache1d.h"
#include "baselayer.h"
#include "names.h"
#include "sndmod.h"
#include "les.h"
#include "jeff.h"
#include "raf.h"
#include "whdefs.h"
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

extern int xdimgame, ydimgame, bppgame;
extern int forcesetup;
extern int goreon, difficulty;
extern short brightness;


//#define NUMOPTIONS 8
#define NUMOPTIONS 12
#define NUMKEYS 28                                               // Les 07/24/95
#define XDIM    320
#define YDIM    200

#define KEYFIFOSIZ 64

extern point3d osprite[];
extern int   smoothratio;
extern int shieldpoints;
extern int poisoned;
extern int poisontime;
extern int  shockme;
extern int  svgahealth;
extern int  victor;
extern int  autohoriz;
extern int  svga;
extern char boardname[];
extern char tempboard[];
extern char loadgamename[];
extern int  delaycnt,
            engineinitflag,
            timerinitflag,
            videoinitflag;

extern
char option2[];

extern int synctics;
extern int globhiz, globloz, globhihit, globlohit;
extern char option[];
extern int keys[];

extern int svgascale, svgaxoff, svgastat, svgaoverstat;


// b5compat.c
void permanentwritesprite(int thex, int they, short tilenum, signed char shade,
        int cx1, int cy1, int cx2, int cy2, unsigned char dapalnum);
void overwritesprite(int thex, int they, short tilenum, signed char shade,
        char stat, unsigned char dapalnum);
void precache();

// whani.c
void animateobjs(struct player *plr);

// whaven.c
void shutdowngame(void );
void crashgame(char *fmt,...);
void doanimations(int numtics);
int getanimationgoal(int *animptr);
int setanimation(int *animptr,int thegoal,int thevel);
void setdelayfunc(void (*func)(int),int item,int delay);
void dodelayitems(int tics);
void setup3dscreen(void);
void setupboard(char *fname);
void drawscreen(struct player *plr);
void drawbackground(void);
void playloop(void);
void drawoverheadmap(struct player *plr);
void readpalettetable(void);
int adjusthp(int hp);

// whfx.c
void initlava(void);
void initwater(void);
void warp(int *x, int *y, int *z, short *daang, short *dasector);
void warpsprite(short spritenum);
void sectorsounds(void);
void dofx(void);
void thunder(void);
void thesplash(void);
void makeasplash(int picnum, struct player *plr);
void makemonstersplash(int picnum, int i);
void bats(short k);
void cracks(void);
void lavadryland(void);
void warpfxsprite(int s);

// whinp.c
void initjstick(void);
void keytimerstuff(void);
void processinput(struct player *plr);
void autothehoriz(struct player *plr);
void nettypeletter(void);
void typeletter(void);
void checkcheat(void);
void dosoundthing(void);

// whmenu.c
void fancyfont(int x,int y,short tilenum,char *string,char pal);
void fancyfontscreen(int x, int y, short tilenum, char *string,char pal);
void fancyfontperm(int x, int y, short tilenum, char *string, char pal);
void svgafullscreenpic(short pic1, short pic2);
int menuscreen(struct player *plr);
void help(void);
void loadsave(struct player *plr);
void quit(void);
void thedifficulty(void);
void startnewgame(struct player *plr);
void loadgame(struct player *plr);
void savegame(struct player *plr);
void savegametext(int select);
int savedgamename(int gn);
int savedgamedat(int gn);
void loadplayerstuff(void);
void screenfx(struct player *plr);
void initpaletteshifts(void);
void startgreenflash(int greentime);
void startblueflash(int bluetime);
void startredflash(int damage);
void startwhiteflash(int bonus);
void updatepaletteshifts(void);
void TEMPSND(void);
void cleanup(void);

// whnet.c
void netcheckargs(int argc, const char * const argv[]);
void netmarkflag(short i);
void netsendmove(void);
void netshootgun(short s, char guntype);
void netdamageactor(short s, short o);
void dropflagstart(short teamno);
void netdropflag(void);
void netgetmove(void);
void netpickmonster(void);
void initmulti(int numplayers);
void netshutdown(void);
void whnetmon(void);
void netrestartplayer(struct player * plr);

// whobj.c
void monitor(void);
void processobjs(struct player *plr);
int potionspace(int vial);
void updatepotion(int vial);
void transformactors(struct player *plr);
void firebreath(int i, int a, int b, int c);
void throwspank(int i);
void castspell(int i);
void skullycastspell(int i);
int checkmedusadist(int i, int x, int y, int z, int lvl);
int checkdist(int i,int x,int y,int z);
void checkspeed(int i,int *dax,int *day,int speed);
int checksight(int i,short *daang);
void checkmove(int i,int dax,int day,short *movestat);
void attack(int i);
void makeafire( int i, int firetype);
void explosion( int i, int x, int y, int z, short owner);
void explosion2( int i, int x, int y, int z, short owner);
void trailingsmoke(short i);
void icecubes( int i, int x, int y, int z, short owner);
int damageactor(int hitobject,int i);
void nukespell(short j);
void medusa(short j);
int movesprite(short spritenum,int dx,int dy,int dz,int ceildist,int flordist,char cliptype);
void newstatus(short sn,int seq);
void trowajavlin(int s);
void throwhalberd(int s);
void monsterweapon(short i);
void madenoise (int val, int x, int y, int z);
void monsternoise(short i);
void randompotion(short i);
void spawnabaddy(short i,short monster);
void spawnapentagram(short sn);

// whplr.c
void playerdead(struct player *plr);
void spikeheart(struct player *plr);
void initplayersprite(void);
void autoweaponchange(int dagun);
void weaponchange(void);
void potiontext(void);
void plrfireweapon(struct player *plr);
void activatedaorb(struct player *plr);
void plruse(struct player *plr);
void loadnewlevel(int mapon);
void victory(void);
void drawweapons(struct player *plr);
void castaorb( struct player *plr );
void shootgun ( struct player *plr, short daang, char guntype );
void singleshot(short bstatus);
void potionpic(int currentpotion);
void usapotion(struct player *plr);
void orbpic(int currentorb);
void healthpic(int hp);
void armorpic(int arm);
void levelpic(void);
void score(int score);
void goesupalevel(struct player *plr);
int checkweapondist(short i,int x,int y,int z,char guntype);
void updatepics(void);
void captureflagpic(void);
void fragspic(void);
void keyspic(void);
int lvlspellcheck(struct player *plr);
void displayspelltext(void);
void painsound(int xplc,int yplc);

// whsndmod.c
void SND_DoBuffers(void);
void SND_UnDoBuffers(void);
void SND_Startup(void);
void SND_Shutdown(void);
void SND_Mixer( unsigned short wSource, unsigned short wVolume );
void SND_MenuMusic(int choose);
void SND_StartMusic(unsigned short level);
void SND_SongFlush(void);
void SND_FadeMusic(void );
void SND_Sting(unsigned short sound);
int SND_PlaySound(unsigned short sound,int x,int y,unsigned short Pan,unsigned short loopcount);
int SND_Sound(unsigned short sound );
void SND_CheckLoops(void);
void SND_StopLoop(int which);
void SND_DIGIFlush(void);
void dolevelmusic(int level);
void playsound_loc(int soundnum,int xplc,int yplc);

// whtag.c
void operatesprite(int s);
void operatesector(int s);
void animatetags(struct player *plr);

// config.c
int loadsetup(const char *fn);
int writesetup(const char *fn);
