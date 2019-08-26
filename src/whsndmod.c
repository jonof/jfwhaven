
#define  SND_CALLER
#define  GAME
#include <malloc.h>
#include "sos.h"
#include "icorp.h"

int   readfile(int,char *,long);
#pragma aux readfile =\
	"mov  ah,0x3f",\
	"int 0x21",\
	parm [ebx] [edx] [ecx]\
	modify [eax];


int flag=0;

extern char tempbuf[];
extern char displaybuf[50];

extern int  displaytime;

//JSA SPOOGE
extern int musiclevel;
extern int digilevel;
extern int loopinstuff;

// local data for hmi INI
static   PSTR  szHexNumbers  =  "0123456789ABCDEF";
static   WORD  wMultiplier[]   =  { 1, 16, 256, 4096, 65536, 1048576, 16777216, 268435456 };
// local function prototypes
WORD  hmiINIHex2Decimal( PSTR szHexValue );
WORD  hmiINIGetHexIndex( BYTE bValue );

//TEMP!!
volatile  unsigned joetime;

static WORD songelements=3;
static WORD arrangements=3;
static WORD songsperlevel;
static WORD totallevels=6;              //really thre use two to test menusong

//JSA_DEMO move these to sndmod.h if they work
LPSTR       BaseSongPtr;
LPSTR       EmbSongPtr;
LPSTR       SpiceSongPtr;


