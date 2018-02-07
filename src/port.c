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
******************************************************************************/

/******************************************************************************
*               (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                  *
*                                                                             *
*                     coded by Igor van den Hoven 2017                        *
******************************************************************************/

#include "tintin.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#ifdef HAVE_PTHREAD_H
	#include <pthread.h>
#endif


#define CALL_TIMEOUT 5

DO_COMMAND(do_port)
{
	char cmd[BUFFER_SIZE], left[BUFFER_SIZE], right[BUFFER_SIZE];
	int cnt;

	arg = get_arg_in_braces(ses, arg, cmd, FALSE);

	if (*cmd == 0)
	{
		tintin_header(ses, " PORT COMMANDS ");

		for (cnt = 0 ; *port_table[cnt].name != 0 ; cnt++)
		{
			tintin_printf2(ses, "  [%-13s] %s", port_table[cnt].name, port_table[cnt].desc);
		}
		tintin_header(ses, "");

		return ses;
	}

	for (cnt = 0 ; *port_table[cnt].name != 0 ; cnt++)
	{
		if (!is_abbrev(cmd, port_table[cnt].name))
		{
			continue;
		}

		if (port_table[cnt].fun != port_initialize && ses->port == NULL)
		{
			tintin_printf(ses, "#PORT: You must initialize a port first.");

			return ses;
		}

		arg = get_arg_in_braces(ses, arg, left,  port_table[cnt].lval);
		substitute(ses, left, left, SUB_VAR|SUB_FUN);

		arg = get_arg_in_braces(ses, arg, right, port_table[cnt].rval);
		substitute(ses, right, right, SUB_VAR|SUB_FUN);

		ses = port_table[cnt].fun(ses, left, right, arg);
		
		return ses;
	}

	do_port(ses, "");

	return ses;
}


/*
	Get ready to receive connections.
*/

DO_PORT(port_initialize)
{
	char hostname[BUFFER_SIZE], temp[BUFFER_SIZE], file[BUFFER_SIZE];
	struct sockaddr_in sa;
	struct hostent *hp = NULL;
	struct linger ld;
	char *reuse = "1";
	int sock, port;

	arg = sub_arg_in_braces(ses, arg, file,  GET_ONE, SUB_VAR|SUB_FUN);

	if (*left == 0 || *right == 0)
	{
		return show_error(ses, LIST_COMMAND, "#SYNTAX: #PORT INITIALIZE {NAME} {PORT} {FILE}");
	}

	if (find_session(left))
	{
		tintin_printf(ses, "#PORT INITIALIZE: THERE'S A SESSION NAMED {%s} ALREADY.", left);

		return ses;
	}

	if (!is_number(right))
	{
		tintin_printf(ses, "#PORT INITIALIZE: {%s} IS NOT A VALID PORT NUMBER.", right);

		return ses;
	}

	tintin_printf(ses, "#TRYING TO LAUNCH '%s' ON PORT '%s'.", left, right);

	sprintf(temp, "{localport} {%d} {%s}", atoi(right), file);

	port = atoi(right);

	gethostname(hostname, BUFFER_SIZE);

	hp = gethostbyname(hostname);

	if (hp == NULL)
	{
		perror("port_initialize: gethostbyname");

		return ses;
	}

	sa.sin_family      = hp->h_addrtype;
	sa.sin_port        = htons(port);
	sa.sin_addr.s_addr = 0;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0)
	{
		perror("port_initialize: socket");

		return ses;
	}

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reuse, sizeof(reuse));

	ld.l_onoff  = 0; 
	ld.l_linger = 100;

	setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld));

	/*
		Might make things unstable
	*/

	if (fcntl(sock, F_SETFL, O_NDELAY|O_NONBLOCK) == -1)
	{
		perror("port_initialize: fcntl O_NDELAY|O_NONBLOCK");
	}

	if (bind(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0)
	{
		tintin_printf(NULL, "#PORT INITIALIZE: PORT %d IS ALREADY IN USE.", port);

		close(sock);

		return ses;
	}

	if (listen(sock, 50) == -1)
	{
		perror("port_initialize: listen:");

		close(sock);

		return ses;
	}

	// kind of dodgy, but should be a mere formality.

	ses = new_session(ses, left, temp, -1, 0);

	ses->port = (struct port_data *) calloc(1, sizeof(struct port_data));

	ses->port->fd       = sock;
	ses->port->port     = port;

	ses->port->name     = strdup(left);
	ses->port->color    = strdup("\033[0;1;36m");
	ses->port->ip       = strdup("<Unknown>");
	ses->port->prefix   = strdup("<PORT>");

	tintin_printf(ses, "#PORT INITIALIZE: SESSION {%s} IS LISTENING ON PORT %d.", ses->name, ses->port->port);

	return ses;
}


