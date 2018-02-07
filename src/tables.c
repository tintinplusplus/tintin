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
	{    "action",            do_action,            TOKEN_TYPE_COMMAND },
	{    "advertise",         do_advertise,         TOKEN_TYPE_COMMAND },
	{    "alias",             do_alias,             TOKEN_TYPE_COMMAND },
	{    "all",               do_all,               TOKEN_TYPE_COMMAND },
	{    "bell",              do_bell,              TOKEN_TYPE_COMMAND },
	{    "break",             do_nop,               TOKEN_TYPE_BREAK   },
	{    "buffer",            do_buffer,            TOKEN_TYPE_COMMAND },
	{    "case",              do_nop,               TOKEN_TYPE_CASE    },
	{    "chat",              do_chat,              TOKEN_TYPE_COMMAND },
	{    "class",             do_class,             TOKEN_TYPE_COMMAND },
	{    "commands",          do_commands,          TOKEN_TYPE_COMMAND },
	{    "config",            do_configure,         TOKEN_TYPE_COMMAND },
	{    "continue",          do_nop,               TOKEN_TYPE_CONTINUE},
	{    "cr",                do_cr,                TOKEN_TYPE_COMMAND },
	{    "cursor",            do_cursor,            TOKEN_TYPE_COMMAND },
	{    "debug",             do_debug,             TOKEN_TYPE_COMMAND },
	{    "default",           do_nop,               TOKEN_TYPE_DEFAULT },
	{    "delay",             do_delay,             TOKEN_TYPE_COMMAND },
	{    "echo",              do_echo,              TOKEN_TYPE_COMMAND },
	{    "else",              do_nop,               TOKEN_TYPE_ELSE    },
	{    "elseif",            do_nop,               TOKEN_TYPE_ELSEIF  },
	{    "end",               do_end,               TOKEN_TYPE_COMMAND },
	{    "event",             do_event,             TOKEN_TYPE_COMMAND },
	{    "forall",            do_forall,            TOKEN_TYPE_COMMAND },
	{    "foreach",           do_nop,               TOKEN_TYPE_FOREACH },
	{    "format",            do_format,            TOKEN_TYPE_COMMAND },
	{    "function",          do_function,          TOKEN_TYPE_COMMAND },
	{    "gag",               do_gag,               TOKEN_TYPE_COMMAND },
	{    "grep",              do_grep,              TOKEN_TYPE_COMMAND },
	{    "help",              do_help,              TOKEN_TYPE_COMMAND },
	{    "highlight",         do_highlight,         TOKEN_TYPE_COMMAND },
	{    "history",           do_history,           TOKEN_TYPE_COMMAND },
	{    "if",                do_nop,               TOKEN_TYPE_IF      },
	{    "ignore",            do_ignore,            TOKEN_TYPE_COMMAND },
	{    "info",              do_info,              TOKEN_TYPE_COMMAND },
	{    "killall",           do_kill,              TOKEN_TYPE_COMMAND },
	{    "line",              do_line,              TOKEN_TYPE_COMMAND },
	{    "list",              do_list,              TOKEN_TYPE_COMMAND },
	{    "local",             do_local,             TOKEN_TYPE_COMMAND },
	{    "log",               do_log,               TOKEN_TYPE_COMMAND },
	{    "loop",              do_nop,               TOKEN_TYPE_LOOP    },
	{    "macro",             do_macro,             TOKEN_TYPE_COMMAND },
	{    "map",               do_map,               TOKEN_TYPE_COMMAND },
	{    "math",              do_math,              TOKEN_TYPE_COMMAND },
	{    "message",           do_message,           TOKEN_TYPE_COMMAND },
	{    "nop",               do_nop,               TOKEN_TYPE_COMMAND },
	{    "parse",             do_nop,               TOKEN_TYPE_PARSE   },
	{    "path",              do_path,              TOKEN_TYPE_COMMAND },
	{    "pathdir",           do_pathdir,           TOKEN_TYPE_COMMAND },
	{    "port",              do_port,              TOKEN_TYPE_COMMAND },
	{    "prompt",            do_prompt,            TOKEN_TYPE_COMMAND },
	{    "read",              do_read,              TOKEN_TYPE_COMMAND },
	{    "regexp",            do_regexp,            TOKEN_TYPE_REGEX   },
	{    "replace",           do_replace,           TOKEN_TYPE_COMMAND },
	{    "return",            do_nop,               TOKEN_TYPE_RETURN  },
	{    "run",               do_run,               TOKEN_TYPE_COMMAND },
	{    "scan",              do_scan,              TOKEN_TYPE_COMMAND },
	{    "script",            do_script,            TOKEN_TYPE_COMMAND },
	{    "send",              do_send,              TOKEN_TYPE_COMMAND },
	{    "session",           do_session,           TOKEN_TYPE_COMMAND },
	{    "showme",            do_showme,            TOKEN_TYPE_COMMAND },
	{    "snoop",             do_snoop,             TOKEN_TYPE_COMMAND },
	{    "split",             do_split,             TOKEN_TYPE_COMMAND },
	{    "ssl",               do_ssl,               TOKEN_TYPE_COMMAND },
	{    "substitute",        do_substitute,        TOKEN_TYPE_COMMAND },
	{    "switch",            do_nop,               TOKEN_TYPE_SWITCH  },
	{    "system",            do_system,            TOKEN_TYPE_COMMAND },
	{    "tab",               do_tab,               TOKEN_TYPE_COMMAND },
	{    "test",              do_test,              TOKEN_TYPE_COMMAND },
	{    "textin",            do_textin,            TOKEN_TYPE_COMMAND },
	{    "ticker",            do_tick,              TOKEN_TYPE_COMMAND },
	{    "unaction",          do_unaction,          TOKEN_TYPE_COMMAND },
	{    "unalias",           do_unalias,           TOKEN_TYPE_COMMAND },
	{    "undelay",           do_undelay,           TOKEN_TYPE_COMMAND },
	{    "unevent",           do_unevent,           TOKEN_TYPE_COMMAND },
	{    "unfunction",        do_unfunction,        TOKEN_TYPE_COMMAND },
	{    "ungag",             do_ungag,             TOKEN_TYPE_COMMAND },
	{    "unhighlight",       do_unhighlight,       TOKEN_TYPE_COMMAND },
	{    "unmacro",           do_unmacro,           TOKEN_TYPE_COMMAND },
	{    "unpathdir",         do_unpathdir,         TOKEN_TYPE_COMMAND },
	{    "unprompt",          do_unprompt,          TOKEN_TYPE_COMMAND },
	{    "unsplit",           do_unsplit,           TOKEN_TYPE_COMMAND },
	{    "unsubstitute",      do_unsubstitute,      TOKEN_TYPE_COMMAND },
	{    "untab",             do_untab,             TOKEN_TYPE_COMMAND },
	{    "unticker",          do_untick,            TOKEN_TYPE_COMMAND },
	{    "unvariable",        do_unvariable,        TOKEN_TYPE_COMMAND },
	{    "variable",          do_variable,          TOKEN_TYPE_COMMAND },
	{    "while",             do_nop,               TOKEN_TYPE_WHILE   },
	{    "write",             do_write,             TOKEN_TYPE_COMMAND },
	{    "zap",               do_zap,               TOKEN_TYPE_COMMAND },
	{    "",                  NULL,                 TOKEN_TYPE_COMMAND }
};


