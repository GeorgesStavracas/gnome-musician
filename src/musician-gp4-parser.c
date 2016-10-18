/* musician-gp4-parser.c
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

#define G_LOG_DOMAIN "musician-gp4-parser"

#include "musician-gpt-beat.h"
#include "musician-gpt-bend.h"
#include "musician-gpt-chord.h"
#include "musician-gpt-measure.h"
#include "musician-gp4-parser.h"
#include "musician-gpt-song.h"
#include "musician-gpt-song-private.h"
#include "musician-gpt-track.h"

struct _MusicianGp4Parser
{
  MusicianGptParser parent_instance;
};

G_DEFINE_TYPE (MusicianGp4Parser, musician_gp4_parser, MUSICIAN_TYPE_GPT_PARSER)

static gboolean
musician_gp4_parser_load_attributes (MusicianGp4Parser       *self,
                                     MusicianGptInputStream  *stream,
                                     MusicianGptSong         *song,
                                     GCancellable            *cancellable,
                                     GError                 **error)
{
  g_autofree gchar *album = NULL;
  g_autofree gchar *artist = NULL;
  g_autofree gchar *copyright = NULL;
  g_autofree gchar *interpret = NULL;
  g_autofree gchar *instructions = NULL;
  g_autofree gchar *subtitle = NULL;
  g_autofree gchar *writer = NULL;
  g_autofree gchar *title = NULL;
  g_auto(GStrv) comments = NULL;
  gint32 n_comments;

  g_assert (MUSICIAN_IS_GP4_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (MUSICIAN_IS_GPT_SONG (song));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  /* Read the title of the piece */
  if (NULL == (title = musician_gpt_input_stream_read_string (stream, cancellable, error)))
    return FALSE;

  /* Read the subtitle of the piece */
  if (NULL == (subtitle = musician_gpt_input_stream_read_string (stream, cancellable, error)))
    return FALSE;

  /* Read the interpret of the piece */
  if (NULL == (interpret = musician_gpt_input_stream_read_string (stream, cancellable, error)))
    return FALSE;

  /* Read the album of the piece */
  if (NULL == (album = musician_gpt_input_stream_read_string (stream, cancellable, error)))
    return FALSE;

  /* Read the artist of the piece */
  if (NULL == (artist = musician_gpt_input_stream_read_string (stream, cancellable, error)))
    return FALSE;

  /* Read the copyright of the piece */
  if (NULL == (copyright = musician_gpt_input_stream_read_string (stream, cancellable, error)))
    return FALSE;

  /* Read the tab_author of the piece */
  if (NULL == (writer = musician_gpt_input_stream_read_string (stream, cancellable, error)))
    return FALSE;

  /* Read the instructions of the piece */
  if (NULL == (instructions = musician_gpt_input_stream_read_string (stream, cancellable, error)))
    return FALSE;

  comments = musician_gpt_input_stream_read_string_array (stream, cancellable, error);

  musician_gpt_song_set_title (song, title);
  musician_gpt_song_set_subtitle (song, subtitle);
  musician_gpt_song_set_interpretation (song, interpret);
  musician_gpt_song_set_album (song, album);
  musician_gpt_song_set_artist (song, artist);
  musician_gpt_song_set_copyright (song, copyright);
  musician_gpt_song_set_writer (song, writer);
  musician_gpt_song_set_instructions (song, instructions);

  return TRUE;
}

static gboolean
musician_gp4_parser_load_lyrics (MusicianGp4Parser       *self,
                                 MusicianGptInputStream  *stream,
                                 MusicianGptSong         *song,
                                 GCancellable            *cancellable,
                                 GError                 **error)
{
  guint32 track_num = 0;

  g_assert (MUSICIAN_IS_GP4_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (MUSICIAN_IS_GPT_SONG (song));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &track_num, error))
    return FALSE;

  for (guint i = 0; i < 5; i++)
    {
      g_autofree gchar *lyrics = NULL;
      guint32 position = 0;

      if (NULL == (lyrics = musician_gpt_input_stream_read_lyric (stream, cancellable, &position, error)))
        return FALSE;

      _musician_gpt_song_add_lyrics (song, position, lyrics);
    }

  return TRUE;
}

