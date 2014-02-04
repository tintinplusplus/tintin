/******************************************************************************
*   TinTin++                                                                  *
*   Copyright (C) 2005 (See CREDITS file)                                     *
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
*              (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                   *
*                                                                             *
*                     coded by Igor van den Hoven 2006                        *
******************************************************************************/


#include "tintin.h"

DO_COMMAND(do_cursor)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[BUFFER_SIZE];
	int cnt;

	arg = get_arg_in_braces(ses, arg, left,  TRUE);
	arg = get_arg_in_braces(ses, arg, right, TRUE);
	substitute(ses, right, right, SUB_VAR|SUB_FUN);

	if (*left == 0)
	{
		tintin_header(ses, " CURSOR OPTIONS ");

		for (cnt = 0 ; *cursor_table[cnt].fun ; cnt++)
		{
			if (*cursor_table[cnt].name)
			{
				convert_meta(cursor_table[cnt].code, temp);

				tintin_printf2(ses, "  [%-18s] [%-6s] %s", 
					cursor_table[cnt].name,
					temp,
					cursor_table[cnt].desc);
			}
		}

		tintin_header(ses, "");
	}
	else
	{
		for (cnt = 0 ; *cursor_table[cnt].fun ; cnt++)
		{
			if (is_abbrev(left, cursor_table[cnt].name))
			{
				cursor_table[cnt].fun(right);

				return ses;
			}
		}
		tintin_printf(ses, "#ERROR: #CURSOR {%s} IS NOT A VALID OPTION.", capitalize(left));
	}
	return ses;
}

// Multi-Byte support routines

int inputline_str_str_len(int start, int end)
{
	int raw_cnt, str_cnt, ret_cnt;

	raw_cnt = str_cnt = ret_cnt = 0;

	while (raw_cnt < gtd->input_len)
	{
		if (str_cnt >= end)
		{
			break;
		}

		if (HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8) && (gtd->input_buf[raw_cnt] & 192) == 192)
		{
			raw_cnt++;

			while (raw_cnt < gtd->input_len && (gtd->input_buf[raw_cnt] & 192) == 128)
			{
				raw_cnt++;
			}
		}
		else
		{
			raw_cnt++;
		}
		str_cnt++;

		if (str_cnt > start)
		{
			ret_cnt++;
		}
	}
	return ret_cnt;
}

int inputline_raw_str_len(int start, int end)
{
	int raw_cnt, ret_cnt;

	raw_cnt = start;
	ret_cnt = 0;

	while (raw_cnt < gtd->input_len)
	{
		if (raw_cnt >= end)
		{
			break;
		}

		if (HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8) && (gtd->input_buf[raw_cnt] & 192) == 192)
		{
			raw_cnt++;

			while (raw_cnt < gtd->input_len && (gtd->input_buf[raw_cnt] & 192) == 128)
			{
				raw_cnt++;
			}
		}
		else
		{
			raw_cnt++;
		}
		ret_cnt++;
	}
	return ret_cnt;
}

int inputline_str_raw_len(int start, int end)
{
	int raw_cnt, str_cnt, ret_cnt;

	raw_cnt = str_cnt = ret_cnt = 0;

	while (raw_cnt < gtd->input_len)
	{
		if (str_cnt >= end)
		{
			break;
		}

		if (HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8) && (gtd->input_buf[raw_cnt] & 192) == 192)
		{
			ret_cnt += (str_cnt >= start) ? 1 : 0;
			raw_cnt++;

			while (raw_cnt < gtd->input_len && (gtd->input_buf[raw_cnt] & 192) == 128)
			{
				ret_cnt += (str_cnt >= start) ? 1 : 0;
				raw_cnt++;
			}
		}
		else
		{
			ret_cnt += (str_cnt >= start) ? 1 : 0;
			raw_cnt++;
		}
		str_cnt++;
	}
	return ret_cnt;
}

int inputline_raw_raw_len(int start, int end)
{
	int raw_cnt, ret_cnt;

	raw_cnt = ret_cnt = 0;

	while (raw_cnt < gtd->input_len)
	{
		if (raw_cnt >= end)
		{
			break;
		}

		if (HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8) && (gtd->input_buf[raw_cnt] & 192) == 192)
		{
			ret_cnt += (raw_cnt >= start) ? 1 : 0;
			raw_cnt++;

			while (raw_cnt < gtd->input_len && (gtd->input_buf[raw_cnt] & 192) == 128)
			{
				ret_cnt += (raw_cnt >= start) ? 1 : 0;
				raw_cnt++;
			}
		}
		else
		{
			ret_cnt += (raw_cnt >= start) ? 1 : 0;
			raw_cnt++;
		}
	}
	return ret_cnt;
}