DO_PORT(port_uninitialize)
{
	int port = ses->port->port;

	while (ses->port->next)
	{
		close_port(ses, ses->port->next, TRUE);
	}
	close_port(ses, ses->port, FALSE);

	ses->port = NULL;

	tintin_printf(ses, "#PORT UNINITIALIZE: CLOSED PORT {%d}.", port);

	return ses;
}


/*
	Accept an incoming port connection.
*/

int port_new(struct session *ses, int s)
{
	struct port_data *new_buddy;
	struct sockaddr_in sock;
	socklen_t i;
	int fd;

	i = sizeof(sock);

	getsockname(s, (struct sockaddr *) &sock, &i);

	if ((fd = accept(s, (struct sockaddr *) &sock, &i)) < 0)
	{
		perror("port_new: accept");

		return -1;
	}

	if (fcntl(fd, F_SETFL, O_NDELAY|O_NONBLOCK) == -1)
	{
		perror("port_new: fcntl O_NDELAY|O_NONBLOCK");
	}

	if (HAS_BIT(ses->port->flags, CHAT_FLAG_DND))
	{
		close(fd);

		return -1;
	}

	new_buddy = (struct port_data *) calloc(1, sizeof(struct port_data));

	new_buddy->fd       = fd;

	new_buddy->name     = strdup(ntos(fd));
	new_buddy->group    = strdup("");
	new_buddy->ip       = strdup(inet_ntoa(sock.sin_addr));

	new_buddy->port     = 0;

	LINK(new_buddy, ses->port->next, ses->port->prev);

	port_printf(ses, "New connection: %s D%d.", new_buddy->ip, new_buddy->fd);

	check_all_events(ses, SUB_ARG|SUB_SEC, 0, 3, "PORT CONNECTION", new_buddy->name, new_buddy->ip, ntos(new_buddy->port));

	return 0;
}


/*
	Places a call to another port client.
*/

#ifdef HAVE_GETADDRINFO

