/***************************************************************************
 *   WHNET.C   - Witchaven network module & multiplayer code               *
 *                                                                         *
 *                                                     - Les Bird 07/12/95 *
 ***************************************************************************/

//#define   DEBUGOUTPUT

#include "icorp.h"
#include "time.h"
#include "mmulti.h"

#define   MAXDATASIZE    255
#define   MAXTEAMS       4

#define   TEAMFLAG       2232

#define   TEAM1SECTOR    15
#define   TEAM2SECTOR    16
#define   TEAM3SECTOR    17
#define   TEAM4SECTOR    18

#define   TEAM1FLAG      1
#define   TEAM2FLAG      2
#define   TEAM3FLAG      3
#define   TEAM4FLAG      4

#define   TEAM1PAL       0
#define   TEAM2PAL       10
#define   TEAM3PAL       11
#define   TEAM4PAL       12


static char netmsg[80];

unsigned char plrindex[256],
     tagindex[MAXPLAYERS];

int  netgame,
     netinitialized;

//#pragma pack(1)

struct netpck {
     unsigned char tagno;          // players random tag number
     short node;                   // NETNOW node number
     char enemytype;               // monster this player chose
     unsigned char teamno;         // team this player chose
     char weap;                    // weapon being carried
     short score;                  // player's score (team or head to head)
     int x,
          y,
          z;                       // x,y,z for this player
     short ang;                    // player's angle
     short horiz;                  // aiming horizontal
     signed char action;           // action being performed
     char taghit;                  // player tag that was hit
     char guntype;                 // weapon in use
     char mapon;                   // map player is on
     char gametype;                // game this player has selected
     char hasflag;                 // player is carrying a team flag
}    netpck[MAXPLAYERS],
     othnetpck,
     sndnetpck;

struct msgpck {
     char team;
     char msg[80];
};

struct mdmpck {
     char hdr[4];
     char src;
     char dest;
     char len;
     struct netpck data;
}    mdmsndpck,
     mdmrcvpck;

#pragma pack()

int myadr,
     othadr[MAXPLAYERS];


short enemytype,
     gametype,
     mynode,
     playersingame;

unsigned char killedby = 0,
     myrandomtag = 255;

short comp;

short debugnet,
     dropflag,
     ihaveflag,
     myteam,
     teamcolor[MAXTEAMS] = {TEAM1PAL, TEAM2PAL, TEAM3PAL, TEAM4PAL},
     teamflagsprite[MAXTEAMS],
     teaminplay[MAXPLAYERS],
     teamscore[MAXPLAYERS],
     xmit = 1;

int bps = 9600L,
     netstartx,
     netstarty,
     netstartz,
     netstarts,
     numrpcks[MAXPLAYERS],
     numspcks[MAXPLAYERS],
     teamflagstartx[MAXTEAMS],
     teamflagstarty[MAXTEAMS],
     teamflagstartz[MAXTEAMS],
     totrhdrs,
     totrpcks,
     totspcks;

int  enempics[] = {
//     737,      // kobold
//     777,      // devil
//     821,      // fat witch
//     922,      // goblin
//     981,      // minotaur
//     1028,     // skull witch
     GRONHAL,                      // grondeval halberd
     GRONSW,                       // grondeval halberd
     GRONMU                        // grondeval halberd
};

int  xreps[] = {
//     54,       // kobold
//     32,       // devil
//     32,       // fat witch
//     32,       // goblin
//     32,       // minotaur
//     32,       // skull witch
     32,//GRONHALXR,
     32,//GRONSWXR,
     32,//GRONMUXR
};

int  yreps[] = {
//     54,       // kobold
//     36,       // devil
//     32,       // fat witch
//     36,       // goblin
//     64,       // minotaur
//     64,       // skull witch
     64,//GRONHALYR,
     64,//GRONSWXR,
     64,//GRONMUXR
};

int  enemheights[] = {
//     45,
//     50,
//     40,
//     35,
//     40,
//     55,
     40,
     40,
     40
};

#define   MAXARGS        10

char *argparm[] = {
     "MAP",
     "CYBERMAXX",
     "NET",
     "MODEM",
     "SERIAL",
     "DEBUGNET",
     "IGLASSES",
     "MPLAYER",
     "VFX1",
     "LOCAL"
};

char *monsternames[] = {
//     "Kobold",
//     "Devil",
//     "Fat Witch",
//     "Goblin",
//     "Minotaur",
//     "Skull Witch",
     "Red",
     "Blue",
     "Purple",
     "Brown"
};

char *teamcolorstrings[] = {
     "RED",
     "BLUE",
     "PURPLE",
     "BROWN"
};

#define   NUMGAMES       3

char *gamenames[] = {
     "Head to Head",
     "Capture the Flag 21",
     "Capture the Flag 7"
};

char *mapnames[] = {
     "War Temple of Abu",
     "The Abyssal Pit",
     "Maze of Midan",
     "Cave of Chaos",
     "Lord Verkapherons Castle"
};

short mapnumbers[] = {
     30, 31, 32, 33, 34
};

int  comadr[] = {
     0x000, 0x3F8, 0x2F8, 0x3E8, 0x2E8
};

extern
int  mapflag,
     mapon;

extern
int  playerdie;

extern
int  currweapon,
     currweaponfired,
     escapetomenu,
     gameactivated;

void whnetmon(void);

extern
int  oldvmode;

extern
int selectedgun;

#ifdef DEBUGOUTPUT
FILE *dbgfp;
#endif

#ifdef DEBUGOUTPUT
void
debugout(char *fmt,...)
{
     va_list argptr;

     va_start(argptr, fmt);
     vfprintf(dbgfp, fmt, argptr);
     va_end(argptr);
}
#else
void
debugout(char *fmt,...)
{
     va_list argptr;

     va_start(argptr, fmt);
     vsprintf(netmsg, fmt, argptr);
     va_end(argptr);
}
#endif

extern
unsigned vixen;

void
netcheckargs(int argc, const char * const argv[])
{
     int  i,
          j;

// fprintf(stdaux, "netcheckargs\r\n");

     i = 1;
     while (i < argc) {
          for (j = 0; j < MAXARGS; j++) {
               if (Bstrcasecmp(argv[i], argparm[j]) == 0) {
                    switch (j) {
                    case 0:        // map number
                         mapon = atoi(argv[i + 1]);
                         mapflag = 1;
                         i++;
                         break;
                    case 1:        // cybermax
                         break;
                    case 2:        // network
                         netgame = 1;
                         break;
                    case 3:        // modem
                         netgame = 2;
                         break;
                    case 4:        // direct serial
                         netgame = 3;
                         break;
                    case 5:
                         debugnet = 1;
                         break;
                    case 6:
                         break;
                    case 7:
                         break;
                    case 8:
                         break;
                    case 9:
                         //_dos_getdrive((unsigned *)&vixen);
                         break;
                    }
               }
          }
          i++;
     }
}

int
netattacking(short p)
{
// fprintf(stdaux, "netattacking\r\n");

     if (netpck[p].action == KEYFIRE) {
          return (1);
     }
     return (0);
}

void
netsendpck(struct netpck * p, short dnode)
{
// fprintf(stdaux, "netsendpck\r\n");

     switch (netgame) {
     case 1:
          break;
     case 2:
     case 3:
          mdmsndpck.hdr[0] = 0x01;
          mdmsndpck.hdr[1] = 0x02;
          mdmsndpck.hdr[2] = 0x03;
          mdmsndpck.hdr[3] = 0x21;
          mdmsndpck.src = mynode;
          mdmsndpck.dest = dnode;
          mdmsndpck.len = sizeof(struct netpck);
          memmove(&mdmsndpck.data, p, sizeof(struct netpck));
          sendpacket(0, (void*)&mdmsndpck, sizeof(struct mdmpck));
          break;
     }
     numspcks[plrindex[myrandomtag]]++;
     totspcks++;
}

