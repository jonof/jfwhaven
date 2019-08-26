/***************************************************************************
 *    WITCHAVEN.C  - main game code for Witchaven game                     *
 *                                                                         *
 ***************************************************************************/

#define WHAVEN
#define GAME
#define SVGA
#include "icorp.h"

jmp_buf jmpenv;

long *animateptr[MAXANIMATES],
      animategoal[MAXANIMATES],
      animatevel[MAXANIMATES],
      animatecnt=0;

unsigned long flags32[32]={
      0x80000000,0x40000000,0x20000000,0x10000000,
      0x08000000,0x04000000,0x02000000,0x01000000,
      0x00800000,0x00400000,0x00200000,0x00100000,
      0x00080000,0x00040000,0x00020000,0x00010000,
      0x00008000,0x00004000,0x00002000,0x00001000,
      0x00000800,0x00000400,0x00000200,0x00000100,
      0x00000080,0x00000040,0x00000020,0x00000010,
      0x00000008,0x00000004,0x00000002,0x00000001
};

extern short cddrive;

long followx, followy;

int ratcnt=0;

int gameactivated=0;
int escapetomenu=0;

long selectedgun;
int  difficulty=2;

long    totsynctics,
        frames;

extern int followmode;
extern int loadedgame;
extern char tempbuf[50];
extern int  spiked;

extern short brightness, gbrightness;
extern int goreon;
extern int musiclevel;
extern int digilevel;


struct delayitem delayitem[MAXSECTORS];

int   actorcnt,
      secnt,
      sexref[MAXSECTORS],
      swingcnt;

short spikecnt;
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
long revolvex[16][16], revolvey[16][16];
long revolvepivotx[16], revolvepivoty[16];

short dragsectorlist[16], dragxdir[16], dragydir[16], dragsectorcnt;
long dragx1[16], dragy1[16], dragx2[16], dragy2[16], dragfloorz[16];

short warpsectorlist[16], warpsectorcnt;

short bobbingsectorlist[16], bobbingsectorcnt;

extern volatile  unsigned joetime;     //temp

//JSA ends

int justteleported=0;
int playerdie=0;

int wet=0;
int oldvmode;
long dasizeofit;

short ironbarsector[16],ironbarscnt;
long  ironbarsgoal1[16],ironbarsgoal2[16];
short ironbarsdone[16], ironbarsanim[16];
long  ironbarsgoal[16];

extern int mapon;
int mapflag;
//
//
//
void (__interrupt __far *oldtimerhandler)();
void __interrupt __far timerhandler(void);

void (__interrupt __far *oldkeyhandler)();
void __interrupt __far keyhandler(void);


    //Here are some nice in-line assembly pragmas that make life easier.
    //(at least for Ken)
#pragma aux setvmode =\
    "int 0x10",\
    parm [eax]\

#pragma aux clearbuf =\
    "rep stosd",\
    parm [edi][ecx][eax]\

#pragma aux clearbufbyte =\
    "shr ecx, 1",\
    "jnc skip1",\
    "stosb",\
    "skip1: shr ecx, 1",\
    "jnc skip2",\
    "stosw",\
    "skip2: test ecx, ecx",\
    "jz skip3",\
    "rep stosd",\
    "skip3:",\
    parm [edi][ecx][eax]\

#pragma aux copybuf =\
    "rep movsd",\
    parm [esi][edi][ecx]\

#pragma aux copybufbyte =\
    "shr ecx, 1",\
    "jnc skip1",\
    "movsb",\
    "skip1: shr ecx, 1",\
    "jnc skip2",\
    "movsw",\
    "skip2: test ecx, ecx",\
    "jz skip3",\
    "rep movsd",\
    "skip3:",\
    parm [esi][edi][ecx]\

