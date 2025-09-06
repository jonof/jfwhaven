/***************************************************************************
 *   WHPLR.C  - code for player character                                  *
 *                                                                         *
 ***************************************************************************/

#include "icorp.h"

//#define   WHDEMO

extern short gametype;


int spelltime;
//extern int poisoned;
//extern int poisontime;
extern int vampiretime;

extern int loadedgame;
extern int goreon;
extern int helmettime;
extern int scoretime;
extern int difficulty;
int lives;
int madeahit=0;
extern int lavasnd;
extern int mapflag;

int  pyrn;
int  dahand=0;
int  mapon=1;

int neartagdist,
	 neartaghitdist;

short neartagsector,
	 neartagsprite,
	 neartagwall;

struct player player[MAXPLAYERS];

extern int justteleported;
extern int selectedgun;
extern short oldmousestatus;

extern char tempbuf[];
extern char displaybuf[];
extern char scorebuf[];
extern char healthbuf[];
extern char armorbuf[];
extern char potionbuf[];

extern int displaytime;

extern int potiontilenum;
extern int shadowtime;

extern int playerdie;

int hasshot;
int orbshot;

int weapondrop;
int weapondropgoal;
int weaponraise;

int currweapon;
int currweapontics;
int currweaponanim;
int currweaponframe;
int currweaponfired;
int currweaponattackstyle;
int currweaponflip;

int spellbooktics;
int spellbook;
int spellbookframe;
int spellbookflip;

int spiketics;
int spikeframe;
int currspikeframe;
int spiked;


int strongtime,
	invincibletime,
	manatime,
	invisibletime=-1,
	nightglowtime;

#define MAXFRAMES 12

struct daweapons {
   int daweapontics;
   int daweaponframe;
   int currx;
   int curry;
};

struct daweapons spikeanimtics[5] =
{ {10,DIESPIKE,136,145},
  {10,DIESPIKE+1,136,124},
  {10,DIESPIKE+2,136,100},
  {10,DIESPIKE+3,136,70},
  {10,DIESPIKE+4,136,50} };

//
struct daweapons spellbookanim[MAXNUMORBS][9] =
{
  // SCARE
  { {8,SPELLBOOK8,121,161},{8,SPELLBOOK8+1,121,161},
	{8,SPELLBOOK8+2,121,156},{8,SPELLBOOK8+3,121,158},
	{8,SPELLBOOK8+4,121,159},{8,SPELLBOOK8+5,121,161},
	{8,SPELLBOOK8+6,121,160},{8,SPELLBOOK8+7,121,161},
	{8,SPELLBOOK8+7,121,161}
  },
  // NIGHT VISION
  { {8,SPELLBOOK6,121,161},{8,SPELLBOOK6+1,121,161},
	{8,SPELLBOOK6+2,121,156},{8,SPELLBOOK6+3,121,158},
	{8,SPELLBOOK6+4,121,159},{8,SPELLBOOK6+5,121,161},
	{8,SPELLBOOK6+6,121,160},{8,SPELLBOOK6+7,121,161},
	{8,SPELLBOOK6+7,121,161}
  },
  // FREEZE
  { {8,SPELLBOOK3,121,161},{8,SPELLBOOK3+1,121,161},
	{8,SPELLBOOK3+2,121,156},{8,SPELLBOOK3+3,121,158},
	{8,SPELLBOOK3+4,121,159},{8,SPELLBOOK3+5,121,161},
	{8,SPELLBOOK3+6,121,160},{8,SPELLBOOK3+7,121,161},
	{8,SPELLBOOK3+7,121,161}
  },
  // MAGIC ARROW
  { {8,SPELLBOOKBLANK,121,161},{8,SPELLBOOKBLANK+1,121,161},
	{8,SPELLBOOKBLANK+2,121,156},{8,SPELLBOOKBLANK+3,121,158},
	{8,SPELLBOOKBLANK+4,121,159},{8,SPELLBOOKBLANK+5,121,161},
	{8,SPELLBOOKBLANK+6,121,160},{8,SPELLBOOKBLANK+7,121,161},	//JonoF: frame 6 of the art aligns the page but the arrow is off
	{8,SPELLBOOKBLANK+7,121,161}
  },
  // OPEN DOORS
  { {8,SPELLBOOK7,121,161},{8,SPELLBOOK7+1,121,161},
	{8,SPELLBOOK7+2,121,156},{8,SPELLBOOK7+3,121,158},
	{8,SPELLBOOK7+4,121,159},{8,SPELLBOOK7+5,121,161},
	{8,SPELLBOOK7+6,121,160},{8,SPELLBOOK7+7,121,161},
	{8,SPELLBOOK7+7,121,161}
  },
  // FLY
  { {8,SPELLBOOK2,121,161},{8,SPELLBOOK2+1,121,161},
	{8,SPELLBOOK2+2,121,156},{8,SPELLBOOK2+3,121,158},
	{8,SPELLBOOK2+4,121,159},{8,SPELLBOOK2+5,121,161},
	{8,SPELLBOOK2+6,121,160},{8,SPELLBOOK2+7,121,161},
	{8,SPELLBOOK2+7,121,161}
  },
  // FIRE BALL
  { {8,SPELLBOOK4,121,161},{8,SPELLBOOK4+1,121,161},
	{8,SPELLBOOK4+2,121,156},{8,SPELLBOOK4+3,121,158},
	{8,SPELLBOOK4+4,121,159},{8,SPELLBOOK4+5,121,161},
	{8,SPELLBOOK4+6,121,160},{8,SPELLBOOK4+7,121,161},
	{8,SPELLBOOK4+7,121,161}
  },
  // NUKE!
  { {8,SPELLBOOK5,121,161},{8,SPELLBOOK5+1,121,161},
	{8,SPELLBOOK5+2,121,156},{8,SPELLBOOK5+3,121,158},
	{8,SPELLBOOK5+4,121,159},{8,SPELLBOOK5+5,121,161},
	{8,SPELLBOOK5+6,121,160},{8,SPELLBOOK5+7,121,161},
	{8,SPELLBOOK5+7,121,161}
  }

};

struct daweapons sspellbookanim[MAXNUMORBS][9] =
{
  // SCARE
  { {8,SSPELLBOOK8,121,389},{8,SSPELLBOOK8+1,121,377},
	{8,SSPELLBOOK8+2,121,383},{8,SSPELLBOOK8+3,121,385},
	{8,SSPELLBOOK8+4,121,389},{8,SSPELLBOOK8+5,121,387},
	{8,SSPELLBOOK8+6,121,389},{8,SSPELLBOOK8+7,121,389},
	{8,SSPELLBOOK8+7,121,389}
  },
  // NIGHT VISION
  { {8,SSPELLBOOK6,121,389},{8,SSPELLBOOK6+1,121,377},
	{8,SSPELLBOOK6+2,121,383},{8,SSPELLBOOK6+3,121,385},
	{8,SSPELLBOOK6+4,121,389},{8,SSPELLBOOK6+5,121,387},
	{8,SSPELLBOOK6+6,121,389},{8,SSPELLBOOK6+7,121,389},
	{8,SSPELLBOOK6+7,121,389}
  },
  // FREEZE
  { {8,SSPELLBOOK3,121,389},{8,SSPELLBOOK3+1,121,377},
	{8,SSPELLBOOK3+2,121,383},{8,SSPELLBOOK3+3,121,385},
	{8,SSPELLBOOK3+4,121,389},{8,SSPELLBOOK3+5,120,387},
	{8,SSPELLBOOK3+6,120,389},{8,SSPELLBOOK3+7,120,389},
	{8,SSPELLBOOK3+7,121,389}
  },
  // MAGIC ARROW
  { {8,SSPELLBOOKBLANK,121,389},{8,SSPELLBOOKBLANK+1,121,377},
	{8,SSPELLBOOKBLANK+2,121,383},{8,SSPELLBOOKBLANK+3,121,385},
	{8,SSPELLBOOKBLANK+4,121,389},{8,SSPELLBOOKBLANK+5,121,387},
	{8,SSPELLBOOKBLANK+6,120,389},{8,SSPELLBOOKBLANK+7,121,389},
	{8,SSPELLBOOKBLANK+7,122,389}
  },
  // OPEN DOORS
  { {8,SSPELLBOOK7,121,389},{8,SSPELLBOOK7+1,121,377},
	{8,SSPELLBOOK7+2,121,383},{8,SSPELLBOOK7+3,121,385},
	{8,SSPELLBOOK7+4,121,389},{8,SSPELLBOOK7+5,121,387},
	{8,SSPELLBOOK7+6,121,389},{8,SSPELLBOOK7+7,121,389},
	{8,SSPELLBOOK7+7,121,389}
  },
  // FLY
  { {8,SSPELLBOOK2,121,389},{8,SSPELLBOOK2+1,121,377},
	{8,SSPELLBOOK2+2,121,383},{8,SSPELLBOOK2+3,121,385},
	{8,SSPELLBOOK2+4,121,389},{8,SSPELLBOOK2+5,121,387},
	{8,SSPELLBOOK2+6,121,389},{8,SSPELLBOOK2+7,121,389},
	{8,SSPELLBOOK2+7,121,389}
  },
  // FIRE BALL
  { {8,SSPELLBOOK4,121,389},{8,SSPELLBOOK4+1,121,377},
	{8,SSPELLBOOK4+2,121,383},{8,SSPELLBOOK4+3,121,385},
	{8,SSPELLBOOK4+4,121,389},{8,SSPELLBOOK4+5,121,387},
	{8,SSPELLBOOK4+6,121,389},{8,SSPELLBOOK4+6,121,389},
	{8,SSPELLBOOK4+6,121,389}
  },
  // NUKE!
  { {8,SSPELLBOOK5,121,389},{8,SSPELLBOOK5+1,121,377},
	{8,SSPELLBOOK5+2,121,383},{8,SSPELLBOOK5+3,121,385},
	{8,SSPELLBOOK5+4,121,389},{8,SSPELLBOOK5+5,121,387},
	{8,SSPELLBOOK5+6,121,389},{8,SSPELLBOOK5+6,121,389},
	{8,SSPELLBOOK5+6,121,389}
  }

};


struct daweapons throwanimtics[MAXNUMORBS][MAXFRAMES+1] =
{
  // MUTWOHANDS
  { {10,MUTWOHANDS,19,155},{10,MUTWOHANDS+1,0,128},{10,MUTWOHANDS+2,0,93},
	{10,MUTWOHANDS+3,0,83},{10,MUTWOHANDS+4,0,72},{10,MUTWOHANDS+5,0,83},
	{10,MUTWOHANDS+6,10,96},{10,MUTWOHANDS+7,43,109},{10,MUTWOHANDS+8,69,113},
	{10,MUTWOHANDS+9,65,115},{10,MUTWOHANDS+10,64,117},{10,MUTWOHANDS+11,63,117},
	{1,0,127,170}
  },
  // MUMEDUSA
  { {10,MUMEDUSA,0,177},{10,MUMEDUSA+1,0,137},{10,MUMEDUSA+2,48,82},
	{10,MUMEDUSA+3,127,41},{10,MUMEDUSA+4,210,9},{10,MUMEDUSA+5,284,26},
	{10,MUMEDUSA+6,213,63},{10,MUMEDUSA+7,147,99},{10,MUMEDUSA+8,91,136},
	{10,MUMEDUSA+9,46,183},{1,0,127,170},{1,0,127,170},
	{1,0,127,170}
  },
  // BMUTWOHANDS
  { {10,MUTWOHANDS,19,155},{10,MUTWOHANDS+1,0,128},{10,MUTWOHANDS+2,0,93},
	{10,MUTWOHANDS+3,0,83},{10,BMUTWOHANDS,0,74},{10,BMUTWOHANDS+1,0,97},
	{10,BMUTWOHANDS+2,10,109},{10,BMUTWOHANDS+3,43,113},{10,BMUTWOHANDS+4,69,115},
	{10,BMUTWOHANDS+5,65,117},{10,BMUTWOHANDS+6,64,117},{10,BMUTWOHANDS+7,63,117},
	{1,0,127,170}
  },
  // MUTWOHANDS
  { {10,MUTWOHANDS,19,155},{10,MUTWOHANDS+1,0,128},{10,MUTWOHANDS+2,0,93},
	{10,MUTWOHANDS+3,0,83},{10,MUTWOHANDS+4,0,72},{10,MUTWOHANDS+5,0,83},
	{10,MUTWOHANDS+6,10,96},{10,MUTWOHANDS+7,43,109},{10,MUTWOHANDS+8,69,113},
	{10,MUTWOHANDS+9,65,115},{10,MUTWOHANDS+10,64,117},{10,MUTWOHANDS+11,63,117},
	{1,0,127,170}
  },
  // MUTWOHANDS
  { {15,MUTWOHANDS,19,155},{15,MUTWOHANDS+1,0,128},{15,MUTWOHANDS+2,0,93},
	{15,MUTWOHANDS+3,0,83},{15,MUTWOHANDS+4,0,72},{15,MUTWOHANDS+5,0,83},
	{15,MUTWOHANDS+6,10,96},{15,MUTWOHANDS+7,43,109},{15,MUTWOHANDS+8,69,113},
	{15,MUTWOHANDS+9,65,115},{15,MUTWOHANDS+10,64,117},{15,MUTWOHANDS+11,63,117},
	{1,0,127,170}
  },
  // MUMEDUSA
  { {10,MUMEDUSA,0,177},{10,MUMEDUSA+1,0,137},{10,MUMEDUSA+2,48,82},
	{10,MUMEDUSA+3,127,41},{10,MUMEDUSA+4,210,9},{10,MUMEDUSA+5,284,26},
	{10,MUMEDUSA+6,213,63},{10,MUMEDUSA+7,147,99},{10,MUMEDUSA+8,91,136},
	{10,MUMEDUSA+9,46,183},{1,0,127,170},{1,0,127,170},
	{1,0,127,170}
  },
  // MUTWOHANDS
  { {10,MUTWOHANDS,19,155},{10,MUTWOHANDS+1,0,128},{10,MUTWOHANDS+2,0,93},
	{10,MUTWOHANDS+3,0,83},{10,MUTWOHANDS+4,0,72},{10,MUTWOHANDS+5,0,83},
	{10,MUTWOHANDS+6,10,96},{10,MUTWOHANDS+7,43,109},{10,MUTWOHANDS+8,69,113},
	{10,MUTWOHANDS+9,65,115},{10,MUTWOHANDS+10,64,117},{10,MUTWOHANDS+11,63,117},
	{1,0,127,170}
  },
  // MUTWOHANDS
  { {10,MUTWOHANDS,19,155},{10,MUTWOHANDS+1,0,128},{10,MUTWOHANDS+2,0,93},
	{10,MUTWOHANDS+3,0,83},{10,MUTWOHANDS+4,0,72},{10,MUTWOHANDS+5,0,83},
	{10,MUTWOHANDS+6,10,96},{10,MUTWOHANDS+7,43,109},{10,MUTWOHANDS+8,69,113},
	{10,MUTWOHANDS+9,65,115},{10,MUTWOHANDS+10,64,117},{10,MUTWOHANDS+11,63,117},
	{1,0,127,170}
  }

};

struct daweapons lefthandanimtics[5][MAXFRAMES] =
{
	{ { 10,RFIST,15,121 },{10,RFIST+1,17,114},{10,RFIST+2,54,131},
	  { 10,RFIST+3,76,152 },{10,RFIST+4,31,126},{10,RFIST+5,26,135},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76}
	},
	// KNIFE
	{ { 8,KNIFEATTACK2,0,113 },{8,KNIFEATTACK2+1,44,111},{8,KNIFEATTACK2+2,119,137},
	  { 8,KNIFEATTACK2+3,187,159},{16,0,136,100},{8,KNIFEATTACK2+3,187,159},
	  { 8,KNIFEATTACK2+2,119,137},{8,KNIFEATTACK2+1,44,111},{8,KNIFEATTACK2,0,113},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76}
	},
	// GOBLINATTACK
	{ {10,GOBSWORDATTACK,243,92},{10,GOBSWORDATTACK+1,255,68 },{10,GOBSWORDATTACK+2,279,65},
	  {10,GOBSWORDATTACK+3,238,55},{10,GOBSWORDATTACK+4,153,52 },{10,GOBSWORDATTACK+5,129,152},
	  {10,GOBSWORDATTACK+6,90,184},{ 1,0,297,169 },{1,0,275,24},
	  {1,0,275,24 },{ 1,0,275,24 },{1,0,275,24}
	},
	// MORNINGATTACK2
	{ { 12,MORNINGATTACK2,38,142 },{12,MORNINGATTACK2+1,0,111},{12,MORNINGATTACK2+2,0,91},
	  { 12,MORNINGATTACK2+3,0,47 },{12,MORNINGATTACK2+4,0,24},{1,0,0,24},
	  { 1,0,0,24 },{1,0,0,24},{1,0,0,24},
	  { 1,0,0,24 },{1,0,0,24},{1,0,0,24}
	},
	// GOBLINATTACK2
	{ { 10,GOBSWORDATTACK2,236,99 },{10,GOBSWORDATTACK2+1,202,24},{10,GOBSWORDATTACK2+2,181,0},
	  { 10,GOBSWORDATTACK2+3,52,12 },{10,GOBSWORDATTACK2+4,72,72},{10,GOBSWORDATTACK2+5,134,139},
	  { 10,GOBSWORDATTACK2+6,297,169 },{1,0,275,24},{1,0,275,24 },
	  { 1,0,275,24 },{1,0,275,24},{1,0,275,24}
	}

};


#define LFIST   NULL

struct daweapons cockanimtics[MAXFRAMES+1] =
{
	{24,0,10,10},
	{12,BOWREADYEND+1,101,115},{12,BOWREADYEND+2,112,0},
	{12,BOWREADYEND+3,115,0},{12,BOWREADYEND+4,75,13}
};