static gboolean
musician_gp4_parser_load_midi_ports (MusicianGp4Parser       *self,
                                     MusicianGptInputStream  *stream,
                                     MusicianGptSong         *song,
                                     GCancellable            *cancellable,
                                     GError                 **error)
{
  MusicianGptMidiPort ports[4];

  g_assert (MUSICIAN_IS_GP4_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (MUSICIAN_IS_GPT_SONG (song));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  for (guint i = 0; i < 4; i++)
    {
      if (!musician_gpt_input_stream_read_midi_port (stream, i + 1, cancellable, &ports[i], error))
        return FALSE;
    }

  _musician_gpt_song_set_midi_ports (song, ports, G_N_ELEMENTS (ports));

  return TRUE;
}

static gboolean
musician_gp4_parser_load_measures (MusicianGp4Parser       *self,
                                   MusicianGptInputStream  *stream,
                                   MusicianGptSong         *song,
                                   guint                    n_measures,
                                   GCancellable            *cancellable,
                                   GError                 **error)
{
  g_assert (MUSICIAN_IS_GP4_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (MUSICIAN_IS_GPT_SONG (song));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  for (guint i = 0; i < n_measures; i++)
    {
      g_autoptr(MusicianGptMeasure) measure = NULL;
      MusicianGptMeasureFlags flags;
      guint8 header;
      guint8 numerator = 0;
      guint8 denominator = 0;
      guint8 n_repeats = 0;
      guint8 nth_ending = 0;
      guint8 key = 0;

      if (!musician_gpt_input_stream_read_byte (stream, cancellable, &header, error))
        return FALSE;

      flags = header;

      measure = musician_gpt_measure_new ();

      musician_gpt_measure_set_id (measure, i + 1);

      if (flags & MUSICIAN_GPT_MEASURE_FLAGS_KEY_NUMERATOR)
        {
          if (!musician_gpt_input_stream_read_byte (stream, cancellable, &numerator, error))
            return FALSE;
          musician_gpt_measure_set_numerator (measure, numerator);
        }

      if (flags & MUSICIAN_GPT_MEASURE_FLAGS_KEY_DENOMINATOR)
        {
          if (!musician_gpt_input_stream_read_byte (stream, cancellable, &denominator, error))
            return FALSE;
          musician_gpt_measure_set_denominator (measure, denominator);
        }

      if (flags & MUSICIAN_GPT_MEASURE_FLAGS_REPEAT_END)
        {
          if (!musician_gpt_input_stream_read_byte (stream, cancellable, &n_repeats, error))
            return FALSE;
          musician_gpt_measure_set_nth_ending (measure, nth_ending);
        }

      if (flags & MUSICIAN_GPT_MEASURE_FLAGS_ALTERNATE_ENDING)
        {
          if (!musician_gpt_input_stream_read_byte (stream, cancellable, &nth_ending, error))
            return FALSE;
          musician_gpt_measure_set_n_repeats (measure, n_repeats);
        }

      if (flags & MUSICIAN_GPT_MEASURE_FLAGS_MARKER)
        {
          g_autofree gchar *name = NULL;
          GdkRGBA color;

          if (NULL == (name = musician_gpt_input_stream_read_string (stream, cancellable, error)))
            return FALSE;

          if (!musician_gpt_input_stream_read_color (stream, cancellable, &color, error))
            return FALSE;

          musician_gpt_measure_set_marker_name (measure, name);
          musician_gpt_measure_set_marker_color (measure, &color);
        }

      if (flags & MUSICIAN_GPT_MEASURE_FLAGS_TONALITY)
        {
          if (!musician_gpt_input_stream_read_byte (stream, cancellable, &key, error) ||
              !musician_gpt_input_stream_read_byte (stream, cancellable, NULL, error))
            return FALSE;
          musician_gpt_measure_set_key (measure, key);
        }

      musician_gpt_song_add_measure (song, measure);
    }

  return TRUE;
}

static gboolean
musician_gp4_parser_load_tracks (MusicianGp4Parser       *self,
                                 MusicianGptInputStream  *stream,
                                 MusicianGptSong         *song,
                                 guint                    n_tracks,
                                 GCancellable            *cancellable,
                                 GError                 **error)
{
  g_assert (MUSICIAN_IS_GP4_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (MUSICIAN_IS_GPT_SONG (song));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  for (guint i = 0; i < n_tracks; i++)
    {
      g_autofree gchar *title = NULL;
      g_autoptr(MusicianGptTrack) track = NULL;
      MusicianGptTrackFlags flags;
      GdkRGBA color;
      guint32 n_strings;
      guint32 port;
      guint32 channel;
      guint32 effects_channel;
      guint32 n_frets;
      guint32 capo_at;
      gint32 tunings[7];
      guint8 header;

      if (!musician_gpt_input_stream_read_byte (stream, cancellable, &header, error))
        return FALSE;

      flags = header;

      if (NULL == (title = musician_gpt_input_stream_read_fixed_string (stream, 40, cancellable, error)))
        return FALSE;

      if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &n_strings, error))
        return FALSE;

      for (guint j = 0; j < G_N_ELEMENTS (tunings); j++)
        {
          if (!musician_gpt_input_stream_read_int32 (stream, cancellable, &tunings[j], error))
            return FALSE;
        }

      if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &port, error) ||
          !musician_gpt_input_stream_read_uint32 (stream, cancellable, &channel, error) ||
          !musician_gpt_input_stream_read_uint32 (stream, cancellable, &effects_channel, error) ||
          !musician_gpt_input_stream_read_uint32 (stream, cancellable, &n_frets, error) ||
          !musician_gpt_input_stream_read_uint32 (stream, cancellable, &capo_at, error) ||
          !musician_gpt_input_stream_read_color (stream, cancellable, &color, error))
        return FALSE;

      track = musician_gpt_track_new ();

      musician_gpt_track_set_capo_at (track, capo_at);
      musician_gpt_track_set_channel (track, channel);
      musician_gpt_track_set_color (track, &color);
      musician_gpt_track_set_effects_channel (track, effects_channel);
      musician_gpt_track_set_id (track, i + 1);
      musician_gpt_track_set_n_frets (track, n_frets);
      musician_gpt_track_set_port (track, port);
      musician_gpt_track_set_title (track, title);
      musician_gpt_track_set_tunings (track, tunings, G_N_ELEMENTS (tunings));

      musician_gpt_song_add_track (song, track);
    }

  return TRUE;
}

