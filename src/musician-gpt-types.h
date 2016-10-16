/* musician-gpt-types.h
 *
 * Copyright (C) 2016 Christian Hergert <chergert@redhat.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MUSICIAN_GPT_TYPES_H
#define MUSICIAN_GPT_TYPES_H

#include <glib-object.h>

#include "musician-enums.h"

G_BEGIN_DECLS

typedef struct _MusicianGptSong    MusicianGptSong;
typedef struct _MusicianGptTrack   MusicianGptTrack;
typedef struct _MusicianGptMeasure MusicianGptMeasure;

typedef gint32 MusicianGptNote;
typedef gint32 MusicianGptTuning;

typedef enum
{
  MUSICIAN_GPT_KEY_C        = 0,

  MUSICIAN_GPT_KEY_G        = 1,   /* 1 Sharp  */
  MUSICIAN_GPT_KEY_D        = 2,   /* 2 Sharps */
  MUSICIAN_GPT_KEY_A        = 3,   /* 3 Sharps */
  MUSICIAN_GPT_KEY_E        = 4,   /* 4 Sharps */
  MUSICIAN_GPT_KEY_B        = 5,   /* 5 Sharps */
  MUSICIAN_GPT_KEY_F_SHARP  = 6,   /* 6 Sharps */
  MUSICIAN_GPT_KEY_C_SHARP  = 7,   /* 7 Sharps */

  MUSICIAN_GPT_KEY_F        = -1,  /* 1 Flat */
  MUSICIAN_GPT_KEY_B_FLAT   = -2,  /* 2 Flats */
  MUSICIAN_GPT_KEY_E_FLAT   = -3,  /* 3 Flats */
  MUSICIAN_GPT_KEY_A_FLAT   = -4,  /* 4 Flats */
  MUSICIAN_GPT_KEY_D_FLAT   = -5,  /* 5 Flats */
  MUSICIAN_GPT_KEY_G_FLAT   = -6,  /* 6 Flats */
  MUSICIAN_GPT_KEY_C_FLAT   = -7,  /* 7 Flats */
} MusicianGptKey;

typedef enum
{
  MUSICIAN_GPT_MEASURE_FLAGS_NONE              = 0,
  MUSICIAN_GPT_MEASURE_FLAGS_KEY_NUMERATOR     = 1 << 0,
  MUSICIAN_GPT_MEASURE_FLAGS_KEY_DENOMINATOR   = 1 << 1,
  MUSICIAN_GPT_MEASURE_FLAGS_REPEAT_BEGIN      = 1 << 2,
  MUSICIAN_GPT_MEASURE_FLAGS_REPEAT_END        = 1 << 3,
  MUSICIAN_GPT_MEASURE_FLAGS_ALTERNATE_ENDING  = 1 << 4,
  MUSICIAN_GPT_MEASURE_FLAGS_MARKER            = 1 << 5,
  MUSICIAN_GPT_MEASURE_FLAGS_TONALITY          = 1 << 6,
  MUSICIAN_GPT_MEASURE_FLAGS_DOUBLE_BAR        = 1 << 7,
} MusicianGptMeasureFlags;

typedef enum
{
  MUSICIAN_GPT_TRACK_FLAGS_NONE          = 0,
  MUSICIAN_GPT_TRACK_FLAGS_DRUMS_TRACK   = 1 << 0,
  MUSICIAN_GPT_TRACK_FLAGS_TWELVE_STRING = 1 << 1,
  MUSICIAN_GPT_TRACK_FLAGS_BANJO_TRACK   = 1 << 2,
} MusicianGptTrackFlags;

typedef enum
{
  MUSICIAN_GPT_BEAT_FLAGS_NONE          = 0,
  MUSICIAN_GPT_BEAT_FLAGS_DOTTED        = 1 << 0,
  MUSICIAN_GPT_BEAT_FLAGS_CHORD_DIAGRAM = 1 << 1,
  MUSICIAN_GPT_BEAT_FLAGS_TEXT          = 1 << 2,
  MUSICIAN_GPT_BEAT_FLAGS_EFFECTS       = 1 << 3,
  MUSICIAN_GPT_BEAT_FLAGS_MIX_TABLE     = 1 << 4,
  MUSICIAN_GPT_BEAT_FLAGS_N_TUPLET      = 1 << 5,
  MUSICIAN_GPT_BEAT_FLAGS_STATUS        = 1 << 6,
} MusicianGptBeatFlags;

typedef enum
{
  MUSICIAN_GPT_OCTAVE_NONE,
  MUSICIAN_GPT_OCTAVE_EIGHTVA = 8,
} MusicianGptOctave;

typedef enum
{
  MUSICIAN_GPT_TRIPLET_FEEL_NONE,
  MUSICIAN_GPT_TRIPLET_FEEL_EIGHTH,
} MusicianGptTripletFeel;

typedef struct
{
  guint32 port_id;
  guint32 channel_id;
  guint32 instrument;
  guint8  volume;
  guint8  balance;
  guint8  chorus;
  guint8  reverb;
  guint8  phaser;
  guint8  tremelo;
  guint8  _blank1;
  guint8  _blank2;
} MusicianGptMidiChannel;

typedef struct
{
  guint32                port_id;
  MusicianGptMidiChannel channels[16];
} MusicianGptMidiPort;

G_END_DECLS

#endif /* MUSICIAN_GPT_TYPES_H */
