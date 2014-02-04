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
	{    "action",            do_action,            TOKEN_OPR_CMD    },
	{    "alias",             do_alias,             TOKEN_OPR_CMD    },
	{    "all",               do_all,               TOKEN_OPR_CMD    },
	{    "bell",              do_bell,              TOKEN_OPR_CMD    },
	{    "buffer",            do_buffer,            TOKEN_OPR_CMD    },
	{    "chat",              do_chat,              TOKEN_OPR_CMD    },
	{    "class",             do_class,             TOKEN_OPR_CMD    },
	{    "commands",          do_commands,          TOKEN_OPR_CMD    },
	{    "config",            do_configure,         TOKEN_OPR_CMD    },
	{    "cr",                do_cr,                TOKEN_OPR_CMD    },
	{    "cursor",            do_cursor,            TOKEN_OPR_CMD    },
	{    "debug",             do_debug,             TOKEN_OPR_CMD    },
	{    "delay",             do_delay,             TOKEN_OPR_CMD    },
	{    "echo",              do_echo,              TOKEN_OPR_CMD    },
	{    "end",               do_end,               TOKEN_OPR_CMD    },
	{    "event",             do_event,             TOKEN_OPR_CMD    },
	{    "forall",            do_forall,            TOKEN_OPR_CMD    },
	{    "format",            do_format,            TOKEN_OPR_CMD    },
	{    "function",          do_function,          TOKEN_OPR_CMD    },
	{    "gag",               do_gag,               TOKEN_OPR_CMD    },
	{    "gagline",           do_gagline,           TOKEN_OPR_CMD    },
	{    "grep",              do_grep,              TOKEN_OPR_CMD    },
	{    "help",              do_help,              TOKEN_OPR_CMD    },
	{    "highlight",         do_highlight,         TOKEN_OPR_CMD    },
	{    "history",           do_history,           TOKEN_OPR_CMD    },
	{    "if",                do_if,                TOKEN_OPR_IF     },
	{    "ignore",            do_ignore,            TOKEN_OPR_CMD    },
	{    "info",              do_info,              TOKEN_OPR_CMD    },
	{    "killall",           do_killall,           TOKEN_OPR_CMD    },
	{    "line",              do_line,              TOKEN_OPR_CMD    },
	{    "list",              do_list,              TOKEN_OPR_CMD    },
	{    "log",               do_log,               TOKEN_OPR_CMD    },
	{    "logline",           do_logline,           TOKEN_OPR_CMD    },
	{    "loop",              do_loop,              TOKEN_OPR_CMD    },
	{    "macro",             do_macro,             TOKEN_OPR_CMD    },
	{    "map",               do_map,               TOKEN_OPR_CMD    },
	{    "math",              do_math,              TOKEN_OPR_CMD    },
	{    "message",           do_message,           TOKEN_OPR_CMD    },
	{    "nop",               do_nop,               TOKEN_OPR_CMD    },
	{    "parse",             do_parse,             TOKEN_OPR_CMD    },
	{    "path",              do_path,              TOKEN_OPR_CMD    },
	{    "pathdir",           do_pathdir,           TOKEN_OPR_CMD    },
	{    "prompt",            do_prompt,            TOKEN_OPR_CMD    },
	{    "read",              do_read,              TOKEN_OPR_CMD    },
	{    "regexp",            do_regexp,            TOKEN_OPR_CMD    },
	{    "replacestring",     do_replace,           TOKEN_OPR_CMD    },
	{    "return",            do_return,            TOKEN_OPR_RETURN },
	{    "run",               do_run,               TOKEN_OPR_CMD    },
	{    "scan",              do_scan,              TOKEN_OPR_CMD    },
	{    "script",            do_script,            TOKEN_OPR_CMD    },
	{    "send",              do_send,              TOKEN_OPR_CMD    },
	{    "session",           do_session,           TOKEN_OPR_CMD    },
	{    "showme",            do_showme,            TOKEN_OPR_CMD    },
	{    "snoop",             do_snoop,             TOKEN_OPR_CMD    },
	{    "split",             do_split,             TOKEN_OPR_CMD    },
	{    "substitute",        do_substitute,        TOKEN_OPR_CMD    },
	{    "suspend",           do_suspend,           TOKEN_OPR_CMD    },
	{    "system",            do_system,            TOKEN_OPR_CMD    },
	{    "tab",               do_tab,               TOKEN_OPR_CMD    },
	{    "textin",            do_textin,            TOKEN_OPR_CMD    },
	{    "ticker",            do_tick,              TOKEN_OPR_CMD    },
	{    "unaction",          do_unaction,          TOKEN_OPR_CMD    },
	{    "unalias",           do_unalias,           TOKEN_OPR_CMD    },
	{    "undelay",           do_undelay,           TOKEN_OPR_CMD    },
	{    "unevent",           do_unevent,           TOKEN_OPR_CMD    },
	{    "unfunction",        do_unfunction,        TOKEN_OPR_CMD    },
	{    "ungag",             do_ungag,             TOKEN_OPR_CMD    },
	{    "unhighlight",       do_unhighlight,       TOKEN_OPR_CMD    },
	{    "unmacro",           do_unmacro,           TOKEN_OPR_CMD    },
	{    "unpathdir",         do_unpathdir,         TOKEN_OPR_CMD    },
	{    "unprompt",          do_unprompt,          TOKEN_OPR_CMD    },
	{    "unsplit",           do_unsplit,           TOKEN_OPR_CMD    },
	{    "unsubstitute",      do_unsubstitute,      TOKEN_OPR_CMD    },
	{    "untab",             do_untab,             TOKEN_OPR_CMD    },
	{    "unticker",          do_untick,            TOKEN_OPR_CMD    },
	{    "unvariable",        do_unvariable,        TOKEN_OPR_CMD    },
	{    "variable",          do_variable,          TOKEN_OPR_CMD    },
	{    "while",             do_while,             TOKEN_OPR_CMD    },
	{    "write",             do_write,             TOKEN_OPR_CMD    },
	{    "zap",               do_zap,               TOKEN_OPR_CMD    },
	{    "",                  NULL,                 TOKEN_OPR_CMD    }
};


