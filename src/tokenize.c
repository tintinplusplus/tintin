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
*                     coded by Igor van den Hoven 2008                        *
******************************************************************************/

#include "tintin.h"

int debug_level;

struct scripttoken
{
	struct scripttoken   * next;
	struct scripttoken   * prev;
	char                 * str;
	int                    lvl;
	int                    opr;
	int                    cmd;
};

void debugtoken(struct session *ses, struct scripttoken *token)
{
	if (debug_level)
	{
		switch (token->opr)
		{
			case TOKEN_OPR_STRING:
			case TOKEN_OPR_SESSION:
				tintin_printf2(ses, "[%02d] %*s%s", token->opr, token->lvl * 2, "", token->str);
				break;
			case TOKEN_OPR_ELSE:
			case TOKEN_OPR_ENDIF:
				tintin_printf2(ses, "[%02d] %*s\033[1;32m%s\033[0m", token->opr, token->lvl * 2, "", token->str);
				break;
			case TOKEN_OPR_RETURN:
			case TOKEN_OPR_CMD:
				tintin_printf2(ses, "[%02d] %*s\033[1;32m%s\033[0m %s", token->opr, token->lvl * 2, "", command_table[token->cmd].name, token->str);
				break;
			case TOKEN_OPR_IF:
				tintin_printf2(ses, "[%02d] %*s\033[1;32m%s {\033[0m%s\033[1;32m}", token->opr, token->lvl * 2, "", command_table[token->cmd].name, token->str);
				break;
		}
	}
}


void addtoken(struct scripttoken *root, int lvl, int opr, int cmd, char *str)
{
	struct scripttoken *token;

	token = calloc(1, sizeof(struct scripttoken));

	token->lvl = lvl;
	token->opr = opr;
	token->cmd = cmd;
	token->str = strdup(str);

	LINK(token, root->next, root->prev);
}


void deltoken(struct scripttoken *root, struct scripttoken *token)
{
	UNLINK(token, root->next, root->prev);

	free(token->str);
	free(token);
}


int find_command(char *command)
{
	int cmd;

	if (isalpha(*command))
	{
		for (cmd = gtd->command_ref[tolower(*command) - 'a'] ; *command_table[cmd].name ; cmd++)
		{
			if (is_abbrev(command, command_table[cmd].name))
			{
				return cmd;
			}
		}
	}
	return -1;
}


void tokenize_script(struct scripttoken *root, int lvl, char *str)
{
	char *arg, *line;
	int cmd;

	if (*str == 0)
	{
		addtoken(root, lvl, TOKEN_OPR_STRING, -1, "");

		return;
	}

	line = calloc(1, BUFFER_SIZE);

	while (*str)
	{
		if (*str != gtd->tintin_char)
		{
			str = get_arg_all(str, line);

			addtoken(root, lvl, TOKEN_OPR_STRING, -1, line);
		}
		else
		{
			arg = get_arg_stop_spaces(str, line);

			cmd = find_command(line+1);

			if (cmd == -1)
			{
				str = get_arg_all(str, line);
				addtoken(root, lvl, TOKEN_OPR_SESSION, -1, line+1);
			}
			else
			{
				switch (command_table[cmd].type)
				{
					case TOKEN_OPR_IF:
						str = get_arg_in_braces(arg, line, FALSE);
						addtoken(root, lvl++, TOKEN_OPR_IF, cmd, line);

						str = get_arg_in_braces(str, line, TRUE);
						tokenize_script(root, lvl--, line);

						if (*str && *str != COMMAND_SEPARATOR)
						{
							str = get_arg_in_braces(str, line, TRUE);
							addtoken(root, lvl++, TOKEN_OPR_ELSE, -1, "else");

							tokenize_script(root, lvl--, line);
						}
						addtoken(root, lvl, TOKEN_OPR_ENDIF, -1, "endif");
						break;

					case TOKEN_OPR_RETURN:
						str = get_arg_all(arg, line);
						addtoken(root, lvl, TOKEN_OPR_RETURN, cmd, line);
						break;

					default:
						str = get_arg_all(arg, line);
						addtoken(root, lvl, TOKEN_OPR_CMD, cmd, line);
						break;
				}
			}
		}
		if (*str == ';')
		{
			str++;
		}

	}

	free(line);
}


