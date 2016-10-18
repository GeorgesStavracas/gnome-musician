/* musician-gpt-lyrics.c
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

#define G_LOG_DOMAIN "musician-gpt-lyrics"

#include "musician-gpt-lyrics.h"

typedef struct
{
  gchar *text;
  guint position;
} MusicianGptLyricsPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (MusicianGptLyrics, musician_gpt_lyrics, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_POSITION,
  PROP_TEXT,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

MusicianGptLyrics *
musician_gpt_lyrics_new (void)
{
  return g_object_new (MUSICIAN_TYPE_GPT_LYRICS, NULL);
}

static void
musician_gpt_lyrics_finalize (GObject *object)
{
  MusicianGptLyrics *self = (MusicianGptLyrics *)object;
  MusicianGptLyricsPrivate *priv = musician_gpt_lyrics_get_instance_private (self);

  g_clear_pointer (&priv->text, g_free);

  G_OBJECT_CLASS (musician_gpt_lyrics_parent_class)->finalize (object);
}

static void
musician_gpt_lyrics_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  MusicianGptLyrics *self = MUSICIAN_GPT_LYRICS (object);

  switch (prop_id)
    {
    case PROP_POSITION:
      g_value_set_uint (value, musician_gpt_lyrics_get_position (self));
      break;

    case PROP_TEXT:
      g_value_set_string (value, musician_gpt_lyrics_get_text (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
musician_gpt_lyrics_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  MusicianGptLyrics *self = MUSICIAN_GPT_LYRICS (object);

  switch (prop_id)
    {
    case PROP_POSITION:
      musician_gpt_lyrics_set_position (self, g_value_get_uint (value));
      break;

    case PROP_TEXT:
      musician_gpt_lyrics_set_text (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
musician_gpt_lyrics_class_init (MusicianGptLyricsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = musician_gpt_lyrics_finalize;
  object_class->get_property = musician_gpt_lyrics_get_property;
  object_class->set_property = musician_gpt_lyrics_set_property;

  properties [PROP_POSITION] =
    g_param_spec_uint ("position",
                       "Position",
                       "Position",
                       0,
                       G_MAXUINT,
                       0,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_TEXT] =
    g_param_spec_string ("text",
                         "Text",
                         "Text",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
musician_gpt_lyrics_init (MusicianGptLyrics *self)
{
}

const gchar *
musician_gpt_lyrics_get_text (MusicianGptLyrics *self)
{
  MusicianGptLyricsPrivate *priv = musician_gpt_lyrics_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_LYRICS (self), NULL);

  return priv->text;
}

void
musician_gpt_lyrics_set_text (MusicianGptLyrics *self,
                              const gchar       *text)
{
  MusicianGptLyricsPrivate *priv = musician_gpt_lyrics_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_LYRICS (self));

  if (g_strcmp0 (priv->text, text) != 0)
    {
      g_free (priv->text);
      priv->text = g_strdup (text);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_TEXT]);
    }
}

guint
musician_gpt_lyrics_get_position (MusicianGptLyrics *self)
{
  MusicianGptLyricsPrivate *priv = musician_gpt_lyrics_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_LYRICS (self), 0);

  return priv->position;
}

void
musician_gpt_lyrics_set_position (MusicianGptLyrics *self,
                                  guint              position)
{
  MusicianGptLyricsPrivate *priv = musician_gpt_lyrics_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_LYRICS (self));

  if (priv->position != position)
    {
      priv->position = position;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_POSITION]);
    }
}
