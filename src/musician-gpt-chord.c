/* musician-gpt-chord.c
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

#define G_LOG_DOMAIN "musician-gpt-chord"

#include "musician-gpt-chord.h"

struct _MusicianGptChord
{
  volatile gint ref_count;
};

G_DEFINE_BOXED_TYPE (MusicianGptChord,
                     musician_gpt_chord,
                     musician_gpt_chord_ref,
                     musician_gpt_chord_unref)

MusicianGptChord *
musician_gpt_chord_new (void)
{
  MusicianGptChord *self;

  self = g_slice_new0 (MusicianGptChord);
  self->ref_count = 1;

  return self;
}

static void
musician_gpt_chord_free (MusicianGptChord *self)
{
  g_assert (self);
  g_assert_cmpint (self->ref_count, ==, 0);

  g_slice_free (MusicianGptChord, self);
}

MusicianGptChord *
musician_gpt_chord_ref (MusicianGptChord *self)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}

void
musician_gpt_chord_unref (MusicianGptChord *self)
{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    musician_gpt_chord_free (self);
}
