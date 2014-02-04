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

/******************************************************************************
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                         coded by Peter Unold 1992                           *
******************************************************************************/

#include "tintin.h"

#include <signal.h>
#include <sys/socket.h>

/*************** globals ******************/

struct session *gts;
struct tintin_data *gtd;

void pipe_handler(int signal)
{
	restore_terminal();

	clean_screen(gtd->ses);

	tintin_printf(NULL, "broken_pipe: dumping stack");

	dump_stack();
}

/*
	when the screen size changes, take note of it
*/

void winch_handler(int signal)
{
	struct session *ses;

	init_screen_size(gts);

	for (ses = gts->next ; ses ; ses = ses->next)
	{
		init_screen_size(ses);

		if (HAS_BIT(ses->telopts, TELOPT_FLAG_NAWS))
		{
			send_sb_naws(ses, 0, NULL);
		}
	}

	/*
		we have to reinitialize the signals for sysv machines

	if (signal(SIGWINCH, winch_handler) == BADSIG)
	{
		syserr("signal SIGWINCH");
	}
	*/
}


void abort_handler(int signal)
{
	static char crashed = FALSE;

	if (crashed)
	{
		exit(-1);
	}
	crashed = TRUE;

	restore_terminal();

	clean_screen(gtd->ses);

	dump_stack();

	fflush(NULL);

	exit(-1);

	if (gtd->ses->connect_retry > utime())
	{
		gtd->ses->connect_retry = 0;
	}
	else if (HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_SGA) && !HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_ECHO))
	{
		socket_printf(gtd->ses, 1, "%c", 3);
	}
	else
	{
		do_zap(gtd->ses, "");
	}
}

void suspend_handler(int signal)
{
	printf("\033[r\033[%d;%dH", gtd->ses->rows, 1);

	fflush(stdout);

	restore_terminal();

	kill(0, SIGSTOP);

	dirty_screen(gtd->ses);

	init_terminal();

	tintin_puts(NULL, "#RETURNING BACK TO TINTIN++.");
}

void trap_handler(int signal)
{
	static char crashed = FALSE;

	if (crashed)
	{
		exit(-1);
	}
	crashed = TRUE;

	restore_terminal();

	clean_screen(gtd->ses);

	dump_stack();

	fflush(NULL);

	exit(-1);
}


/****************************************************************************/
/* main() - show title - setup signals - init lists - readcoms - mainloop() */
/****************************************************************************/


int main(int argc, char **argv)
{
	int greeting = TRUE;

	#ifdef SOCKS
		SOCKSinit(argv[0]);
	#endif

	if (signal(SIGTERM, trap_handler) == BADSIG)
	{
		syserr("signal SIGTERM");
	}

	if (signal(SIGSEGV, trap_handler) == BADSIG)
	{
		syserr("signal SIGSEGV");
	}

	if (signal(SIGHUP, trap_handler) == BADSIG)
	{
		syserr("signal SIGHUP");
	}

	if (signal(SIGABRT, abort_handler) == BADSIG)
	{
		syserr("signal SIGTERM");
	}

	if (signal(SIGINT, abort_handler) == BADSIG)
	{
		syserr("signal SIGINT");
	}

	if (signal(SIGTSTP, suspend_handler) == BADSIG)
	{
		syserr("signal SIGSTOP");
	}

	if (signal(SIGPIPE, pipe_handler) == BADSIG)
	{
		syserr("signal SIGPIPE");
	}

	if (signal(SIGWINCH, winch_handler) == BADSIG)
	{
		syserr("signal SIGWINCH");
	}

/*
	if (getenv("HOME") != NULL)
	{
		char filename[256];

		sprintf(filename, "%s/%s", getenv("HOME"), HISTORY_FILE);

		read_history(gts, filename);
	}
*/

	srand(time(NULL));

	if (argc > 1)
	{
		int c;

		while ((c = getopt(argc, argv, "e: G h r: s: t: v")) != EOF)
		{
			if (c == 'G')
			{
				greeting = FALSE;
			}
		}

		optind = 1;
	}

	init_tintin(greeting);

	if (argc > 1)
	{
		int c;

		optind = 1;

		while ((c = getopt(argc, argv, "e: G h r: s: t: v")) != EOF)
		{
			switch (c)
			{
				case 'e':
					gtd->ses = script_driver(gtd->ses, -1, optarg);
					break;

				case 'G':
					break;

				case 'h':
					tintin_printf(NULL, "Usage: %s [OPTION]... [FILE]...", argv[0]);
					tintin_printf(NULL, "");
					tintin_printf(NULL, "  -e  Execute given command.");
					tintin_printf(NULL, "  -G  Don't show the greeting screen.");
					tintin_printf(NULL, "  -h  This help section.");
					tintin_printf(NULL, "  -r  Read given file.");
					tintin_printf(NULL, "  -t  Set given title.");
					tintin_printf(NULL, "  -v  Enable verbose mode.");

					restore_terminal();
					exit(1);
					break;

				case 'r':
					gtd->ses = do_read(gtd->ses, optarg);
					break;

				case 't':
					printf("\033]0;%s\007", optarg);
					break;

				case 's':
					srand((unsigned int) atoll(optarg));
					break;

				case 'v':
					do_configure(gtd->ses, "{VERBOSE} {ON}");
					break;

				default:
					tintin_printf(NULL, "Unknown option '%c'.", c);
					break;
			}
		}

		if (argv[optind] != NULL)
		{
			gtd->ses = do_read(gtd->ses, argv[optind]);
		}
	}
	check_all_events(gts, SUB_ARG|SUB_SEC, 0, 2, "PROGRAM START", CLIENT_NAME, CLIENT_VERSION);
	check_all_events(gts, SUB_ARG|SUB_SEC, 0, 2, "SCREEN RESIZE", ntos(gts->cols), ntos(gts->rows));

	mainloop();

	return 0;
}


