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

#include <glib/gi18n.h>

#include "musician-gp4-parser.h"
#include "musician-gpt-parser.h"
#include "musician-gpt-song.h"

typedef struct
{
  MusicianGptSong *song;
} MusicianGptParserPrivate;

enum {
  PROP_0,
  PROP_SONG,
  N_PROPS
};

G_DEFINE_TYPE_WITH_PRIVATE (MusicianGptParser, musician_gpt_parser, G_TYPE_OBJECT)

static GParamSpec *properties [N_PROPS];

static MusicianGptSong *
musician_gpt_parser_real_load (MusicianGptParser       *self,
                               MusicianGptInputStream  *stream,
                               const gchar             *version,
                               GCancellable            *cancellable,
                               GError                 **error)
{
  g_autoptr(MusicianGptParser) subparser = NULL;
  GType type_id = G_TYPE_NONE;
  struct {
    const gchar *version;
    GType        type_id;
  } mappings[] = {
    { "FICHIER GUITAR PRO v4.00", MUSICIAN_TYPE_GP4_PARSER },
    { "FICHIER GUITAR PRO v4.06", MUSICIAN_TYPE_GP4_PARSER },
    { "FICHIER GUITAR PRO L4.06", MUSICIAN_TYPE_GP4_PARSER },
  };

  g_assert (MUSICIAN_IS_GPT_PARSER (self));
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (stream));
  g_assert (version != NULL);
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  /*
   * This function serves as a default implementation of ::load that can
   * sniff the proper subclass based on @version. This is helpful because
   * it doesn't require consumers of the parser to know which exact parser
   * to use, but allow us to separate code into different subclasses for
   * maintainability.
   */

  for (guint i = 0; i < G_N_ELEMENTS (mappings); i++)
    {
      if (g_str_equal (version, mappings[i].version))
        {
          type_id = mappings[i].type_id;
          break;
        }
    }

  if (!g_type_is_a (type_id, MUSICIAN_TYPE_GPT_PARSER))
    {
      g_set_error (error,
                   G_IO_ERROR,
                   G_IO_ERROR_NOT_SUPPORTED,
                   "The file format \"%s\" is not supported",
                   version);
      return NULL;
    }

  subparser = g_object_new (type_id, NULL);

  /*
   * Double check that the subclass did in fact override this function
   * or else we just error out to prevent a stack overflow and instead
   * provide a somewhat more helpful error message.
   */
  if (MUSICIAN_GPT_PARSER_GET_CLASS (subparser)->load == musician_gpt_parser_real_load)
    {
      g_set_error (error,
                   G_IO_ERROR,
                   G_IO_ERROR_INVAL,
                   "%s failed to override MusicianGptParserClass::load",
                   G_OBJECT_TYPE_NAME (subparser));
      return NULL;
    }

  return MUSICIAN_GPT_PARSER_GET_CLASS (subparser)->load (subparser, stream, version, cancellable, error);
}

static void
musician_gpt_parser_finalize (GObject *object)
{
  MusicianGptParser *self = (MusicianGptParser *)object;
  MusicianGptParserPrivate *priv = musician_gpt_parser_get_instance_private (self);

  g_clear_object (&priv->song);

  G_OBJECT_CLASS (musician_gpt_parser_parent_class)->finalize (object);
}

static void
musician_gpt_parser_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  MusicianGptParser *self = MUSICIAN_GPT_PARSER (object);

  switch (prop_id)
    {
    case PROP_SONG:
      g_value_set_object (value, musician_gpt_parser_get_song (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
musician_gpt_parser_class_init (MusicianGptParserClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = musician_gpt_parser_finalize;
  object_class->get_property = musician_gpt_parser_get_property;

  klass->load = musician_gpt_parser_real_load;

  properties [PROP_SONG] =
    g_param_spec_object ("song",
                         "Song",
                         "The song that has been loaded",
                         MUSICIAN_TYPE_GPT_SONG,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
musician_gpt_parser_init (MusicianGptParser *self)
{
}

MusicianGptParser *
musician_gpt_parser_new (void)
{
  return g_object_new (MUSICIAN_TYPE_GPT_PARSER, NULL);
}

/**
 * musician_gpt_parser_get_song:
 *
 * Returns: (nullable) (transfer none): A #MusicianGptSong or %NULL.
 */
MusicianGptSong *
musician_gpt_parser_get_song (MusicianGptParser *self)
{
  MusicianGptParserPrivate *priv = musician_gpt_parser_get_instance_private (self);

  g_return_val_if_fail (MUSICIAN_IS_GPT_PARSER (self), NULL);

  return priv->song;
}

gboolean
musician_gpt_parser_load_from_file (MusicianGptParser  *self,
                                    GFile              *file,
                                    GCancellable       *cancellable,
                                    GError            **error)
{
  g_autoptr(GFileInputStream) stream = NULL;

  g_return_val_if_fail (MUSICIAN_IS_GPT_PARSER (self), FALSE);
  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);

  if (NULL != (stream = g_file_read (file, cancellable, error)))
    return musician_gpt_parser_load_from_stream (self, G_INPUT_STREAM (stream), cancellable, error);

  return FALSE;
}

gboolean
musician_gpt_parser_load_from_stream (MusicianGptParser  *self,
                                      GInputStream       *base_stream,
                                      GCancellable       *cancellable,
                                      GError            **error)
{
  MusicianGptParserPrivate *priv = musician_gpt_parser_get_instance_private (self);
  g_autoptr(MusicianGptInputStream) stream = NULL;
  g_autoptr(MusicianGptSong) song = NULL;
  g_autofree gchar *version = NULL;
  GType file_format;

  g_return_val_if_fail (MUSICIAN_IS_GPT_PARSER (self), FALSE);
  g_return_val_if_fail (G_IS_INPUT_STREAM (base_stream), FALSE);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);

  if (priv->song != NULL)
    {
      g_set_error (error,
                   G_IO_ERROR,
                   G_IO_ERROR_INVAL,
                   "Cannot use parser more than once");
      return FALSE;
    }

  /* Create our wrapper stream to read Guitar Pro formats */
  stream = musician_gpt_input_stream_new (base_stream);

  /* Read the version string so we can dispatch to the proper loader.
   * Our default load implementation will lookup based on known subclasses
   * and dispatch to a subparser to perform the parse. To force a specific
   * version loader, just use that subclass (such as MusicianGp4Parser).
   */
  version = musician_gpt_input_stream_read_fixed_string (stream, 30, cancellable, error);

  /* Let our potential subclass override the parsing process */
  song = MUSICIAN_GPT_PARSER_GET_CLASS (self)->load (self, stream, version, cancellable, error);

  if (song != NULL)
    {
      g_clear_object (&priv->song);
      priv->song = g_steal_pointer (&song);
      return TRUE;
    }

  return FALSE;
}