//
//                         INTERNAL ROUTINES
//

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    SND_InitSOSTimer() : Registers the timerevent() function to be        //
//          called by SOS.                                                  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
SND_InitSOSTimer(VOID)
{
	if( ( wError  = sosTIMERRegisterEvent( 120, (VOID(far *))timerevent, &hTimerT_ClockHandle ) ) )
	{
		sosTIMERUnInitSystem( 0 );
		sosDIGIUnInitSystem();
		crash( "TIMER FAILURE!" );
	}
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    timerevent() : Handles timer functions originally in game.c           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
_far _loadds timerevent(VOID)
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
VOID
_far cdecl sosDIGISampleCallback(  WORD wDriverHandle, WORD wCallSource, WORD hSample )
{
//
// recording driver will also use this callback for future!
// will have to switch wDriverHandle as well...
//

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

}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    sosMIDISongCallback(WORD) : Call back routine for SOS                  //
//          Midi function InitSong Struct. hSong indicates which                 //
//              song is complete (handle from sosMIDIInitSong).                      //
//          NEEDS WORK FOR JUGGLING AND MIXING SONGS!!!!!!!                 //
//////////////////////////////////////////////////////////////////////////////
VOID
_far sosMIDISongCallback( WORD hSong )
{

	for(wIndex=0; wIndex<MAX_ACTIVE_SONGS; wIndex++)
		if(hSong == hSOSSongHandles[wIndex])
			break;

	//sosMIDIStopSong(hSOSSongHandles[wIndex]);
	sosMIDIUnInitSong(hSOSSongHandles[wIndex]);
	hSOSSongHandles[wIndex] = 0x7fff;
}



VOID
_far cdecl _loadds sosMIDITriggerCallback( WORD hSong, BYTE bTrack, BYTE bID )
{
	if(SongPending) {
		SND_StartMIDISong(SongPending);
		SongPending=0;
	}


  if(BranchPending) {
	  sosMIDIBranchToSongLocation(hSOSSongHandles[BASE_SONG],0);
	  BranchPending=0;
  }
}


VOID
SND_SetupTables(VOID)
{

	if(SoundMode) {
		hSoundFile = open("JOESND",O_RDONLY | O_BINARY);
		if( hSoundFile == -1 ) {
			crash("COULDN'T OPEN JOESND!");
		}
		DigiList = malloc(0x1000);
		lseek(hSoundFile,-4096L,SEEK_END);
		readfile(hSoundFile,(void *)FP_OFF(DigiList),4096);
	}

	if(MusicMode==_LOOP_MUSIC) {
		hLoopFile = open("LOOPS",O_RDONLY | O_BINARY);
		if( hLoopFile == -1 ) {
			crash("COULDN'T OPEN LOOPS!");
		}
		LoopList = malloc(0x1000);
		lseek(hLoopFile,-4096L,SEEK_END);
		readfile(hLoopFile,(void *)FP_OFF(LoopList),4096);
	}
	else if(MusicMode) {
		hSongFile = open("SONGS",O_RDONLY | O_BINARY);
		if( hSongFile == -1 ) {
			crash("COULDN'T OPEN SONGS!");
		}
		SongList = malloc(0x1000);
		lseek(hSongFile,-4096L,SEEK_END);
		readfile(hSongFile,(void *)FP_OFF(SongList),4096);
	}

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
static WORD  wLength;

//JSA_DEMO check port address to verify FM device

	if( (MusicMode==_STANDARD_MUSIC || MusicMode==_DIG_MIDI_MUSIC) &&
			sMIDIHardware.wPort==0x388 ) {
//              (wMIDIDeviceID != _MIDI_MPU_401 && wMIDIDeviceID != _MIDI_MT_32
//                  && wMIDIDeviceID!=_MIDI_AWE32) ) {
		hMiscHandle = open("melodic.bnk",O_RDONLY);
		if( hMiscHandle == -1 )
			crash("MELODIC BANK FILE FAILED!");
		m_bnkptr = malloc(0x152c);
		read( hMiscHandle,( void * )FP_OFF(m_bnkptr),0x152c );
		close( hMiscHandle );
		if( ( wError = sosMIDISetInsData( hSOSDriverHandles[MIDI], m_bnkptr, 0x00 ) ) )
			crash("BAD SetInsData MEL!" );

		hMiscHandle = open("drum.bnk",O_RDONLY);
		if( hMiscHandle == -1 )
			crash("PERCUSSIVE BANK FILE FAILED!");
		d_bnkptr = malloc(0x152c);
		read( hMiscHandle,( void * )FP_OFF(d_bnkptr),0x152c );
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
		read( hMiscHandle,( void * )FP_OFF(digi_bnkptr),wLength );
		close( hMiscHandle );
		if( ( wError = sosMIDISetInsData( hSOSDriverHandles[DIG_MIDI], digi_bnkptr, 0x00 ) ) )
			crash("BAD SetInsData digmidi!" );
	}
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    SND_LoadMIDISong(WORD) : Handle near/far pointer krap for music file. //
//                                  (LPSTR = BYTE far *)                                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
LPSTR SND_LoadMIDISong( WORD which )
{
LPSTR lpDataPtr;
PSTR  pDataPtr;
WORD  wLength;


   wLength  = (WORD)SongList[(which * 3) + 1];
	SeekIndex = ( SongList[(which * 3) + 0] * 4096 );
   pDataPtr =  malloc( wLength );

	lseek(hSongFile,SeekIndex,0x00);

   read( hSongFile, pDataPtr, wLength );

   // create far pointer  !!!!!
   lpDataPtr   =  (LPSTR)pDataPtr;
   return( lpDataPtr );
}


//
//                         PUBLIC ROUTINES
//



//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       SND_AddTimerEvent( WORD, VOID (_far _loadds *)() ) : Register an   //
//             event with SOS. Save the handle it returns, it is needed     //
//             to release the event. The event routine must be declared as  //
//             _far _loadds routine(void) in the calling module             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

WORD
SND_AddTimerEvent( WORD wEventRate, VOID ( _far _loadds *TimerRoutine)(void) )
{

	for( wIndex=0; wIndex<MAX_AUX_TIMERS; wIndex++ )
		if( !AuxTimerList[wIndex] )
			break;

	if( ( wError = sosTIMERRegisterEvent( wEventRate, (VOID(far *))TimerRoutine,
		&AuxTimerList[wIndex] ) ) )
	{
		crash( "Could not register TimerRoutine!" );
	}

	ActiveTimerBits |= (0x01<<wIndex);
	return(AuxTimerList[wIndex]);

}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       SND_RemoveTimerEvent( WORD ) : Release the timer event registerd   //
//             with handle wTimerHandle.                                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

VOID
SND_RemoveTimerEvent( WORD wTimerHandle )
{

	for( wIndex=0; wIndex<MAX_AUX_TIMERS; wIndex++ )
		if( AuxTimerList[wIndex]==wTimerHandle )
			break;

	sosTIMERRemoveEvent( AuxTimerList[wIndex] );
	ActiveTimerBits ^= (0x01<<wIndex);
	AuxTimerList[wIndex] = 0;

}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       SND_GetNumAuxEvents( VOID ) : Returns the number of currenly       //
//             registered SOSTimer events.                                  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
WORD
SND_GetNumAuxEvents( VOID )
{
WORD  wTimerCount=0;
BYTE  temp;

	temp = ActiveTimerBits;
	for( wIndex=0; wIndex<MAX_AUX_TIMERS; wIndex++ )
		if( (temp>>wIndex) & 0x01 )
			wTimerCount++;

	return(wTimerCount);

}


VOID
SND_DoBuffers(void)
{


	for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ ) {
		sSOSSampleData[wIndex].lpSamplePtr = (LPSTR)malloc((long)55000);
		if(sSOSSampleData[wIndex].lpSamplePtr==_NULL)
			crash("Could Not get a Sound Buffer!!");
	}

}

VOID
SND_UnDoBuffers(void)
{

	for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ ) {
		if( sSOSSampleData[wIndex].lpSamplePtr != NULL )
			free((void *)FP_OFF(sSOSSampleData[wIndex].lpSamplePtr));
	}

}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       SND_Startup() : Initialize all SOS Drivers and start timer         //
//             service.                                                     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
SND_Startup(VOID)
{

	if (SD_Started)
		return;

	if( !sosEZGetConfig( "hmiset.cfg" ) ) {
		crash("Sound Config Problem!");
	}

//GET Volume values
		wMIDIVol = (musiclevel<<3);
		wDIGIVol = (digilevel<<11);


//TIMERSYSTEM

	wError = sosTIMERInitSystem( _TIMER_DOS_RATE, _SOS_DEBUG_NORMAL );
	if(wError!=_ERR_NO_ERROR)
		crash( "Problem with Timer SysInit!" );

	SND_InitSOSTimer();

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

	// read in offset page list's
	SND_SetupTables();
	SD_Started = _TRUE;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       SND_Shutdown() : Un-Initialize all SOS Drivers and releases        //
//             timer service(s).                                            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
SND_Shutdown(VOID)
{

	if (!SD_Started)
		return;

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
			free( ( PSTR )FP_OFF(lpMIDISong) );
		Midi_Loaded=_FALSE;
	}

	sosMIDIUnInitSystem();

	sosDIGIUnInitSystem();

	sosTIMERRemoveEvent( hTimerT_ClockHandle );

	sosTIMERUnInitSystem( 0 );

	SD_Started = _FALSE;

}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       SND_Mixer(WORD wSource,WORD wVolume) : Change Music or SFX Volume  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
SND_Mixer( WORD wSource, WORD wVolume )
{

	if(wSource==MIDI) {
		wMIDIVol = (wVolume<<3);
		sosMIDISetMasterVolume((BYTE)wMIDIVol);
	}

	else {
		wDIGIVol = (wVolume<<11);
		sosDIGISetMasterVolume(hSOSDriverHandles[DIGI],wDIGIVol);
	}
}


