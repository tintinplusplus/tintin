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
*   file: tables.c - various reference tables to make life easier             *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                     coded by Igor van den Hoven 2004                        *
******************************************************************************/


#include "tintin.h"


const struct command_type command_table[MAX_COMMAND] =
{
	{	"action",            do_action,            CMD_FLAG_NONE    },
	{	"alias",             do_alias,             CMD_FLAG_NONE    },
	{	"all",               do_all,               CMD_FLAG_SUB     },
	{	"bell",              do_bell,              CMD_FLAG_NONE    },
	{	"boss",              do_boss,              CMD_FLAG_NONE    },

	{	"buffer",            do_buffer,            CMD_FLAG_SUB     },
	{	"class",             do_class,             CMD_FLAG_SUB     },
	{	"commands",          do_commands,          CMD_FLAG_NONE    },
	{	"config",            do_configure,         CMD_FLAG_SUB     },
	{	"cr",                do_cr,                CMD_FLAG_NONE    },
	{	"debug",             do_debug,             CMD_FLAG_SUB     },
	{	"delay",             do_delay,             CMD_FLAG_NONE    },
	{	"echo",              do_echo,              CMD_FLAG_SUB     },
	{	"end",               do_end,               CMD_FLAG_NONE    },
	{	"forall",            do_forall,            CMD_FLAG_NONE    },
	{	"format",            do_format,            CMD_FLAG_SUB     },
	{	"function",          do_function,          CMD_FLAG_NONE    },
	{	"gag",               do_gag,               CMD_FLAG_NONE    },
	{	"gagline",           do_gagline,           CMD_FLAG_NONE    },
	{	"getitemnr",         do_getitemnr,         CMD_FLAG_SUB     },
	{	"getlistlength",     do_getlistlength,     CMD_FLAG_SUB     },
	{	"grep",              do_grep,              CMD_FLAG_SUB     },
	{	"help",              do_help,              CMD_FLAG_NONE    },
	{	"highlight",         do_highlight,         CMD_FLAG_NONE    },
	{	"if",                do_if,                CMD_FLAG_NONE    },
	{	"ignore",            do_ignore,            CMD_FLAG_SUB     },
	{	"info",              do_info,              CMD_FLAG_NONE    },
	{	"killall",           do_killall,           CMD_FLAG_NONE    },
	{	"loadpath",          do_loadpath,          CMD_FLAG_SUB     },
	{	"log",               do_log,               CMD_FLAG_SUB     },
	{	"logline",           do_logline,           CMD_FLAG_SUB     },
	{	"loop",              do_loop,              CMD_FLAG_NONE    },
	{	"macro",             do_macro,             CMD_FLAG_NONE    },
	{	"map",               do_map,               CMD_FLAG_NONE    },
	{	"math",              do_math,              CMD_FLAG_SUB     },
	{	"mark",              do_mark,              CMD_FLAG_NONE    },
	{	"message",           do_message,           CMD_FLAG_SUB     },
	{	"nop",               do_nop,               CMD_FLAG_NONE    },
	{	"path",              do_path,              CMD_FLAG_NONE    },
	{	"pathdir",           do_pathdir,           CMD_FLAG_NONE    },
	{	"prompt",            do_prompt,            CMD_FLAG_NONE    },
	{	"read",              do_read,              CMD_FLAG_SUB     },
	{	"readmap",           do_readmap,           CMD_FLAG_SUB     },
	{	"redit",             do_redit,             CMD_FLAG_SUB     },
	{	"removestring",      do_removestring,      CMD_FLAG_NONE    },
	{	"replacestring",     do_replacestring,     CMD_FLAG_SUB     },
	{	"return",            do_return,            CMD_FLAG_NONE    },
	{	"savepath",          do_savepath,          CMD_FLAG_SUB     },
	{	"scan",              do_scan,              CMD_FLAG_SUB     },
	{	"session",           do_session,           CMD_FLAG_SUB     },
	{	"showme",            do_showme,            CMD_FLAG_NONE    },
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
	{	"unpath",            do_unpath,            CMD_FLAG_NONE    },
	{	"unprompt",          do_unprompt,          CMD_FLAG_NONE    },
	{	"unsplit",           do_unsplit,           CMD_FLAG_NONE    },
	{	"unsubstitute",      do_unsubstitute,      CMD_FLAG_NONE    },
	{	"untab",             do_untab,             CMD_FLAG_NONE    },
	{	"unticker",          do_untick,            CMD_FLAG_NONE    },
	{	"unvariable",        do_unvariable,        CMD_FLAG_NONE    },
	{	"variable",          do_variable,          CMD_FLAG_NONE    },
	{	"walk",              do_walk,              CMD_FLAG_NONE    },
	{	"write",             do_write,             CMD_FLAG_SUB     },
	{	"writemap",          do_writemap,          CMD_FLAG_SUB     },
	{	"writesession",      do_writesession,      CMD_FLAG_SUB     },
	{	"zap",               do_zap,               CMD_FLAG_NONE    },
	{	"",                  NULL,                 CMD_FLAG_NONE    }
};


