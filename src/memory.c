/******************************************************************************
*   TinTin++                                                                  *
*   Copyright (C) 2007 (See CREDITS file)                                     *
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
******************************************************************************/

/******************************************************************************
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                      coded by Igor van den Hoven 2007                       *
******************************************************************************/

#include "tintin.h"

char *string_alloc(char *string)
{
	struct memory_data *mem;

	mem = (struct memory_data *) calloc(1, sizeof(struct memory_data));

	mem->data = strdup(string);

	LINK(mem, gtd->mem->next, gtd->mem->prev);

	return mem->data;
}


char *string_realloc(char *point, char *string)
{
	struct memory_data *mem;

	for (mem = gtd->mem->prev ; mem ; mem = mem->prev)
	{
		if (mem->data == point)
		{
			free(mem->data);

			mem->data = strdup(string);

			return mem->data;
		}
	}
	printf("string_realloc: %s\n", string);

	return point;
}


char *stringf_alloc(char *fmt, ...)
{
	char string[STRING_SIZE];

	va_list args;

	va_start(args, fmt);
	vsprintf(string, fmt, args);
	va_end(args);

	return string_alloc(string);
}

char *string_free(char *string)
{
	struct memory_data *mem;

	while (TRUE)
	{
		mem = gtd->mem->prev;

		if (mem->data == string)
		{
			memory_free(mem);

			return NULL;
		}
		memory_free(mem);
	}
	printf("failed to free string: %s\n", string);

	return NULL;
}

void memory_free(struct memory_data *mem)
{
	UNLINK(mem, gtd->mem->next, gtd->mem->prev);

	free(mem->data);
	free(mem);
}