struct daweapons readyanimtics[MAXWEAPONS][MAXFRAMES+1]=
{
	// FIST
	{ { 10,RFIST,216,180 },{10,RFIST,216,170},{10,RFIST,216,160},
	  { 10,RFIST,216,150 },{10,RFIST,216,140},{10,RFIST,216,130},
	  { 10,RFIST,216,124 },{1,RFIST,216,124},{1,RFIST,216,124},
	  { 1,RFIST,216,122 },{1,RFIST,216,122},{1,RFIST,216,122},
	  {1,0,147,76}
	},
	// KNIFE
	{ { 10,KNIFEREADY,69,171 },{10,KNIFEREADY+1,11,146},{10,KNIFEREADY+2,25,146},
	  { 10,KNIFEREADY+3,35,158 },{10,KNIFEREADY+4,38,158},{10,KNIFEREADY+5,16,157},
	  { 10,KNIFEREADY+6,37,102},{10,KNIFEREADY+7,239,63},{10,KNIFEREADY+8,214,85},
	  { 10,KNIFEREADY+9,206,110},{10,KNIFEREADY+10,217,108},{10,KNIFEREADY+11,204,95},
	  {1,0,147,76}
	},
	// GOBSWORD
	{ { 12,GOBSWORDPULL,79,169 },{12,GOBSWORDPULL+1,95,115},{12,GOBSWORDPULL+2,94,93},
	  { 12,GOBSWORDPULL+3,156,77 },{12,GOBSWORDPULL+4,218,64},{12,GOBSWORDPULL+5,224,57},
	  { 8,GOBSWORDPULL+6,251,54 },{1,GOBSWORDPULL+7,243,92},{1,GOBSWORDPULL+7,243,92},
	  { 1,GOBSWORDPULL+7,243,92 },{1,GOBSWORDPULL+7,243,92},{1,GOBSWORDPULL+7,243,92},
	  {1,0,147,76}
	},
	// MORNINGSTAR
	{ { 6,MORNINGSTAR,193,190 },{6,MORNINGSTAR,193,180},{6,MORNINGSTAR,193,170},
	  { 6,MORNINGSTAR,193,160 },{6,MORNINGSTAR,193,150},{6,MORNINGSTAR,193,140},
	  { 6,MORNINGSTAR,193,130 },{6,MORNINGSTAR,193,120},{6,MORNINGSTAR,193,110},
	  { 6,MORNINGSTAR,193,100 },{6,MORNINGSTAR,193,90},{1,MORNINGSTAR,193,90},
	  {1,0,147,76}
	},
	// SWORD
	{ { 10,SWORDPULL,58,160 },{10,SWORDPULL+1,81,111},{10,SWORDPULL+2,19,88},
	  { 10,SWORDPULL+3,0,93 },{10,SWORDPULL+4,104,0},{10,SWORDPULL+5,169,0},
	  { 10,SWORDPULL+6,244,38 },{6,SWORDPULL+7,225,121},{1,SWORDPULL+7,225,121},
	  { 1,SWORDPULL+7,225,121 },{1,SWORDPULL+7,225,121},{1,SWORDPULL+7,225,121},
	  {1,0,147,76}
	},
	{ { 12,BIGAXEDRAW,71,108 },{12,BIGAXEDRAW+1,17,58},{12,BIGAXEDRAW+2,0,56},
	  { 12,BIGAXEDRAW+3,0,71 },{12,BIGAXEDRAW+4,0,102},{12,BIGAXEDRAW+5,0,11},
	  { 12,BIGAXEDRAW+6,33,0 },{12,BIGAXEDRAW+7,69,0},{12,BIGAXEDRAW+8,75,20},
	  { 12,BIGAXEDRAW9,150,92 },{12,BIGAXEDRAW10,182,116},{1,0,200,122},
	  {1,0,147,76}
	},
	// BOW
	{ { 12,BOWREADY,0,0 },{12,BOWREADY+1,0,20},{12,BOWREADY+2,0,46},
	  { 12,BOWREADY+3,0,26 },{12,BOWREADY+4,0,0},{12,BOWREADY+5,71,0},
	  { 8,BOWREADYEND,77,23 },{1,BOWREADYEND,77,23},{1,BOWREADYEND,77,23},
	  { 1,BOWREADYEND,77,23 },{1,BOWREADYEND,77,23},{1,BOWREADYEND,77,23},
	  {1,0,147,76}
	},
	{ { 8,PIKEDRAW,0,156 },{8,PIKEDRAW+1,15,98},{8,PIKEDRAW+2,83,49},
	  { 8,PIKEDRAW+3,144,66 },{8,PIKEDRAW+4,197,99},{8,PIKEDRAW+5,216,131},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76},
	  {1,0,147,76}
	},
	{ { 12,EXCALDRAW,167,130 },{12,EXCALDRAW+1,70,117},{12,EXCALDRAW+2,0,128},
	  { 12,EXCALDRAW+3,0,150 },{12,EXCALDRAW+4,4,72},{12,EXCALDRAW+5,38,81},
	  { 12,EXCALDRAW+6,0,44 },{12,EXCALDRAW+7,112,0},{12,EXCALDRAW+8,224,0},
	  { 12,EXCALDRAW+9,198,84 },{12,EXCALDRAW+10,186,120},{12,EXCALDRAW+11,188,123},
	  {1,0,147,76}
	},
	{ { 12,HALBERDDRAW,183,62 },{12,HALBERDDRAW+1,166,10},{12,HALBERDDRAW+2,173,29},
	  { 12,HALBERDDRAW+3,114,35 },{1,HALBERDATTACK1,245,22},{1,HALBERDATTACK1,245,22},
	  { 1,HALBERDATTACK1,245,22 },{1,HALBERDATTACK1,245,22},{1,HALBERDATTACK1,245,22},
	  { 1,HALBERDATTACK1,245,22 },{1,HALBERDATTACK1,245,22},{1,HALBERDATTACK1,245,22},
	  {1,0,147,76}
	}

};



struct daweapons weaponanimtics[MAXWEAPONS][MAXFRAMES] =
{
	// FIST
	{ { 10,RFIST,216,120 },{10,RFIST+1,166,113},{10,RFIST+2,156,129},
	  { 10,RFIST+3,169,151 },{10,RFIST+4,153,124},{10,RFIST+5,224,133},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76}
	},
	// KNIFE
	{ { 8,KNIFEATTACK,189,52 },{8,KNIFEATTACK+1,254,68},{8,0,147,76},
	  { 8,0,80,41 },{8,KNIFEATTACK+2,254,69},{8,KNIFEATTACK+3,218,80},
	  { 8,KNIFEATTACK+4,137,83},{8,KNIFEATTACK+5,136,100},{8,KNIFEATTACK+6,126,140},
	  { 8,KNIFEATTACK+5,136,100},{8,KNIFEATTACK+4,137,83},{8,KNIFEATTACK,189,52}
	},
	// GOBLINATTACK
	{ {10,GOBSWORDATTACK,243,92},{10,GOBSWORDATTACK+1,255,68 },{10,GOBSWORDATTACK+2,279,65},
	  {10,GOBSWORDATTACK+3,238,55},{10,GOBSWORDATTACK+4,153,52 },{10,GOBSWORDATTACK+5,129,152},
	  {10,GOBSWORDATTACK+6,90,184},{ 1,0,297,169 },{1,0,275,24},
	  {1,0,275,24 },{ 1,0,275,24 },{1,0,275,24}
	},
	// MORNINGSTAR
	{ { 12,MORNINGSTAR,193,90 },{12,MORNINGSTAR+1,102,133},{12,MORNINGSTAR+2,77,164},
	  { 12,MORNINGSTAR+3,239,86 },{12,0,299,86},{12,0,107,52},
	  { 12,MORNINGSTAR+4,197,24 },{12,MORNINGSTAR+5,125,124},{12,MORNINGSTAR+6,109,191 },
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76}
	},
	// SWORD
	{ { 8,SWORDATTACK,229,123},{8,SWORDATTACK+1,221,87},{8,SWORDATTACK+2,193,21},
	  { 8,SWORDATTACK+3,173,0},{8,SWORDATTACK+4,61,0},{8,SWORDATTACK+5,33,48},
	  { 8,SWORDATTACK+6,126,131},{8,SWORDATTACK+7,297,164},{3,0,147,76},
	  { 3,0,80,41 },{3,0,107,52},{3,0,147,76}
	},
	{ { 12,BIGAXEATTACK,184,123 },{12,BIGAXEATTACK+1,223,112},{12,BIGAXEATTACK+2,63,151},
	  { 12,BIGAXEATTACK+3,91,133 },{12,BIGAXEATTACK+4,127,138},{12,BIGAXEATTACK+5,106,128},
	  { 12,BIGAXEATTACK+6,117,49 },{12,BIGAXEATTACK+7,140,0},{12,BIGAXEATTACK+8,152,47},
	  { 12,BIGAXEATTACK+9,166,143 },{12,0,107,52},{1,0,147,76}
	},
	// BOW
	{ { 8,BOWWALK,75,13},{8,BOWWALK+1,90,0},{8,BOWWALK+2,70,0},
	  { 8,BOWWALK+3,70,0},{6,BOWWALK+4,70,0},{4,BOWWALK+5,70,0},
	  { 1,0,126,131},{1,0,297,164},{1,0,147,76},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76}
	},
	{ { 10,PIKEDRAW+5,216,131},{10 ,0,80,41 },{10,0,107,52},{10,0,147,76},
	  { 10,PIKEATTACK1,0,47 },{10,PIKEATTACK1+1,0,0},{10,PIKEATTACK1+2,0,0},
	  { 10,PIKEATTACK1+3,73,0 },{10,PIKEATTACK1+4,130,27},{10,PIKEATTACK1+5,138,125},
	  { 12,0,80,41 },{1,0,107,52}
	},
	{ { 8,EXCALATTACK1,98,133},{8,EXCALATTACK1+1,123,130 },{8,EXCALATTACK1+2,125,128},
	  { 8,EXCALATTACK1+3,115,82},{ 8,EXCALATTACK1+4,115,6 },{8,EXCALATTACK1+5,178,0},
	  { 8,EXCALATTACK1+6,155,0},{ 8,EXCALATTACK1+7,143,0 },{8,EXCALATTACK1+8,90,91},
	  { 8,EXCALATTACK1+9,30,159},{ 1,0,80,41 },{1,0,107,52}
	},
	{ { 12,HALBERDATTACK1,245,22 },{12,0,107,52},{12,0,147,76},
	  { 12,HALBERDATTACK1+1,249,45 },{12,HALBERDATTACK1+2,161,60},{12,HALBERDATTACK1+3,45,88},
	  { 12,0,80,41 },{12,HALBERDATTACK1+3,45,88},{12,HALBERDATTACK1+2,161,60},
	  { 12,HALBERDATTACK1+1,249,45 },{12,0,107,52},{1,0,147,76}
	}
};


struct daweapons weaponanimtics2[MAXWEAPONS][MAXFRAMES] =
{
	// FIST
	{ { 10,RFIST,216,120 },{10,RFIST+1,166,113},{10,RFIST+2,156,129},
	  { 10,RFIST+3,169,151 },{10,RFIST+4,153,124},{10,RFIST+5,224,133},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76}
	},
	// KNIFE
	{ { 8,KNIFEATTACK,189,52 },{8,KNIFEATTACK+1,254,68},{16,0,147,76},
	  { 8,KNIFEATTACK2,206,114 },{8,KNIFEATTACK2+1,107,112},{8,KNIFEATTACK2+2,22,138},
	  { 8,KNIFEATTACK2+3,0,161},{16,0,136,100},{8,KNIFEATTACK2+3,0,161},
	  { 8,KNIFEATTACK2+2,22,138},{8,KNIFEATTACK2+1,107,112},{8,KNIFEATTACK2,206,114}
	},
	// GOBLINATTACK
	{ { 10,GOBSWORDATTACK2,236,99 },{10,GOBSWORDATTACK2+1,202,24},{10,GOBSWORDATTACK2+2,181,0},
	  { 10,GOBSWORDATTACK2+3,52,12 },{10,GOBSWORDATTACK2+4,72,72},{10,GOBSWORDATTACK2+5,134,139},
	  { 10,GOBSWORDATTACK2+6,297,169 },{1,0,275,24},{1,0,275,24 },
	  { 1,0,275,24 },{1,0,275,24},{1,0,275,24}
	},
	// MORNINGATTACK2
	{ { 12,MORNINGATTACK2,85,136 },{12,MORNINGATTACK2+1,34,110},{12,MORNINGATTACK2+2,32,91},
	  { 12,MORNINGATTACK2+3,186,47 },{12,MORNINGATTACK2+4,275,24},{1,0,275,24},
	  { 1,0,275,24 },{1,0,275,24},{1,0,275,24 },
	  { 1,0,275,24 },{1,0,275,24},{1,0,275,24}
	},
	// SWORD
	{ { 8,SWORDATTACK2+1,195,63},{8,SWORDATTACK2+2,250,54},{8,SWORDATTACK2+3,275,37},
	  {16,0,61,0},{ 8,SWORDATTACK2+4,229,66},{8,SWORDATTACK2+5,185,0},
	  { 8,SWORDATTACK2+6,158,115},{8,SWORDATTACK2+7,57,163},{1,0,57,163},
	  { 1,0,57,163 },{1,0,57,163},{1,0,57,163}
	},
	{ { 12,BIGAXEATTACK2,200,111 },{12,BIGAXEATTACK2+1,5,136},{12,BIGAXEATTACK2+2,69,162},
	  { 12,BIGAXEATTACK2+3,147,164 },{12,BIGAXEATTACK2+4,76,152},{12,BIGAXEATTACK2+5,33,95},
	  { 12,BIGAXEATTACK2+6,0,91 },{12,BIGAXEATTACK2+7,0,98},{12,0,147,76},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76}
	},
	// BOW
	{ { 8,BOWWALK,75,13},{8,BOWWALK+1,90,0},{8,BOWWALK+2,70,0},
	  { 8,BOWWALK+3,70,0},{6,BOWWALK+4,70,0},{4,BOWWALK+5,70,0},
	  { 1,0,126,131},{1,0,297,164},{1,0,147,76},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76}
	},
	{ { 10,PIKEATTACK2,266,147 },{10,PIKEATTACK2+1,182,117},{10,PIKEATTACK2+2,123,84},
	  { 10,PIKEATTACK2+3,7,48 },{10,PIKEATTACK2+4,0,83},{10,PIKEATTACK2+5,0,158},
	  { 10,PIKEATTACK2+6,25,117 },{10,PIKEATTACK2+7,139,93},{10,PIKEATTACK2+8,234,75},
	  { 8,0,80,41 },{1,0,107,52},{1,0,147,76}
	},
	{ { 8,EXCALATTACK2,0,143},{8,EXCALATTACK2+1,0,103 },{8,EXCALATTACK2+2,0,70},
	  { 8,EXCALATTACK2+3,48,0},{ 8,EXCALATTACK2+4,67,0 },{8,EXCALATTACK2+5,78,21},
	  { 8,EXCALATTACK2+6,165,107},{ 8,EXCALATTACK2+7,260,168 },{1,0,130,27},
	  { 1,0,138,125},{ 1,0,80,41 },{1,0,107,52}
	},
	{ { 12,HALBERDATTACK1,245,22 },{12,HALBERDATTACK2,114,35},{12,HALBERDATTACK2+1,105,87},
	  { 12,HALBERDATTACK2+2,54,107 },{12,HALBERDATTACK2+3,48,102},{1,HALBERDATTACK2+3,48,102},
	  { 1,HALBERDATTACK2+3,48,102 },{12,HALBERDATTACK2+2,54,107},{12,HALBERDATTACK2+1,105,87},
	  { 1,0,80,41 },{1,0,107,52},{1,0,147,76}
	}
};

// fist
// bracers
// hammer
// sword
// bow
// axe
// morning star
// bfg item

#define MAXSTATUSBAR    3

int heartics=0;
int heartincr=0;
int showstatusbar=0;
int showheart=0;

int showmana=0;

int showbook=0;
int showbooktype=0;
int showbookflip;
int showbookanim;
int currentorb;
int currentpotion;


void playerdead(struct player *plr) {

	int i;
	int spellbookpage;
	int clockgoal;
	int goaltime;
	int exit=0;

	clockgoal=totalclock+240;

	playerdie=1;
	currspikeframe=0;

	if(spiked == 1) {
		spiketics=spikeanimtics[0].daweapontics;
		//JSA_SPOOGE
		playsound_loc(S_GORE1,plr->x,plr->y);
		SND_Sound(S_HEARTBEAT);
		//JSA_ENDS
	}

	SND_PlaySound(S_PLRDIE1,0,0,0,0);
	//playsound_loc(S_PLRDIE1,plr->x,plr->y);

//JSA CHECK LATER
	//SND_MenuMusic(DEATHSONG);

	netsendmove();

	while ( totalclock < clockgoal ) {
		handleevents();
		if (plr->horiz < 100+(YDIM>>1)) {
			plr->horiz+=(synctics<<1);
		}
		drawscreen(plr);
		animateobjs(plr);
		animatetags(plr);
		doanimations((int)synctics);
		dodelayitems((int)synctics);
		nextpage();
	}

	goaltime=totalclock+240;

	while( !exit ) {
		handleevents();
		if(totalclock > goaltime)
			exit=1;
		if(keystatus[0x39] > 0)
			exit=1;
	}

	//check loops wando
	SND_CheckLoops();

	keystatus[0x39]=0;

	vampiretime=0;
	shieldpoints=0;
	playerdie=0;
	spiked=0;
	shockme=-1;
	poisoned=0;
	poisontime=0;
	currspikeframe=0;
	spellbookflip=0;
	displaytime=0;

	plr->oldsector=plr->sector;
	plr->horiz=100;
	plr->zoom=256;
	plr->dimension=3;
	plr->height=PLAYERHEIGHT;

	if( difficulty > 1 )
		for(i=0;i<=9;i++) {
			plr->ammo[i]=0;
			plr->weapon[i]=0;
			if( i < 9 ) {
				plr->orb[i]=0;
				plr->orbammo[i]=0;
			}
		}
	else
		for(i=0;i<=9;i++) {
			if(i < 5) {
				plr->ammo[i]=40;
				plr->weapon[i]=1;
			}
			if( i < 9 ) {
				plr->orb[i]=0;
				plr->orbammo[i]=0;
			}
		}

	if( difficulty > 1) {
		plr->weapon[0]=plr->weapon[1]=1;
		plr->ammo[0]=32000;
		plr->ammo[1]=45;
	}

	for(i=0;i<MAXPOTIONS;i++)
		plr->potion[i]=0;

	for(i=0;i<MAXTREASURES;i++)
		plr->treasure[i]=0;

	hasshot=0;
	orbshot=0;
	oldmousestatus=0;

	plr->lvl=1;
	plr->score=0;
	plr->health=100;
	plr->maxhealth=100;
	plr->armor=0;
	plr->armortype=0;
	currentorb=0;
	currentpotion=0;

	for(i=0;i<MAXNUMORBS;i++)
		plr->orbactive[i]=-1;

	lockclock=totalclock;

		if( difficulty > 1)
			currweapon=selectedgun=1;
		else
			currweapon=selectedgun=4;


	currweaponfired=3;
	currweaponflip=0;
	currweaponanim=0;

	helmettime=-1;

	if( plr->screensize <= 320 ) {
		spellbookpage=spellbookanim[currentorb][8].daweaponframe;
		rotatesprite(121<<16,161<<16,65536,0,spellbookpage,0,0,2+8+16,0,0,xdim-1,ydim-1);
	}

	justteleported=1;

	if (netgame == 0) {
		 loadnewlevel(mapon);
	}
	else {
		 netrestartplayer(plr);
	}

		displaytime=-1;
		shadowtime=-1;
		helmettime=-1;
		nightglowtime=-1;
		strongtime=-1;
		invisibletime=-1;
		svgahealth=-1;


	/*
	if(svga == 0) {
		if(plr->screensize <= 320)
			permanentwritesprite(0,0,BACKGROUND,0,0,0,319,199,0);
		if(plr->screensize <= 320)
			permanentwritesprite(0,200-46,NEWSTATUSBAR,0,0,0,319,199,0);
		updatepics();
	}
	if(svga == 1) {
		if(plr->screensize == 320)
			overwritesprite(0,0,639,371,SSTATUSBAR,0,0,0);
	}
	*/

	updatepics();

}


