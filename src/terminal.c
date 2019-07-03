/******************************************************************************
*   This file is part of TinTin++                                             *
*                                                                             *
*   Copyright 2004-2019 Igor van den Hoven                                    *
*                                                                             *
*   TinTin++ is free software; you can redistribute it and/or modify          *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 3 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with TinTin++.  If not, see https://www.gnu.org/licenses.           *
******************************************************************************/

/******************************************************************************
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                      coded by Igor van den Hoven 2006                       *
******************************************************************************/


#include "tintin.h"

#ifdef HAVE_SYS_IOCTL_H
  #include <sys/ioctl.h>
#endif
#include <termios.h>
#include <errno.h>

void init_terminal()
{
	struct termios io;

	if (tcgetattr(0, &gtd->old_terminal))
	{
		perror("tcgetattr");

		exit(errno);
	}

	io = gtd->old_terminal;

	/*
		Canonical mode off
	*/

	DEL_BIT(io.c_lflag, ICANON);

	io.c_cc[VMIN]   = 1;
	io.c_cc[VTIME]  = 0;
	io.c_cc[VSTART] = 255;
	io.c_cc[VSTOP]  = 255;
	io.c_cc[VINTR]  = 4; // ctrl-d

	/*
		Make the terminalal as raw as possible
	*/

/*
	DEL_BIT(io.c_iflag, IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	DEL_BIT(io.c_oflag, OPOST);
	DEL_BIT(io.c_cflag, CSIZE|PARENB);
*/

	DEL_BIT(io.c_lflag, ECHO|ECHONL|IEXTEN|ISIG);
//	DEL_BIT(io.c_lflag, ECHO|ECHONL|IEXTEN|ISIG);

	SET_BIT(io.c_cflag, CS8);

	if (tcsetattr(0, TCSANOW, &io))
	{
		perror("tcsetattr");

		exit(errno);
	}

	if (tcgetattr(0, &gts->cur_terminal))
	{
		perror("tcgetattr");

		exit(errno);
	}
}

void reset_terminal(void)
{
	tcsetattr(0, TCSANOW, &gtd->old_terminal);

	printf("\e[?1000l\e[?1002l\e[?1004l\e[?1006l");
}


void save_session_terminal(struct session *ses)
{
	tcgetattr(0, &ses->cur_terminal);
}

void refresh_session_terminal(struct session *ses)
{
//	tcsetattr(0, TCSANOW, &ses->cur_terminal);
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

void init_terminal_size(struct session *ses)
{
	struct winsize screen;
	int top, bot;

	push_call("init_terminal_size(%p)",ses);

	top = ses->top_row ? ses->top_row : 1;
	bot = ses->bot_row ? gtd->screen->rows - ses->bot_row : 0;

	if (ses == gts)
	{
		if (ioctl(0, TIOCGWINSZ, &screen) == -1)
		{
			init_screen(SCREEN_HEIGHT, SCREEN_WIDTH);
		}
		else
		{
			init_screen(screen.ws_row, screen.ws_col);
		}
		SET_BIT(gtd->flags, TINTIN_FLAG_RESETBUFFER);
	}

	bot = gtd->screen->rows - bot;

	if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
	{
		init_split(ses, top, bot);
	}
	else
	{
		ses->bot_row = bot;
	}

	check_all_events(ses, SUB_ARG, 0, 2, "SCREEN RESIZE", ntos(gtd->screen->cols), ntos(gtd->screen->rows));

	pop_call();
	return;
}

int get_scroll_size(struct session *ses)
{
	return (ses->bot_row - ses->top_row);
}