const struct list_type list_table[LIST_ALL] =
{
	{
		"CONFIG",
		"CONFIGURATIONS",
		PRIORITY,
		2
	},

	{
		"ALIAS",
		"ALIASES",
		ALPHA,
		2

	},
	{
		"ACTION",
		"ACTIONS",
		PRIORITY,
		3
	},
	{
		"SUBSTITUTE",
		"SUBSTITUTIONS",
		PRIORITY,
		2
	},
	{
		"VARIABLE",
		"VARIABLES",
		ALPHA,
		2
	},
	{
		"HIGHLIGHT",
		"HIGHLIGHTS",
		PRIORITY,
		3
	},
	{
		"ANTISUBSTITUTE",
		"ANTISUBSTITUTIONS",
		ALPHA,
		1
	},
	{
		"FUNCTION",
		"FUNCTIONS",
		ALPHA,
		2
	},
	{
		"PATH",
		"PATHS",
		APPEND,
		0
	},
	{
		"PATHDIR",
		"PATHDIRS",
		ALPHA,
		2
	},
	{
		"TICKER",
		"TICKERS",
		PRIORITY,
		3
	},

	{
		"MACRO",
		"MACROS",
		ALPHA,
		2
	},

	{
		"PROMPT",
		"PROMPTS",
		PRIORITY,
		3
	},

	{
		"TAB",
		"TABS",
		ALPHA,
		1
	},

	{
		"CLASS",
		"CLASSES",
		APPEND,
		2
	},

	{
		"MATH",
		"MATHEMATICS",
		APPEND,
		0
	},

	{
		"DELAY",
		"DELAYS",
		PRIORITY,
		3
	}
};


const struct config_type config_table[CONFIG_MAX] =
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
	}
};

const struct color_type color_table[COLOR_MAX] =
{
	{	"reset",          "0", "<088>" },
	{	"light",          "1", "<188>" },
	{	"faint",          "2", "<288>" },
	{	"underscore",     "4", "<488>" },
	{	"blink",          "5", "<588>" },
	{	"reverse",        "7", "<788>" },
	{    "dim",           "22", "<288>" },
	{	"black",         "30", "<808>" },
	{	"red",           "31", "<818>" },
	{    "green",         "32", "<828>" },
	{    "yellow",        "33", "<838>" },
	{    "blue",          "34", "<848>" },
	{	"magenta",       "35", "<858>" },
	{    "cyan",          "36", "<868>" },
	{    "white",         "37", "<878>" },
	{    "b black",       "40", "<880>" },
	{    "b red",         "41", "<881>" },
	{	"b green",       "42", "<882>" },
	{	"b yellow",      "43", "<883>" },
	{	"b blue",        "44", "<884>" },
	{	"b magenta",     "45", "<885>" },
	{	"b cyan",        "46", "<886>" },
	{	"b white",       "47", "<887>" },
	{	"",              ""  , "<888>" }
};

const struct class_type class_table[CLASS_MAX] =
{
	{
		"OPEN",
		class_open
	},

	{
		"CLOSE",
		class_close
	},

	{
		"READ",
		class_read
	},

	{
		"WRITE",
		class_write
	},

	{
		"KILL",
		class_kill
	}
};