void spikeheart(struct player *plr) {

	char dabits=0x02;
	char dashade=sector[plr->sector].ceilingshade;
	int  dax, day;

	plr=&player[pyrn];

	spiketics-=synctics;

	if( spiketics < 0 ) {
		currspikeframe++;

		if( currspikeframe > 4 )
			currspikeframe=4;

		spiketics=spikeanimtics[currspikeframe].daweapontics;
		spikeframe=spikeanimtics[currspikeframe].daweaponframe;
	}
	else
		currweaponframe=spikeanimtics[currspikeframe].daweaponframe;

	dax=spikeanimtics[currspikeframe].currx;
	day=spikeanimtics[currspikeframe].curry;

	overwritesprite(dax,day,spikeframe,dashade,dabits,0);
	startredflash(10);

}

void updateloadedplayer(int i) {

	struct player *plr;
	plr=&player[pyrn];

	playerdie=0;
	spiked=0;
	lives=0;

	plr->oldsector=plr->sector;
	plr->horiz=100;
	plr->zoom=256;
	plr->dimension=3;
	plr->height=PLAYERHEIGHT;
	plr->spritenum=i;

	sprite[plr->spritenum].x=plr->x;
	sprite[plr->spritenum].y=plr->y;
	sprite[plr->spritenum].z=plr->z+(plr->height<<8);
	sprite[plr->spritenum].cstat=1;
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
	sprite[plr->spritenum].sectnum=plr->sector;

	setsprite(i,sprite[i].x,sprite[i].y,sprite[i].z);

}

void initplayersprite(void) {

	struct player *plr;
	int i;
	int spellbookpage;

	plr=&player[pyrn];

	vampiretime=0;
	shieldpoints=0;
	playerdie=0;
	spiked=0;
	shockme=-1;
	poisoned=0;
	poisontime=-1;
	lives=0;

	if( mapflag == 0)
		mapon=1;

	plr->oldsector=plr->sector;
	plr->horiz=100;
	plr->zoom=256;
	plr->dimension=3;
	plr->height=PLAYERHEIGHT;
	plr->z=sector[plr->sector].floorz-(plr->height<<8);

	plr->spritenum=insertsprite(plr->sector,0);

	plr->onsomething=1;

	sprite[plr->spritenum].x=plr->x;
	sprite[plr->spritenum].y=plr->y;
	sprite[plr->spritenum].z=plr->z+(plr->height<<8);
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

	if(loadedgame == 0) {
		selectedgun=0;

	if( difficulty > 1 )
		for(i=0;i<=9;i++) {
			plr->ammo[i]=0;
			plr->weapon[i]=0;
			if( i < 9 ) {
				plr->orb[i]=0;
				plr->orbammo[i]=0;
			}
		}
	else
		for(i=0;i<=9;i++) {
			if(i < 5) {
				plr->ammo[i]=40;
				plr->weapon[i]=1;
			}
			if( i < 9 ) {
				plr->orb[i]=0;
				plr->orbammo[i]=0;
			}
		}

	if( difficulty > 1) {
		plr->weapon[0]=plr->weapon[1]=1;
		plr->ammo[0]=32000;
		plr->ammo[1]=45;
	}

		for(i=0;i<MAXPOTIONS;i++)
			plr->potion[i]=0;

		for(i=0;i<MAXTREASURES;i++)
			plr->treasure[i]=0;

		plr->lvl=1;
		plr->score=0;
		plr->health=100;
		plr->maxhealth=100;
		plr->armor=0;
		plr->armortype=0;
		currentorb=0;
		currentpotion=0;

		if( difficulty > 1)
			currweapon=selectedgun=1;
		else
			currweapon=selectedgun=4;

		currweaponfired=3;
		currweaponflip=0;


		for(i=0;i<MAXNUMORBS;i++)
			plr->orbactive[i]=-1;

		lockclock=totalclock;

		spellbookflip=0;

		if( plr->screensize <= 320 ) {
			spellbookpage=spellbookanim[currentorb][8].daweaponframe;
			rotatesprite(121<<16,161<<16,65536,0,spellbookpage,0,0,2+8+16,0,0,xdim-1,ydim-1);
		}

		invincibletime=manatime=-1;
		hasshot=0;
		orbshot=0;
		oldmousestatus=0;

		displaytime=-1;
		shadowtime=-1;
		helmettime=-1;
		nightglowtime=-1;
		strongtime=-1;
		invisibletime=-1;
		svgahealth=-1;
	}

	updatepics();

}

void autoweaponchange(int dagun) {

		if( currweaponanim > 0)
			return;

		if( dagun > selectedgun ) {
			selectedgun=dagun;
			hasshot = 0;
			currweaponfired=2; // drop weapon
			switch( selectedgun ) {
			case 1:
				weapondropgoal=100;
				weapondrop=0;
			break;
			case 2:
				weapondropgoal=40;
				weapondrop=0;
				levelpic();
			break;
			case 3:
				weapondropgoal=100;
				weapondrop=0;
				levelpic();
			break;
			case 4:
				weapondropgoal=40;
				weapondrop=0;
				levelpic();
			break;
			case 5:
				weapondropgoal=40;
				weapondrop=0;
				levelpic();
			break;
			case 6:
				weapondropgoal=40;
				weapondrop=0;
				levelpic();
			break;
			case 7:
				weapondropgoal=40;
				weapondrop=0;
				levelpic();
			break;
			case 8:
				weapondropgoal=40;
				weapondrop=0;
				levelpic();
			break;
			case 9:
				weapondropgoal=40;
				weapondrop=0;
				levelpic();
			break;
			}
		}

}

void weaponchange(void) {

	int i;
	int  j;
	struct player *plr;

	plr=&player[pyrn];

	if(currweaponanim == 0 && currweaponflip == 0)
	for(i=0x2;i<=0xb;i++) {
		if(keystatus[i] > 0 && plr->weapon[i-0x2] > 0) {
			selectedgun=i-0x2;
			keystatus[i]=0;
			hasshot=0;
			currweaponfired=2; // drop weapon
			levelpic();
			switch(selectedgun) {
			case 0:
				weapondropgoal=40;
				weapondrop=0;
			break;
			case 1:
				weapondropgoal=100;
				weapondrop=0;
			break;
			case 2:
				weapondropgoal=100;
				weapondrop=0;
			break;
			case 3:
				weapondropgoal=100;
				weapondrop=0;
			break;
			case 4:
				weapondropgoal=40;
				weapondrop=0;
			break;
			case 5:
				weapondropgoal=40;
				weapondrop=0;
			break;
			case 6:
				weapondropgoal=40;
				weapondrop=0;
			break;
			case 7:
				weapondropgoal=40;
				weapondrop=0;
			break;
			case 8:
				weapondropgoal=40;
				weapondrop=0;
			break;
			case 9:
				weapondropgoal=40;
				weapondrop=0;
			break;
			}
		 }
	  }

	/*
	if( keystatus[0x3c] > 0
		|| keystatus[0x3d] > 0
		|| keystatus[0x3e] > 0
		|| keystatus[0x41] > 0
		|| keystatus[0x42] > 0) {
		castaorb(plr);
		keystatus[i]=0;
		orbpic(currentorb);
		orbshot=0;
		hasshot=0;
	goto instantspell;
	}
	*/


	  if(currweaponflip == 0)
	  for(i=0x3b;i<=0x42;i++) {
		if (keystatus[i] > 0) {
			if(selectedgun > 0) {
				hasshot=0;
				currweaponfired=2; // drop weapon
				weapondropgoal=100;
				weapondrop=0;
				selectedgun=0;
				//selectedgun=spellcasthands
				levelpic();
			}
			currentorb=i-0x3b;
			if( spellbookflip == 0 ) {
				spellbook=0;
				spellbooktics=10;
				spellbookflip=1;
				//JSA_NEW
				SND_PlaySound(S_PAGE,0,0,0,0);
				displayspelltext();
				spelltime=360;
			}
			orbshot=0;
		}
		keystatus[i]=0;
		orbpic(currentorb);
	}


	for(j=0;j<MAXNUMORBS;j++) {
		if( plr->orbactive[j] > -1 ) {
			plr->orbactive[j]-=synctics;
		 }
	 }

	 if(keystatus[0x1a] > 0) {
		currentpotion--;
		if(currentpotion < 0)
			currentpotion=4;
			keystatus[0x1a]=0;
			//JSA_NEW
			SND_PlaySound(S_BOTTLES,0,0,0,0);
			potionpic(currentpotion);
			potiontext();
	 }
	 if(keystatus[0x1b] > 0) {
		currentpotion++;
		if(currentpotion > 4)//MAXPOTIONS
			currentpotion=0;
			keystatus[0x1B]=0;
			//JSA_NEW
			SND_PlaySound(S_BOTTLES,0,0,0,0);
			potionpic(currentpotion);
			potiontext();
	 }


}


void potiontext(void) {

	struct player *plr;

	plr=&player[pyrn];

	if( plr->potion[currentpotion] > 0)
		switch(currentpotion) {
		case 0:
			strcpy(displaybuf,"Health Potion");
			displaytime=240;
		break;
		case 1:
			strcpy(displaybuf,"Strength Potion");
			displaytime=240;
		break;
		case 2:
			strcpy(displaybuf,"Cure Poison Potion");
			displaytime=240;
		break;
		case 3:
			strcpy(displaybuf,"Resist Fire Potion");
			displaytime=240;
		break;
		case 4:
			strcpy(displaybuf,"Invisiblity Potion");
			displaytime=240;
		break;
		}
}

void swingdacrunch(int daweapon) {


	struct player *plr;

	plr=&player[pyrn];

	switch(daweapon) {
	case 0: //fist
		playsound_loc(S_SOCK1+(rand()%4),plr->x,plr->y);
	break;
	case 1: //dagger
		if(rand()%2)
			playsound_loc(S_GORE1+(rand()%4),plr->x,plr->y);
	break;
	case 2: //short sword
		playsound_loc(S_SWORD2+(rand()%3),plr->x,plr->y);
	break;
	case 3: //morningstar
		playsound_loc(S_SOCK1+(rand()%4),plr->x,plr->y);
	break;
	case 4: //broad sword
		playsound_loc(S_SWORD1+(rand()%3),plr->x,plr->y);
	break;
	case 5: //battle axe
		if(rand()%2)
			playsound_loc(S_SOCK1+(rand()%4),plr->x,plr->y);
		else
			playsound_loc(S_SWORD1+(rand()%3),plr->x,plr->y);
	break;
	case 6: //bow

	break;
	case 7: //pike
		if(rand()%2)
			playsound_loc(S_SOCK1+(rand()%4),plr->x,plr->y);
		else
			playsound_loc(S_SWORD1+(rand()%3),plr->x,plr->y);
	break;
	case 8: //two handed sword
		playsound_loc(S_SWORD1+(rand()%2),plr->x,plr->y);
	break;
	case 9: //halberd
		if(rand()%2)
			playsound_loc(S_SOCK1+(rand()%4),plr->x,plr->y);
		else
			playsound_loc(S_SWORD1+(rand()%3),plr->x,plr->y);
	break;
	}

}

void swingdasound(int daweapon) {

	switch(daweapon) {
	case 0: // fist
		SND_PlaySound(S_PLRWEAPON0,0,0,0,0);
	break;
	case 1:  // knife
		SND_PlaySound(S_PLRWEAPON1,0,0,0,0);
	break;
	case 2:  // short sword
		SND_PlaySound(S_PLRWEAPON4,0,0,0,0);
	break;
	case 3:  // mace
		SND_PlaySound(S_PLRWEAPON2,0,0,0,0);
	break;
	case 4:  //
		SND_PlaySound(S_PLRWEAPON4,0,0,0,0);
	break;
	case 5:  // sword
		SND_PlaySound(S_PLRWEAPON4,0,0,0,0);
	break;
	case 6:  // bow
		SND_PlaySound(S_PLRWEAPON3,0,0,0,0);
	break;
	case 7:  //
		SND_PlaySound(S_PLRWEAPON4,0,0,0,0);
	break;
	case 8:  //
		SND_PlaySound(S_PLRWEAPON4,0,0,0,0);
	break;
	case 9:  //
		SND_PlaySound(S_PLRWEAPON4,0,0,0,0);
	break;
	}


}

void swingdaweapon(struct player *plr) {

	short daang;

	daang=plr->ang;

	if(currweaponframe == BOWWALK+5 && plr->ammo[6] > 0) {
		currweaponfired=5;
		oldmousestatus=0; // NEW
		currweaponanim=0;
	}
	if( currweaponframe == BOWWALK+5 && plr->ammo[6] <= 0 ) {
		currweaponfired=0;
		oldmousestatus=0;
		currweaponanim=0;
		return;
	}

	if(currweaponframe == PIKEATTACK1+4
		//|| currweaponframe == PIKEATTACK2+4
		&& plr->weapon[7] == 2
		&& plr->ammo[7] > 0) {
		shootgun( plr, daang, 10);
		playsound_loc(S_THROWPIKE,plr->x,plr->y);
		hasshot=1;
		return;
	}

	switch( selectedgun ) {
	  case 0:  // fist  & close combat weapons
		 shootgun( plr, daang, 0);
		 hasshot=1;
		 break;
	  case 1:  // knife
		 shootgun( plr, daang, 0);
		 hasshot=1;
		 break;
	  case 2:  // shortsword
		 shootgun( plr, daang, 0);
		 hasshot=1;
		 break;
	  case 3:  // morningstar
		 shootgun( plr, daang, 0);
		 hasshot=1;
		 break;
	  case 4:  // sword
		 shootgun( plr, daang, 0);
		 hasshot=1;
		 break;
	  case 5: //  battleaxe
		 shootgun( plr, daang, 0);
		 hasshot=1;
	  break;
	  case 6: // bow
		 shootgun( plr, daang, 1);
		 hasshot=1;
	  break;
	  case 7: // pike
		 shootgun( plr, daang, 0);
		 hasshot=1;
	  break;
	  case 8: // two handed
		 shootgun( plr, daang, 0);
		 hasshot=1;
	  break;
	  case 9: // halberd
		 shootgun( plr, daang, 0);
		 hasshot=1;
	  break;

   }

}


int weaponuseless=0;

void plrfireweapon(struct player *plr) {

	int i;


	if(currweaponfired == 4) {
		currweapontics=throwanimtics[currentorb][0].daweapontics;
		return;
	}

	if( plr->ammo[selectedgun] <= 0) {
		if( currweapon == 6 ) {
			for(i=0;i<MAXWEAPONS;i++) {
				if(plr->ammo[i] > 0 && plr->weapon[i] == 1) {
					selectedgun=i;
					hasshot = 0;
					currweaponfired=2; // drop weapon
					weapondropgoal=100;
					weapondrop=0;
					levelpic();
				}
			}
		}
		return;
	}
	else {
		madeahit=0;
		plr->ammo[selectedgun]--;
		if( plr->ammo[selectedgun] <= 0 || plr->ammo[selectedgun] == 10) {
			switch(selectedgun) {
			case 0: //fist
				plr->ammo[0]=9999;
			break;
			case 1: //knife
				if( plr->ammo[selectedgun] == 10 ) {
					strcpy(displaybuf,"Dagger is damaged");
					displaytime=360;
				}
				if( plr->ammo[selectedgun] <= 0) {
					plr->ammo[1]=0;
					plr->weapon[1]=0;
					strcpy(displaybuf,"Dagger is Useless");
					displaytime=360;
					weaponuseless=1;
				}
			break;
			case 2: //short sword
				if(plr->ammo[selectedgun] == 10 ) {
					strcpy(displaybuf,"Short Sword is damaged");
					displaytime=360;
				}
				if(plr->ammo[selectedgun] <= 0) {
					plr->ammo[2]=0;
					plr->weapon[2]=0;
					strcpy(displaybuf,"Short Sword is Useless");
					displaytime=360;
					weaponuseless=1;
				}
			break;
			case 3: //mace
				if(plr->ammo[selectedgun] == 10 ) {
					strcpy(displaybuf,"Morning Star is damaged");
					displaytime=360;
				}
				if(plr->ammo[selectedgun] <= 0) {
					plr->ammo[3]=0;
					plr->weapon[3]=0;
					strcpy(displaybuf,"Morning Star is Useless");
					displaytime=360;
					weaponuseless=1;
				}
			break;

			case 4: //sword
				if(plr->ammo[selectedgun] == 10) {
					strcpy(displaybuf,"Sword is damaged");
					displaytime=360;
				}
				if(plr->ammo[selectedgun] <= 0) {
					plr->ammo[4]=0;
					plr->weapon[4]=0;
					strcpy(displaybuf,"Sword is Useless");
					displaytime=360;
					weaponuseless=1;
				}
			break;
			case 5: //battle axe
				if(plr->ammo[selectedgun] == 10) {
					strcpy(displaybuf,"Battle axe is damaged");
					displaytime=360;
				}
				if(plr->ammo[selectedgun] <= 0) {
					plr->ammo[5]=0;
					plr->weapon[5]=0;
					strcpy(displaybuf,"Battle axe is Useless");
					displaytime=360;
					weaponuseless=1;
				}
			break;
			case 6: //bow
			break;
			case 7: //pike
				if(plr->weapon[7] == 1) {
					if(plr->ammo[selectedgun] == 10) {
						strcpy(displaybuf,"Pike is damaged");
						displaytime=360;
					}
					if(plr->ammo[selectedgun] <= 0) {
						plr->ammo[7]=0;
						plr->weapon[7]=0;
						strcpy(displaybuf,"Pike is Useless");
						displaytime=360;
						weaponuseless=1;
					}
				}
				if(plr->weapon[7] == 2 && plr->ammo[7] <= 0) {
					plr->weapon[7]=1;
					plr->ammo[7]=30;
				}
			break;
			case 8: // two handed sword
				if(plr->ammo[selectedgun] == 10) {
					strcpy(displaybuf,"Magic Sword is damaged");
					displaytime=360;
				}
				if(plr->ammo[selectedgun] <= 0) {
					plr->ammo[8]=0;
					plr->weapon[8]=0;
					strcpy(displaybuf,"Magic Sword is Useless");
					displaytime=360;
					weaponuseless=1;
				}
			break;
			case 9: //halberd
				if(plr->ammo[selectedgun] == 10) {
					strcpy(displaybuf,"Halberd is damaged");
					displaytime=360;
				}
				if(plr->ammo[selectedgun] <= 0) {
					plr->ammo[9]=0;
					plr->weapon[9]=0;
					strcpy(displaybuf,"Halberd is Useless");
					displaytime=360;
					weaponuseless=1;
				}
			break;
			}
		}
	}

	if(weaponuseless == 1)
		for(i=0;i<MAXWEAPONS;i++) {
			if( plr->weapon[i] > 0 && plr->ammo[i] > 0 ) {
				currweapon=selectedgun=i;
				//hasshot=0;
				//currweaponfired=2; // drop weapon
				currweaponfired=3; // ready weapon
				//weapondropgoal=100;
				//weapondrop=0;
				currweaponflip=0;
				weaponuseless=0;
				//autoweaponchange(i);
				levelpic();
			}
		}
	else
		currweaponfired=1;


	if(currweapon == 6 || selectedgun == 6)
		levelpic();

	if(currweapon == 7 || selectedgun == 7)
		levelpic();

	currweapon=selectedgun;

	// start from the beginning to cycle
	//currweaponfired=1;

	currweaponattackstyle=krand()%2;

	if( plr->weapon[7] == 2 && currweapon == 7 ) {
		currweaponattackstyle=0;
	}

	if( currweapon == 9 ) {
		if( krand()%100 > 80 )
			currweaponattackstyle=0;
		else
			currweaponattackstyle=1;
	}

	if( currweaponanim > 11 ) {
	  currweapontics=weaponanimtics[currweapon][0].daweapontics;
	}


}

