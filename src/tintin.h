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
******************************************************************************/

/******************************************************************************
*                                                                             *
*              (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                   *
*                                                                             *
*                        coded by peter unold 1992                            *
*                       modified by Bill Reiss 1993                           *
*                    updated by Igor van den Hoven 2004                       *
******************************************************************************/

#include <stdio.h>
#include <zlib.h>
#include <signal.h>
#include <ctype.h>
#include <stdarg.h>
#include <termios.h>

/******************************************************************************
*   Autoconf patching by David Hedbor                                         *
*******************************************************************************/

#include "config.h"

#if defined(HAVE_STRING_H)
#include <string.h>
#elif defined(HAVE_STRINGS_H)
#include <strings.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#endif

#ifdef SOCKS
#include <socks.h>
#endif

#ifndef BADSIG
#define BADSIG (RETSIGTYPE (*)(int))-1
#endif

#ifdef HAVE_NET_ERRNO_H
#include <net/errno.h>
#endif

#ifndef __TINTIN_H__
#define __TINTIN_H__

/*
	Typedefs
*/
typedef struct listnode LNODE;

typedef struct session *ARRAY   (struct session *ses, LNODE *list, char *arg);
typedef void            CHAT    (char *arg);
typedef struct session *CLASS   (struct session *ses, char *left, char *right);
typedef struct session *CONFIG  (struct session *ses, char *arg, int index);
typedef struct session *COMMAND (struct session *ses, char *arg);
typedef void            MAP     (struct session *ses, char *arg);
typedef void            CURSOR  (char *arg);
typedef void            PATH    (struct session *ses, char *arg);

/*
	A bunch of constants
*/

#define TRUE                             1
#define FALSE                            0

#define SCREEN_WIDTH                    80
#define SCREEN_HEIGHT                   24

#define PRIORITY                         0
#define ALPHA                            1
#define APPEND                           2

#define DEFAULT_OPEN                   '{'
#define DEFAULT_CLOSE                  '}'

#define COMMAND_SEPARATOR              ';'

#define HISTORY_FILE         ".tt_history"

#define STRING_SIZE                  50000
#define BUFFER_SIZE                  10000
#define NUMBER_SIZE                    100

#define VERSION_NUM               "1.97.4"

#define ESCAPE                          27

#define TELNET_PORT                     23

#define TIMER_POLL_INPUT                 0
#define TIMER_POLL_SESSIONS              1
#define TIMER_POLL_CHAT                  2
#define TIMER_UPDATE_TICKS               3
#define TIMER_UPDATE_DELAYS              4
#define TIMER_UPDATE_PACKETS             5
#define TIMER_UPDATE_CHAT                6
#define TIMER_UPDATE_TERMINAL            7
#define TIMER_UPDATE_MEMORY              8
#define TIMER_STALL_PROGRAM              9
#define TIMER_CPU                       10

#define PULSE_PER_SECOND                20

#define PULSE_POLL_INPUT                 1
#define PULSE_POLL_SESSIONS              1
#define PULSE_POLL_CHAT                  2
#define PULSE_UPDATE_TICKS               1
#define PULSE_UPDATE_DELAYS              1
#define PULSE_UPDATE_PACKETS             2
#define PULSE_UPDATE_CHAT                2
#define PULSE_UPDATE_TERMINAL            1
#define PULSE_UPDATE_MEMORY              1

/*
	Index for lists used by tintin
*/


#define LIST_ACTION                      0
#define LIST_ALIAS                       1
#define LIST_CLASS                       2
#define LIST_CONFIG                      3
#define LIST_DELAY                       4
#define LIST_EVENT                       5
#define LIST_FUNCTION                    6
#define LIST_GAG                         7
#define LIST_HIGHLIGHT                   8
#define LIST_HISTORY                     9
#define LIST_MACRO                      10
#define LIST_MAP                        11
#define LIST_MATH                       12
#define LIST_PATH                       13
#define LIST_PATHDIR                    14
#define LIST_PROMPT                     15
#define LIST_SUBSTITUTE                 16
#define LIST_TAB                        17
#define LIST_TICKER                     18
#define LIST_VARIABLE                   19
#define LIST_MAX                        20

/*
	Various flags
*/

#define CHAT_NAME_CHANGE                 1
#define CHAT_REQUEST_CONNECTIONS         2
#define CHAT_CONNECTION_LIST             3
#define CHAT_TEXT_EVERYBODY              4
#define CHAT_TEXT_PERSONAL               5
#define CHAT_TEXT_GROUP                  6
#define CHAT_MESSAGE                     7
#define CHAT_DO_NOT_DISTURB              8

#define CHAT_SEND_ACTION                 9
#define CHAT_SEND_ALIAS                 10
#define CHAT_SEND_MACRO                 11
#define CHAT_SEND_VARIABLE              12
#define CHAT_SEND_EVENT                 13
#define CHAT_SEND_GAG                   14
#define CHAT_SEND_HIGHLIGHT             15
#define CHAT_SEND_LIST                  16
#define CHAT_SEND_ARRAY                 17
#define CHAT_SEND_BARITEM               18

#define CHAT_VERSION                    19
#define CHAT_FILE_START                 20
#define CHAT_FILE_DENY                  21
#define CHAT_FILE_BLOCK_REQUEST         22
#define CHAT_FILE_BLOCK                 23     
#define CHAT_FILE_END                   24
#define CHAT_FILE_CANCEL                25
#define CHAT_PING_REQUEST               26 
#define CHAT_PING_RESPONSE              27 
#define CHAT_PEEK_CONNECTIONS           28
#define CHAT_PEEK_LIST                  29
#define CHAT_SNOOP_START                30
#define CHAT_SNOOP_DATA                 31

#define CHAT_END_OF_COMMAND            255

