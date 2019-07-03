/******************************************************************************
*   This file is part of TinTin++                                             *
*                                                                             *
*   Copyright 1992-2019 (See CREDITS file)                                    *
*                                                                             *
*   TinTin++ is free software; you can redistribute it and/or modify          *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 3 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with TinTin++.  If not, see https://www.gnu.org/licenses.           *
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
	if (*s1 == 0)
	{
		return FALSE;
	}
	return !strncasecmp(s2, s1, strlen(s1));
}


int is_color_code(char *str)
{
	if (str[0] == '<')
	{
		if (!isalnum((int) str[1]) || !isalnum((int) str[2]) || !isalnum((int) str[3]) || str[4] != '>')
		{
			return FALSE;
		}
		else if (str[1] >= '0' && str[1] <= '9' && str[2] >= '0' && str[2] <= '9' && str[3] >= '0' && str[3] <= '9')
		{
			return TRUE;
		}
		else if (str[1] >= 'a' && str[1] <= 'f' && str[2] >= 'a' && str[2] <= 'f' && str[3] >= 'a' && str[3] <= 'f')
		{
			return TRUE;
		}
		else if (str[1] >= 'A' && str[1] <= 'F' && str[2] >= 'A' && str[2] <= 'F' && str[3] >= 'A' && str[3] <= 'F')
		{
			return TRUE;
		}
		else if (str[1] == 'g' && str[2] >= '0' && str[2] <= '9' && str[3] >= '0' && str[3] <= '9')
		{
			return TRUE;
		}
		else if (str[1] == 'G' && str[2] >= '0' && str[2] <= '9' && str[3] >= '0' && str[3] <= '9')
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*
	Keep synched with tintoi() and is_math()
*/

int is_number(char *str)
{
	char *ptr = str;
	int i = 1, d = 0;

	if (*ptr == 0)
	{
		return FALSE;
	}

	ptr = str + strlen(str);

	while (TRUE)
	{
		ptr--;

		switch (*ptr)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				break;

			case '.':
				if (d)
				{
					return FALSE;
				}
				d = 1;
				break;

			case ':':
				if (i == 4)
				{
					return FALSE;
				}
				i++;
				break;

			case '!':
			case '~':
			case '+':
			case '-':
				if (ptr != str)
				{
					return FALSE;
				}
				break;

			default:
				return FALSE;
		}

		if (ptr == str)
		{
			break;
		}
	}
	return TRUE;
}

int hex_digit(char *str)
{
	if (isdigit((int) *str))
	{
		return *str - '0';
	}
	else
	{
		return toupper((int) *str) - 'A' + 10;
	}
}

unsigned long long hex_number_64bit(char *str)
{
	unsigned long long len, mul, val = 0;

	for (len = 0 ; len < 16 ; len++)
	{
		if (!isxdigit((int) str[len]))
		{
			break;
		}
	}

	for (mul = 1 ; len > 0 ; mul *= 16)
	{
		val += mul * hex_digit(str + --len);
	}

	return val;
}

int hex_number_8bit(char *str)
{
	int value = 0;

	if (str)
	{
		if (isdigit((int) *str))
		{
			value += 16 * (*str - '0');
		}
		else
		{
			value += 16 * (toupper((int) *str) - 'A' + 10);
		}
		str++;
	}

	if (str)
	{
		if (isdigit((int) *str))
		{
			value += *str - '0';
		}
		else
		{
			value += toupper((int) *str) - 'A' + 10;
		}
		str++;
	}

	return value;
}

int oct_number(char *str)
{
	int value = 0;

	if (str)
	{
		if (isdigit((int) *str))
		{
			value += 8 * (*str - '0');
		}
		str++;
	}

	if (str)
	{
		if (isdigit((int) *str))
		{
			value += *str - '0';
		}
		str++;
	}

	return value;
}

int unicode_16_bit(char *str, char *out)
{
	int val = 0;

	if (isdigit((int) *str))
	{
		val += 4096 * (*str - '0');
	}
	else
	{
		val += 4096 * (toupper((int) *str) - 'A' + 10);
	}
	str++;

	if (isdigit((int) *str))
	{
		val += 256 * (*str - '0');
	}
	else
	{
		val += 256 * (toupper((int) *str) - 'A' + 10);
	}
	str++;

	if (isdigit((int) *str))
	{
		val += 16 * (*str - '0');
	}
	else
	{
		val += 16 * (toupper((int) *str) - 'A' + 10);
	}
	str++;

	if (isdigit((int) *str))
	{
		val += (*str - '0');
	}
	else
	{
		val += (toupper((int) *str) - 'A' + 10);
	}
	str++;

	if (val < 128)
	{
		*out++ = val;
		return 1;
	}
	else if (val < 4096)
	{
		*out++ = 192 + val / 64;
		*out++ = 128 + val % 64;
		return 2;
	}
	else
	{
		*out++ = 224 + val / 4096;
		*out++ = 128 + val / 64 % 64;
		*out++ = 128 + val % 64;
		return 3;
	}
}