void activatedaorb(struct player *plr) {


	if( plr->orbammo[currentorb] <= 0)
		return;

	switch(currentorb) {
		 case 0: // SCARE
			//shadowtime=1200+(plr->lvl*120);
		 break;
		 case 1: // NIGHT VISION
			//nightglowtime=2400+(plr->lvl*600);
		 break;
		 case 2: // FREEZE
			plr->orbactive[currentorb]=-1;
		 break;
		 case 3: // MAGIC ARROW
			plr->orbactive[currentorb]=-1;
		 break;
		 case 4: // OPEN DOORS
			plr->orbactive[currentorb]=-1;
		 break;
		 case 5: // FLY
			//plr->orbactive[currentorb]=3600+(plr->lvl*600);
		 break;
		 case 6: // FIREBALL
			plr->orbactive[currentorb]=-1;
		 break;
		 case 7: // NUKE
			plr->orbactive[currentorb]=-1;
		 break;
	 }

	if( plr->orbammo[currentorb] <= 0 ) {
		plr->orb[currentorb]=0;
		return;
	}
	else
		plr->orbammo[currentorb]--;

	currweaponfired=4;
	currweapontics=throwanimtics[currentorb][0].daweapontics;

}

void plruse(struct player *plr) {

	short hitsect, hitwall, hitsprite, daang2, daang;
	int i, daz2, hitx, hity, hitz;

	neartag(plr->x,plr->y,plr->z,plr->sector,plr->ang,
		  &neartagsector,&neartagwall,&neartagsprite,&neartaghitdist,1024,3);
	if (neartagsector >= 0) {
		if (sector[neartagsector].hitag == 0) {
			operatesector(neartagsector);
		}
		else {
			daang=plr->ang;
			daang2=daang+2048;
			daz2=(100-plr->horiz)*2000;
			hitscan(plr->x,plr->y,plr->z,plr->sector,                   //Start position
				sintable[(daang2+2560)&2047],           //X vector of 3D ang
				sintable[(daang2+2048)&2047],           //Y vector of 3D ang
				daz2,                                   //Z vector of 3D ang
				&hitsect,&hitwall,&hitsprite,&hitx,&hity,&hitz,CLIPMASK1);
				if( hitwall >= 0 ) {
					if( (labs(plr->x-hitx)+labs(plr->y-hity) < 512) && (labs((plr->z>>8)-((hitz>>8)-(64))) <= (512>>3)) ) {
						switch(wall[hitwall].picnum) {
						case PENTADOOR1:
						case PENTADOOR2:
						case PENTADOOR3:
						case PENTADOOR4:
						case PENTADOOR5:
						case PENTADOOR6:
						case PENTADOOR7:
							strcpy(displaybuf,"find door trigger");
							displaytime=360;
						break;
						}
					}
				}
		playsound_loc(S_PUSH1+(rand()%2),plr->x,plr->y);
		}

	}
	if (neartagsprite >= 0) {
		if( sprite[neartagsprite].lotag == 1 ) {
			switch(sprite[neartagsprite].picnum) {
			case PULLCHAIN1:
				sprite[neartagsprite].lotag=0;
				newstatus(neartagsprite,PULLTHECHAIN);
			break;
			case LEVERUP:
				sprite[neartagsprite].lotag=0;
				newstatus(neartagsprite,ANIMLEVERUP);
			break;
			}
			for(i=0;i<numsectors;i++)
				if(sector[i].hitag == sprite[neartagsprite].hitag)
					operatesector(i);
		}
		else
			operatesprite(neartagsprite);
	}

}

void loadnewlevel(int mapon) {

	char mapbuf[16];

	sprintf(mapbuf,"level%d.map",mapon);

//JSA_DEMO2
	 SND_SongFlush();
	 SND_StartMusic(mapon-1);
//JSA_ENDS

#ifdef WHDEMO
    if (mapon > 3) {
        victory();
    }
#endif

	setupboard(mapbuf);

}

void victory(void) {

	int exit;

	victor=1;

	flushperms();

	if(svga == 1) {
		SND_Sound(S_PICKUPFLAG);
		svgafullscreenpic(SVGAVICTORYA1, SVGAVICTORYA2);
		nextpage();
		exit=0;
		while( !exit ){
			handleevents();
			if(keystatus[0x39] > 0 || keystatus[1] > 0)
				exit=1;
		}
			keystatus[0x39]=0;
			keystatus[1]=0;

		nextpage();
		exit=0;
		while( !exit ){
			handleevents();
			if(keystatus[0x39] > 0 || keystatus[1] > 0)
				exit=1;
		}
			keystatus[0x39]=0;
			keystatus[1]=0;

		SND_Sound(S_DROPFLAG);
		svgafullscreenpic(SVGAVICTORYB1, SVGAVICTORYB2);
		nextpage();
		exit=0;
		while( !exit ) {
			handleevents();
			if( keystatus[0x39] > 0 || keystatus[1] > 0)
					exit=1;
		}
			keystatus[0x39]=0;
			keystatus[1]=0;

		SND_Sound(S_WISP2);
		svgafullscreenpic(SVGAVICTORYC1, SVGAVICTORYC2);
		nextpage();
		exit=0;
		while( !exit ){
			handleevents();
			if(keystatus[0x39] > 0 || keystatus[1] > 0)
				exit=1;
		}
			keystatus[0x39]=0;
			keystatus[1]=0;

	}
	else {
		keystatus[0x39]=0;
		keystatus[1]=0;
		SND_Sound(S_PICKUPFLAG);
		exit=0;
		while( !exit ) {
			handleevents();
			if(keystatus[0x39] > 0 || keystatus[1] > 0)
				exit=1;
			rotatesprite(0,0,65536,0,VICTORYA,0,0,2+8+16,0,0,xdim-1,ydim-1);
			nextpage();
		}
		keystatus[0x39]=0;
		keystatus[1]=0;
		SND_Sound(S_DROPFLAG);
		exit=0;
		while( !exit ) {
			handleevents();
			if(keystatus[0x39] > 0 || keystatus[1] > 0)
				exit=1;
			rotatesprite(0,0,65536,0,VICTORYB,0,0,2+8+16,0,0,xdim-1,ydim-1);
			nextpage();
		}
		keystatus[0x39]=0;
		keystatus[1]=0;
		exit=0;
		SND_Sound(S_WISP2);
		exit=0;
		while( !exit ) {
			handleevents();
			if(keystatus[0x39] > 0 || keystatus[1] > 0)
				exit=1;
			rotatesprite(0,0,65536,0,VICTORYC,0,0,2+8+16,0,0,xdim-1,ydim-1);
			nextpage();
		}
		keystatus[0x39]=0;
		keystatus[1]=0;
		exit=0;

	}
	shutdowngame();

}

