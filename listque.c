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
   LISTQUE.C

   04/16/90 

   Linked List, I-Tree, Que, and Stack Library.

   Copyright (c) 1990, 1997, 2019, 2026 Scott Beasley.
   Released into the Public Domain

   Scott Beasley.
   scottbeasley@gmail.com
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "listque.h"
#include <stddef.h>

/*
   Creates a link list for use.
*/
PLLHND LLcreate (long entrysz, int bMemCpy, int style, COMPFUNC cmpfunc)
{
   PLLHND llhnd;

   /* Create a link list information block. */
   llhnd = (PLLHND) malloc (sizeof (LLHND));
   //memset (llhnd, 0, sizeof (LLHND));

   if (!llhnd)
      return (PLLHND) NULL;
   else {      
      llhnd->entrysz = entrysz;
      llhnd->llfirst = (PLLENTRY) NULL;
      llhnd->lllast = (PLLENTRY) NULL;
      llhnd->entrycnt = 0;
      llhnd->bMode = 1;
      llhnd->style = style;
      llhnd->cmpfunc = cmpfunc;
      llhnd->current = (PLLENTRY) NULL;
      llhnd->refcnt = 1;
          
      /* 
         If this is clear, then a assignment is done, and not a copy.
         This is useful for pointer storage.
      */
      llhnd->bMemCpy = bMemCpy;
   }

   return (PLLHND) llhnd;
}

/*
   Delete a link list, and free up it's handle.
*/
int LLdestroy (PLLHND llhnd)
{
   PLLENTRY entry;

   entry = llhnd->llfirst;
   while (LLentrycount (llhnd)) {
      if (llhnd->bMemCpy && entry->entry) {
         free (entry->entry);
      }

      if (entry->next) {
         entry = entry->next;
         if (entry->prev) {
            free (entry->prev);
         }
      }

      else {
         if (entry)
            free (entry);
         break;
      }
      
      llhnd->entrycnt--;
   }

   free (llhnd);
   llhnd = (PLLHND) NULL;

   return OK;
}

/*
   Closes up a view to a linked list.
*/
int LLclose (PLLHND llhnd)
{
   memset (llhnd, 0, sizeof (LLHND));
   free (llhnd);
   llhnd = (PLLHND) NULL;

   return OK;
}

/*
   Stores a entry in a given linked list.
   If ws == LLAPPEND then the entry is appened
   to the end of the list. Else it will insert
   it after pos position. If entry == NULL then
   a blank entry is allocated.
*/
int LLwrite (PLLHND llhnd, LLELEMENT entry, int ws, int pos)
{
   PLLENTRY llentry = NULL;
   PLLENTRY insertentry = NULL;

   if (!llhnd->bMode)
      return ERROR;

   llentry = (PLLENTRY) malloc (sizeof (LLENTRY));
   if (!llentry)
      return ERROR;

   if (llhnd->bMemCpy) {
      if (!llhnd->entrysz) {
         llentry->entry = (LLELEMENT) malloc (entry ? strlen ((char *) entry) + 1 : 1);
      }
         
      else {
         llentry->entry = (LLELEMENT) malloc (llhnd->entrysz);
      }

      if (!llentry->entry) {
         free (llentry);
         return ERROR;
      }
   }
   else
      llentry->entry = (LLELEMENT) entry;

   /* Append the item to the list. */
   if (ws == LLAPPEND) {
      if (!llhnd->llfirst) {
         llhnd->llfirst = llhnd->current = llentry;
         llentry->prev = (PLLENTRY) NULL;
      }

      else {
         llentry->prev = llhnd->lllast;
         llhnd->lllast->next = llentry;
         llhnd->current = llentry;
      }

      llhnd->lllast = llentry;
      llentry->next = (PLLENTRY) NULL;
   }

   /* Insert item into the list. */
   else {
      insertentry = llhnd->llfirst;

      if (!insertentry) {
         llhnd->llfirst = llhnd->lllast = llhnd->current = llentry;
         llentry->prev = (PLLENTRY) NULL;
         llentry->next = (PLLENTRY) NULL;
      }

      else if (pos != 0) {
         while (insertentry->next && pos--) {
            insertentry = insertentry->next;
         }

         llentry->prev = insertentry->prev;
         llentry->next = insertentry;
         llhnd->current = llentry;

         if (llentry->prev != (PLLENTRY)NULL)
            llentry->prev->next = llentry;

         insertentry->prev = llentry;
      } else { // Reset first entry if inserting above the first
         llhnd->llfirst = llentry;
         llentry->prev = insertentry->prev;
         llentry->next = insertentry;
         llhnd->current = llentry;

         if (llentry->prev != (PLLENTRY)NULL)
            llentry->prev->next = llentry;

         insertentry->prev = llentry;
      }
   }

   if (!llhnd->bMemCpy) {
      llentry->entry = (LLELEMENT) entry;
   }

   else {
      if (!llhnd->entrysz) {
         if (entry)
            strcpy ((char *) llentry->entry, (char *) entry);
         else
            *((char *) llentry->entry) = '\0';
      }

      else {
         if (entry)
            memcpy (llentry->entry, entry, llhnd->entrysz);
         else
            memset (llentry->entry, 0, llhnd->entrysz);
      }
   }

   llhnd->entrycnt++;

   return OK;
}