#define CHAT_FLAG_PRIVATE             (1 <<  1)
#define CHAT_FLAG_REQUEST             (1 <<  2)
#define CHAT_FLAG_SERVE               (1 <<  3)
#define CHAT_FLAG_IGNORE              (1 <<  4)
#define CHAT_FLAG_FORWARD             (1 <<  5)
#define CHAT_FLAG_FORWARDBY           (1 <<  6)
#define CHAT_FLAG_FORWARDALL          (1 <<  7)
#define CHAT_FLAG_DND                 (1 <<  8)
#define CHAT_FLAG_LINKLOST            (1 <<  9)

#define SUB_NONE                      0
#define SUB_ARG                       (1 <<  0)
#define SUB_VAR                       (1 <<  1)
#define SUB_FUN                       (1 <<  2)
#define SUB_COL                       (1 <<  3)
#define SUB_ESC                       (1 <<  4)
#define SUB_CMD                       (1 <<  5)
#define SUB_ANC                       (1 <<  6)
#define SUB_SEC                       (1 <<  7)
#define SUB_EOL                       (1 <<  8)
#define SUB_FMT                       (1 <<  9)


#define TINTIN_FLAG_RESETBUFFER       (1 <<  0)
#define TINTIN_FLAG_CONVERTMETACHAR   (1 <<  1)
#define TINTIN_FLAG_HISTORYSEARCH     (1 <<  2)
#define TINTIN_FLAG_QUIET             (1 <<  3)
#define TINTIN_FLAG_USERCOMMAND       (1 <<  4)
#define TINTIN_FLAG_INSERTINPUT       (1 <<  5)

#define SES_FLAG_ECHOCOMMAND          (1 <<  1)
#define SES_FLAG_SNOOP                (1 <<  2)
#define SES_FLAG_MAPPING              (1 <<  4)
#define SES_FLAG_SPLIT                (1 <<  5)
#define SES_FLAG_SPEEDWALK            (1 <<  6)
#define SES_FLAG_READMUD              (1 <<  8)
#define SES_FLAG_WORDWRAP             (1 <<  9)
#define SES_FLAG_VERBATIM             (1 << 10)
#define SES_FLAG_CONNECTED            (1 << 11)
#define SES_FLAG_REPEATENTER          (1 << 12)
#define SES_FLAG_VERBOSE              (1 << 13)
#define SES_FLAG_LOGLEVEL             (1 << 15)
#define SES_FLAG_LOGPLAIN             (1 << 16)
#define SES_FLAG_LOGHTML              (1 << 17)
#define SES_FLAG_GAG                  (1 << 18)
#define SES_FLAG_UPDATEVTMAP          (1 << 19)
#define SES_FLAG_SCROLLLOCK           (1 << 21)
#define SES_FLAG_SCAN                 (1 << 22)
#define SES_FLAG_SCROLLSTOP           (1 << 23)
#define SES_FLAG_CONVERTMETA          (1 << 24)
#define SES_FLAG_RUN                  (1 << 25)
#define SES_FLAG_BREAK                (1 << 28) /* wasn't the greatest idea */


#define TELOPT_FLAG_INIT_SGA          (1 <<  0)
#define TELOPT_FLAG_SGA               (1 <<  1)
#define TELOPT_FLAG_EOR               (1 <<  2)
#define TELOPT_FLAG_ECHO              (1 <<  3)
#define TELOPT_FLAG_NAWS              (1 <<  4)
#define TELOPT_FLAG_PROMPT            (1 <<  5)
#define TELOPT_FLAG_IAC               (1 <<  6)
#define TELOPT_FLAG_DEBUG             (1 <<  7)
#define TELOPT_FLAG_INIT_TSPEED       (1 <<  8)
#define TELOPT_FLAG_INIT_TTYPE        (1 <<  9)
#define TELOPT_FLAG_INIT_NAWS         (1 << 10)


#define LIST_FLAG_IGNORE              (1 <<  0)
#define LIST_FLAG_MESSAGE             (1 <<  1)
#define LIST_FLAG_DEBUG               (1 <<  2)
#define LIST_FLAG_LOG                 (1 <<  3)
#define LIST_FLAG_CLASS               (1 <<  4)
#define LIST_FLAG_READ                (1 <<  5)
#define LIST_FLAG_WRITE               (1 <<  6)
#define LIST_FLAG_SHOW                (1 <<  7)
#define LIST_FLAG_INHERIT             (1 <<  8)

#define LIST_FLAG_DEFAULT             LIST_FLAG_MESSAGE

#define NODE_FLAG_MAX                 (1 <<  0)

#define ROOM_FLAG_AVOID               (1 <<  0)
#define ROOM_FLAG_HIDE                (1 <<  1)
#define ROOM_FLAG_LEAVE               (1 <<  2)
#define ROOM_FLAG_VOID                (1 <<  3)

#define MAP_FLAG_STATIC               (1 <<  0)
#define MAP_FLAG_VTMAP                (1 <<  1)
#define MAP_FLAG_VTGRAPHICS           (1 <<  2)
#define MAP_FLAG_ASCIIGRAPHICS        (1 <<  3)

#define MAP_EXIT_N                    (1 <<  0)
#define MAP_EXIT_E                    (1 <<  1)
#define MAP_EXIT_S                    (1 <<  2)
#define MAP_EXIT_W                    (1 <<  3)
#define MAP_EXIT_U                    (1 <<  4)
#define MAP_EXIT_D                    (1 <<  5)
#define MAP_EXIT_NE                   (1 <<  6)
#define MAP_EXIT_NW                   (1 <<  7)
#define MAP_EXIT_SE                   (1 <<  8)
#define MAP_EXIT_SW                   (1 <<  9)

#define MAP_UNDO_CREATE               (1 <<  0)
#define MAP_UNDO_LINK                 (1 <<  1)

#define STR_HASH_FLAG_NOGREP          (1 <<  0)

#define CMD_FLAG_NONE                 (0 <<  0)
#define CMD_FLAG_SUB                  (1 <<  0)

#define MAX_STR_HASH                  10000

#define MAX_ROOM                      15000