short
netchecktouchflag(struct player * plr)
{
     short i,
          j;

// fprintf(stdaux, "netchecktouchflag\r\n");
     if (gametype < 1) {
          return (0);
     }
     if (ihaveflag) {
          return (ihaveflag);
     }
     i = headspritesect[plr->sector];
     while (i != -1) {
          j = nextspritesect[i];
          if (labs(plr->x - sprite[i].x) + labs(plr->y - sprite[i].y) < 512
              && labs((plr->z >> 8) - ((sprite[i].z >> 8) - (tilesizy[sprite[i].picnum] >> 1))) <= 40) {
               switch (sprite[i].picnum) {
               case TEAMFLAG:
                    switch (sprite[i].pal) {
                    case TEAM1PAL:
                         SND_Sound(S_POTION1);
                         return (1);
                    case TEAM2PAL:
                         SND_Sound(S_POTION1);
                         return (2);
                    case TEAM3PAL:
                         SND_Sound(S_POTION1);
                         return (3);
                    case TEAM4PAL:
                         SND_Sound(S_POTION1);
                         return (4);
                    }
                    break;
               }
          }
          i = j;
     }
     return (0);
}

void
netmarkflag(short i)
{
// fprintf(stdaux, "netmarkflag\r\n");

     if (gametype < 1) {
          deletesprite(i);
          return;
     }
     switch (sprite[i].pal) {
     case TEAM1PAL:
          teamflagsprite[0] = i;
          teamflagstartx[0] = sprite[i].x;
          teamflagstarty[0] = sprite[i].y;
          teamflagstartz[0] = sprite[i].z;
          break;
     case TEAM2PAL:
          teamflagsprite[1] = i;
          teamflagstartx[1] = sprite[i].x;
          teamflagstarty[1] = sprite[i].y;
          teamflagstartz[1] = sprite[i].z;
          break;
     case TEAM3PAL:
          teamflagsprite[2] = i;
          teamflagstartx[2] = sprite[i].x;
          teamflagstarty[2] = sprite[i].y;
          teamflagstartz[2] = sprite[i].z;
          break;
     case TEAM4PAL:
          teamflagsprite[3] = i;
          teamflagstartx[3] = sprite[i].x;
          teamflagstarty[3] = sprite[i].y;
          teamflagstartz[3] = sprite[i].z;
          break;
     }
     sprite[i].xrepeat = 24;
     sprite[i].yrepeat = 24;
}

void
moveflag(int x, int y, int z, int teamno)
{
     short s;

// fprintf(stdaux, "moveflag\r\n");

     if (teamno > 0) {
          s = teamflagsprite[teamno - 1];
          if (s >= 0) {
               setsprite(s, x, y, z + (16 << 8));
          }
     }
}

#define TIMER_RESOLUTION 250

void
sendmyinfo(short dnode, short forcesend)
{
     short p = 0;//FIXME
     struct player *plr;

     if (!forcesend) {
          static clock_t t;
          clock_t n;

          if (t < (n = (clock() + TIMER_RESOLUTION)))
               t = n;
          else
               return;
     }

     plr = &player[0];
     plr->lvl = 9;
     updatesector(plr->x, plr->y, &plr->sector);
     ihaveflag = netchecktouchflag(plr);
     sndnetpck.tagno = myrandomtag;
     sndnetpck.node = mynode;
     sndnetpck.enemytype = enemytype;
     sndnetpck.x = plr->x;
     sndnetpck.y = plr->y;
     sndnetpck.z = plr->z;
     sndnetpck.ang = plr->ang;
     sndnetpck.horiz = plr->horiz;
     sndnetpck.mapon = mapon;
     sndnetpck.gametype = gametype;
     sndnetpck.teamno = myteam;
     sndnetpck.weap = currweapon;
     sndnetpck.hasflag = ihaveflag;
     if (gametype >= 1) {
          sndnetpck.score = teamscore[myteam];
     }
     else {
          sndnetpck.score = teamscore[0];
     }
     if (gametype >= 1) {
          teaminplay[myteam] = 1;
          captureflagpic();
     }
     if (playerdie) {
          sndnetpck.action = -1;
          sndnetpck.taghit = killedby;
          killedby = 0;
          if (ihaveflag) {
               if (teamflagsprite[ihaveflag - 1] >= 0) {
                    sprite[teamflagsprite[ihaveflag - 1]].cstat &= ~32768;
               }
          }
          ihaveflag = 0;
     }
     else if (dropflag) {
          dropflag = 0;
          sndnetpck.action = -2;
          sprite[teamflagsprite[ihaveflag - 1]].cstat &= ~32768;
          if (ihaveflag - 1 == myteam) {
               netmarkflag(teamflagsprite[ihaveflag - 1]);
          }
          ihaveflag = 0;
     }
     else {
          if (ihaveflag) {
               moveflag(plr->x, plr->y, plr->z, ihaveflag);
               if (teamflagsprite[ihaveflag - 1] >= 0) {
                    sprite[teamflagsprite[ihaveflag - 1]].cstat |= 32768;
               }
          }
          if (currweaponfired) {
               sndnetpck.action = KEYFIRE;
          }
     }
     if (forcesend
         || memcmp(&sndnetpck, &netpck[0], sizeof(struct netpck)) != 0) {
          memmove(&netpck[0], &sndnetpck, sizeof(struct netpck));
#if 0
          if (forcesend) {
               sprintf(netmsg, "PCKT FORCED");
          }
          else {
               sprintf(netmsg, "PCKT DIFFERENT");
          }
#endif
          switch (netgame) {
          case 1:
               netsendpck(&sndnetpck, dnode);
               break;
          case 2:
          case 3:
               netsendpck(&sndnetpck, 255);
               break;
          }
     }
#if 0
     else {
          sprintf(netmsg, "PCKT NOT SENT");
     }
#endif
     if (gametype >= 1 && ihaveflag) {
          switch (sector[plr->sector].lotag) {
          case TEAM1SECTOR:
               switch (ihaveflag) {
               case TEAM2FLAG:
               case TEAM3FLAG:
               case TEAM4FLAG:
                    if (teaminplay[ihaveflag - 1]) {
                         teamscore[TEAM1FLAG - 1] += 1;
                         playsound_loc(S_DROPFLAG, netpck[p].x, netpck[p].y);
//                         if (teamscore[ihaveflag-1] > 0) {
//                              teamscore[ihaveflag-1]--;
//                         }
                    }
                    dolevelmusic(rand()%10);
                    break;
               }
               dropflagstart(ihaveflag);
               captureflagpic();
               if (teamflagsprite[ihaveflag - 1] >= 0) {
                    sprite[teamflagsprite[ihaveflag - 1]].cstat &= ~32768;
               }
               ihaveflag = 0;
               break;
          case TEAM2SECTOR:
               switch (ihaveflag) {
               case TEAM1FLAG:
               case TEAM3FLAG:
               case TEAM4FLAG:
                    if (teaminplay[ihaveflag - 1]) {
                         teamscore[TEAM2FLAG - 1] += 1;
                         playsound_loc(S_DROPFLAG, netpck[p].x, netpck[p].y);
//                         if (teamscore[ihaveflag-1] > 0) {
//                              teamscore[ihaveflag-1]--;
//                         }
                    }
                    dolevelmusic(rand()%10);
                    break;
               }
               dropflagstart(ihaveflag);
               captureflagpic();
               if (teamflagsprite[ihaveflag - 1] >= 0) {
                    sprite[teamflagsprite[ihaveflag - 1]].cstat &= ~32768;
               }
               ihaveflag = 0;
               break;
          case TEAM3SECTOR:
               switch (ihaveflag) {
               case TEAM1FLAG:
               case TEAM2FLAG:
               case TEAM4FLAG:
                    if (teaminplay[ihaveflag - 1]) {
                         teamscore[TEAM3FLAG - 1] += 1;
                         playsound_loc(S_DROPFLAG, netpck[p].x, netpck[p].y);
//                         if (teamscore[ihaveflag-1] > 0) {
//                              teamscore[ihaveflag-1]--;
//                         }
                    }
                    dolevelmusic(rand()%10);
                    break;
               }
               dropflagstart(ihaveflag);
               captureflagpic();
               if (teamflagsprite[ihaveflag - 1] >= 0) {
                    sprite[teamflagsprite[ihaveflag - 1]].cstat &= ~32768;
               }
               ihaveflag = 0;
               break;
          case TEAM4SECTOR:
               switch (ihaveflag) {
               case TEAM1FLAG:
               case TEAM2FLAG:
               case TEAM3FLAG:
                    if (teaminplay[ihaveflag - 1]) {
                         teamscore[TEAM4FLAG - 1] += 1;
                         playsound_loc(S_DROPFLAG, netpck[p].x, netpck[p].y);
//                         if (teamscore[ihaveflag-1] > 0) {
//                              teamscore[ihaveflag-1]--;
//                         }
                    }
                    dolevelmusic(rand()%10);
                    break;
               }
               dropflagstart(ihaveflag);
               captureflagpic();
               if (teamflagsprite[ihaveflag - 1] >= 0) {
                    sprite[teamflagsprite[ihaveflag - 1]].cstat &= ~32768;
               }
               ihaveflag = 0;
               break;
          }
     }
}

