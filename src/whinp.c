/***************************************************************************
 *   WHINP.C  - main game code for Apogee engine                           *
 *                                                                         *
 ***************************************************************************/

#include "icorp.h"
#include "keydefs.h"                                             // Les 07/24/95

int soundcontrol;
int musiclevel;
int digilevel;
int soundtoggle;

extern int mapon;

int followmode=0;
extern int followx;
extern int followy;

//JSA BLORB
extern int lavasnd;
int justplayed=0;
int lopoint=0;
int walktoggle=0;
int runningtime=0;

short cybyaw,cybpit,cybrol;

int charsperline=0;
char   nettemp[80];
extern char typemessage[];
extern char typemessageleng, typemode;
extern char scantoasc[];
extern char scantoascwithshift[];
int nettypemode=0;

//scantoasc

extern char displaybuf[];
extern char tempbuf[];
extern int  displaytime;
extern int  spiked;

int mousecalibrate=0;
int mousespeed=3;
int  mousxspeed=3,
	 mousyspeed=3;

int angvel,
	 svel,
	 vel;


extern int escapetomenu;
extern int currweaponflip;
extern int invincibletime;
extern int manatime;

int lockclock;

int oldhoriz;

int pressedkey;
int testpaleffects=0;

char oldjoyb,
	 oldbstatus,
	 butbit[]={0x10,0x20,0x40,0x80},
	 mbutbit[]={0x01,0x02};

short jcalibration=0,
	 jctrx,
	 jctry,
	 jlowx,jhighx,
	 jlowy,jhighy,
	 jmovespeed=16,
	 joyx,joyy,
	 joykeys[4],
	 jstickenabled=0,
	 jturnspeed=16;

short mousekeys[2];

char option2[7];

extern int wet;
extern char foggy;
extern char tremors;
extern char flashflag;

extern int currweaponfired;

extern int currentorb;
extern int currentpotion;
extern int selectedgun;
extern int currweapon;
extern int hasshot;
extern int orbshot;
extern int showstatusbar;
extern int showheart;
extern int showbook;

short  oldmousestatus, brightness, gbrightness;
extern short  compass;
extern int playerdie;

short pitch,
     roll,
     yaw;

int vrangle,vrangle2,
     vrdelta,
     vrpitch;

void
initjstick(void)
{
	 jcalibration=1;
	 jstickenabled=0;
	 if (option2[3] == 0) {
		  option2[3]=KEYFIRE;
	 }
	 if (option2[4] == 0) {
		  option2[4]=KEYUSE;
	 }
	 if (option2[5] == 0) {
		  option2[5]=KEYUSEP;
	 }
	 if (option2[6] == 0) {
		  option2[6]=KEYSTRAFE;
	 }
	 joykeys[0]=option2[3];
	 joykeys[1]=option2[4];
	 joykeys[2]=option2[5];
	 joykeys[3]=option2[6];
}

void
keytimerstuff(void)
{

    if(keystatus[keys[KEYSTRAFE]] == 0) {
		if(keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
			angvel-=16;
			if(angvel < -128) {
				angvel=-128;
			}
		}
		if(keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
			angvel+=16;
			if(angvel > 127) {
				angvel=127;
			}
		}
	}
	else {
		if(keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
			svel+=8;
			if(svel > 127) {
				svel=127;
			}
		}
		if(keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
			svel-=8;
			if(svel < -128) {
				svel=-128;
			}
		}
	}

	 if (keystatus[keys[KEYSTFL]] != 0) {
		  svel+=8;
		  if (svel > 127) {
			   svel=127;
		  }
	 }
	 else if (keystatus[keys[KEYSTFR]] != 0) {
		  svel-=8;
		  if (svel < -128) {
			   svel=-128;
		  }
	 }

	if(keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
		vel+=8;
		if(vel > 201) {
			vel=201;
		}
	}

	if(keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
		vel-=8;
		if(vel < -201) {
			vel=-201;
		}
	}

	if(angvel < 0) {
		angvel+=12;
		if(angvel > 0) {
			angvel=0;
		}
	}

	if(angvel > 0) {
		angvel-=12;
		if(angvel < 0) {
			angvel=0;
		}
	}

	if(svel < 0) {
		svel+=2;
		if(svel > 0) {
			svel=0;
		}
	}

	if(svel > 0) {
		svel-=2;
		if(svel < 0) {
			svel=0;
		}
	}

	if(vel < 0) {
		vel+=2;
		if(vel > 0) {
			vel=0;
		}
	}

	if(vel > 0) {
		vel-=2;
		if(vel < 0) {
			vel=0;
		}
	}

}

void
dophysics(struct player *plr,int goalz,short flyupdn,int v)
{
	 if (plr->orbactive[5] > 0) {
		  if (v > 0) {
			   if (plr->horiz > 125)
					plr->hvel-=(synctics<<6);
			   else if (plr->horiz < 75)
					plr->hvel+=(synctics<<6);
		  }
		  else {
			   if (flyupdn > 0) {
					plr->hvel-=(synctics<<7);
			   }
			   if (flyupdn < 0) {
					plr->hvel+=(synctics<<7);
			   }
		  }
		  plr->hvel+=(sintable[(lockclock<<4)&2047]>>6);
		  plr->fallz=0L;
	 }
	 else if (plr->z < goalz) {
		  plr->hvel+=GRAVITYCONSTANT;
		  plr->onsomething&=~(GROUNDBIT|PLATFORMBIT);
		  plr->fallz+=plr->hvel;
	 }
	 else if (plr->z > goalz) {
		  plr->hvel-=((plr->z-goalz)>>6);
		  plr->onsomething|=GROUNDBIT;
		  plr->fallz=0L;
	 }
	 else {
		  plr->fallz=0L;
	 }
	 plr->z+=plr->hvel;
	 if (plr->hvel > 0 && plr->z > goalz) {
		  plr->hvel>>=2;
	 }
	 else if (plr->onsomething != 0) {
		  if (plr->hvel < 0 && plr->z < goalz) {
			   plr->hvel=0;
			   plr->z=goalz;
		  }
	 }
	 if (plr->z < sector[plr->sector].ceilingz) {
		  plr->z=sector[plr->sector].ceilingz+(plr->height>>2);
		  plr->hvel=0;
	 }
	 else if (plr->z > sector[plr->sector].floorz) {
		  plr->z=sector[plr->sector].floorz-(plr->height>>4);
		  plr->hvel=0;
	 }
#if 0
	 if (plr->forcev != 0) {
		  *xvect=(int)((plr->forcev*(int)sintable[(plr->forcea+512)&2047])>>3);
		  *yvect=(int)((plr->forcev*(int)sintable[plr->forcea])>>3);
		  plr->forcev>>=1;
	 }
#endif
}

