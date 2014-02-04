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
*                      coded by Igor van den Hoven 2004                       *
******************************************************************************/


#include "tintin.h"


struct command_type command_table[] =
{
	{	"action",            do_action,            CMD_FLAG_NONE    },
	{	"alias",             do_alias,             CMD_FLAG_NONE    },
	{	"all",               do_all,               CMD_FLAG_SUB     },
	{	"bell",              do_bell,              CMD_FLAG_NONE    },
	{	"buffer",            do_buffer,            CMD_FLAG_SUB     },
	{	"chat",              do_chat,              CMD_FLAG_SUB     },
	{	"class",             do_class,             CMD_FLAG_SUB     },
	{	"commands",          do_commands,          CMD_FLAG_NONE    },
	{	"config",            do_configure,         CMD_FLAG_SUB     },
	{	"cr",                do_cr,                CMD_FLAG_NONE    },
	{	"cursor",            do_cursor,            CMD_FLAG_SUB     },
	{	"debug",             do_debug,             CMD_FLAG_SUB     },
	{	"delay",             do_delay,             CMD_FLAG_NONE    },
	{	"echo",              do_echo,              CMD_FLAG_SUB     },
	{	"end",               do_end,               CMD_FLAG_NONE    },
	{	"event",             do_event,             CMD_FLAG_NONE    },
	{	"forall",            do_forall,            CMD_FLAG_NONE    },
	{	"format",            do_format,            CMD_FLAG_SUB     },
	{	"function",          do_function,          CMD_FLAG_NONE    },
	{	"gag",               do_gag,               CMD_FLAG_NONE    },
	{	"gagline",           do_gagline,           CMD_FLAG_NONE    },
	{	"grep",              do_grep,              CMD_FLAG_SUB     },
	{	"help",              do_help,              CMD_FLAG_NONE    },
	{	"highlight",         do_highlight,         CMD_FLAG_NONE    },
	{	"history",           do_history,           CMD_FLAG_SUB     },
	{	"if",                do_if,                CMD_FLAG_NONE    },
	{	"ignore",            do_ignore,            CMD_FLAG_SUB     },
	{	"info",              do_info,              CMD_FLAG_NONE    },
	{	"killall",           do_killall,           CMD_FLAG_NONE    },
	{	"list",              do_list,              CMD_FLAG_SUB     },
	{	"log",               do_log,               CMD_FLAG_SUB     },
	{	"logline",           do_logline,           CMD_FLAG_SUB     },
	{	"loop",              do_loop,              CMD_FLAG_NONE    },
	{	"macro",             do_macro,             CMD_FLAG_NONE    },
	{	"map",               do_map,               CMD_FLAG_SUB     },
	{	"math",              do_math,              CMD_FLAG_SUB     },
	{	"message",           do_message,           CMD_FLAG_SUB     },
	{	"nop",               do_nop,               CMD_FLAG_NONE    },
	{	"parse",             do_parse,             CMD_FLAG_NONE    },
	{	"path",              do_path,              CMD_FLAG_SUB     },
	{	"pathdir",           do_pathdir,           CMD_FLAG_NONE    },
	{	"prompt",            do_prompt,            CMD_FLAG_NONE    },
	{	"read",              do_read,              CMD_FLAG_SUB     },
	{	"regexp",            do_regexp,            CMD_FLAG_NONE    },
	{	"replacestring",     do_replacestring,     CMD_FLAG_SUB     },
	{	"return",            do_return,            CMD_FLAG_NONE    },
	{	"run",               do_run,               CMD_FLAG_SUB     },
	{	"scan",              do_scan,              CMD_FLAG_SUB     },
	{	"script",            do_script,            CMD_FLAG_SUB     },
	{	"send",              do_send,              CMD_FLAG_NONE    },
	{	"session",           do_session,           CMD_FLAG_SUB     },
	{	"showme",            do_showme,            CMD_FLAG_SUB     },
	{	"snoop",             do_snoop,             CMD_FLAG_SUB     },
	{	"split",             do_split,             CMD_FLAG_SUB     },
	{	"substitute",        do_substitute,        CMD_FLAG_NONE    },
	{	"suspend",           do_suspend,           CMD_FLAG_NONE    },
	{	"system",            do_system,            CMD_FLAG_SUB     },
	{	"tab",               do_tab,               CMD_FLAG_NONE    },
	{	"textin",            do_textin,            CMD_FLAG_SUB     },
	{	"ticker",            do_tick,              CMD_FLAG_NONE    },
	{	"unaction",          do_unaction,          CMD_FLAG_NONE    },
	{	"unalias",           do_unalias,           CMD_FLAG_NONE    },
	{	"undelay",           do_undelay,           CMD_FLAG_NONE    },
	{	"unevent",           do_unevent,           CMD_FLAG_NONE    },
	{	"unfunction",        do_unfunction,        CMD_FLAG_NONE    },
	{	"ungag",             do_ungag,             CMD_FLAG_NONE    },
	{	"unhighlight",       do_unhighlight,       CMD_FLAG_NONE    },
	{	"unmacro",           do_unmacro,           CMD_FLAG_NONE    },
	{	"unpathdir",         do_unpathdir,         CMD_FLAG_NONE    },
	{	"unprompt",          do_unprompt,          CMD_FLAG_NONE    },
	{	"unsplit",           do_unsplit,           CMD_FLAG_NONE    },
	{	"unsubstitute",      do_unsubstitute,      CMD_FLAG_NONE    },
	{	"untab",             do_untab,             CMD_FLAG_NONE    },
	{	"unticker",          do_untick,            CMD_FLAG_NONE    },
	{	"unvariable",        do_unvariable,        CMD_FLAG_NONE    },
	{	"variable",          do_variable,          CMD_FLAG_SUB     },
	{	"write",             do_write,             CMD_FLAG_SUB     },
	{	"zap",               do_zap,               CMD_FLAG_NONE    },
	{	"",                  NULL,                 CMD_FLAG_NONE    }
};


