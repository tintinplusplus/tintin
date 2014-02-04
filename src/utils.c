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
******************************************************************************/

/******************************************************************************
*               (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                  *
*                                                                             *
*                       coded by Peter Unold 1992                             *
******************************************************************************/

#include "tintin.h"

#include <sys/param.h>
#include <errno.h>


/**********************************************/
/* return: TRUE if s1 is an abbrevation of s2 */
/**********************************************/

int is_abbrev(char *s1, char *s2)
{
	if (strlen(s2) < strlen(s1))
	{
		return (FALSE);
	}
	return(!strncasecmp(s2, s1, strlen(s1)));
}

int is_suffix(char *s1, char *s2)
{
	int sl1, sl2;

	sl1 = strlen(s1);
	sl2 = strlen(s2);

	return (sl1 <= sl2 && !strcasecmp(s1, s2 + sl2 - sl1)) ? FALSE : TRUE;
}
/*
	return TRUE if the string is a number
*/

int is_number(char *str)
{
	int i;

	if (!isdigit(str[0]) && str[0] != '-' && str[0] != '+')
	{
		return FALSE;
	}

	for (i = 1 ; str[i] != 0 ; i++)
	{
		if (!isdigit(str[i]))
		{
			return FALSE;
		}
	}
	return TRUE;
}

int hex_number(char *str)
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


char *capitalize(char *str)
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

void cat_sprintf(char *dest, char *fmt, ...)
{
	char buf[BUFFER_SIZE];

	va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	strcat(dest, buf);
}

void ins_sprintf(char *dest, char *fmt, ...)
{
	char buf[BUFFER_SIZE], tmp[BUFFER_SIZE];

	va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	strcpy(tmp, dest);

	*dest = 0;

	strcat(dest, buf);
	strcat(dest, tmp);
}

void show_message(struct session *ses, int index, char *format, ...)
{
	struct listroot *root;
	char buf[BUFFER_SIZE];
	va_list args;

	push_call("show_message(%p,%p,%p)",ses,index,format);

	va_start(args, format);

	vsprintf(buf, format, args);

	va_end(args);
	
	if (index == -1)
	{
		if (HAS_BIT(gtd->flags, TINTIN_FLAG_USERCOMMAND))
		{
			tintin_puts2(buf, ses);
		}
		pop_call();
		return;
	}

	root = ses->list[index];

	if (HAS_BIT(root->flags, LIST_FLAG_DEBUG))
	{
		tintin_puts2(buf, ses);

		pop_call();
		return;
	}

	if (HAS_BIT(root->flags, LIST_FLAG_MESSAGE))
	{
		if (HAS_BIT(gtd->flags, TINTIN_FLAG_USERCOMMAND))
		{
			tintin_puts2(buf, ses);

			pop_call();
			return;
		}
	}

	if (HAS_BIT(root->flags, LIST_FLAG_LOG))
	{
		if (ses->logfile)
		{
			logit(ses, buf, ses->logfile);
		}
	}
	pop_call();
	return;
}

void show_debug(struct session *ses, int index, char *format, ...)
{
	struct listroot *root;
	char buf[BUFFER_SIZE];
	va_list args;

	root = ses->list[index];

	if (!HAS_BIT(root->flags, LIST_FLAG_DEBUG) && !HAS_BIT(root->flags, LIST_FLAG_LOG))
	{
		return;
	}

	push_call("show_debug(%p,%p,%p)",ses,index,format);		

	va_start(args, format);

	vsprintf(buf, format, args);

	va_end(args);

	if (HAS_BIT(root->flags, LIST_FLAG_DEBUG))
	{
		tintin_puts2(buf, ses);

		pop_call();
		return;
	}

	if (HAS_BIT(root->flags, LIST_FLAG_LOG))
	{
		if (ses->logfile)
		{
			logit(ses, buf, ses->logfile);
		}
	}
	pop_call();
	return;
}