static gboolean
musician_gp4_parser_load_chord (MusicianGp4Parser       *self,
                                MusicianGptInputStream  *stream,
                                GCancellable            *cancellable,
                                MusicianGptChord       **chord_out,
                                GError                 **error)
{
  g_autofree gchar *name = NULL;
  g_autoptr(MusicianGptChord) chord = NULL;
  guint8 header;
  guint8 sharp;
  guint8 root;
  guint8 chord_type;
  guint8 breadth;
  guint8 has_add;
  guint8 fifth;
  guint8 ninth;
  guint8 eleventh;
  guint8 omission1;
  guint8 omission3;
  guint8 omission5;
  guint8 omission7;
  guint8 omission9;
  guint8 omission11;
  guint8 omission13;
  guint8 n_barres;
  guint8 barre_frets[5];
  guint8 barre_start[5];
  guint8 barre_end[5];
  guint8 fingering[7];
  guint8 display;
  gint32 lowest;
  gint32 tonality;
  guint32 bass_fret;
  guint32 frets[7];

  g_assert (MUSICIAN_IS_GP4_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  chord = musician_gpt_chord_new ();

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &header, error))
    return FALSE;

  if (header != 1)
    {
      g_set_error (error,
                   G_IO_ERROR,
                   G_IO_ERROR_NOT_SUPPORTED,
                   "Unsupported chord format: %d",
                   header);
      return FALSE;
    }

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &sharp, error))
    return FALSE;

  g_print ("%d sharp or flat\n", sharp);

  if (!g_input_stream_skip (G_INPUT_STREAM (stream), 3, cancellable, error))
    return FALSE;

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &root, error))
    return FALSE;

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &chord_type, error))
    return FALSE;

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &breadth, error))
    return FALSE;

  if (!musician_gpt_input_stream_read_int32 (stream, cancellable, &lowest, error))
    return FALSE;

  if (!musician_gpt_input_stream_read_int32 (stream, cancellable, &tonality, error))
    return FALSE;

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &has_add, error))
    return FALSE;

  if (NULL != (name = musician_gpt_input_stream_read_fixed_string (stream, 20, cancellable, error)))
    return FALSE;

  if (!g_input_stream_skip (G_INPUT_STREAM (stream), 2, cancellable, error))
    return FALSE;

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &fifth, error))
    return FALSE;

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &ninth, error))
    return FALSE;

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &eleventh, error))
    return FALSE;

  if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &bass_fret, error))
    return FALSE;

  for (guint i = 0; i < G_N_ELEMENTS (frets); i++)
    {
      if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &frets[i], error))
        return FALSE;
    }

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &n_barres, error))
    return FALSE;

  if (n_barres > 5)
    {
      g_set_error (error,
                   G_IO_ERROR,
                   G_IO_ERROR_INVALID_DATA,
                   "Invalid value for number of barres: %d", n_barres);
      return FALSE;
    }

  for (guint i = 0; i < n_barres; i++)
    {
      if (!musician_gpt_input_stream_read_byte (stream, cancellable, &barre_frets[i], error))
        return FALSE;
    }

  for (guint i = 0; i < n_barres; i++)
    {
      if (!musician_gpt_input_stream_read_byte (stream, cancellable, &barre_start[i], error))
        return FALSE;
    }

  for (guint i = 0; i < n_barres; i++)
    {
      if (!musician_gpt_input_stream_read_byte (stream, cancellable, &barre_end[i], error))
        return FALSE;
    }

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &omission1, error) ||
      !musician_gpt_input_stream_read_byte (stream, cancellable, &omission3, error) ||
      !musician_gpt_input_stream_read_byte (stream, cancellable, &omission5, error) ||
      !musician_gpt_input_stream_read_byte (stream, cancellable, &omission7, error) ||
      !musician_gpt_input_stream_read_byte (stream, cancellable, &omission9, error) ||
      !musician_gpt_input_stream_read_byte (stream, cancellable, &omission11, error) ||
      !musician_gpt_input_stream_read_byte (stream, cancellable, &omission13, error))
    return FALSE;

  if (!g_input_stream_skip (G_INPUT_STREAM (stream), 1, cancellable, error))
    return FALSE;

  for (guint i = 0; i < G_N_ELEMENTS (fingering); i++)
    {
      if (!musician_gpt_input_stream_read_byte (stream, cancellable, &fingering[i], error))
        return FALSE;
    }

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &display, error))
    return FALSE;

  *chord_out = g_steal_pointer (&chord);

  return TRUE;
}