struct list_type list_table[LIST_MAX] =
{
	{    "ACTION",            "ACTIONS",            PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "ALIAS",             "ALIASES",            PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "CLASS",             "CLASSES",            APPEND,      2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_INHERIT                                 },
	{    "CONFIG",            "CONFIGURATIONS",     PRIORITY,    2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_INHERIT                 },
	{    "DELAY",             "DELAYS",             PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ                                                   },
	{    "EVENT",             "EVENTS",             ALPHA,       2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "FUNCTION",          "FUNCTIONS",          ALPHA,       2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "GAG",               "GAGS",               ALPHA,       1,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "HIGHLIGHT",         "HIGHLIGHTS",         PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "HISTORY",           "HISTORIES",          APPEND,      1,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE                                                                  },
	{    "MACRO",             "MACROS",             ALPHA,       3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "MAP",               "MAPPING",            APPEND,      3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE                                                                  },
	{    "MATH",              "MATHEMATICS",        APPEND,      3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE                                                                  },
	{    "PATH",              "PATHS",              APPEND,      2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE                                                                  },
	{    "PATHDIR",           "PATHDIRS",           PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "PROMPT",            "PROMPTS",            PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "SUBSTITUTE",        "SUBSTITUTIONS",      PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "TAB",               "TABS",               ALPHA,       1,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "TICKER",            "TICKERS",            PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "VARIABLE",          "VARIABLES",          ALPHA,       2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT }

};


struct config_type config_table[] =
{
	{
		"SPEEDWALK",
		"Your input is scanned for speedwalk directions",
		"Your input is not scanned for speedwalk directions",
		config_speedwalk
	},

	{
		"VERBATIM",
		"Your input is send directly to the mud",
		"Your input is parsed by tintin++",
		config_verbatim
	},

	{
		"REPEAT ENTER",
		"You send the last command on an enter",
		"You send a carriage return on an enter",
		config_repeatenter
	},

	{
		"ECHO COMMAND",
		"Your commands are echoed in split mode.",
		"Your commands are not echoed in split mode.",
		config_echocommand
	},

	{
		"VERBOSE",
		"Messages while reading a script file are echoed",
		"Messages while reading a script file are gagged",
		config_verbose
	},

	{
		"WORDWRAP",
		"Mud output is word wrapped",
		"Mud output is line wrapped",
		config_wordwrap
	},

	{
		"LOG",
		"",
		"The data format of your log files",
		config_log
	},
	{
		"BUFFER SIZE",
		"",
		"The size of your scroll back buffer",
		config_buffersize
	},

	{
		"SCROLL LOCK",
		"You do not see mud output while scrolling",
		"You see mud output while scrolling",
		config_scrolllock
	},