VOID
SND_MenuMusic(int choose)
{

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

}

//
//  SND_StartMusic() will only be called at the beginning of a new level
//      will be used in all other cases.

VOID
SND_StartMusic(WORD level)
{

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

}


VOID
SND_LoadSongs(WORD which)
{
static int index;

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
}


//
//  SND_PrepareMIDISong() loads a particular song into memory, initializes
//      it with SOS and puts it's handle into the hSOSSongHandles array.
//      The song can then be started by passing the handle to SND_StartMIDISong()
//
int
SND_PrepareMIDISong(int SongIndex)
{
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


	sSOSInitSongs[SongIndex].lpSongCallback = (VOID(far *))sosMIDISongCallback;
	if( ( wError = sosMIDIInitSong( &sSOSInitSongs[SongIndex], &sSOSTrackMap[SongIndex], &hSOSSongHandles[SongIndex] ) ) )
	{
			crash("Init Song Failed!");
	}

	return((int)hSOSSongHandles[SongIndex]);

}

int
SND_StartMIDISong(WORD wSongHandle)
{
	sosMIDISetMasterVolume((BYTE)wMIDIVol);
	return(sosMIDIStartSong(wSongHandle));
}


VOID
SND_StopMIDISong(WORD wSongHandle)
{

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
		free((void *)FP_OFF(sSOSInitSongs[wIndex].lpSongData));
	}

}


VOID
SND_SongFlush(void)
{
	if(!MusicMode)
		return;

	if(hSOSSongHandles[BASE_SONG] != 0x7fff) {
		SND_StopMIDISong(hSOSSongHandles[BASE_SONG]);
	}
	if(hSOSSongHandles[EMB_SONG] != 0x7fff)
		SND_StopMIDISong(hSOSSongHandles[EMB_SONG]);
	if(hSOSSongHandles[SPICE_SONG] != 0x7fff)
		SND_StopMIDISong(hSOSSongHandles[SPICE_SONG]);

}