struct list_type list_table[LIST_MAX] =
{
	{    "ACTION",            "ACTIONS",            PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "ALIAS",             "ALIASES",            PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "CLASS",             "CLASSES",            APPEND,      2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_INHERIT                                 },
	{    "CONFIG",            "CONFIGURATIONS",     PRIORITY,    2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_INHERIT                 },
	{    "DELAY",             "DELAYS",             ALPHA,       3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ                                                   },
	{    "EVENT",             "EVENTS",             ALPHA,       2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "FUNCTION",          "FUNCTIONS",          ALPHA,       2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "GAG",               "GAGS",               ALPHA,       1,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "HIGHLIGHT",         "HIGHLIGHTS",         PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "HISTORY",           "HISTORIES",          APPEND,      1,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE                                                                  },
	{    "MACRO",             "MACROS",             ALPHA,       2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "MAP",               "MAPPING",            APPEND,      3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE                                                                  },
	{    "MATH",              "MATHEMATICS",        APPEND,      3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE                                                                  },
	{    "PATH",              "PATHS",              APPEND,      2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE                                                                  },
	{    "PATHDIR",           "PATHDIRS",           PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "PROMPT",            "PROMPTS",            PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "SUBSTITUTE",        "SUBSTITUTIONS",      PRIORITY,    3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "TAB",               "TABS",               ALPHA,       1,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "TABCYCLE",          "TABCYCLE",           APPEND,      1,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE },
	{    "TICKER",            "TICKERS",            ALPHA,       3,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "VARIABLE",          "VARIABLES",          ALPHA,       2,  LIST_FLAG_SHOW|LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT }

};


struct config_type config_table[] =
{
	{
		"AUTO TAB",
		"",
		"Scroll back buffer lines used for tab completion",
		config_autotab
	},

	{
		"BUFFER SIZE",
		"",
		"The size of the scroll back buffer",
		config_buffersize
	},

	{
		"COLOR PATCH",
		"Tintin will properly color the start of each line",
		"Tintin will leave color handling to the server",
		config_colorpatch
	},

	{
		"CONNECT RETRY",
		"",
		"Seconds Tintin will try to connect before giving up",
		config_connectretry
	},

	{
		"CONVERT META",
		"Tintin converts meta prefixed characters",
		"Tintin doesn't convert meta prefixed characters",
		config_convertmeta
	},

	{
		"DEBUG TELNET",
		"You see telnet negotiations",
		"You do not see telnet negotatiations",
		config_debugtelnet
	},

	{
		"COMMAND COLOR",
		"",
		"The color of echoed commands",
		config_commandcolor
	},

	{
		"COMMAND ECHO",
		"Your commands are echoed in split mode",
		"Your commands are not echoed in split mode",
		config_commandecho
	},

	{
		"HISTORY SIZE",
		"",
		"The size of the command history",
		config_historysize
	},

	{
		"LOG",
		"",
		"The data format of the log files",
		config_log
	},

	{
		"LOG LEVEL",
		"Tintin only logs low level mud data",
		"Tintin only logs high level mud data",
		config_loglevel
	},

	{
		"MCCP",
		"MCCP is enabled.",
		"MCCP is disabled.",
		config_mccp
	},

	{
		"PACKET PATCH",
		"",
		"Seconds Tintin will try to patch broken packets",
		config_packetpatch
	},

	{
		"REGEXP",
		"Tintin will use regular expressions for text matching",
		"Tintin will use the * and ? wildcards for text matching",
		config_regexp
	},

