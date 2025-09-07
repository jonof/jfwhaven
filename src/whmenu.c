/*********************************************************************
*
*   WHMENU.C - menu code for Witchaven game
*
*********************************************************************/

#include "icorp.h"


int loopinstuff; //here it is
extern int vampiretime;

//JSA 4_27 cart and elevator sound variables
extern int  cartsnd,gratesnd,lavasnd,batsnd;

extern int gameactivated;
extern int escapetomenu;

extern int difficulty;
extern int mapon;

extern int currweapon;
extern int selectedgun;
extern int currentpotion;
extern int helmettime;
extern int shadowtime;
extern int nightglowtime;
extern int strongtime;
extern int invisibletime;
extern int manatime;
extern int thunderflash;
extern int thundertime;
extern int currentorb;
extern int currweaponfired;
extern int currweaponanim;
extern int currweaponattackstyle;
extern int currweaponflip;

int goreon=1;
int loadedgame=0;
int loadgo=0;

char typemessage[162];
int typemessageleng = 0, typemode = 0;

char scantoasc[128] = {
    0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,
    'q','w','e','r','t','y','u','i','o','p','[',']',0,0,'a','s',
    'd','f','g','h','j','k','l',';',39,'`',0,92,'z','x','c','v',
    'b','n','m',',','.','/',0,'*',0,32,0,0,0,0,0,0,
    0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1',
    '2','3','0','.',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

char scantoascwithshift[128] = {
    0,0,'!','@','#','$','%','^','&','*','(',')','_','+',0,0,
    'Q','W','E','R','T','Y','U','I','O','P','{','}',0,0,'A','S',
    'D','F','G','H','J','K','L',':',34,'~',0,'|','Z','X','C','V',
    'B','N','M','<','>','?',0,'*',0,32,0,0,0,0,0,0,
    0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1',
    '2','3','0','.',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};


extern char boardname[];
extern char loadgamename[];

extern char tempbuf[];
extern int frames;
#define MAXSAVEDGAMES 5

struct savedgame {
     char name[20];
};

struct savedgame savedgamenames[MAXSAVEDGAMES];


static void menubackground(void) {
    if (svga)
        svgafullscreenpic(SVGAMENU, SVGAMENU2);

    rotatesprite(svgaxoff,0<<15,svgascale,0,MAINMENU,0,
                0,svgastat,0,0,xdim-1,ydim-1);
}

static void menuwritesprite(int thex, int they, short tilenum, signed char shade,
        char stat, unsigned char dapalnum) {
    (void)stat;
    rotatesprite(svgaxoff+thex*svgascale,they*svgascale,svgascale,0,tilenum,shade,dapalnum,
                 svgaoverstat,0,0,xdim-1,ydim-1);
}


//
// fancy font
//
// to use this function you will need to name the starting letter
// the function will then scan the string and display its chars

char fancy[41] = {  'a','b','c','d','e',
                    'f','g','h','i','j',
                    'k','l','m','n','o',
                    'p','q','r','s','t',
                    'u','v','w','x','y',
                    'z','0','1','2','3',
                    '4','5','6','7','8',
                    '9','!','?','-',':',' ' };


void fancyfont(int x, int y, short tilenum, char *string, char pal) {

     int i, j;
     int len;
     int incr=0;
     int number;
     char temp[40];

     Bstrlwr(string);
     len=(int)strlen(string);
     strcpy(temp,string);

     for(i=0;i<len;i++) {
        tempbuf[i]=temp[i];
        for(j=0;j<40;j++) {
            if(tempbuf[i]==fancy[j]) {
                number=j;
            }
        }
        if( i == 0 ) {
            menuwritesprite(x,y,tilenum+number,0,0,pal);
            incr+=tilesizx[tilenum+number]+1;
        }
        else if( tempbuf[i] != ' ' ) {
            menuwritesprite(x+incr,y,tilenum+number,0,0,pal);
            incr+=tilesizx[tilenum+number]+1;
        }
        else
            incr+=8;
     }

}



void fancyfontscreen(int x, int y, short tilenum, char *string, char pal) {

     int i, j;
     int len;
     int incr=0;
     int number;
     char temp[40];

     Bstrlwr(string);
     len=(int)strlen(string);
     strcpy(temp,string);

     for(i=0;i<len;i++) {
        tempbuf[i]=temp[i];
        for(j=0;j<40;j++) {
            if(tempbuf[i]==fancy[j]) {
                number=j;
            }
        }
        if( i == 0 ) {
            rotatesprite(x<<16,y<<16,65536L,0,tilenum+number,0,pal,
                         2+8+16,0,0,xdim-1,ydim-1);
            incr+=tilesizx[tilenum+number]+1;
        }
        else if( tempbuf[i] != ' ' ) {
            rotatesprite((x+incr)<<16,y<<16,65536L,0,tilenum+number,0,pal,
                         2+8+16,0,0,xdim-1,ydim-1);
            incr+=tilesizx[tilenum+number]+1;
        }
        else
            incr+=8;
     }
}

void fancyfontperm(int x, int y, short tilenum, char *string, char pal) {

     int i, j;
     int len;
     int incr=0;
     int number;
     char temp[40];

     Bstrlwr(string);
     len=(int)strlen(string);
     strcpy(temp,string);

     for(i=0;i<len;i++) {
        tempbuf[i]=temp[i];
        for(j=0;j<40;j++) {
            if(tempbuf[i]==fancy[j]) {
                number=j;
            }
        }
        if( i == 0 ) {
            rotatesprite(x<<16,y<<16,65536L,0,tilenum+number,0,pal,
                         2+8+16+128,0,0,xdim-1,ydim-1);
            incr+=tilesizx[tilenum+number]+1;
        }
        else if( tempbuf[i] != ' ' ) {
            rotatesprite((x+incr)<<16,y<<16,65536L,0,tilenum+number,0,pal,
                         2+8+16+128,0,0,xdim-1,ydim-1);
            incr+=tilesizx[tilenum+number]+1;
        }
        else
            incr+=8;
     }
}

void svgafullscreenpic(short pic1, short pic2) {
    rotatesprite(svgaxoff,0<<15,svgascale,0,pic1,0,
                0,8+16+64,0,0,xdim-1,ydim-1);
    rotatesprite(svgaxoff,ydim<<15,svgascale,0,pic2,0,
                0,8+16+64,0,0,xdim-1,ydim-1);
}

int menuscreen(struct player *plr) {

    struct {
        int x;
        int y;
    }redpic[5] = {
                        { 142,58 },
                        { 140,80 },
                        { 127,104 },
                        { 184,126 },
                        { 183,150 } };

    int exit=0;
    int select=0;
    short redpicnum;
    int goaltime;


    if (netgame) {
        return(0);
//        netkillme();
    }

//JSA BLORB
    SND_CheckLoops();

    redpicnum=NEWGAMEGREEN;

    keystatus[1]=keystatus[0x1c]=keystatus[0x9c]=0;

    //goaltime=totalclock-20L;
    goaltime=totalclock+10L;

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

    flushperms();

    while( !exit ) {
        handleevents();
        menubackground();
        menuwritesprite(127,58,MENUSELECTIONS,0,0,0);

        if( select < 5) {
            redpicnum=NEWGAMEGREEN+select;
            menuwritesprite(redpic[select].x,redpic[select].y,redpicnum,0,0,0);
        }

        nextpage();

            if( totalclock >= goaltime ) {
                if( keystatus[LDN] || keystatus[RDN] ) {
                    TEMPSND();
                    select++;
                    if ( select > 4 )
                        select=0;
                    goaltime=totalclock+15L;
                }
                if( keystatus[LUP] || keystatus[RUP] ) {
                    TEMPSND();
                    select--;
                    if ( select < 0 )
                        select=4;
                    goaltime=totalclock+15L;
                }
                if( keystatus[1] > 0) {
                    TEMPSND();
                    if(gameactivated == 1) {
                        //select=5;
                        lockclock=totalclock;
                        exit=1;
                    }
                    else
                        select=4;
                    keystatus[1]=0;
                }
                if( keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                    keystatus[0x1c]=keystatus[0x9c]=0;keystatus[1]=0;
                    switch(select) {
                    case 0:                 // ok
                        gameactivated=0;
                        //JSA BLORB
                        SND_Sting(S_STING1);
                        SND_FadeMusic();
                        srand(totalclock&30000);
                        //tille maps are finished
                        //if(loadedgame == 0 && netgame == 0)
                        //   mapon=1;
#if 0
                        if (netgame) {
                            netpickmonster();
                            exit=1;
                            break;
                        }
#endif
                        startnewgame(plr);
                        gameactivated=1;
                        exit=1;
                        keystatus[0x1c]=keystatus[0x9c]=0;keystatus[1]=0;
                    break;
                    case 1:
                        TEMPSND();
                        loadsave(plr);
                        if(loadgo == 1)
                            exit=1;
                            loadgo=0;
                        keystatus[0x1c]=keystatus[0x9c]=0;keystatus[1]=0;
                    break;
                    case 2:
                        TEMPSND();
                        thedifficulty();
                        keystatus[0x1c]=keystatus[0x9c]=0;keystatus[1]=0;
                    break;
                    case 3:
                        TEMPSND();
                        help();
                    break;
                    case 4:
                        TEMPSND();
                        quit();
                    break;
                    case 5:
                        lockclock=totalclock;
                        exit=1;
                    break;
                    } // switch
                } // if
            } // the delay
      } // while

    escapetomenu=0;

//      if (netgame) {
//          netreviveme();
//      }

return(0);
}

//JUNE8
#define  MAXHELPSCREENS   10

void help(void) {

    struct {
        int helpnames;
    }thenames[MAXHELPSCREENS] = {
        { WEAPONS },
        { SPELLS },
        { POTIONS },
        { WALKING },
        { FLYING },
        { CREDIT1 },
        { CREDIT2 },
        { CREDIT3 },
        { CREDIT4 },
        { BETAPAGE }
    };

    int select=0;
    int goaltime = totalclock+10;
    int exit=0;

    while (!exit) {
        handleevents();
        menubackground();
        menuwritesprite(0,0,thenames[select].helpnames,0,0,0);
        nextpage();

        if(totalclock >= goaltime) {
                if( keystatus[LDN]
                    || keystatus[keys[KEYRIGHT]]
                    || keystatus[RDN]
                    || keystatus[keys[KEYBACK]]
                    || keystatus[RRIGHT]) {
                    TEMPSND();
                    select++;
                    if ( select > 9 )
                        select=9;
                    goaltime=totalclock+15L;
                }
                if( keystatus[LUP]
                    || keystatus[keys[KEYLEFT]]
                    || keystatus[RUP]
                    || keystatus[keys[KEYFWD]]
                    || keystatus[RLEFT]) {
                    TEMPSND();
                    select--;
                    if ( select < 0 )
                        select=0;
                    goaltime=totalclock+15L;
                }
                if( keystatus[1] > 0) {
                    exit=1;
                    keystatus[1]=0;
                }
                if( keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                    exit=2;
                    keystatus[0x1c]=keystatus[0x9c]=0;
                }
        }
    }
    keystatus[1]=keystatus[0x1c]=keystatus[0x9c]=0;

}

//JUNE6
void loadsave(struct player *plr) {

    int exit=0;
    int select=0;
    int goaltime;

    goaltime=totalclock+10L;

    while ( !exit ) {
        handleevents();
        menubackground();
        if(select == 0) {
            menuwritesprite(182,80,LOADGREEN,0,0,0);
            menuwritesprite(182,119,SAVERED,0,0,0);
        }
        else {
            menuwritesprite(182,80,LOADRED,0,0,0);
            menuwritesprite(182,119,SAVEGREEN,0,0,0);
        }
        nextpage();

        if( totalclock >= goaltime ) {
                if( keystatus[LDN] || keystatus[RDN] ) {
                    TEMPSND();
                    select=1;
                    goaltime=totalclock+15L;
                }
                if( keystatus[LUP] || keystatus[RUP] ) {
                    TEMPSND();
                    select=0;
                    goaltime=totalclock+15L;
                }
                if( keystatus[1] > 0) {
                    exit=1;
                    keystatus[1]=0;
                }
                if( keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                    exit=2;
                    keystatus[0x1c]=keystatus[0x9c]=0;
                }
            }
    }

    keystatus[1]=0;

    if(exit == 2)
        switch(select){
        case 0:                 // ok
            loadgame(plr);
        break;
        case 1:                 // ok
            if( gameactivated == 1)
                savegame(plr);
        break;
        }

}

//JUNE6
void quit(void) {

    int exit=0;

    while( !exit ) {
        handleevents();
        menubackground();
        menuwritesprite(123,79,AREYOUSURE,0,0,0);
        nextpage();

        if ( keystatus[0x9c] > 0 || keystatus[0x1c] > 0 || keystatus[0x15] > 0 ) {
            exit=1;
            keystatus[0x1c]=keystatus[0x9c]=0;
        }
        if ( keystatus[1] > 0 || keystatus[0x31] > 0 ) {
            exit=2;
            keystatus[1]=0;
        }
    }

    if ( exit==2 ) {
        keystatus[1]=0;
    }
    else {
        if(svga) {
            exit=0;
            while( !exit ){
                handleevents();
                svgafullscreenpic(SVGAORDER1, SVGAORDER2);
                nextpage();

                if(keystatus[0x39] > 0 || keystatus[1] > 0)
                    exit=1;
            }
            keystatus[0x39]=0;
            keystatus[1]=0;
        }
        else {
            keystatus[0x39]=0;
            keystatus[1]=0;
            exit=0;
            while( !exit ){
                handleevents();
                menuwritesprite(0,0,ORDER1,0,0,0);
                nextpage();

                if(keystatus[0x39] > 0 || keystatus[1] > 0)
                    exit=1;
            }
            keystatus[0x39]=0;
            keystatus[1]=0;
            exit=0;
            /*
            while( !exit ){
                if(keystatus[0x39] > 0 || keystatus[1] > 0)
                    exit=1;
                overwritesprite(0,0,ORDER2,0,0,0);
                nextpage();
            }
            keystatus[0x39]=0;
            keystatus[1]=0;
            exit=0;
            */
        }
        shutdowngame();
    }

}


void thedifficulty(void) {

        struct {
            int x;
            int y;
        }redpic[4]={
                        { 148,146 },
                        { 181,146 },
                        { 215,144 },
                        { 257,143 } };

        int exit=0;
        int selected;
        int select;
        int select2=0;
        int select3=goreon;
        int redpicnum;
        int pickone=0;
        int goaltime;

        select=difficulty-1;
        keystatus[1]=0;
        keystatus[0x1c]=0;
        keystatus[0x9c]=0;

        //loadtile(MAINMENU);

        goaltime=totalclock+10L;

        while ( !exit ) {
            handleevents();

        if( totalclock >= goaltime ) {

            if(keystatus[LUP] > 0 || keystatus[RUP] > 0) {
                TEMPSND();
                select2=0;
                goaltime=totalclock+15L;
            }
            if(keystatus[LDN] > 0 || keystatus[RDN] > 0) {
                TEMPSND();
                select2=1;
                goaltime=totalclock+15L;
            }
            if(select2 == 0)
                pickone=0;
            else
                pickone=1;

            redpicnum=HORNYSKULL1+select;

            menubackground();

            if(select2 == 0) {
                menuwritesprite(148,114,DIFFICULTRED,0,0,0);
                menuwritesprite(127,58,BLOODGOREGREEN,0,0,0);
            }
            else {
                menuwritesprite(148,114,DIFFICULTGREEN,0,0,0);
                menuwritesprite(127,58,BLOODGORERED,0,0,0);
            }
            menuwritesprite(147,143,HORNYBACK,0,0,0);
            menuwritesprite(redpic[select].x,redpic[select].y,redpicnum,0,0,0);

            if( goreon == 1 ) {
                menuwritesprite(180,84,NOGORESHADOW,0,0x04,0);
                menuwritesprite(214,81,GORESOLID,0,0,0);
            }
            else {
                menuwritesprite(180,84,NOGORESOLID,0,0,0);
                menuwritesprite(214,81,GORESHADOW,0,0x04,0);
            }
            nextpage();

            if( pickone == 1 ) {
                if( keystatus[LLEFT] > 0 || keystatus[RLEFT] > 0) {
                    TEMPSND();
                    select--;
                    if( select < 0)
                        select=0;
                    goaltime=totalclock+15L;
                }
                if( keystatus[LRIGHT] > 0 || keystatus[RRIGHT] > 0) {
                    TEMPSND();
                    select++;
                    if( select > 3)
                        select=3;
                    goaltime=totalclock+15L;
                }
                selected=select;
            }
            else {
                if( keystatus[LLEFT] > 0 || keystatus[RLEFT] > 0) {
                    TEMPSND();
                    select3=0;
                    goaltime=totalclock+15L;
                }
                if( keystatus[LRIGHT] > 0 || keystatus[RRIGHT] > 0) {
                    TEMPSND();
                    select3=1;
                    goaltime=totalclock+15L;
                }
                if(select3 == 0)
                    goreon=0;
                else
                    goreon=1;
            }
            if( keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                exit=1;
                keystatus[0x1c]=keystatus[0x9c]=0;
            }
            if( keystatus[1] > 0 ) {
                exit=2;
                keystatus[1]=0;
            }
        } // IF

        } // WHILE

        if ( exit == 1 ) {
            switch(selected) {
            case 0:
            difficulty=1;
            break;
            case 1:
            difficulty=2;
            break;
            case 2:
            difficulty=3;
            break;
            case 3:
            difficulty=4;
            break;
            }
        }


}

void startnewgame(struct player *plr) {

    char tempshow2dsector[MAXSECTORS>>3];
    char tempshow2dwall[MAXWALLS>>3];
    char tempshow2dsprite[MAXSPRITES>>3];
    int i;

    if (netgame) {
        SND_StartMusic(mapon-1);
        goto skip;
    }
    if(loadedgame == 0) {
        sprintf(boardname,"level%d.map",mapon);
        setupboard(boardname);
        initplayersprite();
        cleanup();
        SND_StartMusic(mapon-1);
    }
    else if(loadedgame == 1) {
        loadplayerstuff();
            for(i=0;i<(MAXSECTORS>>3);i++) tempshow2dsector[i]=show2dsector[i];
            for(i=0;i<(MAXWALLS>>3);i++) tempshow2dwall[i]=show2dwall[i];
            for(i=0;i<(MAXSPRITES>>3);i++) tempshow2dsprite[i]=show2dsprite[i];
        setupboard(boardname);
            for(i=0;i<(MAXSECTORS>>3);i++) show2dsector[i]=tempshow2dsector[i];
            for(i=0;i<(MAXWALLS>>3);i++) show2dwall[i]=tempshow2dwall[i];
            for(i=0;i<(MAXSPRITES>>3);i++) show2dsprite[i]=tempshow2dsprite[i];
        loadedgame=0;
        SND_StartMusic(mapon-1);
    }
skip:
    if (plr->screensize < 320)
        permanentwritesprite(0,0,BACKGROUND,0,0,0,319,199,0);

    if (plr->screensize <= 320) {
        permanentwritesprite(0,200-46,NEWSTATUSBAR,0,0,0,319,199,0);
    }
    updatepics();

}

void loadgame(struct player *plr) {

    int select=0;
    int exit=0;
    int i;
    int goaltime;

    goaltime=totalclock+10L;

    for(i=0;i<MAXSAVEDGAMES;i++)
        if( !savedgamedat(i))
            strcpy(savedgamenames[i].name,"empty");

    while ( !exit ) {
        handleevents();

        menubackground();
        menuwritesprite(138,84,SAVENUMBERS,0,0,0);
        menuwritesprite(190,48,LOADPIC,0,0,0);

        for(i=0;i<MAXSAVEDGAMES;i++) {
            if( i == select )
                fancyfont(154,81+(i*17),THEFONT,savedgamenames[select].name,7);
            else
                fancyfont(154,81+(i*17),THEFONT,savedgamenames[i].name,0);
        }

        nextpage();

        if( totalclock >= goaltime ) {
            if ( keystatus[LUP] > 0 || keystatus[RUP] > 0) {
                select--;
                if( select < 0 )
                    select=4;
                goaltime=totalclock+15L;
            }
            if ( keystatus[LDN] > 0 || keystatus[RDN] > 0) {
                select++;
                if( select > 4 )
                    select=0;
                goaltime=totalclock+15L;
            }

            if ( keystatus[1] > 0 ) {
                exit=1;
                keystatus[1]=0;
            }

            if ( keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                keystatus[0x1c]=keystatus[0x9c]=0;
                if( strcmp(savedgamenames[select].name,"empty") == 0 )
                    exit=0;
                else {
                    exit=2;
                    loadgo=1;
                }
            }
        }
    }

    if ( exit == 2 ) {
        keystatus[1]=keystatus[0x1c]=keystatus[0x9c]=0;
        sprintf(boardname,"svgm%d.map",select);
        sprintf(loadgamename,"svgn%d.dat",select);
        loadedgame=1;
        gameactivated=1;
        startnewgame(plr);
    }


}

void savegame(struct player *plr) {

    int exit=0;
    int i;
    int select=0;
    int goaltime;

    (void)plr;

    for(i=0;i<MAXSAVEDGAMES;i++)
        if( !savedgamedat(i))
            strcpy(savedgamenames[i].name,"EMPTY");

    goaltime=totalclock+10L;

    while ( !exit ) {
        handleevents();

        menubackground();
        menuwritesprite(138,84,SAVENUMBERS,0,0,0);
        menuwritesprite(188,50,SAVEPIC,0,0,0);

        for( i=0;i<MAXSAVEDGAMES;i++) {
            if(i == select)
                fancyfont(154,81+(i*17),THEFONT,savedgamenames[select].name,7);
            else
                fancyfont(154,81+(i*17),THEFONT,savedgamenames[i].name,0);
        }

        nextpage();

        if( totalclock >= goaltime ) {
            if( keystatus[LUP] > 0 || keystatus[RUP] > 0) {
                select--;
                if ( select < 0)
                    select=4;
                goaltime=totalclock+15L;
            }

            if( keystatus[LDN] > 0 || keystatus[RDN] > 0) {
                select++;
                if( select > 4 )
                    select=0;
                goaltime=totalclock+15L;
            }

            if( keystatus[1] > 0 ) {
                exit=1;
                keystatus[1]=0;
            }

            if( keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                exit=2;
                typemessageleng = 0;
                keystatus[0x1c]=keystatus[0x9c]=0;
                savegametext(select);
            }
        }
    }

}


void savegametext(int select) {

    struct player *plr;

    int exit=0;
    int i, j;
    char tempbuf[40];
    char temp[40];

    int  typemessageleng=0;

    plr=&player[pyrn];

    keystatus[0x1c]=keystatus[0x9c]=0;

    for(i=0;i<128;i++)
        keystatus[i]=0;

    strcpy(temp,"-");

    while( !exit ) {
        handleevents();

        menubackground();
        menuwritesprite(138,84,SAVENUMBERS,0,0,0);
        menuwritesprite(188,50,SAVEPIC,0,0,0);

        for(i=0;i<MAXSAVEDGAMES;i++) {
            if( i == select ) {
                strcpy(tempbuf,temp);
                fancyfont(154,81+(i*17),THEFONT,tempbuf,7);
            }
            else {
                strcpy(tempbuf,savedgamenames[i].name);
                fancyfont(154,81+(i*17),THEFONT,tempbuf,0);
            }
        }
        nextpage();

        if (keystatus[0xe] > 0) {  // backspace
            if (typemessageleng > 0) {
                temp[typemessageleng]='\0';
                typemessageleng--;
                temp[typemessageleng]='\0';
            }
            else {
                strcpy(temp,"-");
                typemessageleng=0;
            }
            keystatus[0xe]=0;
        }
        if(typemessageleng < 10) {
            for(i=0;i<128;i++) {
                if(keystatus[i] > 0
                    && keystatus[0x0e] == 0    // keypressed not
                    && keystatus[1] == 0       //            esc
                    && keystatus[0x1c] == 0    //            enter l or r
                    && keystatus[0x9c] == 0) { //            backspace
                        for(j=0;j<41;j++) {
                            if(scantoasc[i] == ' ') {
                                continue;
                            }
                            else if(scantoasc[i] == fancy[j]) {
                                temp[typemessageleng]=fancy[j];
                                typemessageleng++;
                                temp[typemessageleng]='\0';
                                keystatus[i]=0;
                            }
                            else
                                keystatus[i]=0;
                        }
                }
            }
        }
        if( keystatus[1] > 0) {
            exit=1;
            keystatus[1]=0;
        }
        if ( keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
            if( typemessageleng > 0 ) {
                strcpy(savedgamenames[select].name,temp);
                sprintf(tempbuf,"svgm%d.map",select);
                saveboard(tempbuf,&plr->x,&plr->y,&plr->z,&plr->ang,&plr->sector);
                savedgamename(select);
            }
            else {
                sprintf(tempbuf,"svgm%d.map",select);
                saveboard(tempbuf,&plr->x,&plr->y,&plr->z,&plr->ang,&plr->sector);
                savedgamename(select);
            }
            exit=2;
            keystatus[0x1c]=keystatus[0x9c]=0;
        }
    }

    if( exit == 2 ) {
        keystatus[0x1c]=keystatus[0x9c]=0;
    }

}


int savedgamename(int gn) {

    struct player *plr;
    int  file;
    int  i;
    int tmpanimateptr[MAXANIMATES];

    plr=&player[0];

    plr->screensize=320;

    sprintf(tempbuf,"svgn%d.dat",gn);

    //if ((fil = open("save0000.gam"
    //,O_BINARY|O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE)) == -1)
    //    return(-1);


    //file = open(tempbuf,O_CREAT | O_BINARY | O_WRONLY,S_IREAD | S_IWRITE ); // | S_IFREG);

    file = open(tempbuf,O_BINARY|O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE );

    if (file != -1)
    {
        write(file,savedgamenames[gn].name,sizeof(struct savedgame));
        write(file,&player[0],sizeof(struct player));
        write(file,&currweapon,sizeof(currweapon));
        write(file,&selectedgun,sizeof(selectedgun));
        write(file,&currentpotion,sizeof(currentpotion));
        write(file,&helmettime,sizeof(helmettime));
        write(file,&shadowtime,sizeof(shadowtime));
        write(file,&nightglowtime,sizeof(nightglowtime));
        write(file,&visibility,sizeof(visibility));
        write(file,&strongtime,sizeof(strongtime));
        write(file,&invisibletime,sizeof(invisibletime));
        write(file,&manatime,sizeof(manatime));
        write(file,&thunderflash,sizeof(thunderflash));
        write(file,&thundertime,sizeof(thundertime));

        write(file,&currentorb,sizeof(currentorb));
        write(file,&currweaponfired,sizeof(currweaponfired));
        write(file,&currweaponanim,sizeof(currweaponanim));
        write(file,&currweaponattackstyle,sizeof(currweaponattackstyle));
        write(file,&currweaponflip,sizeof(currweapon));
        write(file,&mapon,sizeof(mapon));

        write(file,&totalclock,sizeof(totalclock));
        write(file,&lockclock,sizeof(lockclock));
        write(file,&synctics,sizeof(synctics));

        //Warning: only works if all pointers are in sector structures!
        for(i=MAXANIMATES-1;i>=0;i--)
            tmpanimateptr[i] = (int)((intptr_t)animateptr[i]-(intptr_t)sector);
        write(file,tmpanimateptr,MAXANIMATES<<2);

        write(file,animategoal,MAXANIMATES<<2);
        write(file,animatevel,MAXANIMATES<<2);
        write(file,&animatecnt,4);

        write(file,show2dsector,MAXSECTORS>>3);
        write(file,show2dwall,MAXWALLS>>3);
        write(file,show2dsprite,MAXSPRITES>>3);
        write(file,&automapping,1);
        write(file,&shieldpoints,2);
        write(file,&vampiretime,2);
        write(file,&poisoned,2);
        write(file,&poisontime,2);

        close(file);
        return 1;
    }
    else
        return 0;



}

int savedgamedat(int gn) {

    int     fh=0;
    char    fname[20];
    char    fsname[20];
    ssize_t nr=0;

    sprintf(fname,"svgm%d.map",gn);

    sprintf(fsname,"svgn%d.dat",gn);

    if( access(fname, F_OK)!=0 )
        return 0;

    if( access(fsname, F_OK)!=0 )
        return 0;

    fh = open(fsname, O_RDONLY | O_BINARY);

    if( fh<0 )
        return 0;

    nr=read(fh, savedgamenames[gn].name, sizeof(struct savedgame));

    close(fh);

    if( nr != sizeof(struct savedgame) )
        return 0;

    return 1;

}

void loadplayerstuff(void) {

    int fh;
    int i;
    int tmpanimateptr[MAXANIMATES];

    fh=open(loadgamename, O_RDONLY | O_BINARY);

    read(fh, savedgamenames[0].name, sizeof(struct savedgame));
    read(fh, &player[0],sizeof(struct player));
    read(fh,&currweapon,sizeof(currweapon));
    read(fh,&selectedgun,sizeof(selectedgun));
    read(fh,&currentpotion,sizeof(currentpotion));
    read(fh,&helmettime,sizeof(helmettime));
    read(fh,&shadowtime,sizeof(shadowtime));
    read(fh,&nightglowtime,sizeof(nightglowtime));
    read(fh,&visibility,sizeof(visibility));
    read(fh,&strongtime,sizeof(strongtime));
    read(fh,&invisibletime,sizeof(invisibletime));
    read(fh,&manatime,sizeof(manatime));
    read(fh,&thunderflash,sizeof(thunderflash));
    read(fh,&thundertime,sizeof(thundertime));

    read(fh,&currentorb,sizeof(currentorb));
    read(fh,&currweaponfired,sizeof(currweaponfired));
    read(fh,&currweaponanim,sizeof(currweaponanim));
    read(fh,&currweaponattackstyle,sizeof(currweaponattackstyle));
    read(fh,&currweaponflip,sizeof(currweaponflip));
    read(fh,&mapon,sizeof(mapon));

    read(fh,&totalclock,sizeof(totalclock));
    read(fh,&lockclock,sizeof(lockclock));
    read(fh,&synctics,sizeof(synctics));

        //Warning: only works if all pointers are in sector structures!
    read(fh,tmpanimateptr,MAXANIMATES<<2);

    for(i=MAXANIMATES-1;i>=0;i--)
        animateptr[i] = (int *)(tmpanimateptr[i]+(intptr_t)sector);

    read(fh,animategoal,MAXANIMATES<<2);
    read(fh,animatevel,MAXANIMATES<<2);
    read(fh,&animatecnt,4);

    read(fh,show2dsector,MAXSECTORS>>3);
    read(fh,show2dwall,MAXWALLS>>3);
    read(fh,show2dsprite,MAXSPRITES>>3);
    read(fh,&automapping,1);
    read(fh,&shieldpoints,2);
    read(fh,&vampiretime,2);
    read(fh,&poisoned,2);
    read(fh,&poisontime,2);

    close(fh);

}

/***************************************************************************
 *   screen effects for TEKWAR follow...fades, palette stuff, etc..        *
 *                                                                         *
 *                                                     12/15/94 Jeff S.    *
 ***************************************************************************/



#define   MESSIAHRANDOMKEY    215
#define   LEDRANDOMKEY        10

char      forcemessiah=0;
int      messcnt;
int      ledcnt;
char      ledflash;
char      tremors;
char      dofadein=0;

#define   DEADTIME       2   // # minutes of nothing happening
#define   FLASHINTERVAL  20

int      passlock,lastastep,lastbstep,astep,bstep;

extern char pee;
extern char flashflag;

void screenfx(struct player *plr) {
    (void)plr;
      updatepaletteshifts();

 return;
}


#define   MAXSTRLEN      30

//
// (extern asm template)
//
// extern int near asm_main (int *parmeter1, int parameter2);
// Extern must be EXACT prototype.
//
//  #pragma aux asm_main "*_"             /* Define the auxilary function. */ \
//    parm caller     [eax] [ebx]         /* Calling parameters.           */ \
//    value           [eax]               /* Return value.                 */ \
//    modify          [ecx]; // Non parameter registers that will be altered.

void
clearpal(void)
{
    debugprintf("clearpal()\n");
}


char  palette1[256][3],palette2[256][3];

void
getpalette(char *palette)
{
    (void)palette;
    debugprintf("getpalette()\n");
}

void
fillpalette(int red, int green, int blue)
{
    (void)red; (void)green; (void)blue;
    debugprintf("fillpalette()\n");
}


char      foggy=0;

void
fadeout(int start, int end, int red, int green, int blue, int steps)
{
    int      i,j,orig,delta;
    char      *origptr,*newptr;

    getpalette(&palette1[0][0]);
    memcpy(palette2,palette1,768);

    for( i=0; i<steps; i++ ) {
        origptr=&palette1[start][0];
        newptr=&palette2[start][0];
        for( j=start; j<=end; j++ ) {
            orig = *origptr++;
            delta = red-orig;
            *newptr++ = orig + delta * i / steps;
            orig = *origptr++;
            delta = green-orig;
            *newptr++ = orig + delta * i / steps;
            orig = *origptr++;
            delta = blue-orig;
            *newptr++ = orig + delta * i / steps;
        }

        //asmsetpalette(&palette2[0][0]);
    }

      if( foggy == 0 )
        fillpalette(red,green,blue);

 return;
}


void
fadein(int start, int end, int steps)
{
    int  i,j,delta;

      if( steps == 0 ) {
             return;
      }

    getpalette(&palette1[0][0]);
    memcpy(&palette2[0][0],&palette1[0][0],sizeof(palette1));

    start *= 3;
    end = end*3+2;

      // fade through intermediate frames
    for( i=0; i<steps; i++ ) {
        for( j=start; j<=end; j++ ) {
            delta=palette[j]-palette1[0][j];
            palette2[0][j]=palette1[0][j] + delta * i / steps;
        }

        //asmsetpalette(&palette2[0][0]);
    }

      // final color
    //asmsetpalette(palette);

      dofadein=0;
      //clearkeys();
}


void
fog1(void)
{
      if( foggy == 0 ) {
             foggy=1;
             fadeout(1,254,8,8,10,2);
      }
      else {
             foggy=0;
             fadein(0,255,2);
      }
}


void
fog2(void)
{
      unsigned char *lookptr;

      lookptr=palookup[0];
      palookup[0]=palookup[1];
      palookup[1]=lookptr;
}

void
makefxlookups(void)
{
      unsigned char palbuf[256];
      short     i;

      for( i=0; i<256; i++ )
             palbuf[i]=*(palookup[0]+i);

      makepalookup(1, palbuf, 60,60,60,1);
}



#define   NUMWHITESHIFTS      3
#define   WHITESTEPS          20
#define   WHITETICS           6

#define   NUMREDSHIFTS        4
#define   REDSTEPS            8

#define   NUMGREENSHIFTS      4
#define   GREENSTEPS          8

#define   NUMBLUESHIFTS       4
#define   BLUESTEPS           8

unsigned char whiteshifts[NUMREDSHIFTS][768];
unsigned char redshifts[NUMREDSHIFTS][768];
unsigned char greenshifts[NUMGREENSHIFTS][768];
unsigned char blueshifts[NUMBLUESHIFTS][768];

int       redcount,whitecount,greencount,bluecount;
char      palshifted;


void
initpaletteshifts(void)
{
    unsigned char *workptr,*baseptr;
    int       i,j,delta;

    for( i=1; i<=NUMREDSHIFTS; i++ ) {
        workptr=(unsigned char *)&redshifts[i-1][0];
        baseptr=&palette[0];
        for( j=0; j<=255; j++) {
            delta=64-*baseptr;
            *workptr++=*baseptr++ + delta * i / REDSTEPS;
            delta=-*baseptr;
            *workptr++=*baseptr++ + delta * i / REDSTEPS;
            delta=-*baseptr;
            *workptr++=*baseptr++ + delta * i / REDSTEPS;
        }
    }

    for( i=1; i<=NUMWHITESHIFTS; i++ ) {
        workptr=&whiteshifts[i-1][0];
        baseptr=&palette[0];
        for( j=0; j<=255; j++ ) {
            delta = 64-*baseptr;
            *workptr++ = *baseptr++ + delta * i / WHITESTEPS;
            delta = 62-*baseptr;
            *workptr++ = *baseptr++ + delta * i / WHITESTEPS;
            delta = 0-*baseptr;
            *workptr++ = *baseptr++ + delta * i / WHITESTEPS;
        }
    }

    for( i=1; i<=NUMGREENSHIFTS; i++) {
        workptr=&greenshifts[i-1][0];
        baseptr=&palette[0];
        for( j=0; j<=255; j++) {
            delta=-*baseptr;
            *workptr++=*baseptr++ + delta * i / GREENSTEPS;
            delta=64-*baseptr;
            *workptr++=*baseptr++ + delta * i / GREENSTEPS;
            delta=-*baseptr;
            *workptr++=*baseptr++ + delta * i / GREENSTEPS;
        }
    }

    for( i=1; i<=NUMBLUESHIFTS; i++) {
        workptr=&blueshifts[i-1][0];
        baseptr=&palette[0];
        for( j=0; j<=255; j++) {
            delta=-*baseptr;
            *workptr++=*baseptr++ + delta * i / BLUESTEPS;
            delta=-*baseptr;
            *workptr++=*baseptr++ + delta * i / BLUESTEPS;
            delta=64-*baseptr;
            *workptr++=*baseptr++ + delta * i / BLUESTEPS;
        }
    }


 return;
}

void
startgreenflash(int greentime)
{
    greencount=0;

    greencount+=greentime;

    if( greencount < 0 ) {
        greencount=0;
    }

}

void
startblueflash(int bluetime)
{
    bluecount=0;

    bluecount+=bluetime;

    if( bluecount < 0 ) {
        bluecount=0;
    }

}


void
startredflash(int damage)
{

      redcount=0;

      redcount+=damage;

      if( redcount < 0 ) {
             redcount=0;
      }
}

void
startwhiteflash(int bonus)
{
      whitecount = 0;

      whitecount+=bonus;

      if( whitecount < 0 ) {
             whitecount=0;
      }

}


void
updatepaletteshifts(void)
{
      int   red,white,green,blue;

    if (whitecount)
    {
        white = whitecount/WHITETICS +1;
        if (white>NUMWHITESHIFTS)
            white = NUMWHITESHIFTS;
        whitecount -= synctics;
        if (whitecount < 0)
            whitecount = 0;
    }
    else {
        white = 0;
      }

    if (redcount)
    {
        red = redcount/10 +1;
        if (red>NUMREDSHIFTS)
            red = NUMREDSHIFTS;
        redcount -= synctics;
        if (redcount < 0)
            redcount = 0;
    }
    else {
        red = 0;
      }

    if (greencount)
    {
        green = greencount/10 +1;
        if (green>NUMGREENSHIFTS)
            green = NUMGREENSHIFTS;
        greencount -= synctics;
        if (greencount < 0)
            greencount = 0;
    }
    else {
        green = 0;
      }

    if (bluecount)
    {
        blue = bluecount/10 +1;
        if (blue>NUMBLUESHIFTS)
            blue = NUMBLUESHIFTS;
        bluecount -= synctics;
        if (bluecount < 0)
            bluecount = 0;
    }
    else {
        blue = 0;
      }

    if( red ) {
#if USE_POLYMOST && USE_OPENGL
        if (getrendermode() >= 3) setpalettefade(64,0,0,64 * red / REDSTEPS);
        else
#endif
        setbrightness(gbrightness,&redshifts[red-1][0],0);
        palshifted = 1;
    }
    else if( white ) {
#if USE_POLYMOST && USE_OPENGL
        if (getrendermode() >= 3) setpalettefade(64,62,0,64 * white / WHITESTEPS);
        else
#endif
        setbrightness(gbrightness,&whiteshifts[white-1][0],0);
        palshifted = 1;
    }
    else if( green ) {
#if USE_POLYMOST && USE_OPENGL
        if (getrendermode() >= 3) setpalettefade(0,64,0,64 * green / GREENSTEPS);
        else
#endif
        setbrightness(gbrightness,&greenshifts[green-1][0],0);
        palshifted=1;
    }
    else if( blue ) {
#if USE_POLYMOST && USE_OPENGL
        if (getrendermode() >= 3) setpalettefade(0,0,64,64 * blue / BLUESTEPS);
        else
#endif
        setbrightness(gbrightness,&blueshifts[blue-1][0],0);
        palshifted=1;
    }

    else if( palshifted ) {
#if USE_POLYMOST && USE_OPENGL
        if (getrendermode() >= 3) setpalettefade(0,0,0,0);
        else
#endif
        setbrightness(gbrightness,palette,0);  // back to normal
        palshifted = 0;
    }

 return;
}


void
finishpaletteshifts(void)
{
    if( palshifted == 1 ) {
        palshifted = 0;
        //asmsetpalette(&palette[0]);
    }

 return;
}

#if 0
void clearkeys(void) {

      memset(keystatus, 0, sizeof(keystatus));

 //return;
}
#endif


//JUST USING THIS FOR NOW
void TEMPSND(void) {
SND_Sound( rand()%60 );
}

void cleanup(void) {

    if( difficulty > 1 ) {
        currweapon=1;
        selectedgun=1;
    }
    else {
        currweapon=4;
        selectedgun=4;
    }
    currentpotion=0;
    helmettime=-1;
    shadowtime=-1;
    nightglowtime=-1;
    strongtime=-1;
    invisibletime=-1;
    manatime=-1;
    currentorb=0;
    currweaponfired=3;
    currweaponanim=0;
    currweaponattackstyle=0;
    currweaponflip=0;

}