	{
		"HISTORY SIZE",
		"",
		"The size of your command history",
		config_historysize
	},

	{
		"CONNECT RETRY",
		"",
		"Seconds tintin will try to connect before giving up",
		config_connectretry
	},

	{
		"PACKET PATCH",
		"",
		"Milli seconds tintin will try to patch broken packets",
		config_packetpatch
	},

	{
		"TINTIN CHAR",
		"",
		"The character used for tintin commands",
		config_tintinchar
	},

	{
		"VERBATIM CHAR",
		"",
		"The character used for unparsed text",
		config_verbatimchar
	},

	{
		"REPEAT CHAR",
		"",
		"The character used for repeating commands",
		config_repeatchar
	},

	{
		"DEBUG TELNET",
		"You see telnet negotiations.",
		"You do not see telnet negotatiations.",
		config_debugtelnet
	},

	{
		"CONVERT META",
		"Tintin converts meta prefixed characters.",
		"Tintin doesn't convert meta prefixed characters.",
		config_convertmeta
	},

	{
		"LOG LEVEL",
		"Tintin only logs low level mud data.",
		"Tintin only logs high level mud data.",
		config_loglevel
	},

	{
		"COLOR PATCH",
		"Tintin will properly color the start of each line.",
		"Tintin will leave color handling to the server.",
		config_colorpatch
	},

	{
		"",
		"",
		0,
		0
	}
};

struct color_type color_table[] =
{
	{    "reset",          "0", "<088>" },
	{    "light",          "1", "<188>" },
	{    "bold",           "1", "<188>" },
	{    "faint",          "2", "<288>" },
	{    "underscore",     "4", "<488>" },
	{    "blink",          "5", "<588>" },
	{    "reverse",        "7", "<788>" },
	{    "dim",           "22", "<288>" },
	{    "dark",          "22", "<288>" },
	{    "black",         "30", "<808>" },
	{    "red",           "31", "<818>" },
	{    "green",         "32", "<828>" },
	{    "yellow",        "33", "<838>" },
	{    "blue",          "34", "<848>" },
	{    "magenta",       "35", "<858>" },
	{    "cyan",          "36", "<868>" },
	{    "white",         "37", "<878>" },
	{    "b black",       "40", "<880>" },
	{    "b red",         "41", "<881>" },
	{    "b green",       "42", "<882>" },
	{    "b yellow",      "43", "<883>" },
	{    "b blue",        "44", "<884>" },
	{    "b magenta",     "45", "<885>" },
	{    "b cyan",        "46", "<886>" },
	{    "b white",       "47", "<887>" },
	{    "",              ""  , "<888>" }
};

struct class_type class_table[] =
{
	{    "OPEN",                       class_open             },
	{	"CLOSE",                      class_close            },
	{    "READ",                       class_read             },
	{    "WRITE",                      class_write            },
	{    "KILL",                       class_kill             },
	{    "",                           NULL                   },
};