struct list_type list_table[LIST_MAX] =
{
	{    "ACTION",            "ACTIONS",            PRIORITY,    3,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "ALIAS",             "ALIASES",            PRIORITY,    3,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "CLASS",             "CLASSES",            ALPHA,       2,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_INHERIT                                 },
	{    "COMMAND",           "COMMANDS",           APPEND,      1,  LIST_FLAG_MESSAGE                                                                  },
	{    "CONFIG",            "CONFIGURATIONS",     ALPHA,       2,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_INHERIT                 },
	{    "DELAY",             "DELAYS",             ALPHA,       3,  LIST_FLAG_MESSAGE|LIST_FLAG_READ                                                   },
	{    "EVENT",             "EVENTS",             ALPHA,       2,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "FUNCTION",          "FUNCTIONS",          ALPHA,       2,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "GAG",               "GAGS",               ALPHA,       1,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "HIGHLIGHT",         "HIGHLIGHTS",         PRIORITY,    3,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "HISTORY",           "HISTORIES",          APPEND,      1,  LIST_FLAG_MESSAGE                                                                  },
	{    "MACRO",             "MACROS",             ALPHA,       2,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "PATH",              "PATHS",              APPEND,      2,  LIST_FLAG_MESSAGE                                                                  },
	{    "PATHDIR",           "PATHDIRS",           ALPHA,       3,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "PROMPT",            "PROMPTS",            PRIORITY,    3,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "SUBSTITUTE",        "SUBSTITUTIONS",      PRIORITY,    3,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "TAB",               "TABS",               ALPHA,       1,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
//	{    "TABCYCLE",          "TABCYCLE",           APPEND,      1,  LIST_FLAG_MESSAGE|LIST_FLAG_HIDE                                                   },
	{    "TICKER",            "TICKERS",            ALPHA,       3,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT },
	{    "VARIABLE",          "VARIABLES",          ALPHA,       2,  LIST_FLAG_MESSAGE|LIST_FLAG_READ|LIST_FLAG_WRITE|LIST_FLAG_CLASS|LIST_FLAG_INHERIT|LIST_FLAG_NEST }
};

struct substitution_type substitution_table[] =
{
//	{    "ARGUMENTS",            1  },
	{    "VARIABLES",            2  },
	{    "FUNCTIONS",            4  },
	{    "COLORS",               8  },
	{    "ESCAPES",             16  },
//	{    "COMMANDS",            32  },
	{    "SECURE",              64  },
	{    "EOL",                128  },
	{    "LNF",                256  },
//	{    "FIX",               1024  },
	{    "",                  0     }
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
		"CHARSET",
		"",
		"The character set encoding used by TinTin++",
		config_charset
	},

