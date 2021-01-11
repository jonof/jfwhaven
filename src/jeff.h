/***************************************************************************
 *  JEFF.H     - new header stuff from Jeff                                *
 *                                                                         *
 *                                                     12/15/94 Jeff S.    *
 ***************************************************************************/

//
//   flic header file stuff
//
extern    char      flicnum;
extern    char      *flicnames[];
extern    char      dofadein;

extern    char      paletteloaded;   // engine.obj

extern    int       flictype;

#define  CT_COLOR_256    4   /* 256 level color pallette info. (FLC only.) */
#define  CT_DELTA_FLC    7   /* Word-oriented delta compression. (FLC only.) */
#define  CT_COLOR_64    11 /* 64 level color pallette info. */
#define  CT_DELTA_FLI   12 /* Byte-oriented delta compression. */
#define  CT_BLK         13 /* whole frame is color 0 */
#define  CT_BYTE_RUN    15 /* Byte run-length compression. */
#define  CT_LITERAL     16 /* Uncompressed pixels. */
#define  CT_PSTAMP      18 /* "Postage stamp" chunk. (FLC only.) */

#define   FT_FULLSCREEN   0
#define   FT_DATALINK     1


#define CLKIPS          120L
