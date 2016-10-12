/* musician-gpt-song-private.h
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

#ifndef MUSICIAN_GPT_SONG_PRIVATE_H
#define MUSICIAN_GPT_SONG_PRIVATE_H

#include "musician-gpt-song.h"

G_BEGIN_DECLS

void _musician_gpt_song_set_midi_ports (MusicianGptSong           *self,
                                        const MusicianGptMidiPort *ports,
                                        gsize                      n_ports);
void _musician_gpt_song_add_lyrics     (MusicianGptSong           *self,
                                        guint                      position,
                                        const gchar               *lyrics);
void _musician_gpt_song_set_version    (MusicianGptSong           *self,
                                        const gchar               *version);

G_END_DECLS

#endif /* MUSICIAN_GPT_SONG_PRIVATE_H */
