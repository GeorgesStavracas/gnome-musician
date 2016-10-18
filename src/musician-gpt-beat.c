/* musician-gpt-beat.c
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

#define G_LOG_DOMAIN "musician-gpt-beat"

#include "musician-gpt-beat.h"
#include "musician-gpt-chord.h"

struct _MusicianGptBeat
{
  volatile gint        ref_count;

  MusicianGptChord    *chord;
  gchar               *text;

  guint                duration : 8;
  MusicianGptBeatMode  mode     : 2;
  MusicianGptDynamics  dynamics : 2;
  guint                n_tuplet : 4;
};

G_DEFINE_BOXED_TYPE (MusicianGptBeat,
                     musician_gpt_beat,
                     musician_gpt_beat_ref,
                     musician_gpt_beat_unref)

static void
musician_gpt_beat_destroy (MusicianGptBeat *self)
{
  if (self != NULL)
    {
      g_clear_pointer (&self->chord, musician_gpt_chord_unref);
      g_clear_pointer (&self->text, g_free);
      g_slice_free (MusicianGptBeat, self);
    }
}

MusicianGptBeat *
musician_gpt_beat_new (void)
{
  MusicianGptBeat *self;

  self = g_slice_new0 (MusicianGptBeat);
  self->ref_count = 1;
  self->mode = MUSICIAN_GPT_BEAT_MODE_NORMAL;

  return self;
}

MusicianGptBeat *
musician_gpt_beat_ref (MusicianGptBeat *self)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}

void
musician_gpt_beat_unref (MusicianGptBeat *self)
{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    musician_gpt_beat_destroy (self);
}

void
musician_gpt_beat_set_mode (MusicianGptBeat     *self,
                            MusicianGptBeatMode  mode)
{
  g_return_if_fail (self != NULL);

  self->mode = mode;
}

MusicianGptBeatMode
musician_gpt_beat_get_mode (MusicianGptBeat *self)
{
  g_return_val_if_fail (self != NULL, 0);

  return self->mode;
}

guint
musician_gpt_beat_get_duration (MusicianGptBeat *self)
{
  g_return_val_if_fail (self != NULL, 0);

  return self->duration;
}

void
musician_gpt_beat_set_duration (MusicianGptBeat *self,
                                guint            duration)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (duration <= G_MAXINT8);

  self->duration = duration;
}

guint
musician_gpt_beat_get_n_tuplet (MusicianGptBeat *self)
{
  g_return_val_if_fail (self != NULL, 0);

  return self->n_tuplet;
}

void
musician_gpt_beat_set_n_tuplet (MusicianGptBeat *self,
                                guint            n_tuplet)
{
  g_return_if_fail (self != NULL);

  switch (n_tuplet)
    {
    case 3:
    case 5:
    case 6:
    case 7:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
      self->n_tuplet = n_tuplet;
      break;

    default:
      g_warning ("Invalid n_tuplet value of %d", n_tuplet);
      break;
    }
}

/**
 * musician_gpt_beat_get_chord:
 *
 * Returns: (transfer none): A #MusicianGptChord or %NULL.
 */
MusicianGptChord *
musician_gpt_beat_get_chord (MusicianGptBeat *self)
{
  g_return_val_if_fail (self != NULL, NULL);

  return self->chord;
}

void
musician_gpt_beat_set_chord (MusicianGptBeat  *self,
                             MusicianGptChord *chord)
{
  g_return_if_fail (self != NULL);

  if (chord != self->chord)
    {
      g_clear_pointer (&self->chord, musician_gpt_chord_unref);
      self->chord = chord ? musician_gpt_chord_ref (chord) : NULL;
    }
}

const gchar *
musician_gpt_beat_get_text (MusicianGptBeat *self)
{
  g_return_val_if_fail (self != NULL, NULL);

  return self->text;
}

void
musician_gpt_beat_set_text (MusicianGptBeat *self,
                            const gchar     *text)
{
  g_return_if_fail (self != NULL);

  if (g_strcmp0 (self->text, text) == 0)
    {
      g_free (self->text);
      self->text = g_strdup (text);
    }
}

MusicianGptDynamics
musician_gpt_beat_get_dynamics (MusicianGptBeat *self)
{
  g_return_val_if_fail (self != NULL, 0);

  return self->dynamics;
}

void
musician_gpt_beat_set_dynamics (MusicianGptBeat     *self,
                                MusicianGptDynamics  dynamics)
{
  g_return_if_fail (self != NULL);

  self->dynamics = dynamics;
}
