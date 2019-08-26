#include "build.h"
#include "names.h"
#include "stdio.h"

extern long zmode, kensplayerheight;

#define AVERAGEFRAMES   16

static long frameval[AVERAGEFRAMES], framecnt = 0;


extern
long posx,
	  posy,
	  posz,
	  horiz,
	  qsetmode;

extern
short ang,
	  cursectnum;

extern
short ceilingheinum,
	  floorheinum;

extern
char names[MAXTILES][17];

extern
short searchsector,
	  searchwall,
	  searchstat;              //search output

static
short temppicnum,
	  tempcstat;

static
char tempshade,
	  tempxrepeat,
	  tempyrepeat,
	  somethingintab=255;

extern
void __interrupt __far timerhandler(void);

long xoldtimerhandler;

void __interrupt __far
xtimerhandler(void)
{
	  timerhandler();
	  if (qsetmode == 480) {
			 Keys2d();
	  }
	  else {
			 Keys3d();
	  }
}


void faketimerhandler(void) {
	return;
}

void
xinittimer(void)
{
	  outp(0x43,54);
	  outp(0x40,9942&255);
	  outp(0x40,9942>>8);      //120 times/sec
	  xoldtimerhandler=(long)_dos_getvect(0x8);
	  _disable();
	  _dos_setvect(0x8,xtimerhandler);
	  _enable();
}

static
char tempbuf[256];            //161

static
int  numsprite[2000];

static
char lo[32];

static
const char *levelname;

static
short curwall=0,
	  wallpicnum=0,
	  curwallnum=0;

static
short cursprite=0,
	  curspritenum=0;

static
char wallsprite=0;

static helpon=0;

static
char once=0;

void
ExtLoadMap(const char *mapname)
{
	  if (once == 0) {
			 xinittimer();
	  }
	  once=1;
	  levelname=mapname;
	  ReadPaletteTable();
	  visibility=11;
//     pskyoff[0]=0;
//     pskybits=3;
}

void
ExtSaveMap(const char *mapname)
{
	  clearmidstatbar16();             //Clear middle of status bar
	  sprintf(tempbuf,"Saving File : %s",mapname);
	  printmessage16(tempbuf);
}

const
char *ExtGetSectorCaption(short sectnum)
{

	  short which;
	  int temp=0;
	  char string[5];
	  char destination[25];

	  which = sector[sectnum].lotag;

	  if ((sector[sectnum].lotag|sector[sectnum].hitag) == 0) {
			 tempbuf[0]=0;
	  }
	  else {
			 if( which >= 1100 && which <= 1900) {
				 temp = sector[sectnum].lotag / 100;
				 which = temp * 100;
				 itoa(sector[sectnum].lotag-which,string,10);
			 }
			 else if( which >=2000 && which <= 2999 ) {


			 }
			 if( which >= 80 && which <= 89 ) {
				// panning
				temp=which-80;
				which=sector[sectnum].lotag-temp;
			 }
			 //switch((unsigned short)sector[sectnum].lotag) {
			 switch (which) {
			 case 1:
					sprintf(lo,"ACTIVATE SECTOR");
					break;
			 case 2:
					sprintf(lo,"ACTIVATE ONCE");
			 case 4:
			 //
			 // need to fix
					sprintf(lo,"FLOOR PANNING");
			 case 6:
					sprintf(lo,"RISING DOOR");
					break;
			 case 7:
					sprintf(lo,"DROP DOOR");
					break;
			 case 8:
					sprintf(lo,"HOR SPLIT DOOR");
					break;
			 case 9:
					sprintf(lo,"VER SPLIT DOOR");
					break;
			 case 10:
					sprintf(lo,"TELEPORTER");
					break;
			 case 11:
					sprintf(lo,"XPANNING WALLS");
			 case 16:
					sprintf(lo,"BOX DOOR");
					break;
			 case 80:
					strcpy(destination,"PANNING ");

					switch(temp) {
						case 0:
							strcpy(string,"N");
						break;
						case 1:
							strcpy(string,"NE");
						break;
						case 2:
							strcpy(string,"E");
						break;
						case 3:
							strcpy(string,"SE");
						break;
						case 4:
							strcpy(string,"S");
						break;
						case 5:
							strcpy(string,"SW");
						break;
						case 6:
							strcpy(string,"W");
						break;
						case 7:
							strcpy(string,"SW");
						break;
					}
					strcat(destination," ");
					strcat(destination,string);
					sprintf(lo,destination);
					break;
			 case 1000:
					sprintf(lo,"PLATFORM ELEVATOR");
					break;
			 case 1003:
					sprintf(lo,"BOX ELEVATOR");
					break;
			 case 1100:
					strcpy(destination,"DROP FLOOR ");
					strcat(destination,string);
					sprintf(lo,destination);
					break;
			 case 1200:
					strcpy(destination,"RAISE FLOOR ");
					strcat(destination,string);
					sprintf(lo,destination);
					break;
			 case 1300:
					strcpy(destination,"DROP CEILING ");
					strcat(destination,string);
					sprintf(lo,destination);
					break;
			 case 1400:
					strcpy(destination,"RAISE CEILING ");
					strcat(destination,string);
					sprintf(lo,destination);
					break;
			 case 1500:
					strcpy(destination,"DROP F&C ");
					strcat(destination,string);
					sprintf(lo,destination);
					break;
			 case 1600:
					strcpy(destination,"RAISE F&C ");
					strcat(destination,string);
					sprintf(lo,destination);
					break;
			 case 1800:
					sprintf(lo,"LIFT");
					break;
			 case 1900:
					sprintf(lo,"FLOOR DOOR");
					break;
			 default:
					sprintf(lo,"%hu",(unsigned short)sector[sectnum].lotag);
					break;
			 }
			 sprintf(tempbuf,"%hu,%s",(unsigned short)sector[sectnum].hitag,lo);
	  }
	  return(tempbuf);
}

