/******************************************************************************
*   TinTin++                                                                  *
*   Copyright (C) 2006 (See CREDITS file)                                     *
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
*                      coded by Igor van den Hoven 2006                       *
******************************************************************************/

#include "tintin.h"

void process_input(void)
{
	char line[BUFFER_SIZE];

	line[0] = 127;

	if (HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_SGA)
	&& !HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_ECHO))
	{
		read_key(line);
	}
	else
	{
		read_line(line);
	}

	if (*line == 127)
	{
		return;
	}

	if (gtd->chat && gtd->chat->paste_time)
	{
		chat_paste(line);

		return;
	}

	if (HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_ECHO))
	{
		add_line_history(gts, line);
	}

	if (HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_ECHO))
	{
		echo_command(gtd->ses, line, FALSE);
	}
	else
	{
		echo_command(gtd->ses, "", FALSE);		
	}

	if (gtd->ses->scroll_line != -1)
	{
		buffer_e("");
	}

	SET_BIT(gtd->flags, TINTIN_FLAG_USERCOMMAND);

	parse_input(gtd->ses, line);

	DEL_BIT(gtd->flags, TINTIN_FLAG_USERCOMMAND);

	if (IS_SPLIT(gtd->ses))
	{
		erase_toeol();
	}
}

void read_line(char *line)
{
	char buffer[BUFFER_SIZE];
	struct listnode *node;
	struct listroot *root;
	int len, cnt, match;

	len = read(0, buffer, 1);

	buffer[len] = 0;

	if (HAS_BIT(gtd->ses->flags, SES_FLAG_CONVERTMETA) || HAS_BIT(gtd->flags, TINTIN_FLAG_CONVERTMETACHAR))
	{
		convert_meta(buffer, &gtd->macro_buf[strlen(gtd->macro_buf)]);
	}
	else
	{
		strcat(gtd->macro_buf, buffer);
	}
	match = 0;
	root  = gtd->ses->list[LIST_MACRO];

	for (node = root->f_node ; node ; node = root->update)
	{
		root->update = node->next;

		if (!strcmp(gtd->macro_buf, node->pr))
		{
			parse_input(gtd->ses, node->right);
			gtd->macro_buf[0] = 0;

			return;
		}
		else if (!strncmp(gtd->macro_buf, node->pr, strlen(gtd->macro_buf)))
		{
			match = 1;
		}
	}

	for (cnt = 0 ; *cursor_table[cnt].code != 0 ; cnt++)
	{
		if (!strcmp(gtd->macro_buf, cursor_table[cnt].code))
		{
			cursor_table[cnt].fun("");
			gtd->macro_buf[0] = 0;

			return;
		}
		else if (!strncmp(gtd->macro_buf, cursor_table[cnt].code, strlen(gtd->macro_buf)))
		{
			match = 1;
		}
	}

	if (match)
	{
		return;
	}

	if (gtd->macro_buf[0] == ESCAPE)
	{
		strcpy(buffer, gtd->macro_buf);

		convert_meta(buffer, gtd->macro_buf);
	}

	for (cnt = 0 ; gtd->macro_buf[cnt] ; cnt++)
	{
		switch (gtd->macro_buf[cnt])
		{
			case 10:
				input_printf("\n");

				gtd->input_buf[gtd->input_len] = 0;

				strcpy(line, gtd->input_buf);

				gtd->input_len    = 0;
				gtd->input_cur    = 0;
				gtd->input_pos    = 0;
				gtd->input_buf[0] = 0;
				gtd->macro_buf[0] = 0;

				if (HAS_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH))
				{
					DEL_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH);

					if (gtd->input_his)
					{
						strcpy(line, gtd->input_his->left);
					}
				}
				gtd->input_his = NULL;
				break;

			default:
				if (HAS_BIT(gtd->flags, TINTIN_FLAG_INSERTINPUT) && gtd->input_len != gtd->input_cur)
				{
					gtd->input_buf[gtd->input_cur] = gtd->macro_buf[cnt];
					gtd->input_cur++;
					gtd->input_pos++;
					input_printf("%c", gtd->macro_buf[cnt]);
				}
				else
				{
					ins_sprintf(&gtd->input_buf[gtd->input_cur], "%c", gtd->macro_buf[cnt]);
					gtd->input_len++;
					gtd->input_cur++;
					gtd->input_pos++;
					input_printf("\033[1@%c", gtd->macro_buf[cnt]);
				}         

				gtd->macro_buf[0] = 0;
				gtd->input_buf[gtd->input_len] = 0;

				cursor_check_line("");

				if (HAS_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH))
				{
					cursor_history_find("");
				}
				break;
		}
	}
}

