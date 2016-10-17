/* musician-gpt-bend.h
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

#ifndef MUSICIAN_GPT_BEND_H
#define MUSICIAN_GPT_BEND_H

#include <gio/gio.h>

#include "musician-gpt-types.h"

G_BEGIN_DECLS

#define MUSICIAN_TYPE_GPT_BEND (musician_gpt_bend_get_type())

MusicianGptBend     *musician_gpt_bend_new           (void);
MusicianGptBend     *musician_gpt_bend_ref           (MusicianGptBend            *self);
void                 musician_gpt_bend_unref         (MusicianGptBend            *self);
MusicianGptBendType  musician_gpt_bend_get_bend_type (MusicianGptBend            *self);
void                 musician_gpt_bend_set_bend_type (MusicianGptBend            *self,
                                                      MusicianGptBendType         bend_type);
void                 musician_gpt_bend_add_point     (MusicianGptBend            *self,
                                                      const MusicianGptBendPoint *point);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (MusicianGptBend, musician_gpt_bend_unref)

G_END_DECLS

#endif /* MUSICIAN_GPT_BEND_H */