void processinput(struct player *plr) {

    short bstatus;
    int mousx,mousy;
	int goalz,hihit,hiz,i,lohit,loz,tics,xvect,yvect;
	int dax,dax2,day,day2,odax,odax2,oday,oday2;
	int  a,s,v;
	 static int  mv;
	int oldposx, oldposy;
	int dist;
	int feetoffground;
	char *ptr;
	short hitobject, onsprite=-1;
	static short tempsectornum;
	short onground;

	a=angvel;
	s=svel;
	v=vel;

	//if backspace > 0 then type mode = 1
	//backspace = 0

	if( keystatus[0xe] > 0 ) {
		if( netgame == 0 ) {
			if(typemode == 1) {
				typemode=0;
				charsperline=0;
				typemessageleng=0;
			}
			else {
				typemode=1;
			}
			keystatus[0xe]=0;
		}
		else {
			if(nettypemode == 1) {
				nettypemode=0;
				charsperline=0;
				typemessageleng=0;
				strcpy(nettemp,"");
			}
			else {
				nettypemode=1;
				typemessageleng=0;
			}
			keystatus[0xe]=0;
		}
	}

	if (typemode == 1) {
		typeletter();
	}

	if (nettypemode == 1) {
		nettypeletter();
	}


	if(keystatus[1] > 0) {
		keystatus[1]=0;
		if(plr->z < sector[plr->sector].floorz-((PLAYERHEIGHT+8)<<8)
		   //     || sector[plr->sector].floorpicnum == LAVA
		   //     || sector[plr->sector].floorpicnum == SLIME
		   //    || sector[plr->sector].floorpicnum == WATER
		   //     || sector[plr->sector].floorpicnum == HEALTHWATER
		   //     || sector[plr->sector].floorpicnum == ANILAVA
		   //     || sector[plr->sector].floorpicnum == LAVA1
		   //     || sector[plr->sector].floorpicnum == LAVA2 )
		   ) {
				strcpy(displaybuf,"must be on the ground");
				displaytime=360;
		}
		else {
			escapetomenu=1;
			plr->z=sector[plr->sector].floorz-(PLAYERHEIGHT<<8);
			setsprite(plr->spritenum,plr->x,plr->y,plr->z+(plr->height<<8));
			sprite[plr->spritenum].ang=plr->ang;
			//visibility=2048;
			visibility=1024;
		}
	}

	if( followmode == 1) {
		if(keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
			followx-=synctics<<5;
		}
		if(keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
			followx+=synctics<<5;
		}
		if(keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
			followy-=synctics<<5;
		}
		if(keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
			followy+=synctics<<5;
		}

		if( labs(followx) >= 65536-1024 )
			followx=65536-1024;

		if( labs(followy) >= 65536-1024 )
			followy=65536-1024;

		if( keystatus[0x21] > 0) {
			followmode=0;
			keystatus[0x21]=0;
		}
		return;
	}


	if (keystatus[0x44] > 0) {  //F10 - brightness
		keystatus[0x44]=0;
		gbrightness=brightness++;
		if (brightness > 8) brightness=0 , gbrightness=0;
			setbrightness(brightness,palette,0);
	}


	if( keystatus[0x43] > 0 ) { //F9 sound controls
		if( soundtoggle == 1)
			soundtoggle=0;
		else
			soundtoggle=1;
	}

	if( soundtoggle == 1) {
		dosoundthing();
	}


	if( option[3] != 0 ) {
		if( keystatus[0x58] > 0 ) {
			if( mousecalibrate == 1 ) {
				mousecalibrate=0;
				strcpy(displaybuf,"Mouse calibrate off");
				displaytime=40;
			}
			else {
				mousecalibrate=1;
				strcpy(displaybuf,"Mouse calibrate on");
				displaytime=360;
			}
			keystatus[0x58]=0;
		}
	}

	if( mousecalibrate == 1 ) {
		if( keystatus[keys[KEYRIGHT]] != 0 ) {
			mousxspeed++;
			if( mousxspeed > 16 )
				mousxspeed=16;

			sprintf(displaybuf,"Mouse Speed x %d",mousxspeed);
			displaytime=40;

		}
		if( keystatus[keys[KEYLEFT]] != 0 ) {
			mousxspeed--;
			if( mousxspeed < 1 )
				mousxspeed=1;

			sprintf(displaybuf,"Mouse Speed x %d",mousxspeed);
			displaytime=40;

		}
		if( keystatus[keys[KEYFWD]] != 0 ) {
			mousyspeed++;
			if( mousyspeed > 16 )
				mousyspeed=16;

			sprintf(displaybuf,"Mouse Speed y %d",mousyspeed);
			displaytime=40;

		}
		if( keystatus[keys[KEYBACK]] != 0 ) {
			mousyspeed--;
			if( mousyspeed < 1 )
				mousyspeed=1;

			sprintf(displaybuf,"Mouse Speed y %d",mousyspeed);
			displaytime=40;

		}
	}

	 if (jstickenabled) {
		  //jstick();
		  //sprintf(displaybuf,"%d %d %X",joyx,joyy,joyb);
		  //displaytime=360;
		  if (keystatus[0x57] != 0) {   // recalibrate joystick (F11)
			   jstickenabled=0;
			   jcalibration=1;
		  }

		  if (joyx < jlowx) {
			   //a-=(jturnspeed<<3);
			   keystatus[keys[KEYLEFT]]=1;
		  }
		  else
			keystatus[keys[KEYLEFT]]=0;

		  if (joyx > jhighx) {
			   //a+=(jturnspeed<<3);
			   keystatus[keys[KEYRIGHT]]=1;
		  }
		  else
			keystatus[keys[KEYRIGHT]]=0;

		  if (joyy < jlowy) {
			   v+=(jmovespeed<<3);
		  }
		  else if (joyy > jhighy) {
			   v-=(jmovespeed<<3);
		  }

		  for (i=0 ; i < 4 ; i++) {
			   if (((joyb&butbit[i]) != butbit[i])          // button is down
				  && ((oldjoyb&butbit[i]) == butbit[i])) {
					keystatus[keys[joykeys[i]]]=1;
					if (joykeys[i] == KEYRUN) {
						 keystatus[keys[KEYFWD]]=1;
					}
			   }
			   else if (((joyb&butbit[i]) != butbit[i])     // button still down
				  && ((oldjoyb&butbit[i]) != butbit[i])) {
					if (joykeys[i] == KEYUSE                // ..one-time actions
					   || joykeys[i] == KEYJUMP
					   || joykeys[i] == KEYMAP
					   || joykeys[i] == KEYUSEP
					   || joykeys[i] == KEYCAST) {
						 keystatus[keys[joykeys[i]]]=0;
					}
			   }
			   else if (((joyb&butbit[i]) == butbit[i])     // button released
				  && ((oldjoyb&butbit[i]) != butbit[i])) {
					keystatus[keys[joykeys[i]]]=0;
					if (joykeys[i] == KEYRUN) {
						 keystatus[keys[KEYFWD]]=0;
					}
			   }
		  }
		  oldjoyb=joyb;
	 }
	 else if (jcalibration) {
		  //jstick();
		  switch (jcalibration) {
		  case 1:
			   strcpy(displaybuf,"center stick press button");
			   displaytime=360;
			   if (((joyb&0xF0) != 0xF0) && ((oldjoyb&0xF0) == 0xF0)) {
					jctrx=joyx;
					jctry=joyy;
					jcalibration++;
			   }
			   break;
		  case 2:
			   strcpy(displaybuf,"top left press button");
			   displaytime=360;
			   if (((joyb&0xF0) != 0xF0) && ((oldjoyb&0xF0) == 0xF0)) {
					jlowx=jctrx-((jctrx-joyx)/4);
					jlowy=jctry-((jctry-joyy)/4);
					jcalibration++;
			   }
			   break;
		  case 3:
			   strcpy(displaybuf,"lower right press button");
			   displaytime=360;
			   if (((joyb&0xF0) != 0xF0) && ((oldjoyb&0xF0) == 0xF0)) {
					jhighx=jctrx+((joyx-jctrx)/4);
					jhighy=jctry+((joyy-jctry)/4);
					jcalibration=0;
					jstickenabled=1;
			   }
			   break;
		  }
		  oldjoyb=joyb;
	 }

	if (v < -201) v=-201;
	else if (v > 201) v=201;

	v+=v>>1;// SUPER MARIO BROTHERS

	if (s < -201) s=-201;
	else if (s > 201) s=201;

	if (a < -112) a=-112;
	else if (a > 112) a=112;

	if(option[3] != 0) {
		getmousevalues(&mousx,&mousy,&bstatus);
		  if(keystatus[keys[KEYSTRAFE]] != 0) {
			i=s;
			i-=(mousx*mousxspeed);
		  }
		  else {
			i=a;
			i+=(mousx*mousxspeed);                                 // Les 07/24/95
		  }

		  //i=a;
//          i+=(mousx<<3);
//          i+=(mousx*mousxspeed);                                 // Les 07/24/95

		  if (i < -128) i=-128;
		  if (i > 127) i=127;

		  //a=i;
		  if(keystatus[keys[KEYSTRAFE]] != 0)
			s=i;
		  else
			a=i;

// Les START - 07/24/95  - if key KEYLOOKING is held, mouse fwd/back looks up/down
		  if (keystatus[keys[KEYLOOKING]]) {
			   i=plr->horiz;
			   i+=(mousy>>4);
			   if(i < 100-(YDIM>>1)) i=100-(YDIM>>1);
			   else if(i > 100+(YDIM>>1)) i=100+(YDIM>>1);
			   plr->horiz=i;
		  }
//                         else mouse fwd/back moves fwd/back
		  else {
			   i=v;
//               i+=(mousy<<3);
			   i-=(mousy*mousyspeed);                            // Les 07/24/95
			   if (i < -128) {
					i=-128;
			   }
			   else if (i > 127) {
					i=127;
			   }
			   v=i;
		  }
		  for (i=0 ; i < 2 ; i++) {
			   if (((bstatus&mbutbit[i]) == mbutbit[i])     // button is down
				  && ((oldbstatus&mbutbit[i]) != mbutbit[i])) {
					keystatus[keys[mousekeys[i]]]=1;
					if (mousekeys[i] == KEYRUN) {
						 keystatus[keys[KEYFWD]]=1;
					}
			   }
			   else if (((bstatus&mbutbit[i]) == mbutbit[i])// button still down
				  && ((oldbstatus&mbutbit[i]) == mbutbit[i])) {
					if (mousekeys[i] == KEYUSE              // ..one-time actions
					   || mousekeys[i] == KEYJUMP
					   || mousekeys[i] == KEYMAP
					   || mousekeys[i] == KEYUSEP
					   || mousekeys[i] == KEYCAST) {
						 keystatus[keys[mousekeys[i]]]=0;
					}
			   }
			   else if (((bstatus&mbutbit[i]) != mbutbit[i])// button released
				  && ((oldbstatus&mbutbit[i]) == mbutbit[i])) {
					keystatus[keys[mousekeys[i]]]=0;
					if (mousekeys[i] == KEYRUN) {
						 keystatus[keys[KEYFWD]]=0;
					}
			   }
		  }
		  oldbstatus=bstatus;
#if 0                                                            // Les 07/27/95
//          if((bstatus&0x02) != 0 && keystatus[keys[KEYFWD]] == 0 && keystatus[keys[KEYBACK]] == 0) {
		  if ((bstatus&0x02) != 0) {
			   mv+=16;
//               if(mv > 201) {
//                    mv=201;
//               }
			v+=mv;
			   if (v > 201) {                                    // Les 07/24/95
					v=201;                                       // Les 07/24/95
			   }                                                 // Les 07/24/95
		}
		else
			mv=0;
#endif                                                           // Les 07/27/95
	}


	i=totalclock-lockclock;
	if (i > 255)
		i=255;

	synctics=tics=i;
	lockclock+=(int)synctics;


	sprite[plr->spritenum].cstat^=1;
	getzrange(plr->x,plr->y,plr->z,plr->sector,&hiz,&hihit,&loz,&lohit,128L,CLIPMASK0);
	sprite[plr->spritenum].cstat^=1;

		if ((lohit&0xc000) == 49152) {
			if( (sprite[lohit&4095].z-plr->z) <= (PLAYERHEIGHT<<8) )
				onsprite=(lohit&4095);
		}
		else
			onsprite=-1;

	feetoffground=(sector[plr->sector].floorz-plr->z);

	 if((keystatus[keys[KEYRUN]] != 0 || keystatus[RSHIFT] != 0) || v > 201) {
//     if (keystatus[keys[KEYRUN]] != 0 || v > 201) {              // Les 07/24/95

		if(v > 201)
			v=201;

			v+=v>>1;// SUPER MARIO BROTHERS

		if(feetoffground > (32<<8))
			tics+=tics>>1;
	 }

//      if(keystatus[0xd1] != 0 ) {
	  if (keystatus[keys[KEYLKDN]] != 0 ) {
		if(plr->horiz > 100-(YDIM>>1)) {
			plr->horiz-=(synctics<<1);
			autohoriz=0;
		}
	 }

//      else if(keystatus[0xc9] != 0) {
	  else if (keystatus[keys[KEYLKUP]] != 0) {
		  if(plr->horiz < 100+(YDIM>>1))
			   plr->horiz+=(synctics<<1);
			   autohoriz=0;
	 }

	  if(keystatus[0xcf] != 0)
//      if (keystatus[keys[KEYUNCAST]] != 0)                       // Les 07/24/95
		plr->orbactive[5]=-1;

//     oldmousestatus=bstatus&0x01;                              // Les 07/27/95

//      if(keystatus[keys[KEYFIRE]] != 0 || keystatus[0x9d] != 0 || (oldmousestatus) != 0 && hasshot == 0) {
//      if (keystatus[keys[KEYFIRE]] != 0 || (oldmousestatus) != 0 && hasshot == 0) { // Les 07/24/95
	 if (keystatus[keys[KEYFIRE]] != 0 && hasshot == 0) {        // Les 07/27/95
		if(currweaponfired == 0)
			plrfireweapon(plr);
	 }

	 // cast
//      if( ( keystatus[LALT] > 0 || keystatus[RALT] > 0 ) && orbshot == 0 && currweaponflip == 0) {
	  if (keystatus[keys[KEYCAST]] > 0 && orbshot == 0 && currweaponflip == 0) { // Les 07/24/95
		if(plr->orb[currentorb] == 1 && selectedgun == 0) {
			if(lvlspellcheck(plr)) {
				orbshot=1;
				//JSA_NEW
				activatedaorb(plr);
			}
		 }
		 if(selectedgun != 0)
			keystatus[WEAPONONE]=1;

//           keystatus[LALT] = 0 , keystatus[RALT] = 0;
		   keystatus[keys[KEYCAST]]=0;                           // Les 07/24/95
	 }

//      if(keystatus[0x1c] > 0) {
	  if (keystatus[keys[KEYUSEP]] > 0) {                        // Les 07/24/95
//          keystatus[0x1c]=0;
		  keystatus[keys[KEYUSEP]]=0;                            // Les 07/24/95
		if(plr->potion[currentpotion] > 0) {
			usapotion(plr);
		  }
	 }

	 if(keystatus[keys[KEYUSE]] != 0) {
		  keystatus[keys[KEYUSE]]=0;
		  if (netgame) {
			   netdropflag();
		  }
		  else {
			   plruse(plr);
		  }
	 }

	if((sector[plr->sector].floorpicnum != LAVA
		|| sector[plr->sector].floorpicnum != SLIME
		|| sector[plr->sector].floorpicnum != WATER
		|| sector[plr->sector].floorpicnum != HEALTHWATER
		|| sector[plr->sector].floorpicnum != ANILAVA
		|| sector[plr->sector].floorpicnum != LAVA1
		|| sector[plr->sector].floorpicnum != LAVA2)
		&& feetoffground <= (32<<8) )
		v>>=1;


	if((sector[plr->sector].floorpicnum == LAVA
		|| sector[plr->sector].floorpicnum == SLIME
		|| sector[plr->sector].floorpicnum == WATER
		|| sector[plr->sector].floorpicnum == HEALTHWATER
		|| sector[plr->sector].floorpicnum == ANILAVA
		|| sector[plr->sector].floorpicnum == LAVA1
		|| sector[plr->sector].floorpicnum == LAVA2)
		&& plr->orbactive[5] < 0                                 //loz
		&& plr->z >= sector[plr->sector].floorz-(plr->height<<8)-(8<<8) ) {
			//goalz = loz-(24<<8);
			goalz = loz-(32<<8);
			switch( sector[plr->sector].floorpicnum ) {
			case ANILAVA:
			case LAVA:
			case LAVA1:
			case LAVA2:
				//if( plr->treasure[5] == 0) {
					//v-=v>>3;
					if( plr->treasure[5] == 1) {
						goalz=loz-(PLAYERHEIGHT<<8);
					break;
					}
					else
						v-=v>>3;

					if( invincibletime > 0 || manatime > 0 )
					break;
					else {
						if( lavasnd == -1 ) {
							lavasnd=SND_PlaySound(S_FIRELOOP1,0,0,0,-1);
						}
						healthpic(-1);
						startredflash(10);
					}

				//}
			break;
			case WATER:
			   //if( plr->treasure[4] == 0) {
				//v-=v>>3;
				if( plr->treasure[4] == 1) {
					goalz=loz-(PLAYERHEIGHT<<8);
				}
				else
					v-=v>>3;

				//}
			break;
			case HEALTHWATER:
				if( plr->health < plr->maxhealth) {
					healthpic(1);
					startblueflash(5);
				}
			break;
			}
	}
	else if(plr->orbactive[5] > 0) {
		goalz=plr->z-(plr->height<<8);
		plr->hvel=0;
	}
	else
		goalz=loz-(plr->height<<8);

	//    if( plr->z < sector[plr->sector].floorz-(12<<8) )
		if (keystatus[keys[KEYJUMP]] != 0) {                   // Les 07/24/95
			if (plr->onsomething) {
				 plr->hvel-=JUMPVEL;
				 plr->onsomething=0;
			}
			keystatus[keys[KEYJUMP]]=0;//WANGO
		}

		if (keystatus[keys[KEYCROUCH]] != 0) {                 // Les 07/24/95
			if(goalz<((sector[plr->sector].floorz)-(plr->height>>3)))
				//goalz+=(32<<8);
				goalz+=(24<<8);

	}

	if(keystatus[keys[KEYMAP]] != 0) {                         // Les 07/24/95
		if(plr->dimension == 3) {
			plr->dimension=2;
			sprintf(displaybuf,"map %d",mapon);
			displaytime=720;
		}
		else {
			plr->dimension=3;
			followmode=0;
		}
		keystatus[keys[KEYMAP]]=0;                             // Les 07/24/95
	}

	if(plr->dimension == 2) {
		if(keystatus[keys[KEYZOOMO]] > 0)                     // Les 07/24/95
			if(plr->zoom > 48)
				plr->zoom-=(plr->zoom>>4);
		if(keystatus[keys[KEYZOOMI]] > 0)                     // Les 07/24/95
			if(plr->zoom < 4096)
				plr->zoom+=(plr->zoom>>4);
		if(keystatus[0x21] > 0) {
			if(followmode == 0) {
				sprintf(displaybuf,"Map %d",mapon);
				displaytime=360;
				followmode=1;
				followx=0L;
				followy=0L;
			}
			else
				followmode=0;
			keystatus[0x21]=0;
		}
	}


	else if(plr->dimension == 3 && svga == 0) {
		if (keystatus[keys[KEYZOOMO]] != 0 && plr->screensize > 64) { // Les 07/24/95
			if(plr->screensize <= 320) {
				updatepics();
			}
			plr->screensize-=8;
			dax=(XDIM>>1)-(plr->screensize>>1);
			dax2=dax+plr->screensize-1;
			day=(STATUSSCREEN>>1)-(((plr->screensize*STATUSSCREEN)/XDIM)>>1);
			day2=day+((plr->screensize*STATUSSCREEN)/XDIM)-1;
			setview(dax,day,dax2,day2);
			odax=(YDIM>>1)-((plr->screensize+8)>>1);
			odax2=dax+(plr->screensize+8)-1;
			oday=(STATUSSCREEN>>1)-((((plr->screensize+8)*STATUSSCREEN)/XDIM)>>1);
			oday2=day+(((plr->screensize+8)*STATUSSCREEN)/XDIM)-1;
			permanentwritesprite(0,0,BACKGROUND,0,odax,oday,dax-1,oday2,0);
			permanentwritesprite(0,0,BACKGROUND,0,dax2+1,oday,odax2,oday2,0);
			permanentwritesprite(0,0,BACKGROUND,0,dax,oday,dax2,day-1,0);
			permanentwritesprite(0,0,BACKGROUND,0,dax,day2+1,dax2,oday2,0);
			if(plr->screensize == XDIM) {
				permanentwritesprite(0,200-46,NEWSTATUSBAR,0,0,0,XDIM-1,YDIM-1,0);
				updatepics();
			}
		}

		if (keystatus[keys[KEYZOOMI]] != 0 && plr->screensize <= XDIM) { // Les 07/24/95
			plr->screensize+=8;
			if(plr->screensize > XDIM) {
				dax=day=0;
				dax2=XDIM-1;
				day2=YDIM-1;
			}
			else {
				dax=(XDIM>>1)-(plr->screensize>>1);
				dax2=dax+plr->screensize-1;
				day=(STATUSSCREEN>>1)-(((plr->screensize*STATUSSCREEN)/XDIM)>>1);
				day2=day+((plr->screensize*STATUSSCREEN)/XDIM)-1;
			}
			setview(dax,day,dax2,day2);
		}
	}

//SVGA STUFF WANGO
	if(plr->dimension == 3 && svga == 1) {
		if (keystatus[keys[KEYZOOMO]] != 0) {
			plr->screensize=320;
			setview(0,0,640-1,372-1);
			overwritesprite(0,372,SSTATUSBAR,0,0,0);
			updatepics();
		}

		if (keystatus[keys[KEYZOOMI]] != 0 ) {
			plr->screensize=328;
			setview(0,0,640-1,480-1);
		}
	}
// SVGA END WANGO

	onground=plr->onsomething;
	if (keystatus[keys[KEYFLYUP]]) {
		dophysics(plr,goalz,1,v);
	}
	else if (keystatus[keys[KEYFLYDN]]) {
		dophysics(plr,goalz,-1,v);
	}
	else {
		dophysics(plr,goalz,0,v);
	}
	if (!onground && plr->onsomething) {
	   if (plr->fallz > 32768L) {
		  if(rand()%2)
			  playsound_loc(S_PLRPAIN1+(rand()%2),plr->x,plr->y);
		  else
			  playsound_loc(S_PUSH1+(rand()%2),plr->x,plr->y);

		  healthpic(-(plr->fallz>>13));
		  plr->fallz=0L;//wango
	   }
	   else if (plr->fallz > 8192L) {
			playsound_loc(S_BREATH1+(rand()%2),plr->x,plr->y);

	   }
	}

	if( ihaveflag > 0 )
		v-=v>>2;

	if(v != 0 || s != 0) {
		xvect=yvect=0;
		if (v != 0) {
			xvect=(v*(int)tics*(int)sintable[((plr->ang+2560)&2047)])>>3;
			yvect=(v*(int)tics*(int)sintable[((plr->ang+2048)&2047)])>>3;
		}
		if (s != 0) {
			xvect+=((s*(int)tics*(int)sintable[((plr->ang+2048)&2047)])>>3);
			yvect+=((s*(int)tics*(int)sintable[((plr->ang+1536)&2047)])>>3);
		}

		oldposx=plr->x; oldposy=plr->y;

		clipmove(&plr->x,&plr->y,&plr->z,&plr->sector,xvect,yvect,128L,4<<8,4<<8,CLIPMASK0);


//JSA BLORB

	  if (plr->sector!=tempsectornum) {
		 if( lavasnd != -1 )
			switch( sector[plr->sector].floorpicnum ) {
			case ANILAVA:
			case LAVA:
			case LAVA1:
			case LAVA2:
			break;
			default:
			SND_StopLoop(lavasnd);
			lavasnd=-1;
			break;
			}
		 sectorsounds();
	  }

	  tempsectornum=plr->sector;


//walking on sprite
		plr->horiz-=oldhoriz;

		dist=ksqrt((plr->x-oldposx)*(plr->x-oldposx)+(plr->y-oldposy)*(plr->y-oldposy));

		  if (keystatus[keys[KEYRUN]] > 0)                       // Les 07/24/95
			dist>>=2;

		if( dist > 0 && feetoffground <= (plr->height<<8) || onsprite != -1) {
			if( svga == 1)
				 oldhoriz = ((dist*sintable[(totalclock<<5)&2047])>>19)>>2;
			else
				 oldhoriz = ((dist*sintable[(totalclock<<5)&2047])>>19)>>1;
			plr->horiz += oldhoriz;
		}
		else
			oldhoriz=0;

		if( plr->horiz > 200) plr->horiz=200;
		if( plr->horiz < 0) plr->horiz=0;

		if( onsprite != -1 && dist > 50 && lopoint == 1 && justplayed == 0) {
//JSA SPOOGE
			switch(sprite[onsprite].picnum) {
			case WALLARROW:
			case OPENCHEST:
			case GIFTBOX:
				if(walktoggle)
					playsound_loc(S_WOOD1,(plr->x+3000),plr->y);
					else
					playsound_loc(S_WOOD1,plr->x,(plr->y+3000));
					walktoggle^=1;
				justplayed=1;
			break;
			case WOODPLANK:           //wood planks
				if(walktoggle)
					playsound_loc(S_SOFTCHAINWALK,(plr->x+3000),plr->y);
					else
					playsound_loc(S_SOFTCHAINWALK,plr->x,(plr->y+3000));
					walktoggle^=1;
				justplayed=1;

			break;
			case SQUAREGRATE:      //square grating
			case SQUAREGRATE+1:
				if(walktoggle)
					playsound_loc(S_LOUDCHAINWALK,(plr->x+3000),plr->y);
					else
					playsound_loc(S_LOUDCHAINWALK,plr->x,(plr->y+3000));
					walktoggle^=1;
				justplayed=1;
			break;
			case SPACEPLANK:      //spaced planks
				if(walktoggle)
					playsound_loc(S_SOFTCREAKWALK,(plr->x+3000),plr->y);
					else
					playsound_loc(S_SOFTCREAKWALK,plr->x,(plr->y+3000));
					walktoggle^=1;
				justplayed=1;
			break;
			case RAT:
				playsound_loc(S_RATS1+(rand()%2),sprite[i].x,sprite[i].y);
				justplayed=1;
				deletesprite((short)i);
			break;
			case SPIDER:
				//STOMP
				playsound_loc(S_DEADSTEP,sprite[i].x,sprite[i].y);
				justplayed=1;
				newstatus(i,DIE);
			break;

			case FREDDEAD:
			case 1980:
			case 1981:
			case 1984:
			case 1979:
			case 1957:
			case 1955:
			case 1953:
			case 1952:
			case 1941:
			case 1940:
				playsound_loc(S_DEADSTEP,plr->x,plr->y);
				justplayed=1;

			break;

			default:
			break;
			}

		}

		if( lopoint == 0 && oldhoriz == -2 && justplayed == 0)
			lopoint=1;

		if( lopoint == 1 && oldhoriz != -2 && justplayed == 1) {
			lopoint=0;
			justplayed=0;
		}


		if( vel > 199 || vel < -199 && dist > 10)
			runningtime+=synctics;
		else
			runningtime-=synctics;

		if( runningtime < -360 )
			runningtime=0;

		if( runningtime > 360 ) {
			SND_PlaySound(S_PLRPAIN1,0,0,0,0);
			runningtime=0;
		}


	}
	if (a != 0) {
		plr->ang+=((a*(int)synctics)>>4);
		plr->ang=(plr->ang+2048)&2047;
	}

	setsprite(plr->spritenum,plr->x,plr->y,plr->z+(plr->height<<8));
	sprite[plr->spritenum].ang=plr->ang;


	if(sector[plr->sector].ceilingz > sector[plr->sector].floorz-(8<<8) )
		healthpic(-10);


	if(plr->health <= 0 ) {

//JSA BLORB
		SND_CheckLoops();

		playerdead(plr);
	}

//     if( keystatus[0xc7] > 0) { // home key
	 if (keystatus[keys[KEYCNTR]] > 0) { // home key             Les 07/24/95
		autohoriz=1;
		keystatus[keys[KEYCNTR]]=0;
	}
	if(autohoriz == 1)
		autothehoriz(plr);

	singleshot(bstatus);

	weaponchange();

}