/*
	Some macros to deal with double linked lists
*/

#define LINK(link, head, tail)                                   \
{                                                                \
     if ((head) == NULL)                                         \
     {                                                           \
          (head)                  = (link);                      \
     }                                                           \
     else                                                        \
     {                                                           \
          (tail)->next             = (link);                     \
     }                                                           \
     (link)->next                  = NULL;                       \
     (link)->prev                  = (tail);                     \
     (tail)                        = (link);                     \
}


#define INSERT_LEFT(link, rght, head)                            \
{                                                                \
     (link)->prev                  = (rght)->prev;               \
     (rght)->prev                  = (link);                     \
     (link)->next                  = (rght);                     \
                                                                 \
     if ((link)->prev)                                           \
     {                                                           \
          (link)->prev->next       = (link);                     \
     }                                                           \
     else                                                        \
     {                                                           \
          (head)                  = (link);                      \
     }                                                           \
}


#define INSERT_RIGHT(link, left, tail)                           \
{                                                                \
     (link)->next                  = (left)->next;               \
     (left)->next                  = (link);                     \
     (link)->prev                  = (left);                     \
                                                                 \
     if ((link)->next)                                           \
     {                                                           \
          (link)->next->prev       = (link);                     \
     }                                                           \
     else                                                        \
     {                                                           \
          (tail)                   = (link);                     \
     }                                                           \
}

#define UNLINK(link, head, tail)                                 \
{                                                                \
     if (((link)->prev == NULL && (link) != head)                \
     ||  ((link)->next == NULL && (link) != tail))               \
     {                                                           \
          tintin_printf2(NULL, "#UNLINK ERROR in file %s on line %d", __FILE__, __LINE__); \
          dump_stack();                                          \
     }                                                           \
     if ((link)->prev == NULL)                                   \
     {                                                           \
          (head)                  = (link)->next;                \
     }                                                           \
     else                                                        \
     {                                                           \
          (link)->prev->next       = (link)->next;               \
     }                                                           \
     if ((link)->next == NULL)                                   \
     {                                                           \
          (tail)                   = (link)->prev;               \
     }                                                           \
     else                                                        \
     {                                                           \
          (link)->next->prev       = (link)->prev;               \
     }                                                           \
     (link)->next = NULL;                                        \
     (link)->prev = NULL;                                        \
}

/*
	string allocation
*/

#define RESTRING(point, value)   \
{                                \
	free(point);                \
	point = strdup((value));    \
}

#define STRFREE(point)           \
{                                \
	free((point));              \
	point = NULL;               \
}

#define URANGE(a, b, c)           ((b) < (a) ? (a) : (b) > (c) ? (c) : (b))
#define UMAX(a, b)                ((a) > (b) ? (a) : (b))
#define UMIN(a, b)                ((a) < (b) ? (a) : (b))

#define IS_SPLIT(ses)             ((ses)->rows != (ses)->bot_row || (ses)->top_row != 1)

#define SCROLL(ses)               (((ses)->cur_row >= (ses)->top_row && (ses)->cur_row <= (ses)->bot_row) || (ses)->cur_row == (ses)->rows)

#define DO_ARRAY(array) struct session *array (struct session *ses, struct listnode *list, char *arg)
#define DO_CHAT(chat) void chat (char *arg)
#define DO_CLASS(class) struct session *class (struct session *ses, char *left, char *right)
#define DO_COMMAND(command) struct session  *command (struct session *ses, char *arg)
#define DO_CONFIG(config) struct session *config (struct session *ses, char *arg, int index)
#define DO_MAP(map) void map (struct session *ses, char *arg)
#define DO_PATH(path) void path (struct session *ses, char *arg)
#define DO_CURSOR(cursor) void cursor (char *arg)




/*
	Bit operations
*/

#define HAS_BIT(bitvector, bit)   ((bitvector)  & (bit))
#define SET_BIT(bitvector, bit)   ((bitvector) |= (bit))
#define DEL_BIT(bitvector, bit)   ((bitvector) &= (~(bit)))
#define TOG_BIT(bitvector, bit)   ((bitvector) ^= (bit))

/*
	Compatibility
*/

#define atoll(str) (strtoll(str, NULL, 10))


/************************ structures *********************/

struct listroot
{
	struct listnode   * f_node;
	struct listnode   * l_node;
	struct listnode   * update;
	int                 count;
	int                 flags;
};

struct listnode
{
	struct listnode   * next;
	struct listnode   * prev;
	char              * left;
	char              * right;
	char              * pr;
	char              * class;
	long long           data;
	short               flags;
};

struct session
{
	struct session        * next;
	struct session        * prev;
	struct map_data       * map;
	z_stream              * mccp;
	char                 ** buffer;
	char                  * name;
	char                  * class;
	FILE                  * logfile;
	FILE                  * logline;
	struct listroot       * list[LIST_MAX];
	struct listroot       * history;
	int                     rows;
	int                     cols;
	int                     top_row;
	int                     bot_row;
	int                     cur_row;
	int                     sav_row;
	int                     cur_col;
	int                     sav_col;
	int                     scroll_max;
	int                     scroll_row;
	int                     scroll_line;
	int                     scroll_base;
	int                     fgc;
	int                     bgc;
	int                     vtc;
	int                     socket;
	int                     telopts;
	int                     flags;
	char                  * host;
	char                  * port;
	long long               connect_retry;
	int                     connect_error;
	char                    more_output[STRING_SIZE];
	long long               check_output;
};

