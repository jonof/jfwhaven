
#define  SND_CALLER
#define  GAME
#include "icorp.h"
#include "fx_man.h"
#include "music.h"

#define TRUE 1
#define FALSE 0

typedef struct
{
	int      handle;
	int      number;
	int      x,y;
	int      looptics;
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
int BaseSongLen, EmbSongLen, SpiceSongLen;

int activesong = -1;

unsigned short     SD_Started=0;
int     Midi_Loaded,Digi_Loaded;

unsigned short     hSoundFile  =  -1;       // Handle for Sound F/X file
unsigned short     hSongFile   =  -1;
unsigned short     hMiscHandle =  -1;


unsigned int    SongList[1024];

static int transmutehmp(char *filedata);

//
//                         INTERNAL ROUTINES
//

static void
soundcallback(unsigned int i)
{
     if (i == (unsigned int)-1) return;

     Samples[SampleRay[i].number].users--;
     if( Samples[SampleRay[i].number].users < 0 )
          Samples[SampleRay[i].number].users=0;
     if( Samples[SampleRay[i].number].users == 0 ) {
          Samples[SampleRay[i].number].cache_lock=199;
     }
     SampleRay[i].handle=-1;
     SampleRay[i].number=-1;
}

static void
stoploop(unsigned int i)
{
	if (SampleRay[i].handle >= 0 && FX_SoundActive(SampleRay[i].handle))
		FX_EndLooping(SampleRay[i].handle);
}


void
SND_SetupTables(void)
{
	int i;
	unsigned int    DigiList[1024];

	memset(&Samples, 0, sizeof(Samples));
	if(SoundMode) {
		hSoundFile = kopen4load("joesnd", 0);
		if( hSoundFile < 0 ) {
			crashgame("Couldn't open JOESND");
		}
		klseek(hSoundFile,-sizeof(DigiList),SEEK_END);
		i = kread(hSoundFile,DigiList,sizeof(DigiList));
		if (i != sizeof(DigiList)) {
			crashgame("JOESND is too short");
		}

		for (i=0; i<MAX_SAMPLES; i++) {
			Samples[i].offset = DigiList[i*3]*4096;
			Samples[i].cache_length = DigiList[i*3+1];
			Samples[i].priority = DigiList[i*3+2];
		}
	}

	if(MusicMode) {
		hSongFile = kopen4load("songs", 0);
		if( hSongFile < 0 ) {
			crashgame("Couldn't open SONGS!");
		}
		klseek(hSongFile,-sizeof(SongList),SEEK_END);
		i = kread(hSongFile,SongList,sizeof(SongList));
		if (i != sizeof(SongList)) {
			crashgame("SONGS is too short");
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    SND_LoadMIDISong(WORD) : Handle near/far pointer krap for music file. //
//                                  (LPSTR = BYTE far *)                                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
char * SND_LoadMIDISong( unsigned short which, int *length )
{
	char * pDataPtr;
	int  wLength, SeekIndex;

	wLength  = SongList[(which * 3) + 1];
	SeekIndex = ( SongList[(which * 3) + 0] * 4096 );
	pDataPtr =  malloc( wLength );

	klseek(hSongFile,SeekIndex,SEEK_SET);
	*length = kread( hSongFile, pDataPtr, wLength );

	if (*length != wLength) {
		free(pDataPtr);
		buildprintf("SND_LoadMIDISong: incomplete file read\n");
		return NULL;
	}

	*length = transmutehmp(pDataPtr);
	if (*length <= 0) {
		free(pDataPtr);
		buildprintf("SND_LoadMIDISong: could not convert HMP to MIDI\n");
		return NULL;
	}

	return pDataPtr;
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

	if(SoundMode) {
		#ifdef _WIN32
		initdata = (void *) win_gethwnd();
		#endif

		err = FX_Init(ASS_AutoDetect, MAX_ACTIVE_SAMPLES, &channels, &bitspersample, &mixrate, initdata);
		if (err != FX_Ok) {
			buildprintf("FX_Init error: %s\n", FX_ErrorString(err));
		} else {
			Digi_Loaded = TRUE;

			FX_SetCallBack(soundcallback);
			FX_SetVolume((digilevel<<4)-1);
		}

		for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ ) {
			SampleRay[wIndex].handle = -1;
			SampleRay[wIndex].number = -1;
		}
	}

	if(MusicMode) {
		songsperlevel=songelements*arrangements;

		err = MUSIC_Init(ASS_AutoDetect, "");
		if (err != MUSIC_Ok) {
			buildprintf("MUSIC_Init error: %s\n", MUSIC_ErrorString(err));
		} else {
			Midi_Loaded = TRUE;
		}
		MUSIC_SetVolume((musiclevel<<4)-1);
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

	if( MusicMode && Midi_Loaded )
	{
		MUSIC_StopSong();
		SND_SongFlush();
		MUSIC_Shutdown();
		Midi_Loaded=FALSE;
	}

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
	if(wSource==MIDI) {
		MUSIC_SetVolume((wVolume<<4)-1);
	}

	else {
		FX_SetVolume((wVolume<<4)-1);
	}
}


void
SND_LoadSongs(unsigned short which)
{
	int index;

	index=songsperlevel*which;

	index+=songelements;	// Skip FM.
	index+=songelements;	// Skip AWE32.

	BaseSongPtr = SND_LoadMIDISong(index+BASE_SONG, &BaseSongLen);
	EmbSongPtr = SND_LoadMIDISong(index+EMB_SONG, &EmbSongLen);
	SpiceSongPtr = SND_LoadMIDISong(index+SPICE_SONG, &SpiceSongLen);
}


int
SND_StartMIDISong(unsigned short wSongHandle)
{
    int rv;
    char *songdata;
    int songlen;

    switch (wSongHandle) {
	    case BASE_SONG:  songdata = BaseSongPtr; songlen = BaseSongLen; break;
	    case EMB_SONG:   songdata = EmbSongPtr; songlen = EmbSongLen; break;
	    case SPICE_SONG: songdata = SpiceSongPtr; songlen = SpiceSongLen; break;
    }
    if (!songdata || songlen <= 0) {
    	return -1;
    }

    //sosMIDISetMasterVolume((BYTE)wMIDIVol);

    rv = MUSIC_PlaySong(songdata, songlen, 1);
    if (rv != MUSIC_Ok) {
    	buildprintf("SND_StartMIDISong: could not play song: %s\n", MUSIC_ErrorString(MUSIC_ErrorCode));
    	return -1;
    }
    activesong = wSongHandle;
	return(0);
}


void
SND_StopMIDISong(unsigned short wSongHandle)
{
	if (wSongHandle != activesong)
		return;

	activesong = -1;
	MUSIC_StopSong();
}


void
SND_SongFlush(void)
{
	activesong = -1;
	MUSIC_StopSong();

	if (BaseSongPtr)
		free(BaseSongPtr);
	if (EmbSongPtr)
		free(EmbSongPtr);
	if (SpiceSongPtr)
		free(SpiceSongPtr);
	BaseSongPtr = NULL;
	EmbSongPtr = NULL;
	SpiceSongPtr = NULL;
}

void
SND_MenuMusic(int choose)
{

	if(!MusicMode || !SD_Started || !Midi_Loaded)
		return;

	SND_SongFlush();

	if(choose==MENUSONG)
		BaseSongPtr = SND_LoadMIDISong((totallevels*songsperlevel)+BASE_SONG+2, &BaseSongLen);
	else BaseSongPtr = SND_LoadMIDISong((totallevels*songsperlevel)+3+BASE_SONG+2, &BaseSongLen);

	SND_StartMIDISong(BASE_SONG);
}

//
//  SND_StartMusic() will only be called at the beginning of a new level
//      will be used in all other cases.

void
SND_StartMusic(unsigned short level)
{
	if((!MusicMode) || !SD_Started || !Midi_Loaded)
		return;

	if(level > 5)
		level=rand()%6;

	SND_SongFlush();
	SND_LoadSongs(level);
	SND_StartMIDISong(BASE_SONG);
}


void
SND_FadeMusic(void)
{
	if(!MusicMode)
		return;
//  sosMIDIFadeSong(hSOSSongHandles[BASE_SONG],_SOS_MIDI_FADE_OUT,200,(BYTE)wMIDIVol,(BYTE)0,10);
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
SND_PlaySound(unsigned short sound, int x,int y, unsigned short Pan,int looptics)
{
	//unsigned short  wVol;
	unsigned short flag=0;
	int  sqrdist, dx, dy, nr, wIndex;

	if(!SoundMode || !Digi_Loaded)
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
		if( SampleRay[wIndex].handle < 0 || !FX_SoundActive(SampleRay[wIndex].handle) ) {
			flag=1;
			break;
		}

	if(!flag && Samples[sound].priority<9)               //none available low prio
		return(-1);

	else if(!flag) {                        //none avail but high prio
		for( wIndex=0; wIndex<MAX_ACTIVE_SAMPLES; wIndex++ )
		{
			if(Samples[SampleRay[wIndex].number].priority<9 && SampleRay[wIndex].looptics !=-1) {
				if(FX_SoundActive(SampleRay[wIndex].handle))
				{
					FX_StopSound(SampleRay[wIndex].handle);
					if (SampleRay[wIndex].number>=0) {
						Samples[SampleRay[wIndex].number].users--;
						if (Samples[SampleRay[wIndex].number].users == 0) {
							Samples[SampleRay[wIndex].number].cache_lock = 199;
						}
					}
					SampleRay[wIndex].looptics = 0;
					SampleRay[wIndex].handle = -1;
					SampleRay[wIndex].number = -1;
					break;
				}
			}
		}
	}

	Samples[sound].cache_lock = 200;

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

	SampleRay[wIndex].looptics = looptics;
	if(looptics) {
		SampleRay[wIndex].handle = FX_PlayLoopedRaw(Samples[sound].cache_ptr, Samples[sound].cache_length,
			Samples[sound].cache_ptr, (char*)Samples[sound].cache_ptr+Samples[sound].cache_length-1,
       		11025, 0, 255, 255, 255, 1, wIndex);
	} else {
		SampleRay[wIndex].handle = FX_PlayRaw3D(Samples[sound].cache_ptr, Samples[sound].cache_length,
			11025, 0, Pan, sqrdist, 1, wIndex);
	}
	if (SampleRay[wIndex].handle < 0) {
		if (Samples[sound].users == 0)
			Samples[sound].cache_lock = 199;
	} else {
		Samples[sound].users++;
		SampleRay[wIndex].x = x;
		SampleRay[wIndex].y = y;
		SampleRay[wIndex].number = sound;
		if (looptics>0) setdelayfunc((void(*)(int))stoploop,wIndex,looptics);
	}
	return(SampleRay[wIndex].handle);
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
	if (wIndex==MAX_ACTIVE_SAMPLES)
		return;

	FX_StopSound(SampleRay[wIndex].handle);
	if (SampleRay[wIndex].number>=0) {
		Samples[SampleRay[wIndex].number].users--;
		if (Samples[SampleRay[wIndex].number].users == 0) {
			Samples[SampleRay[wIndex].number].cache_lock = 199;
		}
	}
	SampleRay[wIndex].looptics = 0;
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
		if (SampleRay[wIndex].number>=0) {
			Samples[SampleRay[wIndex].number].users--;
			if (Samples[SampleRay[wIndex].number].users == 0) {
				Samples[SampleRay[wIndex].number].cache_lock = 199;
			}
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
	(void)level;
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
	//unsigned wVol;
	unsigned wPan;
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


/*
Brutal in-place transformation of a SOS HMP file (a.k.a. NDMF according to sosm.h)
into Standard MIDI, mutating global looping controllers into Apogee EMIDI equivalents.

By Jonathon Fowler, 2023
Provided to the public domain given how dubiously licensed the sources of information
going into this were. Realised through a combination of:
  - SOS.H in the Witchaven code dump exposing the file structure
  - http://www.r-t-c-m.com/knowledge-base/downloads-rtcm/tekwar-tools/sos40.zip
    providing the SOS special MIDI controller descriptions
  - A crucial hint about variable length encoding byte order at
    https://github.com/Mindwerks/wildmidi/blob/master/docs/formats/HmpFileFormat.txt#L84-L96

Overall:
    struct ndmfheader header;
    struct ndmftracks tracks[header.numtracks];
    uint8_t branchtable[];

    NDMF is little-endian, MIDI is big-endian.
    NDMF variable-length encoding: 0aaaaaaa 0bbbbbbb 1ccccccc
    MIDI variable-length encoding: 1ccccccc 1bbbbbbb 0aaaaaaa

Transformation can happen in-place because NDMF has a massive header compared to MIDI,
so every write will be happening onto ground already trodden. Strict aliasing be damned.
*/

#if defined(__GNUC__) || defined(__clang__)
#define PACKED_STRUCT struct __attribute__ ((packed))
#elif defined(_MSC_VER)
#define PACKED_STRUCT struct
#pragma pack(1)
#else
#define PACKED_STRUCT struct
#endif

PACKED_STRUCT ndmfheader {
     uint8_t ident[32];       // "HMIMIDIP013195" \0...
     uint32_t branchofs;      // File offset to the branch table at the end
     uint32_t pad[3];
     uint32_t numtracks;
     uint32_t ticksperqunote;
     uint32_t tempo;          // Game clock dependent
     uint32_t playtime;       // Song length in seconds
     uint32_t channelprio[16];
     uint32_t trackmap[32][5];
     uint8_t  ctrlrestore[128];
     uint32_t pad2[2];
};

PACKED_STRUCT ndmftrackheader {
     uint32_t tracknum;
     uint32_t tracklen;       // Header length inclusive
     uint32_t channel;
     uint8_t data[];          // [tracklen-12]
};

PACKED_STRUCT smfheader {
     uint8_t ident[4];        // "MThd"
     uint32_t headsize;       // 6
     uint16_t format;
     uint16_t numtracks;
     uint16_t ticksperqunote;
};
PACKED_STRUCT smftrackheader {
     uint8_t ident[4];        // "MTrk"
     uint32_t tracklen;       // Header length exclusive
     uint8_t data[];          // [tracklen]
};

static int transmutehmp(char *filedata)
{
     const char ndmfident[] = "HMIMIDIP013195";
     const int commandlengths[8] = { 2, 2, 2, 2, 1, 1, 2, -1 };
     const int syscomlengths[16] =  { -1, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1 };

     if (memcmp(ndmfident, filedata, sizeof ndmfident)) return -1;

     // Extract the important values from the NDMF header.
     struct ndmfheader *ndmfhead = (struct ndmfheader *)filedata;
     int numtracks = B_LITTLE32(ndmfhead->numtracks);
     int ticksperqunote = B_LITTLE32(ndmfhead->ticksperqunote);
     int tempo = 120000000 / B_LITTLE32(ndmfhead->tempo);
     ndmfhead = NULL;

     // Construct a new MIDI header.
     struct smfheader *smfhead = (struct smfheader *)filedata;
     memcpy(smfhead->ident, "MThd", 4);
     smfhead->headsize = B_BIG32(6);
     smfhead->format = B_BIG16(1);
     smfhead->numtracks = B_BIG16(numtracks);
     smfhead->ticksperqunote = B_BIG16(ticksperqunote);

     // Transcribe tracks.
     int ndmffileofs = sizeof(struct ndmfheader);
     int smffileofs = sizeof(struct smfheader);
     for (int trk = 0; trk < numtracks; trk++) {
          struct ndmftrackheader *ndmftrack = (struct ndmftrackheader *)&filedata[ndmffileofs];
          struct smftrackheader *smftrack = (struct smftrackheader *)&filedata[smffileofs];

          int ndmfdatalen = B_LITTLE32(ndmftrack->tracklen) - 12;
          int smfdatalen = ndmfdatalen;
          if (trk == 0) {
               // We need to add a tempo event to the first MIDI track.
               smfdatalen += 7;
          }

          memcpy(smftrack->ident, "MTrk", 4);
          smftrack->tracklen = B_BIG32(smfdatalen);

          uint8_t *ndmfdata = (uint8_t *)&ndmftrack->data[0];
          uint8_t *smfdata = (uint8_t *)&smftrack->data[0];

          if (trk == 0) {
               // Insert a tempo event.
               *(smfdata++) = 0;
               *(smfdata++) = 0xff;
               *(smfdata++) = 0x51;
               *(smfdata++) = 3;
               *(smfdata++) = (tempo>>16)&0xff;
               *(smfdata++) = (tempo>>8)&0xff;
               *(smfdata++) = tempo&0xff;
          }

          // Process events.
          uint8_t status = 0;
          for (int i = 0; i < ndmfdatalen; ) {
               uint8_t b;
               int copylen = 0;

               // Re-encode the offset.
               uint8_t vlenbytes[4], vlencnt = 0;
               do {
                    b = ndmfdata[i++];
                    vlenbytes[vlencnt++] = b & 0x7f;
               } while (!(b & 0x80));
               do {
                    b = vlenbytes[--vlencnt];
                    if (vlencnt) b |= 0x80;
                    *(smfdata++) = b;
               } while (vlencnt > 0);

               b = ndmfdata[i];
               if (b&0x80) {
                    // A new status byte.
                    *(smfdata++) = b;
                    i++;

                    status = b;    // Keep for running status.
                    copylen = commandlengths[(status & 0x7f)>>4];

                    if ((b&0xf0) == 0xf0) {
                         switch (b&0x0f) {
                              case 0x0: // Sysex.
                                   do *(smfdata++) = (b = ndmfdata[i++]);
                                   while (!(b&0x80) && b != 0xf7);
                                   break;
                              case 0xf: // Meta.
                                   *(smfdata++) = ndmfdata[i++];  // Type.
                                   copylen = (*(smfdata++) = ndmfdata[i++]);     // Length.
                                   break;
                              default:  // Sys common.
                                   copylen = syscomlengths[b&0x0f];
                                   break;
                         }
                    } else if ((b&0xf0) == 0xb0) {     // Controller change.
                         // SOS/EMIDI custom controller range. For whatever reason SOS
                         // controller values have their high bit set.
                         if (ndmfdata[i] >= 102 && ndmfdata[i] <= 119) {
                              if (trk == 1 && ndmfdata[i] == 110) {        // Global loop start
                                   *(smfdata++) = 118;
                                   *(smfdata++) = (ndmfdata[i+1] & 0x7f);
                              } else if (trk == 1 && ndmfdata[i] == 111) { // Global loop end.
                                   *(smfdata++) = 119;
                                   *(smfdata++) = 127;
                              } else {
                                   *(smfdata++) = 102; // Neuter all other controllers.
                                   *(smfdata++) = 0;
                              }
                              i += 2;
                              copylen = 0;
                         }
                    }
               } else {
                    copylen = commandlengths[(status & 0x7f)>>4];
               }

               for (; copylen>0; copylen--) {     // Copy data bytes.
                    *(smfdata++) = ndmfdata[i++];
               }
          }

          ndmffileofs += ndmfdatalen + 12;
          smffileofs += smfdatalen + 8;
     }

     return smffileofs;
}

#undef PACKED_STRUCT
#ifdef _MSC_VER
#pragma pack()
#endif