void *threaded_port_call(void *arg)
{
	struct session *ses;
	int sock, error;
	char host[BUFFER_SIZE], port[BUFFER_SIZE], name[BUFFER_SIZE];
	struct addrinfo *address;
	static struct addrinfo hints;
	struct port_data *new_buddy;
	struct timeval to;
	fd_set wds, rds;

	to.tv_sec = CALL_TIMEOUT;
	to.tv_usec = 0;

	arg = (void *) get_arg_in_braces(gtd->ses, (char *) arg, name, FALSE);
	arg = (void *) get_arg_in_braces(gtd->ses, (char *) arg, host, FALSE);
	arg = (void *) get_arg_in_braces(gtd->ses, (char *) arg, port, FALSE);

	ses = find_session(name);

	if (ses == NULL)
	{
		perror("threaded_port_call: null session");
		
		return NULL;
	}

	port_printf(ses, "Attempting to call %s ...", arg);

	hints.ai_family   = AF_UNSPEC;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;

	error = getaddrinfo(host, port, &hints, &address);

	switch (error)
	{
		case 0:
			break;

		case -2:
			port_printf(ses, "Failed to call %s, unknown host.", host);
			return NULL;

		default:
			port_printf(ses, "Failed to call %s.", host);
			return NULL;
	}

	if ((sock = socket(address->ai_family, address->ai_socktype, address->ai_protocol)) < 0)
	{
		syserr("socket");
	}

	switch (address->ai_family)
	{
		case AF_INET:
			inet_ntop(address->ai_family, &((struct sockaddr_in *)address->ai_addr)->sin_addr, host, address->ai_addrlen);
			break;

		case AF_INET6:
			inet_ntop(address->ai_family, &((struct sockaddr_in6 *)address->ai_addr)->sin6_addr, host, address->ai_addrlen);
			break;
	}

	if (connect(sock, address->ai_addr, address->ai_addrlen) != 0)
	{
		port_printf(ses, "Failed to connect to %s:%s", host, port);

		close(sock);
		freeaddrinfo(address);

		return NULL;
	}

	FD_ZERO(&wds);

	FD_SET(sock, &wds); /* mother port desc */

	if (select(FD_SETSIZE, NULL, &wds, NULL, &to) == -1)
	{
		port_printf(ses, "Failed to connect to %s %s", host, port);

		close(sock);
		freeaddrinfo(address);

		return NULL;
	}

	if (!FD_ISSET(sock, &wds))
	{
		port_printf(ses, "Connection timed out.");

		close(sock);
		freeaddrinfo(address);

		return NULL;
	}

	new_buddy = calloc(1, sizeof(struct port_data));

	new_buddy->fd       = sock;
	new_buddy->port     = atoi(port);

	new_buddy->group    = strdup("");
	new_buddy->ip       = strdup(host);
	new_buddy->name     = str_dup_printf("%d", sock);

	check_all_events(ses, SUB_ARG|SUB_SEC, 0, 3, "PORT CONNECTION", new_buddy->name, new_buddy->ip, ntos(new_buddy->port));

	FD_ZERO(&rds);
	FD_SET(sock, &rds);

	to.tv_sec  = CALL_TIMEOUT;
	to.tv_usec = 0;

	if (select(FD_SETSIZE, &rds, NULL, NULL, &to) == -1)
	{
		close_port(ses, new_buddy, FALSE);
		freeaddrinfo(address);

		return NULL;
	}

	if (process_port_input(ses, new_buddy) == -1)
	{
		FD_CLR(new_buddy->fd, &rds);
		close_port(ses, new_buddy, FALSE);
		freeaddrinfo(address);

		return NULL;
	}

	if (*new_buddy->name == 0)
	{
		close_port(ses, new_buddy, FALSE);
	}
	else
	{
		if (fcntl(sock, F_SETFL, O_NDELAY|O_NONBLOCK) == -1)
		{
			perror("port_new: fcntl O_NDELAY|O_NONBLOCK");
		}

		LINK(new_buddy, ses->port->next, ses->port->prev);
		port_printf(ses, "Connection made to %s.", new_buddy->name);
	}
	freeaddrinfo(address);

	return NULL;
}

#else