void drawweapons(struct player *plr) {

	int dax, day;
	int snakex=0;
	int snakey=0;
	char dabits;
	char dashade;
	char dapalnum;

	if( spelltime > 0)
		spelltime-=synctics;

	if( spellbook == 8 && spelltime > 0 && plr->screensize > 320) {
		spellbookframe=spellbookanim[currentorb][8].daweaponframe;
		dax=spellbookanim[currentorb][8].currx;
		day=spellbookanim[currentorb][8].curry;
		rotatesprite(dax<<16,day<<16,65536,0,spellbookframe,0,0,2+8+16,0,0,xdim-1,ydim-1);
		sprintf(tempbuf,"%d",plr->orbammo[currentorb]);
		fancyfontscreen(126,181,SCOREFONT-26,tempbuf,0);
	}


	if( shadowtime > 0 )
		dashade=31, dapalnum=0;
	else
		dashade=sector[plr->sector].ceilingshade, dapalnum=0;

	if( invisibletime > 0)
		dabits=0x06;
	else
		dabits=0x02;

	if( currweaponflip == 1 )
		dabits+=0x08;

	if( currweapon == 0 && dahand == 0 ) {
		if( rand()%2 == 0 )
			dahand=1;
		else
			dahand=2;
	}

	switch( currweaponfired ) {
	case 6:
		switch(currweapon) {
		case 1: // knife
			if(currweaponframe == KNIFEATTACK2+1)
				if((currweaponanim == 2 || currweaponanim == 10) && currweapontics == 8)
					swingdasound(currweapon);
		break;
		case 3: // morning
			if(currweaponframe == MORNINGATTACK2+3)
				if(currweaponanim == 3 && currweapontics == 12)
					swingdasound(currweapon);
		break;
		}
		if( currweaponframe == RFIST+5
			|| currweaponframe == KNIFEATTACK+6
			|| currweaponframe == MORNINGSTAR+5
			|| currweaponframe == SWORDATTACK+7
			|| currweaponframe == BOWWALK+5
			|| currweaponframe == KNIFEATTACK2+2
			|| currweaponframe == SWORDATTACK2+6
			|| currweaponframe == MORNINGATTACK2+3
			|| currweaponframe == HALBERDATTACK1+3
			|| currweaponframe == HALBERDATTACK2+3
			|| currweaponframe == BIGAXEATTACK+7
			|| currweaponframe == BIGAXEATTACK2+6
			|| currweaponframe == PIKEATTACK1+4
			|| currweaponframe == PIKEATTACK2+4
			|| currweaponframe == EXCALATTACK1+7
			|| currweaponframe == EXCALATTACK2+5
			|| currweaponframe == GOBSWORDATTACK2+4
			|| currweaponframe == GOBSWORDATTACK+4)

			swingdaweapon(plr);

		currweapontics-=synctics;

		if( helmettime > 0)
			currweapontics--;

		if( currweapontics < 0 ) {
			currweaponanim++;

			if( currweaponanim > 11 ) {
				currweaponanim=0;
				currweaponfired=0;
				currweaponflip=0;
				currweapon=selectedgun;
				oldmousestatus=0;

				if( dahand > 0 )
					dahand=0;

			}

			currweapontics=lefthandanimtics[currweapon][currweaponanim].daweapontics;
			currweaponframe=lefthandanimtics[currweapon][currweaponanim].daweaponframe;
			dax=lefthandanimtics[currweapon][currweaponanim].currx;
			day=lefthandanimtics[currweapon][currweaponanim].curry+8;
		}

			else {
				currweaponframe=lefthandanimtics[currweapon][currweaponanim].daweaponframe;
				dax=lefthandanimtics[currweapon][currweaponanim].currx;
				day=lefthandanimtics[currweapon][currweaponanim].curry+8;
			}
			if ( currweapon == 0 && currweaponframe != 0) {
				if( dahand == 1 )
					overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
				else if ( dahand == 2 ) {
					dax=lefthandanimtics[0][currweaponanim].currx;
					day=lefthandanimtics[0][currweaponanim].curry+8;
					overwritesprite(dax,day+5,currweaponframe+6,dashade,dabits,dapalnum);
				}
			}
			else {
				if( currweaponframe != 0 ) {
					dax=lefthandanimtics[currweapon][currweaponanim].currx;
					overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
				}
			}

		if( currweapon == 0 && currweaponframe == 0) {
			dahand=0;
			oldmousestatus=0;
			currweaponanim=0;
			currweaponfired=0;
		}

		if ( selectedgun == 4 && currweaponframe == 0 ) {
				currweaponanim=0;
				currweaponfired=0;
				currweaponflip=0;
				currweapon=selectedgun;
				oldmousestatus=0;
		}

		break;
		case 1: // fire
			switch(currweapon) {
			case 0: // fist
				if(currweaponframe == RFIST+5)
				if(currweaponanim == 5 && currweapontics == 10)
					swingdasound(currweapon);
			break;
			case 1: // knife
				if(currweaponframe == KNIFEATTACK+6)
					if(currweaponanim == 8 && currweapontics == 8 )
						swingdasound(currweapon);
				if(currweaponframe == KNIFEATTACK2+2)
					if((currweaponanim == 5 || currweaponanim == 9) && currweapontics == 8)
						swingdasound(currweapon);
			break;
			case 2: // shortsword
				if(currweaponframe == GOBSWORDATTACK+4)
					if(currweaponanim == 4 && currweapontics == 10)
						swingdasound(currweapon);
				if(currweaponframe == GOBSWORDATTACK2+4)
					if(currweaponanim == 4 && currweapontics == 10)
						swingdasound(currweapon);
			break;
			case 3: // morning
				if(currweaponframe == MORNINGSTAR+5)
					if(currweaponanim == 7 && currweapontics == 12)
						swingdasound(currweapon);
				if(currweaponframe == MORNINGATTACK2+3)
					if(currweaponanim == 3 && currweapontics == 12)
						swingdasound(currweapon);
			break;
			case 4: // sword
				if(currweaponframe == SWORDATTACK+7)
					if(currweaponanim == 7 && currweapontics == 8)
						swingdasound(currweapon);
				if(currweaponframe == SWORDATTACK2+6)
					if(currweaponanim == 6 && currweapontics == 8)
						swingdasound(currweapon);
			break;
			case 5: // battleaxe
				if(currweaponframe == BIGAXEATTACK+7)
					if(currweaponanim == 7 && currweapontics == 12)
						swingdasound(currweapon);
				if(currweaponframe == BIGAXEATTACK2+6)
					if(currweaponanim == 6 && currweapontics == 12)
						swingdasound(currweapon);
			break;
			case 6: //bow
				if(currweaponframe == BOWWALK+4)
					if(currweaponanim == 4 && currweapontics == 6)
						swingdasound(currweapon);
			break;
			case 7: // pike
				if(currweaponframe == PIKEATTACK1+4)
					if(currweaponanim == 8 && currweapontics == 10)
						swingdasound(currweapon);
				if(currweaponframe == PIKEATTACK2+4)
					if(currweaponanim == 4 && currweapontics == 10)
						swingdasound(currweapon);
			break;
			case 8: // two handed sword
				if(currweaponframe == EXCALATTACK1+7)
					if(currweaponanim == 7 && currweapontics == 8)
						swingdasound(currweapon);
				if(currweaponframe == EXCALATTACK2+5)
					if(currweaponanim == 5 && currweapontics == 8)
						swingdasound(currweapon);
			break;
			case 9: // halberd
				if(currweaponframe == HALBERDATTACK1+3)
					if(currweaponanim == 7 && currweapontics == 12)
						swingdasound(currweapon);
				if(currweaponframe == HALBERDATTACK2+3)
					if(currweaponanim == 4 && currweapontics == 12)
						swingdasound(currweapon);
			break;
			}

			if( currweaponframe == RFIST+5
				|| currweaponframe == KNIFEATTACK+6
				|| currweaponframe == MORNINGSTAR+5
				|| currweaponframe == SWORDATTACK+7
				|| currweaponframe == BOWWALK+5
				|| currweaponframe == KNIFEATTACK2+2
				|| currweaponframe == SWORDATTACK2+6
				|| currweaponframe == MORNINGATTACK2+3
				|| currweaponframe == HALBERDATTACK1+3
				|| currweaponframe == HALBERDATTACK2+3
				|| currweaponframe == BIGAXEATTACK+7
				|| currweaponframe == BIGAXEATTACK2+6
				|| currweaponframe == PIKEATTACK1+4
				|| currweaponframe == PIKEATTACK2+4
				|| currweaponframe == EXCALATTACK1+7
				|| currweaponframe == EXCALATTACK2+5
				|| currweaponframe == GOBSWORDATTACK2+4
				|| currweaponframe == GOBSWORDATTACK+4) {
					swingdaweapon(plr);
			}

			currweapontics-=((int)synctics);
			if( helmettime > 0)
				currweapontics--;

			if((currweaponframe == SWORDATTACK+7
				|| currweaponframe == SWORDATTACK2+7)
				&& currweapontics < 0 && shieldpoints <= 0) {
				if(krand()%2 == 0) {
					/*
					if( plr->lvl <= 3 ) {
						if( krand()%2 == 0 )
							currweapon=3;
						else
							currweapon=4;
						currweapontics=6;
						currweaponanim=0;
						currweaponfired=6;
						currweaponflip=1;
					}
					else*/
					if(plr->lvl >= 4 && plr->lvl <= 4 && plr->ammo[1] > 0) {
						currweapon=1;
						currweapontics=6;
						currweaponanim=0;
						currweaponfired=6;
						currweaponflip=1;
					}
					else if(plr->lvl >= 5 && plr->ammo[3] > 0) {
						currweapon=3;
						currweapontics=6;
						currweaponanim=0;
						currweaponfired=6;
						currweaponflip=1;
					}
				}
				/*
				else {
				//else if( krand()%100 > 50) {
						if( krand()%2 == 0 )
							currweapon=3;
						else
							currweapon=4;
						currweapontics=6;
						currweaponanim=0;
						currweaponfired=6;
						currweaponflip=1;
				}
				*/
			}
			if( currweapontics < 0 ) {
				currweaponanim++;
				if( currweaponanim > 11 ) {
					//impact=0;
					currweaponanim=0;
					currweaponfired=0;
					oldmousestatus=0; // NEW
					if( dahand > 0 )
						dahand = 0;
				}
				if( currweaponattackstyle == 0 ) {
					currweapontics=weaponanimtics[currweapon][currweaponanim].daweapontics;
					currweaponframe=weaponanimtics[currweapon][currweaponanim].daweaponframe;
					dax=weaponanimtics[currweapon][currweaponanim].currx+8;
					day=weaponanimtics[currweapon][currweaponanim].curry+4;
				}
				else {
					currweapontics=weaponanimtics2[currweapon][currweaponanim].daweapontics;
					currweaponframe=weaponanimtics2[currweapon][currweaponanim].daweaponframe;
					dax=weaponanimtics2[currweapon][currweaponanim].currx+8;
					day=weaponanimtics2[currweapon][currweaponanim].curry+4;
				}
			}
			else {
				if( currweaponattackstyle == 0 ) {
					//flip
					currweaponframe=weaponanimtics[currweapon][currweaponanim].daweaponframe;
					dax=weaponanimtics[currweapon][currweaponanim].currx;
					day=weaponanimtics[currweapon][currweaponanim].curry+4;
				}
				else {
					//flip
					currweaponframe=weaponanimtics2[currweapon][currweaponanim].daweaponframe;
					dax=weaponanimtics2[currweapon][currweaponanim].currx;
					day=weaponanimtics2[currweapon][currweaponanim].curry+4;
				}

			}
			if ( currweapon == 0 && currweaponframe != 0) {
				if( dahand == 1 )
					overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
				else if ( dahand == 2 ) {
					dax=lefthandanimtics[0][currweaponanim].currx;
					day=lefthandanimtics[0][currweaponanim].curry+8;
					overwritesprite(dax,day+5,currweaponframe+6,dashade,dabits,dapalnum);
				}
			}
			else {
				if( currweaponframe != 0 ) {
					if( currweaponattackstyle == 0 )
						//flip
						dax=weaponanimtics[currweapon][currweaponanim].currx;
					else
						//flip
						dax=weaponanimtics2[currweapon][currweaponanim].currx;

					overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
				}
			}

		if( currweapon == 0 && currweaponframe == 0) {
			dahand=0;
			oldmousestatus=0; // NEW
			currweaponanim=0;
			currweaponfired=0;
		}
		break;


		case 0: //walking

			currweapontics=weaponanimtics[currweapon][0].daweapontics;

			if( currweapon == 6 && plr->ammo[6] <= 0 )
				currweaponframe=BOWREADYEND;
			else
				currweaponframe=weaponanimtics[currweapon][0].daweaponframe;

			if( vel != 0 ) {
				snakex=(sintable[(lockclock<<4)&2047]>>12);
				snakey=(sintable[(totalclock<<4)&2047]>>12);

				if( plr->screensize <= 320 ) {
					if( currweaponframe == BOWREADYEND ) {
						day=readyanimtics[currweapon][6].curry+snakey+16;
						dax=readyanimtics[currweapon][6].currx+snakex+16;
					}
					else {
						day=weaponanimtics[currweapon][0].curry+snakey+16;
						dax=weaponanimtics[currweapon][0].currx+snakex+16;
					}
				}
				else {
					if( currweaponframe == BOWREADYEND ) {
						day=readyanimtics[currweapon][6].curry+snakey+8;
						dax=readyanimtics[currweapon][6].currx+snakex+8;
					}
					else {
						day=weaponanimtics[currweapon][0].curry+snakey+8;
						dax=weaponanimtics[currweapon][0].currx+snakex+8;
					}
				}
			}
			else {
				if( currweaponframe == BOWREADYEND ) {
					day=readyanimtics[currweapon][6].curry+3;
					dax=readyanimtics[currweapon][6].currx+3;
				}
				else {
					dax=weaponanimtics[currweapon][0].currx+3;
					day=weaponanimtics[currweapon][0].curry+3;
				}
			}
			if ( currweapon == 0 && currweaponframe != 0) {
				overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
				overwritesprite(0,day+8,currweaponframe+6,dashade,dabits,dapalnum);
			}
			else  {
				if( currweaponframe != 0 ) {
					overwritesprite(dax+snakex,day,currweaponframe,dashade,dabits,dapalnum);
				}
			}
		break;
		case 2: // unready
			if( currweapon == 1 )
				weapondrop+=synctics<<1;
			else
				weapondrop+=synctics;
			if( weapondrop > weapondropgoal ) {
				currweaponfired=3;
				weaponraise=40;
				currweapon=selectedgun;
				weaponuseless=0;
				//hasshot=0;//just in case of bugg
							//make hasshot=1;
			}

			currweapontics=weaponanimtics[currweapon][0].daweapontics;

			if( currweapon == 6 && plr->ammo[6] <= 0 )
				currweaponframe=BOWREADYEND;
			else
				currweaponframe=weaponanimtics[currweapon][0].daweaponframe;

			if( currweaponframe == BOWREADYEND ) {
				day=readyanimtics[currweapon][6].curry+(weapondrop);
				dax=readyanimtics[currweapon][6].currx;
			}
			else {
				dax=weaponanimtics[currweapon][0].currx;
				day=weaponanimtics[currweapon][0].curry+(weapondrop);
			}

			if ( currweapon == 0 && currweaponframe != 0) {
				overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
				overwritesprite(0,day,currweaponframe+6,dashade,dabits,dapalnum);
			}

			else  {
				if( currweaponframe != 0 ) {
					dax=weaponanimtics[currweapon][0].currx;
					overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
				}
			}
		break;
		case 3: // ready
			currweapontics-=((int)synctics);
			if( currweapontics < 0 ) {
				currweaponanim++;
				if( currweaponanim == 12 ) {
					currweaponanim=0;
					currweaponfired=0;

				currweaponframe=readyanimtics[currweapon][11].daweaponframe;
				dax=readyanimtics[currweapon][11].currx;
				day=readyanimtics[currweapon][11].curry+8;

			if ( currweapon == 0 && currweaponframe != 0) {
				overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
				overwritesprite(0,day,currweaponframe+6,dashade,dabits,dapalnum);
			}
			else  {
				if( currweaponframe != 0 ) {
					overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
				}
			}
					break;
					if( dahand > 0 )
						dahand = 0;
				}
				currweapontics=readyanimtics[currweapon][currweaponanim].daweapontics;
				currweaponframe=readyanimtics[currweapon][currweaponanim].daweaponframe;
				dax=readyanimtics[currweapon][currweaponanim].currx;
				day=readyanimtics[currweapon][currweaponanim].curry+8;
			}
			else {
				currweaponframe=readyanimtics[currweapon][currweaponanim].daweaponframe;
				dax=readyanimtics[currweapon][currweaponanim].currx;
				day=readyanimtics[currweapon][currweaponanim].curry+8;
			}
			if ( currweapon == 0 && currweaponframe != 0) {
				overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
				overwritesprite(0,day,currweaponframe+6,dashade,dabits,dapalnum);
			}
			else  {
				if( currweaponframe != 0 ) {
					overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
				}
			}
		break;

		case 5: //cock
			currweapontics-=((int)synctics);
			if( currweapontics < 0 ) {
				currweaponanim++;
				if( currweaponanim == 4 ) {
					currweaponanim=0;
					currweaponfired=0;

					currweaponframe=cockanimtics[3].daweaponframe;
					dax=cockanimtics[3].currx+3;
					day=cockanimtics[3].curry+3;

					if( currweaponframe != 0 )
						overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
					break;
				}
				currweapontics=cockanimtics[currweaponanim].daweapontics;
				currweaponframe=cockanimtics[currweaponanim].daweaponframe;
				dax=cockanimtics[currweaponanim].currx;
				day=cockanimtics[currweaponanim].curry+8;
			}
			else {
				currweaponframe=cockanimtics[currweaponanim].daweaponframe;
				dax=cockanimtics[currweaponanim].currx;
				day=cockanimtics[currweaponanim].curry+8;
			}
			if( currweaponframe != 0 )
				overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);

		break;

		case 4: // throw the orb

			if(currweaponframe == 0) {
				castaorb(plr);
			}

			currweapontics-=((int)synctics);
			if( currweapontics < 0 ) {
				currweaponanim++;
				if( currweaponanim > 12 ) {
					currweaponanim=0;
					currweaponfired=0;
					orbshot=0;
			//
				currweaponframe=throwanimtics[currentorb][currweaponanim].daweaponframe;
				dax=throwanimtics[currentorb][currweaponanim].currx;
				day=throwanimtics[currentorb][currweaponanim].curry+8;

				if ( currweapon == 0 && currweaponframe != 0) {
					overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
					//overwritesprite(0,day,currweaponframe+6,dashade,dabits,0);
				}
				else  {
					if( currweaponframe != 0 ) {
						overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
					}
				}
			//

				break;
				}
				currweapontics=throwanimtics[currentorb][currweaponanim].daweapontics;
				currweaponframe=throwanimtics[currentorb][currweaponanim].daweaponframe;
				dax=throwanimtics[currentorb][currweaponanim].currx;
				day=throwanimtics[currentorb][currweaponanim].curry+8;
			}
			else {
				currweaponframe=throwanimtics[currentorb][currweaponanim].daweaponframe;
				dax=throwanimtics[currentorb][currweaponanim].currx;
				day=throwanimtics[currentorb][currweaponanim].curry+8;
			}
			if( currweaponframe != 0 ) {
				overwritesprite(dax,day,currweaponframe,dashade,dabits,dapalnum);
			}
		break;
	}

	//shield stuff

	if( shieldpoints > 0
		&& (currweaponfired == 0
		|| currweaponfired == 1)
		&& selectedgun > 0
		&& selectedgun < 5) {

		if( currweaponfired == 1 ) {
			snakex=(sintable[(lockclock<<4)&2047]>>12);
			snakey=(sintable[(totalclock<<4)&2047]>>12);
		}

		if( shieldpoints > 75 ) {
			overwritesprite(-40+snakex,70+snakey,GRONSHIELD,dashade,dabits,dapalnum);
		}
		else if( shieldpoints > 50 && shieldpoints < 76 ) {
			overwritesprite(-40+snakex,70+snakey,GRONSHIELD+1,dashade,dabits,dapalnum);
		}
		else if( shieldpoints > 25 && shieldpoints < 51 ) {
			overwritesprite(-40+snakex,70+snakey,GRONSHIELD+2,dashade,dabits,dapalnum);
		}
		else {
			overwritesprite(-40+snakex,70+snakey,GRONSHIELD+3,dashade,dabits,dapalnum);
		}

	}

	//
	// spellbook
	//

	if( spellbookflip == 1) {
		if( plr->screensize <= 320 )
			rotatesprite(121<<16,154<<16,65536,0,SPELLBOOKBACK,0,0,2+8+16,0,0,xdim-1,ydim-1);

		spellbooktics-=synctics;
		if( spellbooktics < 0 ) {
			spellbook++;
			if( spellbook > 8 )
				spellbook=8;
			if( spellbook == 8 ) {
				spellbooktics=spellbookanim[currentorb][8].daweapontics;
				spellbookframe=spellbookanim[currentorb][8].daweaponframe;
				dax=spellbookanim[currentorb][8].currx;
				day=spellbookanim[currentorb][8].curry;
				rotatesprite(dax<<16,day<<16,65536,0,spellbookframe,0,0,2+8+16,0,0,xdim-1,ydim-1);
				spellbookflip=0;
				return;
			}
			else {
				spellbooktics=spellbookanim[currentorb][spellbook].daweapontics;
				spellbookframe=spellbookanim[currentorb][spellbook].daweaponframe;
				dax=spellbookanim[currentorb][spellbook].currx;
				day=spellbookanim[currentorb][spellbook].curry;
				rotatesprite(dax<<16,day<<16,65536,0,spellbookframe,0,0,2+8+16,0,0,xdim-1,ydim-1);
			}
		}
		else {
			spellbookframe=spellbookanim[currentorb][spellbook].daweaponframe;
			dax=spellbookanim[currentorb][spellbook].currx;
			day=spellbookanim[currentorb][spellbook].curry;
			rotatesprite(dax<<16,day<<16,65536,0,spellbookframe,0,0,2+8+16,0,0,xdim-1,ydim-1);
		}
	}


}

void castaorb( struct player *plr ) {

	int k;
	short daang;

	switch(currentorb) {
		case 0: // SCARE
			shadowtime=(plr->lvl*120)<<2;
		break;
		case 1: // NIGHTVISION
			nightglowtime=3600+(plr->lvl*120);
		break;
		case 2: // FREEZE
				daang=plr->ang;
				shootgun( plr, daang, 6);
				playsound_loc(S_SPELL1,plr->x,plr->y);
		break;
		case 3: // MAGIC ARROW
			daang=(plr->ang-36)&2047;
			for(k=0;k<10;k++) {
				daang=(daang+(k<<1))&2047;
				shootgun( plr, daang, 2);
			}
			playsound_loc(S_SPELL1,plr->x,plr->y);
		break;
		case 4: // OPEN DOORS
				daang=plr->ang;
				shootgun( plr, daang, 7);
				playsound_loc(S_SPELL1,plr->x,plr->y);
		break;
		case 5: // FLY
		   plr->orbactive[currentorb]=3600+(plr->lvl*120);
		   playsound_loc(S_SPELL1,plr->x,plr->y);
		break ;
		case 6: // FIREBALL
				daang=plr->ang;
				shootgun( plr, daang, 3);
				playsound_loc(S_SPELL1,plr->x,plr->y);
		break;
		case 7: // NUKE
				daang=plr->ang;
				shootgun( plr, daang, 4);
				playsound_loc(S_SPELL1,plr->x,plr->y);
		break;
	}

}

void chunksofmeat(struct player *plr,short hitsprite,int hitx,int hity,int hitz, short hitsect, short daang) {

	short j;
	short k;
	short zgore;
	int chunk=REDCHUNKSTART;
	int newchunk;

	(void)plr; (void)daang;

	if(goreon == 0)
		return;

	if( sprite[hitsprite].picnum == JUDY
		|| sprite[hitsprite].picnum == JUDYATTACK1
		|| sprite[hitsprite].picnum == JUDYATTACK2 )
		return;


	switch( selectedgun ) {
	case 1:
	case 2:
		zgore=1;
	break;
	case 3:
	case 4:
		zgore=2;
	break;
	case 5:
		zgore=3;
	break;
	case 6:
		zgore=1;
	break;
	case 7:
		zgore=2;
	break;
	case 8:
	case 9:
		zgore=3;
	break;
	}

	if(sprite[hitsprite].picnum==RAT)
		 zgore=1;

	if(sprite[hitsprite].picnum == WILLOW
		|| sprite[hitsprite].picnum == WILLOWEXPLO
		|| sprite[hitsprite].picnum == WILLOWEXPLO+1
		|| sprite[hitsprite].picnum == WILLOWEXPLO+2
		|| sprite[hitsprite].picnum == GUARDIAN
		|| sprite[hitsprite].picnum == GUARDIANATTACK)
		return;

	if(sprite[hitsprite].picnum == SKELETON
		|| sprite[hitsprite].picnum == SKELETONATTACK
		|| sprite[hitsprite].picnum == SKELETONDIE) {
			playsound_loc(S_SKELHIT1 + (rand()%2),sprite[hitsprite].x,sprite[hitsprite].y);
	}
	else {
		if( rand()%100 > 60 )
			playsound_loc(S_GORE1+(rand()%4),sprite[hitsprite].x,sprite[hitsprite].y);
	}

	if((hitsprite >= 0) && (sprite[hitsprite].statnum < MAXSTATUS)) {
		for(k=0;k<zgore;k++) {
			newchunk=0;
			j=insertsprite(hitsect,CHUNKOMEAT);
			sprite[j].x = hitx;
			sprite[j].y = hity;
			sprite[j].z = hitz;
			sprite[j].cstat = 0;
			if(rand()%100 > 50) {
				switch(sprite[hitsprite].picnum) {
				case GRONHAL:
				case GRONHALATTACK:
				case GRONHALPAIN:
				case GRONMU:
				case GRONMUATTACK:
				case GRONMUPAIN:
				case GRONSW:
				case GRONSWATTACK:
				case GRONSWPAIN:
					chunk=REDCHUNKSTART+(rand()%8);
				break;
				case KOBOLD:
				case KOBOLDATTACK:
					if( sprite[hitsprite].pal == 0)
						chunk=BROWNCHUNKSTART+(rand()%8);
					if( sprite[hitsprite].pal == 4)
						chunk=GREENCHUNKSTART+(rand()%8);
					if( sprite[hitsprite].pal == 7)
						chunk=REDCHUNKSTART+(rand()%8);
				break;
				case DRAGON:
				case DRAGONATTACK2:
				case DRAGONATTACK:
					chunk=GREENCHUNKSTART+(rand()%8);
				break;
				case DEVILSTAND:
				case DEVIL:
				case DEVILATTACK:
					chunk=REDCHUNKSTART+(rand()%8);
				break;
				case FREDSTAND:
				case FRED:
				case FREDATTACK:
				case FREDPAIN:
					chunk=BROWNCHUNKSTART+(rand()%8);
				break;
				case GOBLINSTAND:
				case GOBLIN:
				case GOBLINCHILL:
				case GOBLINATTACK:
				case GOBLINPAIN:
					if( sprite[hitsprite].pal == 0)
						chunk=GREENCHUNKSTART+(rand()%8);
					if( sprite[hitsprite].pal == 4)
						chunk=BROWNCHUNKSTART+(rand()%8);
					if( sprite[hitsprite].pal == 5)
						chunk=TANCHUNKSTART+(rand()%8);
				break;
				case MINOTAUR:
				case MINOTAURATTACK:
				case MINOTAURPAIN:
				   chunk=TANCHUNKSTART+(rand()%8);
				break;
				case SPIDER:
				   chunk=GREYCHUNKSTART+(rand()%8);
				break;
				case SKULLY:
				case SKULLYATTACK:
				case FATWITCH:
				case FATWITCHATTACK:
				case JUDYSIT:
				case JUDYSTAND:
				case JUDY:
				case JUDYATTACK1:
				case JUDYATTACK2:
					chunk=REDCHUNKSTART+(rand()%8);
				break;
				}
			}
			else {
				newchunk=1;
				chunk=NEWCHUNK+(rand()%9);
			}
			if(sprite[hitsprite].picnum == SKELETON
				|| sprite[hitsprite].picnum == SKELETONATTACK
				|| sprite[hitsprite].picnum == SKELETONDIE)
				chunk=BONECHUNK1+(rand()%9);

			sprite[j].picnum = chunk;
			sprite[j].shade = -16;
			sprite[j].xrepeat = 64;
			sprite[j].yrepeat = 64;
			sprite[j].clipdist=16;
			sprite[j].ang = ((rand()&1023)-1024)&2047;
			sprite[j].xvel = ((rand()&1023)-512);
			sprite[j].yvel = ((rand()&1023)-512);
			sprite[j].zvel = ((rand()&1023)-512);
			if(newchunk == 1)
				sprite[j].zvel<<=1;
			sprite[j].owner = 4096;
			sprite[j].lotag = 512;
			sprite[j].hitag = 0;
			sprite[j].pal = 0;
			movesprite((short)j,(((int)sintable[(sprite[j].ang+512)&2047])*synctics)<<3,(((int)sintable[sprite[j].ang])*synctics)<<3,0L,4L<<8,4L<<8,0);
		}
	}

}

