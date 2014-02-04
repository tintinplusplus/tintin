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
*   file: input.c - funtions related to keyboard input                        *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                     coded by Igor van den Hoven 2006                        *
******************************************************************************/


#include "tintin.h"

void commandloop(void)
{
	char *line, buffer[BUFFER_SIZE];

	while (TRUE)
	{
		if (HAS_BIT(gts->flags, SES_FLAG_PREPPED))
		{
			line = readkeyboard();
		}
		else
		{
			line = readline(NULL);
		}

		if (line == NULL)
		{
			continue;
		}

		if (gtd->chat && gtd->chat->paste_time)
		{
			chat_paste(line);

			free(line);

			continue;
		}

		if (HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_ECHO))
		{
			if ((line = rlhist_expand(line)) == NULL)
			{
				continue;
			}
		}

		sprintf(buffer, "%s", line);

		if (!HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_ECHO))
		{
			memset(buffer, '*', strlen(line));
		}

		echo_command(gtd->ses, buffer, 0);

		SET_BIT(gts->flags, SES_FLAG_USERCOMMAND);

		if (gtd->ses->scroll_line != -1)
		{
			buffer_e();
		}

		parse_input(line, gtd->ses);

		DEL_BIT(gts->flags, SES_FLAG_USERCOMMAND);

		if (IS_SPLIT(gtd->ses))
		{
			erase_toeol();
		}
		free(line);

		fflush(stdout);
	}
}

char * readkeyboard(void)
{
	char ch, buf[10], *line;

	while (TRUE)
	{
		ch = rl_read_key();

		if (HAS_BIT(gtd->ses->flags, SES_FLAG_CONVERTMETA))
		{
			switch (ch)
			{
				case ESCAPE:
					rl_insert_text("\\e");
					break;			 

				case '\n':
				case '\r':
					printf("\n");
					line = strdup(rl_line_buffer);
					rl_point = 0;
					rl_delete_text(0, rl_end);
					rl_redisplay();
					return line;

				case 127:
					if (rl_point)
					{
						rl_point--;
						rl_delete_text(rl_point, rl_point+1);
					}
					break;

				default:
					if (ch < 27)
					{
						sprintf(buf, "\\C-%c", 'a' + ch - 1);
					}								 
					else
					{
						sprintf(buf, "%c", ch);
					}
					rl_insert_text(&buf[0]);
					break;
			}
			rl_redisplay();
		}
		else
		{
			switch (ch)
			{
				case '\n':
				case '\r':
					line = rl_line_buffer[0] == gtd->tintin_char ? strdup(rl_line_buffer) : NULL;

					rl_point = 0;
					rl_delete_text(0, rl_end);

					if (line == NULL)
					{
						echo_command(gtd->ses, "", FALSE);
						socket_printf(gtd->ses, 2, "%c%c", '\r', '\0');
					}
					else
					{
						echo_command(gtd->ses, "", TRUE);
						printf("\n");
						rl_redisplay();
						return line;
					}
					break;

				case 127:
					if (rl_point)
					{
						rl_point--;
						rl_delete_text(rl_point, rl_point+1);
					}
					if (rl_line_buffer[0] != gtd->tintin_char)
					{
						socket_printf(gtd->ses, 1, "%c", ch);
					}
					break;

				default:
					if (ch < 27)
					{
						sprintf(buf, "\\C-%c", 'a' + ch - 1);
					}								 
					else
					{
						sprintf(buf, "%c", ch);
					}
					rl_insert_text(&buf[0]);

					if (rl_line_buffer[0] != gtd->tintin_char)
					{
						socket_printf(gtd->ses, 1, "%c", ch);
					}
					else
					{
						printf("%s", buf);
					}
					break;
			}
		}
	}
	return NULL;
}

void echo_command(struct session *ses, char *line, int newline)
{
	char buffer[BUFFER_SIZE];

	if (*line)
	{
		sprintf(buffer, "\033[0m%s", line);
	}
	else
	{
		buffer[0] = 0;
	}

	if (newline)
	{
		if (gtd->ses->more_output[0] == 0)
		{
			return;
		}
		gtd->ses->check_output = 0;

		SET_BIT(gtd->ses->flags, SES_FLAG_SCROLLSTOP);

		if (HAS_BIT(gtd->ses->flags, SES_FLAG_SPLIT))
		{
			tintin_printf2(gtd->ses, "%s%s", gtd->ses->more_output, buffer);
		}
		else
		{
			tintin_printf2(gtd->ses, "%s", buffer);
		}

		DEL_BIT(gtd->ses->flags, SES_FLAG_SCROLLSTOP);

		add_line_buffer(gtd->ses, buffer, FALSE);

		return;
	}

	if (HAS_BIT(gtd->ses->flags, SES_FLAG_SPLIT))
	{
		if (!HAS_BIT(gtd->ses->flags, SES_FLAG_ECHOCOMMAND) || gtd->ses->more_output[0] == 0)
		{
			return;
		}
		SET_BIT(gtd->ses->flags, SES_FLAG_SCROLLSTOP);

		if (gtd->ses->more_output[0])
		{
			gtd->ses->check_output = 0;

			tintin_printf2(gtd->ses, "%s%s", gtd->ses->more_output, buffer);
		}
		else
		{
			tintin_printf2(gtd->ses, "%s", buffer);
		}
		DEL_BIT(gtd->ses->flags, SES_FLAG_SCROLLSTOP);
	}
	add_line_buffer(gtd->ses, buffer, -1);
}