struct tintin_data
{
	struct session        * ses;
	struct session        * update;
	struct chat_data      * chat;
	struct memory_data    * mem;
	struct termios          old_terminal;
	struct termios          new_terminal;
	char                  * mud_output_buf;
	int                     mud_output_max;
	int                     mud_output_len;
	unsigned char         * mccp_buf;
	int                     mccp_buf_max;
	char                    input_buf[STRING_SIZE];
	char                    input_tmp[BUFFER_SIZE];
	char                    macro_buf[BUFFER_SIZE];
	int                     input_len;
	int                     input_cur;
	int                     input_pos;
	struct listnode       * input_his;
	long long               time;
	long long               timer[TIMER_CPU][5];
	long long               total_io_ticks;
	long long               total_io_exec;
	long long               total_io_delay;
	int                     str_hash_size;
	int                     history_size;
	int                     command_ref[26];
	int                     flags;
	char                    tintin_char;
	char                    verbatim_char;
	char                    repeat_char;
	char                  * vars[10];
	char                  * cmds[10];
};

struct chat_data
{
	struct chat_data      * next;
	struct chat_data      * prev;
	char                  * name;
	char                  * ip;
	char                  * version;
	char                  * download;
	char                  * reply;
	char                  * paste_buf;
	char                  * color;
	char                  * group;
	int                     port;
	int                     fd;
	time_t                  timeout;
	int                     flags;
	long long               paste_time;
	FILE                  * file_pt;
	char                  * file_name;
	long long               file_size;
	int                     file_block_cnt;
	int                     file_block_tot;
	int                     file_block_patch;
	long long               file_start_time;
};

/*
	Structures for tables.c
*/

struct array_type
{
	char                  * name;
	ARRAY                 * array;
};

struct chat_type
{
	char                  * name;
	CHAT                  * fun;
	char                  * desc;
};

struct class_type
{
	char                  * name;
	CLASS                 * class;
};

struct color_type
{
	char                  * name;
	char                  * number;
	char                  * code;
};

struct command_type
{
	char                  * name;
	COMMAND               * command;
	int                     flags;
};

struct config_type
{
	char                  * name;
	char                  * msg_on;
	char                  * msg_off;
	CONFIG                * config;
};

struct event_type
{
	char                  * name;
	char                  * desc;
};

struct list_type
{
	char                  * name;
	char                  * name_multi;
	int                     mode;
	int                     args;
	int                     flags;
};

struct map_type
{
	char                  * name;
	MAP                   * map;
};

struct cursor_type
{
	char                  * name;
	char                  * desc;
	char                  * code;
	CURSOR                * fun;
};

struct timer_type
{
	char                  * name;
};

struct path_type
{
	char                  * name;
	PATH                  * fun;
};

struct str_hash_data
{
	struct str_hash_data    * next;
	struct str_hash_data    * prev;
	unsigned short            count;
	unsigned short            flags;
	unsigned short            lines;
	unsigned short            hash;
};

struct str_hash_index_data
{
	struct str_hash_data    * f_node;
	struct str_hash_data    * l_node;
};

struct map_data
{
	struct room_data      * room_list[MAX_ROOM];
	int                     flags;
	int                     in_room;
	unsigned char           legenda[17];
};

struct room_data
{
	struct exit_data        * f_exit;
	struct exit_data        * l_exit;
	int                       vnum;
	short                     size;
	short                     flags;
	char                    * color;
	char                    * name;
	char                    * symbol;
};

struct exit_data
{
	struct exit_data        * next;
	struct exit_data        * prev;
	short                     vnum;
	char                    * name;
	char                    * cmd;
};

struct memory_data
{
	struct memory_data      * next;
	struct memory_data      * prev;
	char                    * data;
};

#endif


/*
	Function declarations
*/

#ifndef __CURSOR_H__
#define __CURSOR_H__

extern DO_COMMAND(do_cursor);

extern DO_CURSOR(cursor_backspace);
extern DO_CURSOR(cursor_check_line);
extern DO_CURSOR(cursor_clear_left);
extern DO_CURSOR(cursor_clear_line);
extern DO_CURSOR(cursor_clear_right);
extern DO_CURSOR(cursor_convert_meta);
extern DO_CURSOR(cursor_delete);
extern DO_CURSOR(cursor_delete_word);
extern DO_CURSOR(cursor_echo);
extern DO_CURSOR(cursor_end);
extern DO_CURSOR(cursor_exit);
extern DO_CURSOR(cursor_fix_line);
extern DO_CURSOR(cursor_history_find);
extern DO_CURSOR(cursor_history_next);
extern DO_CURSOR(cursor_history_prev);
extern DO_CURSOR(cursor_history_search);
extern DO_CURSOR(cursor_home);
extern DO_CURSOR(cursor_insert);
extern DO_CURSOR(cursor_left);
extern DO_CURSOR(cursor_paste_buffer);
extern DO_CURSOR(cursor_redraw_input);
extern DO_CURSOR(cursor_redraw_line);
extern DO_CURSOR(cursor_right);
extern DO_CURSOR(cursor_suspend);
extern DO_CURSOR(cursor_tab);



#endif

#ifndef __INPUT_H__
#define __INPUT_H__

extern void process_input(void);
extern void read_line(char *line);
extern void read_key(char *line);
extern void convert_meta(char *input, char *output);
extern void unconvert_meta(char *input, char *output);
extern void echo_command(struct session *ses, char *line, int newline);
extern void input_printf(char *format, ...);
#endif

#ifndef __ARRAY_H__
#define __ARRAY_H__

extern DO_COMMAND(do_list);
extern DO_ARRAY(array_clr);
extern DO_ARRAY(array_ins);
extern DO_ARRAY(array_del);
extern DO_ARRAY(array_fnd);
extern DO_ARRAY(array_get);
extern DO_ARRAY(array_len);
extern DO_ARRAY(array_set);
extern DO_ARRAY(array_srt);

extern char *get_list_item(struct session *ses, struct listnode *node, char *arg);
extern void  set_list_item(struct session *ses, struct listnode *node, char *left, char *right);

#endif

#ifndef __CHAT_H__
#define __CHAT_H__

