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
	int scroll_cnt, grep_cnt, grep_min, grep_max, grep_add, page;

	grep_cnt = grep_add = scroll_cnt = grep_min = 0;
	grep_max = ses->bot_row - ses->top_row - 2;

	get_arg_in_braces(ses, arg, left, FALSE);
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
		page = get_number(ses, left);

		if (page)
		{
			arg = get_arg_in_braces(ses, arg, left, FALSE);
			arg = get_arg_in_braces(ses, arg, right, TRUE);

			if (*right == 0)
			{
				tintin_printf2(ses, "#GREP PAGE {%s} FOR WHAT?", left);

				return ses;
			}
		}
		else
		{
			page = 1;

			arg = get_arg_in_braces(ses, arg, right, TRUE);
		}

		if (page > 0)
		{
			grep_min = grep_max * page - grep_max;
			grep_max = grep_max * page;
		}
		else
		{
			grep_min = grep_max * (page * -1) - grep_max;
			grep_max = grep_max * (page * -1);
		}

		SET_BIT(ses->flags, SES_FLAG_SCROLLSTOP);

		tintin_header(ses, " GREPPING PAGE %d FOR %s ", page, right);

		if (page > 0)
		{
			scroll_cnt = ses->scroll_row;

			do
			{
				if (scroll_cnt == ses->scroll_max - 1)
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

				if (find(ses, ses->buffer[scroll_cnt], right, SUB_NONE))
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

				if (str_hash_grep(ses->buffer[scroll_cnt], FALSE))
				{
					continue;
				}

				if (find(ses, ses->buffer[scroll_cnt], right, SUB_NONE))
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
				if (scroll_cnt == -1)
				{
					scroll_cnt = 0;
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

				if (find(ses, ses->buffer[scroll_cnt], right, SUB_NONE))
				{
					grep_add = str_hash_lines(ses->buffer[scroll_cnt]);

					if (grep_cnt >= grep_min)
					{
						tintin_puts2(ses, ses->buffer[scroll_cnt]);
					}

					if (grep_cnt + grep_add >= grep_max)
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
		}
		tintin_header(ses, "");

		DEL_BIT(ses->flags, SES_FLAG_SCROLLSTOP);
	}
	return ses;
}

int show_buffer(struct session *ses)
{
	char temp[STRING_SIZE];
	int scroll_size, scroll_cnt, scroll_tmp, scroll_add, scroll_cut;

	if (ses != gtd->ses)
	{
		return TRUE;
	}

	scroll_size = get_scroll_size(ses);
	scroll_add  = 0 - ses->scroll_base;
	scroll_tmp  = 0;
	scroll_cnt  = ses->scroll_line;
	scroll_cut  = 0;
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
				scroll_cut = 0;
			}
			else
			{
				scroll_cut = scroll_tmp - (scroll_size - scroll_add);
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
		If the top line exists of multiple lines split it in the middle.
	*/

	if (ses->buffer[scroll_cnt] && scroll_cut)
	{
		if (scroll_add >= 0)
		{
			word_wrap_split(ses, ses->buffer[scroll_cnt], temp, scroll_tmp - scroll_cut, scroll_cut);

			printf("%s\n", temp);
		}
		else
		{
			word_wrap_split(ses, ses->buffer[scroll_cnt], temp, ses->scroll_base, scroll_size);

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
			scroll_cut = scroll_add;
			break;
		}

		scroll_add -= scroll_tmp;

		printf("%s\n", temp);
	}

	/*
		If the bottom line exists of multiple lines split it in the middle
	*/

	if (scroll_tmp && ses->buffer[scroll_cnt])
	{
		word_wrap_split(ses, ses->buffer[scroll_cnt], temp, 0, scroll_cut);
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
	char left[BUFFER_SIZE];
	int cnt;

	arg = get_arg_in_braces(ses, arg, left, FALSE);

	if (HAS_BIT(gtd->flags, TINTIN_FLAG_RESETBUFFER))
	{
		DEL_BIT(gtd->flags, TINTIN_FLAG_RESETBUFFER);

		reset_hash_table();
	}

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

		buffer_table[cnt].fun(ses, arg);

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
	int scroll_cnt, grep_cnt, grep_max, page;

	grep_cnt = grep_max = scroll_cnt = 0;

	sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);

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
		page = get_number(ses, left);

		if (page)
		{
			arg = sub_arg_in_braces(ses, arg, left,  GET_ONE, SUB_VAR|SUB_FUN);
			arg = sub_arg_in_braces(ses, arg, right, GET_ALL, SUB_VAR|SUB_FUN);

			if (*right == 0)
			{
				tintin_printf2(NULL, "#BUFFER, FIND OCCURANCE %d OF WHAT?", page);

				return;
			}
		}
		else
		{
			page = 1;

			arg = sub_arg_in_braces(ses, arg, right, GET_ALL, SUB_VAR|SUB_FUN);
		}

		if (page > 0)
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

				if (find(ses, ses->buffer[scroll_cnt], right, SUB_NONE))
				{
					grep_cnt++;

					if (grep_cnt == page)
					{
						break;
					}
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

				if (find(ses, ses->buffer[scroll_cnt], right, SUB_NONE))
				{
					grep_cnt--;

					if (grep_cnt == page)
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

DO_BUFFER(buffer_get)
{
	char arg1[BUFFER_SIZE], arg2[BUFFER_SIZE], arg3[BUFFER_SIZE];
	int min, max, cur, cnt, add;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_NST, FALSE);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ONE, SUB_VAR|SUB_FUN);

	min = get_number(ses, arg2);
	max = get_number(ses, arg3);

	if (*arg1 == 0 || *arg2 == 0)
	{
		tintin_printf(ses, "#SYNTAX: #BUFFER GET <VARIABLE> <LOWER BOUND> [UPPER BOUND]");
		return;
	}

	if (*arg3 == 0)
	{
		cur = UMAX(0, (ses->scroll_row + min) % ses->scroll_max);

		if (ses->buffer[cur] == NULL)
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg1, "");
		}
		else
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg1, "%s", ses->buffer[cur]);
		}
		return;
	}

	cnt = 0;
	add = (min < max) ? 1 : -1;

	set_nest_node(ses->list[LIST_VARIABLE], arg1, "");

	while ((add == 1 && max >= min) || (add == -1 && max <= min))
	{
		sprintf(arg2, "%s[%d]", arg1, ++cnt);

		cur = (ses->scroll_row + min) % ses->scroll_max;

		if (ses->buffer[cur] == NULL)
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "");
		}
		else
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%s", ses->buffer[cur]);
		}

		min = min + add;
	}
	return;
}

DO_BUFFER(buffer_write)
{
	FILE *fp;
	char left[BUFFER_SIZE], out[STRING_SIZE];
	int cnt;

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);

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