const
char *ExtGetWallCaption(short wallnum)
{
	  long i=0;

	  if (keystatus[0x57] > 0) {    // f11   Grab pic 0x4e +
			 wallpicnum=wall[curwall].picnum;
			 sprintf(tempbuf,"Grabed Wall Picnum %ld",wallpicnum);
			 printmessage16(tempbuf);
	  }
	  if (keystatus[0x2b] > 0) {    // |
			 if (wallsprite == 1) {
					for (i=curwallnum ; i < MAXWALLS ; i++) {
						  if (wall[i].picnum == wall[curwall].picnum) {
								 posx=(wall[i].x)-(((wall[i].x)-(wall[wall[i].point2].x))/2);
								 posy=(wall[i].y)-(( (wall[i].y)-(wall[wall[i].point2].y))/2);
								 printmessage16("Wall Found");
								 curwallnum++;
								 keystatus[0x2b]=0;
								 return(tempbuf);
						  }
						  curwallnum++;
					}
			 }
			 if (wallsprite == 2) {
					for (i=curspritenum ; i < MAXSPRITES ; i++) {
						  if (sprite[i].picnum == sprite[cursprite].picnum && sprite[i].statnum == 0) {
								 posx=sprite[i].x;
								 posy=sprite[i].y;
								 ang=sprite[i].ang;
								 printmessage16("Sprite Found");
								 curspritenum++;
								 keystatus[0x2b]=0;
								 return(tempbuf);
						  }
						  curspritenum++;
					}
			 }
	  }
	  if ((wall[wallnum].lotag|wall[wallnum].hitag) == 0) {
			 tempbuf[0]=0;
	  }
	  else {
			 sprintf(tempbuf,"%hu,%hu",(unsigned short)wall[wallnum].hitag,
										  (unsigned short)wall[wallnum].lotag);
	  }
	  return(tempbuf);
}

const
char *ExtGetSpriteCaption(short spritenum)
{
	  char destination[20];
	  int which;

	  tempbuf[0]=0;

	  if ((sprite[spritenum].lotag|sprite[spritenum].hitag) == 0) {
			 SpriteName(spritenum,lo);
			 if (lo[0] != 0) {
					sprintf(tempbuf,"%s",lo);
			 }
	  }
	  else if ((unsigned short)sprite[spritenum].picnum == 104) {
			 switch((unsigned short)sprite[spritenum].lotag) {
			 case 1000:
					sprintf(lo,"PULSING");
					break;
			 case 1001:
					sprintf(lo,"FLICKERING");
					break;
			 case 1002:
					sprintf(lo,"TIC DELAY");
					break;
			 default:
					sprintf(lo,"%hu",(unsigned short)sprite[spritenum].lotag);
					break;
			 }
			 sprintf(tempbuf,"%hu,%s",(unsigned short)sprite[spritenum].hitag,lo);
	  }
	  else if(sprite[spritenum].hitag >= 90 && sprite[spritenum].hitag <= 99) {
			 which=sprite[spritenum].lotag;
			 strcpy(destination,"GRATE ");
					switch(which){
						case 1:
							strcat(destination,"OUT");
						break;
						case 2:
							strcat(destination,"IN");
						break;
						case 3:
							strcat(destination,"UP");
						break;
						case 4:
							strcat(destination,"DN");
						break;
					}
	  sprintf(lo,destination);
	  }
	  else {
			 SpriteName(spritenum,lo);
			 sprintf(tempbuf,"%hu,%hu %s",(unsigned short)sprite[spritenum].hitag,
											  (unsigned short)sprite[spritenum].lotag,lo);
	  }
	  return(tempbuf);
}

