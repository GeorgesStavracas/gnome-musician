/* musician-gpt-beat.h
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

#ifndef MUSICIAN_GPT_BEAT_H
#define MUSICIAN_GPT_BEAT_H

#include <gio/gio.h>

#include "musician-gpt-types.h"

G_BEGIN_DECLS

#define MUSICIAN_TYPE_GPT_BEAT (musician_gpt_beat_get_type())

GType                musician_gpt_beat_get_type     (void);
MusicianGptBeat     *musician_gpt_beat_new          (void);
MusicianGptBeat     *musician_gpt_beat_ref          (MusicianGptBeat     *self);
void                 musician_gpt_beat_unref        (MusicianGptBeat     *self);
MusicianGptBeatMode  musician_gpt_beat_get_mode     (MusicianGptBeat     *self);
void                 musician_gpt_beat_set_mode     (MusicianGptBeat     *self,
                                                     MusicianGptBeatMode  mode);
guint                musician_gpt_beat_get_duration (MusicianGptBeat     *self);
void                 musician_gpt_beat_set_duration (MusicianGptBeat     *self,
                                                     guint                duration);
guint                musician_gpt_beat_get_n_tuplet (MusicianGptBeat     *self);
void                 musician_gpt_beat_set_n_tuplet (MusicianGptBeat     *self,
                                                     guint                n_tuplet);
MusicianGptChord    *musician_gpt_beat_get_chord    (MusicianGptBeat     *self);
void                 musician_gpt_beat_set_chord    (MusicianGptBeat     *self,
                                                     MusicianGptChord    *chord);
const gchar         *musician_gpt_beat_get_text     (MusicianGptBeat     *self);
void                 musician_gpt_beat_set_text     (MusicianGptBeat     *self,
                                                     const gchar         *text);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (MusicianGptBeat, musician_gpt_beat_unref)

G_END_DECLS

#endif /* MUSICIAN_GPT_BEAT_H */
