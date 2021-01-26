
#define  SND_CALLER
#define  GAME
#include "icorp.h"
#include "fx_man.h"

#define TRUE 1
#define FALSE 0

typedef struct
{
	int      handle;
	int      number;
	int      x,y;
} SampleType;

struct    soundbuffertype {
     int       users;
     int       offset;
     int       priority;
     void *    cache_ptr;
     int       cache_length;
     unsigned char cache_lock;
};

struct soundbuffertype Samples[MAX_SAMPLES];
SampleType  SampleRay[MAX_ACTIVE_SAMPLES];

SampleType  FXLoopRay[MAX_ACTIVE_FXLOOPS];

extern char tempbuf[];
extern char displaybuf[50];

extern int  displaytime;

//JSA SPOOGE
extern int musiclevel;
extern int digilevel;
extern int loopinstuff;

//TEMP!!
volatile  unsigned joetime;

static unsigned short songelements=3;
static unsigned short arrangements=3;
static unsigned short songsperlevel;
static unsigned short totallevels=6;              //really thre use two to test menusong

int lavasnd=-1,
	batsnd=-1,
	cartsnd=-1;

//JSA_DEMO move these to sndmod.h if they work
char *       BaseSongPtr;
char *       EmbSongPtr;
char *       SpiceSongPtr;

unsigned short     SD_Started=0;
int     Midi_Loaded,Digi_Loaded;

unsigned short     hSoundFile  =  -1;       // Handle for Sound F/X file
unsigned short     hSongFile   =  -1;
unsigned short     hMiscHandle =  -1;


unsigned int    *SongList;

//
//                         INTERNAL ROUTINES
//

