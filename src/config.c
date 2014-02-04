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
*   file: config.c - funtions related tintin++ configuration                  *
*              (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                   *
*                     coded by Igor van den Hoven 2004                        *
******************************************************************************/


#include "tintin.h"


DO_COMMAND(do_configure)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int cnt;

	root = ses->list[LIST_CONFIG];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, FALSE);

	if (*left == 0)
	{
		tintin_header(ses, " CONFIGURATIONS ");

		node = root->f_node;

		for (node = root->f_node ; node ; node = node->next)
		{
			tintin_printf2(ses, "  [%-13s] [%6s] %s", 
				node->left,
				node->right,
				strcmp(node->right, "ON") == 0 ? config_table[atoi(node->pr)].msg_on : config_table[atoi(node->pr)].msg_off);
		}

		tintin_header(ses, "");
	}
	else
	{
		for (cnt = 0 ; *config_table[cnt].name != 0 ; cnt++)
		{
			if (is_abbrev(left, config_table[cnt].name))
			{
				if (config_table[cnt].config(ses, right, cnt) != NULL)
				{
					show_message(ses, LIST_CONFIG, "#CONFIG {%s} HAS BEEN SET TO {%s}.", config_table[cnt].name, capitalize(right));
				}
				return ses;
			}
		}
		tintin_printf(ses, "#ERROR: #CONFIG {%s} IS NOT A VALID OPTION.", capitalize(left));
	}
	return ses;
}


DO_CONFIG(config_speedwalk)
{
	char buf[NUMBER_SIZE];

	if (!strcasecmp(arg, "ON"))
	{
		SET_BIT(ses->flags, SES_FLAG_SPEEDWALK);
	}
	else if (!strcasecmp(arg, "OFF"))
	{
		DEL_BIT(ses->flags, SES_FLAG_SPEEDWALK);
	}
	else
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {%s} <ON|OFF>", config_table[index].name);

		return NULL;
	}
	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}


DO_CONFIG(config_verbatim)
{
	char buf[NUMBER_SIZE];

	if (!strcasecmp(arg, "ON"))
	{
		SET_BIT(ses->flags, SES_FLAG_VERBATIM);
	}
	else if (!strcasecmp(arg, "OFF"))
	{
		DEL_BIT(ses->flags, SES_FLAG_VERBATIM);
	}
	else
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {%s} <ON|OFF>", config_table[index].name);

		return NULL;
	}
	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_repeatenter)
{
	char buf[NUMBER_SIZE];

	if (!strcasecmp(arg, "ON"))
	{
		SET_BIT(ses->flags, SES_FLAG_REPEATENTER);
	}
	else if (!strcasecmp(arg, "OFF"))
	{
		DEL_BIT(ses->flags, SES_FLAG_REPEATENTER);
	}
	else
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {%s} <ON|OFF>", config_table[index].name);

		return NULL;
	}
	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_echocommand)
{
	char buf[NUMBER_SIZE];

	if (!strcasecmp(arg, "ON"))
	{
		SET_BIT(ses->flags, SES_FLAG_ECHOCOMMAND);
	}
	else if (!strcasecmp(arg, "OFF"))
	{
		DEL_BIT(ses->flags, SES_FLAG_ECHOCOMMAND);
	}
	else
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {%s} <ON|OFF>", config_table[index].name);

		return NULL;
	}
	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_verbose)
{
	char buf[NUMBER_SIZE];

	if (!strcasecmp(arg, "ON"))
	{
		SET_BIT(ses->flags, SES_FLAG_VERBOSE);
	}
	else if (!strcasecmp(arg, "OFF"))
	{
		DEL_BIT(ses->flags, SES_FLAG_VERBOSE);
	}
	else
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {%s} <ON|OFF>", config_table[index].name);

		return NULL;
	}
	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_wordwrap)
{
	char buf[NUMBER_SIZE];

	if (!strcasecmp(arg, "ON"))
	{
		SET_BIT(ses->flags, SES_FLAG_WORDWRAP);
	}
	else if (!strcasecmp(arg, "OFF"))
	{
		DEL_BIT(ses->flags, SES_FLAG_WORDWRAP);
	}
	else
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {%s} <ON|OFF>", config_table[index].name);

		return NULL;
	}
	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_log)
{
	char buf[NUMBER_SIZE];

	if (!strcasecmp(arg, "HTML"))
	{
		DEL_BIT(ses->flags, SES_FLAG_LOGPLAIN);
		SET_BIT(ses->flags, SES_FLAG_LOGHTML);
	}
	else if (!strcasecmp(arg, "PLAIN"))
	{
		SET_BIT(ses->flags, SES_FLAG_LOGPLAIN);
		DEL_BIT(ses->flags, SES_FLAG_LOGHTML);
	}
	else if (!strcasecmp(arg, "RAW"))
	{
		DEL_BIT(ses->flags, SES_FLAG_LOGPLAIN);
		DEL_BIT(ses->flags, SES_FLAG_LOGHTML);
	}
	else
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG LOG <HTML|PLAIN|RAW>");

		return NULL;
	}
	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_buffersize)
{
	char buf[NUMBER_SIZE];

	if (!is_number(arg))
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {BUFFER SIZE} <NUMBER>");

		return NULL;
	}

	if (atoi(arg) < 100 || atoi(arg) > 999999)
	{
		tintin_printf(ses, "#ERROR: #CONFIG BUFFER: PROVIDE A NUMBER BETWEEN 100 and 999999");

		return NULL;
	}

	init_buffer(ses, atoi(arg));

	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_scrolllock)
{
	char buf[NUMBER_SIZE];

	if (!strcasecmp(arg, "ON"))
	{
		SET_BIT(ses->flags, SES_FLAG_SCROLLLOCK);
	}
	else if (!strcasecmp(arg, "OFF"))
	{
		DEL_BIT(ses->flags, SES_FLAG_SCROLLLOCK);
	}
	else
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {%s} <ON|OFF>", config_table[index].name);

		return NULL;
	}
	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_connectretry)
{
	char buf[NUMBER_SIZE];

	if (!is_number(arg))
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {CONNECT RETRY} <NUMBER>");

		return NULL;
	}

	gts->connect_retry = atoll(arg) * 1000000LL;

	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}