	{
		"COLOR PATCH",
		"TinTin++ will properly color the start of each line",
		"TinTin++ will leave color handling to the server",
		config_colorpatch
	},

	{
		"CONNECT RETRY",
		"",
		"Seconds TinTin++ sessions try to connect on failure.",
		config_connectretry
	},

	{
		"CONVERT META",
		"TinTin++ converts meta prefixed characters",
		"TinTin++ doesn't convert meta prefixed characters",
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
		"TinTin++ only logs low level mud data",
		"TinTin++ only logs high level mud data",
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
		"Seconds TinTin++ will try to patch broken packets",
		config_packetpatch
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
		"The character used for TinTin++ commands",
		config_tintinchar
	},

	{
		"VERBATIM",
		"Your keyboard input isn't modified by TinTin++",
		"Your keyboard input is parsed by TinTin++",
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
		"256 COLORS",
		"Your terminal is 256 color capable",
		"Your terminal is not 256 color capable",
		config_256color
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

	{    "azure",         "<abd>" },
	{    "ebony",         "<g04>" },
	{    "jade",          "<adb>" },
	{    "lime",          "<bda>" },
	{    "orange",        "<dba>" },
	{    "pink",          "<dab>" },
	{    "silver",        "<ccc>" },
	{    "tan",           "<cba>" },
	{    "violet",        "<bad>" },

	{    "light azure",   "<acf>" },
	{    "light ebony",   "<bbb>" },
	{    "light jade",    "<afc>" },
	{    "light lime",    "<cfa>" },
	{    "light orange",  "<fca>" },
	{    "light pink",    "<fac>" },
	{    "light silver",  "<eee>" },
	{    "light tan",     "<eda>" },
	{    "light violet",  "<caf>" },

	{    "reset",         "<088>" },
	{    "light",         "<188>" },
	{    "bold",          "<188>" },
	{    "faint",         "<288>" },
	{    "dim",           "<288>" },
	{    "dark",          "<288>" },
	{    "underscore",    "<488>" },
	{    "blink",         "<588>" },
	{    "reverse",       "<788>" },

	{    "no-underscore", "\e[24m"},
	{    "no-blink",      "\e[25m"},
	{    "no-reverse",    "\e[27m"},
		
	{    "black",         "<808>" },
	{    "red",           "<818>" },
	{    "green",         "<828>" },
	{    "yellow",        "<838>" },
	{    "blue",          "<848>" },
	{    "magenta",       "<858>" },
	{    "cyan",          "<868>" },
	{    "white",         "<878>" },

	{    "b black",       "<880>" },
	{    "b red",         "<881>" },
	{    "b green",       "<882>" },
	{    "b yellow",      "<883>" },
	{    "b blue",        "<884>" },
	{    "b magenta",     "<885>" },
	{    "b cyan",        "<886>" },
	{    "b white",       "<887>" },

	{    "b azure",       "<ABD>" },
	{    "b ebony",       "<G04>" },
	{    "b jade",        "<ADB>" },
	{    "b lime",        "<BDA>" },
	{    "b orange",      "<DBA>" },
	{    "b pink",        "<DAB>" },
	{    "b silver",      "<CCC>" },
	{    "b tan",         "<CBA>" },
	{    "b violet",      "<BAD>" },

	{    "",              "<888>" }
};

struct class_type class_table[] =
{
	{    "OPEN",              class_open             },
	{    "CLOSE",             class_close            },
	{    "LIST",              class_list             },
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
	{     "PREFIX",           chat_prefix,         1, 0, "Prefix before each chat message"                },
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

struct port_type port_table[] =
{
	{     "CALL",             port_call,           0, 0, "Create outgoing socket connection"              },
	{     "COLOR",            port_color,          1, 0, "Set the default port message color"             },
	{     "DND",              port_dnd,            0, 0, "Decline new connections"                        },
	{     "GROUP",            port_group,          0, 1, "Assign a group to a socket"                     },
	{     "IGNORE",           port_ignore,         1, 0, "Ignore all messages from a socket"              },
	{     "INITIALIZE",       port_initialize,     0, 0, "Initialize port with optional file name"        },
	{     "INFO",             port_info,           0, 0, "Display the port settings"                      },
	{     "NAME",             port_name,           0, 0, "Change a socket name"                           },
	{     "PREFIX",           port_prefix,         1, 0, "Prefix before each port message"                },
	{     "SEND",             port_send,           0, 1, "Send a message to a socket"                     },
	{     "UNINITIALIZE",     port_uninitialize,   0, 0, "Uninitializes the port"                         },
	{     "WHO",              port_who,            0, 0, "Show all socket connections"                    },
	{     "ZAP",              port_zap,            1, 0, "Close the connection to a socket"               },
	{     "",                 NULL,                0, 0, ""                                               }
};

struct array_type array_table[] =
{
	{     "ADD",              array_add           },
	{     "CLEAR",            array_clear         },
	{     "CLR",              array_clear         },
	{     "CREATE",           array_create        },
	{     "DELETE",           array_delete        },
	{     "FIND",             array_find          },
	{     "FND",              array_find          },
	{     "GET",              array_get           },
	{     "INSERT",           array_insert        },
	{     "LENGTH",           array_size          },
	{     "SET",              array_set           },
	{     "SIZE",             array_size          },
	{     "SORT",             array_sort          },
	{     "SRT",              array_sort          },
	{     "TOKENIZE",         array_tokenize      },
	{     "",                 NULL                }
};

struct map_type map_table[] =
{
	{     "AT",               map_at,              1    },
	{     "COLOR",            map_color,           1    },
	{     "CREATE",           map_create,          0    },
	{     "DEBUG",            map_debug,           2    },
	{     "DELETE",           map_delete,          1    },
	{     "DESTROY",          map_destroy,         1    },
	{     "DIG",              map_dig,             2    },
	{     "EXIT",             map_exit,            2    },
	{     "EXITFLAG",         map_exitflag,        2    },
	{     "EXPLORE",          map_explore,         2    },
	{     "FIND",             map_find,            2    },
	{     "FLAG",             map_flag,            1    },
	{     "GET",              map_get,             2    },
	{     "GOTO",             map_goto,            1    },
	{     "INFO",             map_info,            1    },
	{     "INSERT",           map_insert,          2    },
	{     "JUMP",             map_jump,            2    },
	{     "LEAVE",            map_leave,           2    },
	{     "LEGENDA",          map_legend,          1    },
	{     "LINK",             map_link,            2    },
	{     "LIST",             map_list,            2    },
	{     "MAP",              map_map,             2    },
	{     "MOVE",             map_move,            2    },
	{     "NAME",             map_name,            2    },
	{     "READ",             map_read,            0    },
	{     "RESIZE",           map_resize,          1    },
	{     "RETURN",           map_return,          1    },
	{     "ROOMFLAG",         map_roomflag,        2    },
	{     "RUN",              map_run,             2    },
	{     "SET",              map_set,             2    },
	{     "TRAVEL",           map_travel,          2    },
	{     "UNDO",             map_undo,            2    },
	{     "UNINSERT",         map_uninsert,        2    },
	{     "UNLINK",           map_unlink,          2    },
	{     "VNUM",             map_vnum,            2    },
	{     "WRITE",            map_write,           1    },
	{     "",                 NULL,                0    }
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
		cursor_delete_or_exit
	},
	{
		"DELETE",
		"Delete character at cursor",
		"[3~",
		cursor_delete
	},
	{
		"DELETE WORD LEFT",
		"Delete backwards till next space",
		"",
		cursor_delete_word_left
	},
	{
		"DELETE WORD RIGHT",
		"Delete forwards till next space",
		"",
		cursor_delete_word_right
	},
	{
		"ECHO ON",
		"Turn local echoing on",
		"",
		cursor_echo_on
	},
	{
		"ECHO OFF",
		"Turn local echoing off",
		"",
		cursor_echo_off
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
		"GET",
		"Copy input line to given variable",
		"",
		cursor_get
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
		"[Z", // shift-tab
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
		"SET",
		"Copy given string to input line",
		"",
		cursor_set
	},
	{
		"SUSPEND",
		"Suspend program, return with fg",
		"",
		cursor_suspend
	},
	{
		"TEST",
		"Print debugging information",
		"",
		cursor_test
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
		"", "", "[5~",   cursor_buffer_up
	},
	{
		"", "", "[6~",   cursor_buffer_down
	},

