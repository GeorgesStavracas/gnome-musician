/* musician-gpt-parser.c
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

#define G_LOG_DOMAIN "musician-gpt-parser"

#include "musician-gpt-input-stream.h"
#include "musician-gpt-parser.h"

/**
 * SECTION:musician-gpt-parser:
 * @title: #MusicianGptParser
 * @short_description: An parser for Guitar Pro™ files
 *
 * For more information on the format, see:
 *
 *   http://dguitar.sourceforge.net/GP4format.html#Summary
 *
 * Guitar Pro is a trademark of Arobas Music.
 */

typedef struct
{
  gchar version[32];
  gboolean (*read_header) (MusicianGptParser       *self,
                           MusicianGptInputStream  *stream,
                           GCancellable            *cancellable,
                           GError                 **error);
  gboolean (*read_body)   (MusicianGptParser       *self,
                           MusicianGptInputStream  *stream,
                           GCancellable            *cancellable,
                           GError                 **error);
} MusicianGptParserVtable;

typedef struct
{
  const MusicianGptParserVtable *vtable;
} MusicianGptParserPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (MusicianGptParser, musician_gpt_parser, G_TYPE_OBJECT)

static gboolean
read_header_v4 (MusicianGptParser       *self,
                MusicianGptInputStream  *stream,
                GCancellable            *cancellable,
                GError                 **error)
{
  g_autofree gchar *album = NULL;
  g_autofree gchar *author = NULL;
  g_autofree gchar *copyright = NULL;
  g_autofree gchar *interpret = NULL;
  g_autofree gchar *instructional = NULL;
  g_autofree gchar *subtitle = NULL;
  g_autofree gchar *writer = NULL;
  g_autofree gchar *title = NULL;
  g_auto(GStrv) comments = NULL;
  gint32 n_comments;

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

  /* Read the author of the piece */
  if (NULL == (author = musician_gpt_input_stream_read_string (stream, cancellable, error)))
    return FALSE;

  /* Read the copyright of the piece */
  if (NULL == (copyright = musician_gpt_input_stream_read_string (stream, cancellable, error)))
    return FALSE;

  /* Read the tab_author of the piece */
  if (NULL == (writer = musician_gpt_input_stream_read_string (stream, cancellable, error)))
    return FALSE;

  /* Read the instructional of the piece */
  if (NULL == (instructional = musician_gpt_input_stream_read_string (stream, cancellable, error)))
    return FALSE;

  g_strstrip (title);
  g_strstrip (subtitle);
  g_strstrip (interpret);
  g_strstrip (album);
  g_strstrip (author);
  g_strstrip (copyright);
  g_strstrip (writer);
  g_strstrip (instructional);

  g_print ("Title:           \"%s\"\n"
           "Subtitle:        \"%s\"\n"
           "Interpret:       \"%s\"\n"
           "Album:           \"%s\"\n"
           "Author:          \"%s\"\n"
           "Copyright:       \"%s\"\n"
           "Writer:          \"%s\"\n"
           "Instructional:   \"%s\"\n",
           title, subtitle, interpret, album, author, copyright, writer, instructional);

  comments = musician_gpt_input_stream_read_string_array (stream, cancellable, error);

  for (guint i = 0; comments && comments[i]; i++)
    g_print ("Comments[%d]: %s", i, comments[i]);

  return TRUE;
}

static gboolean
read_body_v4 (MusicianGptParser       *self,
              MusicianGptInputStream  *stream,
              GCancellable            *cancellable,
              GError                 **error)
{
  MusicianGptTripletFeel triplet_feel;
  MusicianGptOctave octave;
  GError *local_error = NULL;
  guint32 track_num;
  guint32 tempo;
  guint32 n_measures;
  guint32 n_tracks;
  gint8 key;

  g_assert (MUSICIAN_IS_GPT_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  if (!musician_gpt_input_stream_read_triplet_feel (stream, cancellable, &triplet_feel, error))
    return FALSE;

  track_num = g_data_input_stream_read_uint32 (G_DATA_INPUT_STREAM (stream), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return FALSE;
    }

  for (guint i = 0; i < 5; i++)
    {
      g_autofree gchar *lyric = NULL;
      guint32 position;

      if (NULL == (lyric = musician_gpt_input_stream_read_lyric (stream, cancellable, &position, error)))
        return FALSE;

      g_print ("Lyric (%u): %s\n", position, lyric);
    }

  if (!musician_gpt_input_stream_read_tempo (stream, cancellable, &tempo, error))
    return FALSE;

  g_print ("Tempo: %u\n", tempo);

  if (!musician_gpt_input_stream_read_key (stream, cancellable, &key, error))
    return FALSE;

  g_print ("key: %u\n", key);

  if (!musician_gpt_input_stream_read_octave (stream, cancellable, &octave, error))
    return FALSE;

  g_print ("octave: %u\n", octave);

  for (guint i = 0; i < 4; i++)
    {
      MusicianGptMidiPort port;

      if (!musician_gpt_input_stream_read_midi_port (stream, i + 1, cancellable, &port, error))
        return FALSE;

      g_print ("MIDI Port (%d)\n", port.port_id);
      for (guint j = 0; j < G_N_ELEMENTS (port.channels); j++)
        g_print ("  Instrument: %d\n"
                 "      Volume: %d\n"
                 "     Balance: %d\n"
                 "      Chorus: %d\n"
                 "      Reverb: %d\n"
                 "      Phaser: %d\n"
                 "     Tremelo: %d\n\n",
                 port.channels[j].instrument,
                 port.channels[j].volume,
                 port.channels[j].balance,
                 port.channels[j].chorus,
                 port.channels[j].reverb,
                 port.channels[j].phaser,
                 port.channels[j].tremelo);
    }

  if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &n_measures, error))
    return FALSE;

  g_print ("%u measures\n", n_measures);

  if (!musician_gpt_input_stream_read_uint32 (stream, cancellable, &n_tracks, error))
    return FALSE;

  g_print ("%u tracks\n", n_tracks);

  return TRUE;
}