void *threaded_port_call(void *arg)
{
	struct session *ses;
	int sock, dig;
	char host[BUFFER_SIZE], port[BUFFER_SIZE], name[BUFFER_SIZE];
	struct sockaddr_in dest_addr;
	struct port_data *new_buddy;
	struct timeval to;
	fd_set wds, rds;

	to.tv_sec = CALL_TIMEOUT;
	to.tv_usec = 0;

	arg = (void *) get_arg_in_braces(ses, (char *) arg, name, FALSE);
	arg = (void *) get_arg_in_braces(ses, (char *) arg, host, FALSE);
	arg = (void *) get_arg_in_braces(ses, (char *) arg, port, FALSE);

	ses = find_session(name);

	if (ses == NULL)
	{
		perror("threaded_port_call: null session");
		
		return NULL;
	}

	port_printf(ses, "Attempting to call %s ...", arg);

	if (sscanf(host, "%d.%d.%d.%d", &dig, &dig, &dig, &dig) == 4)
	{
		dest_addr.sin_addr.s_addr = inet_addr(host);
	}
	else
	{
		struct hostent *hp;
		int addr, address[4];

		if ((hp = gethostbyname(host)) == NULL)
		{
			port_printf(ses, "Failed to call %s, unknown host.", host);

			return NULL;
		}
		memcpy((char *)&dest_addr.sin_addr, hp->h_addr, sizeof(dest_addr.sin_addr));

		addr = ntohl(dest_addr.sin_addr.s_addr);

		address[0] = ( addr >> 24 ) & 0xFF ; 
		address[1] = ( addr >> 16 ) & 0xFF ;
		address[2] = ( addr >>  8 ) & 0xFF ;
		address[3] = ( addr       ) & 0xFF ;

		sprintf(host, "%d.%d.%d.%d", address[0], address[1], address[2], address[3]);
	}

	if (is_number(port))
	{
		dest_addr.sin_port = htons(atoi(port));
	}
	else
	{
		port_printf(ses, "The port should be a number.");

		return NULL;
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		syserr("socket");
	}

	dest_addr.sin_family = AF_INET;

	if (connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) != 0)
	{
		port_printf(ses, "Failed to connect to %s:%s", host, port);

		close(sock);

		return NULL;
	}

	FD_ZERO(&wds);

	FD_SET(sock, &wds); /* mother port desc */

	if (select(FD_SETSIZE, NULL, &wds, NULL, &to) == -1)
	{
		port_printf(ses, "Failed to connect to %s %s", host, port);

		close(sock);

		return NULL;
	}

	if (!FD_ISSET(sock, &wds))
	{
		port_printf(ses, "Connection timed out.");

		close(sock);

		return NULL;
	}

	new_buddy = (struct port_data *) calloc(1, sizeof(struct port_data));

	new_buddy->fd       = sock;
	new_buddy->port     = atoi(port);

	new_buddy->group    = strdup("");
	new_buddy->ip       = strdup(host);
	new_buddy->name     = str_dup_printf("%d", sock);

	check_all_events(ses, SUB_ARG|SUB_SEC, 0, 3, "PORT CONNECTION", new_buddy->name, new_buddy->ip, ntos(new_buddy->port));

	FD_ZERO(&rds);
	FD_SET(sock, &rds);

	to.tv_sec  = CALL_TIMEOUT;
	to.tv_usec = 0;

	if (select(FD_SETSIZE, &rds, NULL, NULL, &to) == -1)
	{
		close_port(ses, new_buddy, FALSE);

		return NULL;
	}

	if (process_port_input(new_buddy) == -1)
	{
		FD_CLR(new_buddy->fd, &rds);
		close_port(ses, new_buddy, FALSE);

		return NULL;
	}

	if (*new_buddy->name == 0)
	{
		close_port(ses, new_buddy, FALSE);
	}
	else
	{
		if (fcntl(sock, F_SETFL, O_NDELAY|O_NONBLOCK) == -1)
		{
			perror("port_new: fcntl O_NDELAY|O_NONBLOCK");
		}

		LINK(new_buddy, ses->port->next, ses->port->prev);
		port_printf(ses, "Connection made to %s.", new_buddy->name);
	}

	return NULL;
}

#endif

#ifdef HAVE_LIBPTHREAD

DO_PORT(port_call)
{
	char buf[BUFFER_SIZE];

	pthread_t thread;

	sprintf(buf, "{%s} {%s} {%s}", ses->name, left, right);

	pthread_create(&thread, NULL, threaded_port_call, (void *) buf);

	return ses;
}
	
#else

DO_PORT(port_call)
{
	char buf[BUFFER_SIZE];

	sprintf(buf, "{%s} {%s}", left, right);

	threaded_port_call((void *) buf);
}

#endif

/*
	Clean up and close a port conneciton.
*/