//printext16 parameters:
//printext16(long xpos, long ypos, short col, short backcol,
//           char name[82], char fontsize)
//  xpos 0-639   (top left)
//  ypos 0-479   (top left)
//  col 0-15
//  backcol 0-15, -1 is transparent background
//  name
//  fontsize 0=8*8, 1=3*5

//drawline16 parameters:
// drawline16(long x1, long y1, long x2, long y2, char col)
//  x1, x2  0-639
//  y1, y2  0-143  (status bar is 144 high, origin is top-left of STATUS BAR)
//  col     0-15

void
ExtShowSectorData(short sectnum)   //F5
{
	  int  i,c=0;

	  for (i=0 ; i < 2000 ; i++) {
			 numsprite[i]=0;
	  }
	  for (i=0 ; i < MAXSPRITES ; i++) {
			 if (sprite[i].statnum == 0) {
					numsprite[sprite[i].picnum]++;
			 }
	  }
	  clearmidstatbar16();          //Clear middle of status bar
	  sprintf(tempbuf,"Level %s",levelname);
	  printmessage16(tempbuf);
}

void
ExtShowWallData(short wallnum)       //F6
{
	  int  i,nexti,bls,ouls,ss,rss,smss,tss,es,drs,music=0;

	  bls=ouls=ss=rss=smss=tss=es=drs=0;
	  for(i=0 ; i < MAXSPRITES ; i++) {
			 if (sprite[i].picnum == 51 && sprite[i].statnum == 0) {
					if(sprite[i].lotag > drs) {
						  drs=sprite[i].lotag;
					}
			 }
			 if (sprite[i].picnum == 175 && sprite[i].statnum == 0) {
					switch(sprite[i].lotag) {
					case 1:
						  if (sprite[i].hitag > rss) {
								 rss=sprite[i].hitag;
						  }
						  break;
					case 3:
					case 4:
						  if (sprite[i].hitag > bls) {
								 bls=sprite[i].hitag;
						  }
						  break;
					case 7:
						  if (sprite[i].hitag > tss) {
								 tss=sprite[i].hitag;
						  }
						  break;
					case 8:
						  if (sprite[i].hitag > ouls) {
								 ouls=sprite[i].hitag;
						  }
						  break;
					case 13:
						  if (sprite[i].hitag > es) {
								 es=sprite[i].hitag;
						  }
						  break;
					}
			 }
	  }
	  clearmidstatbar16();
	  sprintf(tempbuf,"Level %s",levelname);
	  printmessage16(tempbuf);
	  printext16(1*8,4*8,11,-1,"Sector Effector Status",0);
	  PrintStatus("Breaking Lights       =",bls,2,6,11);
	  PrintStatus("Open Up Lights        =",ouls,2,7,11);
	  PrintStatus("Switches              =",ss,2,8,11);
	  PrintStatus("Rotating Sectors      =",rss,2,9,11);
	  PrintStatus("Subway Moving Sectors =",smss,2,10,11);
	  PrintStatus("Transporting Sectors  =",tss,2,11,11);
	  PrintStatus("Exploding Sectors     =",es,32,6,11);
	  PrintStatus("Delay Reactions       =",drs,32,7,11);
}

void
ExtShowSpriteData(short spritenum)   //F6
{
	  FILE *fp;
	  int  i,t;
	  char x=0,y=4,xmax=0,xx=0,col=0;

	  clearmidstatbar16();
	  if ((fp=fopen("sehelp.hlp","rb")) == NULL) {
			 printext16(1*4,4*8,11,-1,"ERROR: file <SEHELP.HLP> not found.",0);
			 return;
	  }
	  t=65;
	  while (t != EOF && col < 5) {
			 t=getc(fp);
			 while (t != EOF && t != '\n' && x < 250) {
					tempbuf[x]=t;
					t=getc(fp);
					x++;
					if (x > xmax) {
						  xmax=x;
					}
			 }
			 tempbuf[x]=0;
			 printext16(xx*4,(y*6)+2,11,-1,tempbuf,1);
			 x=0;
			 y++;
			 if (y > 18) {
					col++;
					y=6;
					xx+=xmax;
					xmax=0;
			 }
	  }
}