void
netjoingame(void)
{                                  // announce that we've entered the game
     int  i;
     FILE *fp;

// fprintf(stdaux, "netjoingame\r\n");
     if (netgame > 1) {
          for (i = 0; i < 256; i++) {
               plrindex[i] = 255;
          }
          for (i = 0; i < MAXTEAMS; i++) {
               teaminplay[i] = 0;
               teamscore[i] = 0;
          }
     }
     for (i = 0; i < MAXPLAYERS; i++) {
          netpck[i].hasflag = 0;
     }
     ihaveflag = 0;
     switch (netgame) {
     case 1:
          break;
     case 2:
     case 3:
          mynode = 0;
          if (myrandomtag == 255) {
               srand((unsigned)clock());
               do {
                    myrandomtag = rand() & 255;
               } while (myrandomtag == 255);
               fp = fopen("tag.dat", "wb");
               if (fp != NULL) {
                    fwrite(&myrandomtag, sizeof(myrandomtag), 1, fp);
                    fclose(fp);
               }
          }
          plrindex[myrandomtag] = 0;
          tagindex[0] = myrandomtag;
          playersingame = 1;
          break;
     }
     sendmyinfo(-1, 1);
     SND_Sound(S_DROPFLAG);
}

void
netsendmove(void)
{
     //short p;
     static int sendclock = 0L;

// fprintf(stdaux, "netsendmove\r\n");

     if (netgame == 0) {
          return;
     }
     if (netgame == 1) {
          sendmyinfo(-1, 0);
//          for (p=0 ; p < playersingame ; p++) {
//               if (mynode != p || playersingame == 1) {
//                    sendmyinfo(p,0);
//               }
//          }
     }
     else if (xmit) {
          if (playersingame > 1) {
               sendmyinfo(255, 1);
          }
          else if (totalclock >= sendclock) {
               sendclock = totalclock + (CLKIPS * 4);
               sendmyinfo(255, 1);
          }
     }
     memset(&sndnetpck, 0, sizeof(struct netpck));
}

void
netshootgun(short s, char guntype)
{
     short p;

// fprintf(stdaux, "netshootgun\r\n");

     sndnetpck.guntype = guntype;
     if (s != -1) {
          for (p = 0; p < playersingame; p++) {
               if (player[p].spritenum == s) {
                    sndnetpck.taghit = tagindex[p];
                    break;
               }
          }
     }
     else {
          sndnetpck.taghit = myrandomtag;
     }
     netsendmove();
}

void
netdamageactor(short s, short o)
{
     int  val;

// fprintf(stdaux, "netdamageactor\r\n");

     if (player[0].spritenum == s) {
          switch (sprite[o].picnum) {
          case PLASMA:
               val = (krand() & 0x07);
               break;
          case MONSTERBALL:
               val = (krand() & 0x7F);
               break;
          case BULLET:
          case THROWPIKE:
               val = (krand() & 0x7F);
               break;
          }
          if (shieldpoints > 0 && selectedgun < 5) {
               val >>= 1;
               shieldpoints -= val >> 1;
          }
          healthpic(-(val));
          if (player[0].health <= 0) {
               killedby = netpck[sprite[o].owner - 4096].tagno;
          }
     }
}

