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

/* $Id: output.c,v 1.5 2010/06/08 14:59:30 jpff Exp $ */

#include "mp3dec.h"
#include "output.h"
#include <sys/stat.h>
#ifdef OSS
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#endif

extern int quiet;

static struct wave_header wave_header = { "RIFF", { 0, }, "WAVE", "fmt ", { 0, }, { { 0, }, { 0, }, { 0, }, { 0, }, { 0, }, { 0, }, { 0, } }, { "data", { 0, } } };

static void *wave_open(char *filename, int freq, int channels, int format, int endian, uint64_t size)
{
  struct wave_out *wave_out = (struct wave_out *)malloc(sizeof(struct wave_out) + (filename ? (strlen(filename) + 1) : 0));
  int r;

  if (endian != MPADEC_CONFIG_LITTLE_ENDIAN) {
    if (!quiet) fputs("Unsupported sample format\n", stderr);
    return NULL;
  }
  if (!wave_out) {
    if (!quiet) fputs("Not enough memory\n", stderr);
    return NULL;
  }
  wave_out->fd = -1;
  if (size > (0x7FFFFFFF - sizeof(wave_header))) size = (0x7FFFFFFF - sizeof(wave_header));
  if (filename) {
    wave_out->filename = (char *)wave_out + sizeof(struct wave_out);
    strcpy(wave_out->filename, filename);
    if ((wave_out->fd = open(wave_out->filename, O_WRONLY | O_BINARY | O_CREAT | O_EXCL, S_IREAD | S_IWRITE)) < 0) {
      if (!quiet) perror(wave_out->filename);
      free(wave_out);
      return NULL;
    }
  } else {
    wave_out->filename = NULL;
    wave_out->fd = fileno(stdout);
  }
  memcpy(&wave_out->header, &wave_header, sizeof(wave_out->header));
  STORE16LE(&wave_out->header.fmt.wFormatTag, ((format == MPADEC_CONFIG_FLOAT) ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM));
  STORE16LE(&wave_out->header.fmt.nChannels, channels);
  STORE32LE(&wave_out->header.fmt.nSamplesPerSec, freq);
  switch (format) {
    case MPADEC_CONFIG_24BIT: STORE16LE(&wave_out->header.fmt.wBitsPerSample, 24); break;
    case MPADEC_CONFIG_32BIT:
    case MPADEC_CONFIG_FLOAT: STORE16LE(&wave_out->header.fmt.wBitsPerSample, 32); break;
    default:                  STORE16LE(&wave_out->header.fmt.wBitsPerSample, 16);
  }
  STORE16LE(&wave_out->header.fmt.nBlockAlign, (channels*((LOAD16LE(&wave_out->header.fmt.wBitsPerSample) + 7) >> 3)));
  STORE32LE(&wave_out->header.fmt.nAvgBytesPerSec, LOAD16LE(&wave_out->header.fmt.nBlockAlign)*freq);
  STORE16LE(&wave_out->header.fmt.cbSize, 0);
  STORE32LE(&wave_out->header.data_chunk.data_size, (uint32_t)size);
  if (LOAD16LE(&wave_out->header.fmt.wFormatTag) == WAVE_FORMAT_IEEE_FLOAT) {
    STORE32LE(&wave_out->header.riff_size, (sizeof(wave_out->header) - 2*sizeof(uint32_t) + (uint32_t)size));
    STORE32LE(&wave_out->header.format_size, sizeof(wave_out->header.fmt));
    r = write(wave_out->fd, &wave_out->header, sizeof(wave_out->header));
  } else {
    uint32_t wsize = sizeof(wave_out->header) - sizeof(wave_out->header.data_chunk) - sizeof(uint16_t);
    STORE32LE(&wave_out->header.riff_size, (sizeof(wave_out->header) - 2*sizeof(uint32_t) - sizeof(uint16_t) + (uint32_t)size));
    STORE32LE(&wave_out->header.format_size, (sizeof(wave_out->header.fmt) - sizeof(uint16_t)));
    write(wave_out->fd, &wave_out->header, wsize);
    r = write(wave_out->fd, &wave_out->header.data_chunk, sizeof(wave_out->header.data_chunk));
  }
  if (r < 0) {
    if (!quiet) perror((wave_out->filename ? wave_out->filename : "stdout"));
    if (wave_out->filename) {
      close(wave_out->fd);
      unlink(wave_out->filename);
    }
    free(wave_out);
    return NULL;
  }
  wave_out->count = 0;
  return wave_out;
}

