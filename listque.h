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
   LISTQUE.H

   04/16/90 

   Linked List, I-Tree, Que, and Stack Library.

   Copyright (c) 1990, 1997, 2019 Scott Beasley.
   Released into the Public Domain

   Scott Beasley.
   scottbeasley@gmail.com
*/

/*
   Note: Link list entrys start with 0 and go to n...
         So your first element will be 0 and the next 1
         and so on.

         Que's are FIFO.
         Stack's are LIFO.          
*/

#define OK         1
#define ERROR      0
#define EOL        2
#define NOTFND     2
#define BOL        3
#define LLAPPEND   0
#define LLSEEK     1
#define LLNEXT     0
#define LLPREV    -1
#define LLINSERT   1
#define LLOVERWRT -1
#define STRSIZE    0
#define ELMSIZE   -1
#define QNEXT      0
#define QFULL      2
#define SOVERFLOW  2
#define QEMPTY     3
#define SUNDERFLOW 3
#define PTRCPY     0
#define DATACPY    1
#define LLNORMAL   0
#define LLASNSORT  1
#define LLDESSORT  2
#define MEM_STACK  0
#define LL_STACK   1
#define STK_READ   0
#define STK_PEEK   1

/* Pointer to a element to be stored in a list. */
typedef void *PELEMENT;

/* Pointer to a element to be stored in a list. */
typedef void *LLELEMENT;

/* Linked list storage element. */
typedef struct _LLENTRY {
   LLELEMENT       entry;
   long            entrysz;  /* Size of the entry. */
   struct _LLENTRY *prev;
   struct _LLENTRY *next;
} LLENTRY;

/* Pointer to the actual data being stored. */
typedef LLENTRY *PLLENTRY;

/*
   Pointer to a element compare function, for llsearch,
   llshellsort.
*/
typedef int (*COMPFUNC) (const PELEMENT, const PELEMENT);

/* Pointer to a element to be stored in a que. */
typedef char *QELEMENT;

/* LL information structure. */
typedef struct _LLHND {
   PLLENTRY       llfirst;  /* First entry into the link list. */
   PLLENTRY       lllast;   /* Last entry into the link list. */
   PLLENTRY       current;  /* Pointer to current entry. */
   long           entrysz;  /* Size of the link list entry. */
   int            entrycnt; /* Number of entries currently in list. */
   int            bMemCpy;  /* */
   int            bMode;    /* Is the list writtable, or a view. */
   int            style;    /* Style of the list. */
   COMPFUNC       cmpfunc;  /* */
} LLHND;

/* Pointer to a link list handle. */
typedef LLHND *PLLHND;

/* Future Que storage element. */
typedef struct _QENTRY {
   QELEMENT       entry;
   long           entrysz;  /* Size of the entry. */
   int            iprty;
} QENTRY;

/* Pointer to the actual data being stored. */
typedef QENTRY *PQENTRY;

/* Queue information structure. */
typedef struct _QHND {
   QELEMENT      storage;    /* Base of Queue memory. */
   QELEMENT      bottom;
   QELEMENT      qwritep;    /* Write pointer. */
   QELEMENT      qreadp;     /* Read Poiter. */
   int           qsize;      /* Size of queue in entries. */
   int           entrysz;
   int           entrycnt;
   int           mode;   
} QHND;

/* Pointer to a que handle. */
typedef QHND *PQHND;

/* Pointer to a element to be stored in a stack. */
typedef char *SELEMENT;

/*
   Pointer to a element free function.
*/
typedef int (*STACKELMFREE) (SELEMENT);

/* Stack information structure. */
typedef struct _SHND {
   SELEMENT      storage;    /* Base of Stack memory. */
   SELEMENT      bottom;
   SELEMENT      sptr;       /* Stack Poiter. */
   PLLHND        llhnd;
   int           ssize;      /* Size of stack in entries. */
   int           entrysz;
   int           entrycnt;
   int           mode;
   STACKELMFREE  freefunc;
} SHND;

/* Pointer to a stack handle. */
typedef SHND *PSHND;

/* Pointer to a element to be stored in a list. */
typedef void *TELEMENT;

/* I-Tree storage element. */
typedef struct _TENTRY {
   TELEMENT       entry;
   long           entrysz;  /* Size of the entry. */
   struct _TENTRY *left;
   struct _TENTRY *right;
   struct _TENTRY *parent;
   struct _TENTRY *next;
} TENTRY;

/* Pointer to the actual data being stored. */
typedef TENTRY *PTENTRY;

/* I-Tree information structure. */
typedef struct _THND {
   PTENTRY        troot;    /* Root of the tree. */
   PTENTRY        current;  /* Pointer to current node. */
   PTENTRY        lastelm;  /* Last node added to tree. */
   long           entrysz;  /* Size of the node entry. */
   int            entrycnt; /* Number of entries currently in tree. */
   int            bMemCpy;  /* */
   int            bMode;    /* Is the list writtable, or a view. */
   int            style;    /* Style of the tree. */
   COMPFUNC       cmpfunc;  /* */
} THND;

/* Pointer to a tree handle. */
typedef THND *PTHND;

/*
   Macro that returns the number of entries in a link list.
*/
#define LLentrycount(llhnd) (llhnd)->entrycnt

/*
   Macro to set read write cursor to the first element.
*/
#define LLhomecursor(llhnd) (llhnd)->current = (llhnd)->llfirst

