# listque

C Linked list, Que, stack and tree library. 

This is a library I wrote mostly long ago. It contains functions to create and manage Link lists, Stacks and Queues. 
There is a set of non-recursive tree functions as well, but will be the first to say they were never extensively tested.

Here is a simple example of the linked list functions in use.  

<pre>
#include &lt;stdlib.h&gt;
#include &lt;stdio.h&gt;
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
</pre>
