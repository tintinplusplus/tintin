/******************************************************************************
*   TinTin++                                                                  *
*   Copyright (C) 2004 (See CREDITS file)                                     *
*                                                                             *
*   This program is protected under the GNU GPL (See COPYING)                 *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 2 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with this program; if not, write to the Free Software               *
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
*******************************************************************************/

/******************************************************************************
*   file: debug.c - funtions related to the debugging stack                   *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                     coded by Igor van den Hoven 2004                        *
******************************************************************************/


#include "tintin.h"

#include <stdarg.h>

#define MAX_STACK_SIZE     200
#define MAX_DEBUG_SIZE     500

char          debug_stack[MAX_STACK_SIZE][MAX_DEBUG_SIZE];
unsigned char debug_index;

void push_call(char *f, ...)
{
	va_list ap;

	va_start(ap, f);

	if (debug_index >= MAX_STACK_SIZE)
	{
		dump_stack();
		debug_index = 0;
	}

	vsprintf(debug_stack[debug_index], f, ap);

	debug_stack[debug_index][MAX_STACK_SIZE - 1] = 0;

	debug_index++;
}

void pop_call(void)
{
	if (debug_index > 0)
	{
		debug_index--;
	}
	else
	{
		fprintf(stderr, "pop_call: index is zero: %s", debug_stack[0]);
	}
}

void dump_stack(void)
{
	unsigned char i;

	tintin_header(gtd->ses, " DEBUG STACK ");

	for (i = 0 ; i < debug_index ; i++)
	{
		tintin_printf2(gtd->ses, "\033[1;31mDEBUG_STACK[%03d] = %s", i, debug_stack[i]);
	}
	tintin_header(gtd->ses, "");
}