/*
   Reads a entry from a link list from pos if 
   rs == LLSEEK or at the current read postion 
   if rs == LLNEXT.
*/
int LLread (PLLHND llhnd, LLELEMENT entry, int rs, int pos)
{
   if (rs == LLSEEK) {
      llhnd->current = llhnd->llfirst;

      if (!llhnd->current)
         return EOL;

      while (llhnd->current->next && pos--) {
         llhnd->current = llhnd->current->next;
      }
   }
   
   if (llhnd->current) {
      if (!llhnd->bMemCpy) {
         memcpy (entry, &llhnd->current->entry, sizeof (LLELEMENT));
      }

      else if (!llhnd->entrysz) {
         strcpy ((char *) entry, (char *) llhnd->current->entry);
      }

      else {
         memcpy (entry, llhnd->current->entry, llhnd->entrysz);
      }

      llhnd->current = (llhnd->current->next ? llhnd->current->next :
                         (PLLENTRY) NULL);
   } else {
      llhnd->current = (PLLENTRY) NULL;
      return EOL;
   }
   
   return OK;
}

/*
   Deletes a entry from a link list.
*/
int LLdelete (PLLHND llhnd, int pos)
{
   PLLENTRY entry;

   entry = llhnd->llfirst;
   if (!entry)
      return ERROR;

   if (pos) {
      while (entry->next && pos--) {
         entry = entry->next;
      }
   }

   if (entry->prev) {
      entry->prev->next = entry->next;
   }

   else {
      llhnd->llfirst = entry->next;
   }

   if (entry->next) {
      entry->next->prev = entry->prev;
   }

   else {
      llhnd->lllast = entry->prev;
   }

   if (llhnd->current == entry)
      llhnd->current = entry->next ? entry->next : entry->prev;

   if (llhnd->bMemCpy && entry->entry)
      free (entry->entry);

   entry->entry = (LLELEMENT) NULL;
   free (entry);
   llhnd->entrycnt--;

   return OK;
}

/*
   Clears entry's from a link list.
*/
int LLclear (PLLHND llhnd)
{
   PLLENTRY entry;
   PLLENTRY next;

   entry = llhnd->llfirst;
   while (entry) {
      next = entry->next;

      if (llhnd->bMemCpy && entry->entry)
         free (entry->entry);

      free (entry);
      entry = next;
   }

   llhnd->entrycnt = 0;
   llhnd->current = llhnd->llfirst = llhnd->lllast = (PLLENTRY) NULL;

   return OK;
}

/*
   Set read write cursor to a certain element.
*/
int LLsetcursor (PLLHND llhnd, int pos)
{
   llhnd->current = llhnd->llfirst;
   if (!llhnd->current)
      return EOL;

   if (pos) {
      while (llhnd->current->next && pos--) {
         llhnd->current = llhnd->current->next;
      }
   }

   if (!llhnd->current)
      return EOL;

   return OK;
}