DO_CONFIG(config_packetpatch)
{
	char buf[NUMBER_SIZE];

	if (!is_number(arg))
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {PACKET PATCH} <NUMBER>");
	}

	if (atoi(arg) < 0 || atoi(arg) > 10000)
	{
		tintin_printf(ses, "#ERROR: #CONFIG PACKET PATCH: PROVIDE A NUMBER BETWEEN 0 and 10000");

		return NULL;
	}

	gts->check_output = atoi(arg) * 1000;

	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}


DO_CONFIG(config_historysize)
{
	char buf[NUMBER_SIZE];

	if (!is_number(arg))
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {HISTORY SIZE} <NUMBER>");
	}

	if (atoi(arg) < 0 || atoi(arg) > 9999)
	{
		tintin_printf(ses, "#ERROR: #CONFIG HISTORY: PROVIDE A NUMBER BETWEEN 0 and 9999");

		return NULL;
	}

	gtd->history_size = atoi(arg);

/*	stifle_history(gtd->history_size); */

	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_tintinchar)
{
	char buf[NUMBER_SIZE];

	gtd->tintin_char = arg[0];

	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, arg, buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_verbatimchar)
{
	char buf[NUMBER_SIZE];

	gtd->verbatim_char = arg[0];

	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, arg, buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_repeatchar)
{
	char buf[NUMBER_SIZE];

	gtd->repeat_char = arg[0];

	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, arg, buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_debugtelnet)
{
	char buf[NUMBER_SIZE];

	if (!strcasecmp(arg, "ON"))
	{
		SET_BIT(ses->telopts, TELOPT_FLAG_DEBUG);
	}
	else if (!strcasecmp(arg, "OFF"))
	{
		DEL_BIT(ses->telopts, TELOPT_FLAG_DEBUG);
	}
	else
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {%s} <ON|OFF>", config_table[index].name);

		return NULL;
	}
	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_convertmeta)
{
	char buf[NUMBER_SIZE];

	if (!strcasecmp(arg, "ON"))
	{
		SET_BIT(ses->flags, SES_FLAG_CONVERTMETA);
	}
	else if (!strcasecmp(arg, "OFF"))
	{
		DEL_BIT(ses->flags, SES_FLAG_CONVERTMETA);
	}
	else
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {%s} <ON|OFF>", config_table[index].name);

		return NULL;
	}
	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}

DO_CONFIG(config_loglevel)
{
	char buf[NUMBER_SIZE];

	if (!strcasecmp(arg, "LOW"))
	{
		SET_BIT(ses->flags, SES_FLAG_LOGLEVEL);
	}
	else if (!strcasecmp(arg, "HIGH"))
	{
		DEL_BIT(ses->flags, SES_FLAG_LOGLEVEL);
	}
	else
	{
		tintin_printf(ses, "#SYNTAX: #CONFIG {%s} <LOW|HIGH>", config_table[index].name);

		return NULL;
	}
	sprintf(buf, "%d", index);

	updatenode_list(ses, config_table[index].name, capitalize(arg), buf, LIST_CONFIG);

	return ses;
}