void swingdapunch(int daweapon) {

				switch(daweapon) {
					case 0://hands
//JSA SPOOGE
						SND_Sound(S_SOCK4);
						SND_Sound(S_PLRPAIN1+(rand()%2));
						healthpic(-1);
						startredflash(10);
					break;
					case 1: // knife
					case 2: //mace
					case 4: // sword
						SND_PlaySound(S_WALLHIT1,0,0,0,0);
					break;
					case 3: //arrow
					break;
					case 5:
					case 6:
					case 7:
					case 8:
						SND_PlaySound(S_WALLHIT1,0,0,0,0);
					break;

				}


}

void shootgun ( struct player *plr, short daang, char guntype ) {

	short hitsect, hitwall, hitsprite, daang2, k;
	int i, j, daz2, hitx, hity, hitz;
	int dax, day;

	switch(guntype) {
	case 0:
		daang2 = daang + 2048;
		daz2 = (100-plr->horiz)*2000;
		hitscan(plr->x,plr->y,plr->z,plr->sector,                   //Start position
			sintable[(daang2+2560)&2047],           //X vector of 3D ang
			sintable[(daang2+2048)&2047],           //Y vector of 3D ang
			daz2,                                   //Z vector of 3D ang
			&hitsect,&hitwall,&hitsprite,&hitx,&hity,&hitz,CLIPMASK1);

		if(hitsprite >= 0)
			madeahit=1;

		if( hitwall >= 0 ) {
			if( (labs(plr->x-hitx)+labs(plr->y-hity) < 512) && (labs((plr->z>>8)-((hitz>>8)-(64))) <= (512>>3)) ) {
				madeahit=1;
				switch(currweapon) {
				case 0: // fist
					if(currweaponframe == RFIST+5)
					if(currweaponanim == 5 && currweapontics == 10)
						swingdapunch(currweapon);
				break;
				case 1: // knife
					if(currweaponframe == KNIFEATTACK+6)
						if(currweaponanim == 8 && currweapontics == 8 )
							swingdapunch(currweapon);
					if(currweaponframe == KNIFEATTACK2+2)
						if((currweaponanim == 5 || currweaponanim == 9) && currweapontics == 8)
							swingdapunch(currweapon);
				break;
				case 2: // short sword
					if(currweaponframe == GOBSWORDATTACK+4)
						if(currweaponanim == 4 && currweapontics == 10)
							swingdapunch(currweapon);
					if(currweaponframe == GOBSWORDATTACK+4)
						if(currweaponanim == 4 && currweapontics == 10)
							swingdapunch(currweapon);
				break;
				case 3: // morning
					if(currweaponframe == MORNINGSTAR+5)
						if(currweaponanim == 7 && currweapontics == 12)
							swingdapunch(currweapon);
					if(currweaponframe == MORNINGATTACK2+3)
						if(currweaponanim == 3 && currweapontics == 12)
							swingdapunch(currweapon);
				break;
				case 4: // sword
					if(currweaponframe == SWORDATTACK+7)
						if(currweaponanim == 7 && currweapontics == 8) {
							swingdapunch(currweapon);
							madenoise (2, plr->x, plr->y, plr->z);
						}
					if(currweaponframe == SWORDATTACK2+6)
						if(currweaponanim == 6 && currweapontics == 8) {
							swingdapunch(currweapon);
							madenoise (2, plr->x, plr->y, plr->z);
						}
				break;
				case 5: // battleaxe
					if(currweaponframe == BIGAXEATTACK+7)
						if(currweaponanim == 7 && currweapontics == 12)
							swingdapunch(currweapon);
					if(currweaponframe == BIGAXEATTACK2+6)
						if(currweaponanim == 6 && currweapontics == 12)
							swingdapunch(currweapon);
				break;
				case 6: // bow
					if(currweaponframe == BOWWALK+4)
						if(currweaponanim == 4 && currweapontics == 6)
							swingdapunch(currweapon);
				break;
				case 7: // pike
					if(currweaponframe == PIKEATTACK1+4)
						if(currweaponanim == 8 && currweapontics == 10)
							swingdapunch(currweapon);
					if(currweaponframe == PIKEATTACK2+4)
						if(currweaponanim == 4 && currweapontics == 10)
							swingdapunch(currweapon);
				break;
				case 8: // two handed sword
					if(currweaponframe == EXCALATTACK1+7)
						if(currweaponanim == 7 && currweapontics == 8)
							swingdapunch(currweapon);
					if(currweaponframe == EXCALATTACK2+5)
						if(currweaponanim == 5 && currweapontics == 8)
							swingdapunch(currweapon);
				break;
				case 9: // halberd
					if(currweaponframe == HALBERDATTACK1+3)
						if(currweaponanim == 6 && currweapontics == 12)
							swingdapunch(currweapon);
					if(currweaponframe == HALBERDATTACK2+3)
						if(currweaponanim == 4 && currweapontics == 12)
							swingdapunch(currweapon);
				break;
				}
			}
		}
		if( checkweapondist(hitsprite,plr->x,plr->y,plr->z,guntype) ) {
			switch(sprite[hitsprite].picnum) {
			case BARREL:
			case VASEA:
			case VASEB:
			case VASEC:
				newstatus(hitsprite,BROKENVASE);
			break;
			case GRONHAL:
			case GRONHALATTACK:
			case GRONHALPAIN:
			case GRONMU:
			case GRONMUATTACK:
			case GRONMUPAIN:
			case GRONSW:
			case GRONSWATTACK:
			case GRONSWPAIN:
			case KOBOLD:
			case KOBOLDATTACK:
			case DRAGON:
			case DRAGONATTACK2:
			case DRAGONATTACK:
			case DEVILSTAND:
			case DEVIL:
			case DEVILATTACK:
			case FREDSTAND:
			case FRED:
			case FREDATTACK:
			case FREDPAIN:
			case SKELETON:
			case SKELETONATTACK:
			case GOBLINSTAND:
			case GOBLIN:
			case GOBLINCHILL:
			//case GOBLINSURPRISE:
			case GOBLINATTACK:
			case GOBLINPAIN:
			case MINOTAUR:
			case MINOTAURATTACK:
			case MINOTAURPAIN:
			case SPIDER:
			case SKULLY:
			case SKULLYATTACK:
			case FATWITCH:
			case FATWITCHATTACK:
			case FISH:
			case RAT:
			case GUARDIAN:
			case GUARDIANATTACK:
			case WILLOW:
			case JUDYSIT:
			case JUDYSTAND:
			case JUDY:
			case JUDYATTACK1:
			case JUDYATTACK2:

				if (netgame) {
					netshootgun(hitsprite,currweapon);
				}

				if(invisibletime > 0)
					if( (krand()&32) > 15 )
						invisibletime=-1;
						switch(selectedgun) {
						case 0: // fist
							k=(krand()&5)+1;
						break;
						case 1: // dagger
							if(currweaponattackstyle == 0)
								k=(krand()&5)+10;
							else
								k=(krand()&3)+5;
						break;
						case 2: // short sword
							if(currweaponattackstyle == 0)
								k=(krand()&10)+10;
							else
								k=(krand()&6)+10;
						break;
						case 3: // morning star
							if(currweaponattackstyle == 0)
								k=(krand()&8)+10;
							else
								k=(krand()&8)+15;
						break;
						case 4: // broad sword
							if(currweaponattackstyle == 0)
								k=(krand()&5)+20;
							else
								k=(krand()&5)+15;
						break;
						case 5: // battle axe
							if(currweaponattackstyle == 0)
								k=(krand()&5)+25;
							else
								k=(krand()&5)+20;
						break;
						case 6: // bow
							if(currweaponattackstyle == 0)
								k=(krand()&15)+5;
							else
								k=(krand()&15)+5;
						break;
						case 7: // pike axe
							if(currweaponattackstyle == 0)
								k=(krand()&15)+10;
							else
								k=(krand()&15)+5;
						break;
						case 8: // two handed sword
							if(currweaponattackstyle == 0)
								k=(krand()&15)+45;
							else
								k=(krand()&15)+40;
						break;
						case 9: // halberd
							if(currweaponattackstyle == 0)
								k=(krand()&15)+25;
							else
								k=(krand()&15)+15;
						break;

						}
						k+=plr->lvl;
						if(vampiretime > 0) {
							if( plr->health <= plr->maxhealth )
								healthpic((rand()%10)+1);
						}
						if(helmettime > 0)
							k<<=1;
						if(strongtime > 0) {
							k+=k>>1;

							switch(currweapon) {
							case 0: // fist
								if(currweaponframe == RFIST+5)
									if(currweaponanim == 5 && currweapontics == 10)
										swingdacrunch(currweapon);
							break;
							case 1: // knife
								if(currweaponframe == KNIFEATTACK+6)
									if(currweaponanim == 8 && currweapontics == 8 )
										swingdacrunch(currweapon);
								if(currweaponframe == KNIFEATTACK2+2)
									if((currweaponanim == 5 || currweaponanim == 9) && currweapontics == 8)
										swingdacrunch(currweapon);
							break;
							case 2: // short sword
								if(currweaponframe == GOBSWORDATTACK+4)
									if(currweaponanim == 4 && currweapontics == 10)
										swingdacrunch(currweapon);
								if(currweaponframe == GOBSWORDATTACK2+4)
									if(currweaponanim == 4 && currweapontics == 10)
										swingdacrunch(currweapon);
							break;
							case 3: // morning
								if(currweaponframe == MORNINGSTAR+5)
									if(currweaponanim == 7 && currweapontics == 12)
										swingdacrunch(currweapon);
								if(currweaponframe == MORNINGATTACK2+3)
									if(currweaponanim == 3 && currweapontics == 12)
										swingdacrunch(currweapon);
							break;
							case 4: // sword
								if(currweaponframe == SWORDATTACK+7)
									if(currweaponanim == 7 && currweapontics == 8)
										swingdacrunch(currweapon);
								if(currweaponframe == SWORDATTACK2+6)
									if(currweaponanim == 6 && currweapontics == 8)
										swingdacrunch(currweapon);
							break;
							case 5: // battleaxe
								if(currweaponframe == BIGAXEATTACK+7)
									if(currweaponanim == 7 && currweapontics == 12)
										swingdacrunch(currweapon);
								if(currweaponframe == BIGAXEATTACK2+6)
									if(currweaponanim == 6 && currweapontics == 12)
										swingdacrunch(currweapon);
							break;
							case 6: // bow
								if(currweaponframe == BOWWALK+4)
									if(currweaponanim == 4 && currweapontics == 6)
										swingdacrunch(currweapon);
							break;
							case 7: // pike
								if(currweaponframe == PIKEATTACK1+4)
									if(currweaponanim == 8 && currweapontics == 10)
										swingdacrunch(currweapon);
								if(currweaponframe == PIKEATTACK2+4)
									if(currweaponanim == 4 && currweapontics == 10)
										swingdacrunch(currweapon);
							break;
							case 8: // two handed sword
								if(currweaponframe == EXCALATTACK1+7)
									if(currweaponanim == 7 && currweapontics == 8)
										swingdacrunch(currweapon);
								if(currweaponframe == EXCALATTACK2+5)
									if(currweaponanim == 5 && currweapontics == 8)
										swingdacrunch(currweapon);
							break;
							case 9: // halberd
								if(currweaponframe == HALBERDATTACK1+3)
									if(currweaponanim == 6 && currweapontics == 12)
										swingdacrunch(currweapon);
								if(currweaponframe == HALBERDATTACK2+3)
									if(currweaponanim == 4 && currweapontics == 12)
										swingdacrunch(currweapon);
							break;
							}
							sprite[hitsprite].hitag-=(k<<1);
							if( currweapon != 0 ) {
//JSA GORE1 you have strong time
								if( rand()%100 > 50 ) {
									if( sprite[hitsprite].picnum == SKELETON
										|| sprite[hitsprite].picnum == SKELETONATTACK
										|| sprite[hitsprite].picnum == SKELETONDIE)
										playsound_loc(S_SKELHIT1+(rand()%2),sprite[hitsprite].x,sprite[hitsprite].y);
								}

							//HERE
							switch(currweapon) {
							case 0: // fist
							break;
							case 1: // knife
								if(currweaponframe == KNIFEATTACK+6)
									if(currweaponanim == 8 && currweapontics == 8 )
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == KNIFEATTACK2+2)
									if((currweaponanim == 5 || currweaponanim == 9) && currweapontics == 8)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							case 2: // short sword
								if(currweaponframe == GOBSWORDATTACK+4)
									if(currweaponanim == 4 && currweapontics == 10)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == GOBSWORDATTACK2+4)
									if(currweaponanim == 4 && currweapontics == 10)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							case 3: // morning
								if(currweaponframe == MORNINGSTAR+5)
									if(currweaponanim == 7 && currweapontics == 12)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == MORNINGATTACK2+3)
									if(currweaponanim == 3 && currweapontics == 12)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							case 4: // sword
								if(currweaponframe == SWORDATTACK+7)
									if(currweaponanim == 7 && currweapontics == 8)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == SWORDATTACK2+6)
									if(currweaponanim == 6 && currweapontics == 8)
							break;      chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							case 5: // battleaxe
								if(currweaponframe == BIGAXEATTACK+7)
									if(currweaponanim == 7 && currweapontics == 12)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == BIGAXEATTACK2+6)
									if(currweaponanim == 6 && currweapontics == 12)
							break;      chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							case 6: // bow
								if(currweaponframe == BOWWALK+4)
									if(currweaponanim == 4 && currweapontics == 6)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							case 7: // pike
								if(currweaponframe == PIKEATTACK1+4)
									if(currweaponanim == 8 && currweapontics == 10)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == PIKEATTACK2+4)
									if(currweaponanim == 4 && currweapontics == 10)
							break;      chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							case 8: // two handed sword
								if(currweaponframe == EXCALATTACK1+7)
									if(currweaponanim == 7 && currweapontics == 8)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == EXCALATTACK2+5)
									if(currweaponanim == 5 && currweapontics == 8)
							break;      chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							case 9: // halberd
								if(currweaponframe == HALBERDATTACK1+3)
									if(currweaponanim == 6 && currweapontics == 12)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == HALBERDATTACK2+3)
									if(currweaponanim == 4 && currweapontics == 12)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							}
								//ENDOFHERE
								//chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							}
						}
						else {
							switch(currweapon) {
							case 0: // fist
								if(currweaponframe == RFIST+5)
									if(currweaponanim == 5 && currweapontics == 10)
										swingdacrunch(currweapon);
							break;
							case 1: // knife
								if(currweaponframe == KNIFEATTACK+6)
									if(currweaponanim == 8 && currweapontics == 8 )
										swingdacrunch(currweapon);
								if(currweaponframe == KNIFEATTACK2+2)
									if((currweaponanim == 5 || currweaponanim == 9) && currweapontics == 8)
										swingdacrunch(currweapon);
							break;
							case 2: // SHORT SWORD
								if(currweaponframe == GOBSWORDATTACK+4)
									if(currweaponanim == 4 && currweapontics == 10)
										swingdacrunch(currweapon);
								if(currweaponframe == GOBSWORDATTACK2+4)
									if(currweaponanim == 4 && currweapontics == 10)
										swingdacrunch(currweapon);
							break;
							case 3: // morning
								if(currweaponframe == MORNINGSTAR+5)
									if(currweaponanim == 7 && currweapontics == 12)
										swingdacrunch(currweapon);
								if(currweaponframe == MORNINGATTACK2+3)
									if(currweaponanim == 3 && currweapontics == 12)
										swingdacrunch(currweapon);
							break;
							case 4: // sword
								if(currweaponframe == SWORDATTACK+7)
									if(currweaponanim == 7 && currweapontics == 8)
										swingdacrunch(currweapon);
								if(currweaponframe == SWORDATTACK2+6)
									if(currweaponanim == 6 && currweapontics == 8)
										swingdacrunch(currweapon);
							break;
							case 5: // battleaxe
								if(currweaponframe == BIGAXEATTACK+7)
									if(currweaponanim == 7 && currweapontics == 12)
										swingdacrunch(currweapon);
								if(currweaponframe == BIGAXEATTACK2+6)
									if(currweaponanim == 6 && currweapontics == 12)
										swingdacrunch(currweapon);
							break;
							case 6: // bow
								if(currweaponframe == BOWWALK+4)
									if(currweaponanim == 4 && currweapontics == 6)
										swingdacrunch(currweapon);
							break;

							case 7: // pike
								if(currweaponframe == PIKEATTACK1+4)
									if(currweaponanim == 8 && currweapontics == 10)
										swingdacrunch(currweapon);
								if(currweaponframe == PIKEATTACK2+4)
									if(currweaponanim == 4 && currweapontics == 10)
										swingdacrunch(currweapon);
							break;
							case 8: // two handed sword
								if(currweaponframe == EXCALATTACK1+7)
									if(currweaponanim == 7 && currweapontics == 8)
										swingdacrunch(currweapon);
								if(currweaponframe == EXCALATTACK2+5)
									if(currweaponanim == 5 && currweapontics == 8)
										swingdacrunch(currweapon);
							break;
							case 9: // halberd
								if(currweaponframe == HALBERDATTACK1+3)
									if(currweaponanim == 6 && currweapontics == 12)
										swingdacrunch(currweapon);
								if(currweaponframe == HALBERDATTACK2+3)
									if(currweaponanim == 4 && currweapontics == 12)
										swingdacrunch(currweapon);
							break;
							}
							sprite[hitsprite].hitag-=k;
							if( currweapon != 0 ) {
//JSA GORE normal
								if( rand()%100 > 50 ) {
									if( sprite[hitsprite].picnum == SKELETON
										|| sprite[hitsprite].picnum == SKELETONATTACK
										|| sprite[hitsprite].picnum == SKELETONDIE)
										playsound_loc(S_SKELHIT1+(rand()%2),sprite[hitsprite].x,sprite[hitsprite].y);
								}
							//HERE
							switch(currweapon) {
							case 0: // fist
							break;
							case 1: // knife
								if(currweaponframe == KNIFEATTACK+6)
									if(currweaponanim == 8 && currweapontics == 8 )
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == KNIFEATTACK2+2)
									if((currweaponanim == 5 || currweaponanim == 9) && currweapontics == 8)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							case 2: // short sword
								if(currweaponframe == GOBSWORDATTACK+4)
									if(currweaponanim == 4 && currweapontics == 10)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == GOBSWORDATTACK2+4)
									if(currweaponanim == 4 && currweapontics == 10)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							case 3: // morning
								if(currweaponframe == MORNINGSTAR+5)
									if(currweaponanim == 7 && currweapontics == 12)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == MORNINGATTACK2+3)
									if(currweaponanim == 3 && currweapontics == 12)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							case 4: // sword
								if(currweaponframe == SWORDATTACK+7)
									if(currweaponanim == 7 && currweapontics == 8)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == SWORDATTACK2+6)
									if(currweaponanim == 6 && currweapontics == 8)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							case 5: // battleaxe
								if(currweaponframe == BIGAXEATTACK+7)
									if(currweaponanim == 7 && currweapontics == 12)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == BIGAXEATTACK2+6)
									if(currweaponanim == 6 && currweapontics == 12)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							case 6: // bow
								if(currweaponframe == BOWWALK+4)
									if(currweaponanim == 4 && currweapontics == 6)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							case 7: // pike
								if(currweaponframe == PIKEATTACK1+4)
									if(currweaponanim == 8 && currweapontics == 10)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == PIKEATTACK2+4)
									if(currweaponanim == 4 && currweapontics == 10)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							case 8: // two handed sword
								if(currweaponframe == EXCALATTACK1+7)
									if(currweaponanim == 7 && currweapontics == 8)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == EXCALATTACK2+5)
									if(currweaponanim == 5 && currweapontics == 8)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							case 9: // halberd
								if(currweaponframe == HALBERDATTACK1+3)
									if(currweaponanim == 6 && currweapontics == 12)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if(currweaponframe == HALBERDATTACK2+3)
									if(currweaponanim == 4 && currweapontics == 12)
										chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							break;
							}
								//ENDOFHERE

								//chunksomeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
							}
						}

						if (netgame) {
							break;
						}

						if( sprite[hitsprite].hitag <= 0 ) {
							if( selectedgun > 1 ) {
//JSA GORE on death ?
//RAF ans:death
								chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
								if( sprite[hitsprite].picnum == SKELETON
									|| sprite[hitsprite].picnum == SKELETONATTACK
									|| sprite[hitsprite].picnum == SKELETONDIE)
										playsound_loc(S_SKELHIT1+(rand()%2),sprite[hitsprite].x,sprite[hitsprite].y);
							}
							newstatus(hitsprite,DIE);
						}
						sprite[hitsprite].ang=plr->ang+((krand()&32)-64);
						if( sprite[hitsprite].hitag > 0)
							newstatus(hitsprite,PAIN);
			break;
			case GRONHALDIE:
			case GRONMUDIE:
			case GRONSWDIE:
			case KOBOLDDIE:
			case DRAGONDIE:
			case DEVILDIE:
			case FREDDIE:
			case SKELETONDIE:
			case GOBLINDIE:
			case MINOTAURDIE:
			case SKULLYDIE:
			case SPIDERDIE:
			case FATWITCHDIE:
			case JUDYDIE:
			//case FISH:
			//case RAT:
			//case WILLOW:
				if( sprite[hitsprite].pal == 6 ) {
					//JSA_NEW
					SND_PlaySound(S_SOCK1+(rand()%4),0,0,0,0);
					playsound_loc(S_FREEZEDIE,hitx,hity);
					for(k=0;k<32;k++)
						icecubes(hitsprite, hitx, hity, hitz, hitsprite);
				deletesprite((short)hitsprite);
				}
			} // switch
		} // if weapondist
	if(madeahit == 0) {
		plr->ammo[currweapon]++;
		madeahit=1;
	}
	break;
	case 1:

		daang2 = (daang + 2048)&2047;
		daz2 = (100-plr->horiz)*2000;

		hitscan(plr->x,plr->y,plr->z,plr->sector,                   //Start position
			sintable[(daang2+2560)&2047],           //X vector of 3D ang
			sintable[(daang2+2048)&2047],           //Y vector of 3D ang
			daz2,                                   //Z vector of 3D ang
			&hitsect,&hitwall,&hitsprite,&hitx,&hity,&hitz,CLIPMASK1);

		if(hitwall > 0 && hitsprite < 0) {
			neartag(hitx,hity,hitz,hitsect,daang,
					&neartagsector,&neartagwall,&neartagsprite,&neartaghitdist,1024,3);
			if(neartagsector < 0) {
				if(sector[neartagsector].lotag == 0) {
					j=insertsprite(hitsect,0);
					sprite[j].x=hitx;
					sprite[j].y=hity;
					sprite[j].z=hitz+(8<<8);
					sprite[j].cstat=17;//was16
					sprite[j].picnum=WALLARROW;
					sprite[j].shade=0;
					sprite[j].pal=0;
					sprite[j].xrepeat=16;
					sprite[j].yrepeat=48;
					sprite[j].ang=((daang+2048)-512+(rand()&128-64))&2047;
					sprite[j].xvel=0;
					sprite[j].yvel=0;
					sprite[j].zvel=0;
					sprite[j].owner=4096;
					sprite[j].lotag=32;
					sprite[j].hitag=0;
					playsound_loc(S_ARROWHIT,sprite[j].x,sprite[j].y);
				}
			}

			if (netgame) {
				netshootgun(-1,5);
			}

		}
		if(hitwall > 0 && hitsprite > 0) {
			j=insertsprite(hitsect,FX);
			sprite[j].x=hitx;
			sprite[j].y=hity;
			sprite[j].z=hitz+(8<<8);
			sprite[j].cstat=2;
			sprite[j].picnum=PLASMA;
			sprite[j].shade=-32;
			sprite[j].pal=0;
			sprite[j].xrepeat=32;
			sprite[j].yrepeat=32;
			sprite[j].ang=daang;
			sprite[j].xvel=0;
			sprite[j].yvel=0;
			sprite[j].zvel=0;
			sprite[j].owner=4096;
			sprite[j].lotag=32;
			sprite[j].hitag=0;
			movesprite((short)j,(((int)sintable[(sprite[j].ang+512)&2047])*synctics)<<3,(((int)sintable[sprite[j].ang])*synctics)<<3,0L,4L<<8,4L<<8,0);
		}
		if ((hitsprite >= 0) && (sprite[hitsprite].statnum < MAXSTATUS)) {
			switch(sprite[hitsprite].picnum) {
			case VASEA:
			case VASEB:
			case VASEC:
				newstatus(hitsprite,BROKENVASE);
			break;
			case GRONHAL:
			case GRONHALATTACK:
			case GRONHALPAIN:
			case GRONMU:
			case GRONMUATTACK:
			case GRONMUPAIN:
			case GRONSW:
			case GRONSWATTACK:
			case GRONSWPAIN:
			case KOBOLD:
			case KOBOLDATTACK:
			case DRAGON:
			case DRAGONATTACK2:
			case DRAGONATTACK:
			case DEVILSTAND:
			case DEVIL:
			case DEVILATTACK:
			case FREDSTAND:
			case FRED:
			case FREDATTACK:
			case FREDPAIN:
			case SKELETON:
			case SKELETONATTACK:
			case GOBLINSTAND:
			case GOBLIN:
			case GOBLINCHILL:
			//case GOBLINSURPRISE:
			case GOBLINATTACK:
			case GOBLINPAIN:
			case MINOTAUR:
			case MINOTAURATTACK:
			case MINOTAURPAIN:
			case SPIDER:
			case SKULLY:
			case SKULLYATTACK:
			case FATWITCH:
			case FATWITCHATTACK:
			case FISH:
			case RAT:
			case WILLOW:
			case GUARDIAN:
			case GUARDIANATTACK:
			case JUDYSIT:
			case JUDYSTAND:
			case JUDY:
			case JUDYATTACK1:
			case JUDYATTACK2:

				if (netgame) {
					netshootgun(hitsprite,currweapon);
					break;
				}

				sprite[hitsprite].hitag-=(krand()&15)+15;
				if(sprite[hitsprite].hitag <= 0) {
					newstatus(hitsprite,DIE);
					if( sprite[hitsprite].picnum == RAT )
						chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
				}
				else {
					sprite[hitsprite].ang=(getangle(plr->x-sprite[hitsprite].x,plr->y-sprite[hitsprite].y)&2047);
					newstatus(hitsprite,PAIN);
				}
			break;
			// SHATTER FROZEN CRITTER
			case GRONHALDIE:
			case GRONMUDIE:
			case GRONSWDIE:
			case KOBOLDDIE:
			case DRAGONDIE:
			case DEVILDIE:
			case FREDDIE:
			case SKELETONDIE:
			case GOBLINDIE:
			case MINOTAURDIE:
			case SKULLYDIE:
			case SPIDERDIE:
			case FATWITCHDIE:
			case JUDYDIE:
			//case FISH:
			//case RAT:
			//case WILLOW:
				if( sprite[hitsprite].pal == 6 ) {
					//JSA_NEW
					SND_PlaySound(S_SOCK1+(rand()%4),0,0,0,0);
					playsound_loc(S_FREEZEDIE,hitx,hity);
					for(k=0;k<32;k++)
						icecubes(hitsprite, hitx, hity, hitz, hitsprite);
				deletesprite((short)hitsprite);
				}
			} // switch
		}
	break;

	case 6: // MEDUSA
		for(i=0;i<MAXSPRITES;i++) {
			//cansee
			if( i != plr->spritenum )
			if(sprite[i].picnum == FRED
				|| sprite[i].picnum == KOBOLD
				|| sprite[i].picnum == GOBLIN
				|| sprite[i].picnum == MINOTAUR
				|| sprite[i].picnum == SPIDER
				|| sprite[i].picnum == SKELETON
				|| sprite[i].picnum == GRONHAL
				|| sprite[i].picnum == GRONMU
				|| sprite[i].picnum == GRONSW)
				if( cansee(plr->x,plr->y,plr->z,plr->sector,sprite[i].x,sprite[i].y,sprite[i].z-(tilesizy[sprite[i].picnum]<<7),sprite[i].sectnum) == 1 ) {
				//distance check
					if(checkmedusadist(i,plr->x,plr->y,plr->z,plr->lvl) )
					//medusa
						medusa(i);
				}
		}
	break;
	case 7:    //KNOCKSPELL

		daang2=(daang + 2048)&2047;
		daz2=(100-plr->horiz)*2000;

		hitscan(plr->x,plr->y,plr->z,plr->sector,                   //Start position
			sintable[(daang2+2560)&2047],           //X vector of 3D ang
			sintable[(daang2+2048)&2047],           //Y vector of 3D ang
			daz2,                                   //Z vector of 3D ang
			&hitsect,&hitwall,&hitsprite,&hitx,&hity,&hitz,CLIPMASK1);

		if( hitsect < 0 && hitsprite < 0 || hitwall >= 0) {

			neartag(hitx,hity,hitz,hitsect,daang,
				&neartagsector,&neartagwall,&neartagsprite,&neartaghitdist,1024,3);

				if(neartagsector >= 0) {
					if(sector[neartagsector].lotag >= 60 &&
						sector[neartagsector].lotag <= 69) {
					sector[neartagsector].lotag=6;
					sector[neartagsector].hitag=0;
					}
					if(sector[neartagsector].lotag >= 70 &&
						sector[neartagsector].lotag <= 79) {
					sector[neartagsector].lotag=7;
					sector[neartagsector].hitag=0;
					}
					operatesector(neartagsector);
				}

		 }
	  break;
	  case 10: //throw a pike axe
			if(currweaponframe == PIKEATTACK1+4) {
				if(currweaponanim == 8 && currweapontics == 10) {

	if (netgame) {
		netshootgun(-1,15);
	}

	//j=insertsprite(plr->sector,JAVLIN);
	j=insertsprite(plr->sector,MISSILE);

	sprite[j].x=plr->x;
	sprite[j].y=plr->y;
	sprite[j].z=plr->z+(16<<8);

	//sprite[j].cstat=17;
	sprite[j].cstat=21;

		sprite[j].picnum=THROWPIKE;
		sprite[j].ang=((plr->ang+2048+96)-512)&2047;
		sprite[j].xrepeat=24;
		sprite[j].yrepeat=24;
		sprite[j].clipdist=24;

	sprite[j].extra=plr->ang;
	sprite[j].shade=-15;
	sprite[j].xvel=((krand()&256)-128);
	sprite[j].yvel=((krand()&256)-128);
	//sprite[j].zvel=((krand()&256)-128);
	sprite[j].zvel=((100-plr->horiz)<<4);
	sprite[j].owner=4096;
	sprite[j].lotag=1024;
	sprite[j].hitag=0;
	sprite[j].pal=0;


				}
			}
			if(currweaponframe == PIKEATTACK2+4) {
				if(currweaponanim == 4 && currweapontics == 10) {

	//j=insertsprite(plr->sector,JAVLIN);
	j=insertsprite(plr->sector,MISSILE);

	sprite[j].x=plr->x;
	sprite[j].y=plr->y;
	sprite[j].z=plr->z;

	//sprite[j].cstat=17;
	sprite[j].cstat=21;

		sprite[j].picnum=THROWPIKE;
		sprite[j].ang=((plr->ang+2048)-512)&2047;
		sprite[j].xrepeat=24;
		sprite[j].yrepeat=24;
		sprite[j].clipdist=24;

	sprite[j].extra=plr->ang;
	sprite[j].shade=-15;
	sprite[j].xvel=((krand()&256)-128);
	sprite[j].yvel=((krand()&256)-128);
	sprite[j].zvel=((krand()&256)-128);
	sprite[j].owner=4096;
	sprite[j].lotag=1024;
	sprite[j].hitag=0;
	sprite[j].pal=0;

				}
			}
	  break;
	  case 2: // parabolic trajectory

				if (netgame) {
					netshootgun(-1,12);
				}

		 j=insertsprite(plr->sector,MISSILE);
		 sprite[j].x=plr->x;
		 sprite[j].y=plr->y;
		 sprite[j].z=plr->z+(8<<8)+((krand()&10)<<8);
		 sprite[j].cstat=0;
		 sprite[j].picnum=PLASMA;
		 sprite[j].shade=-32;
		 sprite[j].pal=0;
		 sprite[j].xrepeat=16;
		 sprite[j].yrepeat=16;
		 sprite[j].ang=daang;
		 sprite[j].xvel=(sintable[(daang+2560)&2047]>>5);
		 sprite[j].yvel=(sintable[(daang+2048)&2047]>>5);
		 sprite[j].zvel=((100-plr->horiz)<<4);
		 sprite[j].owner=4096;
		 sprite[j].lotag=256;
		 sprite[j].hitag=0;
		 sprite[j].clipdist=48;

		 //movesprite
		 //setsprite
		movesprite((short)j,(((int)sintable[(sprite[j].ang+512)&2047])*synctics)<<3,(((int)sintable[sprite[j].ang])*synctics)<<3,0L,4L<<8,4L<<8,0);
		setsprite(j,sprite[j].x,sprite[j].y,sprite[j].z);

	  break;
	  case 3:

				if (netgame) {
					netshootgun(-1,13);
				}

		 j=insertsprite(plr->sector,MISSILE);
		 sprite[j].x=plr->x;
		 sprite[j].y=plr->y;
		 sprite[j].z=plr->z+(8<<8);
		 sprite[j].cstat=0;        //Hitscan does not hit other bullets
		 sprite[j].picnum=MONSTERBALL;
		 sprite[j].shade=-32;
		 sprite[j].pal=0;
		 sprite[j].xrepeat=64;
		 sprite[j].yrepeat=64;
		 sprite[j].ang=plr->ang;
		 sprite[j].xvel=(sintable[(daang+2560)&2047]>>7);
		 sprite[j].yvel=(sintable[(daang+2048)&2047]>>7);
		 sprite[j].zvel=((100-plr->horiz)<<4);
		 sprite[j].owner=4096;
		 //sprite[j].lotag=1024;
		 sprite[j].lotag=256;
		 sprite[j].hitag=0;
		 sprite[j].clipdist=64;

		//dax=(sintable[(sprite[j].ang+512)&2047]>>6);
		//day=(sintable[sprite[j].ang]>>6);

		movesprite((short)j,(((int)sintable[(sprite[j].ang+512)&2047])*synctics)<<3,(((int)sintable[sprite[j].ang])*synctics)<<3,0L,4L<<8,4L<<8,0);
		setsprite(j,sprite[j].x,sprite[j].y,sprite[j].z);


	  break;
	  case 4:

				if (netgame) {
					netshootgun(-1,14);
				}


			for(j=0;j<MAXSPRITES;j++) {
				switch(sprite[j].picnum) {
				case SPIDER:
				case KOBOLD:
				case KOBOLDATTACK:
				case DEVIL:
				case DEVILATTACK:
				case GOBLIN:
				case GOBLINATTACK:
				case GOBLINCHILL:
				case MINOTAUR:
				case MINOTAURATTACK:
				case SKELETON:
				case SKELETONATTACK:
				case GRONHAL:
				case GRONHALATTACK:
				case GRONMU:
				case GRONMUATTACK:
				case GRONSW:
				case GRONSWATTACK:
				case DRAGON:
				case DRAGONATTACK:
				case DRAGONATTACK2:
				case GUARDIAN:
				case GUARDIANATTACK:
				case FATWITCH:
				case FATWITCHATTACK:
				case SKULLY:
				case SKULLYATTACK:
				case JUDY:
				case JUDYATTACK1:
				case JUDYATTACK2:
				case WILLOW:
					if( cansee(plr->x,plr->y,plr->z,plr->sector,sprite[j].x,sprite[j].y,sprite[j].z-(tilesizy[sprite[j].picnum]<<7),sprite[j].sectnum) == 1 )
						if(checkmedusadist(j,plr->x,plr->y,plr->z,12) )
							nukespell(j);
				break;
				}

			}