int unicode_21_bit(char *str, char *out)
{
	int val = 0;

	if (str)
	{
		if (isdigit((int) *str))
		{
			val += 1048576 * (*str - '0');
		}
		else
		{
			val += 1048576 * (toupper((int) *str) - 'A' + 10);
		}
		str++;
	}

	if (str)
	{
		if (isdigit((int) *str))
		{
			val += 65536 * (*str - '0');
		}
		else
		{
			val += 65536 * (toupper((int) *str) - 'A' + 10);
		}
		str++;
	}

	if (str)
	{
		if (isdigit((int) *str))
		{
			val += 4096 * (*str - '0');
		}
		else
		{
			val += 4096 * (toupper((int) *str) - 'A' + 10);
		}
		str++;
	}

	if (str)
	{
		if (isdigit((int) *str))
		{
			val += 256 * (*str - '0');
		}
		else
		{
			val += 256 * (toupper((int) *str) - 'A' + 10);
		}
		str++;
	}

	if (str)
	{
		if (isdigit((int) *str))
		{
			val += 16 * (*str - '0');
		}
		else
		{
			val += 16 * (toupper((int) *str) - 'A' + 10);
		}
		str++;
	}

	if (str)
	{
		if (isdigit((int) *str))
		{
			val += (*str - '0');
		}
		else
		{
			val += (toupper((int) *str) - 'A' + 10);
		}
		str++;
	}

	if (val < 128)
	{
		*out++ = val;
		return 1;
	}
	else if (val < 4096)
	{
		*out++ = 192 + val / 64;
		*out++ = 128 + val % 64;
		return 2;
	}
	else if (val < 65536)
	{
		*out++ = 224 + val / 4096;
		*out++ = 128 + val / 64 % 64;
		*out++ = 128 + val % 64;
		return 3;
	}
	else if (val < 1114112)
	{
		*out++ = 240 + val / 262144;
		*out++ = 128 + val / 4096 % 64;
		*out++ = 128 + val / 64 % 64;
	        *out++ = 128 + val % 64;
	        return 4;
	}
	else
	{
		*out++ = 239;
		*out++ = 191;
		*out++ = 189;
		return 3;
	}
}

unsigned long long utime()
{
	struct timeval now_time;

	gettimeofday(&now_time, NULL);

	if (gtd->time >= now_time.tv_sec * 1000000ULL + now_time.tv_usec)
	{
		gtd->time++;
	}
	else
	{
		gtd->time = now_time.tv_sec * 1000000ULL + now_time.tv_usec;
	}
	return gtd->time;
}

void seed_rand(struct session *ses, unsigned long long seed)
{
	ses->rand = seed;
}

unsigned long long generate_rand(struct session *ses)
{
	ses->rand = 6364136223846793005ULL * ses->rand + 1ULL;

	return ses->rand;
}

char *capitalize(char *str)
{
	static char outbuf[BUFFER_SIZE];
	int cnt;

	for (cnt = 0 ; str[cnt] != 0 ; cnt++)
	{
		outbuf[cnt] = toupper((int) str[cnt]);
	}
	outbuf[cnt] = 0;

	return outbuf;
}

char *ntos(long long number)
{
	static char outbuf[100][NUMBER_SIZE];
	static int cnt;

	cnt = (cnt + 1) % 100;

	sprintf(outbuf[cnt], "%lld", number);

	return outbuf[cnt];
}

char *indent(int cnt)
{
	static char outbuf[BUFFER_SIZE];

	memset(outbuf, '\t', cnt);

	outbuf[cnt] = 0;

	return outbuf;
}

int cat_sprintf(char *dest, char *fmt, ...)
{
	char buf[STRING_SIZE];
	int size;

	va_list args;

	va_start(args, fmt);
	size = vsprintf(buf, fmt, args);
	va_end(args);

	strcat(dest, buf);

	return size;
}

void ins_sprintf(char *dest, char *fmt, ...)
{
	char buf[STRING_SIZE], tmp[STRING_SIZE];

	va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	strcpy(tmp, dest);
	strcpy(dest, buf);
	strcat(dest, tmp);
}

int str_suffix(char *str1, char *str2)
{
	int len1, len2;

	len1 = strlen(str1);
	len2 = strlen(str2);

	if (len1 >= len2)
	{
		if (!strcasecmp(str1 + len1 - len2, str2))
		{
			return FALSE;
		}
	}
	return TRUE;
}