// Get string length of the input area

int inputline_max_str_len(void)
{
	return gtd->ses->cols + 1 - gtd->input_off - (HAS_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH) ? 11 : 0);
}

int inputline_cur_str_len(void)
{
	return inputline_str_str_len(gtd->input_hid, gtd->input_hid + inputline_max_str_len());
}

// Check for invalid characters.

int inputline_str_chk(int offset, int totlen)
{
	while (offset < totlen)
	{
		if (HAS_BIT(gtd->ses->flags, SES_FLAG_BIG5))
		{
			if (gtd->input_buf[offset] & 128)
			{
				if (offset + 1 < totlen || gtd->input_buf[offset+1] == 0)
				{
					return FALSE;
				}
				offset += 2;
			}
			else
			{
				offset += 1;
			}
		}
		else if (HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8))
		{
			switch (gtd->input_buf[offset] & (128+64+32+16))
			{
				case 128+64:
				case 128+64+16:
					if (offset + 1 < totlen || (gtd->input_buf[offset+1] & 192) != 128)
					{
						return FALSE;
					}
					offset += 2;
					break;

				case 128+64+32:
					if (offset + 2 < totlen || (gtd->input_buf[offset+1] & 192) != 128 || (gtd->input_buf[offset+2] & 192) != 128)
					{
						return FALSE;
					}
					offset += 3;
					break;

				case 128+64+32+16:
					if (offset + 3 < totlen || (gtd->input_buf[offset+1] & 192) != 128 || (gtd->input_buf[offset+2] & 192) != 128 || (gtd->input_buf[offset+3] & 192) != 128)
					{
						return FALSE;
					}
					offset += 4;
					break;

				default:
					offset += 1;
					break;
			}
		}
	}
	return TRUE;
}

DO_CURSOR(cursor_backspace)
{
	if (gtd->input_cur == 0)
	{
		return;
	}

	cursor_left("");
	cursor_delete("");

	modified_input();
}


DO_CURSOR(cursor_buffer_down)
{
	do_buffer(gtd->ses, "DOWN");
}

DO_CURSOR(cursor_buffer_end)
{
	do_buffer(gtd->ses, "END");
}

DO_CURSOR(cursor_buffer_home)
{
	do_buffer(gtd->ses, "HOME");
}

DO_CURSOR(cursor_buffer_lock)
{
	do_buffer(gtd->ses, "LOCK");
}

DO_CURSOR(cursor_buffer_up)
{
	do_buffer(gtd->ses, "UP");
}


DO_CURSOR(cursor_check_line)
{
	if (gtd->input_pos - gtd->input_hid > inputline_max_str_len() - 3)
	{
		return cursor_redraw_line("");
	}

	if (gtd->input_hid && gtd->input_pos - gtd->input_hid < 2)
	{
		return cursor_redraw_line("");
	}
}

DO_CURSOR(cursor_check_line_modified)
{
	if (gtd->input_hid + inputline_max_str_len() < inputline_raw_str_len(0, gtd->input_len))
	{
		return cursor_redraw_line("");
	}

	return cursor_check_line("");
}

DO_CURSOR(cursor_clear_left)
{
	if (gtd->input_cur == 0)
	{
		return;
	}

	sprintf(gtd->paste_buf, "%.*s", gtd->input_cur, gtd->input_buf);

	input_printf("\033[%dG\033[%dP", gtd->input_off, gtd->input_pos - gtd->input_hid);

	memmove(&gtd->input_buf[0], &gtd->input_buf[gtd->input_cur], gtd->input_len - gtd->input_cur);

	gtd->input_len -= gtd->input_cur;

	gtd->input_buf[gtd->input_len] = 0;

	gtd->input_cur  = 0;
	gtd->input_pos  = 0;

	cursor_check_line_modified("");

	modified_input();
}

DO_CURSOR(cursor_clear_line)
{
	if (gtd->input_len == 0)
	{
		return;
	}

	input_printf("\033[%dG\033[%dP", gtd->input_off, inputline_cur_str_len());

	gtd->input_len = 0;
	gtd->input_cur = 0;
	gtd->input_hid = 0;
	gtd->input_pos = 0;
	gtd->input_buf[0] = 0;

	modified_input();
}

DO_CURSOR(cursor_clear_right)
{
	if (gtd->input_cur == gtd->input_len)
	{
		return;
	}

	strcpy(gtd->paste_buf, &gtd->input_buf[gtd->input_cur]);

	input_printf("\033[%dP", inputline_max_str_len() - inputline_str_str_len(gtd->input_hid, gtd->input_pos));

	gtd->input_buf[gtd->input_cur] = 0;

	gtd->input_len = gtd->input_cur;

	modified_input();
}

