#define  GAME
#include "icorp.h"
#include "fx_man.h"
#include "smacker.h"

static smk smkseq;
#define NUMSMKTILES 3
#define SMKPICNUM0 (MAXTILES-NUMSMKTILES)

#define SMKAUDIOBUFSZ (32768)
static char *smkaudiobuf;
static unsigned long smkaudiowrite = 0, smkaudioread = 0;

static void smkplayfeeder(char **ptr, unsigned int *length) {
     *ptr = &smkaudiobuf[smkaudioread];
     *length = 2048;
     smkaudioread = (smkaudioread + 2048) % SMKAUDIOBUFSZ;
}

void
smkplayseq(char *name)
{
     char path[BMAX_PATH], result;
     int fh, flen, i;

     unsigned char smkpal[NUMSMKTILES][768], *smkbuf;
     unsigned char *smkframe[NUMSMKTILES];
     const unsigned char *palptr;

     unsigned long xsiz, ysiz;
     unsigned long audio_rate[7];
     double usf, clocktime;

     int voice = -1, frame, playframe, decodeframe;

     snprintf(path, sizeof(path), "%s.smk", name);
     fh = kopen4load(path, 0);
     if (fh < 0) {
          debugprintf("smkplayseq(\"%s\") failed\n", name);
          return;
     }

     flen = kfilelength(fh);
     smkbuf = (unsigned char *)malloc(flen);

     if (smkbuf == NULL) {
          kclose(fh);
          debugprintf("smkplayseq(\"%s\") malloc of %d bytes failed\n", name, flen);
          return;
     }

     kread(fh, smkbuf, flen);
     kclose(fh);

     smkseq = smk_open_memory(smkbuf, flen);
     if (!smkseq) {
          free(smkbuf);
          debugprintf("smk_open_memory() returned null\n");
          return;
     }

     free(smkbuf);

     smk_enable_all(smkseq, SMK_VIDEO_TRACK | SMK_AUDIO_TRACK_0);
     smk_info_all(smkseq, NULL, NULL, &usf);
     smk_info_video(smkseq, &xsiz, &ysiz, NULL);
     smk_info_audio(smkseq, NULL, NULL, NULL, audio_rate);

     // Smacker frames are decoded linewise, but BUILD expects its
     // tiles columnwise, so we will treat the tile as though it's
     // rotated 90 degrees and flipped horizontally.

     memset(smkframe, 0, sizeof(smkframe));
     for (i = 0; i < NUMSMKTILES; i++) {
          smkframe[i] = malloc(xsiz*ysiz);
          if (!smkframe[i]) {
               debugprintf("smkplayseq(\"%s\") malloc of frames failed\n", name);
               goto end;
          }

          tilesizx[SMKPICNUM0+i] = ysiz;
          tilesizy[SMKPICNUM0+i] = xsiz;
          waloff[SMKPICNUM0+i] = (intptr_t)smkframe[i];
     }

     smkaudiobuf = malloc(SMKAUDIOBUFSZ);
     if (!smkaudiobuf) {
          debugprintf("smkplayseq(\"%s\") malloc of audio buffer failed\n", name);
          goto end;
     }

     smkaudiowrite = 0;
     for (decodeframe = 0, result = smk_first(smkseq);
               decodeframe < NUMSMKTILES;
               decodeframe++, result = smk_next(smkseq)) {
          unsigned long audiolen;
          const unsigned char *audiodata;

          if (result != SMK_MORE && result != SMK_LAST) {
               debugprintf("smkplayseq(\"%s\") error on initial frames (%d)\n", name, result);
               goto end;
          }
          for (i=0, palptr=smk_get_palette(smkseq); i<768; i++, palptr++)
               smkpal[decodeframe][i] = (*palptr) >> 2;
          memcpy(smkframe[decodeframe], smk_get_video(smkseq), xsiz*ysiz);

          audiolen = smk_get_audio_size(smkseq, 0);
          audiodata = smk_get_audio(smkseq, 0);

          while (audiolen > 0) {
               unsigned long slice = SMKAUDIOBUFSZ - smkaudiowrite;
               if (slice > audiolen) slice = audiolen;
               memcpy(&smkaudiobuf[smkaudiowrite], audiodata, slice);
               audiolen -= slice;
               audiodata += slice;
               smkaudiowrite = (smkaudiowrite + slice) % SMKAUDIOBUFSZ;
          }

          if (result == SMK_LAST) break;
     }

     smkaudioread = 0;
     voice = FX_StartDemandFeedPlayback(smkplayfeeder, (int)audio_rate[0],
          0, 255, 255, 255, 1, (unsigned int)-1);
     if (voice < 0) {
          debugprintf("smkplayseq(\"%s\") failed to start audio playback\n", name);
          goto end;
     }

     clocktime = (double)getusecticks();
     playframe = 0;
     while (result == SMK_MORE && playframe < decodeframe) {
          while ((double)getusecticks() - clocktime > usf) {
               clocktime += usf;
               playframe++;
          }

          frame = playframe%NUMSMKTILES;
          setbrightness(brightness, smkpal[frame], 2);
#if USE_POLYMOST && USE_OPENGL
          invalidatetile(SMKPICNUM0+frame, 0, -1);
#endif
          clearallviews(0);
          rotatesprite(160<<16, 100<<16, divscale16(200, tilesizx[SMKPICNUM0+frame]),
                      512, SMKPICNUM0+frame, 0, 0, 2+4+64, 0, 0, xdim-1, ydim-1);
          nextpage();

          while ((decodeframe+1)%NUMSMKTILES != playframe%NUMSMKTILES && result != SMK_LAST) {
               unsigned long audiolen;
               const unsigned char *audiodata;

               result = smk_next(smkseq);
               if (result != SMK_MORE && result != SMK_LAST) {
                    debugprintf("smkplayseq(\"%s\") smk_next error (%d)\n", name, result);
                    break;
               }
               frame = decodeframe%NUMSMKTILES;
               for (i=0, palptr=smk_get_palette(smkseq); i<768; i++, palptr++)
                    smkpal[frame][i] = (*palptr) >> 2;
               memcpy(smkframe[frame], smk_get_video(smkseq), xsiz*ysiz);

               audiolen = smk_get_audio_size(smkseq, 0);
               audiodata = smk_get_audio(smkseq, 0);
               while (audiolen > 0) {
                    unsigned long slice = SMKAUDIOBUFSZ - smkaudiowrite;
                    if (slice > audiolen) slice = audiolen;
                    memcpy(&smkaudiobuf[smkaudiowrite], audiodata, slice);
                    audiolen -= slice;
                    audiodata += slice;
                    smkaudiowrite = (smkaudiowrite + slice) % SMKAUDIOBUFSZ;
               }

               decodeframe++;
          }

          handleevents();
          if (keystatus[0x1]|keystatus[0x1c]|keystatus[0x39]|keystatus[0x9c]) {
               keystatus[0x1]=keystatus[0x1c]=keystatus[0x39]=keystatus[0x9c]=0;
               break;
          }
          if (!FX_SoundActive(voice)) { // If the audio stops for some reason.
               break;
          }
     }

     FX_StopSound(voice);
end:
     if (smkseq) smk_close(smkseq);
     smkseq = NULL;

     if (smkaudiobuf) free(smkaudiobuf);
     smkaudiobuf = NULL;

     for (i = 0; i < NUMSMKTILES; i++) {
          waloff[SMKPICNUM0+i] = 0;
          if (smkframe[i]) free(smkframe[i]);
          smkframe[i] = NULL;
     }
}
