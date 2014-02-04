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
#include <signal.h>

/************************/
/* the #session command */
/************************/

DO_COMMAND(do_session)
{
	char temp[BUFFER_SIZE], left[BUFFER_SIZE];
	struct session *sesptr;
	int cnt;

	substitute(ses, arg, temp, SUB_VAR|SUB_FUN);

	arg = temp;

	arg = get_arg_in_braces(ses, arg, left,  FALSE);

	if (*left == 0)
	{
		tintin_puts(ses, "#THESE SESSIONS HAVE BEEN DEFINED:");

		for (sesptr = gts->next ; sesptr ; sesptr = sesptr->next)
		{
			show_session(ses, sesptr);
		}
	}
	else if (*left && *arg == 0)
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

		tintin_puts(ses, "#THAT SESSION IS NOT DEFINED.");
	}
	else
	{
		ses = new_session(ses, left, arg, 0);
	}
	return gtd->ses;
}


/******************/
/* show a session */
/******************/

void show_session(struct session *ses, struct session *ptr)
{
	char temp[BUFFER_SIZE];

	sprintf(temp, "%3d %-12s %20s:%-5s", ptr->socket, ptr->name, ptr->host, ptr->port);

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

	tintin_puts2(ses, temp);
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
		activate_session(gts);
	}
	pop_call();
	return gtd->ses;
}

struct session *activate_session(struct session *ses)
{
	check_all_events(gtd->ses, SUB_ARG|SUB_SEC, 0, 1, "SESSION DEACTIVATED", gtd->ses->name);

	gtd->ses = ses;

	dirty_screen(ses);

	tintin_printf(ses, "#SESSION '%s' ACTIVATED.", ses->name);

	check_all_events(ses, SUB_ARG|SUB_SEC, 0, 1, "SESSION ACTIVATED", ses->name);

	return ses;
}

/**********************/
/* open a new session */
/**********************/

struct session *new_session(struct session *ses, char *name, char *address, int desc)
{
	int cnt = 0;
	char host[BUFFER_SIZE], port[BUFFER_SIZE];
	struct session *newsession;

	push_call("new_session(%p,%p,%p,%d)",ses,name,address,desc);

	if (HAS_BIT(gtd->flags, TINTIN_FLAG_TERMINATE))
	{
		pop_call();
		return ses;
	}

	address = get_arg_in_braces(ses, address, host, FALSE);
	address = get_arg_in_braces(ses, address, port, FALSE);

	if (desc == 0)
	{
		if (*host == 0)
		{
			tintin_puts(ses, "#HEY! SPECIFY AN ADDRESS WILL YOU?");

			pop_call();
			return ses;
		}

		if (*port == 0)
		{
			tintin_puts(ses, "#HEY! SPECIFY A PORT NUMBER WILL YOU?");

			pop_call();
			return ses;
		}
	}

	for (newsession = gts ; newsession ; newsession = newsession->next)
	{
		if (!strcmp(newsession->name, name))
		{
			tintin_puts(ses, "THERE'S A SESSION WITH THAT NAME ALREADY.");

			pop_call();
			return ses;
		}
	}

	newsession                = (struct session *) calloc(1, sizeof(struct session));

	newsession->name          = strdup(name);
	newsession->host          = strdup(host);
	newsession->ip            = strdup("");
	newsession->port          = strdup(port);

	newsession->group         = strdup(gts->group);
	newsession->flags         = gts->flags;
	newsession->telopts       = gts->telopts;
	newsession->auto_tab      = gts->auto_tab;

	newsession->cmd_color     = strdup(gts->cmd_color);

	newsession->read_max      = gts->read_max;
	newsession->read_buf      = (unsigned char *) calloc(1, gts->read_max);

	LINK(newsession, gts->next, gts->prev);

	for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
	{
		newsession->list[cnt] = copy_list(newsession, gts->list[cnt], cnt);
	}

	newsession->rows          = gts->rows;
	newsession->cols          = gts->cols;
	newsession->top_row       = gts->top_row;
	newsession->bot_row       = gts->bot_row;

