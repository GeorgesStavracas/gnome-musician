/* musician-gpt-input-stream.h
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

#ifndef MUSICIAN_GPT_INPUT_STREAM_H
#define MUSICIAN_GPT_INPUT_STREAM_H

#include <gdk/gdk.h>

#include "musician-gpt-types.h"

G_BEGIN_DECLS

#define MUSICIAN_TYPE_GPT_INPUT_STREAM (musician_gpt_input_stream_get_type())

G_DECLARE_DERIVABLE_TYPE (MusicianGptInputStream, musician_gpt_input_stream, MUSICIAN, GPT_INPUT_STREAM, GDataInputStream)

struct _MusicianGptInputStreamClass
{
  GDataInputStreamClass parent_instance;

  gpointer _reserved1;
  gpointer _reserved2;
  gpointer _reserved3;
  gpointer _reserved4;
  gpointer _reserved5;
  gpointer _reserved6;
  gpointer _reserved7;
  gpointer _reserved8;
  gpointer _reserved9;
  gpointer _reserved10;
  gpointer _reserved11;
  gpointer _reserved12;
};

MusicianGptInputStream *musician_gpt_input_stream_new                (GInputStream            *base_stream);
gboolean                musician_gpt_input_stream_read_color         (MusicianGptInputStream  *self,
                                                                      GCancellable            *cancellable,
                                                                      GdkRGBA                 *color,
                                                                      GError                 **error);
gchar                  *musician_gpt_input_stream_read_fixed_string  (MusicianGptInputStream  *self,
                                                                      guint8                   max_length,
                                                                      GCancellable            *cancellable,
                                                                      GError                 **error);
gchar                  *musician_gpt_input_stream_read_string        (MusicianGptInputStream  *self,
                                                                      GCancellable            *cancellable,
                                                                      GError                 **error);
gchar                 **musician_gpt_input_stream_read_string_array  (MusicianGptInputStream  *self,
                                                                      GCancellable            *cancellable,
                                                                      GError                 **error);
gchar                  *musician_gpt_input_stream_read_lyric         (MusicianGptInputStream  *self,
                                                                      GCancellable            *cancellable,
                                                                      guint32                 *position,
                                                                      GError                 **error);
gboolean                musician_gpt_input_stream_read_triplet_feel  (MusicianGptInputStream  *self,
                                                                      GCancellable            *cancellable,
                                                                      MusicianGptTripletFeel  *triplet_feel,
                                                                      GError                 **error);
gboolean                musician_gpt_input_stream_read_note          (MusicianGptInputStream  *self,
                                                                      GCancellable            *cancellable,
                                                                      MusicianGptNote         *note,
                                                                      GError                 **error);
gboolean                musician_gpt_input_stream_read_tempo         (MusicianGptInputStream  *self,
                                                                      GCancellable            *cancellable,
                                                                      guint32                 *tempo,
                                                                      GError                 **error);
gboolean                musician_gpt_input_stream_read_key           (MusicianGptInputStream  *self,
                                                                      GCancellable            *cancellable,
                                                                      MusicianGptKey          *key,
                                                                      GError                 **error);
gboolean                musician_gpt_input_stream_read_octave        (MusicianGptInputStream  *self,
                                                                      GCancellable            *cancellable,
                                                                      MusicianGptOctave       *octave,
                                                                      GError                 **error);
gboolean                musician_gpt_input_stream_read_midi_port     (MusicianGptInputStream  *self,
                                                                      guint                    portnum,
                                                                      GCancellable            *cancellable,
                                                                      MusicianGptMidiPort     *port,
                                                                      GError                 **error);
gboolean                musician_gpt_input_stream_read_byte          (MusicianGptInputStream  *self,
                                                                      GCancellable            *cancellable,
                                                                      guchar                  *value,
                                                                      GError                 **error);
gboolean                musician_gpt_input_stream_read_int32         (MusicianGptInputStream  *self,
                                                                      GCancellable            *cancellable,
                                                                      gint32                  *value,
                                                                      GError                 **error);
gboolean                musician_gpt_input_stream_read_uint32        (MusicianGptInputStream  *self,
                                                                      GCancellable            *cancellable,
                                                                      guint32                 *value,
                                                                      GError                 **error);

G_END_DECLS

#endif /* MUSICIAN_GPT_INPUT_STREAM_H */
