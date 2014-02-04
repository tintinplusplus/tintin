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

char *restring(char *point, char *string)
{
	free(point);

	return strdup(string);
}

char *refstring(char *point, char *fmt, ...)
{
	char string[STRING_SIZE];
	va_list args;

	va_start(args, fmt);
	vsprintf(string, fmt, args);
	va_end(args);

	if (point)
	{
		free(point);
	}

	return strdup(string);
}

char *str_alloc(int len)
{
	struct str_data *str_ptr = (struct str_data *) malloc(gtd->str_size + len + 1);

	str_ptr->max = len + 1;

	return (char *) str_ptr + gtd->str_size;
}

// set size of out pointer to in pointer

char *str_mim(char *original)
{
	char *string = str_alloc(strlen(original));

	str_cpy(&string, "");

	return string;

//	struct str_data *str_ptr = (struct str_data *) (original - gtd->str_size);

//	return str_alloc(str_ptr->len);
}

void str_fix(char *original)
{
	struct str_data *str_ptr = (struct str_data *) (original - gtd->str_size);

	str_ptr->len = strlen(original);
}

// allocates memory for the given string

char *str_dup(char *original)
{
	char *string = str_alloc(strlen(original));

	str_cpy(&string, original);

	return string;
}

char *str_dup_printf(char *fmt, ...)
{
	char *str, *ptr;
	va_list args;

	push_call("str_dup_printf(%p)", fmt);

	va_start(args, fmt);
	vasprintf(&str, fmt, args);
	va_end(args);

	ptr = str_dup(str);

	free(str);

	pop_call();
	return ptr;
}

char *str_cpy(char **ptr, char *str)
{
	int str_len;
	struct str_data *str_ptr;

	str_len = strlen(str);

	str_ptr = (struct str_data *) (*ptr - gtd->str_size);

	if (str_ptr->max < str_len + 1)
	{
		str_ptr = (struct str_data *) realloc(str_ptr, gtd->str_size + str_len + 1);
		str_ptr->max = str_len + 1;

		*ptr = (char *) str_ptr + gtd->str_size;
	}

	str_ptr->len = str_len;

	strcpy(*ptr, str);

	return *ptr;
}

char *str_cpy_printf(char **ptr, char *fmt, ...)
{
	char *str;
	va_list args;

	va_start(args, fmt);
	vasprintf(&str, fmt, args);
	va_end(args);

	str_cpy(ptr, str);

	free(str);

	return *ptr;
}

char *str_ndup(char *original, int len)
{
	char *string = str_alloc(len + 1);

	str_ncpy(&string, original, len);

	return string;
}

// Like strncpy but handles the string terminator properly

char *str_ncpy(char **ptr, char *str, int len)
{
	int str_len;
	struct str_data *str_ptr;

	str_len = strlen(str);

	str_ptr = (struct str_data *) (*ptr - gtd->str_size);

	if (str_ptr->max < len)
	{
		str_ptr = (struct str_data *) realloc(str_ptr, gtd->str_size + len + 1);
		str_ptr->max = len;

		*ptr = (char *) str_ptr + gtd->str_size;
	}

	str_ptr->len = UMIN(str_len, len);

	strncpy(*ptr, str, len);

	(*ptr)[len] = 0;

	return *ptr;
}

char *str_cat(char **ptr, char *str)
{
	int str_len;
	struct str_data *str_ptr;

	str_len = strlen(str);

	str_ptr = (struct str_data *) (*ptr - gtd->str_size);

	if (str_ptr->max < str_ptr->len + str_len + 1)
	{
		str_ptr = (struct str_data *) realloc(str_ptr, gtd->str_size + str_ptr->max * 2 + str_len);

		str_ptr->max = str_ptr->max * 2 + str_len;

		*ptr = (char *) str_ptr + gtd->str_size;
	}

	strcpy(&(*ptr)[str_ptr->len], str);

	str_ptr->len += str_len;

	return *ptr;
}

char *str_cat_chr(char **ptr, char chr)
{
	struct str_data *str_ptr;

	str_ptr = (struct str_data *) (*ptr - gtd->str_size);

	if (str_ptr->max < str_ptr->len + 1 + 1)
	{
		str_ptr = (struct str_data *) realloc(str_ptr, gtd->str_size + str_ptr->max * 2 + 1);

		str_ptr->max = str_ptr->max * 2 + 1;

		*ptr = (char *) str_ptr + gtd->str_size;
	}

	(*ptr)[str_ptr->len++] = chr;

	(*ptr)[str_ptr->len] = 0;

	return *ptr;
}
	

char *str_cat_printf(char **ptr, char *fmt, ...)
{
	char *str;
	va_list args;

	va_start(args, fmt);
	vasprintf(&str, fmt, args);
	va_end(args);

	str_cat(ptr, str);

	free(str);

	return *ptr;
}

void str_free(char *ptr)
{
	free((struct str_data *) (ptr - gtd->str_size));
}
