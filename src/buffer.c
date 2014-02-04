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
*              (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                   *
*                                                                             *
*                     coded by Igor van den Hoven 2004                        *
******************************************************************************/

#include "tintin.h"


void init_buffer(struct session *ses, int size)
{
	int cnt;

	push_call("init_buffer(%p,%p)",ses,size);

	if (ses->scroll_max == size)
	{
		pop_call();
		return;
	}

	if (ses->buffer)
	{
		cnt = ses->scroll_row;
		
		do
		{
			if (++cnt == ses->scroll_max)
			{
				cnt = 0;
			}

			if (ses->buffer[cnt] == NULL)
			{
				break;
			}

			str_unhash(ses->buffer[cnt]);
		}
		while (cnt != ses->scroll_row);
	}

	if (ses->buffer)
	{
		free(ses->buffer);
	}

	if (size)
	{
		ses->buffer = (char **) calloc(size, sizeof(char *));

		ses->scroll_max  = size;
		ses->scroll_row  = size - 1;
		ses->scroll_line = - 1;
	}
	pop_call();
	return;
}


void add_line_buffer(struct session *ses, char *line, int more_output)
{
	char linebuf[STRING_SIZE];
	char *pti, *pto;
	int lines;
	int sav_row, sav_col, cur_row, cur_col, top_row, bot_row;

	push_call("add_line_buffer(%p,%s,%d)",ses,line,more_output);

	if (ses->buffer == NULL || HAS_BIT(ses->flags, SES_FLAG_SCROLLSTOP))
	{
		pop_call();
		return;
	}

	sav_row = ses->sav_row;
	sav_col = ses->sav_col;
	cur_row = ses->cur_row;
	cur_col = ses->cur_col;
	top_row = ses->top_row;
	bot_row = ses->bot_row;

	if (more_output == TRUE)
	{
		if (strlen(ses->more_output) < BUFFER_SIZE / 2)
		{
			strcat(ses->more_output, line);

			pop_call();
			return;
		}
	}

	if (HAS_BIT(gtd->flags, TINTIN_FLAG_RESETBUFFER))
	{
		DEL_BIT(gtd->flags, TINTIN_FLAG_RESETBUFFER);

		reset_hash_table();
	}

	strcat(ses->more_output, line);

	pti = pto = ses->more_output;

	while (*pti != 0)
	{
		while (skip_vt102_codes_non_graph(pti))
		{
			interpret_vt102_codes(ses, pti, FALSE);

			pti += skip_vt102_codes_non_graph(pti);
		}

		if (*pti == 0)
		{
			break;
		}

		if (SCROLL(ses))
		{
			*pto++ = *pti++;
		}
		else
		{
			pti++;
		}
	}
	*pto = 0;

	lines = word_wrap(ses, ses->more_output, linebuf, FALSE);

	ses->more_output[0] = 0;

	ses->buffer[ses->scroll_row] = str_hash(linebuf, lines);

	if (more_output == -1)
	{
		str_hash_grep(ses->buffer[ses->scroll_row], TRUE);
	}

	if (!HAS_BIT(ses->flags, SES_FLAG_LOGLEVEL))
	{
		if (ses->logfile)
		{
			logit(ses, linebuf, ses->logfile, TRUE);
		}
	}

	if (gtd->chat)
	{
		chat_forward_session(ses, linebuf);
	}

	if (ses->logline)
	{
		logit(ses, linebuf, ses->logline, TRUE);

		fclose(ses->logline);
		ses->logline = NULL;
	}

	if (--ses->scroll_row < 0)
	{
		ses->scroll_row = ses->scroll_max -1;
	}

	if (ses->buffer[ses->scroll_row])
	{
		ses->buffer[ses->scroll_row] = str_unhash(ses->buffer[ses->scroll_row]);
	}

	ses->sav_row = sav_row;
	ses->sav_col = sav_col;
	ses->cur_row = cur_row;
	ses->cur_col = cur_col;
	ses->top_row = top_row;
	ses->bot_row = bot_row;

	pop_call();
	return;
}

DO_COMMAND(do_grep)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int scroll_cnt, grep_cnt, grep_min, grep_max, grep_add;

	grep_cnt = grep_add = scroll_cnt = grep_min = 0;
	grep_max = ses->bot_row - ses->top_row - 2;

	get_arg_in_braces(arg, left, FALSE);
	substitute(ses, left, left, SUB_VAR|SUB_FUN);

	if (ses->buffer == NULL)
	{
		tintin_puts2(ses, "#GREP, NO SCROLL BUFFER AVAILABLE.");
	}
	else if (*left == 0)
	{
		tintin_puts2(ses, "#GREP WHAT?");
	}
	else
	{
		if (is_number(left))
		{
			arg = get_arg_in_braces(arg, left,  FALSE);
			arg = get_arg_in_braces(arg, right, TRUE);

			if (*right == 0)
			{
				tintin_printf2(ses, "#GREP WHAT OF PAGE {%s} ?", left);

				return ses;
			}
			grep_min += grep_max * atoi(left);
			grep_max += grep_max * atoi(left);
		}
		else
		{
			arg = get_arg_in_braces(arg, right, TRUE);
		}

		SET_BIT(ses->flags, SES_FLAG_SCROLLSTOP);

		tintin_header(ses, " GREP %s ", right);

		scroll_cnt = ses->scroll_row;

		do
		{
			if (scroll_cnt == ses->scroll_max -1)
			{
				scroll_cnt = 0;
			}
			else
			{
				scroll_cnt++;
			}

			if (ses->buffer[scroll_cnt] == NULL)
			{
				break;
			}

			if (str_hash_grep(ses->buffer[scroll_cnt], FALSE))
			{
				continue;
			}

			if (find(ses, ses->buffer[scroll_cnt], right))
			{
				grep_add = str_hash_lines(ses->buffer[scroll_cnt]);

				if (grep_cnt + grep_add > grep_max)
				{
					break;
				}

				grep_cnt += grep_add;
			}
		}
		while (scroll_cnt != ses->scroll_row);

		if (grep_cnt <= grep_min)
		{
			tintin_puts2(ses, "#NO MATCHES FOUND.");
		}
		else do
		{
			if (scroll_cnt == 0)
			{
				scroll_cnt = ses->scroll_max -1;
			}
			else
			{
				scroll_cnt--;
			}

			if (ses->buffer[scroll_cnt] == NULL)
			{
				break;
			}

			if (str_hash_grep(ses->buffer[scroll_cnt], FALSE))
			{
				continue;
			}

			if (find(ses, ses->buffer[scroll_cnt], right))
			{
				grep_add = str_hash_lines(ses->buffer[scroll_cnt]);

				if (grep_cnt - grep_add < grep_min)
				{
					break;
				}

				grep_cnt -= grep_add;

				tintin_puts2(ses, ses->buffer[scroll_cnt]);
			}
		}
		while (scroll_cnt != ses->scroll_row);

		tintin_header(ses, "");

		DEL_BIT(ses->flags, SES_FLAG_SCROLLSTOP);
	}
	return ses;
}