void tintin_header(struct session *ses, char *format, ...)
{
	char arg[BUFFER_SIZE], buf[BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsprintf(arg, format, args);
	va_end(args);

	if (strlen(arg) > gtd->ses->cols - 2)
	{
		arg[gtd->ses->cols - 2] = 0;
	}

	memset(buf, '#', gtd->ses->cols);

	memcpy(&buf[(gtd->ses->cols - strlen(arg)) / 2], arg, strlen(arg));

	buf[gtd->ses->cols] = 0;

	tintin_puts2(buf, ses);
}


void socket_printf(struct session *ses, size_t length, char *format, ...)
{
	char buf[BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	if (HAS_BIT(ses->flags, SES_FLAG_CONNECTED))
	{
		write(ses->socket, buf, length);
	}
}

void tintin_printf2(struct session *ses, char *format, ...)
{
	char buf[BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	tintin_puts2(buf, ses);
}

void tintin_printf(struct session *ses, char *format, ...)
{
	char buf[BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	tintin_puts(buf, ses);
}

/*
	output to screen should go through this function
	the output is NOT checked for actions or anything
*/

void tintin_puts2(char *cptr, struct session *ses)
{
	char output[BUFFER_SIZE];

	if (ses == NULL)
	{
		ses = gtd->ses;
	}

	if (!HAS_BIT(gtd->ses->flags, SES_FLAG_VERBOSE) && HAS_BIT(gtd->flags, TINTIN_FLAG_QUIET))
	{
		return;
	}

	sprintf(output, "\033[0m%s\033[0m", cptr);

	add_line_buffer(ses, output, FALSE);

	if (ses != gtd->ses)
	{
		return;
	}

	if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
	{
		save_pos(ses);
		goto_rowcol(ses, ses->bot_row, 1);
	}

	printline(ses, output, FALSE);

	if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
	{
		restore_pos(ses);
	}
}

/*
	output to screen should go through this function
	the output IS treated as though it came from the mud
*/

void tintin_puts(char *cptr, struct session *ses)
{
	char buf[BUFFER_SIZE];

	if (ses == NULL)
	{
		ses = gtd->ses;
	}

	sprintf(buf, "%s", cptr);

	do_one_line(buf, ses);

	if (!HAS_BIT(ses->flags, SES_FLAG_GAG))
	{
		tintin_puts2(buf, ses);
	}
	else
	{
		DEL_BIT(ses->flags, SES_FLAG_GAG);
	}
}


/*************************************************/
/* print system call error message and terminate */
/*************************************************/

void syserr(char *msg)
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

void show_cpu(struct session *ses)
{
	long long total_cpu;
	int timer;

	tintin_printf2(ses, "Section                           Time (usec)    Freq (msec)  %%Prog         %%CPU");

	tintin_printf2(ses, "");

	for (total_cpu = timer = 0 ; timer < TIMER_CPU ; timer++)
	{
		total_cpu += display_timer(ses, timer);
	}

	tintin_printf2(ses, "");

	tintin_printf2(ses, "Unknown CPU Usage:              %6.2f percent", (gtd->total_io_exec - total_cpu) * 100.0 / (gtd->total_io_delay + gtd->total_io_exec));
	tintin_printf2(ses, "Average CPU Usage:              %6.2f percent", (gtd->total_io_exec)             * 100.0 / (gtd->total_io_delay + gtd->total_io_exec));
}


long long display_timer(struct session *ses, int timer)
{
	long long total_usage, indicated_usage;

	total_usage = gtd->total_io_exec + gtd->total_io_delay;

	if (total_usage == 0)
	{
		return 0;
	}

	if (gtd->timer[timer][1] == 0 || gtd->timer[timer][4] == 0)
	{
		return 0;
	}

	indicated_usage = gtd->timer[timer][0] / gtd->timer[timer][1] * gtd->timer[timer][4];

	tintin_printf2(ses, "%-30s%8lld       %8lld      %8.2f     %8.2f",
		timer_table[timer].name,
		gtd->timer[timer][0] / gtd->timer[timer][1],
		gtd->timer[timer][3] / gtd->timer[timer][4] / 1000,
		100.0 * (double) indicated_usage / (double) gtd->total_io_exec,
		100.0 * (double) indicated_usage / (double) total_usage);

	return indicated_usage;
}


void open_timer(int timer)
{
	struct timeval last_time;
	long long current_time;

	gettimeofday(&last_time, NULL);

	current_time = (long long) last_time.tv_usec + 1000000LL * (long long) last_time.tv_sec;

	if (gtd->timer[timer][2] == 0)
	{
		gtd->timer[timer][2] = current_time ;
	}
	else
	{
		gtd->timer[timer][3] += current_time - gtd->timer[timer][2];
		gtd->timer[timer][2]  = current_time;
		gtd->timer[timer][4] ++;
	}
}


void close_timer(int timer)
{
	struct timeval last_time;
	long long current_time;

	gettimeofday(&last_time, NULL);

	current_time = (long long) last_time.tv_usec + 1000000LL * (long long) last_time.tv_sec;

	gtd->timer[timer][0] += (current_time - gtd->timer[timer][2]);
	gtd->timer[timer][1] ++;
}
		
/*
	Whoops, strcasecmp wasn't found.
*/

#if !defined(HAVE_STRCASECMP)
#define UPPER(c) (islower(c) ? toupper(c) : c)

int strcasecmp(char *string1, char *string2)
{
	for ( ; UPPER(*string1) == UPPER(*string2) ; string1++, string2++)
		if (!*string1)
			return(0);
	return(UPPER(*string1) - UPPER(*string2));
}

int strncasecmp(char *string1, char *string2, size_t count)
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