void
nethitsprite(short p, char guntype, char taghit)
{
     short daang,
          hitsect,
          hitsprite,
          hitwall,
          i,
          j,
          k;
     int dax,
          day,
          hitx,
          hity,
          hitz;
     struct player *myplr,
         *plr;

// fprintf(stdaux, "nethitsprite\r\n");

     plr = &player[p];
     myplr = &player[0];
//     sprintf(netmsg,"HIT TAG %d WITH %d",taghit,guntype);
     if (guntype == 5) {
          daang = plr->ang;
          hitscan(plr->x, plr->y, plr->z, plr->sector,
                  sintable[(daang + 512) & 2047],
                  sintable[daang],
                  (100 - plr->horiz) * 2000,
                  &hitsect, &hitwall, &hitsprite, &hitx, &hity, &hitz, CLIPMASK1);
          if (hitwall >= 0 && hitsprite < 0) {
               neartag(hitx, hity, hitz, hitsect, daang,
                       &neartagsector, &neartagwall, &neartagsprite,
                       &neartaghitdist, 1024, 0);
               if (neartagsector < 0) {
                    if (sector[neartagsector].lotag == 0) {
                         j = insertsprite(hitsect, 0);
                         sprite[j].x = hitx;
                         sprite[j].y = hity;
                         sprite[j].z = hitz + (8 << 8);
                         sprite[j].cstat = 17;    // was16
                         sprite[j].picnum = WALLARROW;
                         sprite[j].shade = 0;
                         sprite[j].pal = 0;
                         sprite[j].xrepeat = 16;
                         sprite[j].yrepeat = 48;
                         sprite[j].ang = ((daang + 2048) - 512 + (rand() & 128 - 64)) & 2047;
                         sprite[j].xvel = 0;
                         sprite[j].yvel = 0;
                         sprite[j].zvel = 0;
                         sprite[j].owner = 4096;
                         sprite[j].lotag = 32;
                         sprite[j].hitag = 0;
                    }
               }
          }
     }
     switch (guntype) {
     case 0:
          playsound_loc(S_SOCK1 + rand() % 4, plr->x, plr->y);
          break;
     case 1:
          if (rand() % 2) {
               playsound_loc(S_GORE1 + rand() % 4, plr->x, plr->y);
          }
          break;
     case 2:
          playsound_loc(S_SWORDCLINK1 + rand() % 4, plr->x, plr->y);
          break;
     case 3:
          playsound_loc(S_SOCK1 + rand() % 4, plr->x, plr->y);
          break;
     case 4:
          playsound_loc(S_SWORD1 + rand() % 6, plr->x, plr->y);
          break;
     case 5:
          playsound_loc(S_SWORD1 + rand() % 2, plr->x, plr->y);
          break;
     case 6:
          playsound_loc(S_SWORD1 + rand() % 6, plr->x, plr->y);
          break;
     case 7:
          playsound_loc(S_PLRWEAPON4, plr->x, plr->y);
          break;
     case 8:
          playsound_loc(S_SWORD1 + rand() % 2, plr->x, plr->y);
          break;
     case 9:
          playsound_loc(S_SWORD1 + rand() % 3, plr->x, plr->y);
          if (rand() % 2) {
               playsound_loc(S_GORE1 + rand() % 4, plr->x, plr->y);
          }
          break;
     }
     if (taghit == myrandomtag) {
          sprite[teamflagsprite[ihaveflag - 1]].cstat &= ~32768;
          ihaveflag = 0;
          switch (guntype) {
          case 0:
               k = (krand() % 5);
               break;
          case 1:
               k = (krand() % 5);
               break;
          case 2:
               k = (krand() % 10);
               break;
          case 3:
               k = (krand() % 20);
               break;
          case 4:
               k = (krand() % 25);
               break;
          case 5:
               k = (krand() % 30);
               break;
          case 6:
               k = (krand() % 30);
               break;
          case 7:
               k = (krand() % 10) + 5;
               break;
          case 8:
               k = (krand() % 20) + 5;
               break;
          case 9:
               k = (krand() % 20) + 5;
               break;
          }
          if (shieldpoints > 0 && selectedgun < 5) {
               k >>= 1;
               shieldpoints -= k >> 1;
          }
          healthpic(-k);
          if (player[0].health <= 0) {
               killedby = netpck[p].tagno;
          }
          if (rand() % 2) {
               painsound(myplr->x, myplr->y);
          }
     }
     else {
          switch (guntype) {
          case 12:
               for (i = 0; i < 10; i++) {
                    j = insertsprite(plr->sector, MISSILE);
                    sprite[j].x = plr->x;
                    sprite[j].y = plr->y;
                    sprite[j].z = plr->z + (8 << 8) + ((krand() & 10) << 8);
                    sprite[j].cstat = 0;
                    sprite[j].picnum = PLASMA;
                    sprite[j].shade = -32;
                    sprite[j].pal = 0;
                    sprite[j].xrepeat = 16;
                    sprite[j].yrepeat = 16;
                    daang = sprite[j].ang = (plr->ang - (i * 5)) + (i * 10);
                    sprite[j].xvel = (sintable[(daang + 512) & 2047] >> 5);
                    sprite[j].yvel = (sintable[daang] >> 5);
                    sprite[j].zvel = ((100 - plr->horiz) << 4);
                    sprite[j].owner = 4096 + p;
                    sprite[j].lotag = 256;
                    sprite[j].hitag = 0;
                    sprite[j].clipdist = 48;
                    dax = sprite[j].xvel;
                    day = sprite[j].yvel;
                    movesprite((short) j, (dax * synctics) << 3, (day * synctics) << 3, 0L,
                               4L << 8, 4L << 8, 0);
                    setsprite(j, sprite[j].x, sprite[j].y, sprite[j].z);
               }
               break;
          case 13:
               j = insertsprite(plr->sector, MISSILE);
               sprite[j].x = plr->x;
               sprite[j].y = plr->y;
               sprite[j].z = plr->z + (8 << 8);
               sprite[j].cstat = 0;// Hitscan does not hit other bullets
               sprite[j].picnum = MONSTERBALL;
               sprite[j].shade = -32;
               sprite[j].pal = 0;
               sprite[j].xrepeat = 64;
               sprite[j].yrepeat = 64;
               daang = sprite[j].ang = plr->ang;
               sprite[j].xvel = (sintable[(daang + 512) & 2047] >> 7);
               sprite[j].yvel = (sintable[daang] >> 7);
               sprite[j].zvel = ((100 - plr->horiz) << 4);
               sprite[j].owner = 4096 + p;
               sprite[j].lotag = 256;
               sprite[j].hitag = 0;
               sprite[j].clipdist = 64;
               dax = sprite[j].xvel;
               day = sprite[j].yvel;
               movesprite((short) j, (dax * synctics) << 3, (day * synctics) << 3, 0L,
                          4L << 8, 4L << 8, 0);
               setsprite(j, sprite[j].x, sprite[j].y, sprite[j].z);
               return;
          case 14:
               j = insertsprite(plr->sector, HEATSEEKER);
               sprite[j].x = plr->x;
               sprite[j].y = plr->y;
               sprite[j].z = plr->z + (8 << 8);
               sprite[j].cstat = 0;// Hitscan does not hit other bullets
               sprite[j].picnum = BULLET;
               sprite[j].shade = -32;
               sprite[j].pal = 0;
               sprite[j].xrepeat = 16;
               sprite[j].yrepeat = 16;
               daang = sprite[j].ang = plr->ang;
               sprite[j].xvel = (sintable[(daang + 512) & 2047] >> 7);
               sprite[j].yvel = (sintable[daang] >> 7);
               sprite[j].zvel = ((100 - plr->horiz) << 4);
               sprite[j].owner = 4096 + p;
               sprite[j].lotag = 1024;
               sprite[j].hitag = 0;
               sprite[j].clipdist = 128;
               dax = sprite[j].xvel;
               day = sprite[j].yvel;
               movesprite((short) j, (dax * synctics) << 3, (day * synctics) << 3, 0L,
                          4L << 8, 4L << 8, 0);
               setsprite(j, sprite[j].x, sprite[j].y, sprite[j].z);
               return;
          case 15:
               j = insertsprite(plr->sector, MISSILE);
               sprite[j].x = plr->x;
               sprite[j].y = plr->y;
               sprite[j].z = plr->z + (16 << 8);
               sprite[j].cstat = 21;
               sprite[j].picnum = THROWPIKE;
               sprite[j].ang = ((plr->ang + 2048 + 96) - 512) & 2047;
               sprite[j].xrepeat = 24;
               sprite[j].yrepeat = 24;
               sprite[j].clipdist = 24;
               sprite[j].extra = plr->ang;
               sprite[j].shade = -15;
               sprite[j].xvel = ((krand() & 256) - 128);
               sprite[j].yvel = ((krand() & 256) - 128);
               sprite[j].zvel = ((100 - plr->horiz) << 4);
               sprite[j].owner = 4096 + p;
               sprite[j].lotag = 1024;
               sprite[j].hitag = 0;
               sprite[j].pal = 0;
               playsound_loc(S_ARROWHIT, sprite[j].x, sprite[j].y);
               dax = sprite[j].xvel;
               day = sprite[j].yvel;
               movesprite((short) j, (dax * synctics) << 3, (day * synctics) << 3, 0L,
                          4L << 8, 4L << 8, 0);
               setsprite(j, sprite[j].x, sprite[j].y, sprite[j].z);
               return;
          }
     }
}

void
spawnflag(int x, int y, int z, int teamno)
{
     short s,
          sect;

// fprintf(stdaux, "spawnflag\r\n");
     updatesector(x, y, &sect);
     s = insertsprite(sect, 0);
     sprite[s].x = x;
     sprite[s].y = y;
     sprite[s].z = z;
     sprite[s].ang = 0;
     sprite[s].picnum = TEAMFLAG;
     sprite[s].cstat = 1 + 256;
     sprite[s].xrepeat = 32;
     sprite[s].yrepeat = 32;
     sprite[s].pal = teamcolor[teamno];
     teamflagsprite[teamno] = s;
}