/*
		 j=insertsprite(plr->sector,HEATSEEKER);
		 sprite[j].x=plr->x;
		 sprite[j].y=plr->y;
		 sprite[j].z=plr->z+(8<<8);
		 sprite[j].cstat=0;        //Hitscan does not hit other bullets
		 sprite[j].picnum=PLASMA;
		 sprite[j].shade=-32;
		 sprite[j].pal=0;
		 sprite[j].xrepeat=16;
		 sprite[j].yrepeat=16;
		 sprite[j].ang=plr->ang;
		 sprite[j].xvel=(sintable[(daang+2560)&2047]>>7);
		 sprite[j].yvel=(sintable[(daang+2048)&2047]>>7);
		 sprite[j].zvel=((100-plr->horiz)<<4);
		 sprite[j].owner=4096;
		 sprite[j].lotag=1024;
		 sprite[j].hitag=0;
		 sprite[j].clipdist=128;

		dax=(sintable[(sprite[j].ang+512)&2047]>>6);
		day=(sintable[sprite[j].ang]>>6);

		movesprite((short)j,(((int)sintable[(sprite[j].ang+512)&2047])*synctics)<<3,(((int)sintable[sprite[j].ang])*synctics)<<3,0L,4L<<8,4L<<8,0);
		setsprite(j,sprite[j].x,sprite[j].y,sprite[j].z);

*/

		break;
   }


}




