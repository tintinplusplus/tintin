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
	{    "event",             do_event,             CMD_FLAG_NONE    },
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
	{	"path",              do_path,              CMD_FLAG_NONE    },
	{	"pathdir",           do_pathdir,           CMD_FLAG_NONE    },
	{	"prompt",            do_prompt,            CMD_FLAG_NONE    },
	{	"read",              do_read,              CMD_FLAG_SUB     },
	{	"replacestring",     do_replacestring,     CMD_FLAG_SUB     },
	{	"return",            do_return,            CMD_FLAG_NONE    },
	{	"scan",              do_scan,              CMD_FLAG_SUB     },
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
	{	"variable",          do_variable,          CMD_FLAG_NONE    },
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
	{     "ACCEPT",          chat_accept,         "Accept a file transfer"		               },
	{     "CALL",            chat_call,           "Call a buddy"                                   },
	{     "CANCELFILE",      chat_cancelfile,     "Cancel a file transfer"                         },
	{     "COLOR",           chat_color,          "Set the default chat color"                     },
	{     "DECLINE",         chat_decline,        "Decline a file transfer"                        },
	{     "DND",             chat_dnd,            "Decline new connections"                        },
	{     "DOWNLOADDIR",     chat_downloaddir,    "Set the download directory"                     },
	{     "EMOTE",           chat_emote,          "Send an emoted chat message"                    },
	{     "FORWARD",         chat_forward,        "Forward all chat messages to a buddy"           },
	{     "FORWARDALL",      chat_forwardall,     "Forward all chat/mud messages to a buddy"       },
	{     "FILESTAT",        chat_filestat,       "Show file transfer data"                        },
	{     "GROUP",           chat_group,          "Assign a group to a buddy"                      },
	{     "IGNORE",          chat_ignore,         "Ignore all messages from a buddy"               },
	{     "INITIALIZE",      chat_initialize,     "Initialize chat with an optional port number"   },
	{     "INFO",            chat_info,           "Display your chat settings"                     },
	{     "IP",              chat_ip,             "Change your IP address, unset by default"       },
	{     "MESSAGE",         chat_message,        "Send a private message to a buddy"              },
	{     "NAME",            chat_name,           "Change your chat name"                          },
	{     "PASTE",           chat_paste,          "Paste a block of text to a buddy"               },
	{     "PEEK",            chat_peek,           "Show a buddy's public connections"              },
	{     "PING",            chat_ping,           "Display a buddy's response time"                },
	{     "PRIVATE",         chat_private,        "Do not share a buddy's IP address"              },
	{     "PUBLIC",          chat_public,         "Share a buddy's IP address"                     },
	{     "REPLY",           chat_reply,          "Reply to last private message"                  },
	{     "REQUEST",         chat_request,        "Request a buddy's public connections"           },
	{     "SEND",            chat_send,           "Send a raw data message to a buddy"             },
	{     "SENDFILE",        chat_sendfile,       "Send a file to a buddy"                         },
	{     "SERVE",           chat_serve,          "Forward all public chat messages to a buddy"    },
	{     "UNINITIALIZE",    chat_uninitialize,   "Uninitializes the chat server"                  },
	{     "WHO",             chat_who,            "Show all connections"                           },
	{     "ZAP",             chat_zap,            "Close the connection to a buddy"                },
	{     "",                NULL,                ""                                               }
};


struct array_type array_table[] =
{
	{     "DEL",                       array_del              },
	{     "FND",                       array_fnd              },
	{     "GET",                       array_get              },
	{     "INS",                       array_ins              },
	{     "LEN",                       array_len              },
	{     "SET",                       array_set              },
	{     "",                          NULL                   }
};

struct map_type map_table[] =
{
	{     "COLOR",                     map_color              },
	{     "CREATE",                    map_create             },
	{     "DELETE",                    map_delete             },
	{     "DESTROY",                   map_destroy            },
	{     "DIG",                       map_dig                },
	{     "EXIT",                      map_exit               },
	{     "FIND",                      map_find               },
	{     "FLAG",                      map_flag               },
	{     "GET",                       map_get                },
	{     "GOTO",                      map_goto               },
	{     "INFO",                      map_info               },
	{     "LEAVE",                     map_leave              },
	{     "LEGENDA",                   map_legenda            },
	{     "LINK",                      map_link               },
	{     "LIST",                      map_list               },
	{     "MAP",                       map_map                },
	{     "MOVE",                      map_move               },
	{     "NAME",                      map_name               },
	{     "READ",                      map_read               },
	{     "ROOMFLAG",                  map_roomflag           },
	{     "SET",                       map_set                },
	{     "UNDO",                      map_undo               },
	{     "UNLINK",                    map_unlink             },
	{     "WALK",                      map_walk               },
	{     "WRITE",                     map_write              },
	{     "",                          NULL                   }
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
	{    "Stall Program"               }
};

struct event_type event_table[] =
{
	{    "MAP ENTER ROOM",               "Triggers when entering a map room."    },
	{    "SESSION CONNECTED",            "Triggers when a new session connects." },
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
	{    "SAVE",                       path_save              },
	{    "WALK",                       path_walk              },
	{    "",                           NULL                   }
};
