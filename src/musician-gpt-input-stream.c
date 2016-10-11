/* musician-gpt-input-stream.c
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

#define G_LOG_DOMAIN "musician-gpt-input-stream"

#include <string.h>

#include "musician-gpt-input-stream.h"

/**
 * SECTION:musician-gpt-input-stream:
 * @title: #MusicianGptInputStream
 * @short_description: An input stream to read Guitar Pro™ files
 *
 * This #GInputStream implementation can be used to read the underlying
 * types found in various Guitar Pro™ files. This is used by #MusicianGptParser
 * to parse a document and convert it into a song format that gnome-musician
 * can work with.
 *
 * For more information on the format, see:
 *
 *   http://dguitar.sourceforge.net/GP4format.html#Summary
 *
 * Guitar Pro is a trademark of Arobas Music.
 */

typedef struct
{
  /*
   * We track how much memory we've allocated creating items from
   * the input stream, so that we can prevent allocating big chunks
   * of memory when coming across a potentially mallicous file.
   */
  gsize allocated;

  /*
   * This is the max number of bytes we'll allow ourselves to allocate
   * in the process of extracting data from the stream. See
   * musician_gpt_input_stream_allocate() for where we enforce this.
   */
  gsize max_allocation;
} MusicianGptInputStreamPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (MusicianGptInputStream, musician_gpt_input_stream, G_TYPE_DATA_INPUT_STREAM)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

MusicianGptInputStream *
musician_gpt_input_stream_new (GInputStream *base_stream)
{
  g_return_val_if_fail (G_IS_INPUT_STREAM (base_stream), NULL);

  return g_object_new (MUSICIAN_TYPE_GPT_INPUT_STREAM,
                       "base-stream", base_stream,
                       NULL);
}

static gpointer
musician_gpt_input_stream_allocate (MusicianGptInputStream  *self,
                                    gsize                    n_bytes,
                                    GError                 **error)
{
  MusicianGptInputStreamPrivate *priv = musician_gpt_input_stream_get_instance_private (self);
  gpointer ret;

  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (self));

  if (n_bytes > (priv->max_allocation - priv->allocated))
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "Failed to allocate memory while decoding. The file may be corrupt.");
      return NULL;
    }

  ret = g_try_malloc (n_bytes);

  if (ret == NULL)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "Failed to allocate memory while decoding.");
      return NULL;
    }

  priv->allocated += n_bytes;

  return ret;
}

static gpointer
musician_gpt_input_stream_allocate_n (MusicianGptInputStream  *self,
                                      gsize                    element_size,
                                      gsize                    n_elements,
                                      GError                 **error)
{
  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (self));
  g_assert (element_size > 0);

  if (G_MAXSIZE / element_size < n_elements)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "Failed to allocate memory while decoding");
      return NULL;
    }

  return musician_gpt_input_stream_allocate (self, element_size * n_elements, error);
}

static void
musician_gpt_input_stream_finalize (GObject *object)
{
  G_OBJECT_CLASS (musician_gpt_input_stream_parent_class)->finalize (object);
}