struct chat_type chat_table[] =
{
	{     "ACCEPT",          chat_accept,         1, 0, "Accept a file transfer"		               },
	{     "CALL",            chat_call,           0, 0, "Call a buddy"                                   },
	{     "CANCELFILE",      chat_cancelfile,     1, 0, "Cancel a file transfer"                         },
	{     "COLOR",           chat_color,          1, 0, "Set the default chat color"                     },
	{     "DECLINE",         chat_decline,        1, 0, "Decline a file transfer"                        },
	{     "DND",             chat_dnd,            0, 0, "Decline new connections"                        },
	{     "DOWNLOADDIR",     chat_downloaddir,    1, 0, "Set the download directory"                     },
	{     "EMOTE",           chat_emote,          0, 1, "Send an emoted chat message"                    },
	{     "FORWARD",         chat_forward,        1, 0, "Forward all chat messages to a buddy"           },
	{     "FORWARDALL",      chat_forwardall,     1, 0, "Forward all chat/mud messages to a buddy"       },
	{     "FILESTAT",        chat_filestat,       1, 0, "Show file transfer data"                        },
	{     "GROUP",           chat_group,          0, 1, "Assign a group to a buddy"                      },
	{     "IGNORE",          chat_ignore,         1, 0, "Ignore all messages from a buddy"               },
	{     "INITIALIZE",      chat_initialize,     1, 0, "Initialize chat with an optional port number"   },
	{     "INFO",            chat_info,           0, 0, "Display your chat settings"                     },
	{     "IP",              chat_ip,             1, 0, "Change your IP address, unset by default"       },
	{     "MESSAGE",         chat_message,        0, 1, "Send a private message to a buddy"              },
	{     "NAME",            chat_name,           1, 0, "Change your chat name"                          },
	{     "PASTE",           chat_paste,          0, 1, "Paste a block of text to a buddy"               },
	{     "PEEK",            chat_peek,           1, 0, "Show a buddy's public connections"              },
	{     "PING",            chat_ping,           1, 0, "Display a buddy's response time"                },
	{     "PRIVATE",         chat_private,        1, 0, "Do not share a buddy's IP address"              },
	{     "PUBLIC",          chat_public,         1, 0, "Share a buddy's IP address"                     },
	{     "REPLY",           chat_reply,          1, 0, "Reply to last private message"                  },
	{     "REQUEST",         chat_request,        1, 0, "Request a buddy's public connections"           },
	{     "SEND",            chat_send,           0, 1, "Send a raw data message to a buddy"             },
	{     "SENDFILE",        chat_sendfile,       0, 1, "Send a file to a buddy"                         },
	{     "SERVE",           chat_serve,          1, 0, "Forward all public chat messages to a buddy"    },
	{     "UNINITIALIZE",    chat_uninitialize,   0, 0, "Uninitializes the chat server"                  },
	{     "WHO",             chat_who,            0, 0, "Show all connections"                           },
	{     "ZAP",             chat_zap,            1, 0, "Close the connection to a buddy"                },
	{     "",                NULL,                0, 0, ""                                               }
};


struct array_type array_table[] =
{
	{     "CLR",             array_clr,           0, 0    },
	{     "DEL",             array_del,           0, 0    },
	{     "FND",             array_fnd,           0, 1    },
	{     "GET",             array_get,           0, 1    },
	{     "INS",             array_ins,           0, 1    },
	{     "LEN",             array_len,           1, 0    },
	{     "SET",             array_set,           0, 1    },
	{     "SRT",             array_srt,           1, 0    },
	{     "",                NULL,                0, 0    }
};

struct map_type map_table[] =
{
	{     "COLOR",                     map_color,              1,1},
	{     "CREATE",                    map_create,             1,1},
	{     "DELETE",                    map_delete,             1,1},
	{     "DESTROY",                   map_destroy,            1,1},
	{     "DIG",                       map_dig,                1,1},
	{     "EXIT",                      map_exit,               0,1},
	{     "EXPLORE",                   map_explore,            1,1},
	{     "FIND",                      map_find,               1,1},
	{     "FLAG",                      map_flag,               1,1},
	{     "GET",                       map_get,                0,1},
	{     "GOTO",                      map_goto,               1,1},
	{     "INFO",                      map_info,               1,1},
	{     "INSERT",                    map_insert,             1,1},
	{     "LEAVE",                     map_leave,              1,1},
	{     "LEGENDA",                   map_legenda,            1,1},
	{     "LINK",                      map_link,               0,1},
	{     "LIST",                      map_list,               1,1},
	{     "MAP",                       map_map,                1,1},
	{     "MOVE",                      map_move,               1,1},
	{     "NAME",                      map_name,               1,1},
	{     "READ",                      map_read,               1,1},
	{     "ROOMFLAG",                  map_roomflag,           1,1},
	{     "SET",                       map_set,                0,1},
	{     "TRAVEL",                    map_travel,             1,1},
	{     "UNDO",                      map_undo,               1,1},
	{     "UNLINK",                    map_unlink,             1,1},
	{     "WALK",                      map_walk,               1,1},
	{     "WRITE",                     map_write,              1,1},
	{     "",                          NULL,                   1,1}
};