#pragma aux copybufreverse =\
    "shr ecx, 1",\
    "jnc skipit1",\
    "mov al, byte ptr [esi]",\
    "dec esi",\
    "mov byte ptr [edi], al",\
    "inc edi",\
    "skipit1: shr ecx, 1",\
    "jnc skipit2",\
    "mov ax, word ptr [esi-1]",\
    "sub esi, 2",\
    "ror ax, 8",\
    "mov word ptr [edi], ax",\
    "add edi, 2",\
    "skipit2: test ecx, ecx",\
    "jz endloop",\
    "begloop: mov eax, dword ptr [esi-3]",\
    "sub esi, 4",\
    "bswap eax",\
    "mov dword ptr [edi], eax",\
    "add edi, 4",\
    "dec ecx",\
    "jnz begloop",\
    "endloop:",\
    parm [esi][edi][ecx]\

#pragma aux klabs =\
    "test eax, eax",\
    "jns skipnegate",\
    "neg eax",\
    "skipnegate:",\
    parm [eax]\

#pragma aux ksgn =\
    "add ebx, ebx",\
    "sbb eax, eax",\
    "cmp eax, ebx",\
    "adc eax, 0",\
    parm [ebx]\

#pragma aux mulscale =\
    "imul ebx",\
    "shrd eax, edx, cl",\
    parm [eax][ebx][ecx]\
    modify [edx]\

#pragma aux divscale =\
    "mov edx, eax",\
    "sar edx, 31",\
    "shld edx, eax, cl",\
    "sal eax, cl",\
    "idiv ebx",\
    parm [eax][ebx][ecx]\
    modify [edx]\

#pragma aux scale =\
    "imul ebx",\
    "idiv ecx",\
    parm [eax][ebx][ecx]\
    modify [eax edx]\

#pragma aux umin =\
    "sub eax,ebx",\
    "sbb ecx,ecx",\
    "and eax,ecx",\
    "add eax,ebx",\
    parm [eax][ebx]\
    modify [ecx]\

#pragma aux umax =\
    "sub eax,ebx",\
    "sbb ecx,ecx",\
    "not ecx",\
    "and eax,ecx",\
    "add eax,ebx",\
    parm [eax][ebx]\
    modify [ecx]\

#pragma aux drawpixel =\
    "mov byte ptr [edi], al",\
    parm [edi][eax]\

#pragma aux koutp =\
    "out dx, al",\
    parm [edx][eax]\

#pragma aux koutpw =\
    "out dx, ax",\
    parm [edx][eax]\

#pragma aux kinp =\
    "in al, dx",\
    parm [edx]\


void faketimerhandler(void) {
    return;
}

//   basic text functions from tekwar
int getvmode(void) {

      union REGS regs;

      regs.h.ah=0x0F;
      regs.h.al=0x00;
      int386(0x10,&regs,&regs);
      return(regs.h.al);
}

//
//   basic text functions
//

void cls80x25(int top,int mid,int bot) {

      int  i,j;
      char *ptr;

      ptr=(char *)0xB8000;
      memset(ptr,0,4000);
      for (i=0 ; i < 80 ; i++) {
             *(ptr+(i*2)+1)=top;           // top line attribute set to gray on blue
      }
      *ptr='Û';                          // hides the cursor behind this block
      *(ptr+1)=(char)(((top&0x07)<<4)+((top&0xF0)>>4));
      for (j=1 ; j < 24 ; j++) {         // and rest of screen set to gray
             for (i=0 ; i < 80 ; i++) {
                    *(ptr+(i*2)+(j*160)+1)=mid;
             }
      }
      j=24;
      for (i=0 ; i < 80 ; i++) {
             *(ptr+(i*2)+(j*160)+1)=bot;   // and last line is white on red
      }

}

void tprintf(int x,int y,char *fmt,...) {

      int  i;
      static int curx=0,cury=0;
      char buf[80],*ptr;
      va_list argptr;

      ptr=(char *)0xB8000;
      va_start(argptr,fmt);
      vsprintf(buf,fmt,argptr);
      va_end(argptr);
      if (x == 0) {
             x=curx;
      }
      else if (x == -1) {
             x=40-(strlen(buf)/2);
      }
      if (y == 0) {
             y=cury;
      }
      ptr=(char *)(0xB8000+(x*2)+(y*160));
      for (i=0 ; i < strlen(buf) ; i++) {
             *ptr=buf[i];
             ptr+=2;
      }
      curx=x+strlen(buf);
      cury=y;

}