extern DO_COMMAND(do_chat);
extern DO_CHAT(chat_accept);
extern DO_CHAT(chat_call);
extern DO_CHAT(chat_cancelfile);
extern DO_CHAT(chat_color);
extern DO_CHAT(chat_decline);
extern DO_CHAT(chat_dnd);
extern DO_CHAT(chat_downloaddir);
extern DO_CHAT(chat_emote);
extern DO_CHAT(chat_filestat);
extern DO_CHAT(chat_group);
extern DO_CHAT(chat_forward);
extern DO_CHAT(chat_forwardall);
extern DO_CHAT(chat_ignore);
extern DO_CHAT(chat_initialize);
extern DO_CHAT(chat_info);
extern DO_CHAT(chat_ip);
extern DO_CHAT(chat_message);
extern DO_CHAT(chat_name);
extern DO_CHAT(chat_paste);
extern DO_CHAT(chat_peek);
extern DO_CHAT(chat_ping);
extern DO_CHAT(chat_private);
extern DO_CHAT(chat_public);
extern DO_CHAT(chat_reply);
extern DO_CHAT(chat_request);
extern DO_CHAT(chat_send);
extern DO_CHAT(chat_sendfile);
extern DO_CHAT(chat_transfer);
extern DO_CHAT(chat_serve);
extern DO_CHAT(chat_uninitialize);
extern DO_CHAT(chat_who);
extern DO_CHAT(chat_zap);

extern  int chat_new(int s);
extern void chat_call(char *ip);
extern void close_chat(struct chat_data *buddy, int unlink);
extern void process_chat_connections(fd_set *read_set, fd_set *write_set, fd_set *exc_set);
extern void chat_forward_session(struct session *ses, char *linelog);
extern void chat_socket_printf(struct chat_data *buddy, char *format, ...);
extern void chat_printf(char *format, ...);
extern  int process_chat_input(struct chat_data *buddy);
extern void get_chat_commands(struct chat_data *buddy, char *buf, int len);
extern void chat_name_change(struct chat_data *buddy, char *txt);
extern void chat_receive_text_everybody(struct chat_data *buddy, char *txt);
extern void chat_receive_text_personal(struct chat_data *buddy, char *txt);
extern void chat_receive_text_group(struct chat_data *buddy, char *txt);
extern void chat_receive_message(struct chat_data *buddy, char *txt);
extern void chat_receive_snoop_data(struct chat_data *buddy, char *txt);

extern void ping_response(struct chat_data *ch, char *time);

extern void request_response(struct chat_data *requester);
extern void parse_requested_connections(struct chat_data *buddy, char *txt);

extern void peek_response(struct chat_data *peeker);
extern void parse_peeked_connections(struct chat_data *buddy, char *txt);


extern void chat_receive_file(char *arg, struct chat_data *ch);
extern void send_block(struct chat_data *ch);
extern void receive_block(unsigned char *s, struct chat_data *ch, int size);
extern void deny_file(struct chat_data *ch, char *arg);
extern void file_denied(struct chat_data *ch, char *txt);
extern void file_cleanup(struct chat_data *buddy);

extern int get_file_size(char *fpath);

extern void chat_puts(char *arg);
extern struct chat_data *find_buddy(char *arg);
extern struct chat_data *find_group(char *arg);
extern char *fix_file_name(char *name);

#endif
 
#ifndef __TERMINAL_H__
#define __TERMINAL_H__

extern void init_terminal(void);
extern void reset_terminal(void);
extern void echo_on(struct session *ses);
extern void echo_off(struct session *ses);
extern void init_screen_size(struct session *ses);
extern int get_scroll_size(struct session *ses);

#endif

#ifndef __CLASS_H__
#define __CLASS_H__

extern DO_COMMAND(do_class);
extern int count_class(struct session *ses, struct listnode *class);
extern DO_CLASS(class_open);
extern DO_CLASS(class_close);
extern DO_CLASS(class_read);
extern DO_CLASS(class_write);
extern DO_CLASS(class_kill);
extern void parse_class(struct session *ses, char *input, struct listnode *class);

#endif

#ifndef __MAPPER_H__
#define __MAPPER_H__

extern DO_COMMAND(do_map);

extern DO_MAP(map_color);
extern DO_MAP(map_create);
extern DO_MAP(map_delete);
extern DO_MAP(map_destroy);
extern DO_MAP(map_dig);
extern DO_MAP(map_exit);
extern DO_MAP(map_explore);
extern DO_MAP(map_find);
extern DO_MAP(map_flag);
extern DO_MAP(map_get);
extern DO_MAP(map_goto);
extern DO_MAP(map_info);
extern DO_MAP(map_insert);
extern DO_MAP(map_leave);
extern DO_MAP(map_legenda);
extern DO_MAP(map_link);
extern DO_MAP(map_list);
extern DO_MAP(map_map);
extern DO_MAP(map_move);
extern DO_MAP(map_name);
extern DO_MAP(map_read);
extern DO_MAP(map_roomflag);
extern DO_MAP(map_set);
extern DO_MAP(map_travel);
extern DO_MAP(map_undo);
extern DO_MAP(map_unlink);
extern DO_MAP(map_walk);
extern DO_MAP(map_write);

extern void create_map(struct session *ses);
extern void delete_map(struct session *ses);
extern int  create_room(struct session *ses, char *arg);
extern void delete_room(struct session *ses, int room, int exits);
extern void create_exit(struct session *ses, int room, char *arg);
extern void delete_exit(struct session *ses, int room, struct exit_data *exit);
extern void create_legenda(struct session *ses, char *arg);
extern int  find_room(struct session *ses, char *arg);
extern void goto_room(struct session *ses, int room);
extern struct exit_data *find_exit(struct session *ses, int room, char *arg);
extern int  get_map_exit(struct session *ses, char *arg);
extern int  get_map_exits(struct session *ses, int room);
extern void create_map_grid(struct session *ses, short room, short x, short y);
extern void build_map_grid(short room, short x, short y, short z);
extern void follow_map(struct session *ses, char *argument);
extern void insert_undo(struct session *ses, char *format, ...);
extern char *draw_room(struct session *ses, struct room_data *room, int line);
extern void search_path(short room, short size);
extern void shortest_path(struct session *ses, int walk, char *left, char *right);
extern void explore_path(struct session *ses, int walk, char *left, char *right);
extern int find_coord(struct session *ses, char *arg);
extern void search_coord(int vnum, short x, short y, short z);
extern void show_vtmap(struct session *ses);