DO_CURSOR(cursor_convert_meta)
{
	SET_BIT(gtd->flags, TINTIN_FLAG_CONVERTMETACHAR);
}

DO_CURSOR(cursor_delete_or_exit)
{
	if (gtd->input_len == 0)
	{
		cursor_exit("");
	}
	else
	{
		cursor_delete("");
	}
}

DO_CURSOR(cursor_delete)
{
	if (gtd->input_len == 0)
	{
		return;
	}

	if (gtd->input_len == gtd->input_cur)
	{
		return;
	}

	if (HAS_BIT(gtd->ses->flags, SES_FLAG_BIG5) && (gtd->input_buf[gtd->input_cur] & 128) == 128)
	{
		if (gtd->input_cur + 1 < gtd->input_len && gtd->input_buf[gtd->input_cur+1])
		{
			memmove(&gtd->input_buf[gtd->input_cur+1], &gtd->input_buf[gtd->input_cur+2], gtd->input_len - gtd->input_cur);

			gtd->input_len--;

			input_printf("\033[1P");
		}
	}
	else if (HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8) && (gtd->input_buf[gtd->input_cur] & 192) == 192)
	{
		while (gtd->input_cur + 1 < gtd->input_len && (gtd->input_buf[gtd->input_cur+1] & 192) == 128)
		{
			memmove(&gtd->input_buf[gtd->input_cur+1], &gtd->input_buf[gtd->input_cur+2], gtd->input_len - gtd->input_cur);

			gtd->input_len--;
		}
	}

	memmove(&gtd->input_buf[gtd->input_cur], &gtd->input_buf[gtd->input_cur+1], gtd->input_len - gtd->input_cur);

	gtd->input_len--;

	if (gtd->input_hid + inputline_max_str_len() <= inputline_raw_str_len(0, gtd->input_len))
	{
		cursor_redraw_line("");
	}
	else
	{
		input_printf("\033[1P");
	}

	modified_input();
}

DO_CURSOR(cursor_delete_word_left)
{
	int index_cur;

	if (gtd->input_cur == 0)
	{
		return;
	}

	index_cur = gtd->input_cur;

	while (gtd->input_cur > 0 && gtd->input_buf[gtd->input_cur - 1] == ' ')
	{
		gtd->input_pos--;
		gtd->input_cur--;
	}

	while (gtd->input_cur > 0 && gtd->input_buf[gtd->input_cur - 1] != ' ')
	{
		if (!HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8) || (gtd->input_buf[gtd->input_cur] & 192) != 128)
		{
			gtd->input_pos--;
		}

		gtd->input_cur--;
	}

	sprintf(gtd->paste_buf, "%.*s", index_cur - gtd->input_cur, &gtd->input_buf[gtd->input_cur]);

	memmove(&gtd->input_buf[gtd->input_cur], &gtd->input_buf[index_cur], gtd->input_len - index_cur + 1);

//	input_printf("\033[%dD\033[%dP", index_cur - gtd->input_cur, index_cur - gtd->input_cur);

	gtd->input_len -= index_cur - gtd->input_cur;

	cursor_redraw_line("");

	modified_input();
}


DO_CURSOR(cursor_delete_word_right)
{
	int index_cur;

	if (gtd->input_cur == gtd->input_len)
	{
		return;
	}

	index_cur = gtd->input_cur;

	while (gtd->input_cur != gtd->input_len && gtd->input_buf[gtd->input_cur] == ' ')
	{
		gtd->input_cur++;
	}

	while (gtd->input_cur != gtd->input_len && gtd->input_buf[gtd->input_cur] != ' ')
	{
		gtd->input_cur++;
	}

	sprintf(gtd->paste_buf, "%.*s", gtd->input_cur - index_cur, &gtd->input_buf[gtd->input_cur]);

	memmove(&gtd->input_buf[index_cur], &gtd->input_buf[gtd->input_cur], gtd->input_len - gtd->input_cur + 1);

//	input_printf("\033[%dP", gtd->input_cur - index_cur);

	gtd->input_len -= gtd->input_cur - index_cur;

	gtd->input_cur = index_cur;

	cursor_redraw_line("");
	
	modified_input();
}

DO_CURSOR(cursor_echo_on)
{
	SET_BIT(gtd->ses->telopts, TELOPT_FLAG_ECHO);
}

DO_CURSOR(cursor_echo_off)
{
	DEL_BIT(gtd->ses->telopts, TELOPT_FLAG_ECHO);
}

DO_CURSOR(cursor_end)
{
	gtd->input_cur = gtd->input_len;

	gtd->input_pos = inputline_raw_str_len(0, gtd->input_len);

	cursor_redraw_line("");
}