	{
		"REPEAT ENTER",
		"You send the last command on an enter",
		"You send a carriage return on an enter",
		config_repeatenter
	},

	{
		"REPEAT CHAR",
		"",
		"The character used for repeating commands",
		config_repeatchar
	},

	{
		"SCROLL LOCK",
		"You do not see mud output while scrolling",
		"You see mud output while scrolling",
		config_scrolllock
	},

	{
		"SPEEDWALK",
		"Your input is scanned for speedwalk directions",
		"Your input is not scanned for speedwalk directions",
		config_speedwalk
	},

	{
		"TINTIN CHAR",
		"",
		"The character used for Tintin commands",
		config_tintinchar
	},

	{
		"VERBATIM",
		"Your keyboard input isn't modified by Tintin",
		"Your keyboard input is parsed by Tintin",
		config_verbatim
	},

	{
		"VERBATIM CHAR",
		"",
		"The character used for unparsed text",
		config_verbatimchar
	},

	{
		"VERBOSE",
		"Messages while reading in a script file are echoed",
		"Messages while reading in a script file are gagged",
		config_verbose
	},

	{
		"WORDWRAP",
		"Mud output is word wrapped",
		"Mud output is line wrapped",
		config_wordwrap
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
	{    "OPEN",              class_open             },
	{    "CLOSE",             class_close            },
	{    "READ",              class_read             },
	{    "WRITE",             class_write            },
	{    "KILL",              class_kill             },
	{    "",                  NULL                   },
};

struct chat_type chat_table[] =
{
	{     "ACCEPT",           chat_accept,         1, 0, "Accept a file transfer"		              },
	{     "CALL",             chat_call,           0, 0, "Call a buddy"                                   },
	{     "CANCELFILE",       chat_cancelfile,     1, 0, "Cancel a file transfer"                         },
	{     "COLOR",            chat_color,          1, 0, "Set the default chat color"                     },
	{     "DECLINE",          chat_decline,        1, 0, "Decline a file transfer"                        },
	{     "DND",              chat_dnd,            0, 0, "Decline new connections"                        },
	{     "DOWNLOADDIR",      chat_downloaddir,    1, 0, "Set the download directory"                     },
	{     "EMOTE",            chat_emote,          0, 1, "Send an emoted chat message"                    },
	{     "FORWARD",          chat_forward,        1, 0, "Forward all chat messages to a buddy"           },
	{     "FORWARDALL",       chat_forwardall,     1, 0, "Forward all chat/mud messages to a buddy"       },
	{     "FILESTAT",         chat_filestat,       1, 0, "Show file transfer data"                        },
	{     "GROUP",            chat_group,          0, 1, "Assign a group to a buddy"                      },
	{     "IGNORE",           chat_ignore,         1, 0, "Ignore all messages from a buddy"               },
	{     "INITIALIZE",       chat_initialize,     1, 0, "Initialize chat with an optional port number"   },
	{     "INFO",             chat_info,           0, 0, "Display the chat settings"                      },
	{     "IP",               chat_ip,             1, 0, "Change the IP address, unset by default"        },
	{     "MESSAGE",          chat_message,        0, 1, "Send a private message to a buddy"              },
	{     "NAME",             chat_name,           1, 0, "Change the chat name"                           },
	{     "PASTE",            chat_paste,          0, 1, "Paste a block of text to a buddy"               },
	{     "PEEK",             chat_peek,           1, 0, "Show a buddy's public connections"              },
	{     "PING",             chat_ping,           1, 0, "Display a buddy's response time"                },
	{     "PRIVATE",          chat_private,        1, 0, "Do not share a buddy's IP address"              },
	{     "PUBLIC",           chat_public,         1, 0, "Share a buddy's IP address"                     },
	{     "REPLY",            chat_reply,          1, 0, "Reply to last private message"                  },
	{     "REQUEST",          chat_request,        1, 0, "Request a buddy's public connections"           },
	{     "SEND",             chat_send,           0, 1, "Send a raw data message to a buddy"             },
	{     "SENDFILE",         chat_sendfile,       0, 1, "Send a file to a buddy"                         },
	{     "SERVE",            chat_serve,          1, 0, "Forward all public chat messages to a buddy"    },
	{     "UNINITIALIZE",     chat_uninitialize,   0, 0, "Uninitializes the chat server"                  },
	{     "WHO",              chat_who,            0, 0, "Show all connections"                           },
	{     "ZAP",              chat_zap,            1, 0, "Close the connection to a buddy"                },
	{     "",                 NULL,                0, 0, ""                                               }
};


struct array_type array_table[] =
{
	{     "CLR",              array_clr,           0, 0    },
	{     "DEL",              array_del,           0, 0    },
	{     "FND",              array_fnd,           0, 1    },
	{     "GET",              array_get,           0, 1    },
	{     "INS",              array_ins,           0, 1    },
	{     "LEN",              array_len,           1, 0    },
	{     "SET",              array_set,           0, 1    },
	{     "SRT",              array_srt,           1, 0    },
	{     "",                 NULL,                0, 0    }
};

struct map_type map_table[] =
{
	{     "COLOR",            map_color,           1, 1    },
	{     "CREATE",           map_create,          1, 1    },
	{     "DELETE",           map_delete,          1, 1    },
	{     "DESTROY",          map_destroy,         1, 1    },
	{     "DIG",              map_dig,             1, 1    },
	{     "EXITCMD",          map_exitcmd,         0, 1    },
	{     "EXITDIR",          map_exitdir,         0, 1    },
	{     "EXPLORE",          map_explore,         1, 1    },
	{     "FIND",             map_find,            1, 1    },
	{     "FLAG",             map_flag,            0, 1    },
	{     "GET",              map_get,             0, 1    },
	{     "GOTO",             map_goto,            1, 1    },
	{     "INFO",             map_info,            1, 1    },
	{     "INSERT",           map_insert,          0, 1    },
	{     "JUMP",             map_jump,            0, 0    },
	{     "LEAVE",            map_leave,           1, 1    },
	{     "LEGENDA",          map_legend,          1, 1    },
	{     "LINK",             map_link,            0, 1    },
	{     "LIST",             map_list,            1, 1    },
	{     "MAP",              map_map,             0, 0    },
	{     "MOVE",             map_move,            1, 1    },
	{     "NAME",             map_name,            1, 1    },
	{     "READ",             map_read,            1, 1    },
	{     "RETURN",           map_return,          0, 0    },
	{     "ROOMFLAG",         map_roomflag,        0, 1    },
	{     "RUN",              map_run,             0, 1    },
	{     "SET",              map_set,             0, 1    },
	{     "TRAVEL",           map_travel,          0, 1    },
	{     "UNDO",             map_undo,            1, 1    },
	{     "UNLINK",           map_unlink,          0, 1    },
	{     "WRITE",            map_write,           1, 1    },
	{     "",                 NULL,                1, 1    }
};

struct cursor_type cursor_table[] =
{
	{
		"AUTO TAB BACKWARD",
		"Tab completion from scrollback buffer, backward",
		"",
		cursor_auto_tab_backward
	},
	{
		"AUTO TAB FORWARD",
		"Tab completion from scrollback buffer, forward",
		"",
		cursor_auto_tab_forward
	},
	{
		"BACKSPACE",
		"Delete backward character",
		"",
		cursor_backspace
	},
	{
		"BACKWARD",
		"Move cursor backward",
		"",
		cursor_left
	},
	{
		"CLEAR LEFT",
		"Delete from cursor to start of input",
		"",
		cursor_clear_left
	},
	{
		"CLEAR LINE",
		"Delete the input line",
		"",
		cursor_clear_line
	},
	{
		"CLEAR RIGHT",
		"Delete from cursor to end of input",
		"",
		cursor_clear_right
	},
	{
		"CONVERT META",
		"Meta convert the next character",
		"",
		cursor_convert_meta
	},
	{
		"CTRL DELETE",
		"Delete one character, exit on an empty line",
		"",
		cursor_delete_exit
	},
	{
		"DELETE",
		"Delete character at cursor",
		"[3~",
		cursor_delete
	},
	{
		"DELETE WORD",
		"Delete next word backward",
		"",
		cursor_delete_word_left
	},
	{
		"ECHO",
		"Turn local echoing on or off",
		"",
		cursor_echo
	},

