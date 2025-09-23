/***************************************************************************
 *    WITCHAVEN.C  - main game code for Witchaven game                     *
 *                                                                         *
 ***************************************************************************/

#define WHAVEN
#define GAME
#define SVGA
#include "icorp.h"
#include "osd.h"
#include "startwin.h"
#include "version.h"

int *animateptr[MAXANIMATES],
      animategoal[MAXANIMATES],
      animatevel[MAXANIMATES],
      animatecnt=0;

extern short cddrive;

int followx, followy;

int ratcnt=0;

int gameactivated=0;
int escapetomenu=0;

int selectedgun;
int  difficulty=2;

int xdimgame = 640, ydimgame = 480, bppgame = 8;
int forcesetup = 1;
int numlevels;

const int defaultkeys[NUMKEYS]={
     0xC8,0xD0,0xCB,0xCD,0x2A,0x38,0x1D,0x39,
     0x2D,0x2E,0xC9,0xD1,0xC7,0x33,0x34,
     0x0F,0x1C,0x0D,0x0C,0x9C,0x1C,0x29,
     0x23,0x1E,0xCF,0x1F,0x35,0x1A,0x1B,
     0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB, // KEYWEAP1..9
     0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x41,0x42, // KEYSPELL1..8
     0x45
};
const int modernkeys[NUMKEYS]={
     0x11,0x1F,0xCB,0xCD,0x2A,0xB8,0x1D,0x12,
     0x39,0x38,0xC9,0xD1,0xC7,0x1E,0x20,
     0x0F,0x1C,0x0D,0x0C,0x9C,0x1C,0x29,
     0x23,0x16,0xCF,0x1F,0x35,0x1A,0x1B,
     0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB, // KEYWEAP1..9
     0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x41,0x42, // KEYSPELL1..8
     0x45
};
int keys[NUMKEYS];

extern int loadedgame;
extern char tempbuf[50];
extern int  spiked;

extern short brightness;

int synctics;
int globhiz, globloz, globhihit, globlohit;
char option[NUMOPTIONS] = {
  0,  // 0 - Video type
  0,  // 1 -
  0,  // 2 -
  1,  // 3 - 1 = Mouse, +2 = Joystick
  0,  // 4 -
  0,  // 5 -
  1,  // 6 - Video mode
  64+4+2, // 7 - Sound, 22khz 16bit stereo
  0,  // 8 -
  1,  // 9 - Music on
  0,  // 10 -
  1,  // 11 - Sound on
};
int digihz[8] = {6000,8000,11025,16000,22050,32000,44100,48000};

struct delayitem delayitem[MAXSECTORS];

int   actorcnt,
      secnt,
      sexref[MAXSECTORS],
      swingcnt;

short spikecnt,
      spikesector[64];

short goblinspritelist[100];
short goblinwarcnt;

short lavadrylandsector[32];
short lavadrylandcnt;

short xpanningsectorlist[64], xpanningsectorcnt;
short ypanningwalllist[64], ypanningwallcnt;
short floorpanninglist[64], floorpanningcnt;
short skypanlist[64], skypancnt;

short crushsectorlist[32], crushsectorcnt;
short crushsectoranim[32], crushsectordone[32];

short revolvesector[16], revolveang[16], revolveclip[16], revolvecnt;
int revolvex[16][16], revolvey[16][16];
int revolvepivotx[16], revolvepivoty[16];

short dragsectorlist[16], dragxdir[16], dragydir[16], dragsectorcnt;
int dragx1[16], dragy1[16], dragx2[16], dragy2[16], dragfloorz[16];

short warpsectorlist[16], warpsectorcnt;

short bobbingsectorlist[16], bobbingsectorcnt;

int lockclock, ototalclock, gotlastpacketclock, ready2send;

//JSA ends

int justteleported=0;
int playerdie=0;

int wet=0;
int oldvmode;
int dasizeofit;

short ironbarsector[16],ironbarscnt;
int  ironbarsgoal1[16],ironbarsgoal2[16];
short ironbarsdone[16], ironbarsanim[16];
int  ironbarsgoal[16];

extern int mapon;
int mapflag;

point3d osprite[MAXSPRITESONSCREEN];
int   smoothratio;
int shieldpoints=0;
int poisoned=0;
int poisontime=0;
int  shockme=-1;
int  svgahealth=0;
int  victor=0;
int  autohoriz=0;
int  svga=0;
char boardname[80];
char tempboard[80];
char loadgamename[80];
int   delaycnt,
      engineinitflag,
      timerinitflag,
      videoinitflag;

int svgascale, svgaxoff, svgastat, svgaoverstat;
int nummoves;

#define AVERAGEFRAMES 32
static unsigned int showfps, averagefps, framecnt, frameval[AVERAGEFRAMES];

static int osdcmd_showfps(const osdfuncparm_t *parm);
static int osdcmd_vidmode(const osdfuncparm_t *parm);
static int osdcmd_setkeys(const osdfuncparm_t *parm);
static int osdcmd_setting(const osdfuncparm_t *parm);

//
//
//

void getpackets(void) {
}

void movethings(void) {
    short i;

    gotlastpacketclock = totalclock;
    for(i=connecthead;i>=0;i=connectpoint2[i])
    {
        copybufbyte(&ffsync[i],&baksync[movefifoend[i]][i],sizeof(input));
        movefifoend[i] = ((movefifoend[i]+1)&(MOVEFIFOSIZ-1));
    }
}

void domovethings(void) {
    short i;

    nummoves++;

    for(i=connecthead;i>=0;i=connectpoint2[i])
        copybufbyte(&baksync[movefifoplc][i],&ssync[i],sizeof(input));
    movefifoplc = ((movefifoplc+1)&(MOVEFIFOSIZ-1));

    lockclock += TICSPERFRAME;

    for(i=connecthead;i>=0;i=connectpoint2[i]) {
        player[i].ox = player[i].x;
        player[i].oy = player[i].y;
        player[i].oz = player[i].z;
        player[i].ohoriz = player[i].horiz;
        player[i].oang = player[i].ang;
        syncprocessinput(i);
    }

    processobjs(&player[pyrn]);
    animateobjs(&player[pyrn]);
    animatetags(&player[pyrn]);
    doplrtimes();
    doobjtimes();
    doanimations();
    dodelayitems();
    dofx();
}

void faketimerhandler(void) {
    sampletimer();
    if ((totalclock < ototalclock+TICSPERFRAME) || (ready2send == 0)) return;
    ototalclock += TICSPERFRAME;

    getpackets();
    if (getoutputcirclesize() >= 16) return;
    getinput();

    if (networkmode == 1) {
        // TODO broadcast network mode
        return;
    }

        //MASTER (or 1 player game)
    if ((myconnectindex == connecthead) || !netgame) {
        copybufbyte(&loc,&ffsync[myconnectindex],sizeof(input));

        if (netgame) {
            // TODO
        }
        else if (numplayers >= 2) {
            // TODO
        }
        movethings();
    }
    else {                      //I am a SLAVE
        // TODO
    }
}

//
//   basic text functions
//

void rp_delay(int goal) {

        int dagoal=0L;
        int exit=0;

        dagoal=totalclock+goal;

        while ( !exit ) {
            handleevents();
            if( totalclock >= dagoal )
                exit=1;
        }

}



//
//   game code
//

int  crashflag;

void
shutdowngame(void)
{

      writesetup("whaven.ini");

      SND_Shutdown();

      netshutdown();

      if (engineinitflag) {
             uninitengine();
      }

      uninitgroupfile();

      if (crashflag) {
          return;
      }

      exit(0);

}

void crashgame(char *fmt,...) {
      va_list argptr;

      crashflag=1;
      shutdowngame();

      va_start(argptr,fmt);
      vprintf(fmt,argptr);
      va_end(argptr);
      printf("\n");
      exit(0);

}

void doanimations(void) {

      int i,animval;

      for (i=animatecnt-1 ; i >= 0 ; i--) {
             animval=*animateptr[i];
             if (animval < animategoal[i]) {
                    animval+=(TICSPERFRAME*animatevel[i]);
                    if (animval > animategoal[i]) {
                          animval=animategoal[i];
                    }
             }
             if (animval > animategoal[i]) {
                    animval-=(TICSPERFRAME*animatevel[i]);
                    if (animval < animategoal[i]) {
                        animval=animategoal[i];
                    }
             }
             *animateptr[i]=animval;
             if (animval == animategoal[i]) {
                animatecnt--;
                animateptr[i]=animateptr[animatecnt];
                animategoal[i]=animategoal[animatecnt];
                animatevel[i]=animatevel[animatecnt];
             }
      }
}

