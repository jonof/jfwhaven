/***************************************************************************
 *   WHINP.C  - main game code for Apogee engine                           *
 *                                                                         *
 ***************************************************************************/

#include "icorp.h"
#include "keydefs.h"                                             // Les 07/24/95

int soundcontrol;
int musiclevel=16;
int digilevel=16;
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
extern int  typemessageleng, typemode;
extern char scantoasc[];
extern char scantoascwithshift[];
int nettypemode=0;

//scantoasc

extern char displaybuf[];
extern char tempbuf[];
extern int  displaytime;
extern int  spiked;

int mousecalibrate=0;
int  mousxspeed=3,
	 mousyspeed=3;
int mousekeys[2]={KEYFIRE,KEYUSE};

int angvel,
	 svel,
	 vel,
	 horizvel;

input loc;
input ffsync[MAXPLAYERS], ssync[MAXPLAYERS];
int   movefifoplc, movefifoend[MAXPLAYERS];
input baksync[MOVEFIFOSIZ][MAXPLAYERS];


extern int escapetomenu;
extern int currweaponflip;
extern int invincibletime;
extern int manatime;

int oldhoriz;
int mouselookmode, mouselook;
int pressedkey;
int testpaleffects=0;

char oldjoyb,
	 oldbstatus,
	 butbit[]={0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80};

short jcalibration=0,
	 jctrx,
	 jctry,
	 jlowx,jhighx,
	 jlowy,jhighy,
	 jmovespeed=16,
	 joyx,joyy,
	 jstickenabled=0,
	 jturnspeed=16;
int joykeys[4]={KEYFIRE,KEYUSE,KEYUSEP,KEYSTRAFE};

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

void autothehoriz(struct player *plr);
void dophysics(struct player *plr,int goalz,short flyupdn,int v);

void
initjstick(void)
{
	 jstickenabled=1;
}

