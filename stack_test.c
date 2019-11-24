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
   stack_test.c

   Test out the stack functions.

   Copyright (c) 2019 Scott Beasley.
   Released into the Public Domain

   Scott Beasley.
   scottbeasley@gmail.com

*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "listque.h"

struct stack_entry {
    char data_type;
    void *parm_val;  
};

int main(int argc, char **argv);
int free_var (SELEMENT elm);

#define STACK_SIZE 256

PSHND stack_main;

int main(int argc, char **argv)
{
    int ndx = 0;
    int stk_elm = 0;
    struct stack_entry new_stk_ent;
    float flt_var = 0;
    int int_var = 0;

    stack_main = Screate (sizeof (struct stack_entry), STACK_SIZE, 1, MEM_STACK, free_var);

    while (ndx < 50) {
        if (ndx % 2) {
            new_stk_ent.data_type = 'f';
            new_stk_ent.parm_val = (void *) malloc (sizeof (float));  
            flt_var = (float) ndx; 
             memcpy (new_stk_ent.parm_val, (void *) &flt_var, sizeof (float));              
        } else { 
            new_stk_ent.data_type = 'i';
            new_stk_ent.parm_val = (void *) malloc (sizeof (int));
            int_var = (int) ndx; 
            memcpy (new_stk_ent.parm_val, (void *) &int_var, sizeof (int));             
        }

        ndx++;
    }

    Spop (stack_main, (SELEMENT) &new_stk_ent, STK_READ);
    Spop (stack_main, (SELEMENT) &new_stk_ent, STK_READ);
    new_stk_ent.data_type = 'i';
    new_stk_ent.parm_val = (void *) malloc (sizeof (int));

    int_var = (int) ndx; 
    memcpy (new_stk_ent.parm_val, (void *) &int_var, sizeof (int));    
    Spush (stack_main, (SELEMENT) &new_stk_ent);

    new_stk_ent.data_type = 'f';
    new_stk_ent.parm_val = (void *) malloc (sizeof (float));
    flt_var = (float) ndx + 1; 
    memcpy (new_stk_ent.parm_val, (void *) &flt_var, sizeof (float));    
    Spush (stack_main, (SELEMENT) &new_stk_ent);    

    ndx = Sdepth(stack_main);
    while (ndx) {
        Spop (stack_main, (SELEMENT) &new_stk_ent, STK_READ);
        ndx--;
    }

    Sdestroy (stack_main);
    return (0);
}

int free_var (SELEMENT elm)
{
    struct stack_entry *op;
    float flt_var = 0;
    int int_var = 0;

    op = (struct stack_entry *) elm; 

    if (op->data_type == 'i') {
        int_var = *(int *)op->parm_val;
        printf ("<%d>", int_var);
    } else {
        flt_var = *(float *)op->parm_val;
        printf ("<%f>", flt_var);        
    }
 
    if (op->parm_val) {
        free (op->parm_val);
        op->parm_val = NULL;
    }

    return (0);
}