/*
   Replace data in a element.
*/
int LLreplace (PLLHND llhnd, int pos, LLELEMENT entry)
{
   llhnd->current = llhnd->llfirst;
   if (!llhnd->current)
      return ERROR;

   if (pos) {
      while (llhnd->current->next && pos--) {
         llhnd->current = llhnd->current->next;
      }
   }

   if (!llhnd->bMemCpy) {  // Replace the old entry ptr with the new one.
      llhnd->current->entry = (LLELEMENT) entry;
   } else {      // Replace the old entry (char array / mem block) with the new one.
      LLELEMENT newentry;
      if (!llhnd->entrysz) {
         newentry = (LLELEMENT) malloc (strlen ((char *) entry) + 1);
         if (!newentry)
            return ERROR;
         strcpy ((char *) newentry, (char *) entry);
      } else {
         newentry = (LLELEMENT) malloc (llhnd->entrysz);
         if (!newentry)
            return ERROR;
         if (entry)
            memcpy (newentry, entry, llhnd->entrysz);
         else
            memset (newentry, 0, llhnd->entrysz);
      }
      /* Allocation succeeded — safe to free the old entry now. */
      if (llhnd->current->entry)
         free (llhnd->current->entry);
      llhnd->current->entry = newentry;
   }

   return OK;
}

/*
   Set read write cursor to the next element.
*/
int LLnext (PLLHND llhnd)
{
   if (!llhnd->current)
      return EOL;

   if (llhnd->current->next) {
      llhnd->current = llhnd->current->next;
      return OK;
   }
   else {
      return EOL;
   }
}

/*
   Set read write cursor to the prev element.
*/
int LLprev (PLLHND llhnd)
{
   if (!llhnd->current)
      return BOL;

   if (llhnd->current->prev) {
      llhnd->current = llhnd->current->prev;
      return OK;
   }
   else
      return BOL;
}

/*
   Search for a element in the list.
*/
int LLsearch (PLLHND llhnd, COMPFUNC cmpfunc, LLELEMENT entry)
{
   llhnd->current = llhnd->llfirst;
   if (!llhnd->current)
      return NOTFND;

   while (1) {
      if (cmpfunc) {
         if (!((*cmpfunc) (entry, llhnd->current->entry)))
            return OK;
      }
  
      else {
         if (!llhnd->entrysz) {
            if (!strcmp ((char *) entry, (char *) llhnd->current->entry))
               return OK;
         }

         else {
            if (!memcmp (entry, llhnd->current->entry, llhnd->entrysz))
               return OK;
         }
      }

      if (llhnd->current == llhnd->lllast)
         break;

      llhnd->current = llhnd->current->next;
   }

   return NOTFND;
}

/*
   Search for a element in a sorted list.
*/
int LLbinsearch (PLLHND llhnd, COMPFUNC cmpfunc, LLELEMENT entry)
{
   int icmpres = 0, ilow, ihigh, imid;
   PLLENTRY llentry;

   ilow = 0;
   ihigh = llhnd->entrycnt;

   while (ilow < ihigh) {
      imid = ilow + (ihigh - ilow) / 2;
      LLsetcursor (llhnd, imid);
      llentry = LLreadcursor (llhnd);
      if (llhnd->cmpfunc || cmpfunc) {
         if (cmpfunc)
            icmpres = (*cmpfunc) (entry, llentry);
         else
            icmpres = (*llhnd->cmpfunc) (entry, llentry);
      }
  
      else {
         if (!llhnd->entrysz) {
            icmpres = strcmp ((char *) entry, (char *) llentry);
         }
         else {
            icmpres = memcmp (entry, llentry, llhnd->entrysz);
         }
      }

      if (icmpres < 0)
         ihigh = imid;
      else if (icmpres > 0)
         ilow = imid + 1;      
      else
         return OK;
   }

   return NOTFND; 
}

/*
   Shell sort a list.
*/
void LLshellsort (PLLHND llhnd, int istyle)
{
   int icmpres = 0, igap, i, icmpndx;
   PLLENTRY llentry1, llentry2;
   LLELEMENT tmpdata;

   for (igap = llhnd->entrycnt / 2; igap > 0; igap /= 2) {
      for (i = igap; i < llhnd->entrycnt; i++) {
          icmpndx = i - igap;
          while (icmpndx >= 0) {
             LLsetcursor (llhnd, icmpndx);
             llentry1 = LLcurrent (llhnd);

             LLsetcursor (llhnd, icmpndx + igap);
             llentry2 = LLcurrent (llhnd);

             if (llhnd->cmpfunc) {
                icmpres = (*llhnd->cmpfunc) (llentry1->entry, llentry2->entry);
             }
         
             else {
                if (!llhnd->entrysz) {
                   icmpres = strcmp ((char *) llentry1->entry,
                                     (char *) llentry2->entry);
                }
                else {
                   icmpres = memcmp (llentry1->entry, llentry2->entry,
                                    llhnd->entrysz);
                }
             }
       
             if (icmpres > 0 && istyle == LLDESSORT) {
                break;
             }

             if (icmpres < 0 && istyle == LLASNSORT) {
                break;
             }

             tmpdata = llentry1->entry;
             llentry1->entry = llentry2->entry;
             llentry2->entry = tmpdata;

             icmpndx -= igap;
          }
      }
   }
}