static gboolean
musician_gp4_parser_load_bend (MusicianGp4Parser       *self,
                               MusicianGptInputStream  *stream,
                               GCancellable            *cancellable,
                               MusicianGptBend        **out_bend,
                               GError                 **error)
{
  g_autoptr(MusicianGptBend) bend = NULL;
  guint32 n_points;
  guint8 type;

  g_assert (MUSICIAN_IS_GP4_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  bend = musician_gpt_bend_new ();

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &type, error))
    return FALSE;

  musician_gpt_bend_set_bend_type (bend, type);

  if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &n_points, error))
    return FALSE;

  for (guint i = 0; i < n_points; i++)
    {
      MusicianGptBendPoint point;
      guint32 abspos;
      guint32 vertpos;
      guint8 vibrato;

      if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &abspos, error))
        return FALSE;

      if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &vertpos, error))
        return FALSE;

      if (!musician_gpt_input_stream_read_byte (stream, cancellable, &vibrato, error))
        return FALSE;

      point.absolute_position = abspos;
      point.vertical_position = vertpos;
      point.vibrato = vibrato;

      musician_gpt_bend_add_point (bend, &point);
    }

  *out_bend = g_steal_pointer (&bend);

  return TRUE;
}

static gboolean
musician_gp4_parser_load_beat (MusicianGp4Parser       *self,
                               MusicianGptInputStream  *stream,
                               MusicianGptSong         *song,
                               guint                    n_measures,
                               guint                    n_tracks,
                               guint                    n_beats,
                               GCancellable            *cancellable,
                               GError                 **error)
{
  g_autoptr(MusicianGptBeat) beat = NULL;
  MusicianGptBeatFlags flags;
  guint8 header;
  guint8 duration;

  g_assert (MUSICIAN_IS_GP4_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (MUSICIAN_IS_GPT_SONG (song));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  beat = musician_gpt_beat_new ();

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &header, error))
    return FALSE;

  flags = header;

  if (flags & MUSICIAN_GPT_BEAT_FLAGS_STATUS)
    {
      guint8 status;

      if (!musician_gpt_input_stream_read_byte (stream, cancellable, &status, error))
        return FALSE;

      if (status == 2)
        musician_gpt_beat_set_mode (beat, MUSICIAN_GPT_BEAT_MODE_REST);
      else if (status == 0)
        musician_gpt_beat_set_mode (beat, MUSICIAN_GPT_BEAT_MODE_EMPTY);
      else
        {
          g_set_error (error,
                       G_IO_ERROR,
                       G_IO_ERROR_INVALID_DATA,
                       "Unknown beat mode of %d",
                       status);
          return FALSE;
        }
    }

  if (!musician_gpt_input_stream_read_byte (stream, cancellable, &duration, error))
    return FALSE;

  musician_gpt_beat_set_duration (beat, duration);

  if (flags & MUSICIAN_GPT_BEAT_FLAGS_N_TUPLET)
    {
      guint32 n_tuplet;

      if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &n_tuplet, error))
        return FALSE;

      musician_gpt_beat_set_n_tuplet (beat, n_tuplet);
    }

  if (flags & MUSICIAN_GPT_BEAT_FLAGS_CHORD_DIAGRAM)
    {
      g_autoptr(MusicianGptChord) chord = NULL;

      if (!musician_gp4_parser_load_chord (self, stream, cancellable, &chord, error))
        return FALSE;

      musician_gpt_beat_set_chord (beat, chord);
    }

  if (flags & MUSICIAN_GPT_BEAT_FLAGS_TEXT)
    {
      g_autofree gchar *text = NULL;

      if (NULL == (text = musician_gpt_input_stream_read_string (stream, cancellable, error)))
        return FALSE;

      musician_gpt_beat_set_text (beat, text);
    }

  if (flags & MUSICIAN_GPT_BEAT_FLAGS_EFFECTS)
    {
      guint8 effects1;
      guint8 effects2;

      if (!musician_gpt_input_stream_read_byte (stream, cancellable, &effects1, error) ||
          !musician_gpt_input_stream_read_byte (stream, cancellable, &effects2, error))
        return FALSE;

      if (effects1 & (1 << 5))
        {
          guint8 dynamics;

          if (!musician_gpt_input_stream_read_byte (stream, cancellable, &dynamics, error))
            return FALSE;

          musician_gpt_beat_set_dynamics (beat, dynamics);
        }

      if (effects2 & (1 << 2))
        {
          g_autoptr(MusicianGptBend) bend = NULL;

          if (!musician_gp4_parser_load_bend (self, stream, cancellable, &bend, error))
            return FALSE;
        }
    }

  if (flags & MUSICIAN_GPT_BEAT_FLAGS_MIX_TABLE)
    {
    }

  return TRUE;
}