void rp_delay(long goal) {

        long dagoal=0L;
        int exit=0;

        dagoal=totalclock+goal;

        while ( !exit ) {
            if( totalclock == dagoal )
                exit=1;
        }

}



//
//   game code
//

void
showadditionalinfo(void)
{
      printf("average synctics = %ld\n",totsynctics/frames);
}

int  crashflag;

void
shutdown(void)
{

       int  fil;

        fil=open("pref.dat",O_BINARY|O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE );
        if( fil != NULL ) {
            write(fil,&goreon,2);
            write(fil,&brightness,2);
            write(fil,&gbrightness,2);
            write(fil,&digilevel,2);
            write(fil,&musiclevel,2);
            write(fil,&difficulty,2);
            close(fil);
        }


      SND_Shutdown();

      netshutdown();

      if (engineinitflag) {
             uninitengine();
      }

      if(SoundMode)
          SND_UnDoBuffers();

      if (videoinitflag) {
             setvmode(oldvmode);
      }

        uninitkeys();
        uninitgroupfile();

      if (crashflag) {
          return;
      }

      exit(0);

}

void crash(char *fmt) {

      crashflag=1;
      shutdown();

      printf("\n%s\n",fmt);
      printf("\n\n\n\n\n\n\n\n");
      longjmp(jmpenv,1);

}