int show_buffer(struct session *ses)
{
	char temp[STRING_SIZE], wrap[STRING_SIZE], *temp_ptr, *wrap_ptr;
	int scroll_size, scroll_cnt, scroll_tmp, scroll_add, skip;

	if (ses != gtd->ses)
	{
		return TRUE;
	}

	scroll_size = get_scroll_size(ses);
	scroll_add  = 0 - ses->scroll_base;
	scroll_tmp  = 0;
	scroll_cnt  = ses->scroll_line;

	/*
		Find the upper limit of the buffer shown
	*/

	while (TRUE)
	{
		if (ses->buffer[scroll_cnt] == NULL)
		{
			break;
		}

		scroll_tmp = str_hash_lines(ses->buffer[scroll_cnt]);

		if (scroll_add + scroll_tmp > scroll_size)
		{
			if (scroll_add == scroll_size)
			{
				scroll_tmp = 0;
			}
			else
			{
				scroll_tmp -= scroll_size - scroll_add;
			}
			break;
		}

		scroll_add += scroll_tmp;

		if (scroll_cnt == ses->scroll_max - 1)
		{
			scroll_cnt = 0;
		}
		else
		{
			scroll_cnt++;
		}
	}


	if (ses->buffer[scroll_cnt] == NULL)
	{
		erase_screen(ses);
	}

	if (IS_SPLIT(ses))
	{
		save_pos(ses);
		goto_rowcol(ses, ses->bot_row, 1);
		SET_BIT(ses->flags, SES_FLAG_READMUD);
	}

	/*
		If the top line exists of multiple lines split in the middle.
	*/

	if (ses->buffer[scroll_cnt] && scroll_tmp)
	{
		word_wrap(ses, ses->buffer[scroll_cnt], temp, FALSE);

		temp_ptr = temp;
		wrap_ptr = wrap;

		while (scroll_tmp)
		{
			switch (*temp_ptr)
			{
				case ESCAPE:
					for (skip = skip_vt102_codes(temp_ptr) ; skip > 0 ; skip--)
					{
						*wrap_ptr = *temp_ptr;
						temp_ptr++;
						wrap_ptr++;
					}
					continue;

				case '\n':
					temp_ptr++;
					scroll_tmp--;
					continue;

				case '\0':
					printf("\033[1;31m(buffer error)\n");
					scroll_tmp--;
					break;

				default:
					temp_ptr++;
					break;
			}
		}
		*wrap_ptr = 0;

		if (scroll_add >= 0)
		{
			printf("%s%s\n", wrap, temp_ptr);
		}
		else
		{
			wrap_ptr = temp_ptr;

			for (wrap_ptr = temp_ptr ; scroll_tmp++ < scroll_size ; temp_ptr++)
			{
				temp_ptr = strchr(temp_ptr, '\n');
			}
			*temp_ptr = 0;
			printf("%s\n", wrap_ptr);

			goto eof;
		}
	}

	/*
		Print away
	*/

	while (TRUE)
	{
		if (scroll_cnt == 0)
		{
			scroll_cnt = ses->scroll_max - 1;
		}
		else
		{
			scroll_cnt--;
		}

		if (ses->buffer[scroll_cnt] == NULL)
		{
			break;
		}

		scroll_tmp = word_wrap(ses, ses->buffer[scroll_cnt], temp, FALSE);

		if (scroll_add - scroll_tmp < 0)
		{
			scroll_tmp = scroll_add;
			break;
		}

		scroll_add -= scroll_tmp;

		printf("%s\n", temp);
	}

	/*
		If the bottom line exists of multiple lines split in the middle
	*/

	if (scroll_tmp && ses->buffer[scroll_cnt])
	{
		temp_ptr = temp;

		while (scroll_tmp--)
		{
			temp_ptr = strchr(temp_ptr, '\n');
		}
		*temp_ptr = 0;

		printf("%s\n", temp);
	}

	eof:

	if (IS_SPLIT(ses))
	{
		restore_pos(ses);
		DEL_BIT(ses->flags, SES_FLAG_READMUD);
	}
	return TRUE;
}


