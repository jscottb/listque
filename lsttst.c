#include <stdlib.h>
#include <stdio.h>
#include "listque.h"

int main (void);

int main (void)
{
   PLLHND llhnd;
   int icnt = 0;

   llhnd = LLcreate (0, 1, 1, (COMPFUNC) NULL);

   LLwrite (llhnd, (LLELEMENT) "Nine", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "Two", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "Five", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "Seven", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "Three", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "Eight", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "Four", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "One", LLINSERT, 3);

   icnt = LLentrycount (llhnd);
   char entry[20];
   LLhomecursor(llhnd);
   while (!EOLL(llhnd)) {
      LLread (llhnd, (LLELEMENT)entry, LLNEXT, 0);
      printf ("%s\n", entry);
   }

   LLdestroy (llhnd);
}