void autothehoriz(struct player *plr) {

	if(plr->horiz < 100)
		plr->horiz+=synctics<<1;
	if(plr->horiz > 100)
		plr->horiz-=synctics<<1;
	if(plr->horiz >= 90 && plr->horiz <= 110)
		autohoriz=0;

}

#if 0
int gimmer=0;
int gimmes=0;
int gimmev=0;
int gimmep=0;

void cheatkeys(struct player *plr) {

	int i;

	/*
		the cheat code is R S V P
	*/

	if(keystatus[0x13] > 0) {
		gimmer=1;
//          keystatus[0x13]=0;                                   // Les 07/24/95
	}
	if(gimmer == 1 && keystatus[0x1f] > 0) {
		gimmes=1;
//          keystatus[0x1f]=0;                                   // Les 07/24/95
	}
	if(gimmes == 1 && keystatus[0x2f] > 0) {
		gimmev=1;
//          keystatus[0x2f]=0;                                   // Les 07/24/95
	}
	if(gimmev == 1 && keystatus[0x19] > 0) {
		gimmep=1;
//          keystatus[0x19]=0;                                   // Les 07/24/95
	}
	if(gimmep == 1) {
		gimmer=gimmes=gimmev=gimmep=0;

//        spiked=1;


		for(i=0;i<MAXPOTIONS;i++) {
			plr->potion[i]=9;
		}

		plr->weapon[1]=1;plr->ammo[1]=45; //DAGGER
		plr->weapon[2]=1;plr->ammo[2]=55; //MORNINGSTAR
		plr->weapon[3]=1;plr->ammo[3]=50; //SHORT SWORD
		plr->weapon[4]=1;plr->ammo[4]=80; //BROAD SWORD
		plr->weapon[5]=1;plr->ammo[5]=100; //BATTLE AXE
		plr->weapon[6]=1;plr->ammo[6]=50; // BOW
		plr->weapon[7]=2;plr->ammo[7]=40; //PIKE
		plr->weapon[8]=1;plr->ammo[8]=250; //TWO HANDED
		plr->weapon[9]=1;plr->ammo[9]=50;

		for(i=0;i<8;i++) {
			plr->orb[i]=1;
			plr->orbammo[i]=9;
		}

		currweapon=selectedgun=4;
		plr->health=0;
		healthpic(200);
		plr->armor=150;
		plr->armortype=3;
		plr->lvl=7;
		plr->maxhealth=200;
		plr->treasure[14]=1;
		plr->treasure[15]=1;
		plr->treasure[16]=1;
		plr->treasure[17]=1;
		updatepics();

	}


	//if(keystatus[0x20] > 0 && keystatus[0x18] > 0 && keystatus[0x31] > 0)
	//if( keystatus[0x58] > 0 )
	//    invincibletime=65000;

	return;
	// OHM
	if(keystatus[0x18] > 0 && keystatus[0x23] > 0 && keystatus[0x32] > 0)
		for(i=0;i<MAXWEAPONS;i++) {
			plr->weapon[i]=1;
			plr->ammo[i]=999;
		}

	// BINGO
	if(keystatus[0x30] > 0 && keystatus[0x17] > 0 && keystatus[0x31] > 0 && keystatus[0x22] > 0 && keystatus[0x18] > 0) {
		plr->health=1;
		healthpic(99);
	}


}
#endif