DO_COMMAND(do_buffer)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int cnt;

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);
	substitute(ses, right, right, SUB_VAR|SUB_FUN);

	if (*left == 0)
	{
		tintin_header(ses, " BUFFER COMMANDS ");

		for (cnt = 0 ; *buffer_table[cnt].name != 0 ; cnt++)
		{
			tintin_printf2(ses, "  [%-13s] %s", buffer_table[cnt].name, buffer_table[cnt].desc);
		}
		tintin_header(ses, "");

		return ses;
	}

	for (cnt = 0 ; *buffer_table[cnt].name ; cnt++)
	{
		if (!is_abbrev(left, buffer_table[cnt].name))
		{
			continue;
		}

		buffer_table[cnt].fun(ses, right);

		return ses;
	}

	do_buffer(ses, "");

	return ses;
}

DO_BUFFER(buffer_clear)
{
	int cnt;

	cnt = ses->scroll_row;
		
	do
	{
		if (++cnt == ses->scroll_max)
		{
			cnt = 0;
		}

		if (ses->buffer[cnt] == NULL)
		{
			break;
		}
		ses->buffer[cnt] = str_unhash(ses->buffer[cnt]);
	}
	while (cnt != ses->scroll_row);

	ses->scroll_row  = ses->scroll_max - 1;
	ses->scroll_line = - 1;

	return;
}

