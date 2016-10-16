/* musician-gpt-measure.h
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

#ifndef MUSICIAN_GPT_MEASURE_H
#define MUSICIAN_GPT_MEASURE_H

#include <gdk/gdk.h>

#include "musician-gpt-types.h"

G_BEGIN_DECLS

#define MUSICIAN_TYPE_GPT_MEASURE (musician_gpt_measure_get_type())

G_DECLARE_DERIVABLE_TYPE (MusicianGptMeasure, musician_gpt_measure, MUSICIAN, GPT_MEASURE, GObject)

struct _MusicianGptMeasureClass
{
  GObjectClass parent_class;
};

gint                musician_gpt_measure_compare          (const MusicianGptMeasure *a,
                                                           const MusicianGptMeasure *b);
MusicianGptMeasure *musician_gpt_measure_new              (void);
guint               musician_gpt_measure_get_id           (MusicianGptMeasure       *self);
void                musician_gpt_measure_set_id           (MusicianGptMeasure       *self,
                                                           guint                     id);
guint               musician_gpt_measure_get_nth_ending   (MusicianGptMeasure       *self);
void                musician_gpt_measure_set_nth_ending   (MusicianGptMeasure       *self,
                                                           guint                     nth_ending);
guint               musician_gpt_measure_get_n_repeats    (MusicianGptMeasure       *self);
void                musician_gpt_measure_set_n_repeats    (MusicianGptMeasure       *self,
                                                           guint                     n_repeats);
guint               musician_gpt_measure_get_denominator  (MusicianGptMeasure       *self);
void                musician_gpt_measure_set_denominator  (MusicianGptMeasure       *self,
                                                           guint                     denominator);
guint               musician_gpt_measure_get_numerator    (MusicianGptMeasure       *self);
void                musician_gpt_measure_set_numerator    (MusicianGptMeasure       *self,
                                                           guint                     numerator);
const gchar        *musician_gpt_measure_get_marker_name  (MusicianGptMeasure       *self);
void                musician_gpt_measure_set_marker_name  (MusicianGptMeasure       *self,
                                                           const gchar              *marker_name);
const GdkRGBA      *musician_gpt_measure_get_marker_color (MusicianGptMeasure       *self);
void                musician_gpt_measure_set_marker_color (MusicianGptMeasure       *self,
                                                           const GdkRGBA            *marker_color);
MusicianGptKey      musician_gpt_measure_get_key          (MusicianGptMeasure       *self);
void                musician_gpt_measure_set_key          (MusicianGptMeasure       *self,
                                                           MusicianGptKey            key);

G_END_DECLS

#endif /* MUSICIAN_GPT_MEASURE_H */
