/* musician-gpt-measure.c
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

#define G_LOG_DOMAIN "musician-gpt-measure"

#include "musician-gpt-measure.h"

typedef struct
{
  gchar *marker_name;
  GdkRGBA marker_color;
  guint id;
  guint denominator;
  guint numerator;
  guint nth_ending;
  guint n_repeats;
  MusicianGptKey key;
} MusicianGptMeasurePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (MusicianGptMeasure, musician_gpt_measure, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_DENOMINATOR,
  PROP_ID,
  PROP_KEY,
  PROP_MARKER_NAME,
  PROP_MARKER_COLOR,
  PROP_N_REPEATS,
  PROP_NTH_ENDING,
  PROP_NUMERATOR,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
musician_gpt_measure_finalize (GObject *object)
{
  MusicianGptMeasure *self = (MusicianGptMeasure *)object;
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_clear_pointer (&priv->marker_name, g_free);

  G_OBJECT_CLASS (musician_gpt_measure_parent_class)->finalize (object);
}

static void
musician_gpt_measure_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  MusicianGptMeasure *self = MUSICIAN_GPT_MEASURE (object);

  switch (prop_id)
    {
    case PROP_DENOMINATOR:
      g_value_set_uint (value, musician_gpt_measure_get_denominator (self));
      break;

    case PROP_ID:
      g_value_set_uint (value, musician_gpt_measure_get_id (self));
      break;

    case PROP_KEY:
      g_value_set_enum (value, musician_gpt_measure_get_key (self));
      break;

    case PROP_MARKER_NAME:
      g_value_set_string (value, musician_gpt_measure_get_marker_name (self));
      break;

    case PROP_MARKER_COLOR:
      g_value_set_boxed (value, musician_gpt_measure_get_marker_color (self));
      break;

    case PROP_N_REPEATS:
      g_value_set_uint (value, musician_gpt_measure_get_n_repeats (self));
      break;

    case PROP_NTH_ENDING:
      g_value_set_uint (value, musician_gpt_measure_get_nth_ending (self));
      break;

    case PROP_NUMERATOR:
      g_value_set_uint (value, musician_gpt_measure_get_numerator (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
musician_gpt_measure_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  MusicianGptMeasure *self = MUSICIAN_GPT_MEASURE (object);

  switch (prop_id)
    {
    case PROP_DENOMINATOR:
      musician_gpt_measure_set_denominator (self, g_value_get_uint (value));
      break;

    case PROP_ID:
      musician_gpt_measure_set_id (self, g_value_get_uint (value));
      break;

    case PROP_KEY:
      musician_gpt_measure_set_key (self, g_value_get_enum (value));
      break;

    case PROP_MARKER_NAME:
      musician_gpt_measure_set_marker_name (self, g_value_get_string (value));
      break;

    case PROP_MARKER_COLOR:
      musician_gpt_measure_set_marker_color (self, g_value_get_boxed (value));
      break;

    case PROP_N_REPEATS:
      musician_gpt_measure_set_n_repeats (self, g_value_get_uint (value));
      break;

    case PROP_NTH_ENDING:
      musician_gpt_measure_set_nth_ending (self, g_value_get_uint (value));
      break;

    case PROP_NUMERATOR:
      musician_gpt_measure_set_numerator (self, g_value_get_uint (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
musician_gpt_measure_class_init (MusicianGptMeasureClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = musician_gpt_measure_finalize;
  object_class->get_property = musician_gpt_measure_get_property;
  object_class->set_property = musician_gpt_measure_set_property;

  properties [PROP_DENOMINATOR] =
    g_param_spec_uint ("denominator",
                       "Denominator",
                       "Denominator",
                       0,
                       G_MAXUINT,
                       4,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_ID] =
    g_param_spec_uint ("id",
                       "Id",
                       "Id",
                       0,
                       G_MAXUINT,
                       0,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_KEY] =
    g_param_spec_enum ("key",
                       "Key",
                       "Key",
                       MUSICIAN_TYPE_GPT_KEY,
                       MUSICIAN_GPT_KEY_C,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_MARKER_NAME] =
    g_param_spec_string ("marker-name",
                         "Marker Name",
                         "Marker Name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_MARKER_COLOR] =
    g_param_spec_boxed ("marker-color",
                        "Marker Color",
                        "Marker Color",
                        GDK_TYPE_RGBA,
                        (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_N_REPEATS] =
    g_param_spec_uint ("n-repeats",
                       "N Repeats",
                       "N Repeats",
                       0,
                       G_MAXSHORT,
                       0,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_NTH_ENDING] =
    g_param_spec_uint ("nth-ending",
                       "Nth Ending",
                       "Nth Ending",
                       0,
                       G_MAXSHORT,
                       0,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_NUMERATOR] =
    g_param_spec_uint ("numerator",
                       "Numerator",
                       "Numerator",
                       0,
                       G_MAXUINT,
                       4,
                       (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));


  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
musician_gpt_measure_init (MusicianGptMeasure *self)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  priv->numerator = 4;
  priv->denominator = 4;
}

MusicianGptMeasure *
musician_gpt_measure_new (void)
{
  return g_object_new (MUSICIAN_TYPE_GPT_MEASURE, NULL);
}

guint
musician_gpt_measure_get_denominator (MusicianGptMeasure *self)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_MEASURE (self), 0);

  return priv->denominator;
}

void
musician_gpt_measure_set_denominator (MusicianGptMeasure *self,
                                      guint               denominator)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_MEASURE (self));

  if (priv->denominator != denominator)
    {
      priv->denominator = denominator;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_DENOMINATOR]);
    }
}

guint
musician_gpt_measure_get_id (MusicianGptMeasure *self)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_MEASURE (self), 0);

  return priv->id;
}

void
musician_gpt_measure_set_id (MusicianGptMeasure *self,
                             guint               id)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_MEASURE (self));

  if (priv->id != id)
    {
      priv->id = id;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ID]);
    }
}

MusicianGptKey
musician_gpt_measure_get_key (MusicianGptMeasure *self)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_MEASURE (self), 0);

  return priv->key;
}

void
musician_gpt_measure_set_key (MusicianGptMeasure *self,
                              MusicianGptKey      key)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_MEASURE (self));

  if (priv->key != key)
    {
      priv->key = key;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_KEY]);
    }
}

const gchar *
musician_gpt_measure_get_marker_name (MusicianGptMeasure *self)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_MEASURE (self), NULL);

  return priv->marker_name;
}

void
musician_gpt_measure_set_marker_name (MusicianGptMeasure *self,
                                      const gchar        *marker_name)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_MEASURE (self));

  if (g_strcmp0 (priv->marker_name, marker_name) != 0)
    {
      g_free (priv->marker_name);
      priv->marker_name = g_strdup (marker_name);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_MARKER_NAME]);
    }
}

const GdkRGBA *
musician_gpt_measure_get_marker_color (MusicianGptMeasure *self)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_MEASURE (self), NULL);

  return &priv->marker_color;
}

void
musician_gpt_measure_set_marker_color (MusicianGptMeasure *self,
                                       const GdkRGBA      *marker_color)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);
  static const GdkRGBA empty = { 0 };

  g_return_if_fail (MUSICIAN_IS_GPT_MEASURE (self));

  if (marker_color == NULL)
    marker_color = &empty;

  if (!gdk_rgba_equal (&priv->marker_color, marker_color))
    {
      priv->marker_color = *marker_color;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_MARKER_COLOR]);
    }
}

guint
musician_gpt_measure_get_n_repeats (MusicianGptMeasure *self)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_MEASURE (self), 0);

  return priv->n_repeats;
}

void
musician_gpt_measure_set_n_repeats (MusicianGptMeasure *self,
                                    guint               n_repeats)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_MEASURE (self));

  if (priv->n_repeats != n_repeats)
    {
      priv->n_repeats = n_repeats;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_N_REPEATS]);
    }
}

guint
musician_gpt_measure_get_nth_ending (MusicianGptMeasure *self)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_MEASURE (self), 0);

  return priv->nth_ending;
}

void
musician_gpt_measure_set_nth_ending (MusicianGptMeasure *self,
                                     guint               nth_ending)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_MEASURE (self));

  if (priv->nth_ending != nth_ending)
    {
      priv->nth_ending = nth_ending;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_NTH_ENDING]);
    }
}

guint
musician_gpt_measure_get_numerator (MusicianGptMeasure *self)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_MEASURE (self), 0);

  return priv->numerator;
}

void
musician_gpt_measure_set_numerator (MusicianGptMeasure *self,
                                    guint               numerator)
{
  MusicianGptMeasurePrivate *priv = musician_gpt_measure_get_instance_private (self);

  g_return_if_fail (MUSICIAN_IS_GPT_MEASURE (self));

  if (priv->numerator != numerator)
    {
      priv->numerator = numerator;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_NUMERATOR]);
    }
}

gint
musician_gpt_measure_compare (const MusicianGptMeasure *a,
                              const MusicianGptMeasure *b)
{
  MusicianGptMeasurePrivate *priva = musician_gpt_measure_get_instance_private ((MusicianGptMeasure *)a);
  MusicianGptMeasurePrivate *privb = musician_gpt_measure_get_instance_private ((MusicianGptMeasure *)b);

  return (gint)priva->id - (gint)privb->id;
}