DO_BUFFER(buffer_up)
{
	int scroll_size, scroll_cnt, buffer_add, buffer_tmp;

	if (ses->buffer == NULL)
	{
		return;
	}

	if (ses->scroll_line == -1)
	{
		ses->scroll_line = ses->scroll_row + 1;
	}

	scroll_size = get_scroll_size(ses);
	scroll_cnt  = ses->scroll_line;
	buffer_add  = 0 - ses->scroll_base;

	while (TRUE)
	{
		if (ses->buffer[scroll_cnt] == NULL)
		{
			return;
		}

		buffer_tmp = str_hash_lines(ses->buffer[scroll_cnt]);

		if (scroll_size < buffer_add + buffer_tmp)
		{
			ses->scroll_line = scroll_cnt;
			ses->scroll_base = scroll_size - buffer_add;

			break;
		}

		buffer_add += buffer_tmp;

		if (scroll_cnt == ses->scroll_max - 1)
		{
			scroll_cnt = 0;
		}
		else
		{
			scroll_cnt++;
		}
	}

	show_buffer(ses);
}

DO_BUFFER(buffer_down)
{
	int scroll_size, scroll_cnt, buffer_add, buffer_tmp;

	if (ses->buffer == NULL)
	{
		return;
	}

	if (ses->scroll_line == -1)
	{
		return;
	}

	scroll_size = get_scroll_size(ses);
	scroll_cnt  = ses->scroll_line;
	buffer_add  = ses->scroll_base;

	if (scroll_size <= buffer_add)
	{
		ses->scroll_base = buffer_add - scroll_size;

		show_buffer(ses);
		return;
	}

	if (scroll_cnt == 0)
	{
		scroll_cnt = ses->scroll_max - 1;
	}
	else
	{
		scroll_cnt--;
	}

	while (TRUE)
	{
		if (ses->buffer[scroll_cnt] == NULL)
		{
			buffer_end(ses, "");
			return;
		}

		buffer_tmp = str_hash_lines(ses->buffer[scroll_cnt]);

		if (scroll_size <= buffer_add + buffer_tmp)
		{
			ses->scroll_line = scroll_cnt;
			ses->scroll_base = buffer_tmp - (scroll_size - buffer_add);

			break;
		}

		buffer_add += buffer_tmp;

		if (scroll_cnt == 0)
		{
			scroll_cnt = ses->scroll_max - 1;
		}
		else
		{
			scroll_cnt--;
		}
	}

	show_buffer(ses);
}

DO_BUFFER(buffer_home)
{
	if (ses->buffer == NULL)
	{
		return;
	}

	if (ses->buffer[0])
	{
		ses->scroll_line = ses->scroll_row - 1;
	}
	else
	{
		ses->scroll_line = ses->scroll_max - 1;
	}
	ses->scroll_base = str_hash_lines(ses->buffer[ses->scroll_line]);

	buffer_down(ses, "");
}

DO_BUFFER(buffer_end)
{
	if (ses->buffer == NULL)
	{
		return;
	}

	if (ses->scroll_row == ses->scroll_max - 1)
	{
		ses->scroll_line = 0;
	}
	else
	{
		ses->scroll_line = ses->scroll_row + 1;
	}

	ses->scroll_base = 0;

	show_buffer(ses);

	ses->scroll_line = -1;
	ses->scroll_base =  0;
}

DO_BUFFER(buffer_lock)
{
	if (ses->buffer == NULL)
	{
		return;
	}

	if (ses->scroll_line == -1)
	{
		ses->scroll_line = ses->scroll_row + 1;
	}
	else
	{
		buffer_end(ses, "");
	}
}

