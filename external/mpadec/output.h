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

/* $Id: output.h,v 1.7 2004/08/03 05:22:22 metal_man Exp $ */

#ifndef __OUTPUT_H
#define __OUTPUT_H

#pragma pack(1)

#ifdef WIN32
#undef OSS
#endif

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM        1
#endif
#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT 3
#endif

struct audio_out {
  void *(*open)(char *filename, int frequency, int channels, int format, int endian, uint64_t size);
  int (*write)(void *handle, void *buffer, int size);
  int (*close)(void *handle);
};

struct wave_header {
  uint8_t riff_id[4];
  uint8_t riff_size[4];
  uint8_t wave_id[4];
  uint8_t format_id[4];
  uint8_t format_size[4];
  struct {
    uint8_t wFormatTag[2];
    uint8_t nChannels[2];
    uint8_t nSamplesPerSec[4];
    uint8_t nAvgBytesPerSec[4];
    uint8_t nBlockAlign[2];
    uint8_t wBitsPerSample[2];
    uint8_t cbSize[2];
  } PACKED fmt;
  struct {
    uint8_t data_id[4];
    uint8_t data_size[4];
  } PACKED data_chunk;
} PACKED;

struct aiff_header {
  uint8_t form_id[4];
  uint8_t form_size[4];
  uint8_t aiff_id[4];
  uint8_t comm_id[4];
  uint8_t comm_size[4];
  struct {
    uint8_t channels[2];
    uint8_t samples[4];
    uint8_t sample_size[2];
    uint8_t frequency[10];
  } PACKED fmt;
  uint8_t ssnd_id[4];
  uint8_t ssnd_size[4];
  uint8_t offset[4];
  uint8_t block_size[4];
} PACKED;

struct wave_out {
  int fd;
  char *filename;
  uint64_t count;
  struct wave_header header;
};

struct aiff_out {
  int fd;
  char *filename;
  uint64_t count;
  struct aiff_header header;
};

struct raw_out {
  int fd;
  char *filename;
  uint64_t count;
};

struct audio_out aiff_out;
struct audio_out wave_out;
struct audio_out raw_out;

#ifdef OSS

struct oss_out {
  int fd;
  char *filename;
};

#ifndef AFMT_S32_LE
#define AFMT_S32_LE 0x00001000
#endif
#ifndef AFMT_S32_BE
#define AFMT_S32_BE 0x00002000
#endif

struct audio_out oss_out;

#endif

#define STORE16LE(a,b) {                                                             \
                         ((uint8_t *)a)[0] = (uint8_t)((uint16_t)(b) & 0xFF);        \
                         ((uint8_t *)a)[1] = (uint8_t)(((uint16_t)(b) >> 8)& 0xFF);  \
                       }

#define STORE32LE(a,b) {                                                             \
                         ((uint8_t *)a)[0] = (uint8_t)((uint32_t)(b) & 0xFF);        \
                         ((uint8_t *)a)[1] = (uint8_t)(((uint32_t)(b) >> 8)& 0xFF);  \
                         ((uint8_t *)a)[2] = (uint8_t)(((uint32_t)(b) >> 16)& 0xFF); \
                         ((uint8_t *)a)[3] = (uint8_t)(((uint32_t)(b) >> 24)& 0xFF); \
                       }

#define STORE16BE(a,b) {                                                             \
                         ((uint8_t *)a)[0] = (uint8_t)(((uint16_t)(b) >> 8)& 0xFF);  \
                         ((uint8_t *)a)[1] = (uint8_t)((uint16_t)(b) & 0xFF);        \
                       }

#define STORE32BE(a,b) {                                                             \
                         ((uint8_t *)a)[0] = (uint8_t)(((uint32_t)(b) >> 24)& 0xFF); \
                         ((uint8_t *)a)[1] = (uint8_t)(((uint32_t)(b) >> 16)& 0xFF); \
                         ((uint8_t *)a)[2] = (uint8_t)(((uint32_t)(b) >> 8)& 0xFF);  \
                         ((uint8_t *)a)[3] = (uint8_t)((uint32_t)(b) & 0xFF);        \
                       }

#define LOAD16LE(a) ((uint16_t)(((uint8_t *)a)[1] << 8) | ((uint8_t *)a)[0])
#define LOAD16BE(a) ((uint16_t)(((uint8_t *)a)[0] << 8) | ((uint8_t *)a)[1])
#define LOAD32LE(a) ((uint32_t)(((uint8_t *)a)[3] << 24) | (((uint8_t *)a)[2] << 16) | (((uint8_t *)a)[1] << 8) | ((uint8_t *)a)[0])
#define LOAD32BE(a) ((uint32_t)(((uint8_t *)a)[0] << 24) | (((uint8_t *)a)[1] << 16) | (((uint8_t *)a)[2] << 8) | ((uint8_t *)a)[3])

#pragma pack()

#endif

