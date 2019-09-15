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

   Copyright (c) 1990, 1997, 2019 Scott Beasley.
   Released into the Public Domain

   Scott Beasley.
   scottbeasley@gmail.com
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "listque.h"

/*
   Creates a link list for use.
*/
PLLHND LLcreate (long entrysz, int bMemCpy, int style, COMPFUNC cmpfunc)
{
   PLLHND llhnd;

   /* Create a link list information block. */
   llhnd = (PLLHND) malloc (sizeof (LLHND));

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
  
      /* 
         If this is clear, then a assignment is done, and not a copy.
         This is useful for pointer storage.
      */
      llhnd->bMemCpy = bMemCpy;
   }

   return (PLLHND) llhnd;
}

/*
   Creates a view into a linked list.
*/
PLLHND LLdup (PLLHND llhnd)
{
   PLLHND llhndout;

   /* Create a link list information block. */
   llhndout = (PLLHND) malloc (sizeof (LLHND));

   if (!llhndout)
      return (PLLHND) NULL;
   else {
      memcpy (llhndout, llhnd, sizeof (LLHND));
      llhnd->bMode = 0;
   }

   return (PLLHND) llhndout;
}

/*
   Delete a link list, and free up it's handle.
*/
int LLdestroy (PLLHND llhnd)
{
   PLLENTRY entry;

   entry = llhnd->llfirst;
   while (LLentrycount (llhnd)) {
      if (entry->entry) {
         free (entry->entry);
         entry->entry = (LLELEMENT) NULL;
      }

      if (entry->next) {
         entry = entry->next;
         if (entry->prev) {
            free (entry->prev);
            entry->prev = (PLLENTRY) NULL;
         }
      }

      else {
         if (entry)
            free (entry);
         break;
      }
   }

   memset (llhnd, 0, sizeof (LLHND));
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
   PLLENTRY llentry;
   PLLENTRY insertentry;

   if (!llhnd->bMode)
      return ERROR;

   llentry = (PLLENTRY) malloc (sizeof (LLENTRY));

   if (!llhnd->entrysz) {
      llentry->entry = (LLELEMENT) malloc (strlen ((char *) entry) + 1);
   }
      
   else {
      llentry->entry = (LLELEMENT) malloc (llhnd->entrysz);
   }

   if (!llentry->entry || !llentry) 
      return ERROR;

   /* Append the item to the list. */
   if (ws == LLAPPEND) {
      if (!llhnd->llfirst) {
         llhnd->llfirst = llhnd->current = llentry;
      }

      else {
         llentry->prev = llhnd->lllast;
         llhnd->lllast->next = llentry;
         llentry->next = (PLLENTRY) NULL;
      }

      llhnd->lllast = llentry;
   }

   /* Insert item into the list. */
   else {
      insertentry = llhnd->llfirst;

      while (insertentry->next && pos--) {
         insertentry = insertentry->next;
      }

      llentry->prev = insertentry->prev;
      llentry->next = insertentry;
      llhnd->current = llentry->prev->next = llentry;
      insertentry->prev = llentry;
   }

   if (!llhnd->bMemCpy) {
      llentry->entry = (LLELEMENT) entry;
   }

   else {
      if (!llhnd->entrysz) {
         strcpy ((char *) llentry->entry, (char *) entry);
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

      while (llhnd->current->next && pos--) {
         llhnd->current = llhnd->current->next;
      }
   }

   if (llhnd->current) {
      if (!llhnd->entrysz) {
         strcpy ((char *) entry, (char *) llhnd->current->entry);
      }

      else {
         memcpy (entry, llhnd->current->entry, llhnd->entrysz);
      }

      llhnd->current = (llhnd->current->next ? llhnd->current->next :
                         NULL);
   }

   if (!llhnd->current)
      return EOL;
   else
      return OK;
}

/*
   Deletes a entry from a link list.
*/
int LLdelete (PLLHND llhnd, int pos)
{
   PLLENTRY entry;

   entry = llhnd->llfirst;

   if (pos) {
      while (entry->next && pos--) {
         entry = entry->next;
      }
   }

   if (entry != llhnd->llfirst && entry != llhnd->lllast) {
      entry->prev->next = entry->next;
      entry->next->prev = entry->prev;
   }

   if (entry == llhnd->llfirst) {
      entry->next->prev = (PLLENTRY) NULL;
      llhnd->llfirst = entry->next;
   }

   if (entry == llhnd->lllast) {
      entry->prev->next =  (PLLENTRY) NULL;
      llhnd->lllast = entry->prev;
   }

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

   entry = llhnd->llfirst;
   while (llhnd->entrycnt--) {
      if (entry->entry)
         free (entry->entry);

      if (entry->next)
         entry = entry->next;
      else
         break;

      if (entry->prev)
         free (entry->prev);
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

   if (pos) {
      while (llhnd->current->next && pos--) {
         llhnd->current = llhnd->current->next;
      }
   }

   free (llhnd->current->entry);

   if (!llhnd->bMemCpy) {
      llhnd->current->entry = (LLELEMENT) entry;
   }

   else {
      if (!llhnd->entrysz) {
         llhnd->current->entry = (LLELEMENT) malloc (strlen ((char *)
                                                        entry) + 1);
         if (!llhnd->current->entry)
            return ERROR;
         else {
            strcpy ((char *) llhnd->current->entry, (char *) entry);
         }
      }
   
      else {
         llhnd->current->entry = (LLELEMENT) malloc (llhnd->entrysz);
   
         if (!llhnd->current->entry)
            return ERROR;
         else {
            if (entry)
               memcpy (llhnd->current->entry, entry, llhnd->entrysz);
            else
               memset (llhnd->current->entry, 0, llhnd->entrysz);
         }
      }
   }

   return OK;
}

/*
   Set read write cursor to the next element.
*/
int LLnext (PLLHND llhnd)
{
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
            if (memcmp (entry, llhnd->current->entry, llhnd->entrysz))
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
             llentry1 = LLreadcursor (llhnd);

             LLsetcursor (llhnd, icmpndx + igap);
             llentry2 = LLreadcursor (llhnd);

             if (llhnd->cmpfunc) {
                icmpres = (*llhnd->cmpfunc) (llentry1, llentry2);
             }
         
             else {
                if (!llhnd->entrysz) {
                   icmpres = strcmp ((char *) llentry1, (char *) llentry2);
                }
                else {
                   icmpres = memcmp (llentry1, llentry2, llhnd->entrysz);
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

   /* Create a queue information block. */
   qhnd = (PQHND) malloc (sizeof (QHND));

   qhnd->storage = (QELEMENT) malloc (entrysz * qsize);

   qhnd->qwritep = qhnd->storage;
   qhnd->qreadp = qhnd->storage;
   qhnd->entrysz = entrysz;
   qhnd->bottom = qhnd->storage + (entrysz * qsize);
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
   if (qhnd->entrycnt >= qhnd->qsize) {
      return QFULL;
   }

   if (!qhnd->mode)
      qhnd->qwritep = entry;
   else   
      memcpy (qhnd->qwritep, entry, qhnd->entrysz);


   qhnd->qwritep = (qhnd->qwritep > qhnd->bottom ? NULL : 
                     qhnd->qwritep + qhnd->entrysz);

   if (!qhnd->qwritep) {
      qhnd->qwritep = qhnd->storage;
      return QFULL;
   }

   qhnd->entrycnt++;

   return OK;
}

/*
   Get queue entry.
*/
int Qread (PQHND qhnd, QELEMENT entry)
{
   if (!qhnd->entrycnt) {
      return QEMPTY;
   }

   if (!qhnd->mode)
      entry = qhnd->qreadp;
   else   
      memcpy (entry, qhnd->qreadp, qhnd->entrysz);

   qhnd->qreadp = (qhnd->qreadp > qhnd->bottom ? NULL : 
                    qhnd->qreadp + qhnd->entrysz);

   if (!qhnd->qreadp) {
      qhnd->qreadp = qhnd->storage;
      qhnd->entrycnt = 0;
      return QEMPTY;
   }

   qhnd->entrycnt--;

   return OK;
}

/*
   Create a stack for use.
*/
PSHND Screate (int entrysz, int Stsize, int mode, int type, STACKELMFREE freefunc)
{
   PSHND shnd;

   /* Create a stack information block. */
   shnd = (PSHND) malloc (sizeof (SHND));

   if (type == LL_STACK) {
      shnd->llhnd = LLcreate (entrysz, 1, LLNORMAL, NULL);
      shnd->sptr = (SELEMENT) NULL;
      shnd->bottom = (SELEMENT) NULL;   
   } else {
      shnd->storage = (SELEMENT) malloc (entrysz * Stsize);
      shnd->sptr = shnd->storage;
      shnd->bottom = shnd->storage + (entrysz * Stsize);   
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
   if (shnd->entrycnt >= shnd->ssize) {
      return SOVERFLOW;
   }

   if (!shnd->llhnd) {
      shnd->sptr = (shnd->sptr > shnd->bottom ? NULL : 
                     shnd->sptr + shnd->entrysz);

      if (!shnd->sptr) {
         shnd->sptr = shnd->storage;
         return SOVERFLOW;
      }

      if (!shnd->mode)
         shnd->sptr = entry;
      else   
         memcpy (shnd->sptr, entry, shnd->entrysz);
   } else {

       if (shnd->entrycnt > shnd->ssize) {
            return SOVERFLOW;
            //LLhomecursor (shnd->llhnd);
       }

       LLwrite (shnd->llhnd, (LLELEMENT) entry, LLAPPEND, 0);
   }

   shnd->entrycnt++;

   return OK;
}

/*
   Pop next stack entry (or read with no POP).
*/
int Spop (PSHND shnd, SELEMENT entry, int peek)
{
   if (shnd->entrycnt < 0) {
      if (shnd->llhnd) {
         LLhomecursor (shnd->llhnd);
      } else {
         shnd->sptr = shnd->storage;
      }
      shnd->entrycnt = 0;
      return SUNDERFLOW;
   }

   if (!shnd->llhnd) {
      if (!shnd->mode)
         entry = shnd->sptr;
      else   
         memcpy (entry, shnd->sptr, shnd->entrysz);

      if (!peek) {
         shnd->sptr = (shnd->sptr == shnd->storage ? NULL : 
                        shnd->sptr - shnd->entrysz);

         if (!shnd->sptr) {
            shnd->sptr = shnd->storage;
            shnd->entrycnt = 0;
            return SUNDERFLOW;
         }
      }
   } else {
      LLread (shnd->llhnd, (LLELEMENT)entry, LLSEEK, shnd->entrycnt-1);
   }

   if (!peek) {
      shnd->entrycnt--;

      if (shnd->freefunc) {
         (*shnd->freefunc) (entry);
      }
   }

   return OK;
}

/*
   Read a stack entry.
*/
int Sread (PSHND shnd, SELEMENT entry, int stk_ndx)
{
   SELEMENT sptr = shnd->storage;

   if (stk_ndx > shnd->entrycnt) {
      return SUNDERFLOW;
   }

   if (!shnd->llhnd) {
      sptr = shnd->storage + (shnd->entrysz * stk_ndx);

      if (!shnd->mode)
         entry = sptr;
      else   
         memcpy (entry, sptr, shnd->entrysz);
   } else {
      LLread (shnd->llhnd, (LLELEMENT)entry, LLSEEK, stk_ndx-1);
   }

   return OK;
}

/*
   Creates a I-Tree for use.
*/
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

         if (!thnd->entrysz) {
            tentry->entry = (TELEMENT) malloc (strlen ((char *) entry) + 1);
         }
      
         else {
            tentry->entry = (TELEMENT) malloc (thnd->entrysz);
         }

         if (!tentry->entry || !tentry) 
            return ERROR;

         if (!thnd->bMemCpy) {
            tentry->entry = (TELEMENT) entry;
         }

         else {
            if (!thnd->entrysz) {
               strcpy ((char *) tentry->entry, (char *) entry);
            }

            else {
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

      if (thnd->cmpfunc) {
         icmpres = (*thnd->cmpfunc) (entry, current->entry);
      }
  
      else {
         if (!thnd->entrysz) {
            icmpres = strcmp ((char *) entry, (char *) current->entry);
         }
         else {
            icmpres = memcmp (entry, current->entry, thnd->entrysz);
         }
      }

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
   int iFound = 0, icmpres;

   if (!current)
      return (TELEMENT) NULL;

   while (1) {
      if (iFound) {
         if (retentry) {
            if (!thnd->entrysz) {
               strcpy ((char *) retentry, (char *) current->entry);
            }

            else {
               memcpy (retentry, current->entry, thnd->entrysz);
            }
         }

         return iFound ? current->entry : (TELEMENT) NULL;
      }

      if (thnd->cmpfunc) {
         icmpres = (*thnd->cmpfunc) (entry, current->entry);
      }
  
      else {
         if (!thnd->entrysz) {
            icmpres = strcmp ((char *) entry, (char *) current->entry);
         }
         else {
            icmpres = memcmp (entry, current->entry, thnd->entrysz);
         }
      }

      if (!icmpres) {
         iFound = 1;
         continue;
      }

      if (icmpres < 0) {
         current = current->left;
      }

      else {
         current = current->right;
      }

      if (!current) {
         break;
      }
   }

   return iFound ? current->entry : (TELEMENT) NULL;
}

/*
   Delete a I-Tree, and free up it's handle.
*/
int Tdestroy (PTHND thnd)
{
   PTENTRY entry, lastentry;

   entry = thnd->troot;
   while (1) {
      if (entry) {
         lastentry = entry;
         entry = entry->next;
         free (lastentry->entry);
         free (lastentry);
      }

      else
         break;
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
   int iFound = 0, icmpres;

   if (!current)
      return 0;

   while (1) {
      if (iFound) {
         if (current == thnd->troot) {
            if (current->left) {
               thnd->troot = current->left;
               if (current->right) {
                  Twrite (thnd, current->right->entry);
                  thnd->lastelm->right = current->right;
               }
            }

            else if (current->right) {
               thnd->troot = current->right;
            }
         }

         else {
            if (current->left) {
               Twrite (thnd, current->left->entry);
               thnd->lastelm->left = current->left;
            }

            if (current->right) {
               Twrite (thnd, current->right->entry); 
               thnd->lastelm->right = current->right;
            }
         }

         if (current != thnd->troot)
            thnd->lastelm->next = current->next;

         free (current->entry);
         free (current);

         return iFound;
      }

      if (thnd->cmpfunc) {
         icmpres = (*thnd->cmpfunc) (entry, current->entry);
      }
  
      else {
         if (!thnd->entrysz) {
            icmpres = strcmp ((char *) entry, (char *) current->entry);
         }
         else {
            icmpres = memcmp (entry, current->entry, thnd->entrysz);
         }
      }

      if (!icmpres) {
         iFound = 1;
         continue;
      }

      if (icmpres < 0) {
         current = current->left;
      }

      else {
         current = current->right;
      }

      if (!current) {
         break;
      }
   }

   return iFound;
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