void
getinput(void)
{
	int i;
	int mvel=0, msvel=0, mangvel=0, mhorizvel=0;
	int jvel=0, jsvel=0, jangvel=0, jhorizvel=0;

	horizvel=0;

	if(keystatus[keys[KEYSTRAFE]] == 0) {
		if(keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
			angvel-=16*TICSPERFRAME;
		}
		if(keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
			angvel+=16*TICSPERFRAME;
		}
	}
	else {
		if(keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
			svel+=8*TICSPERFRAME;
		}
		if(keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
			svel-=8*TICSPERFRAME;
		}
	}

	if (keystatus[keys[KEYSTFL]] != 0) {
		svel+=8*TICSPERFRAME;
	}
	if (keystatus[keys[KEYSTFR]] != 0) {
		svel-=8*TICSPERFRAME;
	}

	if(keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
		vel+=8*TICSPERFRAME;
	}
	if(keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
		vel-=8*TICSPERFRAME;
	}

	if(keystatus[keys[KEYLKDN]] > 0) {
		horizvel = -(TICSPERFRAME<<2);
	}
	if(keystatus[keys[KEYLKUP]] > 0) {
		horizvel = (TICSPERFRAME<<2);
	}

	vel = max(-201,min(201,vel));
	svel = max(-127,min(127,svel));
	angvel = max(-127,min(127,angvel));

	if (vel > 0) vel -= min(vel, 2*TICSPERFRAME);
	else if (vel < 0) vel -= max(vel, -2*TICSPERFRAME);
	if (svel > 0) svel -= min(svel, 2*TICSPERFRAME);
	else if (svel < 0) svel -= max(svel, -2*TICSPERFRAME);
	if (angvel > 0) angvel -= min(angvel, 12*TICSPERFRAME);
	else if (angvel < 0) angvel -= max(angvel, -12*TICSPERFRAME);

	if( option[3] & 1 ) {
		int mousx, mousy, bstatus;
		getmousevalues(&mousx,&mousy,&bstatus);
		if(keystatus[keys[KEYSTRAFE]]) msvel-=(mousx*mousxspeed);
		else mangvel+=(mousx*mousxspeed);                                 // Les 07/24/95

		if (mouselookmode) {
			if (keystatus[keys[KEYLOOKING]]) {
				mouselook ^= 1;
				if (mouselook) strcpy(displaybuf, "mouselook on");
				else strcpy(displaybuf, "mouselook off");
				displaytime=40;
				keystatus[keys[KEYLOOKING]]=0;
			}
		}
		else {
			mouselook = keystatus[keys[KEYLOOKING]] > 0;
		}
		if (mouselook) mhorizvel-=(mousy>>1);
		else mvel-=(mousy*mousyspeed);                            // Les 07/24/95
		for (i=0 ; i < 2 ; i++) {
			if (((bstatus&butbit[i]) == butbit[i])     // button is down
					&& ((oldbstatus&butbit[i]) != butbit[i])) {
				keystatus[keys[mousekeys[i]]]=1;
				if (mousekeys[i] == KEYRUN) {
					keystatus[keys[KEYFWD]]=1;
				}
			}
			else if (((bstatus&butbit[i]) == butbit[i])// button still down
					&& ((oldbstatus&butbit[i]) == butbit[i])) {
				if (mousekeys[i] == KEYUSE              // ..one-time actions
				   || mousekeys[i] == KEYJUMP
				   || mousekeys[i] == KEYMAP
				   || mousekeys[i] == KEYUSEP
				   || mousekeys[i] == KEYCAST) {
					keystatus[keys[mousekeys[i]]]=0;
				}
			}
			else if (((bstatus&butbit[i]) != butbit[i])// button released
					&& ((oldbstatus&butbit[i]) == butbit[i])) {
				keystatus[keys[mousekeys[i]]]=0;
				if (mousekeys[i] == KEYRUN) {
					keystatus[keys[KEYFWD]]=0;
				}
			}
		}
		oldbstatus=bstatus;
	}
	if (jstickenabled) {
		const int deadzone = 4096;
		if (joynumaxes == 2) {
			jangvel  = joyaxis[0];
			jvel     =-joyaxis[1];
		} else if (joynumaxes >= 4) {
			jangvel  = joyaxis[2];
			jvel     =-joyaxis[1];
			jsvel    =-joyaxis[0];
			jhorizvel=-joyaxis[3];
		}
		if (klabs(jvel) < deadzone) jvel=0;
		if (klabs(jsvel) < deadzone) jsvel=0;
		if (klabs(jangvel) < deadzone) jangvel=0;
		if (klabs(jhorizvel) < deadzone) jhorizvel=0;
		jvel = 201*(jvel-ksgn(jvel)*deadzone)/(32767-deadzone);
		jsvel = 127*(jsvel-ksgn(jsvel)*deadzone)/(32767-deadzone);
		jangvel = 127*(jangvel-ksgn(jangvel)*deadzone)/(32767-deadzone);
		jhorizvel = (YDIM/8)*(jhorizvel-ksgn(jhorizvel)*deadzone)/(32767-deadzone);

		for (i=0 ; i < 4 ; i++) {
			if (((joyb&butbit[i]) == butbit[i])          // button is down
					&& ((oldjoyb&butbit[i]) != butbit[i])) {
				keystatus[keys[joykeys[i]]]=1;
				if (joykeys[i] == KEYRUN) {
					keystatus[keys[KEYFWD]]=1;
				}
			}
			else if (((joyb&butbit[i]) == butbit[i])     // button still down
					&& ((oldjoyb&butbit[i]) == butbit[i])) {
				if (joykeys[i] == KEYUSE                // ..one-time actions
				   || joykeys[i] == KEYJUMP
				   || joykeys[i] == KEYMAP
				   || joykeys[i] == KEYUSEP
				   || joykeys[i] == KEYCAST) {
					keystatus[keys[joykeys[i]]]=0;
				}
			}
			else if (((joyb&butbit[i]) != butbit[i])     // button released
					&& ((oldjoyb&butbit[i]) == butbit[i])) {
				keystatus[keys[joykeys[i]]]=0;
				if (joykeys[i] == KEYRUN) {
					keystatus[keys[KEYFWD]]=0;
				}
			}
		  }
		  oldjoyb=joyb;
	}

	loc.vel = max(-201,min(201,vel+mvel+jvel));
	loc.svel = max(-127,min(127,svel+msvel+jsvel));
	loc.angvel = max(-127,min(127,angvel+mangvel+jangvel));
	loc.horizvel = max(-(YDIM>>1),min((YDIM>>1),horizvel+mhorizvel+jhorizvel));

	memset(&loc.keys, 0, sizeof(loc.keys));
	loc.keys.run = !!keystatus[keys[KEYRUN]];
	loc.keys.fire = !!keystatus[keys[KEYFIRE]];
	loc.keys.use = !!keystatus[keys[KEYUSE]]; keystatus[keys[KEYUSE]]=0;
	loc.keys.jump = !!keystatus[keys[KEYJUMP]]; keystatus[keys[KEYJUMP]]=0;
	loc.keys.crouch = !!keystatus[keys[KEYCROUCH]];
	loc.keys.cntr = !!keystatus[keys[KEYCNTR]]; keystatus[keys[KEYCNTR]]=0;
	loc.keys.usep = !!keystatus[keys[KEYUSEP]]; keystatus[keys[KEYUSEP]]=0;
	loc.keys.cast = !!keystatus[keys[KEYCAST]]; keystatus[keys[KEYCAST]]=0;
	loc.keys.uncast = !!keystatus[keys[KEYUNCAST]];
	loc.keys.flyup = !!keystatus[keys[KEYFLYUP]];
	loc.keys.flydn = !!keystatus[keys[KEYFLYDN]];
	loc.keys.prevp = !!keystatus[keys[KEYPREVP]]; keystatus[keys[KEYPREVP]]=0;
	loc.keys.nextp = !!keystatus[keys[KEYNEXTP]]; keystatus[keys[KEYNEXTP]]=0;

	loc.keys.weapon = 0;
	for (i=KEYWEAP1; i<=KEYWEAP10; i++) {
		if (keystatus[keys[i]]) {
			loc.keys.weapon = 1+i-KEYWEAP1;
			keystatus[keys[i]]=0;
		}
	}
	loc.keys.spell = 0;
	for (i=KEYSPELL1; i<=KEYSPELL8; i++) {
		if (keystatus[keys[i]]) {
			loc.keys.spell = 1+i-KEYSPELL1;
			keystatus[keys[i]]=0;
		}
	}
}