void init_tintin(int greeting)
{
	int ref, index;

	gts = (struct session *) calloc(1, sizeof(struct session));

	for (index = 0 ; index < LIST_MAX ; index++)
	{
		gts->list[index] = init_list(gts, index, 32);
	}

	gts->name           = strdup("gts");
	gts->group          = strdup("");
	gts->host           = strdup("");
	gts->port           = strdup("");
	gts->cmd_color      = strdup("");
	gts->telopts        = TELOPT_FLAG_ECHO;
	gts->flags          = SES_FLAG_MCCP;
	gts->socket         = 1;
	gts->read_max       = 16384;

	gtd                 = (struct tintin_data *) calloc(1, sizeof(struct tintin_data));

	gtd->ses            = gts;

	gtd->str_hash_size  = sizeof(struct str_hash_data);

	gtd->mccp_len       = 4096;
	gtd->mccp_buf       = (unsigned char *) calloc(1, gtd->mccp_len);

	gtd->mud_output_max = 16384;
	gtd->mud_output_buf = (char *) calloc(1, gtd->mud_output_max);

	gtd->input_off      = 1;

	gtd->term           = strdup(getenv("TERM") ? getenv("TERM") : "UNKNOWN");

	for (index = 0 ; index < 100 ; index++)
	{
		gtd->vars[index] = strdup("");
		gtd->cmds[index] = strdup("");
	}

	for (ref = 0 ; ref < 26 ; ref++)
	{
		for (index = 0 ; *command_table[index].name != 0 ; index++)
		{
			if (*command_table[index].name == 'a' + ref)
			{
				gtd->command_ref[ref] = index;
				break;
			}
		}
	}

	init_screen_size(gts);

	printf("\033="); // set application keypad mode

	gts->input_level++;

	do_configure(gts, "{AUTO TAB}         {5000}");
	do_configure(gts, "{BUFFER SIZE}     {20000}");
	do_configure(gts, "{COMMAND COLOR}   {<078>}");
	do_configure(gts, "{COMMAND ECHO}       {ON}");
	do_configure(gts, "{CONNECT RETRY}      {15}");
	do_configure(gts, "{CHARSET}         {ASCII}");
	do_configure(gts, "{HISTORY SIZE}     {1000}");
	do_configure(gts, "{LOG}               {RAW}");
	do_configure(gts, "{PACKET PATCH}     {0.00}");
	do_configure(gts, "{REPEAT CHAR}         {!}");
	do_configure(gts, "{REPEAT ENTER}      {OFF}");
	do_configure(gts, "{SCROLL LOCK}        {ON}");
	do_configure(gts, "{SPEEDWALK}         {OFF}");
	do_configure(gts, "{TINTIN CHAR}         {#}");
	do_configure(gts, "{VERBATIM}          {OFF}");
	do_configure(gts, "{VERBATIM CHAR}      {\\}");
	do_configure(gts, "{VERBOSE}           {OFF}");
	do_configure(gts, "{WORDWRAP}           {ON}");
	do_configure(gts, "{256 COLORS}       {AUTO}");

	gts->input_level--;

	insert_node_list(gts->list[LIST_PATHDIR],  "n",  "s",  "1");
	insert_node_list(gts->list[LIST_PATHDIR],  "e",  "w",  "2");
	insert_node_list(gts->list[LIST_PATHDIR],  "s",  "n",  "4");
	insert_node_list(gts->list[LIST_PATHDIR],  "w",  "e",  "8");
	insert_node_list(gts->list[LIST_PATHDIR],  "u",  "d", "16");
	insert_node_list(gts->list[LIST_PATHDIR],  "d",  "u", "32");

	insert_node_list(gts->list[LIST_PATHDIR], "ne", "sw",  "3");
	insert_node_list(gts->list[LIST_PATHDIR], "nw", "se",  "9");
	insert_node_list(gts->list[LIST_PATHDIR], "se", "nw",  "6");
	insert_node_list(gts->list[LIST_PATHDIR], "sw", "ne", "12");

	init_terminal();

	if (greeting)
	{
//		do_advertise(gts, "");

		do_help(gts, "GREETING");
	}
}


void quitmsg(char *message)
{
	struct session *ses;

	SET_BIT(gtd->flags, TINTIN_FLAG_TERMINATE);

	while ((ses = gts->next) != NULL)
	{
		cleanup_session(ses);
	}

	if (gtd->chat)
	{
		close(gtd->chat->fd);
	}

	check_all_events(gts, SUB_ARG|SUB_SEC, 0, 0, "PROGRAM TERMINATION");

/*
	if (gtd->history_size)
	{
		char filename[BUFFER_SIZE];

		sprintf(filename, "%s/%s", getenv("HOME"), HISTORY_FILE);

		history_write(gts, filename);
	}
*/
	restore_terminal();

	clean_screen(gts);

	if (message)
	{
		printf("\n%s\n", message);
	}

	printf("\nGoodbye from TinTin++\n\n");

	fflush(NULL);

	exit(0);
}
