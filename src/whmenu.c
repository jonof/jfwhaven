/*********************************************************************
*
*   WHMENU.C - menu code for Witchaven game
*
*********************************************************************/

#include "icorp.h"


int loopinstuff; //here it is
extern int vampiretime;
extern int musiclevel;
extern int digilevel;

extern short gbrightness;

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
extern short brightness;
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

char typemessage[162], typemessageleng = 0, typemode = 0;

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
extern char tempboard[];
extern char loadgamename[];

extern char tempbuf[];
extern int frames;
#define MAXSAVEDGAMES 5

struct savedgame {
     char name[20];
};

struct savedgame savedgamenames[MAXSAVEDGAMES];

extern int gameactivated;

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
     int exit=0;
     int number;
     char temp[40];

     strlwr(string);
     len=strlen(string);
     strcpy(temp,string);

     for(i=0;i<len;i++) {
        tempbuf[i]=temp[i];
        for(j=0;j<40;j++) {
            if(tempbuf[i]==fancy[j]) {
                number=j;
            }
        }
        if( i == 0 ) {
            overwritesprite(x,y,tilenum+number,0,0,pal);
            incr+=tilesizx[tilenum+number]+1;
        }
        else if( tempbuf[i] != ' ' ) {
            overwritesprite(x+incr,y,tilenum+number,0,0,pal);
            incr+=tilesizx[tilenum+number]+1;
        }
        else
            incr+=8;
     }

}