extern char fancy[];

void nettypeletter(void) {

	int i;

	strcpy(nettemp,"");

	if(typemessageleng <= 40) {
		for(i=0;i<128;i++) {
			if( keystatus[i] > 0 ) {
				nettemp[typemessageleng]=scantoasc[i];
				typemessageleng++;
				nettemp[typemessageleng]='\0';
				keystatus[i]=0;
			}
		}
		printext256(0L,0L,31,-1,Bstrupr(nettemp),1);
	}

}

void typeletter(void) {

	int i;
	int j;
	int exit=0;
	char temp[20];

	keystatus[0x1c]=keystatus[0x9c]=0;

	for(i=0;i<128;i++)
		keystatus[i]=0;

	strcpy(temp,"");

	while( !exit ) {

		if(typemessageleng <= 10)
		for(i=0;i<128;i++) {
			if(keystatus[i] > 0
				&& keystatus[0x0e] == 0    // keypressed not
				&& keystatus[1] == 0       //            esc
				&& keystatus[0x1c] == 0    //            enter l or r
				&& keystatus[0x9c] == 0) { //            backspace
				for(j=0;j<41;j++) {
					if(scantoasc[i] == ' ') continue;
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

		if( keystatus[1] > 0) {
			exit=1;
			keystatus[1]=0;
		}
		if ( keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
			exit=2;
			keystatus[0x1c]=keystatus[0x9c]=0;
		}
		strcpy(displaybuf,temp);
		fancyfontscreen(18,24,THEFONT,displaybuf);
		nextpage();
	}

	if( exit == 2 ) {
		keystatus[0x1c]=keystatus[0x9c]=0;
	}

	 checkcheat();
	 lockclock=totalclock;
	 typemode=0;
	 typemessageleng=0;

}

void checkcheat(void) {

	int i;
	struct player *plr;

	plr=&player[pyrn];

	Bstrupr(displaybuf);

	if(strcmp(displaybuf,"RSVP") == 0) {
		healthpic(-plr->health);
		plr->horiz=200;
		updatepics();
	}
	else if(strcmp(displaybuf,"RAMBO") == 0) {
		healthpic(-plr->health);
		plr->horiz=200;
		updatepics();
	}
	else if(strcmp(displaybuf,"IDKFA") == 0) {
		healthpic(-plr->health);
		plr->horiz=200;
		updatepics();
	}
	else if(strcmp(displaybuf,"SCOOTER") == 0) {
		plr->weapon[1]=1;plr->ammo[1]=45; //DAGGER
		plr->weapon[2]=1;plr->ammo[2]=55; //MORNINGSTAR
		plr->weapon[3]=1;plr->ammo[3]=50; //SHORT SWORD
		plr->weapon[4]=1;plr->ammo[4]=80; //BROAD SWORD
		plr->weapon[5]=1;plr->ammo[5]=100; //BATTLE AXE
		plr->weapon[6]=1;plr->ammo[6]=50; // BOW
		plr->weapon[7]=2;plr->ammo[7]=40; //PIKE
		plr->weapon[8]=1;plr->ammo[8]=250; //TWO HANDED
		plr->weapon[9]=1;plr->ammo[9]=50;
		currweapon=selectedgun=4;
		updatepics();
	}
	else if(strcmp(displaybuf,"MOMMY") == 0) {
		for(i=0;i<MAXPOTIONS;i++) {
			plr->potion[i]=9;
		}
		updatepics();
	}
	else if(strcmp(displaybuf,"WANGO") == 0) {
		for(i=0;i<8;i++) {
			plr->orb[i]=1;
			plr->orbammo[i]=9;
		}
		plr->health=0;
		healthpic(200);
		plr->armor=150;
		plr->armortype=3;
		plr->lvl=7;
		plr->maxhealth=200;
		plr->treasure[14]=1;
		plr->treasure[15]=1;
		plr->treasure[16]=1;
		plr->treasure[17]=1;
		updatepics();
	}
	strcpy(displaybuf,"");

}

void typecheat(char ch) {

  int i;
  int j;
  char tempbuf[40];

  charsperline=40;

  for(i=0;i<=typemessageleng;i+=charsperline) {
	for(j=0;j<charsperline;j++)
		tempbuf[j]=typemessage[i+j];
		if (typemessageleng < i+charsperline) {
			tempbuf[(typemessageleng-i)]='-';
			tempbuf[(typemessageleng-i)+1]=0;
		}
		else
			tempbuf[charsperline]=0;
  }

  strcpy(displaybuf,tempbuf);
  displaytime=360;


}

void dosoundthing(void) {

		musiclevel=(wMIDIVol>>3);
		digilevel=(wDIGIVol>>11);

		if( keystatus[keys[KEYFWD]] > 0)
			soundcontrol++;
		if( keystatus[keys[KEYBACK]] > 0)
			soundcontrol--;
		if( soundcontrol > 1)
			soundcontrol=1;
		if( soundcontrol < 0)
			soundcontrol=0;
		switch(soundcontrol) {
		case 0:
			if( keystatus[keys[KEYLEFT]] > 0) {
				musiclevel--;
				if( musiclevel < 0) {
					musiclevel=0;
					sprintf(displaybuf,"Music %d",musiclevel);
					displaytime=10;
				}
				else {
					sprintf(displaybuf,"Music %d",musiclevel);
					displaytime=10;
					SND_Mixer(1,musiclevel);
				}
				break;
			}

			else if( keystatus[keys[KEYRIGHT]] > 0) {
				musiclevel++;
				if( musiclevel > 16 ) {
					musiclevel=16;
					sprintf(displaybuf,"Music %d",musiclevel);
					displaytime=10;
				}
				else {
					sprintf(displaybuf,"Music %d",musiclevel);
					displaytime=10;
					SND_Mixer(1,musiclevel);
				}
				break;
			}

			else {
				sprintf(displaybuf,"Music %d",musiclevel);
				displaytime=10;
			}
			break;

		case 1:
			if( keystatus[keys[KEYLEFT]] > 0) {
				digilevel--;
				if( digilevel <0 ) {
					digilevel=0;
					sprintf(displaybuf,"Sounds %d",digilevel);
					displaytime=10;
				}
				else {
					sprintf(displaybuf,"Sounds %d",digilevel);
					displaytime=10;
					SND_Mixer(0,digilevel);
					SND_Sound(S_LOUDCHAINWALK);
				}
				break;
			}

			else if( keystatus[keys[KEYRIGHT]] > 0) {
				digilevel++;
				if( digilevel > 16 ) {
					digilevel=16;
					sprintf(displaybuf,"Sounds %d",digilevel);
					displaytime=10;
				}
				else {
					sprintf(displaybuf,"Sounds %d",digilevel);
					displaytime=10;
					SND_Mixer(0,digilevel);
					SND_Sound(S_LOUDCHAINWALK);
				}
				break;
			}

			else {
				sprintf(displaybuf,"Sounds %d",digilevel);
				displaytime=10;
			}
			break;

		}

}
