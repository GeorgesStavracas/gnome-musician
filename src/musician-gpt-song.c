/* musician-gpt-song.c
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

#define G_LOG_DOMAIN "musician-gpt-song"

#include "musician-gpt-song.h"

typedef struct
{
  gchar *album;
  gchar *artist;
  gchar *copyright;
  gchar *interpretation;
  gchar *instructions;
  gchar *subtitle;
  gchar *title;
  gchar *version;
  gchar *writer;

  gchar **comments;

  MusicianGptTripletFeel triplet_feel;
  MusicianGptKey key;
  MusicianGptOctave octave;
  guint tempo;

  GArray *ports;

} MusicianGptSongPrivate;

enum {
  PROP_0,
  PROP_ALBUM,
  PROP_ARTIST,
  PROP_COPYRIGHT,
  PROP_INTERPRETATION,
  PROP_INSTRUCTIONS,
  PROP_KEY,
  PROP_OCTAVE,
  PROP_SUBTITLE,
  PROP_TEMPO,
  PROP_TITLE,
  PROP_TRIPLET_FEEL,
  PROP_VERSION,
  PROP_WRITER,
  N_PROPS
};

G_DEFINE_TYPE_WITH_PRIVATE (MusicianGptSong, musician_gpt_song, G_TYPE_OBJECT)

static GParamSpec *properties [N_PROPS];

static void
musician_gpt_song_finalize (GObject *object)
{
  MusicianGptSong *self = (MusicianGptSong *)object;
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_clear_pointer (&priv->album, g_free);
  g_clear_pointer (&priv->artist, g_free);
  g_clear_pointer (&priv->copyright, g_free);
  g_clear_pointer (&priv->interpretation, g_free);
  g_clear_pointer (&priv->instructions, g_free);
  g_clear_pointer (&priv->ports, g_array_unref);
  g_clear_pointer (&priv->subtitle, g_free);
  g_clear_pointer (&priv->title, g_free);
  g_clear_pointer (&priv->version, g_free);
  g_clear_pointer (&priv->writer, g_free);

  G_OBJECT_CLASS (musician_gpt_song_parent_class)->finalize (object);
}

static void
musician_gpt_song_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  MusicianGptSong *self = MUSICIAN_GPT_SONG (object);

  switch (prop_id)
    {
    case PROP_ALBUM:
      g_value_set_string (value, musician_gpt_song_get_album (self));
      break;

    case PROP_ARTIST:
      g_value_set_string (value, musician_gpt_song_get_artist (self));
      break;

    case PROP_COPYRIGHT:
      g_value_set_string (value, musician_gpt_song_get_copyright (self));
      break;

    case PROP_INTERPRETATION:
      g_value_set_string (value, musician_gpt_song_get_interpretation (self));
      break;

    case PROP_INSTRUCTIONS:
      g_value_set_string (value, musician_gpt_song_get_instructions (self));
      break;

    case PROP_KEY:
      g_value_set_int (value, musician_gpt_song_get_key (self));
      break;

    case PROP_OCTAVE:
      g_value_set_enum (value, musician_gpt_song_get_octave (self));
      break;

    case PROP_SUBTITLE:
      g_value_set_string (value, musician_gpt_song_get_subtitle (self));
      break;

    case PROP_TEMPO:
      g_value_set_uint (value, musician_gpt_song_get_tempo (self));
      break;

    case PROP_TITLE:
      g_value_set_string (value, musician_gpt_song_get_title (self));
      break;

    case PROP_TRIPLET_FEEL:
      g_value_set_enum (value, musician_gpt_song_get_triplet_feel (self));
      break;

    case PROP_VERSION:
      g_value_set_string (value, musician_gpt_song_get_version (self));
      break;

    case PROP_WRITER:
      g_value_set_string (value, musician_gpt_song_get_writer (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
musician_gpt_song_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  MusicianGptSong *self = MUSICIAN_GPT_SONG (object);

  switch (prop_id)
    {
    case PROP_ALBUM:
      musician_gpt_song_set_album (self, g_value_get_string (value));
      break;

    case PROP_ARTIST:
      musician_gpt_song_set_artist (self, g_value_get_string (value));
      break;

    case PROP_COPYRIGHT:
      musician_gpt_song_set_copyright (self, g_value_get_string (value));
      break;

    case PROP_INTERPRETATION:
      musician_gpt_song_set_interpretation (self, g_value_get_string (value));
      break;

    case PROP_INSTRUCTIONS:
      musician_gpt_song_set_instructions (self, g_value_get_string (value));
      break;

    case PROP_OCTAVE:
      musician_gpt_song_set_octave (self, g_value_get_enum (value));
      break;

    case PROP_KEY:
      musician_gpt_song_set_key (self, g_value_get_int (value));
      break;

    case PROP_SUBTITLE:
      musician_gpt_song_set_subtitle (self, g_value_get_string (value));
      break;

    case PROP_TEMPO:
      musician_gpt_song_set_tempo (self, g_value_get_uint (value));
      break;

    case PROP_TITLE:
      musician_gpt_song_set_title (self, g_value_get_string (value));
      break;

    case PROP_TRIPLET_FEEL:
      musician_gpt_song_set_triplet_feel (self, g_value_get_enum (value));
      break;

    case PROP_WRITER:
      musician_gpt_song_set_writer (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
musician_gpt_song_class_init (MusicianGptSongClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = musician_gpt_song_finalize;
  object_class->get_property = musician_gpt_song_get_property;
  object_class->set_property = musician_gpt_song_set_property;

  properties [PROP_ALBUM] =
    g_param_spec_string ("album",
                         "Album",
                         "The album of the song",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_ARTIST] =
    g_param_spec_string ("artist",
                         "Artist",
                         "The artist of the song",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_COPYRIGHT] =
    g_param_spec_string ("copyright",
                         "Copyright",
                         "The copyright of the song",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_INTERPRETATION] =
    g_param_spec_string ("interpretation",
                         "Interpretation",
                         "The interpretation of the song",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_INSTRUCTIONS] =
    g_param_spec_string ("instructions",
                         "Instructions",
                         "The instructions for the song",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_OCTAVE] =
    g_param_spec_enum ("ovtave",
                       "Octave",
                       "The octave for the song",
                       MUSICIAN_TYPE_GPT_OCTAVE,
                       MUSICIAN_GPT_OCTAVE_NONE,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_KEY] =
    g_param_spec_int ("key",
                      "Key",
                      "The key for the song",
                      G_MININT8,
                      G_MAXINT8,
                      0,
                      (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_SUBTITLE] =
    g_param_spec_string ("subtitle",
                         "Subtitle",
                         "The subtitle of the song",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_TEMPO] =
    g_param_spec_uint ("tempo",
                       "Tempo",
                       "The tempo of the song",
                       0,
                       G_MAXINT,
                       0,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title",
                         "The title of the song",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_TRIPLET_FEEL] =
    g_param_spec_enum ("triplet-feel",
                       "Triplet Feel",
                       "Name",
                       MUSICIAN_TYPE_GPT_TRIPLET_FEEL,
                       MUSICIAN_GPT_TRIPLET_FEEL_NONE,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "Version",
                         "The version string from the file",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_WRITER] =
    g_param_spec_string ("writer",
                         "Writer",
                         "The writer of this tabulation",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
musician_gpt_song_init (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  priv->ports = g_array_new (FALSE, FALSE, sizeof (MusicianGptMidiPort));
}

MusicianGptSong *
musician_gpt_song_new (void)
{
  return g_object_new (MUSICIAN_TYPE_GPT_SONG, NULL);
}

void
_musician_gpt_song_set_version (MusicianGptSong *self,
                                const gchar     *version)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (version != priv->version)
    {
      g_free (priv->version);
      priv->version = g_strdup (version);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_VERSION]);
    }
}

const gchar *
musician_gpt_song_get_album (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), NULL);

  return priv->album;
}

const gchar *
musician_gpt_song_get_artist (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), NULL);

  return priv->artist;
}

const gchar *
musician_gpt_song_get_copyright (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), NULL);

  return priv->copyright;
}

const gchar *
musician_gpt_song_get_instructions (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), NULL);

  return priv->instructions;
}

const gchar *
musician_gpt_song_get_interpretation (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), NULL);

  return priv->interpretation;
}

const gchar *
musician_gpt_song_get_subtitle (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), NULL);

  return priv->subtitle;
}

const gchar *
musician_gpt_song_get_title (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), NULL);

  return priv->title;
}

MusicianGptTripletFeel
musician_gpt_song_get_triplet_feel (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), 0);

  return priv->triplet_feel;
}

const gchar *
musician_gpt_song_get_version (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), NULL);

  return priv->version;
}

const gchar *
musician_gpt_song_get_writer (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), NULL);

  return priv->writer;
}

void
musician_gpt_song_set_album (MusicianGptSong *self,
                             const gchar     *album)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (album != priv->album)
    {
      g_free (priv->album);
      priv->album = g_strdup (album);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ALBUM]);
    }
}

void
musician_gpt_song_set_artist (MusicianGptSong *self,
                              const gchar     *artist)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (artist != priv->artist)
    {
      g_free (priv->artist);
      priv->artist = g_strdup (artist);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ARTIST]);
    }
}

void
musician_gpt_song_set_copyright (MusicianGptSong *self,
                                 const gchar     *copyright)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (copyright != priv->copyright)
    {
      g_free (priv->copyright);
      priv->copyright = g_strdup (copyright);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_COPYRIGHT]);
    }
}

void
musician_gpt_song_set_instructions (MusicianGptSong *self,
                                    const gchar     *instructions)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (instructions != priv->instructions)
    {
      g_free (priv->instructions);
      priv->instructions = g_strdup (instructions);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_INSTRUCTIONS]);
    }
}

void
musician_gpt_song_set_interpretation (MusicianGptSong *self,
                                      const gchar     *interpretation)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (interpretation != priv->interpretation)
    {
      g_free (priv->interpretation);
      priv->interpretation = g_strdup (interpretation);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_INTERPRETATION]);
    }
}

void
musician_gpt_song_set_subtitle (MusicianGptSong *self,
                                const gchar     *subtitle)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (subtitle != priv->subtitle)
    {
      g_free (priv->subtitle);
      priv->subtitle = g_strdup (subtitle);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_SUBTITLE]);
    }
}

void
musician_gpt_song_set_title (MusicianGptSong *self,
                             const gchar     *title)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (title != priv->title)
    {
      g_free (priv->title);
      priv->title = g_strdup (title);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_TITLE]);
    }
}

void
musician_gpt_song_set_triplet_feel (MusicianGptSong        *self,
                                    MusicianGptTripletFeel  triplet_feel)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (triplet_feel != priv->triplet_feel)
    {
      priv->triplet_feel = triplet_feel;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_TRIPLET_FEEL]);
    }
}

void
musician_gpt_song_set_writer (MusicianGptSong *self,
                              const gchar     *writer)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (writer != priv->writer)
    {
      g_free (priv->writer);
      priv->writer = g_strdup (writer);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_WRITER]);
    }
}

void
_musician_gpt_song_add_lyrics (MusicianGptSong *self,
                               guint            position,
                               const gchar     *lyrics)
{
  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  /* TODO: Lyrics position/lyrics pair */
}