	{
		"END",
		"Move cursor to end of input",
		"",
		cursor_end
	},
	{
		"ENTER",
		"Process the input line",
		"",
		cursor_enter
	},
	{
		"EXIT",
		"Exit current session",
		"",
		cursor_exit
	},
	{
		"FORWARD",
		"Move cursor forward",
		"",
		cursor_right
	},
	{
		"HISTORY NEXT",
		"Select next command history entry",
		"",
		cursor_history_next
	},
	{
		"HISTORY PREV",
		"Select previous command history entry",
		"",
		cursor_history_prev
	},
	{
		"HISTORY SEARCH",
		"Search command history",
		"",
		cursor_history_search
	},
	{
		"HOME",
		"Move the cursor to start of input",
		"",
		cursor_home
	},
	{
		"INSERT",
		"Turn insert mode on or off",
		"",
		cursor_insert
	},
	{
		"MIXED TAB BACKWARD",
		"Tab completion on last word, search backward",
		"[Z",
		cursor_mixed_tab_backward
	},
	{
		"MIXED TAB FORWARD",
		"Tab completion on last word, search forward",
		"\t",
		cursor_mixed_tab_forward
	},
	{
		"NEXT WORD",
		"Move cursor to the next word",
		"f",
		cursor_right_word
	},
	{
		"PASTE BUFFER",
		"Paste the previously deleted input text",
		"",
		cursor_paste_buffer
	},
	{
		"PREV WORD",
		"Move cursor to the previous word",
		"b",
		cursor_left_word
	},
	{
		"REDRAW INPUT",
		"Redraw the input line",
		"",
		cursor_redraw_input
	},
	{
		"SUSPEND",
		"Suspend program, return with fg",
		"",
		cursor_suspend
	},
	{
		"TAB BACKWARD",
		"Tab completion from tab list, backward",
		"",
		cursor_tab_backward
	},
	{
		"TAB FORWARD",
		"Tab completion from tab list, forward",
		"",
		cursor_tab_forward
	},
	{
		"",
		"",
		"[5~",
		cursor_buffer_up
	},
	{
		"",
		"",
		"[6~",
		cursor_buffer_down
	},
	{
		"",
		"",
		"[H",
		cursor_buffer_home
	},
	{
		"",
		"",
		"[F",
		cursor_buffer_end
	},
	{
		"",
		"",
		"",
		cursor_buffer_lock
	},
	{
		"",
		"",
		"[7~",
		cursor_home
	},
	{
		"",
		"",
		"[1~",
		cursor_home
	},
	{
		"",
		"",
		"OH",
		cursor_home
	},
	{
		"",
		"",
		"OD",
		cursor_left
	},

