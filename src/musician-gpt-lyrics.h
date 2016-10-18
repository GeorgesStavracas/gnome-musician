/* musician-gpt-lyrics.h
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

#ifndef MUSICIAN_GPT_LYRICS_H
#define MUSICIAN_GPT_LYRICS_H

#include <gio/gio.h>

G_BEGIN_DECLS

#define MUSICIAN_TYPE_GPT_LYRICS (musician_gpt_lyrics_get_type())

G_DECLARE_DERIVABLE_TYPE (MusicianGptLyrics, musician_gpt_lyrics, MUSICIAN, GPT_LYRICS, GObject)

struct _MusicianGptLyricsClass
{
  GObjectClass parent_class;

  gpointer _reserved1;
  gpointer _reserved2;
  gpointer _reserved3;
  gpointer _reserved4;
};

MusicianGptLyrics *musician_gpt_lyrics_new          (void);
guint              musician_gpt_lyrics_get_position (MusicianGptLyrics *self);
void               musician_gpt_lyrics_set_position (MusicianGptLyrics *self,
                                                     guint              position);
const gchar       *musician_gpt_lyrics_get_text     (MusicianGptLyrics *self);
void               musician_gpt_lyrics_set_text     (MusicianGptLyrics *self,
                                                     const gchar       *text);

G_END_DECLS

#endif /* MUSICIAN_GPT_LYRICS_H */
