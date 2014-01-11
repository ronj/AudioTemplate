/*
 *  mpadec - MPEG audio decoder
 *  Copyright (C) 2002-2004 Dmitriy Startsev (dstartsev@rambler.ru)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* $Id: main.c,v 1.6 2010/06/08 14:59:30 jpff Exp $ */
#include <float.h>
#include <time.h>
#include "mp3dec.h"
#include "output.h"
#ifdef HAVE_CONIO_H
#include <conio.h>
#endif

int quiet = 0;
int skip = -1;
int fd = -1;
void *out = NULL;
mp3dec_t mpa = NULL;

char *input_file = NULL;

mpadec_config_t config = { MPADEC_CONFIG_FULL_QUALITY, MPADEC_CONFIG_AUTO,
                           MPADEC_CONFIG_32BIT, MPADEC_CONFIG_LITTLE_ENDIAN,
                           MPADEC_CONFIG_REPLAYGAIN_NONE, TRUE, TRUE, TRUE, 0.0 };

static void parse_args(int argc, char *argv[])
{
  int i = 1; char *arg;

  while (i < argc) {
    arg = argv[i++];
    if ((arg[0] == '-') && !arg[1]) {
      if (!input_file) input_file = (void *)1;
    }
    else if (*arg == '-') {
      arg++;
      if (!strcasecmp(arg, "quiet")) quiet = 1;
      else if (!strcasecmp(arg, "auto")) config.mode = MPADEC_CONFIG_AUTO;
      else if (!strcasecmp(arg, "mono")) config.mode = MPADEC_CONFIG_MONO;
      else if (!strcasecmp(arg, "stereo")) config.mode = MPADEC_CONFIG_STEREO;
      else if (!strcasecmp(arg, "channel1") || !strcasecmp(arg, "left"))
        config.mode = MPADEC_CONFIG_CHANNEL1;
      else if (!strcasecmp(arg, "channel2") || !strcasecmp(arg, "right"))
        config.mode = MPADEC_CONFIG_CHANNEL2;
      else if (!strcasecmp(arg, "noskip")) config.skip = FALSE;
    } else {
        if (!input_file) input_file = arg;
    }
  }
}

static void print_help(char *progname)
{
  char *s, temp[256];
  static char dflt[] = " (default)";

  s = strrchr(progname, '\\');
  if (!s) s = strrchr(progname, '/');
  if (!s) s = progname; else s++;
  printf("Usage: %s [options] <input file>\n", s);
  puts("  Options:");
  puts("    -quiet                do not display any messages");
  puts("    -skip n               skip first n samples");
  strcpy(temp, "    -auto                 decode all channels");
  if (config.mode == MPADEC_CONFIG_AUTO) strcat(temp, dflt);
  puts(temp);
  strcpy(temp, "    -mono                 downmix to mono");
  if (config.mode == MPADEC_CONFIG_MONO) strcat(temp, dflt);
  puts(temp);
  strcpy(temp, "    -stereo               force stereo output");
  if (config.mode == MPADEC_CONFIG_STEREO) strcat(temp, dflt);
  puts(temp);
  strcpy(temp, "    -channel1 or -left    decode first (left) channel only");
  if (config.mode == MPADEC_CONFIG_CHANNEL1) strcat(temp, dflt);
  puts(temp);
  strcpy(temp, "    -channel2 or -right   decode second (right) channel only");
  if (config.mode == MPADEC_CONFIG_CHANNEL2) strcat(temp, dflt);
  puts(temp);
}

static void cleanup(void)
{
  if ((fd >= 0) && (input_file != (void *)1)) close(fd);
  if (mpa) mp3dec_uninit(mpa);
  fd = -1;
  out = mpa = NULL;
  if (!quiet) fprintf(stderr, "\n");
}