int getanimationgoal(int *animptr)
{
    int i, j;

    j = -1;
    for(i=0;i<animatecnt;i++)
        if (animptr == animateptr[i])
        {
            j = i;
            break;
        }
    return(j);
}

int setanimation(int *animptr,int thegoal,int thevel) {

      int i,j;

      if (animatecnt >= MAXANIMATES-1) {
             return(-1);
      }
      j=animatecnt;
      for(i=0 ; i < animatecnt ; i++) {
             if (animptr == animateptr[i]) {
                    j=i;
                    break;
             }
      }
      animateptr[j]=animptr;
      animategoal[j]=thegoal;
      animatevel[j]=thevel;
      if (j == animatecnt) {
             animatecnt++;
      }
      return(animatecnt-1);
}


void setdelayfunc(void (*func)(int),int item,int delay) {

      int  i;

      for (i=0 ; i < delaycnt ; i++) {
             if (delayitem[i].func == func && delayitem[i].item == item) {
                    if (delay == 0) {
                          delayitem[i].func=NULL;
                    }
                    delayitem[i].timer=delay;
                    return;
             }
      }
      if (delay > 0) {
             delayitem[delaycnt].func=func;
             delayitem[delaycnt].item=item;
             delayitem[delaycnt].timer=delay;
             delaycnt++;
      }
}

void dodelayitems(void) {

      int  cnt,i,j;

      cnt=delaycnt;
      for (i=0 ; i < cnt ; i++) {
             if (delayitem[i].func == NULL) {
                    j=delaycnt-1;
                    memmove(&delayitem[i],&delayitem[j],sizeof(struct delayitem));
                    delaycnt=j;
             }
             if (delayitem[i].timer > 0) {
                    if ((delayitem[i].timer-=TICSPERFRAME) <= 0) {
                          delayitem[i].timer=0;
                          (*delayitem[i].func)(delayitem[i].item);
                          delayitem[i].func=NULL;
                    }
             }
      }
}


void setup3dscreen(void) {

    if (setgamemode(fullscreen, xdimgame, ydimgame, bppgame))
        crashgame("Could not set video mode");
    svga=!(xdim == 320 && ydim == 200) && !(xdim == 640 && ydim == 400);

    if(svga) {
        svgascale = scale(65536L, ydim, 480);
        svgaxoff = (xdim<<15) - scale(640, ydim<<15, 480);
        svgastat = 8+16+64;
        svgaoverstat = 8+16;
    }
    else {
        svgascale = 65536;
        svgaxoff = 0;
        svgastat = 2+8+16+64;
        svgaoverstat = 2+8+16;
    }

    videoinitflag=1;
}


