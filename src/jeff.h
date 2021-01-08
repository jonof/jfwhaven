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


#define CLKIPS          140L

#define PEL_WRITE_ADR   0x3c8
#define PEL_READ_ADR    0x3c7
#define PEL_DATA        0x3c9

//
// VGA manipulation
//
#define SC_INDEX            0x3C4
#define SC_RESET            0
#define SC_CLOCK            1
#define SC_MAPMASK          2
#define SC_CHARMAP          3
#define SC_MEMMODE          4

#define CRTC_INDEX          0x3D4
#define CRTC_H_TOTAL        0
#define CRTC_H_DISPEND      1
#define CRTC_H_BLANK        2
#define CRTC_H_ENDBLANK     3
#define CRTC_H_RETRACE      4
#define CRTC_H_ENDRETRACE   5
#define CRTC_V_TOTAL        6
#define CRTC_OVERFLOW       7
#define CRTC_ROWSCAN        8
#define CRTC_MAXSCANLINE    9
#define CRTC_CURSORSTART    10
#define CRTC_CURSOREND      11
#define CRTC_STARTHIGH      12
#define CRTC_STARTLOW       13
#define CRTC_CURSORHIGH     14
#define CRTC_CURSORLOW      15
#define CRTC_V_RETRACE      16
#define CRTC_V_ENDRETRACE   17
#define CRTC_V_DISPEND      18
#define CRTC_OFFSET         19
#define CRTC_UNDERLINE      20
#define CRTC_V_BLANK        21
#define CRTC_V_ENDBLANK     22
#define CRTC_MODE           23
#define CRTC_LINECOMPARE    24


#define GC_INDEX            0x3CE
#define GC_SETRESET         0
#define GC_ENABLESETRESET   1
#define GC_COLORCOMPARE     2
#define GC_DATAROTATE       3
#define GC_READMAP          4
#define GC_MODE             5
#define GC_MISCELLANEOUS    6
#define GC_COLORDONTCARE    7
#define GC_BITMASK          8

#define ATR_INDEX               0x3c0
#define ATR_MODE                16
#define ATR_OVERSCAN            17
#define ATR_COLORPLANEENABLE    18
#define ATR_PELPAN              19
#define ATR_COLORSELECT         20

#define  STATUS_REGISTER_1    0x3da

#define PEL_WRITE_ADR       0x3c8
#define PEL_READ_ADR        0x3c7
#define PEL_DATA            0x3c9