static void
soundcallback(unsigned int i)
{
     if (i == (unsigned int)-1) return;

     Samples[SampleRay[i].number].users--;
     if( Samples[SampleRay[i].number].users == 0 ) {
          Samples[SampleRay[i].number].cache_lock=0x00;
     }
     SampleRay[i].handle=-1;
     SampleRay[i].number=-1;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    sosMIDISongCallback(WORD) : Call back routine for SOS                  //
//          Midi function InitSong Struct. hSong indicates which                 //
//              song is complete (handle from sosMIDIInitSong).                      //
//          NEEDS WORK FOR JUGGLING AND MIXING SONGS!!!!!!!                 //
//////////////////////////////////////////////////////////////////////////////
void
sosMIDISongCallback( unsigned short hSong )
{
/*
	for(wIndex=0; wIndex<MAX_ACTIVE_SONGS; wIndex++)
		if(hSong == hSOSSongHandles[wIndex])
			break;

	//sosMIDIStopSong(hSOSSongHandles[wIndex]);
	sosMIDIUnInitSong(hSOSSongHandles[wIndex]);
	hSOSSongHandles[wIndex] = 0x7fff;
 */
}



void
sosMIDITriggerCallback( unsigned short hSong, unsigned char bTrack, unsigned char bID )
{
    /*
	if(SongPending) {
		SND_StartMIDISong(SongPending);
		SongPending=0;
	}
     */
}


void
SND_SetupTables(void)
{
	int i;
	unsigned int    DigiList[1024];

	if(SoundMode) {
		hSoundFile = kopen4load("joesnd", 0);
		if( hSoundFile < 0 ) {
			crashgame("Couldn't open JOESND");
		}
		klseek(hSoundFile,-4096L,SEEK_END);
		i = kread(hSoundFile,DigiList,sizeof(DigiList));
		if (i != sizeof(DigiList)) {
			crashgame("JOESND is too short");
		}
	}

	memset(&Samples, 0, sizeof(Samples));
	for (i=0; i<MAX_SAMPLES; i++) {
		Samples[i].offset = DigiList[i*3]*4096;
		Samples[i].cache_length = DigiList[i*3+1];
		Samples[i].priority = DigiList[i*3+2];
	}

/*
	if(MusicMode) {
		hSongFile = open("SONGS",O_RDONLY | O_BINARY);
		if( hSongFile == -1 ) {
			crash("COULDN'T OPEN SONGS!");
		}
		SongList = malloc(0x1000);
		lseek(hSongFile,-4096L,SEEK_END);
		readfile(hSongFile,SongList,4096);
	}
*/
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    SND_LoadMIDISong(WORD) : Handle near/far pointer krap for music file. //
//                                  (LPSTR = BYTE far *)                                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
char * SND_LoadMIDISong( unsigned short which )
{
	/*
char * lpDataPtr;
char *  pDataPtr;
unsigned short  wLength;


   wLength  = (unsigned short)SongList[(which * 3) + 1];
	SeekIndex = ( SongList[(which * 3) + 0] * 4096 );
   pDataPtr =  malloc( wLength );

	lseek(hSongFile,SeekIndex,0x00);

   read( hSongFile, pDataPtr, wLength );

   // create far pointer  !!!!!
   lpDataPtr   =  (char *)pDataPtr;
   return( lpDataPtr );
   */
	return NULL;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       SND_Startup() : Initialize all SOS Drivers and start timer         //
//             service.                                                     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
void
SND_Startup(void)
{
    int err, channels = 2, bitspersample = 16, mixrate = 22050;
    void *initdata = NULL;
    int wIndex;

	if (SD_Started)
		return;
/*
//GET Volume values
		wMIDIVol = (musiclevel<<3);
		wDIGIVol = (digilevel<<11);

//MIDISYSTEM
	if(MusicMode==_STANDARD_MUSIC) {
		if( wError = sosMIDIInitSystem( _NULL, _SOS_DEBUG_NORMAL )) {
			sosTIMERUnInitSystem( 0 );
			sosDIGIUnInitSystem();
			crash( "Could not Init Midi System!" );
		}
	}

//MIDIDRIVER
	if(MusicMode==_STANDARD_MUSIC) {
		sSOSMIDIInitDriver.lpDriverMemory  = _NULL;

		if( ( wError = sosMIDIInitDriver( wMIDIDeviceID, &sMIDIHardware,
				&sSOSMIDIInitDriver, &hSOSDriverHandles[MIDI] ) ) ) {
			sosMIDIUnInitSystem();
			sosDIGIUnInitSystem();
			sosTIMERUnInitSystem( 0 );
			crash( "Could not Init Midi Driver!\n" );
		}

		SND_LoadMidiIns();
		sosMIDIEnableChannelStealing(_FALSE);
		for(wIndex=0; wIndex<MAX_ACTIVE_SONGS; wIndex++)
			hSOSSongHandles[wIndex] = 0x7fff;

		songsperlevel=songelements*arrangements;

		Midi_Loaded = _TRUE;
		sosMIDISetMasterVolume(wMIDIVol);
   }

*/
	if(SoundMode) {
		#ifdef _WIN32
		initdata = (void *) win_gethwnd();
		#endif

		err = FX_Init(ASS_AutoDetect, MAX_ACTIVE_SAMPLES, &channels, &bitspersample, &mixrate, initdata);
		if (err != FX_Ok) {
			buildprintf("FX_Init error: %s\n", FX_ErrorString(err));
			return;
		}

		Digi_Loaded = TRUE;

	    FX_SetCallBack(soundcallback);
		// sosDIGISetMasterVolume(hSOSDriverHandles[DIGI],wDIGIVol);

		for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ ) {
			SampleRay[wIndex].handle = -1;
			SampleRay[wIndex].number = -1;
		}
	}

	// read in offset page list's
	SND_SetupTables();
	SD_Started = 1;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       SND_Shutdown() : Un-Initialize all SOS Drivers and releases        //
//             timer service(s).                                            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
void
SND_Shutdown(void)
{

	if (!SD_Started)
		return;

	if( SoundMode && Digi_Loaded )
	{
		SND_DIGIFlush();
		FX_Shutdown();
		Digi_Loaded=FALSE;
	}

	if( hSoundFile >= 0 ) {
		kclose(hSoundFile);
		hSoundFile = -1;
	}
	if( hSongFile >= 0 ) {
		kclose(hSongFile);
		hSongFile = -1;
	}
/*
	if( MusicMode && Midi_Loaded )
	{
		SND_MIDIFlush();
		sosMIDIUnInitDriver( hSOSDriverHandles[MIDI], _TRUE );
		if( m_bnkptr!=NULL )
			free( m_bnkptr );
		if( d_bnkptr!=NULL )
			free( d_bnkptr );
		if( digi_bnkptr!=NULL )
			free( digi_bnkptr );
		if( lpMIDISong!=NULL )
			free( lpMIDISong );
		Midi_Loaded=_FALSE;
	}
*/
	SD_Started = 0;

}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       SND_Mixer(WORD wSource,WORD wVolume) : Change Music or SFX Volume  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
void
SND_Mixer( unsigned short wSource, unsigned short wVolume )
{
/*
	if(wSource==MIDI) {
		wMIDIVol = (wVolume<<3);
		sosMIDISetMasterVolume((BYTE)wMIDIVol);
	}

	else {
		wDIGIVol = (wVolume<<11);
		sosDIGISetMasterVolume(hSOSDriverHandles[DIGI],wDIGIVol);
	}
 */
}


void
SND_MenuMusic(int choose)
{
/*
	if(!MusicMode || !SD_Started)
		return;

	if( (choose== DEATHSONG) && (wMIDIDeviceID==_MIDI_FM ))
		return;

	SND_SongFlush();

	if(choose==MENUSONG) {
		if(wMIDIDeviceID==_MIDI_MPU_401 || wMIDIDeviceID==_MIDI_AWE32 || wMIDIDeviceID==_MIDI_GUS)
			BaseSongPtr = SND_LoadMIDISong((totallevels*songsperlevel)+BASE_SONG+2);
		else
			BaseSongPtr = SND_LoadMIDISong((totallevels*songsperlevel)+BASE_SONG);
	}
	else if(wMIDIDeviceID==_MIDI_MPU_401 || wMIDIDeviceID==_MIDI_AWE32 || wMIDIDeviceID==_MIDI_GUS)
			BaseSongPtr = SND_LoadMIDISong((totallevels*songsperlevel)+3+BASE_SONG+2);

	SongPending = SND_PrepareMIDISong(BASE_SONG);
	SND_StartMIDISong(SongPending);
	SongPending=0;
*/
}

//
//  SND_StartMusic() will only be called at the beginning of a new level
//      will be used in all other cases.

void
SND_StartMusic(unsigned short level)
{
/*
	if((!MusicMode) || !SD_Started)
		return;

	if(level > 5)
		level=rand()%6;

	{
		SND_SongFlush();
		SND_LoadSongs(level);
		SongPending = SND_PrepareMIDISong(BASE_SONG);
		SND_StartMIDISong(SongPending);
		SongPending=0;
	}
*/
}


void
SND_LoadSongs(unsigned short which)
{
static int index;
/*
			index=songsperlevel*which;                  //vanilla


			//if digi_midi used skip to those songs
			if(wMIDIDeviceID==_MIDI_AWE32)
				index+=songelements;                            //skip past vanilla

			//if soundcanvas skip to those songs
			if(wMIDIDeviceID==_MIDI_MPU_401 || wMIDIDeviceID==_MIDI_GUS)
				index+=songelements*2;

			BaseSongPtr = SND_LoadMIDISong(index+BASE_SONG);
			EmbSongPtr = SND_LoadMIDISong(index+EMB_SONG);
			SpiceSongPtr = SND_LoadMIDISong(index+SPICE_SONG);
 */
}


//
//  SND_PrepareMIDISong() loads a particular song into memory, initializes
//      it with SOS and puts it's handle into the hSOSSongHandles array.
//      The song can then be started by passing the handle to SND_StartMIDISong()
//
int
SND_PrepareMIDISong(int SongIndex)
{
    /*
   if(!MusicMode)
	  return(0x7fff);

	if(hSOSSongHandles[SongIndex] != 0x7fff)
		return(0x7fff);

	if(SongIndex==BASE_SONG)
		sSOSInitSongs[SongIndex].lpSongData = BaseSongPtr;
	if(SongIndex==EMB_SONG)
		sSOSInitSongs[SongIndex].lpSongData = EmbSongPtr;
	if(SongIndex==SPICE_SONG)
		sSOSInitSongs[SongIndex].lpSongData = SpiceSongPtr;


	sSOSInitSongs[SongIndex].lpSongCallback = (void(*))sosMIDISongCallback;
	if( ( wError = sosMIDIInitSong( &sSOSInitSongs[SongIndex], &sSOSTrackMap[SongIndex], &hSOSSongHandles[SongIndex] ) ) )
	{
			crash("Init Song Failed!");
	}

	return((int)hSOSSongHandles[SongIndex]);
*/
    return 0x7fff;
}

int
SND_StartMIDISong(unsigned short wSongHandle)
{
    /*
	sosMIDISetMasterVolume((BYTE)wMIDIVol);
	return(sosMIDIStartSong(wSongHandle));
     */
    return 0x7fff;
}


void
SND_StopMIDISong(unsigned short wSongHandle)
{
    /*
	for(wIndex=0; wIndex<MAX_ACTIVE_SONGS; wIndex++)
		if(hSOSSongHandles[wIndex]==wSongHandle)
			break;

	if(wIndex>=MAX_ACTIVE_SONGS)
		return;

	if( !sosMIDISongDone(hSOSSongHandles[wIndex]) )
	{
		sosMIDIStopSong(hSOSSongHandles[wIndex]);
		sosMIDIUnInitSong( hSOSSongHandles[wIndex] );
		hSOSSongHandles[wIndex] = 0x7fff;
		free(sSOSInitSongs[wIndex].lpSongData);
	}
*/
}


void
SND_SongFlush(void)
{
    /*
	if(!MusicMode)
		return;

	if(hSOSSongHandles[BASE_SONG] != 0x7fff) {
		SND_StopMIDISong(hSOSSongHandles[BASE_SONG]);
	}
	if(hSOSSongHandles[EMB_SONG] != 0x7fff)
		SND_StopMIDISong(hSOSSongHandles[EMB_SONG]);
	if(hSOSSongHandles[SPICE_SONG] != 0x7fff)
		SND_StopMIDISong(hSOSSongHandles[SPICE_SONG]);
*/
}


void
SND_FadeMusic(void)
{
	if(!MusicMode)
		return;
//  sosMIDIFadeSong(hSOSSongHandles[BASE_SONG],_SOS_MIDI_FADE_OUT,200,(BYTE)wMIDIVol,(BYTE)0,10);
}

void
SND_MIDIFlush(void)
{
/*
	for(wIndex=0; wIndex<MAX_ACTIVE_SONGS; wIndex++) {
		if( !sosMIDISongDone(hSOSSongHandles[wIndex]) )
			sosMIDIStopSong(hSOSSongHandles[wIndex]);
		if(hSOSSongHandles[wIndex] != 0x7fff)
			sosMIDIUnInitSong( hSOSSongHandles[wIndex] );
		hSOSSongHandles[wIndex] = 0x7fff;
	}

	free(BaseSongPtr);
	free(EmbSongPtr);
	free(SpiceSongPtr);
*/
}



void
SND_Sting(unsigned short sound)
{
//make sure a sting sound is never 0
	if(!SoundMode)
		return;

	SND_PlaySound(sound,0L,0L,0,0);
}


int
SND_PlaySound(unsigned short sound, int x,int y, unsigned short Pan,unsigned short loopcount)
{
	unsigned short  wVol,flag=0;
	int  sqrdist, dx, dy, nr, wIndex;

	if(!SoundMode)
		return(-1);

	if (Samples[sound].cache_length <= 0)
		return(-1);

	if( ( (x==0)&&(y==0) ) || ( (player[pyrn].x==x)&&(player[pyrn].y == y) ) ) {
		sqrdist = 0;
		Pan=0;
	}
	else {
		/*
		sqrdist = klabs(player[pyrn].x-x)+klabs(player[pyrn].y-y);
		if(sqrdist < 1500)
			wVol = 0x7fff;
		else if(sqrdist > 8500)
			wVol = 0x1f00;
		else
			wVol = 39000-(sqrdist<<2);
		*/
		dx = klabs(player[pyrn].x-x);
		dy = klabs(player[pyrn].y-y);
		sqrdist = ksqrt(dx*dx+dy*dy) >> 6;

		if(Pan) {
			Pan = 2048 + player[pyrn].ang - getangle(player[pyrn].x-x,player[pyrn].y-y);
			Pan = (Pan & 2047) >> 6;
		}
	}


	if(sound==S_STONELOOP1) {
		for( wIndex=0,flag=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
			if(sound==SampleRay[wIndex].number)
				return(-1);
	}

	for( wIndex=0,flag=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
		if( SampleRay[wIndex].handle >= 0 && !FX_SoundActive(SampleRay[wIndex].handle) ) {
			flag=1;
			break;
		}

	if(!flag && Samples[sound].priority<9)               //none available low prio
		return(-1);

	else if(!flag) {                        //none avail but high prio
		for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
		{
			if(Samples[SampleRay[wIndex].number].priority<9 /*&& sSOSSampleData[wIndex].wLoopCount !=-1*/) {
				if(FX_SoundActive(SampleRay[wIndex].handle))
				{
					FX_StopSound(SampleRay[wIndex].handle);
					Samples[SampleRay[wIndex].number].users--;
					if (Samples[SampleRay[wIndex].number].users == 0) {
						Samples[SampleRay[wIndex].number].cache_lock = 0;
					}
					// sSOSSampleData[wIndex].wLoopCount = 0;
					SampleRay[wIndex].handle = -1;
					SampleRay[wIndex].number = -1;
					break;
				}
			}
		}
	}

	Samples[sound].cache_lock = 1;

	if (Samples[sound].cache_ptr == NULL) {
		allocache(&Samples[sound].cache_ptr, Samples[sound].cache_length, &Samples[sound].cache_lock);
		if (Samples[sound].cache_ptr == 0) {
			Samples[sound].cache_lock = 0;
			return -1;
		}
		klseek(hSoundFile,Samples[sound].offset,SEEK_SET);
		nr = kread(hSoundFile,Samples[sound].cache_ptr,Samples[sound].cache_length);
		if (nr != Samples[sound].cache_length) {
			Samples[sound].cache_ptr = NULL;
			Samples[sound].cache_lock = 0;
			return -1;
		}
	}
	// if(loopcount)
	// 	sSOSSampleData[wIndex].wLoopCount = loopcount;

	SampleRay[wIndex].handle = FX_PlayRaw3D(Samples[sound].cache_ptr, Samples[sound].cache_length,
		11025, 0, Pan, sqrdist, 1, wIndex);
	if (SampleRay[wIndex].handle < 0) {
		if (Samples[sound].users == 0)
			Samples[sound].cache_ptr = NULL;
			Samples[sound].cache_lock = 0;
	} else {
		Samples[sound].users++;
		SampleRay[wIndex].x = x;
		SampleRay[wIndex].y = y;
		SampleRay[wIndex].number = sound;
	}
	return(wIndex);
}


int
SND_Sound(unsigned short sound )
{
	if(!SoundMode)
		return(-1);
	return(SND_PlaySound(sound,0,0,0,0));

}

void
SND_CheckLoops(void)
{
	int wIndex;

	//special case loops
	if( cartsnd >= 0) {
		SND_StopLoop(cartsnd);
		cartsnd=-1;
	}
	if( lavasnd >= 0 ) {
		SND_StopLoop(lavasnd);
		lavasnd=-1;
	}
	if( batsnd >= 0 ) {
		SND_StopLoop(batsnd);
		batsnd=-1;
	}

	//ambient sound array
	for(wIndex=0;wIndex<MAX_AMB_SOUNDS;wIndex++)
	{
		if(ambsoundarray[wIndex].hsound != -1) {
			SND_StopLoop(ambsoundarray[wIndex].hsound);
			ambsoundarray[wIndex].hsound=-1;
		}
	}

	SND_DIGIFlush();

}

void
SND_StopLoop(int which)
{
	int wIndex;

	if(!SoundMode)
		return;

	for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
		if(which==SampleRay[wIndex].handle)
			break;

	FX_StopSound(SampleRay[wIndex].handle);
	Samples[SampleRay[wIndex].number].users--;
	if (Samples[SampleRay[wIndex].number].users == 0) {
		Samples[SampleRay[wIndex].number].cache_lock = 0;
	}
	// sSOSSampleData[wIndex].wLoopCount = 0;
	SampleRay[wIndex].handle = -1;
	SampleRay[wIndex].number = -1;
}


void
SND_DIGIFlush(void)
{
	int wIndex;

	if(!SoundMode)
		return;

	for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
	{
		if( SampleRay[wIndex].handle < 0)
			continue;
		FX_StopSound(SampleRay[wIndex].handle);
		Samples[SampleRay[wIndex].number].users--;
		if (Samples[SampleRay[wIndex].number].users == 0) {
			Samples[SampleRay[wIndex].number].cache_lock = 0;
		}
		SampleRay[wIndex].handle = -1;
		SampleRay[wIndex].number = -1;
	}
}



//
//    Sound Location Stuff
//

void
dolevelmusic(int level)
{
}

// JOE START functions called often from external modules

//JSA 3/20/95
//
//    note: soundnum=sound to play; xplc is x location of source; yplc is y loc
//

void
playsound_loc(int soundnum, int xplc, int yplc)
{
	SND_PlaySound(soundnum,xplc,yplc,1,0);
}

void
updatesound_loc(void)
{
	int wIndex;
	unsigned wVol,wPan;
	int sqrdist, dx, dy;

	if(!SoundMode)
		return;

	for(wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++)
		if( SampleRay[wIndex].handle >= 0 && FX_SoundActive(SampleRay[wIndex].handle) &&
			SampleRay[wIndex].x && SampleRay[wIndex].y)
		{
			/*
			sqrdist = labs(player[pyrn].x-SampleRay[wIndex].x) +
					  labs(player[pyrn].y-SampleRay[wIndex].y);

			if(sqrdist < 1500)
				wVol = 0x7fff;
			else if(sqrdist > 8500)
				wVol = 0x1f00;
			else
				wVol = 39000-(sqrdist<<2);

			wPan=((getangle(player[pyrn].x-SampleRay[wIndex].x,player[pyrn].y-SampleRay[wIndex].y)+(2047-player[pyrn].ang))%2047) >> 6;
			*/

			dx = klabs(player[pyrn].x-SampleRay[wIndex].x);
			dy = klabs(player[pyrn].y-SampleRay[wIndex].y);
			sqrdist = ksqrt(dx*dx+dy*dy) >> 6;

			wPan = 2048 + player[pyrn].ang - getangle(player[pyrn].x-SampleRay[wIndex].x,player[pyrn].y-SampleRay[wIndex].y);
			wPan = (wPan & 2047) >> 6;

			FX_Pan3D(SampleRay[wIndex].handle, wPan, sqrdist);

			 //sprintf(displaybuf,"%dVol %x Pan %x Dist %ld",SampleRay[wIndex].number,wVol,wPan,sqrdist);
			 //displaytime=100;

		}
}

// Location Stuff End
