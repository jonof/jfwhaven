//
// WHDEFS.H
//

#define   STATUSHEIGHT   46
#define   STATUSSCREEN   (YDIM-STATUSHEIGHT)

#if 0                                             // Les 07/24/95 see keydefs.h
#define   KEYFWD    0
#define   KEYBACK   1
#define   KEYLEFT   2
#define   KEYRIGHT  3
#define   KEYRUN    4
#define   KEYSTRAFE 5
#define   KEYFIRE   6
#define   KEYUSE    7
#define   KEYSTAND  8
#define   KEYLAY    9
#define   KEYUP     10
#define   KEYDOWN   11
#define   KEYMAP    12
#define   KEYVIEW   13
#define   KEYZOOMIN 14
#define   KEYZOOMOUT 15

#define   KEYLENTER  28

#define   KEYELEVUP 73
#define   KEYELEVDN 81
#endif                                            // Les 07/24/95

#define   ACTIVATESECTOR      1
#define   ACTIVATESECTORONCE  2

#define   DOORUPTAG      6
#define   DOORDOWNTAG    7
#define   DOORSPLITHOR   8
#define   DOORSPLITVER   9
#define   DOORSWINGTAG   13
#define   DOORBOX        16

#define   PLATFORMELEVTAG 1000
#define   BOXELEVTAG     1003

#define   SECTOR         1
#define   WALL           2
#define   SPRITE         3

#define   SECTOREFFECT   104
#define   PULSELIGHT     0
#define   FLICKERLIGHT   1
#define   DELAYEFFECT    2
#define   XPANNING       3

#define   DOORDELAY      480L // 4 second delay for doors to close
#define   DOORSPEED      128L
#define   ELEVSPEED      256L

#define   PICKDISTANCE   512L // for picking up sprites
#define   PICKHEIGHT     40L

#define   JETPACKPIC     93   // sprites available to pick up

#define   MAXSWINGDOORS  32
#define   MAXANIMATES    512

#define   JETPACKITEM    0
#define   SHOTGUNITEM    1

#define   SHOTGUNPIC     101
#define   SHOTGUNVIEW    102

#define   MAXWEAPONS     10
#define   MAXPOTIONS     5


