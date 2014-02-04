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
*******************************************************************************/

/*********************************************************************/
/* file: session.c.c - funtions related to sessions                  */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

#include "tintin.h"

#include <errno.h>

/************************/
/* the #session command */
/************************/

DO_COMMAND(do_session)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	struct session *sesptr;

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
		for (sesptr = gts->next ; sesptr; sesptr = sesptr->next)
		{
			if (!strcmp(sesptr->name, left))
			{
				show_session(ses, sesptr);
				break;
			}
		}
		if (!sesptr)
		{
			tintin_puts("#THAT SESSION IS NOT DEFINED.", ses);
		}
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
	if (gts->next)
	{
		gtd->ses = gts->next;
		tintin_printf(gtd->ses, "#SESSION '%s' ACTIVATED.", gts->next->name);
	}
	else
	{
		gtd->ses = gts;
		tintin_puts("#THERE'S NO ACTIVE SESSION NOW.", NULL);
	}
	dirty_screen(gtd->ses);

	return gtd->ses;
}

/**********************/
/* open a new session */
/**********************/

struct session *new_session(const char *name, const char *address, struct session *ses)
{
	int cnt = 0;
	char host[BUFFER_SIZE], port[BUFFER_SIZE];
	struct session *newsession;

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

	newsession                = calloc(1, sizeof(struct session));

	newsession->name          = strdup(name);
	newsession->flags         = gts->flags;
	newsession->host          = strdup(host);
	newsession->port          = strdup(port);
	newsession->map_size      = gts->map_size;
	newsession->scroll_line   = -1;

	gtd->ses                  = newsession;

	LINK(newsession, gts->next, gts->prev, next, prev);

	for (cnt = 0 ; cnt < LIST_ALL ; cnt++)
	{
		copy_list(newsession, gts->list[cnt], cnt);
	}

	init_screen_size(newsession);

	init_buffer(newsession, gts->scroll_max);

	connect_session(newsession);

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

		SET_BIT(ses->flags, SES_FLAG_CONNECTED);

		if (IS_SPLIT(gts))
		{
			init_split(ses, gts->top_row, gts->bot_row);
		}

		tintin_printf2(ses, "#SESSION '%s' CONNECTED TO '%s' PORT '%s'\n\r", ses->name, ses->host, ses->port);

		if (atoi(ses->port) == TELNET_PORT)
		{
			init_telnet_session(ses);
		}
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

	UNLINK(ses, gts->next, gts->prev, next, prev);

	if (ses->socket)
	{
		if (close(ses->socket) == -1)
		{
			syserr("close in cleanup");
		}
	}

	tintin_printf(ses, "\n\r#SESSION '%s' DIED.", ses->name);

	if (ses == gtd->ses)
	{
		gtd->ses = newactive_session();
	}

	do_killall(ses, NULL);

	if (ses->logfile)
	{
		fclose(ses->logfile);
	}

	if (ses->logline)
	{
		fclose(ses->logline);
	}


	free(ses->name);
	free(ses->host);
	free(ses->port);
	init_buffer(ses, 2);
	free(ses->buffer);

	free(ses);

	pop_call();
	return;
}
