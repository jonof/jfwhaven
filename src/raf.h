/**************************************************************************

      Witchaven

      RAF.H

      Rafael Paiz

**************************************************************************/

#define EASY        0
#define NORMAL      1
#define BITCHAVEN   2

#define LOW       1
#define HIGH      2

#define TOP       1
#define BOTTOM    2

#define   INACTIVE       0
#define   PATROL         1
#define   CHASE          2
#define   AMBUSH         3

#define   BIRTH          4
#define   DODGE          5
#define   ATTACK         6
#define   DEATH          7
#define   STAND          8

#define MISSILE    100
#define FX         101
#define HEATSEEKER 102
#define YELL       103
#define CAST       104
#define PUSH       105
#define FALL       106
#define DIE        107
#define DEAD       108
#define FACE       109
#define SHOVE      110
#define SHATTER    111
#define FIRE       112
#define LIFTUP     113
#define LIFTDN     114
#define PENDULUM   115
#define RESURECT   116
#define BOB        117
#define SHOVER     118
#define TORCHER    119
#define MASPLASH   120
#define CHUNKOMEAT 121
#define FLEE       122
#define DORMANT    123
#define ACTIVE     124
#define ATTACK2    125
#define WITCHSIT   126
#define CHILL      127
#define SKIRMISH   128
#define FLOCK      129
#define FLOCKSPAWN 130
#define PAIN       131
#define WAR        132
#define TORCHLIGHT 133
#define GLOWLIGHT  134
#define BLOOD      135
#define DRIP       136
#define DEVILFIRE  137
#define FROZEN     138
#define PULLTHECHAIN  139
#define FLOCKCHIRP    140
#define CHUNKOWALL    141
#define FINDME        142
#define DRAIN         143
#define RATRACE       144
#define SMOKE         145
#define EXPLO         146
#define JAVLIN        147
#define ANIMLEVERUP   148
#define ANIMLEVERDN   149
#define BROKENVASE    150
#define CHARCOAL      151
#define WARPFX        152

#define MAXRAINDROPS 100

#define WITCHAVEN

extern short ironbarscnt;
extern short ironbarsdone[16];
extern int  ironbarsgoal1[16];
extern int  ironbarsgoal2[16];
extern short ironbarsector[16];
extern short ironbarsanim[16];
extern int  ironbarsgoal[16];

#define MAXKEYS   4

#define MAXTREASURES 18

#define GRATESPEED 16L

#define PLAYERHEIGHT 48
#define MAXNUMORBS   8
#define MAXHEALTH    100

//
// KEYDEFINES
//
#define FN1    0x3b
#define FN2    0x3c
#define FN3    0x3d
#define FN4    0x3e
#define FN5    0x3f
#define FN6    0x40
#define FN7    0x41
#define FN8    0x42
#define FN9    0x43
#define FN10   0x44
#define FN11   0x57
#define FN12   0x58

#define WEAPONONE    0x02
#define WEAPONTWO    0x03
#define WEAPONTHREE  0x04
#define WEAPONFOUR   0x05
#define WEAPONFIVE   0x06
#define WEAPONSIX    0x07
#define WEAPONSEVEN  0x08
#define WEAPONEIGHT  0x09
#define WEAPONNINE   0x0a
#define WEAPONTEN    0x0b

#define LMINUS 0x0c
#define LPLUS  0x0d
#define BKSPC  0x0e

#define LUP    0xc8
#define LDN    0xd0
#define LRIGHT 0xcd
#define LLEFT  0xcb
#define LSHIFT 0x2a
#define RUP    0x48
#define RDN    0x50
#define RRIGHT 0x4d
#define RLEFT  0x4b
#define RSHIFT 0x36

#define LALT   0x38
#define RALT   0xb8

#define KILLSECTOR 4444


typedef struct
{
    int x, y, z;
} point3d;

//point3d osprite[MAXSPRITESONSCREEN];