DO_CURSOR(cursor_enter)
{
	input_printf("\n");

	gtd->input_buf[gtd->input_len] = 0;

	gtd->input_len    = 0;
	gtd->input_cur    = 0;
	gtd->input_pos    = 0;
	gtd->input_off    = 1;
	gtd->macro_buf[0] = 0;
	gtd->input_tmp[0] = 0;

	if (HAS_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH))
	{
		struct listroot *root = gtd->ses->list[LIST_HISTORY];

		if (root->update >= 0 && root->update < root->used)
		{
			strcpy(gtd->input_buf, root->list[root->update]->left);
		}

		DEL_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH);

		gtd->input_off = 1;
	}

	SET_BIT(gtd->flags, TINTIN_FLAG_PROCESSINPUT);

	modified_input();
}

DO_CURSOR(cursor_exit)
{
	if (gtd->ses == gts)
	{
		do_end(NULL, "");
	}
	else
	{
		cleanup_session(gtd->ses);
	}
}

DO_CURSOR(cursor_history_next)
{
	struct listroot *root = gtd->ses->list[LIST_HISTORY];

	if (HAS_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH))
	{
		if (root->update == root->used)
		{
			return;
		}

		for (root->update++ ; root->update < root->used ; root->update++)
		{
			if (*gtd->input_buf && find(gtd->ses, root->list[root->update]->left, gtd->input_buf, SUB_NONE))
			{
				break;
			}
		}

		if (root->update < root->used)
		{
			input_printf("\033[%dG  \033[0K%.*s\033[%dG", gtd->input_off + inputline_cur_str_len() + 2, gtd->input_off + inputline_max_str_len() - inputline_cur_str_len() - 4, root->list[root->update]->left, gtd->input_off + gtd->input_pos - gtd->input_hid);
		}
		return;
	}

	if (root->list[0] == NULL)
	{
		return;
	}

	if (!HAS_BIT(gtd->flags, TINTIN_FLAG_HISTORYBROWSE))
	{
		return;
	}
	else if (root->update < root->used)
	{
		for (root->update++ ; root->update < root->used ; root->update++)
		{
			if (!strncmp(gtd->input_tmp, root->list[root->update]->left, strlen(gtd->input_tmp)))
			{
				break;
			}
		}
	}

	cursor_clear_line("");

	if (root->update == root->used)
	{
		strcpy(gtd->input_buf, gtd->input_tmp);
	}
	else
	{
		strcpy(gtd->input_buf, root->list[root->update]->left);
	}

	gtd->input_len = strlen(gtd->input_buf);

	cursor_end("");

	SET_BIT(gtd->flags, TINTIN_FLAG_HISTORYBROWSE);
}

DO_CURSOR(cursor_history_prev)
{
	struct listroot *root = gtd->ses->list[LIST_HISTORY];

	if (HAS_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH))
	{
		if (root->update <= 0)
		{
			return;
		}

		for (root->update-- ; root->update >= 0 ; root->update--)
		{
			if (*gtd->input_buf && find(gtd->ses, root->list[root->update]->left, gtd->input_buf, SUB_NONE))
			{
				break;
			}
		}

		if (root->update >= 0)
		{
			input_printf("\033[%dG  \033[0K%.*s\033[%dG", gtd->input_off + inputline_cur_str_len() + 2, gtd->input_off + inputline_max_str_len() - inputline_cur_str_len() - 4, root->list[root->update]->left, gtd->input_off + gtd->input_pos - gtd->input_hid);
		}
		return;
	}

	if (root->list[0] == NULL)
	{
		return;
	}

	if (!HAS_BIT(gtd->flags, TINTIN_FLAG_HISTORYBROWSE))
	{
		strcpy(gtd->input_tmp, gtd->input_buf);

		for (root->update = root->used - 1 ; root->update >= 0 ; root->update--)
		{
			if (!strncmp(gtd->input_tmp, root->list[root->update]->left, strlen(gtd->input_tmp)))
			{
				break;
			}
		}
	}
	else if (root->update >= 0)
	{
		for (root->update-- ; root->update >= 0 ; root->update--)
		{
			if (!strncmp(gtd->input_tmp, root->list[root->update]->left, strlen(gtd->input_tmp)))
			{
				break;
			}
		}
	}

	cursor_clear_line("");

	if (root->update == -1)
	{
		strcpy(gtd->input_buf, gtd->input_tmp);
	}
	else
	{
		strcpy(gtd->input_buf, root->list[root->update]->left);
	}

	gtd->input_len = strlen(gtd->input_buf);

	cursor_end("");

	SET_BIT(gtd->flags, TINTIN_FLAG_HISTORYBROWSE);
}

