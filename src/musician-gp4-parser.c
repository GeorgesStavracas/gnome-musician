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
      MusicianGptMeasureFlags flags;
      guint8 header;
      guint8 numerator = 0;
      guint8 denominator = 0;
      guint8 n_repeats = 0;
      guint8 nth_alternate = 0;
      guint8 key = 0;

      if (!musician_gpt_input_stream_read_byte (stream, cancellable, &header, error))
        return FALSE;

      flags = header;

      if (flags & MUSICIAN_GPT_MEASURE_FLAGS_KEY_NUMERATOR)
        {
          if (!musician_gpt_input_stream_read_byte (stream, cancellable, &numerator, error))
            return FALSE;
        }

      if (flags & MUSICIAN_GPT_MEASURE_FLAGS_KEY_DENOMINATOR)
        {
          if (!musician_gpt_input_stream_read_byte (stream, cancellable, &denominator, error))
            return FALSE;
        }

      if (flags & MUSICIAN_GPT_MEASURE_FLAGS_REPEAT_END)
        {
          if (!musician_gpt_input_stream_read_byte (stream, cancellable, &n_repeats, error))
            return FALSE;
        }

      if (flags & MUSICIAN_GPT_MEASURE_FLAGS_ALTERNATE_ENDING)
        {
          if (!musician_gpt_input_stream_read_byte (stream, cancellable, &nth_alternate, error))
            return FALSE;
        }

      if (flags & MUSICIAN_GPT_MEASURE_FLAGS_MARKER)
        {
          g_autofree gchar *name = NULL;
          GdkRGBA color;

          if (NULL == (name = musician_gpt_input_stream_read_string (stream, cancellable, error)))
            return FALSE;

          if (!musician_gpt_input_stream_read_color (stream, cancellable, &color, error))
            return FALSE;
        }

      if (flags & MUSICIAN_GPT_MEASURE_FLAGS_TONALITY)
        {
          if (!musician_gpt_input_stream_read_byte (stream, cancellable, &key, error) ||
              !musician_gpt_input_stream_read_byte (stream, cancellable, NULL, error))
            return FALSE;
        }

      /*
       * We might be able to do a lightweight instance instead of a full
       * object here for the measure. I'm not sure we'll be binding against
       * these instances directly anyway (rather than an intermediate
       * "MusicianMeasure" or something).
       */
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
      guint32 channel_effects;
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
          !musician_gpt_input_stream_read_uint32 (stream, cancellable, &channel_effects, error) ||
          !musician_gpt_input_stream_read_uint32 (stream, cancellable, &n_frets, error) ||
          !musician_gpt_input_stream_read_uint32 (stream, cancellable, &capo_at, error) ||
          !musician_gpt_input_stream_read_color (stream, cancellable, &color, error))
        return FALSE;

      track = musician_gpt_track_new ();

      musician_gpt_track_set_capo_at (track, capo_at);
      musician_gpt_track_set_channel_effects (track, channel_effects);
      musician_gpt_track_set_channel (track, channel);
      musician_gpt_track_set_color (track, &color);
      musician_gpt_track_set_id (track, i + 1);
      musician_gpt_track_set_n_frets (track, n_frets);
      musician_gpt_track_set_port (track, port);
      musician_gpt_track_set_title (track, title);
      musician_gpt_track_set_tunings (track, tunings, G_N_ELEMENTS (tunings));

      musician_gpt_song_add_track (song, track);
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
