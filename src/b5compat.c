#include "build.h"

void permanentwritesprite(int thex, int they, short tilenum, signed char shade,
        int cx1, int cy1, int cx2, int cy2, unsigned char dapalnum) {
    rotatesprite(thex<<16,they<<16,65536L,0,tilenum,shade,
                 dapalnum,8+16,cx1,cy1,cx2,cy2);
}

void overwritesprite(int thex, int they, short tilenum, signed char shade,
        char stat, unsigned char dapalnum) {
    rotatesprite(thex<<16,they<<16,65536L,(stat&8)<<7,tilenum,shade,dapalnum,
                 (((stat&1)^1)<<4)+(stat&2)+((stat&4)>>2)+(((stat&16)>>2)^((stat&8)>>1)),
                 windowx1,windowy1,windowx2,windowy2);
}

void precache()
{
}