/*
   Create a queue for use.
*/

PQHND Qcreate (int entrysz, int qsize, int mode)
{
   PQHND qhnd;
   int slotsz;

   /* Create a queue information block. */
   qhnd = (PQHND) malloc (sizeof (QHND));
   if (!qhnd)
      return (PQHND) NULL;

   slotsz = mode ? entrysz : (int) sizeof (QELEMENT);
   qhnd->storage = (QELEMENT) malloc (slotsz * qsize);
   if (!qhnd->storage) {
      free (qhnd);
      return (PQHND) NULL;
   }

   qhnd->qwritep = qhnd->storage;
   qhnd->qreadp = qhnd->storage;
   qhnd->entrysz = entrysz;
   qhnd->bottom = (char *) qhnd->storage + (slotsz * qsize);
   qhnd->qsize = qsize;
   qhnd->entrycnt = 0;
   qhnd->mode = mode;

   return (PQHND) qhnd;
}

/*
   Destroy a queue.
*/
int Qdestroy (PQHND qhnd)
{
   free (qhnd->storage);
   free (qhnd);

   return OK;
}

/*
   Place item in queue.
*/
int Qwrite (PQHND qhnd, QELEMENT entry)
{
   int slotsz = Qslotsize (qhnd);

   if (qhnd->entrycnt >= qhnd->qsize) {
      return QFULL;
   }

   if (!qhnd->mode)
      memcpy (qhnd->qwritep, &entry, slotsz);
   else   
      memcpy (qhnd->qwritep, entry, qhnd->entrysz);


   qhnd->qwritep = (char *) qhnd->qwritep + slotsz;
   if (qhnd->qwritep >= qhnd->bottom) {
      qhnd->qwritep = qhnd->storage;
   }

   qhnd->entrycnt++;

   return OK;
}

/*
   Get queue entry.
*/
int Qread (PQHND qhnd, QELEMENT entry)
{
   int slotsz = Qslotsize (qhnd);

   if (!qhnd->entrycnt) {
      return QEMPTY;
   }

   if (!qhnd->mode)
      memcpy (entry, qhnd->qreadp, slotsz);
   else   
      memcpy (entry, qhnd->qreadp, qhnd->entrysz);

   qhnd->qreadp = (char *) qhnd->qreadp + slotsz;
   if (qhnd->qreadp >= qhnd->bottom) {
      qhnd->qreadp = qhnd->storage;
   }

   qhnd->entrycnt--;

   return OK;
}

/*
   Create a stack for use.
*/
static int Sslotsize (PSHND shnd)
{
   return shnd->mode ? shnd->entrysz : (int) sizeof (SELEMENT);
}

PSHND Screate (int entrysz, int Stsize, int mode, int type, STACKELMFREE freefunc)
{
   PSHND shnd;
   int slotsz;

   /* Create a stack information block. */
   shnd = (PSHND) malloc (sizeof (SHND));
   if (!shnd)
      return (PSHND) NULL;

   if (type == LL_STACK) {
      shnd->llhnd = LLcreate (entrysz, mode, LLNORMAL, NULL);
      if (!shnd->llhnd) {
         free (shnd);
         return (PSHND) NULL;
      }
      shnd->sptr = (SELEMENT) NULL;
      shnd->bottom = (SELEMENT) NULL;   
   } else {
      slotsz = mode ? entrysz : (int) sizeof (SELEMENT);
      shnd->storage = (SELEMENT) malloc (slotsz * Stsize);
      if (!shnd->storage) {
         free (shnd);
         return (PSHND) NULL;
      }
      shnd->sptr = shnd->storage;
      shnd->bottom = shnd->storage + (slotsz * Stsize);   
      shnd->llhnd = (PLLHND) NULL;   
   }

   shnd->entrysz = entrysz;
   shnd->ssize = Stsize;
   shnd->entrycnt = 0;
   shnd->mode = mode;
   shnd->freefunc = freefunc;

   return (PSHND) shnd;
}