void setupboard(char *fname) {

    int  endwall,i,j,k,startwall;
    struct player *plr;
    int dax, day;
    short treesize;
    int dasector;
    int dax2, day2;
    short daang;

    plr=&player[0];

    randomseed = 17L;

    i = loadboard(fname,0,&plr->x,&plr->y,&plr->z,&daang,&plr->sector);
    if (i == -2) i = loadoldboard(fname,0,&plr->x,&plr->y,&plr->z,&daang,&plr->sector);
    if (i < 0) {
        crashgame("Board not found");
    }

    plr->ang=daang;
    plr->ox = plr->x;
    plr->oy = plr->y;
    plr->oz = plr->z;
    plr->oang = plr->ang;

    precache();

    ratcnt=0;
    actorcnt=secnt=swingcnt=0;
    xpanningsectorcnt=0;
    ypanningwallcnt=0;
    floorpanningcnt=0;
    crushsectorcnt=0;
    revolvecnt=0;
    warpsectorcnt=0;
    dragsectorcnt=0;
    ironbarscnt=0;
    spikecnt=0;
    bobbingsectorcnt=0;
    goblinwarcnt=0;
    lavadrylandcnt=0;
    for (i=0 ; i < MAXSPRITES ; i++) {      // setup sector effect options
        if(sprite[i].picnum == FRED && sprite[i].pal == 1)
            deletesprite((short)i);

            if(sprite[i].picnum == RAT ) {
                ratcnt++;
                if(ratcnt > 10)
                    deletesprite((short)i);
            }

            if(sprite[i].picnum == SPAWN ) {
                deletesprite((short)i);
            }

            if(sprite[i].picnum == TORCH) {
                sprite[i].cstat&=~3;
                changespritestat(i,TORCHLIGHT);
            }

            if(sprite[i].picnum == STANDINTORCH ||
               sprite[i].picnum == BOWLOFFIRE) {
               changespritestat(i,TORCHLIGHT);
            }


            if (sprite[i].picnum == GLOW ) {
               changespritestat(i,GLOWLIGHT);
            }

//JSA BLORB
        if(sprite[i].picnum == SNDEFFECT ) {
            sector[sprite[i].sectnum].extra=sprite[i].lotag;
            deletesprite((short)i);
        }

        if(sprite[i].picnum == SNDLOOP ) {      //loop on
            sector[sprite[i].sectnum].extra=(32768|(sprite[i].lotag<<1)|1);
            deletesprite((short)i);
        }

        if(sprite[i].picnum == SNDLOOPOFF ) {   //loop off
            sector[sprite[i].sectnum].extra=(32768|(sprite[i].lotag<<1));
            deletesprite((short)i);
        }



            if(sprite[i].lotag == 80) {
                ironbarsector[ironbarscnt]=sprite[i].sectnum;
                ironbarsdone[ironbarscnt]=0;
                ironbarsanim[ironbarscnt]=i;
                ironbarsgoal[ironbarscnt]=0;
                ironbarscnt++;
            }
            if(sprite[i].statnum < MAXSTATUS) {
                switch (sprite[i].picnum) {
                case GRONHAL:
                    sprite[i].xrepeat=30;
                    sprite[i].yrepeat=30;
                    sprite[i].clipdist=64;
                    changespritestat(i,FACE);
                    sprite[i].hitag=adjusthp(300);
                    sprite[i].lotag=100;
                    sprite[i].cstat|=0x101;
                    sprite[i].extra=4;
                break;
                case GRONMU:
                    sprite[i].xrepeat=30;
                    sprite[i].yrepeat=30;
                    sprite[i].clipdist=64;
                    changespritestat(i,FACE);
                    sprite[i].hitag=adjusthp(300);
                    sprite[i].lotag=100;
                    sprite[i].cstat|=0x101;
                    sprite[i].extra=2;
                break;
                case GRONSW:
                    sprite[i].xrepeat=30;
                    sprite[i].yrepeat=30;
                    sprite[i].clipdist=64;
                    changespritestat(i,FACE);
                    sprite[i].hitag=adjusthp(300);
                    sprite[i].lotag=100;
                    sprite[i].cstat|=0x101;
                    sprite[i].extra=0;
                break;
                case RAT:
                    sprite[i].xrepeat=32;
                    sprite[i].yrepeat=32;
                    sprite[i].shade=12;
                    sprite[i].pal=5;
                    changespritestat(i,FACE);
                    sprite[i].hitag=10;
                    sprite[i].lotag=100;
                    sprite[i].cstat=0x101;
                break;
                case FISH:
                    sprite[i].clipdist=32;
                    changespritestat(i,FACE);
                    sprite[i].hitag=10;
                    sprite[i].lotag=100;
                    sprite[i].cstat|=0x101;
                break;
                case WILLOW:
                    sprite[i].xrepeat=32;
                    sprite[i].yrepeat=32;
                    sprite[i].clipdist=64;
                    changespritestat(i,FACE);
                    sprite[i].hitag=adjusthp(400);
                    sprite[i].lotag=100;
                    sprite[i].cstat|=0x101;
                break;
                case DRAGON:
                    sprite[i].xrepeat=54;
                    sprite[i].yrepeat=54;
                    sprite[i].clipdist=128;
                    changespritestat(i,FACE);
                    sprite[i].hitag=adjusthp(900);
                    sprite[i].lotag=100;
                    sprite[i].cstat|=0x101;
                break;
                case DEVIL:
                case DEVILSTAND:
                    sprite[i].xrepeat=36;
                    sprite[i].yrepeat=36;
                    sprite[i].clipdist=64;
                    changespritestat(i,FACE);
                    if( sprite[i].pal == 2 )
                        sprite[i].hitag=adjusthp(60);
                    else
                        sprite[i].hitag=adjusthp(50);
                        sprite[i].lotag=100;
                        sprite[i].cstat|=0x101;
                break;
                case HANGMAN+1:
                    sprite[i].xrepeat=28;
                    sprite[i].yrepeat=28;
                break;
                case HANGMAN:
                    sprite[i].xrepeat=28;
                    sprite[i].yrepeat=28;
                    sprite[i].clipdist=64;
                    changespritestat(i,STAND);
                    sprite[i].hitag=adjusthp(30);
                    sprite[i].lotag=100;
                    sprite[i].cstat|=0x101;
                break;
                case SKELETON:
                    sprite[i].xrepeat=24;
                    sprite[i].yrepeat=24;
                    sprite[i].clipdist=64;
                    changespritestat(i,FACE);
                    sprite[i].hitag=adjusthp(30);
                    sprite[i].lotag=100;
                    sprite[i].cstat|=0x101;
                break;
                case GOBLINDEAD:
                    sprite[i].xrepeat=36;
                    sprite[i].yrepeat=36;
                break;
                case GOBLIN:
                case GOBLINSTAND:
                    if( sprite[i].hitag >= 90 && sprite[i].hitag <= 99 ) {
                        if( sprite[i].pal == 0 ) {
                            sprite[i].xrepeat=36;
                            sprite[i].yrepeat=36;
                        }
                        else {
                            sprite[i].xrepeat=30;
                            sprite[i].yrepeat=36;
                        }
                        sprite[i].extra=0;
                        sprite[i].owner=0;
                        sprite[i].lotag=100;
                        sprite[i].cstat|=0x101;
                        sprite[i].clipdist=64;
                        goblinspritelist[goblinwarcnt]=i;
                        goblinwarcnt++;
                    }
                    else {
                        sprite[i].xrepeat=36;
                        sprite[i].yrepeat=36;
                        changespritestat(i,FACE);
                        if(sprite[i].pal == 5)
                            sprite[i].hitag=adjusthp(35);
                        else if(sprite[i].pal == 4)
                            sprite[i].hitag=adjusthp(25);
                        else
                            sprite[i].hitag=adjusthp(15);
                        sprite[i].lotag=100;
                        sprite[i].cstat|=0x101;
                        if( rand()%100 > 50 )
                            sprite[i].extra=1;
                        sprite[i].clipdist=64;
                    }
                    break;
                    case GOBLINCHILL:
                        sprite[i].xrepeat=36;
                        sprite[i].yrepeat=36;
                        changespritestat(i,STAND);
                        sprite[i].hitag=adjusthp(15);
                        sprite[i].lotag=30;
                        sprite[i].cstat|=0x101;
                        if( rand()%100 > 50 )
                            sprite[i].extra=1;
                        sprite[i].clipdist=64;
                    break;
                    case FATWITCH:
                        sprite[i].clipdist=64;
                        sprite[i].xrepeat=32;
                        sprite[i].yrepeat=32;
                        changespritestat(i,FACE);
                        if(sprite[i].pal == 7)
                            sprite[i].hitag=adjusthp(290);
                        else
                            sprite[i].hitag=adjusthp(280);
                        sprite[i].lotag=100;
                        sprite[i].cstat|=0x101;
                        if( rand()%100 > 50 )
                            sprite[i].extra=1;
                    break;
                    case SKULLY:
                        sprite[i].clipdist=64;
                        sprite[i].xrepeat=32;
                        sprite[i].yrepeat=32;
                        changespritestat(i,FACE);
                        sprite[i].hitag=adjusthp(300);
                        sprite[i].lotag=100;
                        sprite[i].cstat|=0x101;
                        if( rand()%100 > 50 )
                            sprite[i].extra=1;
                    break;
                    case MINOTAUR:
                        sprite[i].clipdist=64;
                        changespritestat(i,FACE);
                        sprite[i].hitag=adjusthp(100);
                        sprite[i].lotag=100;
                        sprite[i].cstat|=0x101;
                        if( rand()%100 > 50 )
                            sprite[i].extra=1;
                    break;
                    case FRED:
                    case FREDSTAND:
                        if(sprite[i].pal == 1)
                    break;
                        sprite[i].xrepeat=48;
                        sprite[i].yrepeat=48;
                        changespritestat(i,FACE);
                        sprite[i].hitag=adjusthp(40);
                        sprite[i].lotag=100;
                        sprite[i].cstat|=0x101;
                        sprite[i].clipdist=64;
                        if( rand()%100 > 50 )
                            sprite[i].extra=1;
                    break;
                    case KOBOLD:
                         // gave kobold x2 hp
                        sprite[i].clipdist=64;
                        changespritestat(i,FACE);
                        if(sprite[i].pal == 8)
                            sprite[i].hitag=adjusthp(40);
                        else if(sprite[i].pal == 7)
                            sprite[i].hitag=adjusthp(60);
                        else
                            sprite[i].hitag=adjusthp(20);
                        sprite[i].lotag=100;
                        sprite[i].xrepeat=sprite[i].yrepeat=54;
                        sprite[i].cstat|=0x101;
                    break;
                    case SPIDER:
                        sprite[i].xrepeat=24;
                        sprite[i].yrepeat=18;
                        sprite[i].clipdist=64;
                        changespritestat(i,FACE);
                        sprite[i].hitag=adjusthp(5);
                        sprite[i].lotag=100;
                        sprite[i].cstat|=0x101;
                    break;
                    case GUARDIAN:
                        sprite[i].clipdist=64;
                        changespritestat(i,FACE);
                        sprite[i].hitag=adjusthp(200);
                        sprite[i].lotag=100;
                        sprite[i].xrepeat=sprite[i].yrepeat=32;
                        sprite[i].cstat|=0x101;
                    break;
                    case JUDYSIT:
                        changespritestat(i,WITCHSIT);
                        if( mapon > 24 )
                            sprite[i].hitag=adjusthp(700);
                        else
                            sprite[i].hitag=adjusthp(500);
                        sprite[i].lotag=100;
                        sprite[i].xrepeat=sprite[i].yrepeat=32;
                        sprite[i].cstat|=0x101;
                        sprite[i].extra=1200;
                    break;
                    case JUDY:
                        changespritestat(i,FACE);
                        if( mapon > 24 )
                            sprite[i].hitag=adjusthp(700);
                        else
                            sprite[i].hitag=adjusthp(500);
                        sprite[i].lotag=100;
                        sprite[i].xrepeat=sprite[i].yrepeat=32;
                        sprite[i].cstat|=0x101;
                    break;
                    case GOBWEAPON:
                    case WEAPON1:
                        sprite[i].xrepeat=34;
                        sprite[i].yrepeat=21;
                    break;
                    case WEAPON2:
                        sprite[i].xrepeat=26;
                        sprite[i].yrepeat=26;
                    break;
                    case WEAPON3:
                        sprite[i].xrepeat=44;
                        sprite[i].yrepeat=39;
                    break;
                    case WEAPON4:
                        sprite[i].xrepeat=25;
                        sprite[i].yrepeat=20;
                    break;
                    case WEAPON6:
                        sprite[i].xrepeat=20;
                        sprite[i].yrepeat=15;
                        sprite[i].cstat&=~3;
                    break;
                    case WEAPON7:
                        sprite[i].xrepeat=41;
                        sprite[i].yrepeat=36;
                        sprite[i].cstat&=~3;
                    break;
                    case QUIVER:
                        sprite[i].xrepeat=27;
                        sprite[i].yrepeat=27;
                    break;
                    case LEATHERARMOR:
                        sprite[i].xrepeat=47;
                        sprite[i].yrepeat=50;
                    break;
                    case CHAINMAIL:
                    case PLATEARMOR:
                        sprite[i].xrepeat=sprite[i].yrepeat=26;
                    break;
                    case SHIELD:
                        sprite[i].xrepeat=sprite[i].yrepeat=26;
                    break;
                    case HELMET:
                        sprite[i].xrepeat=27;
                        sprite[i].yrepeat=28;
                    break;
                    case SCROLLSCARE:
                    case SCROLLNUKE:
                    case SCROLLFLY:
                    case SCROLLFIREBALL:
                    case SCROLLFREEZE:
                    case SCROLLNIGHT:
                    case SCROLLMAGIC:
                        sprite[i].xrepeat=35;
                        sprite[i].yrepeat=36;
                        sprite[i].cstat&=~3;
                    break;
                    case DIAMONDRING:
                        sprite[i].xrepeat=14;
                        sprite[i].yrepeat=14;
                    break;
                    case SHADOWAMULET:
                        sprite[i].xrepeat=30;
                        sprite[i].yrepeat=23;
                    break;
                    case GLASSSKULL:
                        sprite[i].xrepeat=22;
                        sprite[i].yrepeat=22;
                    break;
                    case AHNK:
                        sprite[i].xrepeat=51;
                        sprite[i].yrepeat=54;
                    break;
                    case BLUESCEPTER:
                        sprite[i].xrepeat=32;
                        sprite[i].yrepeat=32;
                    break;
                    case YELLOWSCEPTER:
                        sprite[i].xrepeat=32;
                        sprite[i].yrepeat=32;
                    break;
                    case ADAMANTINERING:
                        sprite[i].xrepeat=14;
                        sprite[i].yrepeat=14;
                    break;
                    case ONYXRING:
                        sprite[i].xrepeat=42;
                        sprite[i].yrepeat=28;
                    break;
                    case HORNEDSKULL:
                        sprite[i].xrepeat=64;
                        sprite[i].yrepeat=64;
                    break;
                    case CRYSTALSTAFF:
                        sprite[i].xrepeat=64;
                        sprite[i].yrepeat=64;
                    break;
                    case AMULETOFTHEMIST:
                        sprite[i].xrepeat=26;
                        sprite[i].yrepeat=28;
                    break;
                    case SAPHIRERING:
                        sprite[i].xrepeat=30;
                        sprite[i].yrepeat=20;
                    break;
                    case PINE:
                        sprite[i].xrepeat=treesize=((krand()%5)+3)<<4;
                        sprite[i].yrepeat=treesize;
                    break;
                    case GIFTBOX:
                        sprite[i].xrepeat=56;
                        sprite[i].yrepeat=49;
                    break;
                    case GYSER:
                        sprite[i].xrepeat=32;
                        sprite[i].yrepeat=18;
                        sprite[i].shade=-17;
                        sprite[i].pal=0;
                        //changespritestat(i,DORMANT);
                    break;
                    case BARREL:
                    case VASEA:
                    case VASEB:
                    case VASEC:
                        sprite[i].cstat|=0x101;
                        sprite[i].clipdist=64;
                    break;
                    case BRASSKEY:
                        sprite[i].xrepeat=sprite[i].yrepeat=24;
                    break;
                    case BLACKKEY:
                        sprite[i].xrepeat=sprite[i].yrepeat=24;
                    break;
                    case GLASSKEY:
                        sprite[i].xrepeat=sprite[i].yrepeat=24;
                    break;
                    case IVORYKEY:
                        sprite[i].xrepeat=sprite[i].yrepeat=24;
                    break;
                    case THEHORN:
                        sprite[i].xrepeat=sprite[i].yrepeat=32;
                    break;
                    case 2233:     // team flags
                         netmarkflag(i);
                         break;
                    }
            }
    }

    for (i=0 ; i < numsectors ; i++) {

        if(sector[i].lotag == 100) {
            spikesector[spikecnt++]=i;
        }

        if(sector[i].lotag == 70)
            skypanlist[skypancnt++]=i;

        if(sector[i].lotag >= 80 && sector[i].lotag <= 89)
            floorpanninglist[floorpanningcnt++]=i;

        if(sector[i].lotag >= 900 && sector[i].lotag <=999) {
            lavadrylandsector[lavadrylandcnt]=i;
            lavadrylandcnt++;
        }

        if(sector[i].lotag >= 2100 && sector[i].lotag <= 2199) {
            startwall=sector[i].wallptr;
            endwall=startwall+sector[i].wallnum-1;
            dax=0L;
            day=0L;
            for(j=startwall;j<=endwall;j++) {
                dax+=wall[j].x;
                day+=wall[j].y;
            }
            revolvepivotx[revolvecnt]=dax/(endwall-startwall+1);
            revolvepivoty[revolvecnt]=day/(endwall-startwall+1);

            k=0;
            for(j=startwall;j<=endwall;j++) {
                revolvex[revolvecnt][k]=wall[j].x;
                revolvey[revolvecnt][k]=wall[j].y;
                k++;
            }
            revolvesector[revolvecnt]=i;
            revolveang[revolvecnt]=0;

            revolveclip[revolvecnt]=1;
            if (sector[i].ceilingz == sector[wall[startwall].nextsector].ceilingz)
                revolveclip[revolvecnt]=0;

            revolvecnt++;
        }

            switch (sector[i].lotag) {
            case DOORSWINGTAG:
                startwall=sector[i].wallptr;
                endwall=startwall+sector[i].wallnum-1;
                for(j=startwall ; j <= endwall ; j++) {
                    if(wall[j].lotag == 4) {
                        k=wall[wall[wall[wall[j].point2].point2].point2].point2;
                        if((wall[j].x == wall[k].x) && (wall[j].y == wall[k].y)) {
                            swingdoor[swingcnt].wall[0]=j;
                            swingdoor[swingcnt].wall[1]=wall[j].point2;
                            swingdoor[swingcnt].wall[2]=wall[wall[j].point2].point2;
                            swingdoor[swingcnt].wall[3]=wall[wall[wall[j].point2].point2].point2;
                            swingdoor[swingcnt].angopen=1536;
                            swingdoor[swingcnt].angclosed=0;
                            swingdoor[swingcnt].angopendir=-1;
                        }
                        else {
                            swingdoor[swingcnt].wall[0]=wall[j].point2;
                            swingdoor[swingcnt].wall[1]=j;
                            swingdoor[swingcnt].wall[2]=lastwall(j);
                            swingdoor[swingcnt].wall[3]=lastwall(swingdoor[swingcnt].wall[2]);
                            swingdoor[swingcnt].angopen=512;
                            swingdoor[swingcnt].angclosed=0;
                            swingdoor[swingcnt].angopendir=1;
                        }
                        for(k=0 ; k < 4 ; k++) {
                            swingdoor[swingcnt].x[k]=wall[swingdoor[swingcnt].wall[k]].x;
                            swingdoor[swingcnt].y[k]=wall[swingdoor[swingcnt].wall[k]].y;
                        }
                        swingdoor[swingcnt].sector=i;
                        swingdoor[swingcnt].ang=swingdoor[swingcnt].angclosed;
                        swingdoor[swingcnt].anginc=0;
                        swingcnt++;
                    }
                }
            break;
            case 11:
                xpanningsectorlist[xpanningsectorcnt++]=i;
            break;
            case 12:
                dasector=i;
                dax=0x7fffffff;
                day=0x7fffffff;
                dax2=0x80000000;
                day2=0x80000000;
                startwall=sector[i].wallptr;
                endwall=startwall+sector[i].wallnum-1;
                for(j=startwall;j<=endwall;j++) {
                    if(wall[j].x < dax) dax=wall[j].x;
                    if(wall[j].y < day) day=wall[j].y;
                    if(wall[j].x > dax2) dax2=wall[j].x;
                    if(wall[j].y > day2) day2=wall[j].y;
                    if(wall[j].lotag == 3) k=j;
                }
                if(wall[k].x == dax) dragxdir[dragsectorcnt]=-16;
                if(wall[k].y == day) dragydir[dragsectorcnt] = -16;
                if(wall[k].x == dax2) dragxdir[dragsectorcnt] = 16;
                if(wall[k].y == day2) dragydir[dragsectorcnt] = 16;

                dasector=wall[startwall].nextsector;
                dragx1[dragsectorcnt]=0x7fffffff;
                dragy1[dragsectorcnt]=0x7fffffff;
                dragx2[dragsectorcnt]=0x80000000;
                dragy2[dragsectorcnt]=0x80000000;
                startwall=sector[dasector].wallptr;
                endwall=startwall+sector[dasector].wallnum-1;
                for(j=startwall;j<=endwall;j++) {
                    if(wall[j].x < dragx1[dragsectorcnt]) dragx1[dragsectorcnt]=wall[j].x;
                    if(wall[j].y < dragy1[dragsectorcnt]) dragy1[dragsectorcnt]=wall[j].y;
                    if(wall[j].x > dragx2[dragsectorcnt]) dragx2[dragsectorcnt]=wall[j].x;
                    if(wall[j].y > dragy2[dragsectorcnt]) dragy2[dragsectorcnt]=wall[j].y;
                }

                dragx1[dragsectorcnt]+=(wall[sector[i].wallptr].x-dax);
                dragy1[dragsectorcnt]+=(wall[sector[i].wallptr].y-day);
                dragx2[dragsectorcnt]-=(dax2-wall[sector[i].wallptr].x);
                dragy2[dragsectorcnt]-=(day2-wall[sector[i].wallptr].y);

                dragfloorz[dragsectorcnt]=sector[i].floorz;

                dragsectorlist[dragsectorcnt++]=i;
            break;
            case 10:
            case 14:
            //case 15:
            //captureflag sector
            case 4002:
                warpsectorlist[warpsectorcnt++]=i;
            break;
            case 10000:
                bobbingsectorlist[bobbingsectorcnt++]=i;
            }
            if( sector[i].floorpicnum == TELEPAD
                && sector[i].lotag == 0)
                    warpsectorlist[warpsectorcnt++]=i;
    }

    ypanningwallcnt=0;
    for(i=0;i<numwalls;i++) {
        if(wall[i].lotag == 1)
            ypanningwalllist[ypanningwallcnt++]=i;
    }

    automapping=1;

    if(justteleported == 1 || loadedgame == 1) {

        plr->hvel=0;
        angvel=0;
        svel=0;
        vel=0;
        horizvel=0;

        plr->spritenum=insertsprite(plr->sector,0);
        plr->oldsector=plr->sector;

        sprite[plr->spritenum].x=plr->x;
        sprite[plr->spritenum].y=plr->y;
        sprite[plr->spritenum].z=sector[plr->sector].floorz;
        sprite[plr->spritenum].cstat=1+256;
        sprite[plr->spritenum].picnum=FRED;
        sprite[plr->spritenum].shade=0;
        sprite[plr->spritenum].xrepeat=36;
        sprite[plr->spritenum].yrepeat=36;
        sprite[plr->spritenum].ang=plr->ang;
        sprite[plr->spritenum].xvel=0;
        sprite[plr->spritenum].yvel=0;
        sprite[plr->spritenum].zvel=0;
        sprite[plr->spritenum].owner=4096;
        sprite[plr->spritenum].lotag=0;
        sprite[plr->spritenum].hitag=0;
        sprite[plr->spritenum].pal=1;
        setsprite(plr->spritenum,plr->x,plr->y,plr->z+(PLAYERHEIGHT<<8));
        justteleported=0;
        loadedgame=0;
    }

    movefifoplc=0;
    clearbufbyte(&movefifoend,sizeof(movefifoend),0);
    clearbufbyte(&ffsync[i],sizeof(ffsync),0L);
    clearbufbyte(&ssync[i],sizeof(ssync),0L);

}