	{
		"", "", "",      cursor_buffer_lock
	},
	{
		"", "", "OM",    cursor_enter
	},
	{
		"", "", "[7~",   cursor_home
	},
	{
		"", "", "[1~",   cursor_home
	},
	{
		"", "", "OH",    cursor_home
	},
	{
		"", "", "[H",    cursor_home
	},
	{
		"", "", "OD",    cursor_left
	},
	{
		"", "", "[D",    cursor_left
	},
	{
		"", "", "[8~",   cursor_end
	},
	{
		"", "", "[4~",   cursor_end
	},
	{
		"", "", "OF",    cursor_end
	},
	{
		"", "", "[F",    cursor_end
	},
	{
		"", "", "OC",    cursor_right
	},
	{
		"", "", "[C",    cursor_right
	},
	{
		"", "", "",      cursor_backspace
	},
	{
		"", "", "OB",    cursor_history_next
	},
	{
		"", "", "[B",    cursor_history_next
	},
	{
		"", "", "OA",    cursor_history_prev
	},
	{
		"", "", "[A",    cursor_history_prev
	},
	{
		"", "", "",     cursor_delete_word_left
	},
	{
		"", "", "d",     cursor_delete_word_right
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
	{    "Poll Port Sessions"          },
	{    "Update Tickers"              },
	{    "Update Delays"               },
	{    "Update Packet Patcher"       },
	{    "Update Chat Server"          },
	{    "Update Terminal"             },
	{    "Update Time Events"          },
	{    "Update Memory"               },
	{    "Stall Program"               }
};

struct event_type event_table[] =
{
	{    "CHAT MESSAGE",                           "Triggers on any chat related message."   },
	{    "DATE",                                   "Triggers on the given date."             },
	{    "DAY",                                    "Triggers each day or given day."         },
	{    "HOUR",                                   "Triggers each hour or given hour."       },
	{    "END OF PATH",                            "Triggers when walking the last room."    },
	{    "IAC ",                                   "Triggers on telopt negotiation."         },
	{    "MAP ENTER MAP",                          "Triggers when entering the map."         },
	{    "MAP ENTER ROOM",                         "Triggers when entering a map room."      },
	{    "MAP EXIT MAP",                           "Triggers when exiting the map."          },
	{    "MAP EXIT ROOM",                          "Triggers when exiting a map room."       },
	{    "MINUTE",                                 "Triggers each minute or given minute."   },
	{    "MONTH",                                  "Triggers each month or given month."     },
	{    "PORT CONNECTION",                        "Triggers when socket connects."          },
	{    "PORT DISCONNECTION",                     "Triggers when socket disconnects."       },
	{    "PORT MESSAGE",                           "Triggers when socket data is received."  },
	{    "PROGRAM START",                          "Triggers when main session starts."      },
	{    "PROGRAM TERMINATION",                    "Triggers when main session exists."      },
	{    "RECEIVED INPUT",                         "Triggers when new input is received."    },
	{    "RECEIVED LINE",                          "Triggers when a new line is received."   },
	{    "RECEIVED OUTPUT",                        "Triggers when new output is received."   },
	{    "RECEIVED PROMPT",                        "Triggers when a prompt is received."     },
	{    "SCREEN RESIZE",                          "Triggers when the screen is resized."    },
	{    "SECOND",                                 "Trigers each second or given second."    },
	{    "SEND OUTPUT",                            "Triggers when sending output."           },
	{    "SESSION ACTIVATED",                      "Triggers when a session is activated."   },
	{    "SESSION CONNECTED",                      "Triggers when a new session connects."   },
	{    "SESSION CREATED",                        "Triggers when a new session is created." },
	{    "SESSION DEACTIVATED",                    "Triggers when a session is deactivated." },
	{    "SESSION DISCONNECTED",                   "Triggers when a session disconnects."    },
	{    "SESSION TIMED OUT",                      "Triggers when a session doesn't connect."},
	{    "TIME",                                   "Triggers on the given time."             },
	{    "VARIABLE UPDATE ",                       "Triggers on a variable update."          },
	{    "VT100 CPR",                              "Triggers on an ESC [ 6 n call."          },
	{    "VT100 DA",                               "Triggers on an ESC [ c call."            },
	{    "VT100 DECID",                            "Triggers on an ESC Z call."              },
	{    "VT100 DSR",                              "Triggers on an ESC [ 5 n call."          },
	{    "WEEK",                                   "Triggers each week or given week."       },
	{    "YEAR",                                   "Triggers each year or given year."       },
	{    "",                                       ""                                        }
};

struct path_type path_table[] =
{
	{    "DELETE",            path_del               },
	{    "END",               path_end               },
	{    "INSERT",            path_ins               },
	{    "LOAD",              path_load              },
	{    "MAP",               path_show              },
	{    "NEW",               path_new               },
	{    "RUN",               path_run               },
	{    "SAVE",              path_save              },
	{    "SHOW",              path_show              },
	{    "UNZIP",             path_unzip             },
	{    "WALK",              path_walk              },
	{    "ZIP",               path_zip               },
	{    "",                  NULL                   }
};

struct line_type line_table[] =
{
	{    "GAG",               line_gag               },
	{    "IGNORE",            line_ignore            },
	{    "LOG",               line_log               },
	{    "LOGVERBATIM",       line_logverbatim       },
	{    "QUIET",             line_quiet             },
	{    "STRIP",             line_strip             },
	{    "SUBSTITUTE",        line_substitute        },
	{    "VERBOSE",           line_verbose           },
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
	{    "CLEAR",             buffer_clear,        "Clear buffer."                                  },
	{    "DOWN",              buffer_down,         "Scroll down one page."                          },
	{    "END",               buffer_end,          "Scroll down to the end of the buffer."          },
	{    "FIND",              buffer_find,         "Move to the given string in the buffer."        },
	{    "GET",               buffer_get,          "Store in given variable a given line or range." },
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
	{    "MSDP",              TEL_N,               NEG_U }, /* Mud Server Data Protocol */
	{    "MSSP",              TEL_N,               NEG_U }, /* Mud Server Status Protocol */
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
	{    "MCCP1",             TEL_N,               NEG_U }, /* Obsolete */
	{    "MCCP2",             TEL_Y,               NEG_U }, /* Mud Client Compression Protocol */
	{    "87",                TEL_N,               NEG_U },
	{    "88",                TEL_N,               NEG_U },
	{    "89",                TEL_N,               NEG_U },
	{    "MSP",               TEL_N,               NEG_U }, /* Mud Sound Protocl */
	{    "MXP",               TEL_N,               NEG_U }, /* Mud eXtension Protocol */
	{    "92",                TEL_N,               NEG_U }, /* MSP2 draft */
	{    "ZMP",               TEL_N,               NEG_U }, /* Zenith Mud Protocl */
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
	{    "GMCP",              TEL_N,               NEG_U }, /* Generic Mud Communication Protocol */
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

struct term_type term_table[] =
{
	{    "UNKNOWN",               3 }, // ANSI + VT100
	{    "XTERM",                59 }
};
