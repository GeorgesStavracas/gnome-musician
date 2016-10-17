/* musician-gpt-bend.c
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

#define G_LOG_DOMAIN "musician-gpt-bend"

#include "musician-gpt-bend.h"

struct _MusicianGptBend
{
  volatile gint ref_count;
  GArray *points;
  MusicianGptBendType bend_type;
};

G_DEFINE_BOXED_TYPE (MusicianGptBend,
                     musician_gpt_bend,
                     musician_gpt_bend_ref,
                     musician_gpt_bend_unref)

MusicianGptBend *
musician_gpt_bend_new (void)
{
  MusicianGptBend *self;

  self = g_slice_new0 (MusicianGptBend);
  self->ref_count = 1;
  self->points = g_array_new (FALSE, FALSE, sizeof (MusicianGptBendPoint));

  return self;
}

static void
musician_gpt_bend_free (MusicianGptBend *self)
{
  g_assert (self);
  g_assert_cmpint (self->ref_count, ==, 0);

  g_clear_pointer (&self->points, g_array_unref);

  g_slice_free (MusicianGptBend, self);
}

MusicianGptBend *
musician_gpt_bend_ref (MusicianGptBend *self)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}

void
musician_gpt_bend_unref (MusicianGptBend *self)
{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    musician_gpt_bend_free (self);
}

void
musician_gpt_bend_set_bend_type (MusicianGptBend     *self,
                                 MusicianGptBendType  bend_type)
{
  g_return_if_fail (self != NULL);

  self->bend_type = bend_type;
}

void
musician_gpt_bend_add_point (MusicianGptBend            *self,
                             const MusicianGptBendPoint *point)
{
  g_return_if_fail (self != NULL);

  g_array_append_vals (self->points, point, 1);
}