DO_BUFFER(buffer_find)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int scroll_cnt, grep_cnt, grep_max;

	grep_cnt = grep_max = scroll_cnt = 0;

	get_arg_in_braces(arg, left, FALSE);

	if (ses->buffer == NULL)
	{
		tintin_puts2(NULL, "#BUFFER, NO SCROLL BUFFER AVAILABLE.");
	}
	else if (*left == 0)
	{
		tintin_puts2(NULL, "#BUFFER, FIND WHAT?");
	}
	else
	{
		if (is_number(left))
		{
			arg = get_arg_in_braces(arg, left,  FALSE);
			arg = get_arg_in_braces(arg, right, TRUE);

			if (*right == 0)
			{
				tintin_printf2(NULL, "#BUFFER, FIND WHAT OF OCCURANCE {%s} ?", left);

				return;
			}
			grep_max = atoi(left);
		}
		else
		{
			arg = get_arg_in_braces(arg, right, TRUE);
		}

		if (grep_max >= 0)
		{
			scroll_cnt = ses->scroll_row;

			do
			{
				if (scroll_cnt == ses->scroll_max -1)
				{
					scroll_cnt = 0;
				}
				else
				{
					scroll_cnt++;
				}

				if (ses->buffer[scroll_cnt] == NULL)
				{
					break;
				}

				if (str_hash_grep(ses->buffer[scroll_cnt], FALSE))
				{
					continue;
				}

				if (find(ses, ses->buffer[scroll_cnt], right))
				{
					if (grep_cnt == grep_max)
					{
						break;
					}
					grep_cnt++;
				}
			}
			while (scroll_cnt != ses->scroll_row);
		}
		else
		{
			if (ses->buffer[0])
			{
				scroll_cnt = ses->scroll_row - 1;
			}
			else
			{
				scroll_cnt = ses->scroll_max - 1;
			}

			do
			{
				if (scroll_cnt == 0)
				{
					scroll_cnt = ses->scroll_max -1;
				}
				else
				{
					scroll_cnt--;
				}

				if (ses->buffer[scroll_cnt] == NULL)
				{
					break;
				}

				if (str_hash_grep(ses->buffer[scroll_cnt], FALSE))
				{
					continue;
				}

				if (find(ses, ses->buffer[scroll_cnt], right))
				{
					grep_cnt--;

					if (grep_cnt == grep_max)
					{
						break;
					}

				}
			}
			while (scroll_cnt != ses->scroll_row);
		}

		if (ses->buffer[scroll_cnt] == NULL || scroll_cnt == ses->scroll_row)
		{
			tintin_puts(NULL, "#BUFFER FIND, NO MATCHES FOUND.");
		}
		else
		{
			ses->scroll_line = scroll_cnt;

			show_buffer(ses);
		}
	}
	return;
}

DO_BUFFER(buffer_write)
{
	FILE *fp;
	char left[BUFFER_SIZE], out[STRING_SIZE];
	int cnt;

	arg = get_arg_in_braces(arg, left, FALSE);

	if (*left == 0)
	{
		tintin_printf(ses, "#SYNTAX: #BUFFER WRITE <FILENAME>]");
	}
	else
	{
		if ((fp = fopen(left, "w")))
		{
			show_message(ses, LIST_MESSAGE, "#OK: WRITING BUFFER TO '%s'.", left);

			if (HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
			{
				write_html_header(fp);
			}

			cnt = ses->scroll_row;

			do
			{
				if (cnt == 0)
				{
					cnt = ses->scroll_max - 1;
				}
				else
				{
					cnt--;
				}

				if (ses->buffer[cnt] == NULL)
				{
					continue;
				}

				if (HAS_BIT(ses->flags, SES_FLAG_LOGPLAIN))
				{
					strip_vt102_codes(ses->buffer[cnt], out);
				}
				else if (HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
				{
					vt102_to_html(ses, ses->buffer[cnt], out);
				}
				else
				{
					strcpy(out, ses->buffer[cnt]);
				}
				strcat(out, "\n");

				fputs(out, fp);
			}
			while (cnt != ses->scroll_row);

			fclose(fp);
		}
		else
		{
			tintin_printf(ses, "#ERROR: #BUFFER WRITE {%s} - FAILED TO OPEN FILE.", left);
		}
	}
	return;
}