struct scripttoken *parse_script(struct session *ses, int lvl, struct scripttoken *token)
{
	while (token)
	{
		if (token->lvl < lvl)
		{
			return token;
		}

		debugtoken(ses, token);

		switch (token->opr)
		{
			case TOKEN_OPR_STRING:
				ses = parse_input(ses, token->str);
				break;

			case TOKEN_OPR_CMD:
				ses = (*command_table[token->cmd].command) (ses, token->str);
				break;

			case TOKEN_OPR_IF:
				if (mathexp(ses, token->str))
				{
					token = token->next;

					if (token)
					{
						token = parse_script(ses, lvl + 1, token);
					}

					if (token && token->opr == TOKEN_OPR_ELSE)
					{
						token = token->next;

						while (token && lvl < token->lvl)
						{
							token = token->next;
						}
					}
				}
				else
				{
					token = token->next;

					while (token && token->lvl > lvl)
					{
						token = token->next;
					}
				}
				continue;

			case TOKEN_OPR_ELSE:
				token = token->next;

				if (token)
				{
					token = parse_script(ses, lvl + 1, token);
				}
				continue;

			case TOKEN_OPR_ENDIF:
				break;

			case TOKEN_OPR_RETURN:
				ses = (*command_table[token->cmd].command) (ses, token->str);

				if (lvl)
				{
					return NULL;
				}
				else
				{
					token = NULL;
				}
				break;

			case TOKEN_OPR_SESSION:
				ses = parse_tintin_command(ses, token->str);
				break;
		}

		if (token)
		{
			token = token->next;
		}
	}
	return (struct scripttoken *) ses;
}


char *write_script(struct session *ses, struct scripttoken *root)
{
	struct scripttoken *token;
	static char buf[STRING_SIZE];

	token = root->next;

	buf[0] = 0;

	while (token)
	{
		printf("(%3d) (%2d) (%2d) %s\n", token->lvl, token->opr, token->cmd, token->str);

		switch (token->opr)
		{
			case TOKEN_OPR_STRING:
				cat_sprintf(buf, "%s%s", indent(token->lvl), token->str);
				break;

			case TOKEN_OPR_CMD:
			case TOKEN_OPR_RETURN:
				cat_sprintf(buf, "%s%c%s%s%s", indent(token->lvl), gtd->tintin_char, command_table[token->cmd].name, *token->str ? " " : "", token->str);
				break;

			case TOKEN_OPR_IF:
				cat_sprintf(buf, "%s%cif {%s}\n%s{\n", indent(token->lvl), gtd->tintin_char, token->str, indent(token->lvl));
				break;

			case TOKEN_OPR_ELSE:
				cat_sprintf(buf, "\n%s}\n%s{\n", indent(token->lvl), indent(token->lvl));
				break;

			case TOKEN_OPR_ENDIF:
				cat_sprintf(buf, "\n%s}", indent(token->lvl));
				break;
			case TOKEN_OPR_SESSION:
				cat_sprintf(buf, "%s%c%s", indent(token->lvl), gtd->tintin_char, token->str);
				break;
		}

		if (token->next && token->lvl == token->next->lvl)
		{
			strcat(buf, ";\n");
		}

		token = token->next;
	}

	while (root->next)
	{
		deltoken(root, root->next);
	}

	free(root);

	return buf;
}

struct session *script_driver(struct session *ses, int list, char *str)
{
	struct scripttoken *root;
	int level;

	root = calloc(1, sizeof(struct scripttoken));

	tokenize_script(root, 0, str);

	level = list != -1 ? HAS_BIT(ses->list[list]->flags, LIST_FLAG_DEBUG) : 0;

	debug_level += level;

	ses = (struct session *) parse_script(ses, 0, root->next);

	debug_level -= level;

	while (root->prev)
	{
		deltoken(root, root->prev);
	}

	free(root);

	return ses;
}


char *script_writer(struct session *ses, char *str)
{
	struct scripttoken *root;

	root = calloc(1, sizeof(struct scripttoken));

	tokenize_script(root, 1, str);

	free(root);

	return write_script(ses, root);
}
