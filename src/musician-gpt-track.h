/* musician-gpt-track.h
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

#ifndef MUSICIAN_GPT_TRACK_H
#define MUSICIAN_GPT_TRACK_H

#include <gdk/gdk.h>

#include "musician-gpt-types.h"

G_BEGIN_DECLS

#define MUSICIAN_TYPE_GPT_TRACK (musician_gpt_track_get_type())

G_DECLARE_DERIVABLE_TYPE (MusicianGptTrack, musician_gpt_track, MUSICIAN, GPT_TRACK, GObject)

struct _MusicianGptTrackClass
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
};

MusicianGptTrack          *musician_gpt_track_new                 (void);
const gchar               *musician_gpt_track_get_title           (MusicianGptTrack          *self);
void                       musician_gpt_track_set_title           (MusicianGptTrack          *self,
                                                                   const gchar               *name);
guint                      musician_gpt_track_get_id              (MusicianGptTrack          *self);
void                       musician_gpt_track_set_id              (MusicianGptTrack          *self,
                                                                   guint                      id);
const GdkRGBA             *musician_gpt_track_get_color           (MusicianGptTrack          *self);
void                       musician_gpt_track_set_color           (MusicianGptTrack          *self,
                                                                   const GdkRGBA             *color);
const MusicianGptTuning   *musician_gpt_track_get_tunings         (MusicianGptTrack          *self,
                                                                   gsize                     *n_tunings);
void                       musician_gpt_track_set_tunings         (MusicianGptTrack          *self,
                                                                   const MusicianGptTuning   *tunings,
                                                                   gsize                      n_tunings);
guint                      musician_gpt_track_get_capo_at         (MusicianGptTrack          *self);
void                       musician_gpt_track_set_capo_at         (MusicianGptTrack          *self,
                                                                   guint                      capo_at);
guint                      musician_gpt_track_get_channel         (MusicianGptTrack          *self);
void                       musician_gpt_track_set_channel         (MusicianGptTrack          *self,
                                                                   guint                      channel);
MusicianGptChannelEffects  musician_gpt_track_get_channel_effects (MusicianGptTrack          *self);
void                       musician_gpt_track_set_channel_effects (MusicianGptTrack          *self,
                                                                   MusicianGptChannelEffects  channel_effects);
guint                      musician_gpt_track_get_n_frets         (MusicianGptTrack          *self);
void                       musician_gpt_track_set_n_frets         (MusicianGptTrack          *self,
                                                                   guint                      n_frets);
guint                      musician_gpt_track_get_port            (MusicianGptTrack          *self);
void                       musician_gpt_track_set_port            (MusicianGptTrack          *self,
                                                                   guint                      port);
guint                      musician_gpt_track_get_n_strings       (MusicianGptTrack          *self);

G_END_DECLS

#endif /* MUSICIAN_GPT_TRACK_H */