#define   CSIZE  32

int outsideview=0;

void drawscreen(struct player *plr, int dasmoothratio) {
    int cposx, cposy, cposz, choriz;
    short cang;
    smoothratio = max(min(dasmoothratio,65536),0);

    if (plr == &player[myconnectindex] && (networkmode == 1 || myconnectindex != connecthead)) {
        // TODO
    } else {
        cposx = plr->ox+mulscale16(plr->x-plr->ox,smoothratio);
        cposy = plr->oy+mulscale16(plr->y-plr->oy,smoothratio);
        cposz = plr->oz+mulscale16(plr->z-plr->oz,smoothratio);
        choriz = plr->ohoriz+mulscale16(plr->horiz-plr->ohoriz,smoothratio);
        cang = plr->oang+mulscale16(((plr->ang+1024-plr->oang)&2047)-1024,smoothratio);
    }

    if (plr->dimension == 3 || plr->dimension == 2) {

        sprite[plr->spritenum].cstat |= 0x8000;
        drawrooms(cposx,cposy,cposz,cang,choriz,plr->sector);
        sprite[plr->spritenum].cstat &= ~0x8000;
        transformactors(plr);
        drawmasks();
        if( playerdie == 0)
            drawweapons(plr);
        if(spiked == 1) {
            spikeheart(plr);
        }
    }
    if (plr->dimension == 2) {
        drawoverheadmap(plr);
    }
    monitor();
    screenfx(plr);
    plr->oldsector=plr->sector;

    if (netgame) {
        whnetmon();
    }

    if (showfps) {
        int i,j;
        char fps[60];
        i = frameval[framecnt&(AVERAGEFRAMES-1)];
        j = frameval[framecnt&(AVERAGEFRAMES-1)] = getticks(); framecnt++;
        if (i != j) averagefps = ((mul3(averagefps)+((AVERAGEFRAMES*1000)/(j-i)) )>>2);
        #ifdef DEBUGGING
        snprintf(fps,sizeof(fps),"fps %d nummoves %d numframes %d totalclock %d",averagefps,nummoves,numframes,totalclock);
        #else
        snprintf(fps,sizeof(fps),"fps %d",averagefps);
        #endif
        printext256(0L,0L,31,-1,fps,0);
    }

    nextpage();

    while (ready2send && totalclock >= ototalclock+TICSPERFRAME)
        faketimerhandler();
}