void show_message(struct session *ses, int index, char *format, ...)
{
	struct listroot *root;
	char *buffer;
	va_list args;

	push_call("show_message(%p,%p,%p)",ses,index,format);

	va_start(args, format);
	vasprintf(&buffer, format, args);
	va_end(args);

	root = ses->list[index];

	if (HAS_BIT(root->flags, LIST_FLAG_LOG))
	{
		if (ses->logfile)
		{
			logit(ses, buffer, ses->logfile, TRUE);
		}
	}

	if (gtd->verbose_level)
	{
		tintin_puts2(ses, buffer);

		goto end;
	}

	if (HAS_BIT(root->flags, LIST_FLAG_DEBUG))
	{
		tintin_puts2(ses, buffer);

		goto end;
	}

	if (HAS_BIT(root->flags, LIST_FLAG_MESSAGE))
	{
		if (gtd->input_level == 0)
		{
			tintin_puts2(ses, buffer);
		}
	}

	end:

	free(buffer);

	pop_call();
	return;
}

struct session *show_error(struct session *ses, int index, char *format, ...)
{
	struct listroot *root;
	char *buffer;
	va_list args;

	push_call("show_error(%p,%p,%p)",ses,index,format);

	va_start(args, format);
	vasprintf(&buffer, format, args);
	va_end(args);

	if (gtd->verbose_level)
	{
		tintin_puts2(ses, buffer);

		goto end;
	}

	root = ses->list[index];

	if (HAS_BIT(root->flags, LIST_FLAG_LOG))
	{
		if (ses->logfile)
		{
			logit(ses, buffer, ses->logfile, TRUE);
		}
	}

	if (HAS_BIT(root->flags, LIST_FLAG_DEBUG))
	{
		tintin_puts2(ses, buffer);

		goto end;
	}

	if (HAS_BIT(root->flags, LIST_FLAG_MESSAGE))
	{
		tintin_puts2(ses, buffer);
	}

	end:

	free(buffer);

	pop_call();
	return ses;
}

void show_debug(struct session *ses, int index, char *format, ...)
{
	struct listroot *root;
	char buf[STRING_SIZE];
	va_list args;

	push_call("show_debug(%p,%p,%p)",ses,index,format);

	root = ses->list[index];

	if (!HAS_BIT(root->flags, LIST_FLAG_DEBUG) && !HAS_BIT(root->flags, LIST_FLAG_LOG))
	{
		pop_call();
		return;
	}

	va_start(args, format);

	vsprintf(buf, format, args);

	va_end(args);

	if (HAS_BIT(root->flags, LIST_FLAG_DEBUG))
	{
		gtd->verbose_level++;

		tintin_puts2(ses, buf);

		gtd->verbose_level--;

		pop_call();
		return;
	}

	if (HAS_BIT(root->flags, LIST_FLAG_LOG))
	{
		if (ses->logfile)
		{
			logit(ses, buf, ses->logfile, TRUE);
		}
	}
	pop_call();
	return;
}

void show_info(struct session *ses, int index, char *format, ...)
{
	struct listroot *root;
	char buf[STRING_SIZE];
	va_list args;

	push_call("show_info(%p,%p,%p)",ses,index,format);

	root = ses->list[index];

	if (!HAS_BIT(root->flags, LIST_FLAG_INFO))
	{
		pop_call();
		return;
	}

	va_start(args, format);

	vsprintf(buf, format, args);

	va_end(args);

	gtd->verbose_level++;

	tintin_puts(ses, buf);

	gtd->verbose_level--;

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

	if ((int) strlen(arg) > gtd->screen->cols - 2)
	{
		arg[gtd->screen->cols - 2] = 0;
	}

	if (HAS_BIT(ses->flags, SES_FLAG_SCREENREADER))
	{
		memset(buf, ' ', gtd->screen->cols);
	}
	else
	{
		memset(buf, '#', gtd->screen->cols);
	}

	memcpy(&buf[(gtd->screen->cols - strlen(arg)) / 2], arg, strlen(arg));

	buf[gtd->screen->cols] = 0;

	tintin_puts2(ses, buf);
}


void socket_printf(struct session *ses, size_t length, char *format, ...)
{
	size_t size;

	char buf[STRING_SIZE];
	va_list args;

	va_start(args, format);
	size = vsprintf(buf, format, args);
	va_end(args);

	if (size != length && HAS_BIT(ses->telopts, TELOPT_FLAG_DEBUG))
	{
		tintin_printf(ses, "DEBUG TELNET: socket_printf size difference: %d vs %d", size, length);
	}

	if (HAS_BIT(ses->flags, SES_FLAG_CONNECTED))
	{
		write_line_mud(ses, buf, length);
	}
}