static void
musician_gpt_input_stream_get_property (GObject    *object,
                                        guint       prop_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
  MusicianGptInputStream *self = MUSICIAN_GPT_INPUT_STREAM (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
musician_gpt_input_stream_set_property (GObject      *object,
                                        guint         prop_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
  MusicianGptInputStream *self = MUSICIAN_GPT_INPUT_STREAM (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
musician_gpt_input_stream_class_init (MusicianGptInputStreamClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = musician_gpt_input_stream_finalize;
  object_class->get_property = musician_gpt_input_stream_get_property;
  object_class->set_property = musician_gpt_input_stream_set_property;
}

static void
musician_gpt_input_stream_init (MusicianGptInputStream *self)
{
  MusicianGptInputStreamPrivate *priv = musician_gpt_input_stream_get_instance_private (self);

  /* Max to 100 Mbyte */
  priv->max_allocation = 1024 * 1024 * 1024;

  g_data_input_stream_set_byte_order (G_DATA_INPUT_STREAM (self),
                                      G_DATA_STREAM_BYTE_ORDER_LITTLE_ENDIAN);
}

/**
 * musician_gpt_input_stream_read_color:
 * @self: A #MusicianGptInputStream.
 * @cancellable: (nullable): A #GCancellable or %NULL.
 * @color: (out) (nullable): A location for a #GdkRGBA or %NULL.
 * @error: a location for a #GError, or %NULL.
 *
 * This function will try to read a color from the underlying stream
 * and store the value as a GdkRGBA.
 *
 * Colors are stored as 4-bytes in the underlying stream.
 *
 *   byte[0] => Red, 0-255
 *   byte[1] => Green, 0-255
 *   byte[2] => Blue, 0-255
 *   byte[3] => Unused, always zero
 *
 * Returns: %TRUE if successful, otherwise %FALSE and @error is set.
 */
gboolean
musician_gpt_input_stream_read_color (MusicianGptInputStream  *self,
                                      GCancellable            *cancellable,
                                      GdkRGBA                 *color,
                                      GError                 **error)
{
  gsize n_read;
  guint8 bytes[4];

  g_return_val_if_fail (MUSICIAN_IS_GPT_INPUT_STREAM (self), FALSE);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);
  g_return_val_if_fail (color != NULL, FALSE);

  if (!g_input_stream_read_all (G_INPUT_STREAM (self), bytes, G_N_ELEMENTS (bytes), &n_read, cancellable, error))
    return FALSE;

  if (n_read != G_N_ELEMENTS (bytes))
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "Failed to read enough data for color");
      return FALSE;
    }

  color->red = bytes[0] / 255.0;
  color->green = bytes[1] / 255.0;
  color->blue = bytes[2] / 255.0;
  color->alpha = 1.0;

  return TRUE;
}

/**
 * musician_gpt_input_stream_read_fixed_string:
 * @self: A #MusicianGptInputStream
 * @max_length: the max_length of the string
 * @cancellable: (nullable): A #GCancellable or %NULL.
 * @error: a location for a #GError or %NULL
 *
 * Reads a short string from the underlying stream. This requires that
 * the string is < 255 bytes and takes the form of a single byte (the
 * length) encoded in the stream followed by @length bytes.
 *
 * This is generally used for the file header.
 *
 * This method will ensure that the resulting string is valid UTF-8.
 *
 * Returns: A newly allocated string containing the string read from the
 *   underlying stream. The string is %NULL terminated.
 */
gchar *
musician_gpt_input_stream_read_fixed_string (MusicianGptInputStream  *self,
                                             guint8                   max_length,
                                             GCancellable            *cancellable,
                                             GError                 **error)
{
  g_autofree gchar *ret = NULL;
  GError *local_error = NULL;
  gsize len;
  gsize n_read;

  g_return_val_if_fail (MUSICIAN_IS_GPT_INPUT_STREAM (self), NULL);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), NULL);

  len = g_data_input_stream_read_byte (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return NULL;
    }

  if (len > max_length)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "The underlying string is larger than max_length");
      return NULL;
    }

  if (NULL == (ret = musician_gpt_input_stream_allocate (self, (gsize)max_length + 1, error)))
    return FALSE;

  if (!g_input_stream_read_all (G_INPUT_STREAM (self), ret, max_length, &n_read, cancellable, error))
    return FALSE;

  if (max_length != n_read)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "Failed to read string from underlying stream");
      return NULL;
    }

  ret[len] = '\0';

  if (!g_utf8_validate (ret, -1, NULL))
    {
      /*
       * TODO: Can we sanitize the UTF-8 string here so we can continue if
       *       we come across a bad string? I mean, of course we can, but is
       *       there an easy glib routine for it.
       */
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "Invalid UTF-8 in underlying string");
      return NULL;
    }

  return g_steal_pointer (&ret);
}

/**
 * musician_gpt_input_stream_read_note:
 * @self: #MusicianGptInputStream
 * @cancellable: (nullable): A #GCancellable
 * @note: (out) (nullable): A location for a #MusicianGptNote or %NULL.
 * @error: A location for a #GError or %NULL.
 *
 * Reads a note from the underlying stream.
 *
 * Notes are stored as 32-bit integers.
 *
 * Returns: %TRUE if successful; otherwise %FALSE and @error is set.
 */
gboolean
musician_gpt_input_stream_read_note (MusicianGptInputStream  *self,
                                     GCancellable            *cancellable,
                                     MusicianGptNote         *note,
                                     GError                 **error)
{
  GError *local_error = NULL;
  gint32 value;

  g_return_val_if_fail (MUSICIAN_IS_GPT_INPUT_STREAM (self), 0);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);

  value = g_data_input_stream_read_int32 (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      if (note != NULL)
        *note = 0;
      return FALSE;
    }

  if (note != NULL)
    *note = value;

  return TRUE;
}