int app_main(int argc,const char * const argv[]) {

    struct player *plr;

    plr=&player[0];

#if defined(DATADIR)
    {
        const char *datadir = DATADIR;
        if (datadir && datadir[0]) {
            addsearchpath(datadir);
        }
    }
#endif

    {
        char *supportdir = Bgetsupportdir(1);
        char *appdir = Bgetappdir();
        char dirpath[BMAX_PATH+1];

        // the OSX app bundle, or on Windows the directory where the EXE was launched
        if (appdir) {
            addsearchpath(appdir);
            free(appdir);
        }

        // the global support files directory
        if (supportdir) {
            Bsnprintf(dirpath, sizeof(dirpath), "%s/JFWitchaven", supportdir);
            addsearchpath(dirpath);
            free(supportdir);
        }
    }

    netcheckargs(argc,argv);

    // default behaviour is to write to the user profile directory, but
    // creating a 'user_profiles_disabled' file in the current working
    // directory where the game was launched makes the installation
    // "portable" by writing into the working directory
    if (access("user_profiles_disabled", F_OK) == 0) {
        char cwd[BMAX_PATH+1];
        if (getcwd(cwd, sizeof(cwd))) {
            addsearchpath(cwd);
        }
    } else {
        char *supportdir;
        char dirpath[BMAX_PATH+1];
        int asperr;

        if ((supportdir = Bgetsupportdir(0))) {
    #if defined(_WIN32) || defined(__APPLE__)
            const char *confdir = "JFWitchaven";
    #else
            const char *confdir = ".jfwhaven";
    #endif
            Bsnprintf(dirpath, sizeof(dirpath), "%s/%s", supportdir, confdir);
            asperr = addsearchpath(dirpath);
            if (asperr == -2) {
                if (Bmkdir(dirpath, S_IRWXU) == 0) {
                    asperr = addsearchpath(dirpath);
                } else {
                    asperr = -1;
                }
            }
            if (asperr == 0) {
                chdir(dirpath);
            }
            free(supportdir);
        }
    }

    buildsetlogfile("whaven.log");

    wm_setapptitle("JFWitchaven");
    buildprintf("\nJFWitchaven\n"
        "Based on WITCHAVEN Copyright (C) 1995 IntraCorp, Inc.\n"
        "Version %s.\nBuilt %s %s.\n",
      game_version, game_date, game_time);

    if (preinitengine()) {
        wm_msgbox("Build Engine Initialisation Error",
            "There was a problem initialising the Build engine: %s", engineerrstr);
        exit(1);
    }

    initgroupfile("stuff.dat");
    countlevels();
    if (numlevels > 3) {
        wm_setwindowtitle("Registered Version");
    } else {
        wm_setwindowtitle("3 Level Preview");
    }

    buildprintf(" map name: level%d\n",mapon);
    buildputs(" initengine()\n");

    memcpy(keys,defaultkeys,sizeof(keys));
    loadsetup("whaven.ini");

#if defined RENDERTYPEWIN || (defined RENDERTYPESDL && (defined __APPLE__ || defined HAVE_GTK))
    {
        int i;
        struct startwin_settings settings;

        memset(&settings, 0, sizeof(settings));
        settings.fullscreen = fullscreen;
        settings.xdim3d = xdimgame;
        settings.ydim3d = ydimgame;
        settings.bpp3d = bppgame;
        settings.forcesetup = forcesetup;
        settings.usemouse = !!(option[3]&1);
        settings.usejoy = !!(option[3]&2);
        settings.samplerate = digihz[option[7]>>4];
        settings.bitspersample = 1<<(((option[7]&2)>0)+3);
        settings.channels = ((option[7]&4)>0)+1;

        if (forcesetup) {
            if (startwin_run(&settings) == STARTWIN_CANCEL) {
                uninitengine();
                exit(0);
            }
        }

        fullscreen = settings.fullscreen;
        xdimgame = settings.xdim3d;
        ydimgame = settings.ydim3d;
        bppgame = settings.bpp3d;
        forcesetup = settings.forcesetup;
        option[3] = (settings.usemouse) | (settings.usejoy<<1);
        option[7] = 0;
        for (i=0;i<8;i++) if (digihz[i] <= settings.samplerate) option[7] = i<<4;
        option[7] |= (settings.bitspersample == 16)<<1;
        option[7] |= (settings.channels == 2)<<2;
    }
#endif

    if( option[8] == 1 || option[9] == 1)
        MusicMode=1;
    else
        MusicMode=0;

    if( option[10] == 1 || option[11] == 1)
        SoundMode=1;
    else
        SoundMode=0;


    initinput();
    if (option[3]&1) {                                        // Les 07/27/95
        initmouse();
    }                                                            // Les 07/27/95

     if (option[3]&2) {                                      // Les 07/27/95
          initjstick();                                          // Les 07/27/95
     }

    if (initengine()) {
        wm_msgbox("Build Engine Initialisation Error",
            "There was a problem initialising the Build engine: %s", engineerrstr);
        exit(1);
    }
    engineinitflag=1;

    pskyoff[0] = 0; pskyoff[1] = 0; pskybits = 1;

    inittimer(CLKIPS, NULL);
    buildputs(" loadpics()\n");
    buildputs(" tiles000.art\n");
    loadpics("tiles000.art",8*1048576);


    initlava();
    initwater();

    buildputs(" setupboard()\n");

    pyrn=0;
    buildprintf(" initplayersprite(%d)\n",pyrn);
    buildputs(" resettiming()\n");
    totalclock=0;

    //JSA_DEMO start menu song here use
    //flc_init();
    //flc_playseq(plr,0,FT_FULLSCREEN);

    //precache();

    playerdie=0;
    plr->oldsector=plr->sector;
    plr->horiz=plr->ohoriz=100;
    plr->zoom=256;
    plr->screensize=320;

    plr->dimension=3;
    plr->height=PLAYERHEIGHT;
    plr->z=sector[plr->sector].floorz-(plr->height<<8);

    //visibility=2048;
    visibility=1024;

    parallaxtype=1;

    setup3dscreen();
    SND_Startup();

    smkplayseq("intro");
    setbrightness(brightness, palette, 0);

    SND_MenuMusic(MENUSONG);

    initpaletteshifts();

    readpalettetable();

    OSD_RegisterFunction("showfps","showfps [state]: show frame rate", osdcmd_showfps);
    OSD_RegisterFunction("restartvid","restartvid: reinitialise the video mode",osdcmd_vidmode);
    OSD_RegisterFunction("vidmode","vidmode [xdim ydim] [bpp] [fullscreen]: immediately change the video mode",osdcmd_vidmode);
    OSD_RegisterFunction("setkeys","setkeys [modern|default]: set keys to modern (WASD) or default (arrows)", osdcmd_setkeys);
    OSD_RegisterFunction("mouselookmode","mouselookmode [onf]: set mouse look mode (0 momentary, 1 toggle)", osdcmd_setting);
    OSD_RegisterFunction("mousespeed","mousespeed [x] [y]: set mouse sensitivity (range 1 to 16)", osdcmd_setting);
    OSD_RegisterFunction("mousebutton","mousebutton [button] [action]: set mouse button actions (use 'mousebutton list' to show actions)", osdcmd_setting);
    OSD_RegisterFunction("joystickbutton","joystickbutton [button] [action]: set joystick button actions (use 'joystickbutton list' to show actions)", osdcmd_setting);
    OSD_RegisterFunction("keybutton","keybutton [scancode] [action]: set an action to a key scancode (use 'keybutton list' to show actions)", osdcmd_setting);

    gameactivated=0;
    escapetomenu=0;

/*
     if (option[MULTIOPT] != 0) {
          initmultiplayers(MAXPLAYERS);
     }
*/

    playloop();

    return 0;
}