void doanimations(long numtics) {

      long i,animval;

      for (i=animatecnt-1 ; i >= 0 ; i--) {
             animval=*animateptr[i];
             if (animval < animategoal[i]) {
                    animval+=(numtics*animatevel[i]);
                    if (animval > animategoal[i]) {
                          animval=animategoal[i];
                    }
             }
             if (animval > animategoal[i]) {
                    animval-=(numtics*animatevel[i]);
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

long getanimationgoal(long *animptr)
{
    long i, j;

    j = -1;
    for(i=0;i<animatecnt;i++)
        if (*animptr == animateptr[i])
        {
            j = i;
            break;
        }
    return(j);
}

long setanimation(long *animptr,long thegoal,long thevel) {

      long i,j;

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


void setdelayfunc(void (*func)(),int item,int delay) {

      int  i,j;

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

void dodelayitems(int tics) {

      int  cnt,i,j;

      cnt=delaycnt;
      for (i=0 ; i < cnt ; i++) {
             if (delayitem[i].func == NULL) {
                    j=delaycnt-1;
                    memmove(&delayitem[i],&delayitem[j],sizeof(struct delayitem));
                    delaycnt=j;
             }
             if (delayitem[i].timer > 0) {
                    if ((delayitem[i].timer-=tics) <= 0) {
                          delayitem[i].timer=0;
                          (*delayitem[i].func)(delayitem[i].item);
                          delayitem[i].func=NULL;
                    }
             }
      }
}


void setup3dscreen(void) {

    struct player *plr;
    long i, dax, day, dax2, day2;

    plr=&player[0];
    setgamemode();

    videoinitflag=1;

    switch(option[0]) {
    case 1:
       if( svga == 0 ) {
            dax=160-(plr->screensize>>1);
            dax2=dax+plr->screensize-1;
            day=84-(((plr->screensize*168)/320)>>1);
            day2=STATUSSCREEN-1;
            setview(dax,day,dax2,day2);
       }
       else
        setview(0L,0L,vesares[option[6]&15][0]-1,vesares[option[6]&15][1]-1);
    break;
    default:
        setview(0L,0L,319L,199L);
    break;
    }

    if(svga == 0) {
        if(plr->screensize <= 320)
            permanentwritesprite(0,0,BACKGROUND,0,0,0,319,199,0);
        if(plr->screensize <= 320)
            permanentwritesprite(0,200-46,NEWSTATUSBAR,0,0,0,319,199,0);
    }
    else if(svga == 1) {
        permanentwritesprite(0,0,SVGAMENU,0,0,0,639,239,0);
        permanentwritesprite(0,240,SVGAMENU2,0,0,240,639,479,0);
    }

}


void setupboard(char *fname) {

    int  effect,endwall,i,j,k,s,startwall;
    struct player *plr;
    long dax, day;
    short treesize;
    long dasector;
    long dax2, day2;
    short daang;

    plr=&player[0];

    randomseed = 17L;

    if (loadboard(fname,&plr->x,&plr->y,&plr->z,&daang,&plr->sector) == -1) {
        crash("Board not found");
    }

    plr->ang=daang;

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


}

#define   CSIZE  32

int outsideview=0;

void drawscreen(struct player *plr) {

    long dax, day, dax2, day2;
    long olddist;
    long dist;
    int i,k;
    short daang;


    if (plr->dimension == 3 || plr->dimension == 2) {

        drawrooms(plr->x,plr->y,plr->z,plr->ang,plr->horiz,plr->sector);
        transformactors(plr);
        drawmasks();
        if( playerdie == 0)
            drawweapons(plr);
        if(spiked == 1) {
            spikeheart(plr);
        }
        dofx();
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

    nextpage();

}

extern
char option2[];

extern
short mousekeys[];

void main(int argc,char *argv[]) {

    int fil;
    long i, j, k, l;
    char *ptr;
    char temp1[10]={"DEMO"};
    struct player *plr;
    int buttons;
    int done;
    unsigned vixen;

    plr=&player[0];


    if (setjmp(jmpenv) != 0) {
        printf("!! Program abnormally terminated !!");
        exit(1);
    }
    installcrerrhndlr();

    netcheckargs(argc,argv);

    initgroupfile("stuff.dat");

     oldvmode=getvmode();
     setvmode(0x03);                                             // Les 07/24/95
     cls80x25(0x1F,0x07,0x0F);

    tprintf(-1,0,"WITCHAVEN Copyright (C) 1995 IntraCorp, Inc. ver 1.1",temp1);

    tprintf(2,2," map name: level%d",mapon);

    tprintf(2,3," initengine()");

    if ((fil = open("setup.dat",O_BINARY|O_RDWR,S_IREAD)) != -1) {
        read(fil,&option,NUMOPTIONS);
        read(fil,keys,NUMKEYS);                                  // Les 07/24/95
        read(fil,option2,7);                                     // Les 07/27/95
        close(fil);
    }


    if (access("pref.dat",F_OK) != 0) {
        goreon=1;
        brightness=gbrightness=0;
        digilevel=11;
        musiclevel=11;
        fil=open("pref.dat",O_BINARY|O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE );
        if( fil != NULL ) {
            write(fil,&goreon,2);
            write(fil,&brightness,2);
            write(fil,&gbrightness,2);
            write(fil,&digilevel,2);
            write(fil,&musiclevel,2);
            write(fil,&difficulty,2);
            close(fil);
        }
     }
     else {
        fil=open("pref.dat", O_RDONLY | O_BINARY);
        if( fil != NULL ) {
            read(fil,&goreon,2);
            read(fil,&brightness,2);
            read(fil,&gbrightness,2);
            read(fil,&digilevel,2);
            read(fil,&musiclevel,2);
            read(fil,&difficulty,2);
            close(fil);
        }
     }

    if( option[8] == 1 || option[9] == 1)
        MusicMode=1;
    else
        MusicMode=0;

    if( option[10] == 1 || option[11] == 1)
        SoundMode=1;
    else
        SoundMode=0;

    //JSA_SPOOGE
    if(SoundMode)
        SND_DoBuffers();


    if (option[3] != 0) {                                        // Les 07/27/95
        initmouse();
        mousekeys[0]=option2[0];                                 // Les 07/27/95
        mousekeys[1]=option2[1];                                 // Les 07/27/95
    }                                                            // Les 07/27/95

     if (option2[2] != 0) {                                      // Les 07/27/95
          initjstick();                                          // Les 07/27/95
     }                                                           // Les 07/27/95

    switch(option[0]) {
    case 1:
        initengine(1,vesares[option[6]&15][0],vesares[option[6]&15][1]);
        if(vesares[option[6]&15][0] == 640)
            svga=1;
    break;
    default:
        initengine(1,320,200);
    break;
    }
    initkeys();
    engineinitflag=1;

    pskyoff[0] = 0; pskyoff[1] = 0; pskybits = 1;

    SND_Startup();
    tprintf(2,4," loadpics()");
    tprintf(2,5," tiles000.art");
    loadpics("tiles000.art");


#if 0
     if (vixen=ismscdex()) {
          sprintf(tempbuf,"%c:\\whaven\\intro.smk",vixen);
          if (access(tempbuf,F_OK) != 0) {
              crash("\nCD-ROM NOT DETECTED  \n");
         }
     }
#endif
    initlava();
    initwater();

    tprintf(2,6," setupboard()");

    pyrn=0;
    tprintf(2,7," initplayersprite(%d)",pyrn);
    tprintf(2,8," resettiming()");
    totalclock=0;

    //JSA_DEMO start menu song here use
    //flc_init();
    //flc_playseq(plr,0,FT_FULLSCREEN);

    SND_MenuMusic(MENUSONG);

    //precache();

    playerdie=0;
    plr->oldsector=plr->sector;
    plr->horiz=100;
    plr->zoom=256;
    if( svga == 1)
        plr->screensize=328;
    else
        plr->screensize=320;

    plr->dimension=3;
    plr->height=PLAYERHEIGHT;
    plr->z=sector[plr->sector].floorz-(plr->height<<8);

    //visibility=2048;
    visibility=1024;

    parallaxtype=1;

    setup3dscreen();

    //intro();

    initpaletteshifts();

    readpalettetable();

    gameactivated=0;
    escapetomenu=0;

/*
     if (option[MULTIOPT] != 0) {
          initmultiplayers(MAXPLAYERS);
     }
*/

    playloop();

}


void intro(void) {

    int i;


    //loadtile(CAPSTONE);

    //for(i=0;i<32;i++) {
    //   overwritesprite(80,40,CAPSTONE,0,0,0);
    //   nextpage();
    //}


    loadtile(TITLEPIC);

    for(i=0;i<32;i++) {
        overwritesprite(0L,0L,TITLEPIC,32-i,0,0);
        nextpage();
    }

    rp_delay(120);

    for(i=0;i<32;i++) {
        overwritesprite(0L,0L,TITLEPIC,i,0,0);
        nextpage();
    }

}

void playloop(void) {

    struct player *plr;
    int exit=0;

    plr=&player[pyrn];

    if (netgame) {
         initmulti(MAXPLAYERS);
    }

    while (!exit) {

        if(gameactivated == 0 || escapetomenu == 1) {
            exit=menuscreen(plr);
            if( svga == 0 ) {
                permanentwritesprite(0,0,BACKGROUND,0,0,0,319,199,0);
                permanentwritesprite(0,200-46,STATUSBAR,0,0,0,319,199,0);
                updatepics();
            }
            if( svga == 1) {
                if(plr->screensize == 320) {
                    overwritesprite(0,372,SSTATUSBAR,0,0,0);
                    updatepics();
                }
            }
            plr->z=sector[plr->sector].floorz-(PLAYERHEIGHT<<8);
        }

        drawscreen(plr);
        processinput(plr);

        if (netgame) {
             netgetmove();
             netsendmove();
        }
        processobjs(plr);
        animateobjs(plr);
        animatetags(plr);
        doanimations((long)synctics);
        dodelayitems((long)synctics);

    }


}

void drawoverheadmap(struct player *plr) {

    long i, j, k, l, x1, y1, x2, y2, x3, y3, x4, y4, ox, oy, xoff, yoff;
    long dax, day, cosang, sinang, xspan, yspan, sprx, spry;
    long xrepeat, yrepeat, z1, z2, startwall, endwall, tilenum, daang;
    long xvect, yvect, xvect2, yvect2;
    char col;
    walltype *wal, *wal2;
    spritetype *spr;
    short cang;
    long czoom;

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
                                rotatesprite((x1<<4)+(xdim<<15),(y1<<4)+(ydim<<15),mulscale(czoom*spr->yrepeat,yxaspect,16),daang,spr->picnum,spr->shade,spr->pal,(spr->cstat&2)>>1);
                            }
                        }
                        break;
                    case 16:
                        x1 = sprx; y1 = spry;
                        tilenum = spr->picnum;
                        xoff = (long)((signed char)((picanm[tilenum]>>8)&255))+((long)spr->xoffset);
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
                            xoff = (long)((signed char)((picanm[tilenum]>>8)&255))+((long)spr->xoffset);
                            yoff = (long)((signed char)((picanm[tilenum]>>16)&255))+((long)spr->yoffset);
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
      FILE *fp;
      int  i,j;
      char num_tables,lookup_num;

      if ((fp=fopen("lookup.dat","rb")) == NULL) {
             return;
      }
      num_tables=getc(fp);
      for (j=0 ; j < num_tables ; j++) {
             lookup_num=getc(fp);
             for (i=0 ; i < 256 ; i++) {
                    tempbuf[i]=getc(fp);
             }
             makepalookup(lookup_num,tempbuf,0,0,0,1);
      }
      fclose;
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


void inittimer(void)
{
    outp(0x43,54); outp(0x40,9942&255); outp(0x40,9942>>8);  //120 times/sec
    oldtimerhandler = _dos_getvect(0x8);
    _disable(); _dos_setvect(0x8, timerhandler); _enable();
}

void uninittimer(void)
{
    outp(0x43,54); outp(0x40,255); outp(0x40,255);           //18.2 times/sec
    _disable(); _dos_setvect(0x8, oldtimerhandler); _enable();
}

void __interrupt __far timerhandler()
{
    totalclock++;
    outp(0x20,0x20);
}

void initkeys(void)
{
    long i;

    keyfifoplc = 0; keyfifoend = 0;
    for(i=0;i<256;i++) keystatus[i] = 0;
    oldkeyhandler = _dos_getvect(0x9);
    _disable(); _dos_setvect(0x9, keyhandler); _enable();
}

void uninitkeys(void)
{
    short *ptr;

    _dos_setvect(0x9, oldkeyhandler);
        //Turn off shifts to prevent stucks with quitting
    ptr = (short *)0x417; *ptr &= ~0x030f;
}

void __interrupt __far keyhandler(void)
{
    koutp(0x20,0x20);
    oldreadch = readch; readch = kinp(0x60);
    keytemp = kinp(0x61); koutp(0x61,keytemp|128); koutp(0x61,keytemp&127);
    if ((readch|1) == 0xe1) { extended = 128; return; }
    if (oldreadch != readch)
    {
        if ((readch&128) == 0)
        {
            keytemp = readch+extended;
            if (keystatus[keytemp] == 0)
            {
                keystatus[keytemp] = 1;
                keyfifo[keyfifoend] = keytemp;
                keyfifo[(keyfifoend+1)&(KEYFIFOSIZ-1)] = 1;
                keyfifoend = ((keyfifoend+2)&(KEYFIFOSIZ-1));
            }
        }
        else
        {
            keytemp = (readch&127)+extended;
            keystatus[keytemp] = 0;
            keyfifo[keyfifoend] = keytemp;
            keyfifo[(keyfifoend+1)&(KEYFIFOSIZ-1)] = 0;
            keyfifoend = ((keyfifoend+2)&(KEYFIFOSIZ-1));
        }
    }
    extended = 0;
}