void
ExtEditSectorData(short sectnum)    //F7
{
	  FILE *fp;
	  int  i,t;
	  char x=0,y=4,xmax=0,xx=0,col=0;

	  clearmidstatbar16();
	  if ((fp=fopen("sthelp.hlp","rb")) == NULL) {
			 printext16(1*4,4*8,11,-1,"ERROR: file <STHELP.HLP> not found.",0);
			 return;
	  }
	  t=65;
	  while (t != EOF && col < 5) {
			 t=getc(fp);
			 while (t != EOF && t != '\n' && x < 250) {
					tempbuf[x]=t;
					t=getc(fp);
					x++;
					if (x > xmax) {
						  xmax=x;
					}
			 }
			 tempbuf[x]=0;
			 printext16(xx*4,(y*6)+2,11,-1,tempbuf,1);
			 x=0;
			 y++;
			 if (y > 18) {
					col++;
					y=6;
					xx+=xmax;
					xmax=0;
			 }
	  }
}

void
ExtEditWallData(short wallnum)       //F8
{
	  wallsprite=1;
	  curwall=wallnum;
	  curwallnum=0;
	  sprintf(tempbuf,"Current Wall %ld",curwall);
	  printmessage16(tempbuf);
}

void
ExtEditSpriteData(short spritenum)   //F8
{
	  wallsprite=2;
	  cursprite=spritenum;
	  curspritenum=0;
	  sprintf(tempbuf,"Current Sprite %ld",cursprite);
	  printmessage16(tempbuf);
}

	 //Just thought you might want my getnumber16 code
/*
getnumber16(char namestart[80], short num, long maxnumber)
{
	char buffer[80];
	long j, k, n, danum, oldnum;

	danum = (long)num;
	oldnum = danum;
	while ((keystatus[0x1c] != 2) && (keystatus[0x1] == 0))  //Enter, ESC
	{
		sprintf(&buffer,"%s%ld_ ",namestart,danum);
		printmessage16(buffer);

		for(j=2;j<=11;j++)                //Scan numbers 0-9
			if (keystatus[j] > 0)
			{
				keystatus[j] = 0;
				k = j-1;
				if (k == 10) k = 0;
				n = (danum*10)+k;
				if (n < maxnumber) danum = n;
			}
		if (keystatus[0xe] > 0)    // backspace
		{
			danum /= 10;
			keystatus[0xe] = 0;
		}
		if (keystatus[0x1c] == 1)   //L. enter
		{
			oldnum = danum;
			keystatus[0x1c] = 2;
			asksave = 1;
		}
	}
	keystatus[0x1c] = 0;
	keystatus[0x1] = 0;
	return((short)oldnum);
}
*/

void
PrintStatus(char *string,int num,char x,char y,char color)
{
	  sprintf(tempbuf,"%s %d",string,num);
	  printext16(x*8,y*8,color,-1,tempbuf,0);
}

void
SpriteName(short spritenum, char *lo2)
{
	  sprintf(lo2,names[sprite[spritenum].picnum]);
/*
 switch((unsigned short)sprite[spritenum].picnum)
 {
  case APLAYER : sprintf(lo2,"GRONDOVAL"); break;
  case KOBOLD : sprintf(lo2,"KOBOLD"); break;
  case JUDY : sprintf(lo2,"JUDY"); break;
  case MANDROID : sprintf(lo2,"MANDROID"); break;
  case TROOP1 : sprintf(lo2,"TROOPER"); break;
  case SEENINE : sprintf(lo2,"C-9"); break;
  case BAT : sprintf(lo2,"BAT"); break;
  case REACTOR : sprintf(lo2,"REACTOR"); break;
  case EXPLODINGBARREL : sprintf(lo2,"EBARREL"); break;

  case COKE : sprintf(lo2,"COKE"); break;
  case SIXPAK : sprintf(lo2,"SIXPAK"); break;
  case STEROIDS : sprintf(lo2,"STEROIDS"); break;
  case SHIELD : sprintf(lo2,"SHIELD"); break;
  case AIRTANK : sprintf(lo2,"AIRTANK"); break;
  case JETPACK : sprintf(lo2,"JETPACK"); break;
  case HOLODUKE : sprintf(lo2,"HOLODUKE"); break;

  case FIRSTGUNSPRITE : sprintf(lo2,"PISTOL"); break;
  case GUN2ASPRITE : sprintf(lo2,"ELECTROGUN"); break;
  case RPGSPRITE : sprintf(lo2,"RPG"); break;

  case AMMO : sprintf(lo2,"AMMO"); break;
  case BATTERYAMMO : sprintf(lo2,"BATTERY"); break;
  case HEAVYHBOMB : sprintf(lo2,"H-BOMB"); break;
  case RPGAMMO : sprintf(lo2,"3-RPG"); break;

  default : lo2[0]=0;
 }
*/
}