	init_buffer(newsession, gts->scroll_max);

	if (desc)
	{
		tintin_printf(ses, "#TRYING TO LAUNCH '%s' RUNNING '%s'.", newsession->name, newsession->host);
	}
	else
	{
		tintin_printf(ses, "#TRYING TO CONNECT '%s' TO '%s' PORT '%s'.", newsession->name, newsession->host, newsession->port);
	}

	gtd->ses = newsession;

	dirty_screen(newsession);

	if (desc == 0)
	{
		connect_session(newsession);
	}
	else
	{
		SET_BIT(newsession->flags, SES_FLAG_CONNECTED|SES_FLAG_RUN);

		SET_BIT(newsession->telopts, TELOPT_FLAG_SGA);
		DEL_BIT(newsession->telopts, TELOPT_FLAG_ECHO);

		gtd->ses = newsession;

		gtd->ses->socket = desc;
	}

	pop_call();
	return gtd->ses;
}

void connect_session(struct session *ses)
{
	int sock;

	ses->connect_retry = utime() + gts->connect_retry;

	reconnect:

	sock = connect_mud(ses, ses->host, ses->port);

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

		tintin_printf2(ses, "");

		tintin_printf(ses, "#SESSION '%s' CONNECTED TO '%s' PORT '%s'", ses->name, ses->host, ses->port);

		check_all_events(ses, SUB_ARG|SUB_SEC, 0, 4, "SESSION CONNECTED", ses->name, ses->host, ses->ip, ses->port);

		return;
	}

	if (ses->connect_retry > utime())
	{
		goto reconnect;
	}

	switch (ses->connect_error)
	{
		case EINTR:
			tintin_puts(ses, "#COULD NOT CONNECT - CALL INTERUPTED.");
			break;

		case ECONNREFUSED:
			tintin_puts(ses, "#COULD NOT CONNECT - REMOTE SERVER IS NOT REACHABLE.");
			break;

		case ENETUNREACH:
			tintin_puts(ses, "#COULD NOT CONNECT - THE NETWORK IS NOT REACHABLE FROM THIS HOST.");
			break;

		case ETIMEDOUT:
			tintin_puts(ses, "#COULD NOT CONNECT - CONNECTION TIMED OUT.");
			break;

		case EINPROGRESS:
			tintin_puts(ses, "#COULD NOT CONNECT - CONNECTION TIMED OUT.");
			break;

		default:
			tintin_puts(ses, "#COULD NOT CONNECT.");
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

		// the PID is stored in the session's port.

		if (HAS_BIT(ses->flags, SES_FLAG_RUN))
		{
			kill(atoi(ses->port), SIGKILL);
		}

	}

	check_all_events(ses, SUB_ARG|SUB_SEC, 0, 4, "SESSION DISCONNECTED", ses->name, ses->host, ses->ip, ses->port);

	tintin_printf(gtd->ses, "#SESSION '%s' DIED.", ses->name);

	if (ses == gtd->ses)
	{
		gtd->ses = newactive_session();
	}

	if (ses->logfile)
	{
		fclose(ses->logfile);
	}

	if (ses->logline)
	{
		fclose(ses->logline);
	}

	LINK(ses, gtd->dispose_next, gtd->dispose_prev);

	pop_call();
	return;
}

void dispose_session(struct session *ses)
{
	int index;

	push_call("dispose_session(%p)", ses);

	UNLINK(ses, gtd->dispose_next, gtd->dispose_prev);

	for (index = 0 ; index < LIST_MAX ; index++)
	{
		free_list(ses->list[index]);
	}

	if (ses->map)
	{
		delete_map(ses);
	}

	if (ses->mccp)
	{
		inflateEnd(ses->mccp);
		free(ses->mccp);
	}

	init_buffer(ses, 0);

	free(ses->name);
	free(ses->host);
	free(ses->ip);
	free(ses->port);
	free(ses->group);
	free(ses->read_buf);
	free(ses->cmd_color);

	free(ses);

	pop_call();
	return;
}