void
dropflagstart(short teamno)
{
     if (teamno > 0) {
          teamno -= 1;
          if (teamflagsprite[teamno] >= 0) {
               setsprite(teamflagsprite[teamno], teamflagstartx[teamno],
                         teamflagstarty[teamno], teamflagstartz[teamno]);
          }
     }
}

void
netdropflag(void)
{
// fprintf(stdaux, "netdropflag\r\n");
     if (ihaveflag) {
          dropflag = 1;
     }
}

void
wongamescreen(short p)
{
     int  cnt = 0,
          i,
          n;
     int oth;
     char tmpbuf[32];
     int rc;

// fprintf(stdaux, "wongamescreen\r\n");

     keystatus[0x1C] = keystatus[0x01] = 0;
     SND_Sound(S_DROPFLAG);
     do {
          if (cnt < 5) {
               cnt++;
               netsendmove();
          }
          rc = 0;
          switch (netgame) {
          case 1:
               break;
          case 2:
          case 3:
               if ((i = getpacket(&oth, (void*)&mdmrcvpck)) == 0) {
                    break;
               }
               rc = 1;
               break;
          }
          if (rc == 1) {
               memset(&othnetpck, 0, sizeof(struct netpck));
               switch (netgame) {
               case 1:
                    break;
               case 2:
               case 3:
                    memmove(&othnetpck, &mdmrcvpck.data, sizeof(struct netpck));
                    break;
               }
          }
          switch (p) {
          case TEAM1FLAG - 1:      // red team
               if (svga) {
                    overwritesprite(0, 0, SREDKNIGHTS1, 0, 0, 0);
                    overwritesprite(0, 240, SREDKNIGHTS2, 0, 0, 0);
               }
               else {
                    overwritesprite(0, 0, REDKNIGHTS, 0, 0, 0);
               }
               break;
          case TEAM2FLAG - 1:      // blue team
               if (svga) {
                    overwritesprite(0, 0, SBLUEKNIGHTS1, 0, 0, 0);
                    overwritesprite(0, 240, SBLUEKNIGHTS2, 0, 0, 0);
               }
               else {
                    overwritesprite(0, 0, BLUEKNIGHTS, 0, 0, 0);
               }
               break;
          case TEAM3FLAG - 1:      // purple team
               if (svga) {
                    overwritesprite(0, 0, SPURPLEKNIGHTS1, 0, 0, 0);
                    overwritesprite(0, 240, SPURPLEKNIGHTS2, 0, 0, 0);
               }
               else {
                    overwritesprite(0, 0, PURPLEKNIGHTS, 0, 0, 0);
               }
               break;
          case TEAM4FLAG - 1:      // brown team
               if (svga) {
                    overwritesprite(0, 0, SBROWNKNIGHTS1, 0, 0, 0);
                    overwritesprite(0, 240, SBROWNKNIGHTS2, 0, 0, 0);
               }
               else {
                    overwritesprite(0, 0, BROWNKNIGHTS, 0, 0, 0);
               }
               break;
          }
          for (i = 0, n = 0; i < MAXTEAMS; i++) {
               if (teaminplay[i]) {
                    sprintf(tmpbuf, "%s: %d", teamcolorstrings[i], teamscore[i]);
                    printext256(xdim - ((int)strlen(tmpbuf) * 4),
                                ydim - (n * 8) - tilesizy[STATUSBAR] - 8, 31, -1, tmpbuf, 1);
                    n++;
               }
          }
          nextpage();
     } while (keystatus[0x1C] == 0 && keystatus[0x01] == 0);
     gameactivated = 0;
}