static gboolean
musician_gpt_parser_load_vtable (MusicianGptParser  *self,
                                 const gchar        *version,
                                 GError            **error)
{
  MusicianGptParserPrivate *priv = musician_gpt_parser_get_instance_private (self);
  static const MusicianGptParserVtable vtables[] = {
    { "FICHIER GUITARE PRO v1", NULL },
    { "FICHIER GUITARE PRO v1.01", NULL },
    { "FICHIER GUITARE PRO v1.02", NULL },
    { "FICHIER GUITARE PRO v1.03", NULL },
    { "FICHIER GUITARE PRO v1.04", NULL },
    { "FICHIER GUITAR PRO v2.20", NULL },
    { "FICHIER GUITAR PRO v2.21", NULL },
    { "FICHIER GUITAR PRO v3.00", NULL },
    { "FICHIER GUITAR PRO v4.00", read_header_v4, read_body_v4 },
    { "FICHIER GUITAR PRO v4.06", read_header_v4, read_body_v4 },
    { "FICHIER GUITAR PRO L4.06", read_header_v4, read_body_v4 },
  };

  g_assert (MUSICIAN_IS_GPT_PARSER (self));
  g_assert (version != NULL);

  for (guint i = 0; i < G_N_ELEMENTS (vtables); i++)
    {
      const MusicianGptParserVtable *vtable = &vtables[i];

      if (g_str_equal (vtable->version, version))
        {
          priv->vtable = vtable;
          return TRUE;
        }
    }

  g_set_error (error,
               G_IO_ERROR,
               G_IO_ERROR_NOT_SUPPORTED,
               "Guitar Pro version “%s” is not supported",
               version);

  return FALSE;
}

static void
musician_gpt_parser_class_init (MusicianGptParserClass *klass)
{
}

static void
musician_gpt_parser_init (MusicianGptParser *self)
{
}

static gboolean
musician_gpt_parser_load_header (MusicianGptParser       *self,
                                 MusicianGptInputStream  *stream,
                                 GCancellable            *cancellable,
                                 GError                 **error)
{
  MusicianGptParserPrivate *priv = musician_gpt_parser_get_instance_private (self);

  g_assert (MUSICIAN_IS_GPT_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));
  g_assert (priv->vtable != NULL);

  if (priv->vtable->read_header == NULL)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_NOT_SUPPORTED,
                           "The file format is not supported");
      return FALSE;
    }

  return priv->vtable->read_header (self, stream, cancellable, error);
}

static gboolean
musician_gpt_parser_load_body (MusicianGptParser       *self,
                               MusicianGptInputStream  *stream,
                               GCancellable            *cancellable,
                               GError                 **error)
{
  MusicianGptParserPrivate *priv = musician_gpt_parser_get_instance_private (self);

  g_assert (MUSICIAN_IS_GPT_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));
  g_assert (priv->vtable != NULL);

  if (priv->vtable->read_body == NULL)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_NOT_SUPPORTED,
                           "The file format is not supported");
      return FALSE;
    }

  return priv->vtable->read_body (self, stream, cancellable, error);
}

MusicianGptParser *
musician_gpt_parser_new (void)
{
  return g_object_new (MUSICIAN_TYPE_GPT_PARSER, NULL);
}

gboolean
musician_gpt_parser_load_from_stream (MusicianGptParser  *self,
                                      GInputStream       *base_stream,
                                      GCancellable       *cancellable,
                                      GError            **error)
{
  g_autoptr(MusicianGptInputStream) stream = NULL;
  g_autofree gchar *version = NULL;

  g_return_val_if_fail (MUSICIAN_IS_GPT_PARSER (self), FALSE);
  g_return_val_if_fail (G_IS_INPUT_STREAM (base_stream), FALSE);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);

  stream = musician_gpt_input_stream_new (base_stream);

  /* Read the file version */
  if (NULL == (version = musician_gpt_input_stream_read_fixed_string (stream, 30, cancellable, error)))
    return FALSE;

  /* Discover the vtable for the version at hand */
  if (!musician_gpt_parser_load_vtable (self, version, error))
    return FALSE;

  /* Read the file header */
  if (!musician_gpt_parser_load_header (self, stream, cancellable, error))
    return FALSE;

  /* Read the file body */
  if (!musician_gpt_parser_load_body (self, stream, cancellable, error))
    return FALSE;

  return TRUE;
}