gchar *
musician_gpt_input_stream_read_lyric (MusicianGptInputStream  *self,
                                      GCancellable            *cancellable,
                                      guint32                 *position,
                                      GError                 **error)
{
  g_autofree gchar *ret = NULL;
  GError *local_error = NULL;
  gsize n_read;
  guint32 len;
  guint32 real_position;

  g_return_val_if_fail (MUSICIAN_IS_GPT_INPUT_STREAM (self), NULL);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), NULL);

  /*
   * Before the string, appears to be the position of the lyric.
   * This likely matches up to a given measure that we should
   * display the lyrics above the music.
   */
  real_position = g_data_input_stream_read_uint32 (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return NULL;
    }

  if (position != NULL)
    *position = real_position;

  /*
   * Now decode the string, which is a 32-bit string length followed by
   * the string bytes themselves.
   */
  len = g_data_input_stream_read_uint32 (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return NULL;
    }

  if (len == G_MAXUINT32)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "Invalid string length");
      return NULL;
    }

  if (NULL == (ret = musician_gpt_input_stream_allocate (self, len + 1, error)))
    return NULL;

  if (!g_input_stream_read_all (G_INPUT_STREAM (self), ret, len, &n_read, cancellable, error))
    return NULL;

  if (n_read != len)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "Unexpected end of file");
      return NULL;
    }

  ret[len] = '\0';

  return g_steal_pointer (&ret);
}

gchar *
musician_gpt_input_stream_read_string (MusicianGptInputStream  *self,
                                       GCancellable            *cancellable,
                                       GError                 **error)
{
  g_autofree gchar *ret = NULL;
  GError *local_error = NULL;
  gsize n_read;
  guint32 len;
  guint32 plen;

  g_return_val_if_fail (MUSICIAN_IS_GPT_INPUT_STREAM (self), NULL);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), NULL);

  /*
   * So as idiotic as this sounds, there are two string lengths for
   * the string. A 32-bit string, (which is string length + 1), and
   * then a pascal like string which is a 1-byte length followed by
   * the string. So we waste 5 bytes of overhead for a 255-byte max
   * string.
   */

  len = g_data_input_stream_read_uint32 (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return NULL;
    }

  if (len == G_MAXUINT32)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "Invalid string length");
      return NULL;
    }

  plen = g_data_input_stream_read_byte (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return NULL;
    }

  if (plen + 1 != len)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "Corrupt or invalid data discovered");
      return NULL;
    }

  if (NULL == (ret = musician_gpt_input_stream_allocate (self, plen + 1, error)))
    return NULL;

  if (!g_input_stream_read_all (G_INPUT_STREAM (self), ret, plen, &n_read, cancellable, error))
    return NULL;

  if (n_read != plen)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "Unexpected end of file");
      return NULL;
    }

  ret[plen] = '\0';

  return g_steal_pointer (&ret);
}

gchar **
musician_gpt_input_stream_read_string_array (MusicianGptInputStream  *self,
                                             GCancellable            *cancellable,
                                             GError                 **error)
{
  g_auto(GStrv) ret = NULL;
  GError *local_error = NULL;
  gint32 n_strings;

  g_return_val_if_fail (MUSICIAN_IS_GPT_INPUT_STREAM (self), NULL);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), NULL);

  n_strings = g_data_input_stream_read_int32 (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return NULL;
    }

  if (n_strings < 0)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_DATA,
                           "Invalid string array length");
      return NULL;
    }

  if (NULL == (ret = musician_gpt_input_stream_allocate_n (self, sizeof (gchar *), n_strings + 1, error)))
    return NULL;

  for (gint32 i = 0; i < n_strings; i++)
    {
      g_autofree gchar *str = NULL;

      if (NULL == (str = musician_gpt_input_stream_read_string (self, cancellable, error)))
        return NULL;

      ret[i] = g_steal_pointer (&str);
    }

  ret[n_strings] = NULL;

  return g_steal_pointer (&ret);
}

gboolean
musician_gpt_input_stream_read_triplet_feel  (MusicianGptInputStream  *self,
                                              GCancellable            *cancellable,
                                              MusicianGptTripletFeel  *triplet_feel,
                                              GError                 **error)
{
  GError *local_error = NULL;
  guint8 byte;

  g_return_val_if_fail (MUSICIAN_IS_GPT_INPUT_STREAM (self), FALSE);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);

  byte = g_data_input_stream_read_byte (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return FALSE;
    }

  if (triplet_feel != NULL)
    {
      if (byte == 0)
        *triplet_feel = MUSICIAN_GPT_TRIPLET_FEEL_NONE;
      else
        *triplet_feel = MUSICIAN_GPT_TRIPLET_FEEL_EIGHTH;
    }

  return TRUE;
}