struct cursor_type cursor_table[] =
{
	{
		"",
		"",
		"[5~",
		buffer_u
	},
	{

		"",
		"",
		"[6~",
		buffer_d
	},
	{
		"",
		"",
		"[7~",
		buffer_h
	},
	{
		"",
		"",
		"[1~",
		buffer_h
	},
	{
		"",
		"",
		"OH",
		buffer_h
	},
	{
		"",
		"",
		"[8~",
		buffer_e
	},
	{
		"",
		"",
		"[4~",
		buffer_e
	},
	{
		"",
		"",
		"OF",
		buffer_e
	},
	{
		"",
		"",
		"[H",
		buffer_h
	},
	{
		"",
		"",
		"[F",
		buffer_e
	},
	{
		"",
		"",
		"",
		buffer_l
	},
	{
		"HOME",
		"Move the cursor to start of input",
		"",
		cursor_home
	},
	{
		"",
		"",
		"b",
		cursor_home
	},
	{
		"BACKWARD",
		"Move cursor backward",
		"",
		cursor_left
	},
	{
		"",
		"",
		"OD",
		cursor_left
	},
	{
		"EXIT",
		"Exit current session",
		"",
		cursor_exit
	},
	{
		"DELETE",
		"Delete character at cursor",
		"",
		cursor_delete
	},
	{
		"",
		"",
		"[3~",
		cursor_delete
	},
	{
		"END",
		"Move cursor to end of input",
		"",
		cursor_end
	},
	{
		"",
		"",
		"f",
		cursor_end
	},
	{
		"FORWARD",
		"Move cursor forward",
		"",
		cursor_right
	},
	{
		"",
		"",
		"OC",
		cursor_right
	},
	{
		"BACKSPACE",
		"Delete backward character",
		"",
		cursor_backspace
	},
	{
		"",
		"",
		"",
		cursor_backspace
	},
	{
		"TAB",
		"Tab completion on last word",
		"\t",
		cursor_tab
	},
	{
		"ENTER",
		"Process the input line",
		"",
		cursor_enter
	},
	{
		"CLEAR RIGHT",
		"Delete from cursor to end of input",
		"",
		cursor_clear_right
	},
	{
		"REDRAW INPUT",
		"Redraw the input line",
		"",
		cursor_redraw_input
	},
	{
		"HISTORY NEXT",
		"Select next command history entry",
		"",
		cursor_history_next
	},
	{
		"",
		"",
		"OB",
		cursor_history_next
	},
	{
		"HISTORY PREV",
		"Select previous command history entry",
		"",
		cursor_history_prev
	},
	{
		"",
		"",
		"OA",
		cursor_history_prev
	},
	{
		"HISTORY SEARCH",
		"Search command history",
		"",
		cursor_history_search
	},
	{
		"CLEAR LEFT",
		"Delete from cursor to start of input",
		"",
		cursor_clear_left
	},
	{
		"CONVERT META",
		"Meta convert the next character",
		"",
		cursor_convert_meta
	},
	{
		"DELETE WORD",
		"Delete next word backward",
		"",
		cursor_delete_word
	},
	{
		"PASTE BUFFER",
		"Paste the previously deleted input text",
		"",
		cursor_paste_buffer
	},
	{
		"SUSPEND",
		"Suspend program, return with fg",
		"",
		cursor_suspend
	},
	{
		"ECHO",
		"Turn local echoing on or off",
		"",
		cursor_echo
	},
	{
		"INSERT",
		"Turn insert mode on or off",
		"",
		cursor_insert
	},
	{
		"",
		"",
		"",
		NULL
	}
};

struct timer_type timer_table[] =
{
	{    "Poll Stdin"                  },
	{    "Poll Sessions"               },
	{    "Poll Chat Server"            },
	{    "Update Tickers"              },
	{    "Update Delays"               },
	{    "Update Packet Patcher"       },
	{    "Update Chat Server"          },
	{    "Update Terminal"             },
	{    "Update Memory"               },
	{    "Stall Program"               }
};

struct event_type event_table[] =
{
	{    "MAP ENTER ROOM",               "Triggers when entering a map room."    },
	{    "SESSION CONNECTED",            "Triggers when a new session connects." },
	{    "SESSION DISCONNECTED",         "Triggers when session disconnects."    },
	{    "",                             ""                                      }
};

struct path_type path_table[] =
{
	{    "DEL",                        path_del               },
	{    "END",                        path_end               },
	{    "INS",                        path_ins               },
	{    "LOAD",                       path_load              },
	{    "MAP",                        path_map               },
	{    "NEW",                        path_new               },
	{    "RUN",                        path_run               },
	{    "SAVE",                       path_save              },
	{    "WALK",                       path_walk              },
	{    "",                           NULL                   }
};