VOID
SND_FadeMusic(void)
{
	if(!MusicMode)
		return;
//  sosMIDIFadeSong(hSOSSongHandles[BASE_SONG],_SOS_MIDI_FADE_OUT,200,(BYTE)wMIDIVol,(BYTE)0,10);
}

VOID
SND_MIDIFlush(void)
{

	for(wIndex=0; wIndex<MAX_ACTIVE_SONGS; wIndex++) {
		if( !sosMIDISongDone(hSOSSongHandles[wIndex]) )
			sosMIDIStopSong(hSOSSongHandles[wIndex]);
		if(hSOSSongHandles[wIndex] != 0x7fff)
			sosMIDIUnInitSong( hSOSSongHandles[wIndex] );
		hSOSSongHandles[wIndex] = 0x7fff;
	}

	free((void *)FP_OFF(BaseSongPtr));
	free((void *)FP_OFF(EmbSongPtr));
	free((void *)FP_OFF(SpiceSongPtr));

}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    SND_TriggerHook(WORD) : Set the pending song metronome call.          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
SND_TriggerHook(WORD wSongNum)
{
	sosMIDIRegisterTriggerFunction( hSOSSongHandles[wSongNum], 0x04,(VOID(far *))sosMIDITriggerCallback );
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    SND_LoadLoop(VOID) : Load and start digiloop - use looppending        //
//          in the callback to cue.                                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
SND_LoadLoop(WORD load_start)
{

	if(!SoundMode)
		return;

	SeekIndex = ( LoopList[(LoopIndex * 3)+0] * 4096 );
	LoopSampleData[looptoggle].dwSampleSize= (WORD)LoopList[(LoopIndex * 3) + 1];


	lseek(hLoopFile,SeekIndex,0x00);

   if(!load_start) {
	readfile(hLoopFile,( void * )FP_OFF(LoopSampleData[looptoggle].lpSamplePtr),
			LoopSampleData[looptoggle].dwSampleSize);
   }

	else {
	LoopSampleData[0].lpSamplePtr = (LPSTR)malloc( 40000 );
	LoopSampleData[1].lpSamplePtr = (LPSTR)malloc( 40000 );
	readfile(hLoopFile,( void * )FP_OFF(LoopSampleData[looptoggle].lpSamplePtr),
			LoopSampleData[looptoggle].dwSampleSize);

		LoopHandles[looptoggle] = sosDIGIStartSample(hSOSDriverHandles[DIGI],&LoopSampleData[looptoggle] );
	looptoggle^=1;
	}

	LoopIndex++;
	if(LoopIndex>MAX_SND_LOOPS-1)
		LoopIndex = 0;

}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    SND_LoadLoop(VOID) : Load and start digiloop - use looppending        //
//          in the callback to cue.                                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
SND_SwapLoops(VOID)
{
int temp;

	if(!SoundMode)
		return;

	temp=looptoggle^1;

	if( !sosDIGISampleDone(hSOSDriverHandles[DIGI], LoopHandles[temp]) )
	{
		sosDIGIStopSample( hSOSDriverHandles[DIGI], LoopHandles[temp]);
//      free(( void * )FP_OFF(LoopSampleData[temp].lpSamplePtr));
		LoopHandles[looptoggle] = sosDIGIStartSample(hSOSDriverHandles[DIGI],&LoopSampleData[looptoggle] );
	}

	looptoggle^=1;

}


VOID
SND_Sting(WORD sound)
{
//make sure a sting sound is never 0
int temp;

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
}


//WORD
int
SND_PlaySound(WORD sound, long x,long y, WORD Pan,WORD loopcount)
{
WORD  wVol,flag=0;
long  sqrdist;
long  prioritize;

	if(!SoundMode)
		return(0);
		//return((WORD)0);


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


	sSOSSampleData[wIndex].dwSampleSize= (WORD)DigiList[(sound * 3) + 1];

	SeekIndex = ( DigiList[(sound * 3) + 0] * 4096 );

	lseek(hSoundFile,SeekIndex,0x00);
	memset(( void *)FP_OFF(sSOSSampleData[wIndex].lpSamplePtr),'0',55000);
	read(hSoundFile,( void *)FP_OFF(sSOSSampleData[wIndex].lpSamplePtr),sSOSSampleData[wIndex].dwSampleSize);

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

}


WORD
SND_Sound(WORD sound )
{
static WORD handle;

	if(!SoundMode)
		return(-1);
	return(SND_PlaySound(sound,0,0,0,0));

}

VOID
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

VOID
SND_StopLoop(WORD which)
{

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

}


VOID
SND_DIGIFLush(void)
{
	if(!SoundMode)
		return;

	for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
	{
		if( SampleRay[wIndex].playing)
			sosDIGIStopSample(hSOSDriverHandles[DIGI],SampleRay[wIndex].SOSHandle );
		//if( sSOSSampleData[wIndex].lpSamplePtr != NULL )
		//  free((void *)FP_OFF(sSOSSampleData[wIndex].lpSamplePtr));
		SampleRay[wIndex].SOSHandle = -1;
		SampleRay[wIndex].playing = 0;
		SampleRay[wIndex].number = -1;
		ActiveSampleBits |= (0x01<<wIndex);
	}
}



//
//    Sound Location Stuff
//


VOID
SND_UpdateSoundLoc(WORD which,WORD Volume,WORD Pan)
{

	gVol=Volume;
	gPan=sosDIGISetPanLocation(hSOSDriverHandles[DIGI],SampleRay[which].SOSHandle,PanArray[Pan]);
	sosDIGISetSampleVolume(hSOSDriverHandles[DIGI],SampleRay[which].SOSHandle,Volume);

}



// JOE START functions called often from external modules

//JSA 3/20/95
//
//    note: soundnum=sound to play; xplc is x location of source; yplc is y loc
//

void
playsound_loc(int soundnum, long xplc, long yplc)
{
long sqrdist;
unsigned wVol,wPan;

	 SND_PlaySound(soundnum,xplc,yplc,1,0);
}

void
updatesound_loc(void)
{
int wIndex;
unsigned wVol,wPan;
long sqrdist;

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

}

// Location Stuff End


//
//    SOS INI File Stuff
//


#if 1
// BOOL  sosEZGetConfig( PSTR szName )
BOOL  cdecl sosEZGetConfig( PSTR szName )
{
		_INI_INSTANCE  sInstance;
		BOOL           wError;

		// open .ini file
		if ( !hmiINIOpen( &sInstance, szName ) )
			return( _FALSE );

		// locate section for digital settings
		if ( !hmiINILocateSection( &sInstance, "DIGITAL" ) )
		{
			// close file
			hmiINIClose( &sInstance );

			// return error
			return( _FALSE );
		}

		// fetch device ID, Port, DMA, IRQ
		wError   =  hmiINIGetItemDecimal( &sInstance, "DeviceID", &wDIGIDeviceID );
		wError   =  hmiINIGetItemDecimal( &sInstance, "DevicePort", &sDIGIHardware.wPort );
		wError   =  hmiINIGetItemDecimal( &sInstance, "DeviceDMA", &sDIGIHardware.wDMA );
		wError   =  hmiINIGetItemDecimal( &sInstance, "DeviceIRQ", &sDIGIHardware.wIRQ );

		// error
		if ( !wError )
		{
			// close file
			hmiINIClose( &sInstance );

			// return error
			return( _FALSE );
		}

		// locate section for MIDI settings
		if ( !hmiINILocateSection( &sInstance, "MIDI" ) )
		{
			// close file
			hmiINIClose( &sInstance );

			// return error
			return( _FALSE );
		}

		// fetch device ID, Port, DMA, IRQ
		wError   =  hmiINIGetItemDecimal( &sInstance, "DeviceID", &wMIDIDeviceID );
		wError   =  hmiINIGetItemDecimal( &sInstance, "DevicePort", &sMIDIHardware.wPort );

		// error
		if ( !wError )
		{
			// close file
			hmiINIClose( &sInstance );

			// return error
			return( _FALSE );
		}

		// close file
		hmiINIClose( &sInstance );

		// return success
		return( _TRUE );
}



WORD  hmiINIHex2Decimal( PSTR szHexValue )
	{
		WORD  wDecimal    =  0;
		WORD  wPlaces     =  strlen( szHexValue );
		WORD  wMultIndex;
		WORD  wIndex      =  0;

		// count down
		do
			{
				// accumulate value
				wDecimal += wMultiplier[ wPlaces - 1 ] * hmiINIGetHexIndex( (BYTE)szHexValue[ wIndex++ ] );

				// decrement places
				wPlaces--;
			}
		while( wPlaces > 0 );

		// return decimal
		return( wDecimal );
	}

/****************************************************************************
*
*  Syntax
*
*     WORD  hmiINIGetHexIndex( BYTE bValue )
*
*  Description
*
*     Get the index of a hex character
*
*  Parameters
*
*        Type           Description
*        --------------------------
*        bValue         value to located
*
*  Return
*
*     index into table of hex value
*
****************************************************************************/
WORD  hmiINIGetHexIndex( BYTE bValue )
	{
		WORD  wIndex;

		// search
		for ( wIndex = 0; wIndex < 16; wIndex++ )
			if ( szHexNumbers[ wIndex ] == toupper( bValue ) )
				return( wIndex );

		// error return
		return( -1 );
	}



/****************************************************************************
*
*  Syntax
*
*     BOOL  cdecl hmiINIOpen( _INI_INSTANCE * sInstance, PSTR szName )
*
*  Description
*
*     opens and instance of a .ini file
*
*  Parameters
*
*        Type           Description
*        --------------------------
*        sInstance      pointer to an .ini instance
*        szName         pointer to the name of the .ini file
*
*  Return
*
*     _TRUE       the file was opened correctly
*     _FALSE      there was a problem opening the file
*
****************************************************************************/
BOOL  cdecl hmiINIOpen( _INI_INSTANCE * sInstance, PSTR szName )
	{
		WORD  hFile;

		// save the name of the .ini file
		strcpy( sInstance->szName, szName );

		// open .ini file, return error if file is not
		// found.
		if ( ( hFile = open( szName, O_RDONLY | O_BINARY ) ) == -1 )
			return( _FALSE );

		// determine size of file
		sInstance->wSize  =  lseek( hFile, 0, SEEK_END );

		// set the new maximum size
		sInstance->wMaxSize  =  sInstance->wSize + _INI_EXTRA_MEMORY;

		// seek back to start of file
		lseek( hFile, 0, SEEK_SET );

		// allocate memory for the file
		if ( ( sInstance->pData  =  ( PSTR )malloc( sInstance->wMaxSize ) ) == _NULL )
		{
			// close file
			close( hFile );

			// return error, not enough memory
			return( _FALSE );
		}

		// read in file
		if ( read( hFile, sInstance->pData, sInstance->wSize ) != sInstance->wSize )
		{
			// close file
			close( hFile );

			// free memory
			free( sInstance->pData );

			// return error, not file size incorrect
			return( _FALSE );
		}

		// close file
		close( hFile );

		// init current position
		sInstance->pCurrent  =  sInstance->pData;
		sInstance->wCurrent  =  0;

		// initalize current item pointer
		sInstance->pItem     =  _NULL;
		sInstance->pList     =  _NULL;
		sInstance->pItemPtr  =  _NULL;
		sInstance->pListPtr  =  _NULL;

		// reset the modified flag to indicate that the
		// file is unmodified.
		sInstance->wFlags    &= ~_INI_MODIFIED;

		// return success
		return( _TRUE );
	}

/****************************************************************************
*
*  Syntax
*
*     BOOL  cdecl hmiINIClose( _INI_INSTANCE * sInstance )
*
*  Description
*
*     close and instance of a .ini file. note that if the file is modified
*     it will be written back to the original file.
*
*  Parameters
*
*        Type           Description
*        --------------------------
*        sInstance      pointer to .ini instance
*
*  Return
*
*     _TRUE       file was closed and/or written correctly
*     _FALSE      a problem was encountered when writing/closing the file
*
****************************************************************************/
BOOL  cdecl hmiINIClose( _INI_INSTANCE * sInstance )
	{
		WORD  hFile;

		// determine if the .ini file has been modified
		if ( sInstance->wFlags & _INI_MODIFIED )
		{
			// create and open file
			if ( ( hFile =  open( (const char * )sInstance->szName, O_CREAT | O_TRUNC | O_RDWR | O_BINARY, 0 ) ) == -1 )
			{
				// free memory
				free( sInstance->pData );

				// error creating file
				return( _FALSE );
			}

			// write data back out
			write( hFile, sInstance->pData, sInstance->wSize );

			// close file
			close( hFile );
		}

		// free memory
		free( sInstance->pData );

		// return success
		return( _TRUE );
	}


/****************************************************************************
*
*  Syntax
*
*     BOOL cdecl  hmiINILocateSection( _INI_INSTANCE * sInstance, PSTR szName )
*
*  Description
*
*     locates a section in a file.  a section is determined by enclosing
*     it in [].  ie.  [SECTION]
*
*  Parameters
*
*        Type           Description
*        --------------------------
*        sInstance      pointer to .ini instance
*        szName         pointer to section name
*
*  Return
*
*     _TRUE       section located
*     _FALSE      section not located
*
****************************************************************************/
BOOL  cdecl hmiINILocateSection( _INI_INSTANCE * sInstance, PSTR szName )
	{
		PSTR  pDataPtr;
		PSTR  pSectionPtr;
		PSTR  szSection;
		WORD  wIndex;
		WORD  wFoundFlag  =  _FALSE;

		// set data pointer to pointer to start of .ini in memory
		pDataPtr    =  sInstance->pData;

		// initialize index
		wIndex      =  0;

		// search data until we have found a start section character
		// and then attempt to match section string.  continue to process
		// entire data set until the end is reached or a match is
		// found.
		do
			{
				// check if character we are pointing to is a
				// start section character
				if ( *pDataPtr == _INI_SECTION_START )
				{
					// save pointer to start of section for use by the
					// delete functions.
					pSectionPtr =  pDataPtr;

					// advance past the start section character
					pDataPtr++;

					// set pointer to section name
					szSection   =  szName;

					// search the string character by character to determine
					// if we have a match.
					while( *pDataPtr == *szSection && wIndex < sInstance->wSize )
					{
						// advance section pointer
						szSection++;

						// advance data pointer
						pDataPtr++;

						// advance data index
						wIndex++;
					}

					// determine if we are sitting on a end section
					// character. if so then we have a complete match
					// so set the found flag to true.
					if ( *pDataPtr == _INI_SECTION_END && *szSection == _NULL )
					{
						// set found flag
						wFoundFlag  =  _TRUE;

						// move to the next line
						while( *pDataPtr != _INI_LF )
							pDataPtr++;

						// advance past line feed
						pDataPtr++;

						// set list pointer for raw name
						sInstance->pListPtr  =  pDataPtr;

						// set current data pointer to new section
						// location.
						sInstance->pCurrent  =  pDataPtr;
						sInstance->wCurrent  =  wIndex;

						// save pointer to start of current section for
						// use by other functions.
						sInstance->pSection  =  pSectionPtr;
					}

				}

				// advance pointer
				pDataPtr++;

				// advance index
				wIndex++;
			}
		while( !wFoundFlag && wIndex < sInstance->wSize );

		// return the status of the found flag, this will indicate
		// if the desired section was located.
		return( ( BOOL )wFoundFlag );
	}


/****************************************************************************
*
*  Syntax
*
*     BOOL  cdecl hmiINILocateItem( _INI_INSTANCE * sInstance, PSTR szItem )
*
*  Description
*
*     locates an item under a section in an .ini file
*
*  Parameters
*
*        Type           Description
*        --------------------------
*        sInstance      pointer to an .ini instance
*        szItem         pointer to the name of the item
*
*  Return
*
*     _TRUE    the item was located
*     _FALSE   the item was not located
*
****************************************************************************/
BOOL  cdecl hmiINILocateItem( _INI_INSTANCE * sInstance, PSTR szItem )
	{
		PSTR  pDataPtr;
		PSTR  pItemPtr;
		PSTR  szSearch;
		WORD  wIndex;
		WORD  wFoundFlag  =  _FALSE;

		// initialize current location pointers
		pDataPtr =  sInstance->pCurrent;
		wIndex   =  sInstance->wCurrent;

		// search each data item until match is found or the start
		// of a new section is found
		do
			{
				// set up search pointer
				szSearch =  szItem;

				// check if current character matches first
				// character of search string
				if ( *pDataPtr == *szSearch )
				{
					// set pointer to start of potential item.  this pointer
					// will be used later to mark the start of the item
					// string.
					pItemPtr    =  pDataPtr;

					// advance data pointer and search pointer
					pDataPtr++;
					szSearch++;

					// advance search index
					wIndex++;

					// search the rest of the string, make sure we do not overrun
					// the end of file and that the string still matches.
					while( *pDataPtr == *szSearch && wIndex < sInstance->wSize )
					{
						// advance data pointer
						pDataPtr++;

						// advance search pointer
						szSearch++;

						// advance index
						wIndex++;
					}

					// check if we located the string
					if ( *szSearch == _NULL )
					{
						// skip any white until we locate the '='
						// sign.
						while( *pDataPtr != _INI_EQUATE && *pDataPtr != _INI_EOL )
						{
							// advance data pointer
							pDataPtr++;

							// advance index
							wIndex++;
						}

						// check if we found and equate '=' character, if not
						// only set the start of line indicator so the string
						// and decimal routines know there is no value.
						if ( *pDataPtr == _INI_EQUATE )
						{
							// advance data pointer one past the equate
							pDataPtr++;

							// advance index
							wIndex++;

							// set the pointer to the new item
							sInstance->pItem     =  pDataPtr;
						}
						else
							sInstance->pItem     =  _NULL;

						// set the start of line item pointer for later use
						sInstance->pItemPtr  =  pItemPtr;

						// reset list pointer to indicate that we
						// do not have a list yet.
						sInstance->pList  =  _NULL;

						// set the found flag
						wFoundFlag  =  _TRUE;
					}
				}

				// advance to next place in data
				pDataPtr++;

				// advance index
				wIndex++;
			}
		while( !wFoundFlag && wIndex < sInstance->wSize && *pDataPtr != _INI_SECTION_START );

		// return found flag status
		return( ( BOOL )wFoundFlag );
	}



/****************************************************************************
*
*  Syntax
*
*     BOOL  cdecl hmiINIGetDecimal( _INI_INSTANCE * sInstance, WORD * wValue )
*
*  Description
*
*     retrieves a decimal value from an item in a .ini file.  note that if
*     the value is in hex (0x....) it will be automatically converted to
*     decimal.
*
*  Parameters
*
*        Type           Description
*        --------------------------
*        sInstance      pointer to an .ini instance
*        wValue         pointer to a word to store decimal value in
*
*  Return
*
*     _TRUE    value was located and converted correctly
*     _FALSE   value was not valid, or item search was not performed
*
****************************************************************************/
BOOL  cdecl hmiINIGetDecimal( _INI_INSTANCE * sInstance, WORD * wValue )
	{
		PSTR  pDataPtr;
		WORD  wDValue;
		BYTE  bBuffer[ 32 ];
		WORD  wIndex;

		// initialize pointer to data
		if ( sInstance->pList )
			pDataPtr    =  sInstance->pList;
		else
			pDataPtr    =  sInstance->pItem;

		// check if it is null
		if ( pDataPtr == _NULL )
			return( _FALSE );

		// skip all white space
		while( *pDataPtr == _INI_SPACE )
			pDataPtr++;

		// check if we are pointing to and EOL
		if ( *pDataPtr == _INI_EOL )
			return( _FALSE );

		// initialize buffer index
		wIndex   =  0;

		// fetch string for value
		while( *pDataPtr != _INI_EOL && *pDataPtr != _INI_LIST_SEPERATOR
					&& *pDataPtr != _INI_SPACE )
		{
			// save character
			bBuffer[ wIndex++ ]  =  *pDataPtr++;
		}

		// set null at the end of buffer
		bBuffer[ wIndex ]       =  '\0';

		// check if we have simply reached the end of the
		// line with no number.
		if ( wIndex == 0 )
			return( _FALSE );

		// skip all white space
		while( *pDataPtr == _INI_SPACE )
			pDataPtr++;

		// check if we have a list of numbers
		if ( *pDataPtr == _INI_LIST_SEPERATOR )
		{
			// set list pointer to one past the current
			// seperator.
			sInstance->pList  =  ++pDataPtr;
		}
		else
			sInstance->pList  =  pDataPtr;

		// check if the buffer contains a hex value
		if ( bBuffer[ 1 ] == _INI_HEX_INDICATOR )
		{
			// fetch hex value
			wDValue  =  hmiINIHex2Decimal( &bBuffer[ 2 ] );
		}
		else
		{
			// fetch value
			wDValue  =  (WORD)atoi( bBuffer );
		}

		// set value
		*wValue  =  wDValue;

		// return status
		return( _TRUE );
	}

/****************************************************************************
*
*  Syntax
*
*     BOOL  cdecl hmiINIGetItemDecimal( _INI_INSTANCE * sInstance, PSTR szItem,
*                                       WORD * wValue )
*
*  Description
*
*     locates and item and returns the decimal value associated with it
*
*  Parameters
*
*        Type           Description
*        --------------------------
*        sInstance      pointer to .ini instance
*        szItem         pointer to item string
*        wValue         pointer to word value
*
*  Return
*
*     _TRUE       value located and retrieved
*     _FALSE      error getting value
*
****************************************************************************/
BOOL  cdecl hmiINIGetItemDecimal( _INI_INSTANCE * sInstance, PSTR szItem,
											 WORD * wValue )
	{
		// attempt to locate item
		if ( !hmiINILocateItem( sInstance, szItem ) )
			return( _FALSE );

		// get decimal value
		if ( !hmiINIGetDecimal( sInstance, wValue ) )
			return( _FALSE );

		// return success
		return( _TRUE );
	}



#endif
