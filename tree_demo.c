/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*
   TREE_DEMO.C

   Simple demo for the listque I-Tree functions.

   Copyright (c) 1990, 1997, 2019 Scott Beasley.
   Released into the Public Domain
*/

#include <stdio.h>
#include "listque.h"

int main (void);
int cmpint (const PELEMENT left, const PELEMENT right);

int cmpint (const PELEMENT left, const PELEMENT right)
{
   int a = *(const int *) left;
   int b = *(const int *) right;

   return (a > b) - (a < b);
}

static void find_value (PTHND thnd, int key)
{
   int found = 0;

   if (Tsearch (thnd, (TELEMENT) &key, (TELEMENT) &found)) {
      printf ("found %d\n", found);
   }
   else {
      printf ("%d was not found\n", key);
   }
}

static void delete_value (PTHND thnd, int key)
{
   if (Tdelete (thnd, (TELEMENT) &key) == OK) {
      printf ("deleted %d\n", key);
   }
   else {
      printf ("could not delete %d\n", key);
   }
}

static void print_tree_forward (PTHND thnd)
{
   puts ("Tree values in ascending order:");
   for (Thomecursor (thnd); !EOT (thnd); Tnext (thnd)) {
      printf ("  %d\n", *(int *) Treadcursor (thnd));
   }
}

static void print_tree_reverse (PTHND thnd)
{
   puts ("Tree values in descending order:");
   for (Tlastcursor (thnd); !EOT (thnd); Tprev (thnd)) {
      printf ("  %d\n", *(int *) Treadcursor (thnd));
   }
}

int main (void)
{
   int values[] = { 50, 25, 75, 10, 30, 60, 90, 55, 80, 95 };
   int i;
   PTHND thnd;

   thnd = Tcreate (sizeof (int), DATACPY, LLNORMAL, cmpint);
   if (!thnd) {
      puts ("could not create tree");
      return 1;
   }

   puts ("Adding values:");
   for (i = 0; i < (int) (sizeof (values) / sizeof (values[0])); i++) {
      if (Twrite (thnd, (TELEMENT) &values[i]) == OK) {
         printf ("  %d\n", values[i]);
      }
   }

   printf ("Tree entry count: %d\n\n", thnd->entrycnt);
   print_tree_forward (thnd);
   putchar ('\n');
   print_tree_reverse (thnd);

   puts ("\nSearching:");
   find_value (thnd, 60);
   find_value (thnd, 95);
   find_value (thnd, 42);

   puts ("\nDeleting:");
   delete_value (thnd, 10);  /* leaf */
   delete_value (thnd, 60);  /* one child */
   delete_value (thnd, 75);  /* two children */
   delete_value (thnd, 50);  /* root */
   delete_value (thnd, 42);  /* missing */

   printf ("\nTree entry count after deletes: %d\n", thnd->entrycnt);
   print_tree_forward (thnd);

   puts ("\nSearching after deletes:");
   find_value (thnd, 75);
   find_value (thnd, 80);

   Tdestroy (thnd);
   return 0;
}