#endif

#ifndef __MATH_H__
#define __MATH_H__

extern DO_COMMAND(do_math);
extern DO_COMMAND(do_if);
extern double get_number(struct session *ses, char *str);
extern void get_number_string(struct session *ses, char *str, char **result);
extern double mathexp(struct session *ses, char *str);
extern int mathexp_tokenize(struct session *ses, char *str);
extern void mathexp_level(struct listnode *node);
extern void mathexp_compute(struct listnode *node);
extern double tintoi(char *str);
extern double tincmp(char *left, char *right);
extern double tineval(char *left, char *right);
extern double tindice(char *left, char *right);

#endif

#ifndef __PROMPT_H__
#define __PROMPT_H__

extern DO_COMMAND(do_split);
extern DO_COMMAND(do_unsplit);
extern void init_split(struct session *ses, int top, int bot);
extern void clean_screen(struct session *ses);
extern void dirty_screen(struct session *ses);
extern DO_COMMAND(do_prompt);
extern DO_COMMAND(do_unprompt);
extern void check_all_prompts(struct session *ses, char *original, char *line);
extern void do_one_prompt(struct session *ses, char *prompt, int row);

#endif


#ifndef __REGEXP_H__
#define __REGEXP_H__

extern int regexp(char *exp, char *str, unsigned char cs);
extern void substitute(struct session *ses, char *string, char **result, int flags);
extern int check_one_action(char *line, char *original, char *action, struct session *ses);
extern int action_regexp(char *exp, char *str, unsigned char arg);


#endif


#ifndef __CONFIG_H__
#define __CONFIG_H__

extern DO_COMMAND(do_configure);
extern DO_CONFIG(config_speedwalk);
extern DO_CONFIG(config_verbatim);
extern DO_CONFIG(config_repeatenter);
extern DO_CONFIG(config_echocommand);
extern DO_CONFIG(config_verbose);
extern DO_CONFIG(config_wordwrap);
extern DO_CONFIG(config_log);
extern DO_CONFIG(config_buffersize);
extern DO_CONFIG(config_scrolllock);
extern DO_CONFIG(config_historysize);
extern DO_CONFIG(config_connectretry);
extern DO_CONFIG(config_packetpatch);
extern DO_CONFIG(config_tintinchar);
extern DO_CONFIG(config_verbatimchar);
extern DO_CONFIG(config_repeatchar);
extern DO_CONFIG(config_debugtelnet);
extern DO_CONFIG(config_convertmeta);
extern DO_CONFIG(config_loglevel);

#endif


#ifndef __MACRO_H__
#define __MACRO_H__

extern DO_COMMAND(do_macro);
extern DO_COMMAND(do_unmacro);
extern void macro_update(void);
#endif

#ifndef __STRHASH_H__
#define __STRHASH_H__

extern char *str_hash(char *str, int lines);
extern char *str_unhash(char *str);
extern unsigned short str_hash_lines(char *str);
extern unsigned short str_hash_grep(char *str, int write);
extern void reset_hash_table(void);

extern DO_COMMAND(do_hash);

#endif

#ifndef __BUFFER_H__
#define __BUFFER_H__

extern void init_buffer(struct session *ses, int size);
extern void add_line_buffer(struct session *ses, char *line, int more_output);
extern DO_COMMAND(do_buffer);
extern DO_COMMAND(do_grep);
extern int show_buffer(struct session *ses);
extern DO_CURSOR(buffer_u);
extern DO_CURSOR(buffer_d);
extern DO_CURSOR(buffer_h);
extern DO_CURSOR(buffer_e);
extern DO_CURSOR(buffer_l);
extern void buffer_f(char *arg);

#endif

#ifndef __DEBUG_H__
#define __DEBUG_H__

extern int push_call(char *f, ...);
extern void pop_call(void);
extern void dump_stack(void);

#endif

#ifndef __EVENT_H__
#define __EVENT_H__

extern DO_COMMAND(do_event);
extern DO_COMMAND(do_unevent);
extern void check_all_events(struct session *ses, char *line);

#endif

#ifndef __ACTION_H__
#define __ACTION_H__

extern struct session *do_action(struct session *ses, char *arg);
extern struct session *do_unaction(struct session *ses, char *arg);

extern void check_all_actions(struct session *ses, char *original, char *line);

#endif

#ifndef __ALIAS_H__
#define __ALIAS_H__

extern struct session *do_alias(struct session *ses, char *arg);
extern struct session *do_unalias(struct session *ses, char *arg);
extern int check_all_aliases(struct session *ses, char *original, char *line, char **command);

#endif


#ifndef __FILES_H__
#define __FILES_H__

extern DO_COMMAND(do_read);
extern DO_COMMAND(do_write);

extern void prepare_for_write(int mode, struct listnode *node, char *result);

#endif 

#ifndef __FUNCTION_H__
#define __FUNCTION_H__

extern DO_COMMAND(do_function);
extern DO_COMMAND(do_unfunction);

#endif


#ifndef __GAG_H__
#define __GAG_H__

extern DO_COMMAND(do_gag);
extern DO_COMMAND(do_ungag);
extern void check_all_gags(struct session *ses, char *original, char *line);

#endif


#ifndef __HELP_H__
#define __HELP_H__

extern DO_COMMAND(do_help);

#endif

#ifndef __HIGHLIGHT_H__
#define __HIGHLIGHT_H__

extern DO_COMMAND(do_highlight);
extern int is_high_arg(char *s);
extern DO_COMMAND(do_unhighlight);
extern void check_all_highlights(struct session *ses, char **original, char *line);
extern int get_highlight_codes(struct session *ses, char *htype, char *result);

