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
	{	"action",            do_action            },
	{	"alias",             do_alias             },
	{	"all",               do_all               },
	{	"antisubstitute",    do_antisubstitute    },
	{	"bell",              do_bell              },
	{	"boss",              do_boss              },
	{	"buffer",            do_buffer            },
	{	"class",             do_class             },
	{	"commands",          do_commands          },
	{	"config",            do_configure         },
	{	"cr",                do_cr                },
	{	"debug",             do_debug             },
	{	"delay",             do_delay             },
	{	"echo",              do_echo              },
	{	"end",               do_end               },
	{	"forall",            do_forall            },
	{	"format",            do_format            },
	{	"function",          do_function          },
	{	"gag",               do_gag               },
	{    "gagline",           do_gagline           },
	{	"getitemnr",         do_getitemnr         },
	{	"getlistlength",     do_getlistlength     },
	{	"grep",              do_grep              },
	{	"help",              do_help              },
	{	"highlight",         do_highlight         },
	{	"if",                do_if                },
	{	"ignore",            do_ignore            },
	{	"info",              do_info              },
	{	"killall",           do_killall           },
	{	"loadpath",          do_loadpath          },
	{	"log",               do_log               },
	{	"logline",           do_logline           },
	{	"loop",              do_loop              },
	{	"macro",             do_macro             },
	{	"map",               do_map               },
	{	"math",              do_math              },
	{	"mark",              do_mark              },
	{	"message",           do_message           },
	{	"nop",               do_nop               },
	{	"path",              do_path              },
	{	"pathdir",           do_pathdir           },
	{	"prompt",            do_prompt            },
	{	"redit",             do_redit             },
	{	"removestring",      do_removestring      },
	{	"replacestring",     do_replacestring     },
	{	"read",              do_read              },
	{	"readmap",           do_readmap           },
	{	"savepath",          do_savepath          },
	{	"session",           do_session           },
	{	"showme",            do_showme            },
	{	"snoop",             do_snoop             },
	{	"split",             do_split             },
	{	"substitute",        do_substitute        },
	{	"suspend",           do_suspend           },
	{	"system",            do_system            },
	{	"tab",               do_tab               },
	{	"textin",            do_textin            },
	{	"ticker",            do_tick              },
	{	"unaction",          do_unaction          },
	{	"unalias",           do_unalias           },
	{	"unantisubstitute",  do_unantisubstitute  },
	{	"unfunction",        do_unfunction        },
	{	"ungag",             do_ungag             },
	{	"unhighlight",       do_unhighlight       },
	{	"unmacro",           do_unmacro           },
	{	"unpath",            do_unpath            },
	{	"unprompt",          do_unprompt          },
	{	"unsplit",           do_unsplit           },
	{	"unsubstitute",      do_unsubstitute      },
	{	"untab",             do_untab             },
	{	"unticker",          do_untick            },
	{	"unvariable",        do_unvariable        },
	{	"variable",          do_variable          },
	{	"walk",              do_walk              },
	{	"write",             do_write             },
	{	"writemap",          do_writemap          },
	{	"writesession",      do_writesession      },
	{	"zap",               do_zap               },
	{	"",                  NULL                 }
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