void drawbackground(void) {
    struct player *plr;

    plr=&player[pyrn];

    flushperms();

    if(plr->screensize > 320) return;

    if(plr->screensize < 320) {
        rotatesprite(0,0,65536L,0,BACKGROUND,0,0,2+8+16+64+128,0,0,xdim-1,windowy1-1);
        rotatesprite(0,0,65536L,0,BACKGROUND,0,0,2+8+16+64+128,0,windowy1,windowx1-1,windowy2);
        rotatesprite(0,0,65536L,0,BACKGROUND,0,0,2+8+16+64+128,windowx2+1,windowy1,xdim-1,windowy2);
        rotatesprite(0,0,65536L,0,BACKGROUND,0,0,2+8+16+64+128,0,windowy2+1,xdim-1,ydim-1);
    }
    rotatesprite(0,(200-46)<<16,65536L,0,NEWSTATUSBAR,0,0,2+8+16+64+128,0,0,xdim-1,ydim-1);
    updatepics();
}

void setviewport(int screensize) {
    int dax, day, dax2, day2;

    if(screensize > 320) {
        dax=day=0;
        dax2=xdim;
        day2=ydim;
    }
    else {
        int width = scale(xdim, screensize, 320);
        int statusheight = scale(STATUSHEIGHT, ydim, 200);
        int height = scale(ydim - statusheight, screensize, 320);

        dax=(xdim - width)>>1;
        dax2=(xdim + width)>>1;
        day=(ydim - statusheight - height)>>1;
        day2=(ydim - statusheight + height)>>1;
    }
    setview(dax,day,dax2-1,day2-1);
}

void playloop(void) {

    struct player *plr;
    int exit=0, olockclock;

    plr=&player[pyrn];

    if (netgame) {
         initmulti(MAXPLAYERS);
    } else {
        initsingleplayers();
    }

    ready2send = 1;
    while (!exit) {
        handleevents();
        OSD_DispatchQueued();

        if(gameactivated == 0 || escapetomenu == 1) {
            exit=menuscreen(plr);
            setviewport(plr->screensize);
            drawbackground();
            plr->z=sector[plr->sector].floorz-(PLAYERHEIGHT<<8);
        }

        olockclock = lockclock;
        if (networkmode == 0 || !netgame) {
            while (movefifoplc != movefifoend[0]) domovethings();
        } else {
            // TODO
        }
        synctics = lockclock - olockclock;
        drawscreen(plr, (totalclock-gotlastpacketclock)*(65536/TICSPERFRAME));

        if (netgame) {
            // TODO: Reorganise into getpackets/sendpackets?
             netgetmove();
             netsendmove();
        }
        processinput(plr);
    }
}