DO_CURSOR(cursor_history_search)
{
	struct listroot *root = gtd->ses->list[LIST_HISTORY];

	if (root->list[0] == NULL)
	{
		return;
	}

	if (!HAS_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH))
	{
		strcpy(gtd->input_tmp, gtd->input_buf);

		cursor_clear_line("");

		SET_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH);

		gtd->input_off = 11;

		root->update = root->used - 1;

		input_printf("(search) [ ] \033[3D");
	}
	else
	{
		if (root->update >= 0 && root->update < root->used)
		{
			strcpy(gtd->input_buf, root->list[root->update]->left);
		}
		input_printf("\033[1G\033[0K");

		gtd->input_len = strlen(gtd->input_buf);
		gtd->input_cur = gtd->input_len;
		gtd->input_pos = 0;

		root->update = -1;

		DEL_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH);

		gtd->input_off = 1;

		cursor_redraw_line("");
	}
}

DO_CURSOR(cursor_history_find)
{
	struct listroot *root = gtd->ses->list[LIST_HISTORY];

	push_call("cursor_history_find(%s)", gtd->input_buf);

	if (HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8|SES_FLAG_BIG5))
	{
		if (inputline_str_chk(0, gtd->input_len) == FALSE)
		{
			pop_call();
			return;
		}
	}

	for (root->update = root->used - 1 ; root->update >= 0 ; root->update--)
	{
		if (*gtd->input_buf && find(gtd->ses, root->list[root->update]->left, gtd->input_buf, SUB_NONE))
		{
			break;
		}
	}

	if (root->update >= 0)
	{
		input_printf("\033[%dG ]  %.*s\033[%dG", gtd->input_off + inputline_cur_str_len(), gtd->input_off + inputline_max_str_len() - inputline_cur_str_len() - 4, root->list[root->update]->left, gtd->input_off + gtd->input_pos - gtd->input_hid);
	}
	else
	{
		input_printf("\033[%dG ] \033[0K\033[%dG", gtd->input_off + inputline_cur_str_len(), gtd->input_off + gtd->input_pos - gtd->input_hid);
	}
	pop_call();
	return;
}

DO_CURSOR(cursor_home)
{
	if (gtd->input_cur == 0)
	{
		return;
	}

	input_printf("\033[%dD", gtd->input_pos - gtd->input_hid);

	gtd->input_cur = 0;
	gtd->input_pos = 0;

	if (gtd->input_hid)
	{
		gtd->input_hid = 0;

		cursor_redraw_line("");
	}
}

DO_CURSOR(cursor_insert)
{
	if (*arg == 0)
	{
		TOG_BIT(gtd->flags, TINTIN_FLAG_INSERTINPUT);
	}
	else if (!strcasecmp(arg, "ON"))
	{
		SET_BIT(gtd->flags, TINTIN_FLAG_INSERTINPUT);
	}
	else if (!strcasecmp(arg, "OFF"))
	{
		DEL_BIT(gtd->flags, TINTIN_FLAG_INSERTINPUT);
	}
	else
	{
		tintin_printf(NULL, "#SYNTAX: #CURSOR {INSERT} {ON|OFF}.");
	}
}


DO_CURSOR(cursor_left)
{
	if (gtd->input_cur > 0)
	{
		gtd->input_cur--;
		gtd->input_pos--;

		if (HAS_BIT(gtd->ses->flags, SES_FLAG_BIG5) && inputline_str_chk(0, gtd->input_cur) == FALSE)
		{
			gtd->input_cur--;
			gtd->input_pos--;

			input_printf("\033[1D");
		}
		else if (HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8))
		{
			while (gtd->input_cur > 0 && (gtd->input_buf[gtd->input_cur] & 192) == 128)
			{
				gtd->input_cur--;
			}
		}

		input_printf("\033[1D");

		cursor_check_line("");
	}
}

DO_CURSOR(cursor_left_word)
{
//	int index_pos;

	if (gtd->input_cur == 0)
	{
		return;
	}

//	index_pos = gtd->input_pos;

	while (gtd->input_cur > 0 && gtd->input_buf[gtd->input_cur - 1] == ' ')
	{
		gtd->input_pos--;
		gtd->input_cur--;
	}

	while (gtd->input_cur > 0 && gtd->input_buf[gtd->input_cur - 1] != ' ')
	{
		if (!HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8) || (gtd->input_buf[gtd->input_cur] & 192) != 128)
		{
			gtd->input_pos--;
		}
		gtd->input_cur--;
	}

//	input_printf("\033[%dD", index_pos - gtd->input_pos);

	cursor_redraw_line("");
}