/*
   Destroy a stack.
*/
int Sdestroy (PSHND shnd)
{
   if (!shnd->llhnd) {
      free (shnd->storage);
   } else {
      LLdestroy (shnd->llhnd);
   }

   free (shnd);
   return OK;
}

/*
   Push item onto stack.
*/
int Spush (PSHND shnd, SELEMENT entry)
{
   int slotsz = Sslotsize (shnd);
   SELEMENT dest;

   if (shnd->entrycnt >= shnd->ssize) {
      return SOVERFLOW;
   }

   if (!shnd->llhnd) {
      dest = shnd->storage + (slotsz * shnd->entrycnt);
      if (!shnd->mode)
         memcpy (dest, &entry, slotsz);
      else   
         memcpy (dest, entry, shnd->entrysz);
      shnd->sptr = dest;
   } else {
      if (LLwrite (shnd->llhnd, (LLELEMENT) entry, LLAPPEND, 0) != OK)
         return ERROR;
   }

   shnd->entrycnt++;

   return OK;
}

/*
   Pop next stack entry (or read with no POP).
*/
int Spop (PSHND shnd, SELEMENT entry, int peek)
{
   int slotsz = Sslotsize (shnd);
   SELEMENT src;

   if (shnd->entrycnt <= 0) {
      if (shnd->llhnd) {
         LLhomecursor (shnd->llhnd);
      } else {
         shnd->sptr = shnd->storage;
      }
      shnd->entrycnt = 0;
      return SUNDERFLOW;
   }

   if (!shnd->llhnd) {
      src = shnd->storage + (slotsz * (shnd->entrycnt - 1));
      if (!shnd->mode)
         memcpy (entry, src, slotsz);
      else   
         memcpy (entry, src, shnd->entrysz);

      if (!peek) {
         shnd->entrycnt--;
         shnd->sptr = shnd->entrycnt ?
                      shnd->storage + (slotsz * (shnd->entrycnt - 1)) :
                      shnd->storage;
      }
   } else {
      LLread (shnd->llhnd, (LLELEMENT)entry, LLSEEK, shnd->entrycnt-1);
      if (!peek)
         LLdelete (shnd->llhnd, shnd->entrycnt - 1);
   }

   if (!peek && shnd->llhnd) {
      shnd->entrycnt--;
   }

   return OK;
}

/*
   Read a stack entry.
*/
int Sread (PSHND shnd, SELEMENT entry, int stk_ndx)
{
   SELEMENT sptr = shnd->storage;
   int slotsz = Sslotsize (shnd);

   if (stk_ndx < 0 || stk_ndx >= shnd->entrycnt) {
      return SUNDERFLOW;
   }

   if (!shnd->llhnd) {
      sptr = shnd->storage + (slotsz * stk_ndx);

      if (!shnd->mode)
         memcpy (entry, sptr, slotsz);
      else   
         memcpy (entry, sptr, shnd->entrysz);
   } else {
      LLread (shnd->llhnd, (LLELEMENT)entry, LLSEEK, stk_ndx);
   }

   return OK;
}

/*
   Creates a I-Tree for use.
*/
static int Tcompare (PTHND thnd, TELEMENT entry1, TELEMENT entry2)
{
   if (thnd->cmpfunc) {
      return (*thnd->cmpfunc) (entry1, entry2);
   }

   if (!thnd->entrysz) {
      return strcmp ((char *) entry1, (char *) entry2);
   }

   return memcmp (entry1, entry2, thnd->entrysz);
}

static void Tfreeentry (PTHND thnd, PTENTRY tentry)
{
   if (thnd->bMemCpy && tentry->entry) {
      free (tentry->entry);
   }
}

static PTENTRY Tleftmost (PTENTRY entry)
{
   while (entry && entry->left) {
      entry = entry->left;
   }

   return entry;
}

static PTENTRY Trightmost (PTENTRY entry)
{
   while (entry && entry->right) {
      entry = entry->right;
   }

   return entry;
}