void telnet_printf(struct session *ses, size_t length, char *format, ...)
{
	size_t size;

	char buf[STRING_SIZE];
	va_list args;

	va_start(args, format);
	size = vsprintf(buf, format, args);
	va_end(args);

	if (size != length && HAS_BIT(ses->telopts, TELOPT_FLAG_DEBUG))
	{
		tintin_printf(ses, "DEBUG TELNET: telnet_printf size difference: %d vs %d", size, length);
	}

	if (HAS_BIT(ses->flags, SES_FLAG_CONNECTED))
	{
		SET_BIT(ses->telopts, TELOPT_FLAG_TELNET);

		write_line_mud(ses, buf, length);

		DEL_BIT(ses->telopts, TELOPT_FLAG_TELNET);
	}
}

void tintin_printf2(struct session *ses, char *format, ...)
{
	char *buffer;
	va_list args;

	push_call("tintin_printf2(%p,%p,...)",ses,format);

	va_start(args, format);
	vasprintf(&buffer, format, args);
	va_end(args);

	tintin_puts2(ses, buffer);

	free(buffer);

	pop_call();
	return;
}

void tintin_printf(struct session *ses, char *format, ...)
{
	char buffer[BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	tintin_puts(ses, buffer);
}

/*
	Like tintin_puts2, but no color codes added
*/

void tintin_puts3(struct session *ses, char *string)
{
	char *output;

	push_call("tintin_puts3(%p,%p)",ses,string);

	if (ses == NULL)
	{
		ses = gtd->ses;
	}

	if (!HAS_BIT(gtd->ses->flags, SES_FLAG_VERBOSE) && gtd->quiet && gtd->verbose_level == 0)
	{
		pop_call();
		return;
	}

	if (strip_vt102_strlen(ses, ses->more_output) != 0)
	{
		output = str_dup_printf("\n%s", string);
	}
	else
	{
		output = str_dup_printf("%s", string);
	}

	add_line_buffer(ses, output, FALSE);

	if (ses == gtd->ses)
	{
		if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
		{
			save_pos(ses);
			goto_rowcol(ses, ses->bot_row, 1);
		}

		printline(ses, &output, FALSE);

		if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
		{
			restore_pos(ses);
		}
	}

	str_free(output);

	pop_call();
	return;
}

/*
	output to screen should go through this function
	the output is NOT checked for actions or anything
*/

void tintin_puts2(struct session *ses, char *string)
{
	char *output;

	push_call("tintin_puts2(%p,%p)",ses,string);

	if (ses == NULL)
	{
		ses = gtd->ses;
	}

	if (!HAS_BIT(gtd->ses->flags, SES_FLAG_VERBOSE) && gtd->quiet && gtd->verbose_level == 0)
	{
		pop_call();
		return;
	}

	if (strip_vt102_strlen(ses, ses->more_output) != 0)
	{
		output = str_dup_printf("\n\e[0m%s\e[0m", string);
	}
	else
	{
		output = str_dup_printf("\e[0m%s\e[0m", string);
	}

	add_line_buffer(ses, output, FALSE);

	if (ses == gtd->ses)
	{
		if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
		{
			save_pos(ses);
			goto_rowcol(ses, ses->bot_row, 1);
		}

		printline(ses, &output, FALSE);

		if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
		{
			restore_pos(ses);
		}
	}

	str_free(output);

	pop_call();
	return;
}

/*
	output to screen should go through this function
	the output IS treated as though it came from the mud
*/

void tintin_puts(struct session *ses, char *string)
{
	if (ses == NULL)
	{
		ses = gtd->ses;
	}

	do_one_line(string, ses);

	if (HAS_BIT(ses->flags, SES_FLAG_GAG))
	{
		DEL_BIT(ses->flags, SES_FLAG_GAG);

		gtd->ignore_level++;

		show_info(ses, LIST_GAG, "#INFO GAG {%s}", string);

		gtd->ignore_level--;
	}
	else
	{
		tintin_puts2(ses, string);
	}
}


/*************************************************/
/* print system call error message and terminate */
/*************************************************/

void syserr(char *msg)
{
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

	tintin_printf2(ses, "Unknown CPU Usage:             %7.3f percent", (gtd->total_io_exec - total_cpu) * 100.0 / (gtd->total_io_delay + gtd->total_io_exec));
	tintin_printf2(ses, "Average CPU Usage:             %7.3f percent", (gtd->total_io_exec)             * 100.0 / (gtd->total_io_delay + gtd->total_io_exec));
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

	tintin_printf2(ses, "%-30s%8lld       %8lld      %8.2f     %8.3f",
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