DO_CURSOR(cursor_paste_buffer)
{
	if (*gtd->paste_buf == 0)
	{
		return;
	}

	ins_sprintf(&gtd->input_buf[gtd->input_cur], "%s", gtd->paste_buf);

	gtd->input_len += strlen(gtd->paste_buf);
	gtd->input_cur += strlen(gtd->paste_buf);

	cursor_redraw_line("");

	modified_input();
}


DO_CURSOR(cursor_redraw_input)
{
	if (IS_SPLIT(gtd->ses))
	{
		cursor_redraw_line("");
	}
	else
	{
		input_printf("\033[1G\033[0K%s%s\033[0K", gtd->ses->more_output, gtd->input_buf);

		gtd->input_cur = gtd->input_len;

		gtd->input_pos = gtd->input_len % gtd->ses->cols;
	}
}


DO_CURSOR(cursor_redraw_line)
{
	if (HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8|SES_FLAG_BIG5))
	{
		if (inputline_str_chk(0, gtd->input_len) == FALSE)
		{
			return;
		}
	}

	// Erase current input

	input_printf("\033[%dG\033[%dP", gtd->input_off, inputline_max_str_len());

	// Center long lines of input

	if (gtd->input_pos > inputline_max_str_len() - 3)
	{
		while (gtd->input_pos - gtd->input_hid > inputline_max_str_len() - 3)
		{
			gtd->input_hid += inputline_max_str_len() / 2;
		}

		while (gtd->input_pos - gtd->input_hid < 2)
		{
			gtd->input_hid -= inputline_max_str_len() / 2;
		}
	}
	else
	{
		if (gtd->input_hid && gtd->input_pos - gtd->input_hid < 2)
		{
			gtd->input_hid = 0;
		}
	}

	// Print the entire thing

	if (gtd->input_hid)
	{
		if (gtd->input_hid + inputline_max_str_len() >= inputline_raw_str_len(0, gtd->input_len))
		{
			input_printf("<%.*s\033[%dG",  inputline_str_raw_len(gtd->input_hid + 1, gtd->input_hid + inputline_max_str_len() - 0), &gtd->input_buf[inputline_str_raw_len(0, gtd->input_hid + 1)], gtd->input_off + gtd->input_pos - gtd->input_hid);
		}
		else
		{
			input_printf("<%.*s>\033[%dG", inputline_str_raw_len(gtd->input_hid + 1, gtd->input_hid + inputline_max_str_len() - 1), &gtd->input_buf[inputline_str_raw_len(0, gtd->input_hid + 1)], gtd->input_off + gtd->input_pos - gtd->input_hid);
		}
	}
	else
	{
		if (gtd->input_hid + inputline_max_str_len() >= inputline_raw_str_len(0, gtd->input_len))
		{
			input_printf("%.*s\033[%dG",   inputline_str_raw_len(gtd->input_hid + 0, gtd->input_hid + inputline_max_str_len() - 0), &gtd->input_buf[inputline_str_raw_len(0, gtd->input_hid + 0)], gtd->input_off + gtd->input_pos - gtd->input_hid);
		}
		else
		{
			input_printf("%.*s>\033[%dG",  inputline_str_raw_len(gtd->input_hid + 0, gtd->input_hid + inputline_max_str_len() - 1), &gtd->input_buf[inputline_str_raw_len(0, gtd->input_hid + 0)], gtd->input_off + gtd->input_pos - gtd->input_hid);
		}
	}
}

DO_CURSOR(cursor_right)
{
	if (gtd->input_cur < gtd->input_len)
	{
		gtd->input_cur++;
		gtd->input_pos++;

		if (HAS_BIT(gtd->ses->flags, SES_FLAG_BIG5))
		{
			if (gtd->input_cur < gtd->input_len && inputline_str_chk(0, gtd->input_cur) == FALSE)
			{
				gtd->input_cur++;
				gtd->input_pos++;

				input_printf("\033[1C");
			}
		}
		else if (HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8))
		{
			while (gtd->input_cur < gtd->input_len && (gtd->input_buf[gtd->input_cur] & 192) == 128)
			{
				gtd->input_cur++;
			}
		}

		input_printf("\033[1C");
	}

	cursor_check_line("");
}

DO_CURSOR(cursor_right_word)
{
	int index_cur;

	if (gtd->input_cur == gtd->input_len)
	{
		return;
	}

	index_cur = gtd->input_cur;

	while (gtd->input_cur < gtd->input_len && gtd->input_buf[gtd->input_cur] == ' ')
	{
		gtd->input_cur++;
		gtd->input_pos++;
	}

	while (gtd->input_cur < gtd->input_len && gtd->input_buf[gtd->input_cur] != ' ')
	{
		if (!HAS_BIT(gtd->ses->flags, SES_FLAG_UTF8) || (gtd->input_buf[gtd->input_cur] & 192) != 128)
		{
			gtd->input_pos++;
		}
		gtd->input_cur++;
	}

	cursor_redraw_line("");
}