PTHND Tcreate (long entrysz, int bMemCpy, int style, COMPFUNC cmpfunc)
{
   PTHND thnd;

   /* Create a tree information block. */
   thnd = (PTHND) malloc (sizeof (THND));

   if (!thnd)
      return (PTHND) NULL;
   else {      
      thnd->entrysz = entrysz;
      thnd->troot = (PTENTRY) NULL;
      thnd->current = (PTENTRY) NULL;
      thnd->lastelm = (PTENTRY) NULL;
      thnd->entrycnt = 0;
      thnd->bMode = 1;
      thnd->style = style;
      thnd->cmpfunc = cmpfunc;
 
      /* 
         If this is clear, then a assignment is done, and not a copy.
         This is useful for pointer storage.
      */
      thnd->bMemCpy = bMemCpy;
   }

   return (PTHND) thnd;
}

/*
   Writes to a I-Tree.
*/
int Twrite (PTHND thnd, TELEMENT entry)
{
   PTENTRY current = thnd->troot, tentry, tlast = thnd->troot;
   int iSide = 0, icmpres;

   if (!thnd->bMode)
      return ERROR;

   while (1) {
      if (!current) {
         tentry = (PTENTRY) malloc (sizeof (TENTRY));

         if (!tentry)
            return ERROR;

         if (!thnd->bMemCpy) {
            tentry->entry = (TELEMENT) entry;
         }

         else {
            if (!thnd->entrysz) {
               tentry->entry = (TELEMENT) malloc (strlen ((char *) entry) + 1);
               if (!tentry->entry) {
                  free (tentry);
                  return ERROR;
               }
               strcpy ((char *) tentry->entry, (char *) entry);
            }

            else {
               tentry->entry = (TELEMENT) malloc (thnd->entrysz);
               if (!tentry->entry) {
                  free (tentry);
                  return ERROR;
               }

               if (entry)
                  memcpy (tentry->entry, entry, thnd->entrysz);
               else
                  memset (tentry->entry, 0, thnd->entrysz);
            }
         }

         tentry->left = (PTENTRY) NULL;
         tentry->right = (PTENTRY) NULL;
         tentry->next = (PTENTRY) NULL;
         tentry->parent = tlast;
         
         if (thnd->troot) 
            thnd->lastelm->next = tentry;

         thnd->lastelm = tentry;
         thnd->current = tentry;
         thnd->entrycnt++;

         if (!thnd->troot) {
            thnd->troot = tentry;
            break; 
         }

         if (iSide) {
            tlast->right = tentry;
         }

         else {
            tlast->left = tentry;
         }

         break;
      }

      icmpres = Tcompare (thnd, entry, current->entry);

      tlast = current;
      if (icmpres < 0) {
         current = current->left;
         iSide = 0;
      }

      else {
         current = current->right;
         iSide = 1;
      }
   }

   return OK;
}

/*
   Finds a node in a I-Tree.
*/
TELEMENT Tsearch (PTHND thnd, TELEMENT entry, TELEMENT retentry)
{
   PTENTRY current = thnd->troot;
   int icmpres;

   if (!current)
      return (TELEMENT) NULL;

   while (current) {
      icmpres = Tcompare (thnd, entry, current->entry);

      if (!icmpres) {
         thnd->current = current;
         if (retentry) {
            if (!thnd->entrysz) {
               strcpy ((char *) retentry, (char *) current->entry);
            }

            else {
               memcpy (retentry, current->entry, thnd->entrysz);
            }
         }

         return current->entry;
      }

      if (icmpres < 0) {
         current = current->left;
      }

      else {
         current = current->right;
      }

   }

   return (TELEMENT) NULL;
}

/*
   Sets the I-Tree iterator to the first node in sort order.
*/
int Thomecursor (PTHND thnd)
{
   thnd->current = Tleftmost (thnd->troot);

   return thnd->current ? OK : EOL;
}

/*
   Sets the I-Tree iterator to the last node in sort order.
*/
int Tlastcursor (PTHND thnd)
{
   thnd->current = Trightmost (thnd->troot);

   return thnd->current ? OK : BOL;
}