void fancyfontscreen(int x, int y, short tilenum, char *string) {

     int i, j;
     int len;
     int incr=0;
     int exit=0;
     int number;
     char temp[40];

     strlwr(string);
     len=strlen(string);
     strcpy(temp,string);

     for(i=0;i<len;i++) {
        tempbuf[i]=temp[i];
        for(j=0;j<40;j++) {
            if(tempbuf[i]==fancy[j]) {
                number=j;
            }
        }
        if( i == 0 ) {
            overwritesprite(x,y,tilenum+number,0,0x02,7);
            incr+=tilesizx[tilenum+number]+1;
        }
        else if( tempbuf[i] != ' ' ) {
            overwritesprite(x+incr,y,tilenum+number,0,0x02,7);
            incr+=tilesizx[tilenum+number]+1;
        }
        else
            incr+=8;
     }

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
    char tempbuf[40];
    int goaltime;
    int i;


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

    if(svga == 1) {
        permanentwritesprite(0,0,SVGAMENU,0,0,0,639,239,0);
        permanentwritesprite(0,240,SVGAMENU2,0,0,240,639,479,0);
    }

    while( !exit ) {

            overwritesprite(0,0,MAINMENU,0,0,0);
            overwritesprite(127,58,MENUSELECTIONS,0,0,0);

            if( select < 5) {
                redpicnum=NEWGAMEGREEN+select;
                overwritesprite(redpic[select].x,redpic[select].y,redpicnum,0,0,0);
            }

            nextpage();
            if( totalclock >= goaltime ) {
                goaltime=totalclock+10L;
                if( keystatus[0xd0] || keystatus[0x50] ) {
                    TEMPSND();
                    select++;
                    if ( select > 4 )
                        select=0;
                }
                if( keystatus[0xc8] || keystatus[0x48] ) {
                    TEMPSND();
                    select--;
                    if ( select < 0 )
                        select=4;
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
    }thenames[MAXHELPSCREENS] = {   WEAPONS,
                                    SPELLS,
                                    POTIONS,
                                    WALKING,
                                    FLYING,
                                    CREDIT1,
                                    CREDIT2,
                                    CREDIT3,
                                    CREDIT4,
                                    BETAPAGE};

    int select=0;
    int goaltime;
    int exit=0;

    while (!exit) {
        if(totalclock >= goaltime) {
            overwritesprite(0,0,MAINMENU,0,0,0);
            overwritesprite(0,0,thenames[select].helpnames,0,0,0);
            nextpage();
            goaltime=totalclock+10L;
                if( keystatus[0xd0]
                    || keystatus[keys[KEYRIGHT]]
                    || keystatus[RDN]
                    || keystatus[keys[KEYBACK]]
                    || keystatus[RRIGHT]) {
                    TEMPSND();
                    select++;
                    if ( select > 9 )
                        select=9;
                }
                if( keystatus[0xc8]
                    || keystatus[keys[KEYLEFT]]
                    || keystatus[RUP]
                    || keystatus[keys[KEYFWD]]
                    || keystatus[RLEFT]) {
                    TEMPSND();
                    select--;
                    if ( select < 0 )
                        select=0;
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

    overwritesprite(0L,0L,MAINMENU,0,0,0);
    overwritesprite(182,80,LOADGREEN,0,0,0);
    overwritesprite(182,119,SAVERED,0,0,0);

    while ( !exit ) {
        if(select == 0) {
            overwritesprite(182,80,LOADGREEN,0,0,0);
            overwritesprite(182,119,SAVERED,0,0,0);
        }
        else {
            overwritesprite(182,80,LOADRED,0,0,0);
            overwritesprite(182,119,SAVEGREEN,0,0,0);
        }
        if( totalclock >= goaltime ) {
            goaltime=totalclock+10L;
                if( keystatus[keys[KEYBACK]] || keystatus[RDN] ) {
                    TEMPSND();
                    select=1;
                }
                if( keystatus[keys[KEYFWD]] || keystatus[RUP] ) {
                    TEMPSND();
                    select=0;
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
            nextpage();
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
    int goaltime;
    char temp[20];

    overwritesprite(0L,0L,MAINMENU,0,0,0);
    overwritesprite(123,79,AREYOUSURE,0,0,0);
    nextpage();

    while( !exit ) {

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
        if(svga == 1) {
            permanentwritesprite(0,0,SVGAORDER1,0,0,0,639,239,0);
            permanentwritesprite(0,240,SVGAORDER2,0,0,240,639,479,0);
            nextpage();
            exit=0;
            while( !exit ){
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
                if(keystatus[0x39] > 0 || keystatus[1] > 0)
                    exit=1;
                overwritesprite(0,0,ORDER1,0,0,0);
                nextpage();
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
        shutdown();
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

        struct player *plr;

        plr=&player[0];

        select=difficulty-1;
        keystatus[1]=0;
        keystatus[0x1c]=0;
        keystatus[0x9c]=0;

        //loadtile(MAINMENU);

        overwritesprite(0,0,MAINMENU,0,0,0);
        overwritesprite(127,58,BLOODGOREGREEN,0,0,0);
        overwritesprite(148,114,DIFFICULTRED,0,0,0);

        if( goreon == 1 ) {
            overwritesprite(180,84,NOGORESHADOW,0,0x04,0);
            overwritesprite(214,81,GORESOLID,0,0,0);
        }
        else {
            overwritesprite(180,84,NOGORESOLID,0,0,0);
            overwritesprite(214,81,GORESHADOW,0,0x04,0);
        }

        goaltime=totalclock+10L;

        while ( !exit ) {

        if( totalclock >= goaltime ) {
            goaltime=totalclock+10L;

            if(keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
                TEMPSND();
                select2=0;
            }
            if(keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
                TEMPSND();
                select2=1;
            }
            if(select2 == 0)
                pickone=0;
            else
                pickone=1;

            redpicnum=HORNYSKULL1+select;

            overwritesprite(0,0,MAINMENU,0,0,0);

            if(select2 == 0) {
                overwritesprite(148,114,DIFFICULTRED,0,0,0);
                overwritesprite(127,58,BLOODGOREGREEN,0,0,0);
            }
            else {
                overwritesprite(148,114,DIFFICULTGREEN,0,0,0);
                overwritesprite(127,58,BLOODGORERED,0,0,0);
            }
            overwritesprite(147,143,HORNYBACK,0,0,0);
            overwritesprite(redpic[select].x,redpic[select].y,redpicnum,0,0,0);

            if( goreon == 1 ) {
                overwritesprite(180,84,NOGORESHADOW,0,0x04,0);
                overwritesprite(214,81,GORESOLID,0,0,0);
            }
            else {
                overwritesprite(180,84,NOGORESOLID,0,0,0);
                overwritesprite(214,81,GORESHADOW,0,0x04,0);
            }

            if( pickone == 1 ) {
                if( keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
                    TEMPSND();
                    select--;
                    if( select < 0)
                        select=0;
                }
                if( keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
                    TEMPSND();
                    select++;
                    if( select > 3)
                        select=3;
                }
                selected=select;
            }
            else {
                if( keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
                    TEMPSND();
                    select3=0;
                }
                if( keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
                    TEMPSND();
                    select3=1;
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
        nextpage();
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

    char temp[20];
    char tempshow2dsector[MAXSECTORS>>3];
    char tempshow2dwall[MAXWALLS>>3];
    char tempshow2dsprite[MAXSPRITES>>3];
    int i;

    if (netgame) {
        SND_StartMusic(mapon-1);
        goto skip;
    }
    if(loadedgame == 0) {
        strcpy(boardname,"level");
        itoa(mapon,temp,10);
        strcat(boardname,temp);
        strcat(boardname,".map");
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
    int gn, i;
    char temp[20];
    int goaltime;

    goaltime=totalclock+10L;

    for(i=0;i<MAXSAVEDGAMES;i++)
        if( !savedgamedat(i))
            strcpy(savedgamenames[i].name,"empty");

    while ( !exit ) {

        overwritesprite(0,0,MAINMENU,0,0,0);
        overwritesprite(138,84,SAVENUMBERS,0,0,0);
        overwritesprite(190,48,LOADPIC,0,0,0);

        for(i=0;i<MAXSAVEDGAMES;i++) {
            if( i == select )
                fancyfont(154,81+(i*17),THEFONT,savedgamenames[select].name,7);
            else
                fancyfont(154,81+(i*17),THEFONT,savedgamenames[i].name,0);
        }

        nextpage();

        if( totalclock >= goaltime ) {
            goaltime=totalclock+10L;


            if ( keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
                select--;
                if( select < 0 )
                    select=4;
                keystatus[keys[KEYFWD]]=0;
            }
            if ( keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
                select++;
                if( select > 4 )
                    select=0;
                keystatus[keys[KEYBACK]]=0;
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
        strcpy(boardname,"svgm");
        itoa(select,temp,10);
        strcat(boardname,temp);
        strcpy(loadgamename,"svgn");
        strcat(loadgamename,temp);
        strcat(loadgamename,".dat");
        strcat(boardname,".map");
        loadedgame=1;
        gameactivated=1;
        startnewgame(plr);
    }


}

void savegame(struct player *plr) {

    int exit=0;
    int gn, i;
    int select=0;
    char temp[20];
    int goaltime;


    for(i=0;i<MAXSAVEDGAMES;i++)
        if( !savedgamedat(i))
            strcpy(savedgamenames[i].name,"EMPTY");

    goaltime=totalclock+10L;

    while ( !exit ) {

        overwritesprite(0,0,MAINMENU,0,0,0);
        overwritesprite(138,84,SAVENUMBERS,0,0,0);
        overwritesprite(188,50,SAVEPIC,0,0,0);

        for( i=0;i<MAXSAVEDGAMES;i++) {
            if(i == select)
                fancyfont(154,81+(i*17),THEFONT,savedgamenames[select].name,7);
            else
                fancyfont(154,81+(i*17),THEFONT,savedgamenames[i].name,0);
        }

        nextpage();

        if( totalclock >= goaltime ) {
            goaltime=totalclock+10L;

            if( keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
                select--;
                if ( select < 0)
                    select=4;
            }

            if( keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
                select++;
                if( select > 4 )
                    select=0;
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
    int i, len, j;
    char tempbuf[40];
    char temp[40];
    char temp1[40];
    char temp2[40];

    int  typemessageleng=0;

    plr=&player[pyrn];

    keystatus[0x1c]=keystatus[0x9c]=0;

    for(i=0;i<128;i++)
        keystatus[i]=0;

    strcpy(temp,"-");

    while( !exit ) {

        overwritesprite(0,0,MAINMENU,0,0,0);
        overwritesprite(138,84,SAVENUMBERS,0,0,0);
        overwritesprite(188,50,SAVEPIC,0,0,0);

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
                strcpy(tempbuf,"svgm");
                itoa(select,temp2,10);
                strcat(tempbuf,temp2);
                strcat(tempbuf,".map");
                saveboard(tempbuf,&plr->x,&plr->y,&plr->z,&plr->ang,&plr->sector);
                savedgamename(select);
            }
            else {
                strcpy(tempbuf,"svgm");
                itoa(select,temp2,10);
                strcat(tempbuf,temp2);
                strcat(tempbuf,".map");
                saveboard(tempbuf,&plr->x,&plr->y,&plr->z,&plr->ang,&plr->sector);
                savedgamename(select);
            }
            exit=2;
            keystatus[0x1c]=keystatus[0x9c]=0;
        }
        nextpage();
    }

    if( exit == 2 ) {
        keystatus[0x1c]=keystatus[0x9c]=0;
    }

}


int savedgamename(int gn) {

    struct player *plr;
    int  file;
    int  i;
    char temp[3];
    int tmpanimateptr[MAXANIMATES];

    plr=&player[0];

    if(svga == 1)
        plr->screensize=320;
    else
        plr->screensize=320;

    strcpy(tempbuf,"svgn");
    itoa(gn,temp,10);
    strcat(tempbuf,temp);
    strcat(tempbuf,".dat");

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
        write(file,&brightness,sizeof(brightness));
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

    int     fh=0,nr=0;
    char    fname[20];
    char    fsname[20];
    char    savedgame[40];
    char    temp[20];

    itoa(gn,temp,10);
    strcpy(fname,"svgm");
    strcat(fname,temp);
    strcat(fname,".map");

    itoa(gn,temp,10);
    strcpy(fsname,"svgn");
    strcat(fsname,temp);
    strcat(fsname,".dat");

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
    char temp[40];
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
    read(fh,&brightness,sizeof(brightness));
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
 *   TEKFLC    - flic playing code for TEKWAR                              *
 *                                                                         *
 *                                                     12/15/94 Jeff S.    *
 ***************************************************************************/
#if 0
char      flicnum=0;
char      *flicnames[3] = { "FLC1", "FLC2", "FLC3" };
int       flictype=FT_FULLSCREEN;

#pragma pack(1)

struct flichdr {
      long      fsize;              // size of entire flic file
      short     type;               // type of flic file 0xAF12=.FLC
      short     frames;             // frames in flic
      short     width;              // width of screen for this flic
      short     height;             // height of screen for this flic
      short     depth;              // bits per pixel (always 8)
      short     flags;              // set to 0x0003
      long      speed;              // time delay between frames. .FLC=millisecs
      short     reserved;           // set to 0
      long      created;            // MS-DOS formatted date & time
      long      creator;            // set to 0 and ignore
      long      updated;            // MS-DOS formatted date & time
      long      updater;            // serial number of prgm that last updated
      short     aspectx;            // aspect ratio X-axis
      short     aspecty;            // aspect ratio Y-axis
      char      reserved2[38];      // set to 0
      long      oframe1;            // offset from beg of file to 1st frame
      long      oframe2;            // offset from beg of file to 2nd frame
      char      reserved3[40];      // set to 0
} flchdr;

struct flicfrm {
      long      size;               // size of frame chunk
      short     type;               // prefix chunk id.  Always 0xF1FA
      short     chunks;             // number of subchunks
      char      reserved[8];        // set to 0
} flcfrm;

struct flichnk {
      long      size;
      short     type;
} flchnk;

struct rgbinf {
      unsigned char skip;
      unsigned char count;
} rgbinf;

struct rgbdta {
      unsigned char r;
      unsigned char g;
      unsigned char b;
} rgbdta;

signed    char      flcbuf[64*1024];
signed    char      *flcptr=flcbuf;
int       vidoff[200];
char      *Video=(0xA000<<4);
char      *vidptr=(0xA000<<4);

int       flchdl;
int       flcframe;
long      flcpos;

void
flc_abort(char *str)
{
     #ifdef QUIT_ON_FLC_ERROR
      crash(str);
     #endif
}

void
flc_init(void)
{
      int  i;

      for( i=0 ; i < 200 ; i++ ) {
             vidoff[i]=i*320;
      }
}

int                                // returns 1 if ok, 0 if bad open
flc_openflic(void)
{
      char i;

      if( (flchdl=open(flicnames[flicnum],O_RDONLY|O_BINARY,S_IREAD)) == -1 ) {
             return(0);
      }

      if( read(flchdl,&flchdr,sizeof(struct flichdr)) != sizeof(struct flichdr) ) {
             goto badopen;
      }

      if (flchdr.type == ( short)0xAF12) {
             return(1);
      }

badopen:
      close(flchdl);
      return(0);
}

void
flc_closeflic(void)
{
      char i,mask;

      close(flchdl);
}

#define   TRANSPCOL      0x00


void
flc_charstoscreen(int count)
{
      while( count-- > 0 ) {
             if( *flcptr != TRANSPCOL ) {
                    *vidptr++=*flcptr++;
             }
             else {
                    vidptr++;
                    flcptr++;
             }
      }
}

void
flc_wordstoscreen(int count)
{
      unsigned data;

      count<<=1;

      while (count-- > 0) {
             if( *flcptr != TRANSPCOL ) {
                    *vidptr++=*flcptr++;
                    }
             else {
                    vidptr++;
                    flcptr++;
             }
      }
}

#pragma   aux  repchrs =                   \
      "rep      stosb",                     \
      parm      [edi][ecx][eax]             \

void
flc_repeatchars(unsigned char c,int count)
{
      if( c != TRANSPCOL )
             repchrs(vidptr,( long)count,( long)c);

      vidptr+=count;
}

#pragma   aux  repwrds =                    \
      "rep      stosw",                      \
      parm      [edi][ecx][eax]              \

void
flc_repeatwords(unsigned c,int count)
{
      if( c != TRANSPCOL )
             repwrds(vidptr,( long)count,( long)c);

      vidptr+=(sizeof(short)*count);
}

void
flc_read(int hdl,char *buf,long size)
{
      int  rv;

      rv=read(hdl,buf,size);
      if( rv == -1 ) {
             flc_abort("Bad READ of FLC file.");
      }

 return;
}


#pragma   aux  blackscr =               \
      "mov  ax,0A000h",                  \
      "mov  es,ax",                      \
      "mov  di,0",                       \
      "mov  cx,32000",                   \
      "mov  ax,0",                       \
      "rep  stosw"                       \

short
shortvalue(void *byteptr)
{
      short  *iptr=byteptr;

 return(*iptr);
}


//#define LOADPALETTE


int                           // returns 1 if decompression successful
flc_decompframe(void)
{
      short     i,ipacket,lines,linetype,n,width,x,xorg,y,yorg;
      short     lastx,lpcount,opcount,psize;
      unsigned  char c;
      signed    char packet;
      char      *ptr;

      flcptr=flcbuf;
      memset(&flcfrm,0,sizeof(struct flicfrm));
      lseek(flchdl,flcpos,SEEK_SET);
      read(flchdl,&flcfrm,sizeof(struct flicfrm));
      flcpos+=flcfrm.size;

      if ((unsigned)flcfrm.type != ( short)0xF1FA) {
             return(0);
      }

      if( flcfrm.size > 64000L ) {   // should only happen for case 16 chunks
             read(flchdl,&flchnk,sizeof(struct flichnk));
             switch( flchnk.type ) {
             case CT_LITERAL:                 // LITERAL chunk type
                    y=0;
                    flchnk.size-=4;
                    read(flchdl,&i,sizeof(int));  // dummy integers
                    read(flchdl,&i,sizeof(int));  // dummy integers
                    flc_read(flchdl,flcbuf,flchnk.size);
                    memmove(vidptr,flcbuf,flchnk.size);
             }
             return(1);
      }

      flc_read(flchdl,flcbuf,flcfrm.size-sizeof(struct flicfrm));

      while (flcfrm.chunks-- > 0) {

             ptr=flcptr;
             memmove(&flchnk,flcptr,sizeof(struct flichnk));
             flcptr+=sizeof(struct flichnk);
             switch (flchnk.type) {

             case CT_COLOR_256:                  // 256 color
                    if( flictype != FT_FULLSCREEN ) {
                          break;
                    }
                    c=0;
                    ipacket=shortvalue(flcptr);
                    flcptr+=sizeof(short);
                    while (ipacket-- > 0) {
                          memmove(&rgbinf,flcptr,sizeof(struct rgbinf));
                          flcptr+=sizeof(struct rgbinf);
                          c+=rgbinf.skip;
                          if (rgbinf.count == 0) {
                                 i=256;
                          }
                          else {
                                 i=rgbinf.count;
                          }
                          outp(0x3C8,c);
                          for (n=0 ; n < i ; n++) {
                                 memmove(&rgbdta,flcptr,sizeof(struct rgbdta));
                                 flcptr+=sizeof(struct rgbdta);
                                 outp(0x3C9,(rgbdta.r)>>2);
                                 outp(0x3C9,(rgbdta.g)>>2);
                                 outp(0x3C9,(rgbdta.b)>>2);
                          }
                          c+=i;
                    }
                    break;

             case CT_DELTA_FLC:                  // DELTA FLC chunk type
                    x=xorg=y=0;
                    lines=shortvalue(flcptr);
                    flcptr+=sizeof(short);
                    while (lines-- > 0) {
nextpacket:
                          linetype=shortvalue(flcptr);
                          flcptr+=sizeof(short);
                          if( linetype < 0) {
                                 if (linetype&0x4000) {   // line skip count
                                        y+=abs(linetype);
                                        goto nextpacket;
                                 }
                                 else {                   // last pixel on line
                                        vidptr=Video+vidoff[y]+lastx;
                                        *vidptr=(unsigned char)linetype;
                                        linetype=shortvalue(flcptr);
                                        flcptr+=sizeof(short);
                                        if( linetype == 0) {
                                              y++;
                                              if (--lines > 0) {
                                                     goto nextpacket;
                                              }
                                        }
                                 }
                          }
                          else {
                                 x=xorg;
                                 while (linetype-- > 0) { // packets to process
                                        x+=(unsigned char)(*flcptr++);
                                        packet=(*flcptr++);
                                        if( packet >= 0 ) {
                                              vidptr=Video+vidoff[y]+x;
                                              flc_charstoscreen(packet<<1);
                                        }
                                        else {
                                              packet=abs(packet);
                                              vidptr=Video+vidoff[y]+x;
                                              flc_repeatwords(shortvalue(flcptr),packet);
                                              flcptr+=sizeof(short);
                                        }
                                        x+=packet<<1;
                                 }
                                 y++;
                          }
                    }
                    break;

             case CT_BYTE_RUN:
                    lines=0;
                    vidptr=Video;
                    while( lines < flchdr.height ) {
                          width=0;
                          (*flcptr++);          // throw away byte
                          while( width < flchdr.width ) {
                                 packet=(*flcptr++);
                                 if( packet < 0 ) {   // copy pixels
                                        packet=abs(packet);
                                        flc_charstoscreen(packet);
                                 }
                                 else {
                                        flc_repeatchars((*flcptr++),packet);
                                 }
                                 width+=packet;
                          }
                          lines++;
                    }
                    break;

             case CT_BLK:
                    #ifdef TRANSPARENT
                     blackscr();
                    #endif
                    break;

             case CT_COLOR_64:
                    flc_abort("ERROR: .FLI only COLOR_64 chunk");
                    break;

             case CT_DELTA_FLI:
                    flc_abort("ERROR: .FLI only DELTA_FLI chunk");
                    break;

             case CT_PSTAMP:
                    break;

             default:
                    break;
             }
             flcptr=ptr+flchnk.size;
      }

 return(1);
}



#pragma aux setvmode =    \
    "int 0x10",          \
    parm [eax]           \

#pragma   aux  clr =               \
      "mov ebx, 200",               \
      "nextrow:",                   \
      "mov ecx, 320",               \
      "rep stosb",                  \
      "dec ebx",                    \
      "jg  nextrow",                \
      parm [esi][edi][eax]          \


void
flc_playseq(struct  player *plr, int num, int ftype)
{
      if ( plr->dimension == 2 ) {
             return;
      }

      flictype=ftype;
      flcframe=0;

      flicnum=(num&3);

      if( flc_openflic() == 0 )
             return;

      if( ftype == FT_FULLSCREEN ) {
             //fadeout(4);
             fadeout(0,255,10,10,15,200);
             setvmode(0x13);
      }

      flcpos=flchdr.oframe1;
      flc_decompframe();
      flcpos=flchdr.oframe2;
      flcframe=0;

      if( ftype == FT_DATALINK )
        while( (keystatus[1]==0) && (flcframe < flchdr.frames-1) && flc_decompframe() ) {
          if( keystatus[25] != 0 ) {
                 keystatus[25]=0;
                    while( keystatus[25]==0 )
                          ;
                    keystatus[25]=0;
          }
        }
      else if( ftype == FT_FULLSCREEN )
      while( (flcframe < flchdr.frames-1) && flc_decompframe() ) {
        }


      flc_closeflic();

      if( ftype == FT_FULLSCREEN ) {
             setgamemode();
             clearpal();
             paletteloaded=0;
             loadpalette();
             dofadein=1;
             //fadein(1);
             fadein(0,255,2);
      }

 return;
}
#endif
/***************************************************************************
 *   TEKERR    - crirical error handler for TEKWAR                         *
 *                                                                         *
 *                                                     12/15/94 Jeff S.    *
 ***************************************************************************/

int criterr_flag;

int
cehndlr(unsigned deverr, unsigned errcode, unsigned far *devhdr)
{
      criterr_flag=errcode;
      return(_HARDERR_IGNORE);
}


void
installcrerrhndlr(void)
{
      _harderr(cehndlr);

 return;
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

extern    void near asmwaitvrt(int parm1);
#pragma aux asmwaitvrt  "*_"       \
     parm caller     [ecx]          \
     modify          [eax edx];

extern    void near asmsetpalette(char *pal);
#pragma aux asmsetpalette "*_"     \
    parm caller     [esi]           \
    modify          [eax ecx edx];

void
clearpal(void)
{
      short     i;

      outp(PEL_WRITE_ADR,0);
    for(i=0;i<768;i++)
          outp(PEL_DATA,0x00);

 return;
}


char  palette1[256][3],palette2[256][3];

void
getpalette(char *palette)
{
    int   i;

    outp(PEL_READ_ADR,0);
    for( i=0; i<768; i++)
        *palette++ = inp(PEL_DATA);
}

void
fillpalette(int red, int green, int blue)
{
    int   i;

    outp(PEL_WRITE_ADR,0);
    for( i=0; i<256; i++ ) {
        outp(PEL_DATA,red);
        outp(PEL_DATA,green);
        outp(PEL_DATA,blue);
    }
}


char      foggy=0;

void
fadeout(int start, int end, int red, int green, int blue, int steps)
{
    int      i,j,orig,delta;
    char      *origptr,*newptr;

      asmwaitvrt(1);
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

        asmwaitvrt(1);
        asmsetpalette(&palette2[0][0]);
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

    asmwaitvrt(1);
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

        asmwaitvrt(1);
        asmsetpalette(&palette2[0][0]);
    }

      // final color
    asmsetpalette(palette);

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
      char      *lookptr;

      lookptr=palookup[0];
      palookup[0]=palookup[1];
      palookup[1]=lookptr;
}

void
makefxlookups(void)
{
      char      palbuf[256];
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

char      whiteshifts[NUMREDSHIFTS][768];
char      redshifts[NUMREDSHIFTS][768];
char      greenshifts[NUMGREENSHIFTS][768];
char      blueshifts[NUMBLUESHIFTS][768];

int       redcount,whitecount,greencount,bluecount;
char      palshifted;

extern    char palette[];

void
initpaletteshifts(void)
{
    char      *workptr,*baseptr;
    int       i,j,delta;

    for( i=1; i<=NUMREDSHIFTS; i++ ) {
        workptr=( char *)&redshifts[i-1][0];
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
        workptr=( char *)&whiteshifts[i-1][0];
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
        workptr=( char *)&greenshifts[i-1][0];
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
        workptr=( char *)&blueshifts[i-1][0];
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
        asmwaitvrt(1);
        asmsetpalette(redshifts[red-1]);
        palshifted = 1;
    }
    else if( white ) {
        asmwaitvrt(1);
        asmsetpalette(whiteshifts[white-1]);
        palshifted = 1;
    }
    else if( green ) {
        asmwaitvrt(1);
        asmsetpalette(greenshifts[green-1]);
        palshifted=1;
    }
    else if( blue ) {
        asmwaitvrt(1);
        asmsetpalette(blueshifts[blue-1]);
        palshifted=1;
    }

    else if( palshifted ) {
        asmwaitvrt(1);
        asmsetpalette(&palette[0]);     // back to normal
        setbrightness(gbrightness);
        palshifted = 0;
    }

 return;
}


void
finishpaletteshifts(void)
{
    if( palshifted == 1 ) {
        palshifted = 0;
        asmwaitvrt(1);
        asmsetpalette(&palette[0]);
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

