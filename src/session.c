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
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                         coded by Peter Unold 1992                           *
******************************************************************************/

#include "tintin.h"

#include <errno.h>

/************************/
/* the #session command */
/************************/

DO_COMMAND(do_session)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	struct session *sesptr;
	int cnt;

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

	if (!*left)
	{
		tintin_puts("#THESE SESSIONS HAVE BEEN DEFINED:", ses);

		for (sesptr = gts->next ; sesptr ; sesptr = sesptr->next)
		{
			show_session(ses, sesptr);
		}
	}
	else if (*left && *right == 0)
	{
		if (*left == '+')
		{
			return activate_session(ses->next ? ses->next : gts->next ? gts->next : ses);
		}

		if (*left == '-')
		{
			return activate_session(ses->prev ? ses->prev : gts->prev ? gts->prev : ses);
		}

		if (is_number(left))
		{
			for (cnt = 0, sesptr = gts ; sesptr ; cnt++, sesptr = sesptr->next)
			{
				if (cnt == atoi(left))
				{
					return activate_session(sesptr);
				}
			}
		}

		tintin_puts("#THAT SESSION IS NOT DEFINED.", ses);
	}
	else
	{
		for (sesptr = gts->next ; sesptr ; sesptr = sesptr->next)
		{
			if (!strcmp(sesptr->name, left))
			{
				tintin_puts("#THERE'S A SESSION WITH THAT NAME ALREADY.", ses);
				return ses;
			}
		}
		ses = new_session(left, right, ses);
	}
	return gtd->ses;
}


/******************/
/* show a session */
/******************/

void show_session(struct session *ses, struct session *ptr)
{
	char temp[BUFFER_SIZE];

	sprintf(temp, "%-12s%-15s", ptr->name, ptr->host);

	if (ptr == gtd->ses)
	{
		strcat(temp, " (active)");
	}
	else
	{
		strcat(temp, "         ");
	}

	if (ptr->mccp)
	{
		strcat(temp, " (mccp)   ");
	}

	if (HAS_BIT(ptr->flags, SES_FLAG_SNOOP))
	{
		strcat(temp, " (snooped)");
	}

	if (ptr->logfile)
	{
		strcat(temp, " (logging)");
	}

	tintin_puts2(temp, ses);
}

/**********************************/
/* find a new session to activate */
/**********************************/

struct session *newactive_session(void)
{
	push_call("newactive_session(void)");

	if (gts->next)
	{
		activate_session(gts->next);
	}
	else
	{
		gtd->ses = gts;
		dirty_screen(gtd->ses);

		tintin_printf(NULL, "#THERE'S NO ACTIVE SESSION NOW.");
	}
	pop_call();
	return gtd->ses;
}

struct session *activate_session(struct session *ses)
{
	gtd->ses = ses;

	dirty_screen(ses);

	tintin_printf(ses, "#SESSION '%s' ACTIVATED.", ses->name);

	return ses;
}

/**********************/
/* open a new session */
/**********************/

struct session *new_session(char *name, char *address, struct session *ses)
{
	int cnt = 0;
	char host[BUFFER_SIZE], port[BUFFER_SIZE];
	struct session *newsession;

	if (name)
	{
		address = get_arg_in_braces(address, host, FALSE);
		address = get_arg_in_braces(address, port, FALSE);

		if (*host == 0)
		{
			tintin_puts("#HEY! SPECIFY AN ADDRESS WILL YOU?", ses);
			return(ses);
		}

		if (*port == 0)
		{
			tintin_puts("#HEY! SPECIFY A PORT NUMBER WILL YOU?", ses);
			return(ses);
		}
	}

	newsession                = calloc(1, sizeof(struct session));

	newsession->name          = strdup(name);
	newsession->class         = strdup(gts->class);
	newsession->flags         = gts->flags;
	newsession->telopts       = gts->telopts;
	newsession->host          = strdup(host);
	newsession->port          = strdup(port);

	gtd->ses                  = newsession;

	LINK(newsession, gts->next, gts->prev);

	for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
	{
		newsession->list[cnt] = copy_list(newsession, gts->list[cnt], cnt);
	}

	init_screen_size(newsession);

	if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
	{
		init_split(newsession, gts->top_row, gts->bot_row);
	}
	init_buffer(newsession, gts->scroll_max);

	dirty_screen(newsession);

	if (name)
	{
		connect_session(newsession);
	}
	else
	{
		newsession->socket = atoi(address);

		SET_BIT(newsession->flags, SES_FLAG_CONNECTED);

		gtd->ses = ses;
	}
	return gtd->ses;
}

void connect_session(struct session *ses)
{
	int sock;


	tintin_printf2(ses, "\n\r#Trying to connect to %s port %s.\n\r", ses->host, ses->port);

	ses->connect_retry = utime() + gts->connect_retry;


	reconnect:

	sock = connect_mud(ses->host, ses->port, ses);

	if (sock == -1)
	{
		cleanup_session(ses);

		return;
	}

	if (sock)
	{
		gtd->ses    = ses;
		ses->socket = sock;

		ses->connect_retry = 0;

		SET_BIT(ses->flags, SES_FLAG_CONNECTED);

		tintin_printf(ses, "#SESSION '%s' CONNECTED TO '%s' PORT '%s'\n\r", ses->name, ses->host, ses->port);

		if (atoi(ses->port) == TELNET_PORT)
		{
			init_telnet_session(ses);
		}

		check_all_events(ses, "SESSION CONNECTED");

		return;
	}

	if (ses->connect_retry > utime())
	{
		goto reconnect;
	}

	switch (ses->connect_error)
	{
		case EINTR:
			tintin_puts("#COULD NOT CONNECT - CONNECTION TIMED OUT.", ses);
			break;

		case ECONNREFUSED:
			tintin_puts("#COULD NOT CONNECT - CONNECTION REFUSED.", ses);
			break;

		case ENETUNREACH:
			tintin_puts("#COULD NOT CONNECT - THE NETWORK IS NOT REACHABLE FROM THIS HOST.", ses);
			break;

		default:
			tintin_puts("#COULD NOT CONNECT", ses);
			break;
	}

	cleanup_session(ses);
}

/*****************************************************************************/
/* cleanup after session died. if session=gtd->ses, try find new active      */
/*****************************************************************************/

void cleanup_session(struct session *ses)
{
	push_call("cleanup_session(%p)",ses);

	if (ses == gtd->update)
	{
		gtd->update = ses->next;
	}

	UNLINK(ses, gts->next, gts->prev);

	if (ses->socket)
	{
		if (close(ses->socket) == -1)
		{
			syserr("close in cleanup");
		}
	}

	if (ses == gtd->ses)
	{
		gtd->ses = newactive_session();
	}

	tintin_printf(gtd->ses, "");

	tintin_printf(gtd->ses, "#SESSION '%s' DIED.", ses->name);

	do_killall(ses, NULL);

	if (ses->map)
	{
		delete_map(ses);
	}

	if (ses->logfile)
	{
		fclose(ses->logfile);
	}

	if (ses->logline)
	{
		fclose(ses->logline);
	}

	pop_call();
	push_call("cleanup_session(%p) (free)", ses);

	init_buffer(ses, 0);

	free(ses->name);
	free(ses->host);
	free(ses->port);

	free(ses);

	pop_call();
	return;
}