void close_port(struct session *ses, struct port_data *buddy, int unlink)
{
	buddy->flags = 0;

	if (unlink)
	{
		UNLINK(buddy, ses->port->next, ses->port->prev);
	}

	if (buddy != ses->port)
	{
		if (*buddy->name == 0)
		{
			port_printf(ses, "Closing connection to %s D%d", buddy->ip, buddy->fd);
		}
		else
		{
			port_printf(ses, "Closing connection to %s@%s.", buddy->name, buddy->ip);
		}
	}

	check_all_events(ses, SUB_ARG|SUB_SEC, 0, 3, "PORT DISCONNECTION", buddy->name, buddy->ip, ntos(buddy->port));

	close(buddy->fd);

	free(buddy->group);
	free(buddy->ip);
	free(buddy->name);

	free(buddy);
}


/*
	Check for incoming calls and poll for input on 
	                              connected sockets.
*/

void process_port_connections(struct session *ses, fd_set *read_set, fd_set *write_set, fd_set *exc_set)
{
	struct port_data *buddy, *buddy_next;

	push_call("process_port_connections(%p,%p,%p)",read_set,write_set,exc_set);

	/*
		accept incoming connections
	*/

	if (FD_ISSET(ses->port->fd, read_set))
	{
		port_new(ses, ses->port->fd);
	}

	/*
		read from sockets
	*/

	for (buddy = ses->port->next ; buddy ; buddy = buddy_next)
	{
		buddy_next = buddy->next;

		if (HAS_BIT(buddy->flags, CHAT_FLAG_LINKLOST) || FD_ISSET(buddy->fd, exc_set))
		{
			FD_CLR(buddy->fd, write_set);
			FD_CLR(buddy->fd, read_set);

			close_port(ses, buddy, TRUE);
		}
		else if (FD_ISSET(buddy->fd, read_set) && process_port_input(ses, buddy) < 0)
		{
			FD_CLR(buddy->fd, write_set);
			FD_CLR(buddy->fd, read_set);

			close_port(ses, buddy, TRUE);
		}
	}
	pop_call();
	return;
}