#endif


#ifndef __HISTORY_H__
#define __HISTORY_H__

extern DO_COMMAND(do_history);

extern void add_line_history(struct session *ses, char *line);
extern void search_line_history(struct session *ses, char *line);

extern int write_history(struct session *ses, char *filename);
extern int read_history(struct session *ses, char *filename);
#endif


#ifndef __LLIST_H__
#define __LLIST_H__

extern struct listroot *init_list(int index);
extern void kill_list(struct session *ses, int index);
extern DO_COMMAND(do_killall);
extern struct listroot *copy_list(struct session *ses, struct listroot *sourcelist, int index);
extern void insertnode_list(struct session *ses, char *ltext, char *rtext, char *prtext, int ondex);
extern void updatenode_list(struct session *ses, char *ltext, char *rtext, char *prtext, int index);
extern void deletenode_list(struct session *ses, struct listnode *node, int index);
extern struct listnode *searchnode_list(struct listroot *listhead, char *cptr);
extern struct listnode *searchnode_list_begin(struct listroot *listhead, char *cptr, int mode);
extern void shownode_list(struct session *ses, struct listnode *node, int index);
extern void show_list(struct session *ses, struct listroot *listhead, int index);
extern int show_node_with_wild(struct session *ses, char *cptr, int index);
extern struct listnode *search_node_with_wild(struct listroot *listhead, char *cptr);
extern void delete_node_with_wild(struct session *ses, int index, char *string);

extern void addnode_list(struct listroot *listhead, char *ltext, char *rtext, char *prtext);
extern int count_list(struct listroot *listhead);
extern DO_COMMAND(do_message);
extern DO_COMMAND(do_ignore);
extern DO_COMMAND(do_debug);
#endif


#ifndef __LOG_H__
#define __LOG_H__

extern void logit(struct session *ses, char *txt, FILE *file);
extern DO_COMMAND(do_log);
extern DO_COMMAND(do_logline);
extern DO_COMMAND(do_writebuffer);
extern void write_html_header(FILE *fp);
extern void vt102_to_html(struct session *ses, char *txt, char *out);
#endif


#ifndef __MAIN_H__
#define __MAIN_H__

extern struct session *gts;
extern struct tintin_data *gtd;


extern void winch_handler(int signal);
extern void abort_handler(int signal);
extern void pipe_handler(int signal);
extern void suspend_handler(int signal);
extern void trap_handler(int signal);

extern int main(int argc, char **argv);
extern void init_tintin(void);
extern void quitmsg(char *message);

#endif


#ifndef __MEMORY_H__
#define __MEMORY_H__

extern char *string_alloc(char *string);
extern char *stringf_alloc(char *fmt, ...);
extern char *string_realloc(char *point, char *string);
extern char *stringf_realloc(char *point, char *fmt, ...);
extern char *string_free(char *string);
extern void memory_free(struct memory_data *mem);

#endif


#ifndef __MISC_H__
#define __MISC_H__

extern DO_COMMAND(do_all);
extern DO_COMMAND(do_bell);
extern DO_COMMAND(do_commands);
extern DO_COMMAND(do_cr);
extern DO_COMMAND(do_echo);
extern DO_COMMAND(do_end);
extern DO_COMMAND(do_forall);
extern DO_COMMAND(do_info);
extern DO_COMMAND(do_loop);
extern DO_COMMAND(do_nop);
extern DO_COMMAND(do_parse);
extern DO_COMMAND(do_return);
extern DO_COMMAND(do_run);extern DO_COMMAND(do_send);
extern DO_COMMAND(do_showme);
extern DO_COMMAND(do_snoop);
extern DO_COMMAND(do_suspend);
extern DO_COMMAND(do_zap);
extern DO_COMMAND(do_gagline);

#endif


#ifndef __NET_H__
#define __NET_H__

extern int connect_mud(char *host, char *port, struct session *ses);
extern void write_line_mud(char *line, struct session *ses);
extern int read_buffer_mud(struct session *ses);
extern void readmud(struct session *ses);
extern void process_mud_output(struct session *ses, char *linebuf, int prompt);

#endif
#ifndef __TELOPT_H__
#define __TELOPT_H__

extern void translate_telopts(struct session *ses, unsigned char *src, int cplen);

