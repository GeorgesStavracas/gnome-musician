/* musician-gpt-track.c
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

#define G_LOG_DOMAIN "musician-gpt-track"

#include "musician-gpt-track.h"

typedef struct
{
  gchar *title;
  GArray *tunings;
  GdkRGBA color;
  guint id;
  guint capo_at;
  guint n_frets;
  guint port;
  guint channel;
  guint effects_channel;
} MusicianGptTrackPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (MusicianGptTrack, musician_gpt_track, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_COLOR,
  PROP_CAPO_AT,
  PROP_CHANNEL,
  PROP_EFFECTS_CHANNEL,
  PROP_ID,
  PROP_N_FRETS,
  PROP_PORT,
  PROP_TITLE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

MusicianGptTrack *
musician_gpt_track_new (void)
{
  return g_object_new (MUSICIAN_TYPE_GPT_TRACK, NULL);
}

static void
musician_gpt_track_finalize (GObject *object)
{
  MusicianGptTrack *self = (MusicianGptTrack *)object;
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_clear_pointer (&priv->title, g_free);
  g_clear_pointer (&priv->tunings, g_array_unref);

  G_OBJECT_CLASS (musician_gpt_track_parent_class)->finalize (object);
}

static void
musician_gpt_track_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  MusicianGptTrack *self = MUSICIAN_GPT_TRACK (object);

  switch (prop_id)
    {
    case PROP_CAPO_AT:
      g_value_set_uint (value, musician_gpt_track_get_capo_at (self));
      break;

    case PROP_CHANNEL:
      g_value_set_uint (value, musician_gpt_track_get_channel (self));
      break;

    case PROP_EFFECTS_CHANNEL:
      g_value_set_flags (value, musician_gpt_track_get_effects_channel (self));
      break;

    case PROP_COLOR:
      g_value_set_boxed (value, musician_gpt_track_get_color (self));
      break;

    case PROP_ID:
      g_value_set_uint (value, musician_gpt_track_get_id (self));
      break;

    case PROP_N_FRETS:
      g_value_set_uint (value, musician_gpt_track_get_n_frets (self));
      break;

    case PROP_PORT:
      g_value_set_uint (value, musician_gpt_track_get_port (self));
      break;

    case PROP_TITLE:
      g_value_set_string (value, musician_gpt_track_get_title (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
musician_gpt_track_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  MusicianGptTrack *self = MUSICIAN_GPT_TRACK (object);

  switch (prop_id)
    {
    case PROP_CAPO_AT:
      musician_gpt_track_set_capo_at (self, g_value_get_uint (value));
      break;

    case PROP_CHANNEL:
      musician_gpt_track_set_channel (self, g_value_get_uint (value));
      break;

    case PROP_EFFECTS_CHANNEL:
      musician_gpt_track_set_effects_channel (self, g_value_get_flags (value));
      break;

    case PROP_COLOR:
      musician_gpt_track_set_color (self, g_value_get_boxed (value));
      break;

    case PROP_ID:
      musician_gpt_track_set_id (self, g_value_get_uint (value));
      break;

    case PROP_N_FRETS:
      musician_gpt_track_set_n_frets (self, g_value_get_uint (value));
      break;

    case PROP_PORT:
      musician_gpt_track_set_port (self, g_value_get_uint (value));
      break;

    case PROP_TITLE:
      musician_gpt_track_set_title (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
musician_gpt_track_class_init (MusicianGptTrackClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = musician_gpt_track_finalize;
  object_class->get_property = musician_gpt_track_get_property;
  object_class->set_property = musician_gpt_track_set_property;

  properties [PROP_CAPO_AT] =
    g_param_spec_uint ("capo-at",
                       "Capo At",
                       "Capo At",
                       0,
                       100,
                       0,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_CHANNEL] =
    g_param_spec_uint ("channel",
                       "Channel",
                       "Channel",
                       0,
                       16,
                       0,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_EFFECTS_CHANNEL] =
    g_param_spec_uint ("channel-effects",
                       "Effects Channel",
                       "Effects Channel",
                       0,
                       1000,
                       0,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_COLOR] =
    g_param_spec_boxed ("color",
                        "Color",
                        "Color",
                        GDK_TYPE_RGBA,
                        (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_ID] =
    g_param_spec_uint ("id",
                       "Id",
                       "Id",
                       0,
                       G_MAXUINT,
                       0,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_N_FRETS] =
    g_param_spec_uint ("n-frets",
                       "N Frets",
                       "N Frets",
                       0,
                       100,
                       0,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_PORT] =
    g_param_spec_uint ("port",
                       "Port",
                       "Port",
                       0,
                       1000,
                       0,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title",
                         "Title",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
musician_gpt_track_init (MusicianGptTrack *self)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  priv->tunings = g_array_new (FALSE, FALSE, sizeof (MusicianGptTuning));
}

const GdkRGBA *
musician_gpt_track_get_color (MusicianGptTrack *self)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_TRACK (self), NULL);

  return &priv->color;
}

void
musician_gpt_track_set_color (MusicianGptTrack *self,
                              const GdkRGBA    *color)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);
  static GdkRGBA empty = { 0 };

  g_return_if_fail (MUSICIAN_IS_GPT_TRACK (self));

  if (color == NULL)
    color = &empty;

  if (!gdk_rgba_equal (&priv->color, color))
    {
      priv->color = *color;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_COLOR]);
    }
}

guint
musician_gpt_track_get_id (MusicianGptTrack *self)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_TRACK (self), 0);

  return priv->id;
}

void
musician_gpt_track_set_id (MusicianGptTrack *self,
                           guint             id)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_TRACK (self));

  if (priv->id != id)
    {
      priv->id = id;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ID]);
    }
}

const gchar *
musician_gpt_track_get_title (MusicianGptTrack *self)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_TRACK (self), NULL);

  return priv->title;
}

void
musician_gpt_track_set_title (MusicianGptTrack *self,
                              const gchar      *title)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_TRACK (self));

  if (g_strcmp0 (priv->title, title) != 0)
    {
      g_free (priv->title);
      priv->title = g_strdup (title);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_TITLE]);
    }
}

const MusicianGptTuning *
musician_gpt_track_get_tunings (MusicianGptTrack *self,
                                gsize            *n_tunings)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_TRACK (self), NULL);

  if (n_tunings != NULL)
    *n_tunings = priv->tunings->len;

  return (const MusicianGptTuning *)(gpointer)priv->tunings->data;
}

void
musician_gpt_track_set_tunings (MusicianGptTrack        *self,
                                const MusicianGptTuning *tunings,
                                gsize                    n_tunings)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_TRACK (self));
  g_return_if_fail (n_tunings == 0 || tunings != NULL);

  if (priv->tunings->len > 0)
    g_array_remove_range (priv->tunings, 0, priv->tunings->len);

  if (n_tunings > 0)
    g_array_append_vals (priv->tunings, tunings, n_tunings);
}

guint
musician_gpt_track_get_capo_at (MusicianGptTrack *self)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_TRACK (self), 0);

  return priv->capo_at;
}

guint
musician_gpt_track_get_channel (MusicianGptTrack *self)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_TRACK (self), 0);

  return priv->channel;
}

guint
musician_gpt_track_get_effects_channel (MusicianGptTrack *self)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_TRACK (self), 0);

  return priv->effects_channel;
}

guint
musician_gpt_track_get_n_frets (MusicianGptTrack *self)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_TRACK (self), 0);

  return priv->n_frets;
}

guint
musician_gpt_track_get_port (MusicianGptTrack *self)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_TRACK (self), 0);

  return priv->port;
}

void
musician_gpt_track_set_capo_at (MusicianGptTrack *self,
                                guint             capo_at)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_TRACK (self));

  if (capo_at != priv->capo_at)
    {
      priv->capo_at = capo_at;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_CAPO_AT]);
    }
}

void
musician_gpt_track_set_channel (MusicianGptTrack *self,
                                guint             channel)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_TRACK (self));

  if (priv->channel != channel)
    {
      priv->channel = channel;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_CHANNEL]);
    }
}

void
musician_gpt_track_set_effects_channel (MusicianGptTrack *self,
                                        guint             effects_channel)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_TRACK (self));

  if (priv->effects_channel != effects_channel)
    {
      priv->effects_channel = effects_channel;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_EFFECTS_CHANNEL]);
    }
}

void
musician_gpt_track_set_n_frets (MusicianGptTrack *self,
                                guint             n_frets)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_TRACK (self));

  if (priv->n_frets != n_frets)
    {
      priv->n_frets = n_frets;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_N_FRETS]);
    }
}

void
musician_gpt_track_set_port (MusicianGptTrack *self,
                             guint             port)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_TRACK (self));

  if (priv->port != port)
    {
      priv->port = port;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_PORT]);
    }
}

guint
musician_gpt_track_get_n_strings (MusicianGptTrack *self)
{
  MusicianGptTrackPrivate *priv = musician_gpt_track_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_TRACK (self), 0);

  return priv->tunings->len;
}
