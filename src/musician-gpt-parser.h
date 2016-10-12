/* musician-gpt-parser.h
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

#ifndef MUSICIAN_GPT_PARSER_H
#define MUSICIAN_GPT_PARSER_H

#include <gio/gio.h>

#include "musician-gpt-input-stream.h"
#include "musician-gpt-types.h"

G_BEGIN_DECLS

#define MUSICIAN_TYPE_GPT_PARSER (musician_gpt_parser_get_type())

G_DECLARE_DERIVABLE_TYPE (MusicianGptParser, musician_gpt_parser, MUSICIAN, GPT_PARSER, GObject)

struct _MusicianGptParserClass
{
  GObjectClass parent_instance;

  MusicianGptSong *(*load) (MusicianGptParser       *self,
                            MusicianGptInputStream  *stream,
                            const gchar             *version,
                            GCancellable            *cancellable,
                            GError                 **error);

  gpointer _reserved1;
  gpointer _reserved2;
  gpointer _reserved3;
  gpointer _reserved4;
};

MusicianGptParser *musician_gpt_parser_new              (void);
MusicianGptSong   *musician_gpt_parser_get_song         (MusicianGptParser   *self);
gboolean           musician_gpt_parser_load_from_stream (MusicianGptParser   *self,
                                                         GInputStream        *base_stream,
                                                         GCancellable        *cancellable,
                                                         GError             **error);
gboolean           musician_gpt_parser_load_from_file   (MusicianGptParser   *self,
                                                         GFile               *file,
                                                         GCancellable        *cancellable,
                                                         GError             **error);

G_END_DECLS

#endif /* MUSICIAN_GPT_PARSER_H */