void
dophysics(struct player *plr,int goalz,short flyupdn,int v)
{
	 if (plr->orbactive[5] > 0) {
		  if (v > 0) {
			   if (plr->horiz > 125)
					plr->hvel-=(TICSPERFRAME<<6);
			   else if (plr->horiz < 75)
					plr->hvel+=(TICSPERFRAME<<6);
		  }
		  else {
			   if (flyupdn > 0) {
					plr->hvel-=(TICSPERFRAME<<7);
			   }
			   if (flyupdn < 0) {
					plr->hvel+=(TICSPERFRAME<<7);
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
			//JonoF: seems unwise sync-wise.
			plr->z=sector[plr->sector].floorz-(PLAYERHEIGHT<<8);
			setsprite(plr->spritenum,plr->x,plr->y,plr->z+(plr->height<<8));
			sprite[plr->spritenum].ang=plr->ang;
			//visibility=2048;
			visibility=1024;
		}
	}

	if( followmode == 1) {
		if(keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
			followx-=synctics<<5; //TODO
		}
		if(keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
			followx+=synctics<<5; //TODO
		}
		if(keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
			followy-=synctics<<5; //TODO
		}
		if(keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
			followy+=synctics<<5; //TODO
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


	if (keystatus[88]) {	//F12 - screenshot
		keystatus[88] = 0;
		screencapture("captxxxx.tga", 0);
	}
	if (keystatus[0x44] > 0) {  //F10 - brightness
		keystatus[0x44]=0;
		gbrightness=brightness++;
		if (brightness > 8) brightness=0, gbrightness=0;
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
				plr->zoom-=(synctics/TICSPERFRAME)*(plr->zoom>>4); //TODO
		if(keystatus[keys[KEYZOOMI]] > 0)                     // Les 07/24/95
			if(plr->zoom < 4096)
				plr->zoom+=(synctics/TICSPERFRAME)*(plr->zoom>>4); //TODO
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


	else if(plr->dimension == 3) {
		int didchange = 0;

		if (keystatus[keys[KEYZOOMO]] != 0 && plr->screensize > 64) { // Les 07/24/95
			plr->screensize-=(synctics/TICSPERFRAME)*8; //TODO
			didchange = 1;
		}
		if (keystatus[keys[KEYZOOMI]] != 0 && plr->screensize <= 320) { // Les 07/24/95
			plr->screensize+=(synctics/TICSPERFRAME)*8; //TODO
			didchange = 1;
		}

		if (didchange) {
			setviewport(plr->screensize);
			drawbackground();
		}
	}

}

void
syncprocessinput(short snum) {
	int goalz,hihit,hiz,lohit,loz,xvect,yvect;
	int tics;
	int oldposx, oldposy;
	int dist;
	int feetoffground;
	short onground,onsprite=-1;
	struct player *plr = &player[snum];

	short v=ssync[snum].vel,
	      s=ssync[snum].svel,
	      a=ssync[snum].angvel,
	      h=ssync[snum].horizvel;
	struct inputkeys k=ssync[snum].keys;

	tics=TICSPERFRAME;

	sprite[plr->spritenum].cstat &= ~1;
	getzrange(plr->x,plr->y,plr->z,plr->sector,&hiz,&hihit,&loz,&lohit,128L,CLIPMASK0);
	sprite[plr->spritenum].cstat |= 1;

	if ((lohit&0xc000) == 49152) {
		if( (sprite[lohit&4095].z-plr->z) <= (PLAYERHEIGHT<<8) )
			onsprite=(lohit&4095);
	}
	else
		onsprite=-1;

	feetoffground=(sector[plr->sector].floorz-plr->z);

	if(k.run || v > 201) {
		if(v > 201)
			v=201;

		v+=v>>1;// SUPER MARIO BROTHERS

		if(feetoffground > (32<<8))
			tics+=tics>>1;
	}

	if (h) {
		plr->horiz+=h;
		if(plr->horiz < 100-(YDIM>>1)) plr->horiz=100-(YDIM>>1);
		else if(plr->horiz > 100+(YDIM>>1)) plr->horiz=100+(YDIM>>1);
		autohoriz=0;
	}

	if(k.uncast)
		plr->orbactive[5]=-1;

	if (k.fire && hasshot == 0) {
		if(currweaponfired == 0)
			plrfireweapon(plr);
	}

	if (k.cast && orbshot == 0 && currweaponflip == 0) {
		if(plr->orb[currentorb] == 1 && selectedgun == 0) {
			if(lvlspellcheck(plr)) {
				orbshot=1;
				//JSA_NEW
				activatedaorb(plr);
			}
		}
		if(selectedgun != 0)
			k.weapon=1;
	}

	if (k.usep) {
		if(plr->potion[currentpotion] > 0) {
			usapotion(plr);
		}
	}

	if (k.use) {
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

	if (k.jump) {
		if (plr->onsomething) {
			 plr->hvel-=JUMPVEL;
			 plr->onsomething=0;
		}
	}

	if (k.crouch) {
		if(goalz<((sector[plr->sector].floorz)-(plr->height>>3)))
			goalz+=(24<<8);
	}

	onground=plr->onsomething;
	if (k.flyup) {
		dophysics(plr,goalz,1,v);
	}
	else if (k.flydn) {
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

		if (plr->sector!=plr->lastsndsector) {
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

		plr->lastsndsector=plr->sector;

		//walking on sprite
		plr->horiz-=oldhoriz;

		dist=ksqrt((plr->x-oldposx)*(plr->x-oldposx)+(plr->y-oldposy)*(plr->y-oldposy));

		if (k.run)
			dist>>=2;

		if( dist > 0 && feetoffground <= (plr->height<<8) || onsprite != -1) {
			if( svga == 1)
				 oldhoriz = ((dist*sintable[(lockclock<<5)&2047])>>19)>>2;
			else
				 oldhoriz = ((dist*sintable[(lockclock<<5)&2047])>>19)>>1;
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
				playsound_loc(S_RATS1+(rand()%2),sprite[onsprite].x,sprite[onsprite].y);
				justplayed=1;
				deletesprite((short)onsprite);
			break;
			case SPIDER:
				//STOMP
				playsound_loc(S_DEADSTEP,sprite[onsprite].x,sprite[onsprite].y);
				justplayed=1;
				newstatus(onsprite,DIE);
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


		if( (v > 199 || v < -199) && dist > 10)
			runningtime+=TICSPERFRAME;
		else
			runningtime-=TICSPERFRAME;

		if( runningtime < -360 )
			runningtime=0;

		if( runningtime > 360 ) {
			SND_PlaySound(S_PLRPAIN1,0,0,0,0);
			runningtime=0;
		}


	}
	if (a != 0) {
		plr->ang+=(a*(int)TICSPERFRAME)>>4;
		plr->ang=(plr->ang+2048)&2047;
	}

	setsprite(plr->spritenum,plr->x,plr->y,plr->z+(plr->height<<8));
	sprite[plr->spritenum].ang=plr->ang;

	if(sector[plr->sector].ceilingz > sector[plr->sector].floorz-(8<<8) )
		healthpic(-10);

	if(plr->health <= 0 ) {
		SND_CheckLoops();
		playerdead(plr);
	}

	if (k.cntr) {
		autohoriz=1;
	}
	if(autohoriz == 1)
		autothehoriz(plr);

	singleshot(0);

	weaponchange(plr, k.weapon, k.spell, k.nextp ? 1 : (k.prevp ? -1 : 0));

}

void autothehoriz(struct player *plr) {

	if(plr->horiz < 100)
		plr->horiz+=TICSPERFRAME<<1;
	if(plr->horiz > 100)
		plr->horiz-=TICSPERFRAME<<1;
	if(plr->horiz >= 90 && plr->horiz <= 110)
		autohoriz=0;
}


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

	ready2send = 0;
	while( !exit ) {
		handleevents();
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

		if( keystatus[0xe] ) {
			if (typemessageleng > 0) typemessageleng--;
			temp[typemessageleng]=0;
			keystatus[0xe]=0;
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
		displaytime=40;
		drawscreen(&player[pyrn],65535);
	}
	totalclock = ototalclock;
	ready2send = 1;

	if( exit == 2 ) {
		keystatus[0x1c]=keystatus[0x9c]=0;
	}

	 checkcheat();
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

  (void)ch;
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