void
ReadPaletteTable()
{
	  FILE *fp;
	  int  i,j;
	  char num_tables,lookup_num;

	  if ((fp=fopen("lookup.dat","rb")) == NULL) {
			 return;
	  }
	  num_tables=getc(fp);
	  for (j=0 ; j < num_tables ; j++) {
			 lookup_num=getc(fp);
			 for (i=0 ; i < 256 ; i++) {
					tempbuf[i]=getc(fp);
			 }
			 makepalookup(lookup_num,tempbuf,0,0,0);
	  }
	  fclose;
}

void
Keys3d(void)
{
	  if (helpon == 1) {
			 printext256(0*8,0*8,15,0,"HELP SCREEN",0);
			 printext256(0*8,2*8,15,0," ' + H = Help",0);
			 printext256(0*8,3*8,15,0," ' + Enter = copy graphic Only",0);
	  }
	  if (keystatus[0x28] == 1 && keystatus[0x23] == 1) { // ' H
			 helpon=1;
	  }
	  if (keystatus[0x29] == 1) { // `
			 helpon=0;
	  }
	  if (keystatus[0x28] == 1 && keystatus[0x1c] == 1) { // ' ENTER
			 printext256(0,0,15,0,"Put Graphic ONLY",0);
			 keystatus[0x1c]=0;
			 switch(searchstat) {
			 case 0:
					wall[searchwall].picnum=temppicnum;
					break;
			 case 1:
					sector[searchsector].ceilingpicnum=temppicnum;
					break;
			 case 2:
					sector[searchsector].floorpicnum=temppicnum;
					break;
			 case 3:
					sprite[searchwall].picnum=temppicnum;
					break;
			 case 4:
					wall[searchwall].overpicnum=temppicnum;
					break;
			 }
	  }
	  if (keystatus[0x0f] == 1) { //TAB
			 switch (searchstat) {
			 case 0:
					temppicnum=wall[searchwall].picnum;
					tempshade=wall[searchwall].shade;
					tempxrepeat=wall[searchwall].xrepeat;
					tempyrepeat=wall[searchwall].yrepeat;
					tempcstat=wall[searchwall].cstat;
					break;
			 case 1:
					temppicnum=sector[searchsector].ceilingpicnum;
					tempshade=sector[searchsector].ceilingshade;
					tempxrepeat=sector[searchsector].ceilingxpanning;
					tempyrepeat=sector[searchsector].ceilingypanning;
					tempcstat=sector[searchsector].ceilingstat;
					break;
			 case 2:
					temppicnum=sector[searchsector].floorpicnum;
					tempshade=sector[searchsector].floorshade;
					tempxrepeat=sector[searchsector].floorxpanning;
					tempyrepeat=sector[searchsector].floorypanning;
					tempcstat=sector[searchsector].floorstat;
					break;
			 case 3:
					temppicnum=sprite[searchwall].picnum;
					tempshade=sprite[searchwall].shade;
					tempxrepeat=sprite[searchwall].xrepeat;
					tempyrepeat=sprite[searchwall].yrepeat;
					tempcstat=sprite[searchwall].cstat;
					break;
			 case 4:
					temppicnum=wall[searchwall].overpicnum;
					tempshade=wall[searchwall].shade;
					tempxrepeat=wall[searchwall].xrepeat;
					tempyrepeat=wall[searchwall].yrepeat;
					tempcstat=wall[searchwall].cstat;
			 }
	  }
}

void
Keys2d(void)
{

}

void ExtInit(void)
{
	long i;

		//You can load your own palette lookup tables here if you just
		//copy the right code!
	for(i=0;i<256;i++)
		tempbuf[i] = ((i+32)&255);  //remap colors for screwy palette sectors
	makepalookup(16,tempbuf,0,0,0);

	kensplayerheight = 54;
	zmode = 0;

}

void ExtUnInit(void)
{
}

void ExtCheckKeys(void)
{
	long i;

	if (qsetmode == 200)    //In 3D mode
	{
		i = totalclock;
		if (i != frameval[framecnt])
		{
			sprintf(tempbuf,"%ld",(120*AVERAGEFRAMES)/(i-frameval[framecnt]));
			printext256(0L,0L,31,-1,tempbuf,1);
			frameval[framecnt] = i;
		}
		framecnt = ((framecnt+1)&(AVERAGEFRAMES-1));

		editinput();
	}
	else
	{
	}
}

