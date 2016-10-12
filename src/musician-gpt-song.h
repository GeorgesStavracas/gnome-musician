/* musician-gpt-song.h
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

#ifndef MUSICIAN_GPT_SONG_H
#define MUSICIAN_GPT_SONG_H

#include <gio/gio.h>

#include "musician-gpt-types.h"

G_BEGIN_DECLS

#define MUSICIAN_TYPE_GPT_SONG (musician_gpt_song_get_type())

G_DECLARE_DERIVABLE_TYPE (MusicianGptSong, musician_gpt_song, MUSICIAN, GPT_SONG, GObject)

struct _MusicianGptSongClass
{
  GObjectClass parent_class;

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

MusicianGptSong        *musician_gpt_song_new                (void);
const gchar            *musician_gpt_song_get_album          (MusicianGptSong        *self);
const gchar            *musician_gpt_song_get_artist         (MusicianGptSong        *self);
const gchar            *musician_gpt_song_get_copyright      (MusicianGptSong        *self);
const gchar            *musician_gpt_song_get_interpretation (MusicianGptSong        *self);
const gchar            *musician_gpt_song_get_instructions   (MusicianGptSong        *self);
MusicianGptKey          musician_gpt_song_get_key            (MusicianGptSong        *self);
MusicianGptOctave       musician_gpt_song_get_octave         (MusicianGptSong        *self);
const gchar            *musician_gpt_song_get_subtitle       (MusicianGptSong        *self);
guint                   musician_gpt_song_get_tempo          (MusicianGptSong        *self);
const gchar            *musician_gpt_song_get_title          (MusicianGptSong        *self);
MusicianGptTripletFeel  musician_gpt_song_get_triplet_feel   (MusicianGptSong        *self);
const gchar            *musician_gpt_song_get_version        (MusicianGptSong        *self);
const gchar            *musician_gpt_song_get_writer         (MusicianGptSong        *self);
void                    musician_gpt_song_set_album          (MusicianGptSong        *self,
                                                              const gchar            *album);
void                    musician_gpt_song_set_artist         (MusicianGptSong        *self,
                                                              const gchar            *artist);
void                    musician_gpt_song_set_copyright      (MusicianGptSong        *self,
                                                              const gchar            *copyright);
void                    musician_gpt_song_set_instructions   (MusicianGptSong        *self,
                                                              const gchar            *instructions);
void                    musician_gpt_song_set_interpretation (MusicianGptSong        *self,
                                                              const gchar            *interpretation);
void                    musician_gpt_song_set_octave         (MusicianGptSong        *self,
							      MusicianGptOctave       octave);
void                    musician_gpt_song_set_key            (MusicianGptSong        *self,
							      MusicianGptKey          key);
void                    musician_gpt_song_set_subtitle       (MusicianGptSong        *self,
                                                              const gchar            *subtitle);
void                    musician_gpt_song_set_tempo          (MusicianGptSong        *self,
                                                              guint                   tempo);
void                    musician_gpt_song_set_title          (MusicianGptSong        *self,
                                                              const gchar            *title);
void                    musician_gpt_song_set_triplet_feel   (MusicianGptSong        *self,
                                                              MusicianGptTripletFeel  triplet_feel);
void                    musician_gpt_song_set_writer         (MusicianGptSong        *self,
                                                              const gchar            *writer);

G_END_DECLS

#endif /* MUSICIAN_GPT_SONG_H */