void port_socket_printf(struct session *ses, struct port_data *buddy, char *format, ...)
{
	char buf[BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsnprintf(buf, BUFFER_SIZE / 3, format, args);
	va_end(args);

	if (!HAS_BIT(buddy->flags, CHAT_FLAG_LINKLOST))
	{
		if (write(buddy->fd, buf, strlen(buf)) < 0)
		{
			port_printf(ses, "Lost link to socket '%s'.", buddy->name);

			SET_BIT(buddy->flags, CHAT_FLAG_LINKLOST);
		}
	}
}

void port_printf(struct session *ses, char *format, ...)
{
	char buf[STRING_SIZE], tmp[STRING_SIZE];
	va_list args;

	va_start(args, format);
	vsnprintf(buf, BUFFER_SIZE / 3, format, args);
	va_end(args);

	sprintf(tmp, "%s%s", ses->port->prefix, buf);

	strip_vt102_codes_non_graph(tmp, buf);

//	check_all_events(ses, SUB_ARG|SUB_SEC, 0, 2, "PORT MESSAGE", tmp, buf);

	sprintf(tmp, "%s%s%s", ses->port->color, buf, "\033[0m");

	tintin_puts(ses, tmp);
}

/*
	Read and store for parsing all input on a socket.
	Then call get_port_commands() to parse out the commands.
*/

int process_port_input(struct session *ses, struct port_data *buddy)
{
	char buf[BUFFER_SIZE];
	int size;

	push_call("process_port_input(%p)",buddy);

	size = read(buddy->fd, buf, BUFFER_SIZE / 3);

	if (size <= 0)
	{
		pop_call();
		return -1;
	}

	buf[size] = 0;

	get_port_commands(ses, buddy, buf, size);

	pop_call();
	return 0;
}

/*
	Parse the input queue for commands and execute the
	apropriate function.
*/

void get_port_commands(struct session *ses, struct port_data *buddy, char *buf, int len)
{
	char txt[STRING_SIZE];

	push_call("get_port_commands(%s,%d,%s)",buddy->name,len,buf);

	port_receive_message(ses, buddy, buf);

	strip_vt102_codes(buf, txt);

	check_all_events(ses, SUB_ARG|SUB_SEC, 0, 4, "PORT MESSAGE", buddy->name, buddy->ip, txt, buf);

	pop_call();
	return;
}

/**************************************************************************
 *  CHAT COMMUNICATION ROUTINES                                           *
 *************************************************************************/


void port_receive_message(struct session *ses, struct port_data *buddy, char *txt)
{
	if (HAS_BIT(buddy->flags, CHAT_FLAG_IGNORE))
	{
		return;
	}

	port_printf(ses, "%s", txt);
}


DO_PORT(port_info)
{
	tintin_printf2(NULL, "Port                 : %d", ses->port->port);
	tintin_printf2(NULL, "Prefix               : %s", ses->port->prefix);
	tintin_printf2(NULL, "DND                  : %s", HAS_BIT(ses->port->flags, CHAT_FLAG_DND) ? "On" : "Off");

	return ses;
}


DO_PORT(port_name)
{
	struct port_data *buddy;

	buddy = find_port_buddy(ses, left);

	substitute(ses, right, right, SUB_COL|SUB_ESC);

	if (buddy == NULL)
	{
		port_printf(ses, "There is no socket named '%s'.", left);

		return ses;
	}

	RESTRING(buddy->name, right);

	port_printf(ses, "Name of socket '%s' changed to '%s'.", left, buddy->name);

	return ses;
}


DO_PORT(port_prefix)
{
	RESTRING(ses->port->prefix, left);

	port_printf(ses, "Prefix set to '%s'", ses->port->prefix);

	return ses;
}

DO_PORT(port_send)
{
	struct port_data *buddy;

	substitute(gtd->ses, right, right, SUB_COL|SUB_ESC|SUB_EOL);

	if (!strcasecmp(left, "ALL"))
	{
		for (buddy = ses->port->next ; buddy ; buddy = buddy->next)
		{
			port_socket_printf(ses, buddy, "%s", right);
		}
	}
	else
	{
		if ((buddy = find_port_buddy(ses, left)) != NULL)
		{
			port_socket_printf(ses, buddy, "%s", right);
		}
		else if (find_port_group(ses, left) != NULL)
		{
			for (buddy = ses->port->next ; buddy ; buddy = buddy->next)
			{
				if (!strcmp(buddy->group, left))
				{
					port_socket_printf(ses, buddy, "%s", right);
				}
			}
		}
		else
		{
			port_printf(ses, "There is no socket named '%s'.", left);
		}
	}

	return ses;
}

DO_PORT(port_who)
{
	struct port_data *buddy;
	int cnt = 1;

	tintin_printf(ses, "     %-15s  %-5s  %-20s  %-5s  ", "Name", "Flags", "Address", "Port");
	tintin_printf(ses, "     ===============  =====  ====================  =====  ");

	for (buddy = ses->port->next ; buddy ; buddy = buddy->next)
	{
		tintin_printf(ses, "here goes");
		tintin_printf(ses, " %03d %+15s  %s%s%s%s%s  %+20s  %+5u",
			cnt++,
			buddy->name,
			HAS_BIT(buddy->flags, CHAT_FLAG_PRIVATE)   ? "P" : " ",
			HAS_BIT(buddy->flags, CHAT_FLAG_IGNORE)    ? "I" : " ",
			HAS_BIT(buddy->flags, CHAT_FLAG_SERVE)     ? "S" : " ",
			HAS_BIT(buddy->flags, CHAT_FLAG_FORWARD)   ? "F" :
			HAS_BIT(buddy->flags, CHAT_FLAG_FORWARDBY) ? "f" : " ",
			" ",
			buddy->ip,
			buddy->port);
	}
	tintin_printf(ses, "     ===============  =====  ====================  ===== ");

	return ses;
}


DO_PORT(port_zap)
{
	struct port_data *buddy;

	if (!strcasecmp(left, "ALL"))
	{
		while (ses->port->next)
		{
			close_port(ses, ses->port->next, TRUE);
		}
	}
	else
	{
		if ((buddy = find_port_buddy(ses, left)))
		{
			close_port(ses, buddy, TRUE);
		}
		else
		{
			port_printf(ses, "There is no socket named '%s'.", left);
		}
	}

	return ses;
}


DO_PORT(port_color)
{
	if (*left == 0 || get_highlight_codes(gtd->ses, left, right) == FALSE)
	{
		port_printf(ses, "Valid colors are:\n\nreset, bold, dim, light, dark, underscore, blink, reverse, black, red, green, yellow, blue, magenta, cyan, white, b black, b red, b green, b yellow, b blue, b magenta, b cyan, b white");

		return ses;
	}
	RESTRING(ses->port->color, right);

	port_printf(ses, "Color has been set to %s", left);

	return ses;
}

DO_PORT(port_dnd)
{
	TOG_BIT(ses->port->flags, CHAT_FLAG_DND);

	if (HAS_BIT(ses->port->flags, CHAT_FLAG_DND))
	{
		port_printf(ses, "New connections are no longer accepted.");
	}
	else
	{
		port_printf(ses, "New connections are accepted.");
	}
	
	return ses;
}

DO_PORT(port_group)
{
	struct port_data *buddy;
	int cnt = 0;

	if (*left == 0)
	{
		tintin_printf(NULL, "     %-15s  %-20s  %-5s  %-15s", "Name", "Address", "Port", "Group");
		tintin_printf(NULL, "     ===============  ====================  =====  ==================== ");

		for (buddy = ses->port->next ; buddy ; buddy = buddy->next)
		{
			tintin_printf(NULL, " %03d %-15s  %-20s  %-5u  %-20s",
				cnt++,
				buddy->name,
				buddy->ip,
				buddy->port,
				buddy->group);
		}
		tintin_printf(NULL, "     ===============  ====================  =====  ==================== ");
	}
	else if (!strcasecmp(left, "ALL"))
	{
		port_printf(ses, "You set everyone's group to '%s'", right);

		for (buddy = ses->port->next ; buddy ; buddy = buddy->next)
		{
			RESTRING(buddy->group, right);
		}
	}
	else
	{
		if ((buddy = find_port_buddy(ses, left)) != NULL)
		{
			RESTRING(buddy->group, right);

			port_printf(ses, "You set %s's group to '%s'", buddy->name, right);
		}
		else
		{
			port_printf(ses, "You are not connected to anyone named '%s'.", left);
		}
	}

	return ses;
}

/*
	The ignore feature.
*/

DO_PORT(port_ignore)
{
	struct port_data *buddy;

	if ((buddy = find_port_buddy(ses, left)) == NULL)
	{
		port_printf(ses, "You are not connected to anyone named '%s'.", left);

		return ses;
	}

	TOG_BIT(buddy->flags, CHAT_FLAG_IGNORE);

	if (HAS_BIT(buddy->flags, CHAT_FLAG_IGNORE))
	{
		port_printf(ses, "You are now ignoring %s.", buddy->name);
	}
	else
	{
		port_printf(ses, "You are no longer ignoring %s.", buddy->name);
	}
	
	return ses;
}



struct port_data *find_port_buddy(struct session *ses, char *arg)
{
	struct port_data *buddy;

	if (*arg == 0)
	{
		return NULL;
	}

	if (is_number(arg))
	{
		for (buddy = ses->port->next ; buddy ; buddy = buddy->next)
		{
			if (atoi(arg) == buddy->fd)
			{
				return buddy;
			}
		}
	}

	for (buddy = ses->port->next ; buddy ; buddy = buddy->next)
	{
		if (!strcmp(arg, buddy->ip))
		{
			return buddy;
		}
	}

	for (buddy = ses->port->next ; buddy ; buddy = buddy->next)
	{
		if (is_abbrev(arg, buddy->name))
		{
			return buddy;
		}
	}

	return NULL;
}


struct port_data *find_port_group(struct session *ses, char *arg)
{
	struct port_data *buddy;

	if (*arg == 0)
	{
		return NULL;
	}

	for (buddy = ses->port->next ; buddy ; buddy = buddy->next)
	{
		if (!strcmp(arg, buddy->group))
		{
			return buddy;
		}
	}
	return NULL;
}