void drawoverheadmap(struct player *plr) {

    int i, j, k, l, x1, y1, x2, y2, x3, y3, x4, y4, ox, oy, xoff, yoff;
    int dax, day, cosang, sinang, xspan, yspan, sprx, spry;
    int xrepeat, yrepeat, z1, z2, startwall, endwall, tilenum, daang;
    int xvect, yvect, xvect2, yvect2;
    char col;
    walltype *wal, *wal2;
    spritetype *spr;
    short cang;
    int czoom;

    cang=(short)plr->ang;
    czoom=plr->zoom;

    xvect = sintable[(2048-cang)&2047] * czoom;
    yvect = sintable[(1536-cang)&2047] * czoom;
    xvect2 = mulscale(xvect,yxaspect,16);
    yvect2 = mulscale(yvect,yxaspect,16);

        //Draw red lines
    for(i=0;i<numsectors;i++)
    {
        startwall = sector[i].wallptr;
        endwall = sector[i].wallptr + sector[i].wallnum - 1;

        z1 = sector[i].ceilingz; z2 = sector[i].floorz;

        for(j=startwall,wal=&wall[startwall];j<=endwall;j++,wal++)
        {
            k = wal->nextwall; if (k < 0) continue;

            if ((show2dwall[j>>3]&(1<<(j&7))) == 0) continue;
            if ((k > j) && ((show2dwall[k>>3]&(1<<(k&7))) > 0)) continue;

            if (sector[wal->nextsector].ceilingz == z1)
                if (sector[wal->nextsector].floorz == z2)
                    if (((wal->cstat|wall[wal->nextwall].cstat)&(16+32)) == 0) continue;

            col = 152;

            if (plr->dimension == 2)
            {
                if (sector[i].floorz != sector[i].ceilingz)
                    if (sector[wal->nextsector].floorz != sector[wal->nextsector].ceilingz)
                        if (((wal->cstat|wall[wal->nextwall].cstat)&(16+32)) == 0)
                            if (sector[i].floorz == sector[wal->nextsector].floorz) continue;
                if (sector[i].floorpicnum != sector[wal->nextsector].floorpicnum) continue;
                if (sector[i].floorshade != sector[wal->nextsector].floorshade) continue;
                col = 12;
            }

            ox = wal->x-plr->x; oy = wal->y-plr->y;
            x1 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
            y1 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

            wal2 = &wall[wal->point2];
            ox = wal2->x-plr->x; oy = wal2->y-plr->y;
            x2 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
            y2 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

            drawline256(x1+(xdim<<11),y1+(ydim<<11),x2+(xdim<<11),y2+(ydim<<11),col);
        }
    }

        //Draw sprites
    k = plr->spritenum;
    for(i=0;i<numsectors;i++)
        for(j=headspritesect[i];j>=0;j=nextspritesect[j])
            if ((show2dsprite[j>>3]&(1<<(j&7))) > 0)
            {
                spr = &sprite[j];
                col = 56;
                if ((spr->cstat&1) > 0) col = 248;
                if (j == k) col = 31;

                sprx = spr->x;
                spry = spr->y;

                k = spr->statnum;
                if ((k >= 1) && (k <= 8) && (k != 2))  //Interpolate moving sprite
                {
                    sprx = osprite[j].x+mulscale(sprx-osprite[j].x,smoothratio,16);
                    spry = osprite[j].y+mulscale(spry-osprite[j].y,smoothratio,16);
                }

                switch (spr->cstat&48)
                {
                    case 0:
                        ox = sprx-plr->x; oy = spry-plr->y;
                        x1 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
                        y1 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

                        if (plr->dimension == 1)
                        {
                            ox = (sintable[(spr->ang+512)&2047]>>7);
                            oy = (sintable[(spr->ang)&2047]>>7);
                            x2 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
                            y2 = mulscale(oy,xvect,16) + mulscale(ox,yvect,16);

                            if (j == plr->spritenum)
                            {
                                x2 = 0L;
                                y2 = -(czoom<<5);
                            }

                            x3 = mulscale(x2,yxaspect,16);
                            y3 = mulscale(y2,yxaspect,16);

                            drawline256(x1-x2+(xdim<<11),y1-y3+(ydim<<11),
                                            x1+x2+(xdim<<11),y1+y3+(ydim<<11),col);
                            drawline256(x1-y2+(xdim<<11),y1+x3+(ydim<<11),
                                            x1+x2+(xdim<<11),y1+y3+(ydim<<11),col);
                            drawline256(x1+y2+(xdim<<11),y1-x3+(ydim<<11),
                                            x1+x2+(xdim<<11),y1+y3+(ydim<<11),col);
                        }
                        else
                        {
                            if (((gotsector[i>>3]&(1<<(i&7))) > 0) && (czoom > 192))
                            {
                                daang = (spr->ang-cang)&2047;
                                if (j == plr->spritenum )
                                    { x1 = 0; y1 = (yxaspect<<2); daang = 0; }
                                rotatesprite((x1<<4)+(xdim<<15),(y1<<4)+(ydim<<15),mulscale(czoom*spr->yrepeat,yxaspect,16),daang,spr->picnum,spr->shade,spr->pal,(spr->cstat&2)>>1,0,0,xdim-1,ydim-1);
                            }
                        }
                        break;
                    case 16:
                        x1 = sprx; y1 = spry;
                        tilenum = spr->picnum;
                        xoff = (int)((signed char)((picanm[tilenum]>>8)&255))+((int)spr->xoffset);
                        if ((spr->cstat&4) > 0) xoff = -xoff;
                        k = spr->ang; l = spr->xrepeat;
                        dax = sintable[k&2047]*l; day = sintable[(k+1536)&2047]*l;
                        l = tilesizx[tilenum]; k = (l>>1)+xoff;
                        x1 -= mulscale(dax,k,16); x2 = x1+mulscale(dax,l,16);
                        y1 -= mulscale(day,k,16); y2 = y1+mulscale(day,l,16);

                        ox = x1-plr->x; oy = y1-plr->y;
                        x1 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
                        y1 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

                        ox = x2-plr->x; oy = y2-plr->y;
                        x2 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
                        y2 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

                        drawline256(x1+(xdim<<11),y1+(ydim<<11),
                                        x2+(xdim<<11),y2+(ydim<<11),col);

                        break;
                    case 32:
                        if (plr->dimension == 1)
                        {
                            tilenum = spr->picnum;
                            xoff = (int)((signed char)((picanm[tilenum]>>8)&255))+((int)spr->xoffset);
                            yoff = (int)((signed char)((picanm[tilenum]>>16)&255))+((int)spr->yoffset);
                            if ((spr->cstat&4) > 0) xoff = -xoff;
                            if ((spr->cstat&8) > 0) yoff = -yoff;

                            k = spr->ang;
                            cosang = sintable[(k+512)&2047]; sinang = sintable[k];
                            xspan = tilesizx[tilenum]; xrepeat = spr->xrepeat;
                            yspan = tilesizy[tilenum]; yrepeat = spr->yrepeat;

                            dax = ((xspan>>1)+xoff)*xrepeat; day = ((yspan>>1)+yoff)*yrepeat;
                            x1 = sprx + mulscale(sinang,dax,16) + mulscale(cosang,day,16);
                            y1 = spry + mulscale(sinang,day,16) - mulscale(cosang,dax,16);
                            l = xspan*xrepeat;
                            x2 = x1 - mulscale(sinang,l,16);
                            y2 = y1 + mulscale(cosang,l,16);
                            l = yspan*yrepeat;
                            k = -mulscale(cosang,l,16); x3 = x2+k; x4 = x1+k;
                            k = -mulscale(sinang,l,16); y3 = y2+k; y4 = y1+k;

                            ox = x1-plr->x; oy = y1-plr->y;
                            x1 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
                            y1 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

                            ox = x2-plr->x; oy = y2-plr->y;
                            x2 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
                            y2 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

                            ox = x3-plr->x; oy = y3-plr->y;
                            x3 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
                            y3 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

                            ox = x4-plr->x; oy = y4-plr->y;
                            x4 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
                            y4 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

                            drawline256(x1+(xdim<<11),y1+(ydim<<11),
                                            x2+(xdim<<11),y2+(ydim<<11),col);

                            drawline256(x2+(xdim<<11),y2+(ydim<<11),
                                            x3+(xdim<<11),y3+(ydim<<11),col);

                            drawline256(x3+(xdim<<11),y3+(ydim<<11),
                                            x4+(xdim<<11),y4+(ydim<<11),col);

                            drawline256(x4+(xdim<<11),y4+(ydim<<11),
                                            x1+(xdim<<11),y1+(ydim<<11),col);

                        }
                        break;
                }
            }

        //Draw white lines
    for(i=0;i<numsectors;i++)
    {
        startwall = sector[i].wallptr;
        endwall = sector[i].wallptr + sector[i].wallnum - 1;

        for(j=startwall,wal=&wall[startwall];j<=endwall;j++,wal++)
        {
            if (wal->nextwall >= 0) continue;

            if ((show2dwall[j>>3]&(1<<(j&7))) == 0) continue;

            if (tilesizx[wal->picnum] == 0) continue;
            if (tilesizy[wal->picnum] == 0) continue;

            ox = wal->x-plr->x; oy = wal->y-plr->y;
            x1 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
            y1 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

            wal2 = &wall[wal->point2];
            ox = wal2->x-plr->x; oy = wal2->y-plr->y;
            x2 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
            y2 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

            drawline256(x1+(xdim<<11),y1+(ydim<<11),x2+(xdim<<11),y2+(ydim<<11),24);
        }
    }
}