/*
   Return Pointer to current list element's storage data.
*/
#define LLreadcursor(llhnd) (llhnd)->current->entry

/*
   Set current list element a new element ptr.
*/
#define LLdirectsetcursor(llhnd,elm) (llhnd)->current=(elm)

/*
   Get current link element ptr.
*/
#define LLcurrent(llhnd) (llhnd)->current

/* Returns NULL if end of list is reached. */
#define EOLL(llhnd) !(llhnd)->current

/*
   Creates a link list for use.
*/
PLLHND LLcreate (long entrysz, int bMemCpy, int style, COMPFUNC cmpfunc);

/*
   Delete a link list, and free up it's handle.
*/
int LLdestroy (PLLHND llhnd);

/*
   Closes up a view to a linked list.
*/
int LLclose (PLLHND llhnd);

/*
   Stores a entry in a given linked list.
   If ws == LLAPPEND then the entry is appened
   to the end of the list. Else it will insert
   it after pos position. If entry == NULL then
   a blank entry is allocated.
*/
int LLwrite (PLLHND llhnd, LLELEMENT entry, int ws, int pos);

/*
   Reads a entry from a link list from pos if 
   rs == LLSEEK or at the current read postion 
   if rs == LLNEXT.
*/
int LLread (PLLHND llhnd, LLELEMENT entry, int rs, int pos);

/*
   Deletes a entry from a link list.
*/
int LLdelete (PLLHND llhnd, int pos);

/*
   Clears entry's from a link list.
*/
int LLclear (PLLHND llhnd);

/*
   Set read write cursor to a certain element.
*/
int LLsetcursor (PLLHND llhnd, int pos);

/*
   Set read write cursor to next element.
*/
int LLnext (PLLHND llhnd);

/*
   Set read write cursor to prev element.
*/
int LLprev (PLLHND llhnd);

/*
   Replace data in a element.
*/
int LLreplace (PLLHND llhnd, int pos, LLELEMENT entry);

/*
   Search for a element in a list.
*/
int LLsearch (PLLHND llhnd, COMPFUNC cmpfunc, LLELEMENT entry);

/*
   Search for a element in a sorted list.
*/
int LLbinsearch (PLLHND llhnd, COMPFUNC cmpfunc, LLELEMENT entry);

/*
   Shell sort a list.
*/
void LLshellsort (PLLHND llhnd, int istyle);

/*
   Macro that returns the number of entries in a queue.
*/
#define Qentrycount(qhnd) (qhnd)->entrycnt

/*
   Macro that checks for any queue entries.
*/
#define Qempty(qhnd) (qhnd)->entrycnt ? 0 : 1

/*
   Macro to reset que pointers
   to the top of the queue.
*/
#define Qreset(qhnd) (qhnd)->qwritep = (qhnd)->storage; \
                     (qhnd)->qreadp = (qhnd)->storage; \
                     (qhnd)->entrycnt = 0

/*
   Create a queue for use.
*/
PQHND Qcreate (int entrysz, int qsize, int mode);

/*
   Destroy a queue.
*/
int Qdestroy (PQHND qhnd);

/*
   Place item in queue.
*/
int Qwrite (PQHND qhnd, QELEMENT entry);

/*
   Get queue entry.
*/
int Qread (PQHND qhnd, QELEMENT entry);

/*
   Macro to read queue entry without removeing it.
*/
#define Qpeek(qhnd,entry) memcpy((entry),(qhnd)->qreadp,(qhnd)->entrysz)

/*
   Macro to read top stack entry without removeing it.
*/
//#define Speek(shnd,entry) memcpy((entry),(shnd)->sptr,(shnd)->entrysz)

/*
   Macro to get stack entry count.
*/
#define Sdepth(shnd) (shnd)->entrycnt
/*
   Create a stack for use.
*/
//PSHND Screate (int entrysz, int Stsize, int mode);
PSHND Screate (int entrysz, int Stsize, int mode, int type, STACKELMFREE freefunc);

/*
   Destroy a stack.
*/
int Sdestroy (PSHND shnd);

/*
   Push item onto stack.
*/
int Spush (PSHND shnd, SELEMENT entry);

/*
   Pop next stack entry.
*/
int Spop (PSHND shnd, SELEMENT entry, int peek);

/*
   Read a stack entry.
*/
int Sread (PSHND shnd, SELEMENT entry, int stk_ndx);

/*
   Creates a I-Tree for use.
*/
PTHND Tcreate (long entrysz, int bMemCpy, int style, COMPFUNC cmpfunc);

/*
   Creates a view into a I-Tree.
*/
PTHND Tdup (PTHND thnd);

/*
   Delete a I-Tree, and free up it's handle.
*/
int Tdestroy (PTHND thnd);

/*
   Writes to a I-Tree.
*/
int Twrite (PTHND thnd, TELEMENT entry);

/*
   Finds a node in a I-Tree.
*/
TELEMENT Tsearch (PTHND thnd, TELEMENT entry, TELEMENT retentry);

/*
   Deletes a node in a I-Tree.
*/
int Tdelete (PTHND thnd, TELEMENT entry);

/*
   Closes up a view to a I-Tree.
*/
int Tclose (PTHND thnd);

/* Simple hashing  macros. */
// #define	NOBUCK 255
// #define	hashint(x) ((int)(((unsigned int)(x))&NOBUCK))
// #define	hashstr(str) (((unsigned char)(str)[1]<<5)+(unsigned char)(str)[0])

