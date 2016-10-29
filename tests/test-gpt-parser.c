/* test-gpt-parser.c
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

#include <musician.h>

static GInputStream *
get_test_file (const gchar   *name,
               GCancellable  *cancellable,
               GError       **error)
{
  g_autofree gchar *path = g_build_filename (TESTS_SRCDIR, "data", name, NULL);
  g_autoptr(GFile) file = g_file_new_for_path (path);
  return G_INPUT_STREAM (g_file_read (file, cancellable, error));
}

static void
test_parser_basic (void)
{
  MusicianGptParser *parser;
  g_autoptr(GError) error = NULL;
  g_autoptr(GInputStream) base_stream = NULL;
  gint r;

  parser = musician_gpt_parser_new ();

  base_stream = get_test_file ("test1.gp4", NULL, &error);
  g_assert_no_error (error);
  g_assert (base_stream != NULL);

  r = musician_gpt_parser_load_from_stream (parser, base_stream, NULL, &error);
  g_assert_no_error (error);
  g_assert_cmpint (r, ==, 1);

  g_object_add_weak_pointer (G_OBJECT (parser), (gpointer *)&parser);
  g_object_unref (parser);
  g_assert (parser == NULL);
}

gint
main (gint argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/Musician/GptParser/basic", test_parser_basic);
  return g_test_run ();
}