/*
   Moves the I-Tree iterator to the next node in sort order.
*/
int Tnext (PTHND thnd)
{
   PTENTRY entry;

   if (!thnd->current)
      return EOL;

   if (thnd->current->right) {
      thnd->current = Tleftmost (thnd->current->right);
      return OK;
   }

   entry = thnd->current;
   while (entry->parent && entry == entry->parent->right) {
      entry = entry->parent;
   }

   thnd->current = entry->parent;

   return thnd->current ? OK : EOL;
}

/*
   Moves the I-Tree iterator to the previous node in sort order.
*/
int Tprev (PTHND thnd)
{
   PTENTRY entry;

   if (!thnd->current)
      return BOL;

   if (thnd->current->left) {
      thnd->current = Trightmost (thnd->current->left);
      return OK;
   }

   entry = thnd->current;
   while (entry->parent && entry == entry->parent->left) {
      entry = entry->parent;
   }

   thnd->current = entry->parent;

   return thnd->current ? OK : BOL;
}

/*
   Delete a I-Tree, and free up it's handle.
*/
int Tdestroy (PTHND thnd)
{
   PTENTRY entry, parent;

   entry = thnd->troot;
   while (entry) {
      if (entry->left) {
         entry = entry->left;
      }

      else if (entry->right) {
         entry = entry->right;
      }

      else {
         parent = entry->parent;

         if (parent) {
            if (parent->left == entry)
               parent->left = (PTENTRY) NULL;
            else
               parent->right = (PTENTRY) NULL;
         }

         else {
            thnd->troot = (PTENTRY) NULL;
         }

         Tfreeentry (thnd, entry);
         free (entry);
         entry = parent;
      }
   }

   memset (thnd, 0, sizeof (THND));
   free (thnd);
   thnd = (PTHND) NULL;

   return OK;
}

/*
   Creates a view into a I-Tree.
*/
PTHND Tdup (PTHND thnd)
{
   PTHND thndout;

   /* Create a I-Tree information block. */
   thndout = (PTHND) malloc (sizeof (THND));

   if (!thndout)
      return (PTHND) NULL;
   else {
      memcpy (thndout, thnd, sizeof (THND));
      thnd->bMode = 0;
   }

   return (PTHND) thndout;
}

/*
   Deletes a node in a I-Tree.
*/
int Tdelete (PTHND thnd, TELEMENT entry)
{
   PTENTRY current = thnd->troot;
   PTENTRY replace, child, nextprev, replaceparent;
   int icmpres;

   if (!current)
      return ERROR;

   while (current) {
      icmpres = Tcompare (thnd, entry, current->entry);

      if (!icmpres) {
         break;
      }

      if (icmpres < 0) {
         current = current->left;
      }

      else {
         current = current->right;
      }
   }

   if (!current)
      return ERROR;

   nextprev = (PTENTRY) NULL;
   replace = thnd->troot;
   while (replace && replace != current) {
      nextprev = replace;
      replace = replace->next;
   }

   if (current->left && current->right) {
      replace = current->right;
      while (replace->left) {
         replace = replace->left;
      }

      replaceparent = replace->parent;
      replace->left = current->left;
      replace->parent = current->parent;

      if (replace != current->right) {
         child = replace->right;
         if (replaceparent->left == replace)
            replaceparent->left = child;
         else
            replaceparent->right = child;

         if (child)
            child->parent = replaceparent;

         replace->right = current->right;
      }

      if (replace->left)
         replace->left->parent = replace;

      if (replace->right)
         replace->right->parent = replace;
   }

   else {
      replace = current->left ? current->left : current->right;

      if (replace)
         replace->parent = current->parent;
   }

   if (!current->parent) {
      thnd->troot = replace;
   }

   else if (current->parent->left == current) {
      current->parent->left = replace;
   }

   else {
      current->parent->right = replace;
   }

   if (nextprev)
      nextprev->next = current->next;

   if (thnd->lastelm == current)
      thnd->lastelm = nextprev;

   if (thnd->current == current)
      thnd->current = replace ? replace : thnd->troot;

   Tfreeentry (thnd, current);
   free (current);
   thnd->entrycnt--;

   return OK;
}

/*
   Closes up a view to a I-Tree.
*/
int Tclose (PTHND thnd)
{
   memset (thnd, 0, sizeof (THND));
   free (thnd);
   thnd = (PTHND) NULL;

   return OK;
}