void readpalettetable(void) {
      int fp;
      int  i,j;
      int num_tables,lookup_num;
      unsigned char tempbuf[256];

      if ((fp=kopen4load("lookup.dat",0)) < 0) {
             return;
      }
      num_tables=kgetc(fp);
      for (j=0 ; j < num_tables ; j++) {
             lookup_num=kgetc(fp);
             for (i=0 ; i < 256 ; i++) {
                    tempbuf[i]=(unsigned char)kgetc(fp);
             }
             makepalookup(lookup_num,tempbuf,0,0,0,1);
      }
      kclose(fp);
}

int adjusthp(int hp) {

    float factor;
    int   howhard;

    factor=(krand()%20)/100;
    howhard=difficulty;

    if( krand()%100 > 50)
        return((hp*(factor+1))*howhard);
    else
        return((hp-(hp*(factor)))*howhard);

}


static int osdcmd_showfps(const osdfuncparm_t *parm)
{
    if (parm->numparms == 1) {
        if (parm->numparms == 1) showfps = min(1,(unsigned)atoi(parm->parms[0]));
        buildprintf("showfps is %u\n", showfps);
        return OSDCMD_OK;
    }
    return OSDCMD_SHOWHELP;
}

static int osdcmd_vidmode(const osdfuncparm_t *parm)
{
    int newx = xdim, newy = ydim, newbpp = bpp, newfullscreen = fullscreen;

    if (!strcasecmp(parm->name, "restartvid")) {
        resetvideomode();
    } else {
        if (parm->numparms < 1 || parm->numparms > 4) return OSDCMD_SHOWHELP;
        switch (parm->numparms) {
            case 1:   // bpp switch
                newbpp = atoi(parm->parms[0]);
                break;
            case 2: // res switch
                newx = atoi(parm->parms[0]);
                newy = atoi(parm->parms[1]);
                break;
            case 3:   // res & bpp switch
            case 4:
                newx = atoi(parm->parms[0]);
                newy = atoi(parm->parms[1]);
                newbpp = atoi(parm->parms[2]);
                if (parm->numparms == 4)
                    newfullscreen = (atoi(parm->parms[3]) != 0);
                break;
        }
        if (checkvideomode(&newx, &newy, newbpp, newfullscreen, 0) < 0) {
            buildputs("vidmode: unrecognised video mode.\n");
            return OSDCMD_OK;
        }
    }

    if (setgamemode(newfullscreen,newx,newy,newbpp))
        buildputs("vidmode: Mode change failed!\n");
    else {
        xdimgame = newx;
        ydimgame = newy;
        bppgame = newbpp;
        fullscreen = newfullscreen;
        setup3dscreen();
    }
    return OSDCMD_OK;
}

static int osdcmd_setkeys(const osdfuncparm_t *parm)
{
    if (parm->numparms == 1) {
        if (!strcasecmp(parm->parms[0], "modern")) {
            memcpy(keys,modernkeys,sizeof(keys)-sizeof(keys[KEYCONSOLE]));
            buildputs("keys set to modern\n");
            return OSDCMD_OK;
        }
        else if (!strcasecmp(parm->parms[0], "default")) {
            memcpy(keys,defaultkeys,sizeof(keys)-sizeof(keys[KEYCONSOLE]));
            buildputs("keys set to default\n");
            return OSDCMD_OK;
        }
    }
    return OSDCMD_SHOWHELP;
}

static const struct {
    const char *name;
    int key;
} keymap[] = {
    { "backward", KEYBACK },
    { "turnleft", KEYLEFT },
    { "turnright", KEYRIGHT },
    { "run", KEYRUN },
    { "strafe", KEYSTRAFE },
    { "fire", KEYFIRE },
    { "use", KEYUSE },
    { "jump", KEYJUMP },
    { "crouch", KEYCROUCH },
    { "lookup", KEYLKUP },
    { "lookdown", KEYLKDN },
    { "centre", KEYCNTR },
    { "strafeleft", KEYSTFL },
    { "straferight", KEYSTFR },
    { "map", KEYMAP },
    { "zoomin", KEYZOOMI },
    { "zoomout", KEYZOOMO },
    { "usepotion", KEYUSEP },
    { "cast", KEYCAST },
    { "fly", KEYFLY },
    { "looking", KEYLOOKING },
    { "uncast", KEYUNCAST },
    { "flyup", KEYFLYUP },
    { "flydown", KEYFLYDN },
    { "console", KEYCONSOLE },
};
static int matchkeyname(const char *name) {
    for (unsigned i=0; i<Barraylen(keymap); i++)
        if (!strcasecmp(name, keymap[i].name))
            return keymap[i].key;
    return -1;
}
static const char * matchkeynum(int key) {
    for (unsigned i=0; i<Barraylen(keymap); i++)
        if (keymap[i].key == key)
            return keymap[i].name;
    return NULL;
}

static int osdcmd_setting(const osdfuncparm_t *parm)
{
    int button, action;
    const char *name;

    if (!strcasecmp(parm->name, "mouselookmode")) {
        if (parm->numparms == 1) mouselookmode = (int)max(0,min(1,(unsigned)atoi(parm->parms[0])));
        buildprintf("mouselookmode is %d\n", mouselookmode);
        return OSDCMD_OK;
    }
    else if (!strcasecmp(parm->name, "mousespeed")) {
        if (parm->numparms == 2) {
            mousxspeed = max(1,min(16,(unsigned)atoi(parm->parms[0])));
            mousyspeed = max(1,min(16,(unsigned)atoi(parm->parms[1])));
        }
        buildprintf("mousespeed is %d,%d\n", mousxspeed,mousyspeed);
        return OSDCMD_OK;
    }
    else if (!strcasecmp(parm->name, "mousebutton")) {
        if (parm->numparms == 1 && !strcasecmp("list", parm->parms[0])) {
            buildputs("mouse buttons\n");
            for (button=0; button<2; button++) {
                name = matchkeynum(mousekeys[button]);
                buildprintf(" %u ... %s\n", button+1, name ? name : "");
            }
            return OSDCMD_OK;
        } else if (parm->numparms == 2) {
            button = atoi(parm->parms[0]);
            action = matchkeyname(parm->parms[1]);
            if (button < 1 || button > 2) return OSDCMD_SHOWHELP;
            if (action < 0 || action == KEYCONSOLE) return OSDCMD_SHOWHELP;
            mousekeys[button-1] = action;
            return OSDCMD_OK;
        }
    }
    else if (!strcasecmp(parm->name, "joystickbutton")) {
        if (parm->numparms == 1 && !strcasecmp("list", parm->parms[0])) {
            buildputs("joystick buttons\n");
            for (button=0; button<4; button++) {
                const char *name = matchkeynum(joykeys[button]);
                buildprintf(" %u ... %s\n", button+1, name ? name : "");
            }
            return OSDCMD_OK;
        } else if (parm->numparms == 2) {
            button = atoi(parm->parms[0]);
            action = matchkeyname(parm->parms[1]);
            if (button < 1 || button > 4) return OSDCMD_SHOWHELP;
            if (action < 0 || action == KEYCONSOLE) return OSDCMD_SHOWHELP;
            joykeys[button-1] = action;
            return OSDCMD_OK;
        }
    }
    else if (!strcasecmp(parm->name, "keybutton")) {
        if (parm->numparms == 1 && !strcasecmp("list", parm->parms[0])) {
            buildputs("key buttons\n");
            for (int i=0; i < (int)Barraylen(keymap); i++)
                buildprintf(" %-11s ... %02X\n", keymap[i].name, keys[keymap[i].key]);
            return OSDCMD_OK;
        } else if (parm->numparms == 2) {
            button = (int)strtoul(parm->parms[0], NULL, 16);
            action = matchkeyname(parm->parms[1]);
            if (button < 2 || button > 255) return OSDCMD_SHOWHELP;
            if (action < 0) return OSDCMD_SHOWHELP;
            keys[action] = button;
            if (action == KEYCONSOLE) OSD_CaptureKey(button);
            return OSDCMD_OK;
        }
    }
    return OSDCMD_SHOWHELP;
}

