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
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                      coded by Igor van den Hoven 2006                       *
******************************************************************************/


#include "tintin.h"

#include <sys/ioctl.h>
#include <termios.h>
#include <errno.h>

void init_terminal()
{
	struct termios io;

	if (tcgetattr(0, &gtd->terminal))
	{
		perror("tcgetattr");

		exit(errno);
	}

	io = gtd->terminal;

	/*
		Canonical mode off
	*/

	DEL_BIT(io.c_lflag, ICANON);

	io.c_cc[VMIN]  = 1;
	io.c_cc[VTIME] = 0;

	/*
		Make the terminalal as raw as possible
	*/

/*
	DEL_BIT(io.c_iflag, IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	DEL_BIT(io.c_oflag, OPOST);

	DEL_BIT(io.c_cflag, CSIZE|PARENB);
*/
	DEL_BIT(io.c_lflag, ECHO|ECHONL|IEXTEN);
	DEL_BIT(io.c_lflag, ECHO|ECHONL);

	SET_BIT(io.c_cflag, CS8);

	if (tcsetattr(0, TCSANOW, &io))
	{
		perror("tcsetattr");

		exit(errno);
	}
}

void reset_terminal(void)
{
	tcsetattr(0, TCSANOW, &gtd->terminal);
}

void echo_off(struct session *ses)
{
	struct termios io;

	tcgetattr(STDIN_FILENO, &io);

	DEL_BIT(io.c_lflag, ECHO|ECHONL);

	tcsetattr(STDIN_FILENO, TCSADRAIN, &io);
}

void echo_on(struct session *ses)
{
	struct termios io;

	tcgetattr(STDIN_FILENO, &io);

	SET_BIT(io.c_lflag, ECHO|ECHONL);

	tcsetattr(STDIN_FILENO, TCSADRAIN, &io);
}

void init_screen_size(struct session *ses)
{
	int top, bot;
	struct winsize screen;

	top = ses->top_row == 0 ? 1 : ses->top_row;
	bot = ses->bot_row == 0 ? 0 : ses->rows - ses->bot_row;

	if (ioctl(1, TIOCGWINSZ, &screen))
	{
		ses->rows = SCREEN_HEIGHT;
		ses->cols = SCREEN_WIDTH;
	}
	else
	{
		ses->rows = screen.ws_row;
		ses->cols = screen.ws_col;
	}

	ses->top_row = top;
	ses->bot_row = ses->rows - bot;

	if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
	{
		init_split(ses, ses->top_row, ses->bot_row);
	}
	SET_BIT(gtd->flags, TINTIN_FLAG_RESETBUFFER);
}

int get_scroll_size(struct session *ses)
{
	return (ses->bot_row - ses->top_row);
}
