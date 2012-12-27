/*
** RopGadget - Release v3.4.2
** Jonathan Salwan - http://twitter.com/JonathanSalwan
** Allan Wirth - http://allanwirth.com/
** http://shell-storm.org
** 2012-11-11
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "ropgadget.h"

/* gadget necessary for combo */
/* don't touch this att syntax for parsing */
static char *tab_combo_ropsh[] =
{
  "xor %rax,%rax",
  "mov %r?x,(%r?x)", CR_AND,
  "pop %rax", CR_AND,
  "pop %rbx", CR_AND,
  "pop %rcx", CR_AND,
  "pop %rdx", CR_AND,

    "inc %rax",
    "inc %eax",
    CR_OR,
    "inc %ax",
    CR_OR,
    "inc %al",
    CR_OR,
  CR_AND,

  "syscall", CR_AND,
  NULL
};

/* gadget necessary for combo importsc */
static char *tab_combo_importsc[] =
{
  "mov %r?x,(%r?x)",
  "", CR_AND,           /*set in combo_ropmaker_importsc() */
  "", CR_AND,           /*            //            */
  "", CR_AND,           /*            //            */
  NULL
};

void x8664_ropmaker(void)
{
  int flag = 0;
  t_gadget *gadgets;

  char **ropsh = importsc_mode.flag?tab_combo_importsc:tab_combo_ropsh;

  if (importsc_mode.flag)
    {
      char reg1, reg2, reg3;
      char gad1[] = "pop %rXx";
      char gad2[] = "mov (%rXx),%rXx";
      char gad3[] = "mov %rXx,%rXx";
      t_asm *gadget = search_instruction(tab_x8664, ropsh[0]);
      if (gadget && gadget->addr)
        {
          reg1 = getreg(gadget->instruction, 1);
          reg2 = getreg(gadget->instruction, 2);
          ropsh[1] = gad1;
          ropsh[3] = gad2;
          ropsh[5] = gad3;
          ropsh[1][6]  = reg2;
          ropsh[3][7]  = reg2;
          ropsh[3][13] = '?';
          gadget = search_instruction(tab_x8664, ropsh[3]);
          reg3 = getreg(gadget->instruction, 3);
          ropsh[5][6]  = reg3;
          ropsh[5][11] = reg1;

          if (reg3 == reg1) {/* gadget useless */
            ropsh[5] = NULL;
          }
        }
    }

  flag = !combo_ropmaker(ropsh, tab_x8664, &gadgets);

  if (importsc_mode.flag)
    {
      if (importsc_mode.size > (importsc_mode.gotsize + importsc_mode.gotpltsize))
        {
          fprintf(stderr, "\n\t%s/!\\ Possible to make a ROP payload but .got size & .got.plt size isn't sufficient.%s\n", RED, ENDC);
          fprintf(stderr, "  \t%s    got + got.plt = %s" SIZE_FORMAT " bytes%s and your shellcode size is %s" SIZE_FORMAT " bytes%s\n", RED, YELLOW, SIZE_WIDTH, (importsc_mode.gotsize + importsc_mode.gotpltsize), RED, YELLOW, SIZE_WIDTH, (Size)importsc_mode.size, ENDC);
          return ;
        }
      /* build a python code */
      if (!flag)
        x86_makecode_importsc(gadgets, 8);
    }
  else
    {
    /* build a python code */
    if (!flag)
      x86_makecode(gadgets, 8);
    }
  free(gadgets);
}