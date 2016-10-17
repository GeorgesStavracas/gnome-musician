/* musician-gpt-chord.h
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

#ifndef MUSICIAN_GPT_CHORD_H
#define MUSICIAN_GPT_CHORD_H

#include <gio/gio.h>

#include "musician-gpt-types.h"

G_BEGIN_DECLS

#define MUSICIAN_TYPE_GPT_CHORD (musician_gpt_chord_get_type())

GType             musician_gpt_chord_get_type (void);
MusicianGptChord *musician_gpt_chord_new      (void);
MusicianGptChord *musician_gpt_chord_ref      (MusicianGptChord *self);
void              musician_gpt_chord_unref    (MusicianGptChord *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (MusicianGptChord, musician_gpt_chord_unref)

G_END_DECLS

#endif /* MUSICIAN_GPT_CHORD_H */