extern int send_will_sga(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_do_eor(struct session *ses, int cplen, unsigned char *cpsrc);
extern int mark_prompt(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_will_ttype(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_will_tspeed(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_will_naws(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_wont_xdisploc(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_wont_new_environ(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_sb_naws(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_wont_lflow(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_sb_tspeed(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_sb_ttype(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_wont_status(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_dont_status(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_dont_sga(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_do_sga(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_wont_oldenviron(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_echo_on(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_echo_off(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_echo_will(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_ip(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_wont_telopt(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_dont_telopt(struct session *ses, int cplen, unsigned char *cpsrc);
extern int exec_zmp(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_do_mccp2(struct session *ses, int cplen, unsigned char *cpsrc);
extern int send_dont_mccp2(struct session *ses, int cplen, unsigned char *cpsrc);
extern int init_mccp(struct session *ses, int cplen, unsigned char *cpsrc);
extern void *zlib_alloc(void *opaque, unsigned int items, unsigned int size);
extern void zlib_free(void *opaque, void *address);

extern void init_telnet_session(struct session *ses);

#endif

#ifndef __PARSE_H__
#define __PARSE_H__

extern struct session *parse_input(struct session *ses, char *input);
extern int is_speedwalk_dirs(char *cp);
extern void process_speedwalk(char *cp, struct session *ses);
extern struct session *parse_tintin_command(struct session *ses, char *command, char *arg);
extern char *get_arg_all(char *string, char **result);
extern char *get_arg_in_braces(char *string, char **result, int flag);
extern char *get_arg_with_spaces(char *string, char **result);
extern char *get_arg_stop_spaces(char *string, char **result);
extern char *cpy_arg_in_braces(char *pti, char *arg, int flag);
extern char *cpy_arg_with_spaces(char *pti, char *pto);
extern char *cpy_arg_stop_spaces(char *pti, char *pto);
extern char *space_out(char *string);
extern void write_mud(struct session *ses, char *command);
extern void do_one_line(char **line, struct session *ses);

#endif

#ifndef __PATH_H__
#define __PATH_H__

extern DO_COMMAND(do_path);
extern DO_PATH(path_del);
extern DO_PATH(path_end);
extern DO_PATH(path_ins);
extern DO_PATH(path_load);
extern DO_PATH(path_map);
extern DO_PATH(path_new);
extern DO_PATH(path_save);
extern DO_PATH(path_walk);

extern DO_COMMAND(do_pathdir);
extern DO_COMMAND(do_unpathdir);
extern void check_insert_path(char *command, struct session *ses);

#endif


#ifndef __RLTAB_H__
#define __RLTAB_H__

extern DO_COMMAND(do_tab);
extern DO_COMMAND(do_untab);

#endif


#ifndef __SESSION_H__
#define __SESSION_H__

extern DO_COMMAND(do_session);
extern struct session *session_command(char *arg, struct session *ses);
extern void show_session(struct session *ses, struct session *ptr);
extern struct session *newactive_session(void);
extern struct session *activate_session(struct session *ses);
extern struct session *new_session(struct session *ses, char *name, char *address, int desc);
extern void connect_session(struct session *ses);
extern void cleanup_session(struct session *ses);

#endif


#ifndef __SUBSTITUTE_H__
#define __SUBSTITUTE_H__

extern DO_COMMAND(do_substitute);
extern DO_COMMAND(do_unsubstitute);
extern void check_all_substitutions(struct session *ses, char **original, char *line);

#endif


#ifndef __SYSTEM_H__
#define __SYSTEM_H__

extern DO_COMMAND(do_run);
extern DO_COMMAND(do_scan);
extern DO_COMMAND(do_script);
extern DO_COMMAND(do_system);
extern DO_COMMAND(do_textin);
#endif


#ifndef __TABLES_H__
#define __TABLES_H__

extern struct array_type array_table[];
extern struct chat_type chat_table[];
extern struct class_type class_table[];
extern struct color_type color_table[];
extern struct command_type command_table[];
extern struct config_type config_table[];
extern struct cursor_type cursor_table[];
extern struct event_type event_table[];
extern struct list_type list_table[LIST_MAX];
extern struct map_type map_table[];
extern struct timer_type timer_table[];
extern struct path_type path_table[];

#endif

#ifndef __TEXT_H__
#define __TEXT_H__

extern void printline(struct session *ses, char *str, int isaprompt);
int word_wrap(struct session *ses, char *textin, char *textout, int scroll);

#endif

#ifndef __TICKS_H__
#define __TICKS_H__

extern DO_COMMAND(do_tick);
extern DO_COMMAND(do_untick);
extern DO_COMMAND(do_delay);
extern DO_COMMAND(do_undelay);


#endif


#ifndef __UPDATE_H__
#define __UPDATE_H__

extern void mainloop(void);
extern void poll_input(void);
extern void poll_sessions(void);
extern void poll_chat(void);
extern void tick_update(void);
extern void delay_update(void);
extern void packet_update(void);
extern void chat_update(void);
extern void terminal_update(void);
extern void memory_update(void);

#endif


#ifndef __UTILS_H__
#define __UTILS_H__

extern int is_abbrev(char *s1, char *s2);
extern int is_suffix(char *s1, char *s2);
extern int is_number(char *str);
extern int hex_number(char *str);
extern long long utime(void);
extern char *capitalize(char *str);
extern void cat_sprintf(char *dest, char *fmt, ...);
extern void ins_sprintf(char *dest, char *fmt, ...);
extern void syserr(char *msg);
extern void show_message(struct session *ses, int index, char *format, ...);
extern void show_debug(struct session *ses, int index, char *format, ...);
extern void tintin_header(struct session *ses, char *format, ...);
extern void socket_printf(struct session *ses, size_t length, char *format, ...);

extern void tintin_printf2(struct session *ses, char *format, ...);
extern void tintin_printf(struct session *ses, char *format, ...);
extern void tintin_puts2(struct session *ses, char *string);
extern void tintin_puts(struct session *ses, char *string);

extern void show_cpu(struct session *ses);
extern long long display_timer(struct session *ses, int timer);
extern void open_timer(int timer);
extern void close_timer(int timer);

#endif


#ifndef __VARIABLES_H__
#define __VARIABLES_H__

extern DO_COMMAND(do_variable);
extern DO_COMMAND(do_unvariable);

extern void internal_variable(struct session *ses, char *format, ...);

extern DO_COMMAND(do_format);
extern DO_COMMAND(do_tolower);
extern DO_COMMAND(do_toupper);
extern DO_COMMAND(do_postpad);
extern DO_COMMAND(do_prepad);
extern DO_COMMAND(do_replacestring);

#endif


#ifndef __VT102_H__
#define __VT102_H__

extern void save_pos(struct session *ses);
extern void restore_pos(struct session *ses);
extern void goto_rowcol(struct session *ses, int row, int col);
extern void erase_screen(struct session *ses);
extern void erase_toeol(void);
extern void erase_scroll_region(struct session *ses);
extern void reset(void);
extern void scroll_region(struct session *ses, int top, int bottom);
extern void reset_scroll_region(struct session *ses);
extern int skip_vt102_codes(char *str);
extern int skip_vt102_codes_non_graph(char *str);
extern void strip_vt102_codes(char *str, char *buf);
extern void strip_vt102_codes_non_graph(char *str, char *buf);
extern void strip_non_vt102_codes(char *str, char *buf);
extern int strip_vt102_strlen(char *str);
extern int interpret_vt102_codes(struct session *ses, char *str, int real);

#endif
