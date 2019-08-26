//#define NUMOPTIONS 8
#define NUMOPTIONS 12
//#define NUMKEYS 19
#define NUMKEYS 27                                               // Les 07/24/95
#define XDIM    320
#define YDIM    200

#define KEYFIFOSIZ 64
extern void (__interrupt __far *oldkeyhandler)();
extern void __interrupt __far keyhandler(void);

#ifdef WHAVEN
volatile char keystatus[256], keyfifo[KEYFIFOSIZ], keyfifoplc, keyfifoend;
volatile char readch, oldreadch, extended, keytemp;
#else
extern volatile char keystatus[], keyfifo[], keyfifoplc, keyfifoend;
extern volatile char readch, oldreadch, extended, keytemp;
#endif

#ifdef WHAVEN
point3d osprite[MAXSPRITESONSCREEN];
long   smoothratio;
volatile char keystatus[256];
int shieldpoints=0;
int poisoned=0;
int poisontime=0;
int  shockme=-1;
int  cyberenabled=0;
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
#else
extern point3d osprite[];
extern long   smoothratio;
extern volatile char keystatus[];
extern int shieldpoints;
extern int poisoned;
extern int poisontime;
extern int  shockme;
extern int  cyberenabled;
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
#endif

#ifdef SVGA
long synctics;
long globhiz, globloz, globhihit, globlohit;
long chainxres[4] = {256,320,360,400};
long chainyres[11] = {200,240,256,270,300,350,360,400,480,512,540};
long vesares[7][2] = {320,200,640,400,640,480,800,600,1024,768,
                                      1280,1024,1600,1200};
char option[NUMOPTIONS] = {0,0,0,0,0,0,1,0};

#if 0                                                            // Les 07/24/95
char keys[NUMKEYS] =
{
    0xc8,0xd0,0xcb,0xcd,0x2a,0x9d,0x1d,0x39,
    0x1e,0x2c,0xd1,0xc9,0x33,0x34,
    0x9c,0x1c,0xd,0xc,0xf,
};
#endif                                                           // Les 07/24/95

//** Les START - 07/24/95
char keys[]={
     0xC8,0xD0,0xCB,0xCD,0x2A,0x9D,0x1D,0x39,
     0x2D,0x2E,0xC9,0xD1,0xC7,0x33,0x34,
     0x0F,0x1C,0x0D,0x0C,0x9C,0x13,0x16,
     0x23,0x1E,0x14,0x1F,0x35
};
//** Les  END  - 07/24/95

//** Les START - 07/27/95
extern
char option2[];
//** Les END   - 07/27/95

#else
extern long synctics;
extern long globhiz, globloz, globhihit, globlohit;
extern long chainxres[];
extern long chainyres[];
extern long vesares[7][2];
extern char option[];
extern char keys[];
#endif
