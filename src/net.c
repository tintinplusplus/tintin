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
/* file: net.c - do all the net stuff                                */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

#include "tintin.h"

#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

/**************************************************/
/* try connect to the mud specified by the args   */
/* return fd on success / 0 on failure            */
/**************************************************/


int connect_mud(const char *host, const char *port, struct session *ses)
{
	int sock, d;
	struct sockaddr_in sockaddr;

	if (sscanf(host, "%d.%d.%d.%d", &d, &d, &d, &d) == 4)
	{
		sockaddr.sin_addr.s_addr = inet_addr(host);
	}
	else
	{
		struct hostent *hp;

		if (!(hp = gethostbyname(host)))
		{
			tintin_puts2("#ERROR - UNKNOWN HOST.", ses);
			return -1;
		}
		memcpy((char *)&sockaddr.sin_addr, hp->h_addr, sizeof(sockaddr.sin_addr));
	}

	if (is_number(port))
	{
		sockaddr.sin_port = htons(atoi(port));      /* inteprete port part */
	}
	else
	{
		tintin_puts("#THE PORT SHOULD BE A NUMBER.", ses);
		return -1;
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		syserr("socket");
	}

	sockaddr.sin_family = AF_INET;

	ses->connect_error = connect(sock, (struct sockaddr *)&sockaddr, sizeof(sockaddr));

	if (ses->connect_error)
	{
		close(sock);

		return 0;
	}

	if (fcntl(sock, F_SETFL, O_NDELAY|O_NONBLOCK) == -1)
	{
		perror("connect_mud: fcntl O_NDELAY|O_NONBLOCK");
	}

	return sock;
}

/************************************************************/
/* write line to the mud ses is connected to - add \n first */
/************************************************************/

void write_line_mud(const char *line, struct session *ses)
{
	if (ses == gts)
	{
		tintin_printf2(ses, "#NO SESSION ACTIVE. USE: %csession {name} {host port} TO START ONE.", gtd->tintin_char);
		return;
	}

	if (!HAS_BIT(ses->flags, SES_FLAG_CONNECTED))
	{
		tintin_printf2(ses, "#THIS SESSION IS NOT CONNECTED.");
		return;
	}

	if (write(ses->socket, line, strlen(line)) == -1)
	{
		syserr("write in write_line_mud");
	}
	return;
}


/*******************************************************************/
/* read at most BUFFER_SIZE chars from mud - parse protocol stuff  */
/*******************************************************************/

void read_buffer_mud(struct session *ses)
{
	unsigned char buf[BUFFER_SIZE];
	int size;

	gtd->mud_output_len = 0;

	if ((size = read(ses->socket, buf, BUFFER_SIZE - 1)) == -1)
	{
		perror("\n\033[0mread_buffer_mud: read");
	}

	if (size <= 0)
	{
		gtd->mud_output_len = -1;
		return;
	}

	buf[size] = 0;

	translate_telopts(ses, buf, size);

	gtd->mud_output_buf[gtd->mud_output_len] = 0;

	return;
}