int main(int argc, char **argv)
{
  int r; time_t t;
  mpadec_info_t mpainfo;
  uint32_t bufsize, bufused = 0;
  uint64_t progress = 0, maxsize;
  char *s, temp[256];
  static char out_name[256];
  static uint8_t buffer[0x10000];

  quiet = 0;
  fd = skip = -1;
  out = mpa = NULL;
  input_file = NULL;
  *((int32_t *)temp) = 1;
  parse_args(argc, argv);
  if (!quiet)
    fprintf((input_file ? stderr : stdout),
            "\nMPADEC v%d.%d - Copyright (C) 2002-2004, Dmitriy Startsev\n\n",
            ((MPADEC_VERSION >> 16) & 0xFF), ((MPADEC_VERSION >> 8) & 0xFF));
  if (!input_file) {
    if (!quiet) print_help(argv[0]);
    exit(0);
  }
  atexit(cleanup);
  mpa = mp3dec_init();
  if (!mpa) {
    if (!quiet) fputs("Not enough memory\n", stderr);
    exit(1);
  }
  if ((r = mp3dec_configure(mpa, &config)) != MP3DEC_RETCODE_OK) {
    if (!quiet) fputs(mp3dec_error(r), stderr);
    exit(1);
  }
  if (input_file == (void *)1) fd = fileno(stdin);
  else fd = open(input_file, O_RDONLY | O_BINARY);
  if (fd < 0) {
    if (!quiet) perror(input_file);
    exit(1);
  }
  if ((r = mp3dec_init_file(mpa, fd, 0, FALSE)) != MP3DEC_RETCODE_OK) {
    if (!quiet) fputs(mp3dec_error(r), stderr);
    exit(1);
  } 
  else fd = -1;
  if ((r = mp3dec_get_info(mpa, &mpainfo, MPADEC_INFO_STREAM)) !=
      MP3DEC_RETCODE_OK) {
    if (!quiet) fputs(mp3dec_error(r), stderr);
    exit(1);
  }

  maxsize = mpainfo.decoded_sample_size
            *mpainfo.decoded_frame_samples
            *mpainfo.frames;
  if (!quiet) {
    if (mpainfo.frequency < 16000) strcpy(temp, "MPEG-2.5 ");
    else if (mpainfo.frequency < 32000) strcpy(temp, "MPEG-2 "); else strcpy(temp, "MPEG-1 ");
    if (mpainfo.layer == 1) strcat(temp, "Layer I");
    else if (mpainfo.layer == 2) strcat(temp, "Layer II"); else strcat(temp, "Layer III");
    fprintf(stderr, "Input:  %s, %s, %d kbps, %d Hz  (%d:%02d)\n",
            temp, ((mpainfo.channels > 1) ? "stereo" : "mono"),
            mpainfo.bitrate, mpainfo.frequency, mpainfo.duration/60,
            mpainfo.duration%60);
  }
  bufsize = sizeof(buffer)/mpainfo.decoded_sample_size;
  if (!quiet) fprintf(stderr, "\n\rDecoding... %d%%", 0);
  t = time(NULL);
  if (skip > 0) {
    if ((uint32_t)skip > bufsize) skip = bufsize;
    mp3dec_decode(mpa, buffer, mpainfo.decoded_sample_size*skip, &bufused);
  }
  bufsize *= mpainfo.decoded_sample_size;
  r = mp3dec_decode(mpa, buffer, bufsize, &bufused);
  while ((r == MP3DEC_RETCODE_OK) && bufused) {
    int i = bufused;
    int *bb = (int*)buffer;
    float *ff = (float*)buffer;
    printf("\n");
    /*for (i=0; i<bufused; i++)  printf("%6d ", bb[i]/0x10000); */
    /* for (i=0; i<bufused; i++)  printf("%f (%d) ", ff[i], (int)(0x7fff * ff[i])); */
    printf("\n");
    if (i <= 0) break;
    progress += bufused;
    if (!quiet) fprintf(stderr, "\rDecoding... %d%%", (int)(100*progress/maxsize));
    r = mp3dec_decode(mpa, buffer, bufsize, &bufused);
  }
  t = time(NULL) - t;
  if (!quiet) {
    fprintf(stderr, "\rDecoding... done\n");
    fprintf(stderr, "Decode time: %d:%02d\n", t/60, t%60);
    if (r > MP3DEC_RETCODE_OK) fputs(mp3dec_error(r), stderr);
  }
  out = NULL;
  mp3dec_uninit(mpa);
  mpa = NULL;
  return ((r == MP3DEC_RETCODE_OK) ? 0 : 1);
}