gboolean
musician_gpt_input_stream_read_tempo (MusicianGptInputStream  *self,
                                      GCancellable            *cancellable,
                                      guint32                 *tempo,
                                      GError                 **error)
{
  GError *local_error = NULL;
  guint32 real_tempo;

  g_return_val_if_fail (MUSICIAN_IS_GPT_INPUT_STREAM (self), FALSE);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);

  real_tempo = g_data_input_stream_read_uint32 (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return FALSE;
    }

  if (tempo != NULL)
    *tempo = real_tempo;

  return TRUE;
}

gboolean
musician_gpt_input_stream_read_key (MusicianGptInputStream  *self,
                                    GCancellable            *cancellable,
                                    MusicianGptKey          *key,
                                    GError                 **error)
{
  GError *local_error = NULL;
  gint32 real_key;

  g_return_val_if_fail (MUSICIAN_IS_GPT_INPUT_STREAM (self), FALSE);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);

  real_key = g_data_input_stream_read_int32 (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return FALSE;
    }

  if (key != NULL)
    *key = real_key;

  return TRUE;
}

gboolean
musician_gpt_input_stream_read_octave (MusicianGptInputStream  *self,
                                       GCancellable            *cancellable,
                                       MusicianGptOctave       *octave,
                                       GError                 **error)
{
  GError *local_error = NULL;
  guint8 real_octave;

  g_return_val_if_fail (MUSICIAN_IS_GPT_INPUT_STREAM (self), FALSE);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);

  real_octave = g_data_input_stream_read_byte (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return FALSE;
    }

  if (octave != NULL)
    *octave = real_octave;

  return TRUE;
}

gboolean
musician_gpt_input_stream_read_int32 (MusicianGptInputStream  *self,
                                      GCancellable            *cancellable,
                                      gint32                  *value,
                                      GError                 **error)
{
  GError *local_error = NULL;
  gint32 real_value;

  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (self));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  real_value = g_data_input_stream_read_int32 (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return FALSE;
    }

  if (value != NULL)
    *value = real_value;

  return TRUE;
}

gboolean
musician_gpt_input_stream_read_uint32 (MusicianGptInputStream  *self,
                                       GCancellable            *cancellable,
                                       guint32                 *value,
                                       GError                 **error)
{
  GError *local_error = NULL;
  guint32 real_value;

  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (self));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  real_value = g_data_input_stream_read_uint32 (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return FALSE;
    }

  if (value != NULL)
    *value = real_value;

  return TRUE;
}

static gboolean
musician_gpt_input_stream_read_byte (MusicianGptInputStream  *self,
                                     GCancellable            *cancellable,
                                     guchar                  *value,
                                     GError                 **error)
{
  GError *local_error = NULL;
  guchar real_value;

  g_assert (MUSICIAN_IS_GPT_INPUT_STREAM (self));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  real_value = g_data_input_stream_read_byte (G_DATA_INPUT_STREAM (self), cancellable, &local_error);

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return FALSE;
    }

  if (value != NULL)
    *value = real_value;

  return TRUE;
}

gboolean
musician_gpt_input_stream_read_midi_port (MusicianGptInputStream  *self,
                                          guint                    portnum,
                                          GCancellable            *cancellable,
                                          MusicianGptMidiPort     *port,
                                          GError                 **error)
{
  MusicianGptMidiPort dummy_port;
  GError *local_error = NULL;

  g_return_val_if_fail (MUSICIAN_IS_GPT_INPUT_STREAM (self), FALSE);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);

  if (port == NULL)
    port = &dummy_port;

  port->port_id = portnum;

  for (guint i = 0; i < G_N_ELEMENTS (port->channels); i++)
    {
      port->channels[i].port_id = portnum;
      port->channels[i].channel_id = i + 1;

      if (!musician_gpt_input_stream_read_uint32 (self, cancellable, &port->channels[i].instrument, error) ||
          !musician_gpt_input_stream_read_byte (self, cancellable, &port->channels[i].volume, error) ||
          !musician_gpt_input_stream_read_byte (self, cancellable, &port->channels[i].balance, error) ||
          !musician_gpt_input_stream_read_byte (self, cancellable, &port->channels[i].chorus, error) ||
          !musician_gpt_input_stream_read_byte (self, cancellable, &port->channels[i].reverb, error) ||
          !musician_gpt_input_stream_read_byte (self, cancellable, &port->channels[i].phaser, error) ||
          !musician_gpt_input_stream_read_byte (self, cancellable, &port->channels[i].tremelo, error))
        return FALSE;

      if (!g_input_stream_skip (G_INPUT_STREAM (self), 2, cancellable, error))
        return FALSE;

      port->channels[i]._blank1 = 0;
      port->channels[i]._blank2 = 0;
    }

  return TRUE;
}