static gboolean
musician_gp4_parser_load_measure_pairs (MusicianGp4Parser       *self,
                                        MusicianGptInputStream  *stream,
                                        MusicianGptSong         *song,
                                        guint                    n_measures,
                                        guint                    n_tracks,
                                        GCancellable            *cancellable,
                                        GError                 **error)
{
  g_assert (MUSICIAN_IS_GP4_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (MUSICIAN_IS_GPT_SONG (song));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  for (guint measure = 0; measure < n_measures; measure++)
    {
      for (guint track = 0; track < n_tracks; track++)
        {
          guint32 n_beats;

          if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &n_beats, error))
            return FALSE;

          if (!musician_gp4_parser_load_beat (self, stream, song, measure + 1, track + 1, n_beats, cancellable, error))
            return FALSE;
        }
    }

  return TRUE;
}

static MusicianGptSong *
musician_gp4_parser_load (MusicianGptParser       *parser,
                          MusicianGptInputStream  *stream,
                          const gchar             *version,
                          GCancellable            *cancellable,
                          GError                 **error)
{
  MusicianGp4Parser *self = (MusicianGp4Parser *)parser;
  g_autoptr(MusicianGptSong) song = NULL;
  MusicianGptTripletFeel triplet_feel = 0;
  MusicianGptKey key = 0;
  MusicianGptOctave octave = 0;
  guint32 tempo = 0;
  guint32 n_measures = 0;
  guint32 n_tracks = 0;

  g_assert (MUSICIAN_IS_GP4_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  song = musician_gpt_song_new ();

  _musician_gpt_song_set_version (song, version);

  /* Load basic stuff like title, subtitle, artist, etc */
  if (!musician_gp4_parser_load_attributes (self, stream, song, cancellable, error))
    return NULL;

  /* Load the triplet feel field */
  if (musician_gpt_input_stream_read_triplet_feel (stream, cancellable, &triplet_feel, error))
    musician_gpt_song_set_triplet_feel (song, triplet_feel);
  else
    return NULL;

  /* Load the track lyrics */
  if (!musician_gp4_parser_load_lyrics (self, stream, song, cancellable, error))
    return NULL;

  /* Load the document tempo */
  if (musician_gpt_input_stream_read_tempo (stream, cancellable, &tempo, error))
    musician_gpt_song_set_tempo (song, tempo);
  else
    return NULL;

  /* Load the document key */
  if (musician_gpt_input_stream_read_key (stream, cancellable, &key, error))
    musician_gpt_song_set_key (song, key);
  else
    return NULL;

  /* Read the document octave */
  if (musician_gpt_input_stream_read_octave (stream, cancellable, &octave, error))
    musician_gpt_song_set_octave (song, octave);
  else
    return NULL;

  /* Load midi port/channel mappings */
  if (!musician_gp4_parser_load_midi_ports (self, stream, song, cancellable, error))
    return NULL;

  /* Load the number of measures in the song */
  if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &n_measures, error))
    return NULL;

  /* Load the number of tracks in the song */
  if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &n_tracks, error))
    return NULL;

  /* Load information about our mesaures */
  if (!musician_gp4_parser_load_measures (self, stream, song, n_measures, cancellable, error))
    return NULL;

  /* Load information about the tracks */
  if (!musician_gp4_parser_load_tracks (self, stream, song, n_tracks, cancellable, error))
    return NULL;

  /* Load measure/track pairs */
  if (!musician_gp4_parser_load_measure_pairs (self, stream, song, n_measures, n_tracks, cancellable, error))
    return NULL;

  return g_steal_pointer (&song);
}

static void
musician_gp4_parser_finalize (GObject *object)
{
  G_OBJECT_CLASS (musician_gp4_parser_parent_class)->finalize (object);
}

static void
musician_gp4_parser_class_init (MusicianGp4ParserClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  MusicianGptParserClass *parser_class = MUSICIAN_GPT_PARSER_CLASS (klass);

  object_class->finalize = musician_gp4_parser_finalize;

  parser_class->load = musician_gp4_parser_load;
}

static void
musician_gp4_parser_init (MusicianGp4Parser *self)
{
}