void read_key(char *line)
{
	char buffer[BUFFER_SIZE];
	struct listnode *node;
	struct listroot *root;
	int len, cnt, match;

	if (gtd->input_buf[0] == gtd->tintin_char)
	{
		read_line(line);

		return;
	}

	len = read(0, buffer, 1);

	buffer[len] = 0;

	if (HAS_BIT(gtd->ses->flags, SES_FLAG_CONVERTMETA) || HAS_BIT(gtd->flags, TINTIN_FLAG_CONVERTMETACHAR))
	{
		convert_meta(buffer, &gtd->macro_buf[strlen(gtd->macro_buf)]);
	}
	else
	{
		strcat(gtd->macro_buf, buffer);
	}

	match = 0;
	root  = gtd->ses->list[LIST_MACRO];

	for (node = root->f_node ; node ; node = root->update)
	{
		root->update = node->next;

		if (!strcmp(gtd->macro_buf, node->pr))
		{
			parse_input(gtd->ses, node->right);
			gtd->macro_buf[0] = 0;
			return;
		}
		else if (!strncmp(gtd->macro_buf, node->pr, strlen(gtd->macro_buf)))
		{
			match = 1;
		}
	}

	if (match)
	{
		return;
	}

	for (cnt = 0 ; gtd->macro_buf[cnt] ; cnt++)
	{
		switch (gtd->macro_buf[cnt])
		{
			case '\n':
			case '\r':
				gtd->input_buf[0] = 0;
				gtd->macro_buf[0] = 0;
				echo_command(gtd->ses, "", FALSE);
				socket_printf(gtd->ses, 2, "%c%c", '\r', '\0');
				break;

			default:
				if (gtd->macro_buf[cnt] == gtd->tintin_char && gtd->input_buf[0] == 0)
				{
					printf("\033[1@%c", gtd->macro_buf[cnt]);

					gtd->input_buf[0] = gtd->tintin_char;
					gtd->input_len = 1;
					gtd->input_cur = 1;
					gtd->input_pos = 1;
					gtd->macro_buf[0] = 0;
					gtd->input_buf[1] = 0;
				}
				else
				{
					socket_printf(gtd->ses, 1, "%c", gtd->macro_buf[cnt]);
					gtd->input_buf[0] = 127;
					gtd->macro_buf[0] = 0;
				}
				break;
		}
	}
}

void convert_meta(char *input, char *output)
{
	char *pti, *pto;

	DEL_BIT(gtd->flags, TINTIN_FLAG_CONVERTMETACHAR);

	pti = input;
	pto = output;

	while (*pti)
	{
		switch (*pti)
		{
			case ESCAPE:
				*pto++ = '\\';
				*pto++ = 'e';
				pti++;
				break;

			case 127:
				*pto++ = *pti++;
				break;

			case '\n':
			case '\r':
				*pto++ = *pti++;
				break;

			default:
				if (*pti > 0 && *pti < 30)
				{
					*pto++ = '\\';
					*pto++ = 'C';
					*pto++ = '-';
					*pto++ = 'a' + *pti - 1;
					pti++;
					break;
				}
				else
				{
					*pto++ = *pti++;
				}
				break;
		}
	}
	*pto = 0;
}

void unconvert_meta(char *input, char *output)
{
	char *pti, *pto;

	pti = input;
	pto = output;

	while (*pti)
	{
		switch (pti[0])
		{
			case '\\':
				switch (pti[1])
				{
					case 'e':
						*pto++  = ESCAPE;
						pti    += 2;
						break;

					case 'C':
						if (pti[2] == '-')
						{
							*pto++  = pti[3] - 'a' + 1;
							pti    += 4;
						}
						else
						{
							*pto++ = *pti++;
						}
						break;

					default:
						*pto++ = *pti++;
						break;
				}
				break;

			default:
				*pto++ = *pti++;
				break;
		}
	}
	*pto = 0;
}

void echo_command(struct session *ses, char *line, int newline)
{
	char buffer[BUFFER_SIZE];

	if (IS_SPLIT(ses) && ses->more_output[0] == 0)
	{
		sprintf(buffer, "\033[0m%s", line);
	}
	else
	{
		sprintf(buffer, "%s", line);
	}

	if (newline == TRUE)
	{
		if (ses->more_output[0] == 0)
		{
			return;
		}
		ses->check_output = 0;

		SET_BIT(ses->flags, SES_FLAG_SCROLLSTOP);

		tintin_printf2(ses, "%s", buffer);

		DEL_BIT(ses->flags, SES_FLAG_SCROLLSTOP);

		add_line_buffer(ses, buffer, FALSE);

		return;
	}

	if (IS_SPLIT(ses))
	{
		if (!HAS_BIT(ses->flags, SES_FLAG_ECHOCOMMAND) && ses->more_output[0] == 0)
		{
			return;
		}
		SET_BIT(ses->flags, SES_FLAG_SCROLLSTOP);

		if (ses->more_output[0])
		{
			ses->check_output = 0;

			tintin_printf2(ses, "%s%s", ses->more_output, buffer);
		}
		else
		{
			tintin_printf2(ses, "%s", buffer);
		}
		DEL_BIT(ses->flags, SES_FLAG_SCROLLSTOP);
	}
	add_line_buffer(ses, buffer, -1);
}

void input_printf(char *format, ...)
{
	char buf[BUFFER_SIZE];
	va_list args;

	if (!HAS_BIT(gtd->flags, TINTIN_FLAG_HISTORYSEARCH))
	{
		if (!HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_ECHO) && gtd->input_buf[0] != gtd->tintin_char)
		{
			return;
		}
	}

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	printf("%s", buf);
}
