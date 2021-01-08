
#define  SND_CALLER
#define  GAME
#include "icorp.h"

int   readfile(int,char *,int);

int flag=0;

extern char tempbuf[];
extern char displaybuf[50];

extern int  displaytime;

//JSA SPOOGE
extern int musiclevel;
extern int digilevel;
extern int loopinstuff;

// local data for hmi INI
static   char *  szHexNumbers  =  "0123456789ABCDEF";
static   unsigned short  wMultiplier[]   =  { 1, 16, 256, 4096, 65536, 1048576, 16777216, 268435456 };
// local function prototypes

//TEMP!!
volatile  unsigned joetime;

static unsigned short songelements=3;
static unsigned short arrangements=3;
static unsigned short songsperlevel;
static unsigned short totallevels=6;              //really thre use two to test menusong

//JSA_DEMO move these to sndmod.h if they work
char *       BaseSongPtr;
char *       EmbSongPtr;
char *       SpiceSongPtr;


//
//                         INTERNAL ROUTINES
//

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    timerevent() : Handles timer functions originally in game.c           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
void
timerevent(void)
{
   joetime++;
	totalclock++;
	keytimerstuff();
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    sosDIGISampleCallback(WORD,WORD,WORD) : Call back routine from SOS    //
//          Digi functions. wCallSource indicates which process is complete //
//          hSample is the particular sample handle.                        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
void
sosDIGISampleCallback(  unsigned short wDriverHandle, unsigned short wCallSource, unsigned short hSample )
{
//
// recording driver will also use this callback for future!
// will have to switch wDriverHandle as well...
//
/*
	switch(wCallSource)
	{
		case _SAMPLE_DONE:
			for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
				if( hSample==SampleRay[wIndex].SOSHandle )
					break;
#if 0
//a sting has just played so restart the loopmusic
			if(SampleRay[wIndex].number == loopmusepauseflag && MusicMode==_LOOP_MUSIC)
				sosDIGISetSampleVolume(hSOSDriverHandles[DIGI],LoopHandles[(looptoggle^1)],0x4fff);
#endif
			sSOSSampleData[wIndex].wLoopCount = 0;
			sSOSSampleData[wIndex].dwSampleSize= 0;
			SampleRay[wIndex].SOSHandle = -1;
			SampleRay[wIndex].playing = 0;
			SampleRay[wIndex].priority = 0;
			SampleRay[wIndex].number = -1;
			break;

#if 0
		case _SAMPLE_LOOPING:
//will have to differentiate between a music and sfx loop via handle
			if(LoopPending)
			{
				SND_SwapLoops();
				LoopPending = 0;
			}
			//Metronome = 0;
			break;
#endif
	}
*/
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


  if(BranchPending) {
	  sosMIDIBranchToSongLocation(hSOSSongHandles[BASE_SONG],0);
	  BranchPending=0;
  }
     */
}


void
SND_SetupTables(void)
{
/*
	if(SoundMode) {
		hSoundFile = open("JOESND",O_RDONLY | O_BINARY);
		if( hSoundFile == -1 ) {
			crash("COULDN'T OPEN JOESND!");
		}
		DigiList = malloc(0x1000);
		lseek(hSoundFile,-4096L,SEEK_END);
		readfile(hSoundFile,DigiList,4096);
	}

	if(MusicMode==_LOOP_MUSIC) {
		hLoopFile = open("LOOPS",O_RDONLY | O_BINARY);
		if( hLoopFile == -1 ) {
			crash("COULDN'T OPEN LOOPS!");
		}
		LoopList = malloc(0x1000);
		lseek(hLoopFile,-4096L,SEEK_END);
		readfile(hLoopFile,LoopList,4096);
	}
	else if(MusicMode) {
		hSongFile = open("SONGS",O_RDONLY | O_BINARY);
		if( hSongFile == -1 ) {
			crash("COULDN'T OPEN SONGS!");
		}
		SongList = malloc(0x1000);
		lseek(hSongFile,-4096L,SEEK_END);
		readfile(hSongFile,SongList,4096);
	}
*/
	return;
}



//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    SND_LoadMidiIns(void) : Read melodic and percussive banks into mem.   //
//          Conditional : GENERAL MIDI and WAVE synths don't need patches   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
void
SND_LoadMidiIns(void)
{
static unsigned short  wLength;

//JSA_DEMO check port address to verify FM device
/*
	if( (MusicMode==_STANDARD_MUSIC || MusicMode==_DIG_MIDI_MUSIC) &&
			sMIDIHardware.wPort==0x388 ) {
//              (wMIDIDeviceID != _MIDI_MPU_401 && wMIDIDeviceID != _MIDI_MT_32
//                  && wMIDIDeviceID!=_MIDI_AWE32) ) {
		hMiscHandle = open("melodic.bnk",O_RDONLY);
		if( hMiscHandle == -1 )
			crash("MELODIC BANK FILE FAILED!");
		m_bnkptr = malloc(0x152c);
		read( hMiscHandle,m_bnkptr,0x152c );
		close( hMiscHandle );
		if( ( wError = sosMIDISetInsData( hSOSDriverHandles[MIDI], m_bnkptr, 0x00 ) ) )
			crash("BAD SetInsData MEL!" );

		hMiscHandle = open("drum.bnk",O_RDONLY);
		if( hMiscHandle == -1 )
			crash("PERCUSSIVE BANK FILE FAILED!");
		d_bnkptr = malloc(0x152c);
		read( hMiscHandle,d_bnkptr,0x152c );
		close( hMiscHandle );
		if( ( wError = sosMIDISetInsData( hSOSDriverHandles[MIDI], d_bnkptr, 0x00 ) ) )
			printf("BAD SetInsData DRUM!" );
	}

	if(MusicMode==_DIG_MIDI_MUSIC) {
		hMiscHandle = open("test.dig",O_RDONLY);
		if( hMiscHandle == -1 )
			crash("DIGI_MIDI FILE FAILED!");
	wLength  =  lseek( hMiscHandle, 0L, SEEK_END );
	lseek( hMiscHandle, 0L, SEEK_SET );
		digi_bnkptr = malloc(wLength);
		read( hMiscHandle,digi_bnkptr,wLength );
		close( hMiscHandle );
		if( ( wError = sosMIDISetInsData( hSOSDriverHandles[DIG_MIDI], digi_bnkptr, 0x00 ) ) )
			crash("BAD SetInsData digmidi!" );
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
}


//
//                         PUBLIC ROUTINES
//


void
SND_DoBuffers(void)
{

/*
	for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ ) {
		sSOSSampleData[wIndex].lpSamplePtr = (char *)malloc((int)55000);
		if(sSOSSampleData[wIndex].lpSamplePtr==_NULL)
			crash("Could Not get a Sound Buffer!!");
	}
*/
}

void
SND_UnDoBuffers(void)
{
/*
	for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ ) {
		if( sSOSSampleData[wIndex].lpSamplePtr != NULL )
			free(sSOSSampleData[wIndex].lpSamplePtr);
	}
*/
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

	if (SD_Started)
		return;
/*
//GET Volume values
		wMIDIVol = (musiclevel<<3);
		wDIGIVol = (digilevel<<11);

	if(wDIGIDeviceID == 0xffffffff)
		SoundMode=_OFF;
	if(wMIDIDeviceID == 0xffffffff)
		MusicMode=_OFF;

//DIGISYSTEM

	if(SoundMode==_ON) { // && wDIGIDeviceID!=0xffffffff) {
		wError = sosDIGIInitSystem( _NULL, _SOS_DEBUG_NORMAL );
		if(wError!=_ERR_NO_ERROR)
			crash( "Error on Digi SysInit!" );
	}

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


//DIGIDRIVER
	if(SoundMode==_ON) { // && wDIGIDeviceID!=0xffffffff) {

		if( ( wError = sosDIGIInitDriver( wDIGIDeviceID,&sDIGIHardware,
				 &sSOSInitDriver, &hSOSDriverHandles[DIGI] ) ) ) {
			sosTIMERUnInitSystem( 0 );
			sosDIGIUnInitSystem();
			crash( "Could not initialize Digi Driver!" );
		}
		if( ( wError = sosTIMERRegisterEvent( _SOS_FILL_TIMER_RATE, sSOSInitDriver.lpFillHandler,
			&hTimerDig_FillHandle ) ) ) {
			sosDIGIUnInitDriver( hSOSDriverHandles[DIGI], _TRUE, _TRUE );
			sosTIMERUnInitSystem( 0 );
			sosDIGIUnInitSystem();
			crash( "Could not register lpFillHandler!" );
		}
		Digi_Loaded = _TRUE;

		sosDIGISetMasterVolume(hSOSDriverHandles[DIGI],wDIGIVol);

		for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
			SampleRay[wIndex].number = -1;
	}
*/
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
/*
	if( SoundMode && Digi_Loaded )
	{
	  SND_DIGIFlush();

		sosTIMERRemoveEvent( hTimerDig_FillHandle );
		flag = sosDIGIUnInitDriver( hSOSDriverHandles[DIGI], _TRUE, _TRUE );
		if( hSoundFile != -1 )
			close(hSoundFile);
		if( hLoopFile != -1 )
			close( hLoopFile );
		if( hSongFile != -1 )
			close( hSongFile );
		if( DigiList!=NULL )
			free( DigiList );
		if( LoopList!=NULL )
			free( LoopList );
		Digi_Loaded=_FALSE;
	}

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

	sosMIDIUnInitSystem();

	sosDIGIUnInitSystem();
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

	if(MusicMode==_LOOP_MUSIC) {
		  SND_LoadLoop(0);
		  LoopPending=1;
	}

	else {
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

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    SND_LoadLoop(void) : Load and start digiloop - use looppending        //
//          in the callback to cue.                                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
void
SND_LoadLoop(unsigned short load_start)
{
/*
	if(!SoundMode)
		return;

	SeekIndex = ( LoopList[(LoopIndex * 3)+0] * 4096 );
	LoopSampleData[looptoggle].dwSampleSize= (unsigned short)LoopList[(LoopIndex * 3) + 1];


	lseek(hLoopFile,SeekIndex,0x00);

   if(!load_start) {
	readfile(hLoopFile,LoopSampleData[looptoggle].lpSamplePtr,
			LoopSampleData[looptoggle].dwSampleSize);
   }

	else {
	LoopSampleData[0].lpSamplePtr = (char *)malloc( 40000 );
	LoopSampleData[1].lpSamplePtr = (char *)malloc( 40000 );
	readfile(hLoopFile,LoopSampleData[looptoggle].lpSamplePtr,
			LoopSampleData[looptoggle].dwSampleSize);

		LoopHandles[looptoggle] = sosDIGIStartSample(hSOSDriverHandles[DIGI],&LoopSampleData[looptoggle] );
	looptoggle^=1;
	}

	LoopIndex++;
	if(LoopIndex>MAX_SND_LOOPS-1)
		LoopIndex = 0;
*/
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    SND_LoadLoop(VOID) : Load and start digiloop - use looppending        //
//          in the callback to cue.                                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
void
SND_SwapLoops(void)
{
int temp;
/*
	if(!SoundMode)
		return;

	temp=looptoggle^1;

	if( !sosDIGISampleDone(hSOSDriverHandles[DIGI], LoopHandles[temp]) )
	{
		sosDIGIStopSample( hSOSDriverHandles[DIGI], LoopHandles[temp]);
//      free(LoopSampleData[temp].lpSamplePtr);
		LoopHandles[looptoggle] = sosDIGIStartSample(hSOSDriverHandles[DIGI],&LoopSampleData[looptoggle] );
	}

	looptoggle^=1;
*/
}


void
SND_Sting(unsigned short sound)
{
//make sure a sting sound is never 0
int temp;

    /*
	if(!SoundMode)
		return;

//  if(wMIDIDeviceID==_MIDI_MPU_401 || wMIDIDeviceID==_MIDI_MT_32
//      || wMIDIDeviceID==_MIDI_AWE32)
//      return;

   if(MusicMode==_LOOP_MUSIC) {

		temp=looptoggle^1;
		//mute the loop - play the sound set variable for callback
		sosDIGISetSampleVolume(hSOSDriverHandles[DIGI],LoopHandles[temp],0);
		loopmusepauseflag=sound;
	}
	SND_PlaySound(sound,0L,0L,0,0);
 */
}


//unsigned short
int
SND_PlaySound(unsigned short sound, int x,int y, unsigned short Pan,unsigned short loopcount)
{
unsigned short  wVol,flag=0;
int  sqrdist;
int  prioritize;

    /*
	if(!SoundMode)
		return(0);
		//return((unsigned short)0);


	prioritize = DigiList[(sound * 3) + 2];

	if( ( (x==0)&&(y==0) ) || ( (player[pyrn].x==x)&&(player[pyrn].y == y) ) ) {
		wVol = 0x7fff;
		Pan=0;
	}
	else {
		sqrdist = labs(player[pyrn].x-x)+labs(player[pyrn].y-y);
		if(sqrdist < 1500)
			wVol = 0x7fff;
		else if(sqrdist > 8500)
			wVol = 0x1f00;
		else
			wVol = 39000-(sqrdist<<2);
	}


	if(sound==S_STONELOOP1) {
		for( wIndex=0,flag=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
			if(sound==SampleRay[wIndex].number)
				return(0);
		}

	for( wIndex=0,flag=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
		if( !SampleRay[wIndex].playing ) {
			flag=1;
			break;
		}

	if(!flag && prioritize<9)               //none available low prio
		return(0);

	else if(!flag) {                        //none avail but high prio
		for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
		{
			if(SampleRay[wIndex].priority<9 && sSOSSampleData[wIndex].wLoopCount !=-1) {
				if(!sosDIGISampleDone(hSOSDriverHandles[DIGI],SampleRay[wIndex].SOSHandle) &&
				(sSOSSampleData[wIndex].dwSampleSize!=0) )
				{
					sosDIGIStopSample(hSOSDriverHandles[DIGI],SampleRay[wIndex].SOSHandle );
					sSOSSampleData[wIndex].wLoopCount = 0;
					sSOSSampleData[wIndex].dwSampleSize= 0;
					SampleRay[wIndex].SOSHandle = -1;
					SampleRay[wIndex].playing = 0;
					SampleRay[wIndex].priority = 0;
					SampleRay[wIndex].number = -1;
					break;
				}
			}
		}
	}


	sSOSSampleData[wIndex].dwSampleSize= (unsigned short)DigiList[(sound * 3) + 1];

	SeekIndex = ( DigiList[(sound * 3) + 0] * 4096 );

	lseek(hSoundFile,SeekIndex,0x00);
	memset(sSOSSampleData[wIndex].lpSamplePtr,'0',55000);
	read(hSoundFile,sSOSSampleData[wIndex].lpSamplePtr,sSOSSampleData[wIndex].dwSampleSize);

	if(loopcount)
		sSOSSampleData[wIndex].wLoopCount = loopcount;

	if(Pan)
		Pan=((getangle(player[pyrn].x-x,player[pyrn].y-y)+(2047-player[pyrn].ang))%2047) >> 6;


	sSOSSampleData[wIndex].wSamplePanLocation = PanArray[Pan];
	sSOSSampleData[wIndex].wVolume = wVol;
	SampleRay[wIndex].SOSHandle = sosDIGIStartSample(hSOSDriverHandles[DIGI],&sSOSSampleData[wIndex] );
	SampleRay[wIndex].x = x;
	SampleRay[wIndex].y = y;
	SampleRay[wIndex].playing = 1;
	SampleRay[wIndex].number = sound;
	SampleRay[wIndex].priority = prioritize;
	ActiveSampleBits |= (0x01<<wIndex);

	return(SampleRay[wIndex].SOSHandle);
*/
    return 0;
}


unsigned short
SND_Sound(unsigned short sound )
{
static unsigned short handle;

	if(!SoundMode)
		return(-1);
	return(SND_PlaySound(sound,0,0,0,0));

}

void
SND_CheckLoops(void)
{

	//special case loops
	if( cartsnd != -1) {
		SND_StopLoop(cartsnd);
		cartsnd=-1;
	}
	if( lavasnd != -1 ) {
		SND_StopLoop(lavasnd);
		lavasnd=-1;
	}
	if( batsnd != -1 ) {
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
SND_StopLoop(unsigned short which)
{
/*
	if(!SoundMode)
		return;

		for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
			if(which==SampleRay[wIndex].SOSHandle)
				break;

		sosDIGIStopSample(hSOSDriverHandles[DIGI],SampleRay[wIndex].SOSHandle );
		sSOSSampleData[wIndex].wLoopCount = 0;
		SampleRay[wIndex].SOSHandle = -1;
		SampleRay[wIndex].playing = 0;
		SampleRay[wIndex].number = -1;
*/
}


void
SND_DIGIFlush(void)
{
    /*
	if(!SoundMode)
		return;

	for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
	{
		if( SampleRay[wIndex].playing)
			sosDIGIStopSample(hSOSDriverHandles[DIGI],SampleRay[wIndex].SOSHandle );
		//if( sSOSSampleData[wIndex].lpSamplePtr != NULL )
		//  free(sSOSSampleData[wIndex].lpSamplePtr);
		SampleRay[wIndex].SOSHandle = -1;
		SampleRay[wIndex].playing = 0;
		SampleRay[wIndex].number = -1;
		ActiveSampleBits |= (0x01<<wIndex);
	}*/
}



//
//    Sound Location Stuff
//


void
SND_UpdateSoundLoc(unsigned short which,unsigned short Volume,unsigned short Pan)
{
/*
	gVol=Volume;
	gPan=sosDIGISetPanLocation(hSOSDriverHandles[DIGI],SampleRay[which].SOSHandle,PanArray[Pan]);
	sosDIGISetSampleVolume(hSOSDriverHandles[DIGI],SampleRay[which].SOSHandle,Volume);
*/
}


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
int sqrdist;
unsigned wVol,wPan;

	 SND_PlaySound(soundnum,xplc,yplc,1,0);
}

void
updatesound_loc(void)
{
int wIndex;
unsigned wVol,wPan;
int sqrdist;
/*
	if(!SoundMode)
		return;

	 for(wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++)
		  if( SampleRay[wIndex].playing && SampleRay[wIndex].x && SampleRay[wIndex].y)
		  {
			if(sSOSSampleData[wIndex].dwSampleSize!= 0) {
				sqrdist = labs(player[pyrn].x-SampleRay[wIndex].x) +
						  labs(player[pyrn].y-SampleRay[wIndex].y);

				if(sqrdist < 1500)
					wVol = 0x7fff;
				else if(sqrdist > 8500)
					wVol = 0x1f00;
				else
					wVol = 39000-(sqrdist<<2);

				wPan=((getangle(player[pyrn].x-SampleRay[wIndex].x,player[pyrn].y-SampleRay[wIndex].y)+(2047-player[pyrn].ang))%2047) >> 6;
				SND_UpdateSoundLoc(wIndex,wVol,wPan);
			 //sprintf(displaybuf,"%dVol %x Pan %x Dist %ld",SampleRay[wIndex].number,wVol,wPan,sqrdist);
			 //displaytime=100;
			}

		  }
*/
}

// Location Stuff End
