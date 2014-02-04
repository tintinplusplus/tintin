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

/*********************************************************************/
/* file: utils.c - some utility-functions                            */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

/* note: changed a little bit for readline support -- daw */

#include "tintin.h"

#include <sys/param.h>
#include <errno.h>


/**********************************************/
/* return: TRUE if s1 is an abbrevation of s2 */
/**********************************************/

int is_abbrev(const char *s1, const char *s2)
{
	if (strlen(s2) < strlen(s1))
	{
		return (FALSE);
	}
	return(!strncasecmp(s2, s1, strlen(s1)));
}

int is_suffix(const char *s1, const char *s2)
{
	int sl1, sl2;

	sl1 = strlen(s1);
	sl2 = strlen(s2);

	return (sl1 <= sl2 && !strcasecmp(s1, s2 + sl2 - sl1)) ? FALSE : TRUE;
}
/*
	return TRUE if the string is a number
*/

int is_number(const char *str)
{
	int i;

	for (i = 0 ; str[i] != 0 ; i++)
	{
		if (!isdigit(str[i]))
		{
			return FALSE;
		}
	}
	return TRUE;
}

int hex_number(const char *str)
{
	int value = '?';

	if (str)
	{
		if (isdigit(*str))
		{
			value = (*str - '0') << 4;
		}
		else
		{
			value = (*str - 'A' + 10) << 4;
		}
		str++;
	}

	if (str)
	{
		if (isdigit(*str))
		{
			value += *str - '0';
		}
		else
		{
			value += *str - 'A' + 10;
		}
		str++;
	}

	return value;
}

long long utime()
{
	struct timeval now_time;

	gettimeofday(&now_time, NULL);

	if (gtd->time >= now_time.tv_sec * 1000000LL + now_time.tv_usec)
	{
		gtd->time++;
	}
	else
	{
		gtd->time = now_time.tv_sec * 1000000LL + now_time.tv_usec;
	}
	return gtd->time;
}


char *capitalize(const char *str)
{
	static char outbuf[BUFFER_SIZE];
	int cnt;

	for (cnt = 0 ; str[cnt] != 0 ; cnt++)
	{
		outbuf[cnt] = toupper(str[cnt]);
	}
	outbuf[cnt] = 0;

	return outbuf;
}

void cat_sprintf(char *dest, const char *fmt, ...)
{
	char buf[BUFFER_SIZE];

	va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	strcat(dest, buf);
}

/*************************************************/
/* print system call error message and terminate */
/*************************************************/

void syserr(const char *msg)
{
	extern int errno;

	char s[128], *syserrmsg;

	syserrmsg = strerror(errno);

	if (syserrmsg)
		sprintf(s, "ERROR: %s (%d: %s)", msg, errno, syserrmsg);
	else
		sprintf(s, "ERROR: %s (%d)", msg, errno);
	quitmsg(s);
}

/*
	Whoops, strcasecmp wasn't found.
*/

#if !defined(HAVE_STRCASECMP)
#define UPPER(c) (islower(c) ? toupper(c) : c)

int strcasecmp(const char *string1, const char *string2)
{
	for ( ; UPPER(*string1) == UPPER(*string2) ; string1++, string2++)
		if (!*string1)
			return(0);
	return(UPPER(*string1) - UPPER(*string2));
}

int strncasecmp(const char *string1, const char *string2, size_t count)
{
	if (count)
		do
		{
			if (UPPER(*string1) != UPPER(*string2))
				return(UPPER(*string1) - UPPER(*string2));
					if (!*string1++)
						break;
		string2++;
	}
	while (--count);

	return(0);
}

#endif