void
netgetmove(void)
{
     int rc;
     short etype,
          i,
          sect,
          s,
          teamno;
     int p;
     struct player *plr;

// fprintf(stdaux, "netgetmove\r\n");
     do {
          rc = 0;
          switch (netgame) {
          case 1:
               break;
          case 2:
          case 3:
               if ((i = getpacket(&p, (void*)&mdmrcvpck)) == 0) {
                    break;
               }
               rc = 1;
               break;
          }
          if (rc == 1) {
               memset(&othnetpck, 0, sizeof(struct netpck));
               switch (netgame) {
               case 1:
                    break;
               case 2:
               case 3:
                    memmove(&othnetpck, &mdmrcvpck.data, sizeof(struct netpck));
                    break;
               }
               if (othnetpck.mapon != mapon) {    // on a different map
                    if (debugnet) {
                         debugout("SKIPPED - NOT ON SAME MAP");
                    }
                    continue;
               }
               if (othnetpck.gametype != gametype) {   // playing a different
                                                       // game
                    if (debugnet) {
                         debugout("SKIPPED - NOT IN SAME GAME");
                    }
                    continue;
               }
#if 0
               if (debugnet) {
                    debugout("PCKT RECV FROM P: %02d TAG: %d",
                             plrindex[othnetpck.tagno], othnetpck.tagno);
               }
#endif
               if (othnetpck.tagno == myrandomtag) {
                    if (debugnet) {
                         debugout("SKIPPED - OWN TAG");
                    }
                    continue;
               }
               p = plrindex[othnetpck.tagno];
               if (p == 255) {     // new player identified
                    switch (netgame) {
                    case 1:
                         break;
                    case 2:
                    case 3:
                         mynode = 0;
                         p = plrindex[othnetpck.tagno] = playersingame++;
                         tagindex[p] = othnetpck.tagno;
                         break;
                    }
#ifdef DEBUGOUTPUT
                    debugout("PCKT: %06ld TAG: %03d P: %02d NEWPLAYER\n",
                             gethdr.seq, tagindex[plrindex[othnetpck.tagno]],
                             plrindex[othnetpck.tagno]);
                    sprintf(netmsg, "ADDING TAG %d", othnetpck.tagno);
#endif
// ccw               sendmyinfo(_NETNOW_BROADCAST,1);
                    sendmyinfo(-1, 0);
               }
               if (p < 0 || p >= MAXPLAYERS) {
//					fprintf(stdaux, "error = player index invalid p = %d\r\n", p);
                    crashgame("netgetmove: player index invalid (p=%d)", p);
               }
               if (xmit != 1) {
                    xmit = 1;
               }
               memmove(&netpck[p], &othnetpck, sizeof(struct netpck));
               if (netpck[p].action == -2) {
                    i = teamflagsprite[netpck[p].hasflag - 1];
                    if (i >= 0 && ihaveflag == netpck[p].hasflag) {
                         sprite[i].cstat &= ~32768;
                    }
                    if (netpck[p].hasflag - 1 == netpck[p].teamno) {
                         netmarkflag(i);
                    }
               }
               plr = &player[p];
               switch (netpck[p].weap) {
               case 1:
               case 2:
               case 3:
               case 4:
               case 8:
                    etype = 1;
                    break;
               case 5:
               case 6:
               case 7:
               case 9:
                    etype = 0;
                    break;
               default:
                    etype = 2;
                    break;
               }
               if (plr->spritenum <= 0) {
//                    etype=netpck[p].enemytype;
                    updatesector(netpck[p].x, netpck[p].y, &sect);
                    s = insertsprite(sect, 1);
                    sprite[s].x = netpck[p].x;
                    sprite[s].y = netpck[p].y;
                    sprite[s].z = netpck[p].z;
                    sprite[s].ang = netpck[p].ang;
                    sprite[s].picnum = enempics[etype];
                    sprite[s].cstat = 1 + 256;
                    sprite[s].xrepeat = xreps[etype];
                    sprite[s].yrepeat = yreps[etype];
                    sprite[s].owner = MAXSPRITES + p;
                    sprite[s].pal = teamcolor[netpck[p].teamno];
                    plr = &player[p];
                    plr->spritenum = s;
                    plr->x = sprite[s].x;
                    plr->y = sprite[s].y;
                    plr->z = sprite[s].z;
                    plr->ang = sprite[s].ang;
                    plr->height = PLAYERHEIGHT;
                    plr->sector = plr->oldsector = sprite[s].sectnum;
               }
               else {
//                    etype=netpck[p].enemytype;
                    if (gametype >= 1) {
                         teamno = netpck[p].teamno;
                    }
                    else {
                         teamno = plrindex[netpck[p].tagno];
                    }
                    plr->x = netpck[p].x;
                    plr->y = netpck[p].y;
                    plr->z = netpck[p].z;
                    plr->ang = netpck[p].ang;
                    plr->horiz = netpck[p].horiz;
                    if (plr->spritenum <= 0 || plr->spritenum >= MAXSPRITES) {
                         crashgame("netgetmove: player sprite number (%d) invalid",
                                 plr->spritenum);
                    }
                    sprite[plr->spritenum].ang = plr->ang;
                    sprite[plr->spritenum].cstat = 1 + 256;
                    sprite[plr->spritenum].xrepeat = xreps[etype];
                    sprite[plr->spritenum].yrepeat = yreps[etype];
                    sprite[plr->spritenum].pal = teamcolor[netpck[p].teamno];
                    setsprite(plr->spritenum, plr->x, plr->y, plr->z + (plr->height << 8));
                    updatesector(plr->x, plr->y, &plr->sector);
//                    plr->sector = sprite[plr->spritenum].sectnum;
                    if (plr->sector < 0 || plr->sector >= numsectors) {
                         crashgame("netgetmove (p=%d): sector number"
                                 "invalid (t=%d s=%d x=%d, y=%d, z=%d)",p,
                                 teamno,plr->sector,
                                 plr->x, plr->y, plr->z);
                    }
                    if (gametype >= 1) {
                         if (teamno < 0 || teamno >= MAXTEAMS) {
                              crashgame("netgetmove: invalid team number %d", teamno);
                         }
                         if (teaminplay[teamno] == 0) {
                              teaminplay[teamno] = 1;
                              captureflagpic();
                         }
                    }
                    if (teamscore[teamno] < netpck[p].score) {
                         teamscore[teamno] = netpck[p].score;
                         if (gametype >= 1) {
                              captureflagpic();
                         }
                         else {
                              fragspic();
                         }
                    }
                    if (gametype >= 1 && netpck[p].hasflag) {
                         if (netpck[p].hasflag < 1 || netpck[p].hasflag > MAXTEAMS) {
                              crashgame("netgetmove: flag referencing invalid team %d",
                                      netpck[p].hasflag);
                         }
                         if (ihaveflag < 0 || ihaveflag > MAXTEAMS) {
                              crashgame("netgetmove: ihaveflag (%d) invalid value",
                                      ihaveflag);
                         }
                         if (netpck[p].hasflag == ihaveflag) {
                              if (teamflagsprite[ihaveflag - 1] >= 0) {
                                   playsound_loc(S_LAUGH, netpck[p].x, netpck[p].y);
                                   sprite[teamflagsprite[ihaveflag - 1]].cstat &= ~32768;
                              }
                              ihaveflag = 0;
                         }
                         moveflag(plr->x, plr->y, plr->z, netpck[p].hasflag);
                         switch (sector[plr->sector].lotag) {
                         case TEAM1SECTOR:
                              switch (netpck[p].hasflag) {
                              case TEAM2FLAG:
                              case TEAM3FLAG:
                              case TEAM4FLAG:
                                   if (teaminplay[netpck[p].hasflag - 1]) {
                                        teamscore[TEAM1FLAG - 1] += 1;
                                        playsound_loc(S_DROPFLAG, netpck[p].x, netpck[p].y);
//                                        if (teamscore[netpck[p].hasflag-1] > 0) {
//                                             teamscore[netpck[p].hasflag-1]--;
//                                        }
                                   }
                                   break;
                              }
                              dropflagstart(netpck[p].hasflag);
                              captureflagpic();
                              break;
                         case TEAM2SECTOR:
                              switch (netpck[p].hasflag) {
                              case TEAM1FLAG:
                              case TEAM3FLAG:
                              case TEAM4FLAG:
                                   if (teaminplay[netpck[p].hasflag - 1]) {
                                        teamscore[TEAM2FLAG - 1] += 1;
                                        playsound_loc(S_DROPFLAG, netpck[p].x, netpck[p].y);
//                                        if (teamscore[netpck[p].hasflag-1] > 0) {
//                                             teamscore[netpck[p].hasflag-1]--;
//                                        }
                                   }
                                   break;
                              }
                              dropflagstart(netpck[p].hasflag);
                              captureflagpic();
                              break;
                         case TEAM3SECTOR:
                              switch (netpck[p].hasflag) {
                              case TEAM1FLAG:
                              case TEAM2FLAG:
                              case TEAM4FLAG:
                                   if (teaminplay[netpck[p].hasflag - 1]) {
                                        teamscore[TEAM3FLAG - 1] += 1;
                                        playsound_loc(S_DROPFLAG, netpck[p].x, netpck[p].y);
//                                        if (teamscore[netpck[p].hasflag-1] > 0) {
//                                             teamscore[netpck[p].hasflag-1]--;
//                                        }
                                   }
                                   break;
                              }
                              dropflagstart(netpck[p].hasflag);
                              captureflagpic();
                              break;
                         case TEAM4SECTOR:
                              switch (netpck[p].hasflag) {
                              case TEAM1FLAG:
                              case TEAM2FLAG:
                              case TEAM3FLAG:
                                   if (teaminplay[netpck[p].hasflag - 1]) {
                                        teamscore[TEAM4FLAG - 1] += 1;
                                        playsound_loc(S_DROPFLAG, netpck[p].x, netpck[p].y);
//                                        if (teamscore[netpck[p].hasflag-1] > 0) {
//                                             teamscore[netpck[p].hasflag-1]--;
//                                        }
                                   }
                                   break;
                              }
                              dropflagstart(netpck[p].hasflag);
                              captureflagpic();
                              break;
                         }
                    }
               }
               switch (netpck[p].action) {
               case KEYFIRE:
                    if (sprite[plr->spritenum].statnum != ATTACK) {
                         newstatus(plr->spritenum, ATTACK);
                    }
                    if (netpck[p].guntype != 0) {
                         nethitsprite(p, netpck[p].guntype, netpck[p].taghit);
                         netpck[p].taghit = 0;
                         netpck[p].guntype = 0;
                    }
                    break;
               case -1:
                    if (sprite[plr->spritenum].statnum != DIE) {
                         newstatus(plr->spritenum, DIE);
                         if (netpck[p].taghit == myrandomtag) {
                              if (gametype < 1) {
                                   teamscore[0]++;
                                   fragspic();
                              }
                         }
                         if (playersingame == 2) {
                              xmit = 0;
                         }
                    }
                    break;
               default:
                    sprite[plr->spritenum].picnum = enempics[etype];
                    if (sprite[plr->spritenum].statnum != 1) {
                         changespritestat(plr->spritenum, 1);
                    }
                    break;
               }
               numspcks[p]++;
               totrpcks++;
          }
     } while (rc != 0);
     for (i = 0; i < MAXTEAMS; i++) {
          switch (gametype) {
          case 0:                  // head to head
               break;
          case 1:                  // capture the flag 21
               if (teamscore[i] >= 21) {
                    wongamescreen(i);
                    netpickmonster();
               }
               break;
          case 2:                  // capture the flag 7
               if (teamscore[i] >= 7) {
                    wongamescreen(i);
                    netpickmonster();
               }
               break;
          }
     }
}