static int wave_write(void *handle, void *buffer, int size)
{
  struct wave_out *wave_out = (struct wave_out *)handle;
  int r = 0;

  if (wave_out) {
    r = write(wave_out->fd, buffer, size);
    if ((r < 0) && !quiet) perror((wave_out->filename ? wave_out->filename : "stdout"));
    else wave_out->count += r;
  }
  return r;
}

static int wave_close(void *handle)
{
  struct wave_out *wave_out = (struct wave_out *)handle;
  int r = 0;

  if (wave_out) {
    if (wave_out->filename) {
      if (wave_out->count) {
        int r = lseek(wave_out->fd, 0, SEEK_SET);
        if (wave_out->count > (0x7FFFFFFF - sizeof(wave_header))) wave_out->count = (0x7FFFFFFF - sizeof(wave_header));
        STORE32LE(&wave_out->header.data_chunk.data_size, (uint32_t)wave_out->count);
        if (LOAD16LE(&wave_out->header.fmt.wFormatTag) == WAVE_FORMAT_IEEE_FLOAT) {
          wave_out->count += (sizeof(wave_out->header) - 2*sizeof(uint32_t));
        } else wave_out->count += (sizeof(wave_out->header) - 2*sizeof(uint32_t) - sizeof(uint16_t));
        STORE32LE(&wave_out->header.riff_size, (uint32_t)wave_out->count);
        if (r && !quiet) perror((wave_out->filename ? wave_out->filename : "stdout"));
        else {
          if (LOAD16LE(&wave_out->header.fmt.wFormatTag) == WAVE_FORMAT_IEEE_FLOAT) {
            write(wave_out->fd, &wave_out->header, sizeof(wave_out->header));
          } else {
            uint32_t wsize = sizeof(wave_out->header) - sizeof(wave_out->header.data_chunk) - 2;
            write(wave_out->fd, &wave_out->header, wsize);
            write(wave_out->fd, &wave_out->header.data_chunk, sizeof(wave_out->header.data_chunk));
          }
        }
        close(wave_out->fd);
      } else {
        close(wave_out->fd);
        unlink(wave_out->filename);
      }
    }
    free(wave_out);
  }
  return r;
}

static void *raw_open(char *filename, int freq, int channels, int format, int endian, uint64_t size)
{
  struct raw_out *raw_out = (struct raw_out *)malloc(sizeof(struct raw_out) + (filename ? (strlen(filename) + 1) : 0));

  if (!raw_out) {
    if (!quiet) fputs("Not enough memory\n", stderr);
    return NULL;
  }
  raw_out->fd = -1;
  if (size > 0x7FFFFFFF) size = 0x7FFFFFFF;
  if (filename) {
    raw_out->filename = (char *)raw_out + sizeof(struct raw_out);
    strcpy(raw_out->filename, filename);
    if ((raw_out->fd = open(raw_out->filename, O_WRONLY | O_BINARY | O_CREAT | O_EXCL, S_IREAD | S_IWRITE)) < 0) {
      if (!quiet) perror(raw_out->filename);
      free(raw_out);
      return NULL;
    }
  } else {
    raw_out->filename = NULL;
    raw_out->fd = fileno(stdout);
  }
  raw_out->count = 0;
  return raw_out;
}

static int raw_write(void *handle, void *buffer, int size)
{
  struct raw_out *raw_out = (struct raw_out *)handle;
  int r = 0;

  if (raw_out) {
    r = write(raw_out->fd, buffer, size);
    if ((r < 0) && !quiet) perror((raw_out->filename ? raw_out->filename : "stdout"));
    else raw_out->count += r;
  }
  return r;
}

static int raw_close(void *handle)
{
  struct raw_out *raw_out = (struct raw_out *)handle;

  if (raw_out) {
    if (raw_out->filename) {
      close(raw_out->fd);
      if (!raw_out->count) unlink(raw_out->filename);
    }
    free(raw_out);
  }
  return 0;
}

struct audio_out wave_out = { wave_open, wave_write, wave_close };
struct audio_out raw_out = { raw_open, raw_write, raw_close };