DO_CURSOR(cursor_suspend)
{
	do_suspend(gtd->ses, "");
}

DO_CURSOR(cursor_test)
{
	tintin_printf2(gtd->ses, "bar_len %d off %d pos %d hid %d cur %d len %d max %d tot %d", inputline_max_str_len(), gtd->input_off, gtd->input_pos, gtd->input_hid, gtd->input_cur, gtd->input_len, inputline_max_str_len(), inputline_cur_str_len());
}

/*
	Improved tab handling by Ben Love
*/

int cursor_tab_add(int input_now, int stop_after_first)
{
	struct listroot *root = gtd->ses->list[LIST_TAB];
	char tab[BUFFER_SIZE];

	for (root->update = 0 ; root->update < root->used ; root->update++)
	{
		substitute(gtd->ses, root->list[root->update]->left, tab, SUB_VAR|SUB_FUN);

		if (!strncmp(tab, &gtd->input_buf[input_now], strlen(&gtd->input_buf[input_now])))
		{
			if (search_node_list(gtd->ses->list[LIST_TABCYCLE], tab))
			{
				continue;
			}
			insert_node_list(gtd->ses->list[LIST_TABCYCLE], tab, "", "");

			if (stop_after_first)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}


int cursor_auto_tab_add(int input_now, int stop_after_first)
{
	char tab[BUFFER_SIZE], buf[BUFFER_SIZE];
	int scroll_cnt, line_cnt;
	char *arg;

	line_cnt = 0;

	scroll_cnt = gtd->ses->scroll_row;

	do
	{
		if (scroll_cnt == gtd->ses->scroll_max -1)
		{
			scroll_cnt = 0;
		}
		else
		{
			scroll_cnt++;
		}

		if (gtd->ses->buffer[scroll_cnt] == NULL)
		{
			break;
		}

		if (str_hash_grep(gtd->ses->buffer[scroll_cnt], FALSE))
		{
			continue;
		}

		if (line_cnt++ >= gtd->ses->auto_tab)
		{
			break;
		}

		strip_vt102_codes(gtd->ses->buffer[scroll_cnt], buf);

		arg = buf;

		while (*arg)
		{
			arg = get_arg_stop_spaces(gtd->ses, arg, tab, 0);

			if (!strncmp(tab, &gtd->input_buf[input_now], strlen(&gtd->input_buf[input_now])))
			{
				if (search_node_list(gtd->ses->list[LIST_TABCYCLE], tab))
				{
					continue;
				}
				insert_node_list(gtd->ses->list[LIST_TABCYCLE], tab, "", "");

				if (stop_after_first)
				{
					return TRUE;
				}
			}

			if (*arg == COMMAND_SEPARATOR)
			{
				arg++;
			}

		}
	}
	while (scroll_cnt != gtd->ses->scroll_row);

	return FALSE;
}

void cursor_hide_completion(int input_now)
{
	struct listroot *root = gtd->ses->list[LIST_TABCYCLE];
	struct listnode *f_node;
	struct listnode *l_node;
	int len_change;

	f_node = root->list[0];
	l_node = root->list[root->used - 1];

	if (root->used && !strcmp(l_node->left, gtd->input_buf + input_now))
	{
		len_change = strlen(l_node->left) - strlen(f_node->left);

		if (len_change > 0)
		{
/*
			if (gtd->input_cur < gtd->input_len)
			{
				input_printf("\033[%dC", gtd->input_len - gtd->input_cur);
			}
			input_printf("\033[%dD\033[%dP", len_change, len_change);
*/
			gtd->input_len = gtd->input_len - len_change;
			gtd->input_buf[gtd->input_len] = 0;
			gtd->input_cur = gtd->input_len;
			gtd->input_pos = gtd->input_pos;
		}
	}
	return;
}

void cursor_show_completion(int input_now, int show_last_node)
{
	struct listroot *root = gtd->ses->list[LIST_TABCYCLE];
	struct listnode *node;

	if (!root->used)
	{
		return;
	}

	node = show_last_node ? root->list[root->used - 1] : root->list[0];
/*
	if (gtd->input_cur < gtd->input_len)
	{
		input_printf("\033[%dC", gtd->input_len - gtd->input_cur);
	}
	if (gtd->input_len > input_now)
	{
		input_printf("\033[%dD\033[%dP", gtd->input_len - input_now, gtd->input_len - input_now);
	}
	if (input_now + (int) strlen(node->left) < gtd->ses->cols - 2)
	{
		input_printf("%s", node->left);
	}
*/
	strcpy(&gtd->input_buf[input_now], node->left);

	gtd->input_len = input_now + strlen(node->left);
	gtd->input_cur = gtd->input_len;
	gtd->input_pos = gtd->input_len;

	cursor_redraw_line("");

	if (HAS_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH))
	{
		cursor_history_find("");
	}

	if (node == root->list[0])
	{
		kill_list(gtd->ses->list[LIST_TABCYCLE]);
	}

	return;
}

int cursor_calc_input_now(void)
{
	int input_now;

	if (gtd->input_len == 0 || gtd->input_buf[gtd->input_len - 1] == ' ')
	{
		return -1;
	}

	for (input_now = gtd->input_len - 1 ; input_now ; input_now--)
	{
		if (gtd->input_buf[input_now] == ' ')
		{
			return input_now + 1;
		}
	}
	return input_now;
}

DO_CURSOR(cursor_tab_forward)
{
	int input_now, tab_found;

	input_now = cursor_calc_input_now();

	if (input_now == -1)
	{
		return;
	}

	cursor_hide_completion(input_now);

	if (!gtd->ses->list[LIST_TABCYCLE]->list[0])
	{
		insert_node_list(gtd->ses->list[LIST_TABCYCLE], &gtd->input_buf[input_now], "", "");
	}
	tab_found = cursor_tab_add(input_now, TRUE);

	cursor_show_completion(input_now, tab_found);
}

DO_CURSOR(cursor_tab_backward)
{
	struct listroot *root = gtd->ses->list[LIST_TABCYCLE];
	int input_now;

	input_now = cursor_calc_input_now();

	if (input_now == -1)
	{
		return;
	}

	cursor_hide_completion(input_now);

	if (root->used)
	{
		delete_index_list(gtd->ses->list[LIST_TABCYCLE], root->used - 1);
	}

	if (!root->list[0])
	{
		insert_node_list(gtd->ses->list[LIST_TABCYCLE], &gtd->input_buf[input_now], "", "");

		cursor_tab_add(input_now, FALSE);
	}
	cursor_show_completion(input_now, TRUE);
}

DO_CURSOR(cursor_auto_tab_forward)
{
	int input_now, tab_found;

	input_now = cursor_calc_input_now();

	if (input_now == -1)
	{
		return;
	}

	cursor_hide_completion(input_now);

	if (!gtd->ses->list[LIST_TABCYCLE]->list[0])
	{
		insert_node_list(gtd->ses->list[LIST_TABCYCLE], &gtd->input_buf[input_now], "", "");
	}
	tab_found = cursor_auto_tab_add(input_now, TRUE);

	cursor_show_completion(input_now, FALSE);
}

DO_CURSOR(cursor_auto_tab_backward)
{
	struct listroot *root = gtd->ses->list[LIST_TABCYCLE];
	int input_now;

	input_now = cursor_calc_input_now();

	if (input_now == -1)
	{
		return;
	}

	cursor_hide_completion(input_now);

	if (root->used)
	{
		delete_index_list(gtd->ses->list[LIST_TABCYCLE], root->used - 1);
	}

	if (!root->list[0])
	{
		insert_node_list(gtd->ses->list[LIST_TABCYCLE], &gtd->input_buf[input_now], "", "");

		cursor_auto_tab_add(input_now, FALSE);
	}

	cursor_show_completion(input_now, TRUE);
}


DO_CURSOR(cursor_mixed_tab_forward)
{
	int input_now, tab_found;

	input_now = cursor_calc_input_now();

	if (input_now == -1)
	{
		return;
	}

	cursor_hide_completion(input_now);

	if (!gtd->ses->list[LIST_TABCYCLE]->list[0])
	{
		insert_node_list(gtd->ses->list[LIST_TABCYCLE], &gtd->input_buf[input_now], "", "");
	}
	tab_found = cursor_tab_add(input_now, TRUE) || cursor_auto_tab_add(input_now, TRUE);

	cursor_show_completion(input_now, tab_found);
}

DO_CURSOR(cursor_mixed_tab_backward)
{
	struct listroot *root = gtd->ses->list[LIST_TABCYCLE];
	int input_now;

	input_now = cursor_calc_input_now();

	if (input_now == -1)
	{
		return;
	}

	cursor_hide_completion(input_now);

	if (root->used)
	{
		delete_index_list(gtd->ses->list[LIST_TABCYCLE], root->used - 1);
	}

	if (!root->list[0])
	{
		insert_node_list(gtd->ses->list[LIST_TABCYCLE], &gtd->input_buf[input_now], "", "");

		cursor_tab_add(input_now, FALSE);
		cursor_auto_tab_add(input_now, FALSE);
	}

	cursor_show_completion(input_now, TRUE);
}