void singleshot(short bstatus) {
	(void)bstatus;

   hasshot = 0;
   return;
   if( selectedgun != 1 && oldmousestatus == 0) {
	  keystatus[keys[KEYFIRE]]=keystatus[0x9d]=0;
	  hasshot = 0;
   }

}

void potionpic(int currentpotion) {

	struct player *plr;
	int i;
	int tilenum;

	plr=&player[pyrn];

	if( netgame )
		return;

	rotatesprite(260<<16,161<<16,65536,0,POTIONBACKPIC,0,0,2+8+16+64+128,0,0,xdim-1,ydim-1);
	rotatesprite((260-2)<<16,(161-4)<<16,65536,0,POTIONARROW+currentpotion,0,0,2+8+16+64+128,0,0,xdim-1,ydim-1);

	for(i=0;i<MAXPOTIONS;i++) {
		if(plr->potion[i] < 0)
			plr->potion[i]=0;
		if(plr->potion[i] > 0) {
			switch(i) {
			case 0:
				tilenum=FLASKBLUE;
			break;
			case 1:
				tilenum=FLASKGREEN;
			break;
			case 2:
				tilenum=FLASKOCHRE;
			break;
			case 3:
				tilenum=FLASKRED;
			break;
			case 4:
				tilenum=FLASKTAN;
			break;
			}

			potiontilenum=tilenum;

			rotatesprite((262+i*10)<<16,169<<16,65536,0,potiontilenum,0,0,2+8+16+128,0,0,xdim-1,ydim-1);

			sprintf(potionbuf,"%d",plr->potion[i]);

			fancyfontperm(266+(i*10),164,SMFONT-26,potionbuf,0);

		}
		else {
			rotatesprite((262+i*10)<<16,(161+8)<<16,65536,0,FLASKBLACK,0,0,2+8+16+128,0,0,xdim-1,ydim-1);
		}
	}


}


void usapotion(struct player *plr) {

	if( currentpotion == 0  && plr->health >= plr->maxhealth )
		return;

	if( currentpotion == 2  && poisoned == 0 )
		return;

	if( plr->potion[currentpotion] <= 0)
		return;
	else
		plr->potion[currentpotion]--;

	switch(currentpotion) {
	case 0: // health potion
		if( plr->health+25 > plr->maxhealth) {
			plr->health=0;
			SND_PlaySound(S_DRINK,0,0,0,0);
			healthpic(plr->maxhealth);
		}
		else {
			SND_PlaySound(S_DRINK,0,0,0,0);
			healthpic(25);
		}
		startblueflash(10);
	break;
	case 1: // strength
	   strongtime=3200;
	   SND_PlaySound(S_DRINK,0,0,0,0);
	   startredflash(10);
	break;
	case 2: // anti venom
	   SND_PlaySound(S_DRINK,0,0,0,0);
	   poisoned=0;
	   poisontime=0;
	   startwhiteflash(10);
	   strcpy(displaybuf,"poison cured");
	   healthpic(0);
	   displaytime=360;
	break;
	case 3: // fire resist
	   SND_PlaySound(S_DRINK,0,0,0,0);
	   manatime=3200;
	   startwhiteflash(10);
			if( lavasnd != -1 ) {
				SND_StopLoop(lavasnd);
				lavasnd=-1;
			}
	break;
	case 4: // invisi
	   SND_PlaySound(S_DRINK,0,0,0,0);
	   invisibletime=3200;
	   startgreenflash(10);
	break;
	}

	potionpic(currentpotion);

}

void orbpic(int currentorb) {

	char tempbuf[8];
	int  spellbookpage;
	struct player *plr;

	plr=&player[pyrn];

	if( plr->orbammo[currentorb] < 0)
		plr->orbammo[currentorb]=0;

	sprintf(tempbuf,"%d",plr->orbammo[currentorb]);

	spellbookpage=spellbookanim[currentorb][8].daweaponframe;

	rotatesprite(121<<16,154<<16,65536,0,SPELLBOOKBACK,0,0,2+8+16+64+128,0,0,xdim-1,ydim-1);
	rotatesprite(121<<16,161<<16,65536,0,spellbookpage,0,0,2+8+16+128,0,0,xdim-1,ydim-1);
	fancyfontperm(126,181,SCOREFONT-26,tempbuf,0);

}

void healthpic(int hp) {

	struct player *plr;

	plr=&player[pyrn];
	plr->health+=hp;

	if(plr->health < 0)
		plr->health=0;

	sprintf(healthbuf,"%d",plr->health);

	if( plr->screensize <= 320 ) {
		if( poisoned == 1 ) {
			rotatesprite(72<<16,168<<16,65536,0,HEALTHBACKPIC,0,6,2+8+16+64+128,0,0,xdim-1,ydim-1);
			fancyfontperm(74,170,BGZERO-26,healthbuf,6);
		}
		else {
			rotatesprite(72<<16,168<<16,65536,0,HEALTHBACKPIC,0,0,2+8+16+64+128,0,0,xdim-1,ydim-1);
			fancyfontperm(74,170,BGZERO-26,healthbuf,0);
		}
	}

}


void armorpic(int arm) {

	struct player *plr;

	plr=&player[pyrn];
	plr->armor+=arm;

	if(plr->armor < 0) {
		plr->armor=0;
		plr->armortype=0;
	}

	sprintf(armorbuf,"%d",plr->armor);

	if( plr->screensize <= 320 ) {
		rotatesprite((196+1)<<16,168<<16,65536,0,HEALTHBACKPIC,0,0,2+8+16+64+128,0,0,xdim-1,ydim-1);
		fancyfontperm(200+1,170,BGZERO-26,armorbuf,0);
	}

}

void levelpic(void) {

	struct player *plr;
	int tilenum;
	char temp[20];
	char temp2[20];

	plr=&player[pyrn];

	if( selectedgun == 6) {
		if( plr->ammo[6] < 0)
			plr->ammo[6]=0;
			sprintf(temp,"%d",plr->ammo[6]);
			strcpy(temp2,temp);
			rotatesprite(3<<16,181<<16,65536,0,ARROWS,0,0,2+8+16+128,0,0,xdim-1,ydim-1);
			fancyfontperm(42,183,SCOREFONT-26,temp2,0);
	}
	else if( selectedgun == 7 && plr->weapon[7] == 2) {
		if( plr->ammo[7] < 0)
			plr->ammo[7]=0;
			sprintf(temp,"%d",plr->ammo[7]);
			strcpy(temp2,temp);
			// need pike pic
			rotatesprite(3<<16,181<<16,65536,0,PIKES,0,0,2+8+16+128,0,0,xdim-1,ydim-1);
			fancyfontperm(42,183,SCOREFONT-26,temp2,0);
	}
	else {
		tilenum=PLAYERLVL+(plr->lvl-1);
		rotatesprite(3<<16,181<<16,65536,0,tilenum,0,0,2+8+16+128,0,0,xdim-1,ydim-1);
	}

}


void score(int score) {

	char tempbuf[8];
	struct player *plr;

	plr=&player[pyrn];

	plr->score+=score;

	sprintf(tempbuf,"%d",plr->score);

	rotatesprite(29<<16,163<<16,65536,0,SCOREBACKPIC,0,0,2+8+16+64+128,0,0,xdim-1,ydim-1);

	strcpy(scorebuf,tempbuf);

	if( plr->screensize <= 320)
		fancyfontperm(30,165,SCOREFONT-26,scorebuf,0);

	goesupalevel(plr);

}

void goesupalevel(struct player *plr) {


	if(plr->score > 2250 && plr->score < 4499 && plr->lvl < 2) {
		strcpy(displaybuf,"thou art 2nd level");
		displaytime=360;
		plr->lvl=2;
		plr->maxhealth=120;
		levelpic();
	}
	else if( plr->score > 4500 && plr->score < 8999 && plr->lvl < 3) {
		strcpy(displaybuf,"thou art 3rd level");
		displaytime=360;
		plr->lvl=3;
		plr->maxhealth=140;
		levelpic();
	}
	else if( plr->score > 9000 && plr->score < 17999 && plr->lvl < 4) {
		strcpy(displaybuf,"thou art 4th level");
		displaytime=360;
		plr->lvl=4;
		plr->maxhealth=160;
		levelpic();
	}
	else if( plr->score > 18000 && plr->score < 35999 && plr->lvl < 5) {
		strcpy(displaybuf,"thou art 5th level");
		displaytime=360;
		plr->lvl=5;
		plr->maxhealth=180;
		levelpic();
	}
	else if( plr->score > 36000 && plr->score < 74999 && plr->lvl < 6) {
		strcpy(displaybuf,"thou art 6th level");
		displaytime=360;
		plr->lvl=6;
		plr->maxhealth=200;
		levelpic();
	}
	else if( plr->score > 75000 && plr->score < 179999 && plr->lvl < 7) {
		strcpy(displaybuf,"thou art 7th level");
		displaytime=360;
		plr->lvl=7;
		levelpic();
	}
	else if( plr->score > 180000 && plr->score < 279999 && plr->lvl < 8) {
		strcpy(displaybuf,"thou art 8th level");
		displaytime=360;
		plr->lvl=8;
		levelpic();
	}
	else if( plr->score > 280000 && plr->score < 379999 && plr->lvl < 9) {
		strcpy(displaybuf,"thou art hero");
		displaytime=360;
		plr->lvl=9;
		levelpic();
	}


}


int checkweapondist(short i,int x,int y,int z,char guntype) {

   int length;

   (void)guntype;

   if(selectedgun == 0) {
	  length=1024;
   }
   else {
	  switch(selectedgun) {
	  case 1:
		length=1024;
	  break;
	  case 2:
	  case 3:
	  case 4:
	  case 5:
		length=1536;
	  break;
	  case 6:
		length=2048;
	  break;
	  case 7:
		length=1024;
	  break;
	  case 8:
	  case 9:
		length=2048;
	  break;
	  }
   }
   if( (labs(x-sprite[i].x)+labs(y-sprite[i].y) < length) && (labs((z>>8)-((sprite[i].z>>8)-(tilesizy[sprite[i].picnum]>>1))) <= (length>>3)) )
	  return(1);
   else
	  return(0);

}


void updatepics(void) {

	score(0);  //OK
	if(netgame != 0) {
		if(gametype >= 1)
			captureflagpic();
		else
			fragspic();
	}
	else {
		potionpic(currentpotion); //OK
	}
	levelpic(); //ok
	healthpic(0); //OK
	armorpic(0); //OK
	orbpic(currentorb);
	keyspic(); //OK

}

extern short teamscore[];
extern short teaminplay[];

struct capt {
int x;
int y;
int palnum;
};

void captureflagpic(void) {

	int i;

	struct capt flag[4] = {{ 260, 161, 0},
							{ 286, 161, 10},
							{ 260, 176, 11},
							{ 286, 176, 12}};
	/*
	struct capt sflag[4] = { { 260, 387, 0},
							{ 286, 387, 10},
							{ 260, 417, 11},
							{ 286, 417, 12}};
	*/
	rotatesprite(260<<16,161<<16,65536,0,POTIONBACKPIC,0,0,2+8+16+64+128,0,0,xdim-1,ydim-1);

	for(i=0;i<4;i++) {
		if( teaminplay[i] ) {
			rotatesprite((flag[i].x+3)<<16,(flag[i].y+3)<<16,65536,0,THEFLAG,0,flag[i].palnum,2+8+16+128,0,0,xdim-1,ydim-1);
			sprintf(tempbuf,"%d",teamscore[i]);
			fancyfontperm(flag[i].x+6,flag[i].y+6,SMFONT-26,tempbuf,0);
		}
	}
}

void fragspic(void) {

	struct player *plr;

	plr=&player[pyrn];

	if( plr->screensize <= 320) {
		rotatesprite(260<<16,161<<16,65536,0,POTIONBACKPIC,0,0,2+8+16+64+128,0,0,xdim-1,ydim-1);
		sprintf(tempbuf,"%d",teamscore[pyrn]);
		//overwritesprite(72,168,HEALTHBACKPIC,0,0,0);
		fancyfontperm(260+15,161+5,BGZERO-26,tempbuf,0);
	}

}


void keyspic(void) {

	struct player *plr;

	plr=&player[pyrn];

	if( plr->treasure[14] == 1 )
		rotatesprite(242<<16,160<<16,65536,0,KEYBRASS,0,0,2+8+16+128,0,0,xdim-1,ydim-1);
	else
		rotatesprite(242<<16,160<<16,65536,0,KEYBLANK,0,0,2+8+16+128,0,0,xdim-1,ydim-1);

	if( plr->treasure[15] == 1 )
		rotatesprite(242<<16,169<<16,65536,0,KEYBLACK,0,0,2+8+16+128,0,0,xdim-1,ydim-1);
	else
		rotatesprite(242<<16,169<<16,65536,0,KEYBLANK,0,0,2+8+16+128,0,0,xdim-1,ydim-1);

	if( plr->treasure[16] == 1 )
		rotatesprite(242<<16,178<<16,65536,0,KEYGLASS,0,0,2+8+16+128,0,0,xdim-1,ydim-1);
	else
		rotatesprite(242<<16,178<<16,65536,0,KEYBLANK,0,0,2+8+16+128,0,0,xdim-1,ydim-1);

	if( plr->treasure[17] == 1 )
		rotatesprite(242<<16,187<<16,65536,0,KEYIVORY,0,0,2+8+16+128,0,0,xdim-1,ydim-1);
	else
		rotatesprite(242<<16,187<<16,65536,0,KEYBLANK,0,0,2+8+16+128,0,0,xdim-1,ydim-1);

}


int adjustscore(int score) {

	float factor;

	factor=(krand()%20)/100;

	if( krand()%100 > 50)
		return(score*(factor+1));
	else
		return(score-(score*(factor)));

}

int lvlspellcheck(struct player *plr) {

	int legal=0;

	switch(currentorb) {
	case 0:
		legal=1;
	break;
	case 1:
		legal=1;
	break;
	case 2:
		if(plr->lvl > 1) {
			legal=1;
		}
		else {
			strcpy(displaybuf,"must attain 2nd level");
			displaytime=360;
		}
	break;
	case 3:
		if(plr->lvl > 1) {
			legal=1;
		}
		else {
			strcpy(displaybuf,"must attain 2nd level");
			displaytime=360;
		}
	break;
	case 4:
		if(plr->lvl > 2) {
			legal=1;
		}
		else {
			strcpy(displaybuf,"must attain 3rd level");
			displaytime=360;
		}
	break;
	case 5:
		if(plr->lvl > 2) {
			legal=1;
		}
		else {
			strcpy(displaybuf,"must attain 3rd level");
			displaytime=360;
		}
	break;
	case 6:
		if(plr->lvl > 3) {
			legal=1;
		}
		else {
			strcpy(displaybuf,"must attain 4th level");
			displaytime=360;
		}
	break;
	case 7:
		if(plr->lvl > 4) {
			legal=1;
		}
		else {
			strcpy(displaybuf,"must attain 5th level");
			displaytime=360;
		}
	break;
	}

return(legal);
}

void gronmissile(int s) {

	struct player *plr;
	short k, j, daang;
	int discrim;

	plr=&player[pyrn];

	daang=(sprite[s].ang-36)&2047;

	for(k=0;k<10;k++) {

		daang=(daang+(k<<1))&2047;

		j=insertsprite(sprite[s].sectnum,MISSILE);
		sprite[j].x=sprite[s].x;
		sprite[j].y=sprite[s].y;
		sprite[j].z=sprite[s].z+(8<<8)+((krand()&10)<<8);
		sprite[j].cstat=0;
		sprite[j].picnum=PLASMA;
		sprite[j].shade=-32;
		sprite[j].pal=0;
		sprite[j].xrepeat=16;
		sprite[j].yrepeat=16;
		sprite[j].ang=daang;
		sprite[j].xvel=(sintable[(daang+2560)&2047]>>5);
		sprite[j].yvel=(sintable[(daang+2048)&2047]>>5);
		discrim=ksqrt((plr->x-sprite[s].x)*(plr->x-sprite[s].x)
					 +(plr->y-sprite[s].y)*(plr->y-sprite[s].y));
		if(discrim == 0)
			discrim=1;
		sprite[j].zvel=(((plr->z+(8<<8))-sprite[s].z)<<7)/discrim;
		sprite[j].owner=s;
		sprite[j].lotag=256;
		sprite[j].hitag=0;
		sprite[j].clipdist=48;

	}

}


void displayspelltext(void) {
	 switch(currentorb) {
	 case 0:
		strcpy(displaybuf,"scare spell");
		displaytime=360;
	 break;
	 case 1:
		strcpy(displaybuf,"night vision spell");
		displaytime=360;
	 break;
	 case 2:
		strcpy(displaybuf,"freeze spell");
		displaytime=360;
	 break;
	 case 3:
		strcpy(displaybuf,"magic arrow spell");
		displaytime=360;
	 break;
	 case 4:
		strcpy(displaybuf,"open door spell");
		displaytime=360;
	 break;
	 case 5:
		strcpy(displaybuf,"fly spell");
		displaytime=360;
	 break;
	 case 6:
		strcpy(displaybuf,"fireball spell");
		displaytime=360;
	 break;
	 case 7:
		strcpy(displaybuf,"nuke spell");
		displaytime=360;
	 break;
	 }

}


void
painsound(int xplc,int yplc)
{
	playsound_loc(S_BREATH1+(rand()%6),xplc,yplc);
}
