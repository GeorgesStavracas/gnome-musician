/* musician-gp4-parser.h
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

#ifndef MUSICIAN_GP4_PARSER_H
#define MUSICIAN_GP4_PARSER_H

#include "musician-gpt-parser.h"

G_BEGIN_DECLS

#define MUSICIAN_TYPE_GP4_PARSER (musician_gp4_parser_get_type())

G_DECLARE_FINAL_TYPE (MusicianGp4Parser, musician_gp4_parser, MUSICIAN, GP4_PARSER, MusicianGptParser)

MusicianGp4Parser *musician_gp4_parser_new (void);

G_END_DECLS

#endif /* MUSICIAN_GP4_PARSER_H */