	{
		"",
		"",
		"[8~",
		cursor_end
	},
	{
		"",
		"",
		"[4~",
		cursor_end
	},
	{
		"",
		"",
		"OF",
		cursor_end
	},
	{
		"",
		"",
		"OC",
		cursor_right
	},
	{
		"",
		"",
		"",
		cursor_backspace
	},
	{
		"",
		"",
		"OB",
		cursor_history_next
	},
	{
		"",
		"",
		"OA",
		cursor_history_prev
	},
	{
		"",
		"",
		"",
		cursor_delete_word_left
	},
	{
		"",
		"",
		"d",
		cursor_delete_word_right
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
	{    "END OF PATH",                            "Triggers when walking the last room."  },
	{    "IAC ",                                   "Triggers on telopt negotiation."       },
	{    "MAP ENTER ROOM",                         "Triggers when entering a map room."    },
	{    "PROGRAM TERMINATION",                    "Triggers when main session exists."    },
	{    "RECEIVED INPUT",                         "Triggers when new input is received."  },
	{    "RECEIVED LINE",                          "Triggers when a new line is received." },
	{    "SESSION CONNECTED",                      "Triggers when a new session connects." },
	{    "SESSION DISCONNECTED",                   "Triggers when a session disconnects."  },
	{    "",                                       ""                                      }
};

struct path_type path_table[] =
{
	{    "DELETE",            path_del               },
	{    "END",               path_end               },
	{    "INSERT",            path_ins               },
	{    "LOAD",              path_load              },
	{    "MAP",               path_map               },
	{    "NEW",               path_new               },
	{    "RUN",               path_run               },
	{    "SAVE",              path_save              },
	{    "WALK",              path_walk              },
	{    "ZIP",               path_zip               },
	{    "",                  NULL                   }
};

struct line_type line_table[] =
{
	{    "GAG",               line_gag               },
	{    "LOG",               line_log               },
	{    "LOGRAW",            line_lograw            },
	{    "",                  NULL                   }
};

struct history_type history_table[] =
{
/*	{    "CHARACTER",         history_character,   "Set the character used for repeating commands." }, */
	{    "DELETE",            history_delete,      "Delete last command history entry."             },
	{    "INSERT",            history_insert,      "Insert a new command history entry."            },
	{    "LIST",              history_list,        "Display command history list."                  },
	{    "READ",              history_read,        "Read a command history list from file."         },
/*	{    "SIZE",              history_size,        "The size of the command history."               }, */
	{    "WRITE",             history_write,       "Write a command history list to file."          },
	{    "",                  NULL,                ""                                               }
};

struct buffer_type buffer_table[] =
{
	{    "DOWN",              buffer_down,         "Scroll down one page."                          },
	{    "END",               buffer_end,          "Scroll down to the end of the buffer."          },
	{    "FIND",              buffer_find,         "Move to the given string in the buffer."        },
	{    "HOME",              buffer_home,         "Scroll up to the start of the buffer."          },
	{    "INFO",              buffer_info,         "Display statistics about the buffer."           },
	{    "LOCK",              buffer_lock,         "Toggle the locking state of the buffer."        },
	{    "UP",                buffer_up,           "Scroll up one page."                            },
	{    "WRITE",             buffer_write,        "Write the buffer to file."                      },
	{    "",                  NULL,                ""                                               }
};

#define NEG_U 0

struct telopt_type telopt_table[] =
{
	{    "BINARY",            TEL_N,               NEG_U },
	{    "ECHO",              TEL_Y,               NEG_U },
	{    "RCP",               TEL_N,               NEG_U },
	{    "SGA",               TEL_Y,               NEG_U },
	{    "NAME",              TEL_N,               NEG_U },
	{    "STATUS",            TEL_N,               NEG_U },
	{    "TIMING MARK",       TEL_N,               NEG_U },
	{    "RCTE",              TEL_N,               NEG_U },
	{    "NAOL",              TEL_N,               NEG_U },
	{    "NAOP",              TEL_N,               NEG_U },
	{    "NAORCD",            TEL_N,               NEG_U }, /* 10 */
	{    "NAOHTS",            TEL_N,               NEG_U },
	{    "NAOHTD",            TEL_N,               NEG_U },
	{    "NAOFFD",            TEL_N,               NEG_U },
	{    "NAOVTS",            TEL_N,               NEG_U },
	{    "NAOVTD",            TEL_N,               NEG_U },
	{    "NAOLFD",            TEL_N,               NEG_U },
	{    "EXTEND ASCII",      TEL_N,               NEG_U },
	{    "LOGOUT",            TEL_N,               NEG_U },
	{    "BYTE MACRO",        TEL_N,               NEG_U },
	{    "DATA ENTRY TERML",  TEL_N,               NEG_U }, /* 20 */
	{    "SUPDUP",            TEL_N,               NEG_U },
	{    "SUPDUP OUTPUT",     TEL_N,               NEG_U },
	{    "SEND LOCATION",     TEL_N,               NEG_U },
	{    "TTYPE",             TEL_Y,               NEG_U },
	{    "EOR",               TEL_Y,               NEG_U },
	{    "TACACS UID",        TEL_N,               NEG_U },
	{    "OUTPUT MARKING",    TEL_N,               NEG_U },
	{    "TTYLOC",            TEL_N,               NEG_U },
	{    "3270 REGIME",       TEL_N,               NEG_U },
	{    "X.3 PAD",           TEL_N,               NEG_U }, /* 30 */
	{    "NAWS",              TEL_Y,               NEG_U },
	{    "TSPEED",            TEL_Y,               NEG_U },
	{    "LFLOW",             TEL_N,               NEG_U },
	{    "LINEMODE",          TEL_N,               NEG_U },
	{    "XDISPLOC",          TEL_N,               NEG_U },
	{    "OLD-ENVIRON",       TEL_N,               NEG_U },
	{    "AUTH",              TEL_N,               NEG_U },
	{    "ENCRYPT",           TEL_N,               NEG_U },
	{    "NEW-ENVIRON",       TEL_N,               NEG_U },
	{    "TN3270E",           TEL_N,               NEG_U }, /* 40 */
	{    "XAUTH",             TEL_N,               NEG_U },
	{    "CHARSET",           TEL_N,               NEG_U },
	{    "RSP",               TEL_N,               NEG_U },
	{    "COM PORT",          TEL_N,               NEG_U },
	{    "SLE",               TEL_N,               NEG_U },
	{    "STARTTLS",          TEL_N,               NEG_U },
	{    "KERMIT",            TEL_N,               NEG_U },
	{    "SEND-URL",          TEL_N,               NEG_U },
	{    "FORWARD_X",         TEL_N,               NEG_U },
	{    "50",                TEL_N,               NEG_U },
	{    "51",                TEL_N,               NEG_U },
	{    "52",                TEL_N,               NEG_U },
	{    "53",                TEL_N,               NEG_U },
	{    "54",                TEL_N,               NEG_U },
	{    "55",                TEL_N,               NEG_U },
	{    "56",                TEL_N,               NEG_U },
	{    "57",                TEL_N,               NEG_U },
	{    "58",                TEL_N,               NEG_U },
	{    "59",                TEL_N,               NEG_U },
	{    "60",                TEL_N,               NEG_U },
	{    "61",                TEL_N,               NEG_U },
	{    "62",                TEL_N,               NEG_U },
	{    "63",                TEL_N,               NEG_U },
	{    "64",                TEL_N,               NEG_U },
	{    "65",                TEL_N,               NEG_U },
	{    "66",                TEL_N,               NEG_U },
	{    "67",                TEL_N,               NEG_U },
	{    "68",                TEL_N,               NEG_U },
	{    "MSDP",              TEL_N,               NEG_U },
	{    "MSSP",              TEL_N,               NEG_U },
	{    "71",                TEL_N,               NEG_U },
	{    "72",                TEL_N,               NEG_U },
	{    "73",                TEL_N,               NEG_U },
	{    "74",                TEL_N,               NEG_U },
	{    "75",                TEL_N,               NEG_U },
	{    "76",                TEL_N,               NEG_U },
	{    "77",                TEL_N,               NEG_U },
	{    "78",                TEL_N,               NEG_U },
	{    "79",                TEL_N,               NEG_U },
	{    "80",                TEL_N,               NEG_U },
	{    "81",                TEL_N,               NEG_U },
	{    "82",                TEL_N,               NEG_U },
	{    "83",                TEL_N,               NEG_U },
	{    "84",                TEL_N,               NEG_U },
	{    "MCCP1",             TEL_N,               NEG_U },
	{    "MCCP2",             TEL_Y,               NEG_U },
	{    "87",                TEL_N,               NEG_U },
	{    "88",                TEL_N,               NEG_U },
	{    "89",                TEL_N,               NEG_U },
	{    "MSP",               TEL_N,               NEG_U },
	{    "MXP",               TEL_N,               NEG_U },
	{    "92",                TEL_N,               NEG_U }, /* MSP2 draft */
	{    "ZMP",               TEL_N,               NEG_U },
	{    "94",                TEL_N,               NEG_U },
	{    "95",                TEL_N,               NEG_U },
	{    "96",                TEL_N,               NEG_U },
	{    "97",                TEL_N,               NEG_U },
	{    "98",                TEL_N,               NEG_U },
	{    "99",                TEL_N,               NEG_U },
	{    "100",               TEL_N,               NEG_U },
	{    "101",               TEL_N,               NEG_U },
	{    "102",               TEL_N,               NEG_U }, /* Used by Aardwolf */
	{    "103",               TEL_N,               NEG_U },
	{    "104",               TEL_N,               NEG_U },
	{    "105",               TEL_N,               NEG_U },
	{    "106",               TEL_N,               NEG_U },
	{    "107",               TEL_N,               NEG_U },
	{    "108",               TEL_N,               NEG_U },
	{    "109",               TEL_N,               NEG_U },
	{    "110",               TEL_N,               NEG_U },
	{    "111",               TEL_N,               NEG_U },
	{    "112",               TEL_N,               NEG_U },
	{    "113",               TEL_N,               NEG_U },
	{    "114",               TEL_N,               NEG_U },
	{    "115",               TEL_N,               NEG_U },
	{    "116",               TEL_N,               NEG_U },
	{    "117",               TEL_N,               NEG_U },
	{    "118",               TEL_N,               NEG_U },
	{    "119",               TEL_N,               NEG_U },
	{    "120",               TEL_N,               NEG_U },
	{    "121",               TEL_N,               NEG_U },
	{    "122",               TEL_N,               NEG_U },
	{    "123",               TEL_N,               NEG_U },
	{    "124",               TEL_N,               NEG_U },
	{    "125",               TEL_N,               NEG_U },
	{    "126",               TEL_N,               NEG_U },
	{    "127",               TEL_N,               NEG_U },
	{    "128",               TEL_N,               NEG_U },
	{    "129",               TEL_N,               NEG_U },
	{    "130",               TEL_N,               NEG_U },
	{    "131",               TEL_N,               NEG_U },
	{    "132",               TEL_N,               NEG_U },
	{    "133",               TEL_N,               NEG_U },
	{    "134",               TEL_N,               NEG_U },
	{    "135",               TEL_N,               NEG_U },
	{    "136",               TEL_N,               NEG_U },
	{    "137",               TEL_N,               NEG_U },
	{    "138",               TEL_N,               NEG_U },
	{    "139",               TEL_N,               NEG_U },
	{    "140",               TEL_N,               NEG_U },
	{    "141",               TEL_N,               NEG_U },
	{    "142",               TEL_N,               NEG_U },
	{    "143",               TEL_N,               NEG_U },
	{    "144",               TEL_N,               NEG_U },
	{    "145",               TEL_N,               NEG_U },
	{    "146",               TEL_N,               NEG_U },
	{    "147",               TEL_N,               NEG_U },
	{    "148",               TEL_N,               NEG_U },
	{    "149",               TEL_N,               NEG_U },
	{    "150",               TEL_N,               NEG_U },
	{    "151",               TEL_N,               NEG_U },
	{    "152",               TEL_N,               NEG_U },
	{    "153",               TEL_N,               NEG_U },
	{    "154",               TEL_N,               NEG_U },
	{    "155",               TEL_N,               NEG_U },
	{    "156",               TEL_N,               NEG_U },
	{    "157",               TEL_N,               NEG_U },
	{    "158",               TEL_N,               NEG_U },
	{    "159",               TEL_N,               NEG_U },
	{    "160",               TEL_N,               NEG_U },
	{    "161",               TEL_N,               NEG_U },
	{    "162",               TEL_N,               NEG_U },
	{    "163",               TEL_N,               NEG_U },
	{    "164",               TEL_N,               NEG_U },
	{    "165",               TEL_N,               NEG_U },
	{    "166",               TEL_N,               NEG_U },
	{    "167",               TEL_N,               NEG_U },
	{    "168",               TEL_N,               NEG_U },
	{    "169",               TEL_N,               NEG_U },
	{    "170",               TEL_N,               NEG_U },
	{    "171",               TEL_N,               NEG_U },
	{    "172",               TEL_N,               NEG_U },
	{    "173",               TEL_N,               NEG_U },
	{    "174",               TEL_N,               NEG_U },
	{    "175",               TEL_N,               NEG_U },
	{    "176",               TEL_N,               NEG_U },
	{    "177",               TEL_N,               NEG_U },
	{    "178",               TEL_N,               NEG_U },
	{    "179",               TEL_N,               NEG_U },
	{    "180",               TEL_N,               NEG_U },
	{    "181",               TEL_N,               NEG_U },
	{    "182",               TEL_N,               NEG_U },
	{    "183",               TEL_N,               NEG_U },
	{    "184",               TEL_N,               NEG_U },
	{    "185",               TEL_N,               NEG_U },
	{    "186",               TEL_N,               NEG_U },
	{    "187",               TEL_N,               NEG_U },
	{    "188",               TEL_N,               NEG_U },
	{    "189",               TEL_N,               NEG_U },
	{    "190",               TEL_N,               NEG_U },
	{    "191",               TEL_N,               NEG_U },
	{    "192",               TEL_N,               NEG_U },
	{    "193",               TEL_N,               NEG_U },
	{    "194",               TEL_N,               NEG_U },
	{    "195",               TEL_N,               NEG_U },
	{    "196",               TEL_N,               NEG_U },
	{    "197",               TEL_N,               NEG_U },
	{    "198",               TEL_N,               NEG_U },
	{    "199",               TEL_N,               NEG_U },
	{    "200",               TEL_N,               NEG_U }, /* Used by Achaea */
	{    "201",               TEL_N,               NEG_U },
	{    "202",               TEL_N,               NEG_U },
	{    "203",               TEL_N,               NEG_U },
	{    "204",               TEL_N,               NEG_U },
	{    "205",               TEL_N,               NEG_U },
	{    "206",               TEL_N,               NEG_U },
	{    "207",               TEL_N,               NEG_U },
	{    "208",               TEL_N,               NEG_U },
	{    "209",               TEL_N,               NEG_U },
	{    "210",               TEL_N,               NEG_U },
	{    "211",               TEL_N,               NEG_U },
	{    "212",               TEL_N,               NEG_U },
	{    "213",               TEL_N,               NEG_U },
	{    "214",               TEL_N,               NEG_U },
	{    "215",               TEL_N,               NEG_U },
	{    "216",               TEL_N,               NEG_U },
	{    "217",               TEL_N,               NEG_U },
	{    "218",               TEL_N,               NEG_U },
	{    "219",               TEL_N,               NEG_U },
	{    "220",               TEL_N,               NEG_U },
	{    "221",               TEL_N,               NEG_U },
	{    "222",               TEL_N,               NEG_U },
	{    "223",               TEL_N,               NEG_U },
	{    "224",               TEL_N,               NEG_U },
	{    "225",               TEL_N,               NEG_U },
	{    "226",               TEL_N,               NEG_U },
	{    "227",               TEL_N,               NEG_U },
	{    "228",               TEL_N,               NEG_U },
	{    "229",               TEL_N,               NEG_U },
	{    "230",               TEL_N,               NEG_U },
	{    "231",               TEL_N,               NEG_U },
	{    "232",               TEL_N,               NEG_U },
	{    "233",               TEL_N,               NEG_U },
	{    "234",               TEL_N,               NEG_U },
	{    "235",               TEL_N,               NEG_U },
	{    "236",               TEL_N,               NEG_U },
	{    "237",               TEL_N,               NEG_U },
	{    "238",               TEL_N,               NEG_U },
	{    "239",               TEL_N,               NEG_U },
	{    "240",               TEL_N,               NEG_U },
	{    "241",               TEL_N,               NEG_U },
	{    "242",               TEL_N,               NEG_U },
	{    "243",               TEL_N,               NEG_U },
	{    "244",               TEL_N,               NEG_U },
	{    "245",               TEL_N,               NEG_U },
	{    "246",               TEL_N,               NEG_U },
	{    "247",               TEL_N,               NEG_U },
	{    "248",               TEL_N,               NEG_U },
	{    "249",               TEL_N,               NEG_U },
	{    "250",               TEL_N,               NEG_U },
	{    "251",               TEL_N,               NEG_U },
	{    "252",               TEL_N,               NEG_U },
	{    "253",               TEL_N,               NEG_U },
	{    "254",               TEL_N,               NEG_U },
	{    "255",               TEL_N,               NEG_U }
};