guint
musician_gpt_song_get_tempo (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), 0);

  return priv->tempo;
}

void
musician_gpt_song_set_tempo (MusicianGptSong *self,
                             guint            tempo)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (tempo != priv->tempo)
    {
      priv->tempo = tempo;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_TEMPO]);
    }
}

MusicianGptKey
musician_gpt_song_get_key (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), 0);

  return priv->key;
}

void
musician_gpt_song_set_key (MusicianGptSong *self,
                           MusicianGptKey   key)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (priv->key != key)
    {
      priv->key = key;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_KEY]);
    }
}

MusicianGptOctave
musician_gpt_song_get_octave (MusicianGptSong *self)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_SONG (self), 0);

  return priv->key;
}

void
musician_gpt_song_set_octave (MusicianGptSong   *self,
                              MusicianGptOctave  octave)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  if (priv->octave != octave)
    {
      priv->octave = octave;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_OCTAVE]);
    }
}

void
_musician_gpt_song_set_midi_ports (MusicianGptSong           *self,
                                   const MusicianGptMidiPort *ports,
                                   gsize                      n_ports)
{
  MusicianGptSongPrivate *priv = musician_gpt_song_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_SONG (self));

  g_array_set_size (priv->ports, 0);
  if (n_ports > 0)
    g_array_append_vals (priv->ports, ports, n_ports);
}