void
netpickmonster(void)
{
     int  animpic,
          i,
          more = 1,
          numenemchoices,
          numgamechoices,
          nummapchoices,
          pal,
          picking,
          selected,
          step;
     int animclock = 0L;
     char tmpbuf[32];
     struct player *plr;

// fprintf(stdaux, "netpickmonster\r\n");

     animpic = picking = selected = 0;
     keystatus[0x1C] = 0;
     numenemchoices = MAXTEAMS;
     numgamechoices = NUMGAMES;
     nummapchoices = sizeof(mapnumbers) / sizeof(short);
     animclock = totalclock + (CLKIPS >> 3);
     keystatus[0x1C] = keystatus[0x01] = 0;
     do {
          if (svga) {
               rotatesprite(0L << 16, 0L << 16, 65536L, 0, VMAINBLANK,
                            0, 0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
          }
          else {
               rotatesprite(0L << 16, 0L << 16, 65536L, 0, 1028,
                            0, 0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
          }
          switch (picking) {
          case 0:
               for (i = 0; i < numenemchoices; i++) {
                    if (i == selected) {
                         rotatesprite(((xdim>>1)-135)<<16,
                                      (((ydim>>1)-72)+(i * 19))<<16,
                                      16384L, 0, HELMET + step,
                                      0, 0, 8 + 16,
                                      0L, 0L, xdim-1L, ydim-1L);
                    }
                    pal = 3;
                    strcpy(tmpbuf, monsternames[i]);
                    fancyfont((xdim >> 1) - 100, (ydim >> 1) - 69 + (i * 19),
                              THEFONT, tmpbuf, pal);
               }
               if (totalclock >= animclock) {
                    if (selected == 9) {
                         animpic = (animpic + 1) & 15;
                    }
                    else {
                         animpic = (animpic + 1) & 3;
                    }
                    step = (step + 1) & 7;
                    animclock = totalclock + (CLKIPS >> 3);
               }
               overwritesprite((xdim >> 1) + 35, 10,
                               enempics[0] + animpic,
                               0, 0, teamcolor[selected]);
               if (keystatus[0x1C]) {
                    keystatus[0x1C] = 0;
                    enemytype = selected;
                    myteam = selected;
                    selected = 0;

                         picking = 1;
               }
               else if (keystatus[0x01]) {
                    keystatus[0x01] = 0;
                    shutdowngame();
               }
               break;
          case 1:
               for (i = 0; i < numgamechoices; i++) {
                    if (i == selected) {
                         rotatesprite(((xdim>>1)-135)<<16,
                                      (((ydim>>1)-72)+(i * 19))<<16,
                                      16384L, 0, HELMET + step,
                                      0, 0, 8 + 16,
                                      0L, 0L, xdim-1L, ydim-1L);
                    }
                    pal = 3;
                    strcpy(tmpbuf, gamenames[i]);
                    fancyfont((xdim >> 1) - 100, (ydim >> 1) - 69 + (i * 19),
                              THEFONT, tmpbuf, pal);
               }
               if (totalclock >= animclock) {
                    step = (step + 1) & 7;
                    animclock = totalclock + (CLKIPS >> 3);
               }
               if (keystatus[0x1C]) {
                    keystatus[0x1C] = 0;
                    gametype = selected;
                    selected = 0;
                    picking = 2;
               }
               else if (keystatus[0x01]) {
                    keystatus[0x01] = 0;
                    selected = 0;
                    picking = 0;
               }
               break;
          case 2:
               for (i = 0; i < nummapchoices; i++) {
                    if (i == selected) {
                         rotatesprite(((xdim>>1)-152)<<16,
                                      (((ydim>>1)-72)+(i * 19))<<16,
                                      16384L, 0, HELMET + step,
                                      0, 0, 8 + 16,
                                      0L, 0L, xdim-1L, ydim-1L);
                    }
                    pal = 3;
                    strcpy(tmpbuf, mapnames[i]);
                    fancyfont((xdim >> 1) - 130, (ydim >> 1) - 69 + (i * 19),
                              THEFONT, tmpbuf, pal);
               }
               if (totalclock >= animclock) {
                    step = (step + 1) & 7;
                    animclock = totalclock + (CLKIPS >> 3);
               }
               if (keystatus[0x1C]) {
                    keystatus[0x1C] = 0;
                    mapon = mapnumbers[selected];
                    more = 0;
               }
               else if (keystatus[0x01]) {
                    keystatus[0x01] = 0;
                    selected = 0;
                    picking = 1;
               }
               break;
          }
          nextpage();
          if (keystatus[keys[KEYBACK]] != 0) {
               keystatus[keys[KEYBACK]] = 0;
               selected++;
               switch (picking) {
               case 0:
                    if (selected >= numenemchoices) {
                         selected = 0;
                    }
                    break;
               case 1:
                    if (selected >= numgamechoices) {
                         selected = 0;
                    }
                    break;
               case 2:
                    if (selected >= nummapchoices) {
                         selected = 0;
                    }
                    break;
               }
          }
          else if (keystatus[keys[KEYFWD]] != 0) {
               keystatus[keys[KEYFWD]] = 0;
               selected--;
               switch (picking) {
               case 0:
                    if (selected < 0) {
                         selected = numenemchoices - 1;
                    }
                    break;
               case 1:
                    if (selected < 0) {
                         selected = numgamechoices - 1;
                    }
                    break;
               case 2:
                    if (selected < 0) {
                         selected = nummapchoices - 1;
                    }
                    break;
               }
          }
     } while (more);
     for (i = 0; i < MAXSECTORS; i++) {
          // memset(&sector[i],0,sizeof(struct sectortype));
          memset(&sector[i], 0, sizeof(sectortype));
     }
     for (i = 0; i < MAXWALLS; i++) {
          // memset(&wall[i],0,sizeof(struct walltype));
          memset(&wall[i], 0, sizeof(walltype));
     }
     for (i = 0; i < MAXSPRITES; i++) {
          // memset(&sprite[i],0,sizeof(struct spritetype));
          memset(&sprite[i], 0, sizeof(spritetype));
     }
     for (i = 0; i < MAXPLAYERS; i++) {
          memset(&player[i], 0, sizeof(struct player));
     }
     for (i = 0; i < MAXPLAYERS; i++) {
          teaminplay[i] = 0;
          teamscore[i] = 0;
     }
     plr = &player[pyrn];
     plr->oldsector = plr->sector;
     plr->horiz = 100;
     plr->zoom = 256;
     if (svga == 1) {
          plr->screensize = 328;
     }
     else {
          plr->screensize = 320;
     }
     plr->dimension = 3;
     plr->height = PLAYERHEIGHT;
     plr->z = sector[plr->sector].floorz - (plr->height << 8);

     loadnewlevel(mapon);
     mapflag = 1;
     if (plr->spritenum <= 0) {
          initplayersprite();
     }
     setup3dscreen();
     netjoingame();
     netstartx = player[pyrn].x;
     netstarty = player[pyrn].y;
     netstartz = player[pyrn].z;
     netstarts = player[pyrn].sector;
     gameactivated = 1;
     updatepics();
}

#define   NUMSETOPTS          2

char *setopts[] = {
     "COM PORT:",
     "BAUD RATE:"
};

void
mdmreadsettings(void)
{
     short i;
     int n, l;
     char buf[80],
         *ptr;
     FILE *fp;

// fprintf(stdaux, "mdmreadsettings\r\n");
     fp = fopen("modem.dat", "r");
     if (fp == NULL) {
          return;
     }
     while (fgets(buf, 80, fp) != NULL) {
          for (i = 0; i < NUMSETOPTS; i++) {
               if ((ptr = strstr(buf, setopts[i])) != NULL) {
                    ptr += strlen(setopts[i]);
                    switch (i) {
                    case 0:        // com port
                         sscanf(ptr, "%d", &n);
                         if (n < 1 || n > 4) {
                              break;
                         }
                         comp = n;
                         break;
                    case 1:        // baud rate
                         sscanf(ptr, "%d", &l);
                         if (l < 2400L || l > 115200L) {
                              break;
                         }
                         bps = l;
                         break;
                    }
               }
          }
     }
     fclose(fp);
}

char comtable[] = {0x00, 0x01, 0x02, 0x03, 0x04};

void
initmulti(int numplayers)
{
     int  i;
     FILE *fp;

// fprintf(stdaux, "initmulti\r\n");
//     installGPhandler();
     fp = fopen("tag.dat", "rb");
     if (fp != NULL) {
          fread(&myrandomtag, sizeof(myrandomtag), 1, fp);
          fclose(fp);
     }
#ifdef DEBUGOUTPUT
     dbgfp = fopen("debug.out", "w");
#endif
     if (numplayers > MAXPLAYERS) {
          numplayers = MAXPLAYERS;
     }
     switch (netgame) {
     case 1:
          break;
     case 2:
     case 3:
          mdmsndpck.hdr[0] = 0x03;
          mdmsndpck.hdr[1] = 0x02;
          mdmsndpck.hdr[2] = 0x01;
          mdmsndpck.hdr[3] = 0x21;
          mdmsndpck.len = sizeof(struct netpck);
          mdmreadsettings();
          initmultiplayers(0, NULL);
          netinitialized = 1;
          break;
     }
     for (i = 0; i < 256; i++) {
          plrindex[i] = 255;
     }
     for (i = 0; i < MAXTEAMS; i++) {
          teamflagsprite[i] = -1;
     }
     netpickmonster();
}

void
netshutdown(void)
{
// fprintf(stdaux, "netshutdown\r\n");
     if (netinitialized) {
          switch (netgame) {
 case 1:
               break;
          case 2:
          case 3:
               uninitmultiplayers();
               break;
          }
     }
#ifdef DEBUGOUTPUT
     if (dbgfp != NULL) {
          fclose(dbgfp);
     }
#endif
}

void
whnetmon(void)
{
     int  i,
          n;
     int loz;
     char tmpbuf[80];

// fprintf(stdaux, "whnetmon\r\n");

     if (debugnet) {
//	 {
          sprintf(tmpbuf, "P: %02d S: %05d R: %05d HDR: %05d "
                  "N: %02d T:%03d SPR: %04d %s",
             playersingame, totspcks, totrpcks, totrhdrs, mynode, myrandomtag,
                  player[pyrn].spritenum, netmsg);
          memset(netmsg, 0, sizeof(netmsg));
          printext256(0L, 8L, 31, -1, tmpbuf, 1);
          for (i = 0; i < playersingame; i++) {
               n = plrindex[tagindex[i]];
               sprintf(tmpbuf, "P: %02d T: %03d X: %05d Y: %05d Z: %05d "
                       "M: %02d AN: %05d F: %02d SPR: %04d",
                       n, tagindex[i],
                       player[n].x, player[n].y, player[n].z,
                       netpck[n].mapon, player[n].ang, netpck[n].hasflag,
                       player[n].spritenum);
               printext256(0L, 16L + (i * 8), 31, -1, tmpbuf, 1);
          }
          for (i = 0; i < MAXTEAMS; i++) {
               if (teamflagsprite[i] == -1) {
                    continue;
               }
               sprintf(tmpbuf, "F: %03d S: %d X: %05d Y: %05d Z: %05d",
                       i, teamflagsprite[i],
                     sprite[teamflagsprite[i]].x, sprite[teamflagsprite[i]].y,
                       sprite[teamflagsprite[i]].z);
               printext256(0L, 16L + (playersingame * 8) + (i * 8), 31, -1, tmpbuf, 1);
          }
          for (i = 0, n = 0; i < MAXTEAMS; i++) {
               if (teaminplay[i]) {
                    sprintf(tmpbuf, "%s: %d", teamcolorstrings[i], teamscore[i]);
                    printext256(xdim - ((int)strlen(tmpbuf) * 4),
                                ydim - (n * 8) - tilesizy[STATUSBAR] - 8, 31, -1, tmpbuf, 1);
                    n++;
               }
          }
     }
     if (gametype >= 1) {
          if (svga) {
               overwritesprite(600, 48, THEFLAG, 0, 0x02, teamcolor[myteam]);
          }
          else {
               overwritesprite(xdim - tilesizx[THEFLAG], (ydim - 40), THEFLAG,
                               0, 0x02, teamcolor[myteam]);
          }
          if (ihaveflag) {
               if (svga) {
                    rotatesprite(28L << 16, 48L << 16, 16384L, 0, TEAMFLAG, 0,
                                 teamcolor[ihaveflag - 1], 8 + 16,
                                 0L, 0L, xdim - 1L, ydim - 1L);
               }
               else {
                    rotatesprite(8L << 16, 48L << 16, 16384L, 0, TEAMFLAG, 0,
                                 teamcolor[ihaveflag - 1], 8 + 16,
                                 0L, 0L, xdim - 1L, ydim - 1L);
               }
          }
          for (n = 0; n < MAXTEAMS; n++) {
               i = teamflagsprite[n];
               if (i >= 0) {
                    loz = sector[sprite[i].sectnum].floorz;
                    for (i = 0; i < playersingame; i++) {
                         if (netpck[i].hasflag == n + 1) {
                              break;
                         }
                    }
                    if (i == playersingame) {
                         i = teamflagsprite[n];
                         if (sprite[i].z < loz) {
                              sprite[i].zvel += GRAVITYCONSTANT;
                         }
                         sprite[i].z += sprite[i].zvel;
                         if (sprite[i].z > loz) {
                              sprite[i].zvel = -(sprite[i].zvel >> 2);
                              sprite[i].z = loz;
                         }
                    }
               }
          }
     }
     if (netgame == 2) {
         /*
          if ((inp(comadr[comp] + 6) & 0x80) == 0) {
               sprintf(tmpbuf, "NO CONNECTION");
               printext256((xdim >> 1) - (strlen(tmpbuf) << 2), (ydim >> 1), 31, -1, tmpbuf, 1);
          }
          */
     }
     if (escapetomenu) {
          if (keystatus[0x15]) {
               shutdowngame();
          }
          else if (keystatus[0x31]) {
               keystatus[0x31] = 0;
               escapetomenu = 0;
          }
          sprintf(tmpbuf, "QUIT GAME (Y/N)?");
          printext256(0L, 0L, 31, -1, tmpbuf, 0);
     }
}

void
netrestartplayer(struct player * plr)
{
     plr->x = netstartx;
     plr->y = netstarty;
     plr->z = netstartz;
     plr->sector = netstarts;

// fprintf(stdaux, "netstartplayer %d %d %d %d\r\n", plr->x, plr->y, plr->z, plr->sector);
}

void
netkillme(void)
{
// fprintf(stdaux, "netkillme\r\n");
     playerdie = 1;
     netsendmove();
}

void
netreviveme(void)
{
// fprintf(stdaux, "netreviveme\r\n");
     playerdie = 0;
}
