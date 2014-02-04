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
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                      coded by Igor van den Hoven 2004                       *
******************************************************************************/

#include "tintin.h"

/*
	todo:
*/

void show_map(struct session *ses, char *left, char *right);
void create_legend(struct session *ses, char *arg);

#define MAX_MAP_X 201
#define MAX_MAP_Y 101

int                 map_grid_x;
int                 map_grid_y;
int                 map_grid_z;

struct room_data  * map_grid[MAX_MAP_X][MAX_MAP_Y];

struct session    * map_search_ses;
struct exit_data  * map_search_exit_best;
struct exit_data  * map_search_exit_node;
int                 map_search_last_room;
int                 map_search_max_size;
int                 map_search_tar_room;

#define             MAP_SEARCH_SIZE 500

/*
	MUD font

	#map legend {128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143 161}
	#map legend {144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159 161}
*/

unsigned char	map_palet0[]	= { 42, 35, 35, 43, 35,124, 43, 43, 35, 43, 45, 43, 43, 43, 43, 43, 120};

unsigned char	map_palet1[]	= {126,247,247,109,247,120,108,116,247,106,113,118,107,117,119,110, 88};

#define CHECK_MAP()                                                                                            \
{                                                                                                                  \
	if (ses->map == NULL)                                                                                         \
	{                                                                                                             \
		tintin_printf2(ses, "#MAP: This session has no map data. Use #map create or #map read to create one.");  \
                                                                                                                   \
		return;                                                                                                  \
	}                                                                                                             \
}

#define CHECK_INSIDE()                                                                                         \
{                                                                                                                  \
	if (ses->map == NULL)                                                                                         \
	{                                                                                                             \
		tintin_printf2(ses, "#MAP: This session has no map data. Use #map create or #map read to create one.");  \
                                                                                                                   \
		return;                                                                                                  \
	}                                                                                                             \
	if (ses->map->room_list[ses->map->in_room] == NULL)                                                           \
	{                                                                                                             \
		tintin_printf2(ses, "#MAP: You are not inside the map. Use #map goto to enter it.");                     \
                                                                                                                   \
		return;                                                                                                  \
	}                                                                                                             \
}

DO_COMMAND(do_map)
{
	int cnt;
	char cmd[BUFFER_SIZE], left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, cmd, FALSE);

	if (*cmd == 0)
	{
		tintin_printf2(ses, "Available map options");
		tintin_printf2(ses, "");
		tintin_printf2(ses, "#map color    <color code>             (set the room's color for map)");
		tintin_printf2(ses, "#map create                            (creates the initial map)");
		tintin_printf2(ses, "#map destroy                           (destroys the map)");
		tintin_printf2(ses, "#map delete   <direction>              (delete the room at given dir)");
		tintin_printf2(ses, "#map dig      <direction>              (creates a new room)");
		tintin_printf2(ses, "#map exit     <direction>  <command>   (sets the exit command)");
		tintin_printf2(ses, "#map explore  <direction>              (saves path to #path)");
		tintin_printf2(ses, "#map info                              (info on map and current room)");
		tintin_printf2(ses, "#map insert   <direction>  [roomflag]  (insert a new room)");
		tintin_printf2(ses, "#map jump     <x> <y>                  (go to given coordinate)");
		tintin_printf2(ses, "#map find     <room name>              (saves path to #path)");
		tintin_printf2(ses, "#map flag     <map flag>               (set map wide flags)");
		tintin_printf2(ses, "#map get      <option>     <variable>  (get various values)");
		tintin_printf2(ses, "#map goto     <room name>              (moves you to given room)");
		tintin_printf2(ses, "#map leave                             (leave the map, return with goto)");
		tintin_printf2(ses, "#map legend   <symbols>                (sets the map legend)");
		tintin_printf2(ses, "#map link     <direction>  <room name> (links 2 rooms)");
		tintin_printf2(ses, "#map list                              (shows list of created rooms)");
		tintin_printf2(ses, "#map map      <radius>                 (shows an ascii map)");
		tintin_printf2(ses, "#map name     <room name>              (set the room's name)");
		tintin_printf2(ses, "#map read     <filename>               (load your map from a file)");
		tintin_printf2(ses, "#map roomflag <room flag>              (set room based flags)");
		tintin_printf2(ses, "#map set      <option>     <value>     (set various values)");
		tintin_printf2(ses, "#map run      <room name>  <delay>     (run to given room)");
		tintin_printf2(ses, "#map travel   <direction>  <delay>     (run in given direction)");
		tintin_printf2(ses, "#map undo                              (undo last move)");
		tintin_printf2(ses, "#map unlink   <direction>              (deletes an exit)");
		tintin_printf2(ses, "#map write    <filename>               (save the map to a file)");

		return ses;
	}
	else
	{
		for (cnt = 0 ; *map_table[cnt].name ; cnt++)
		{
			if (is_abbrev(cmd, map_table[cnt].name))
			{
				arg = get_arg_in_braces(arg, left,  map_table[cnt].lval);
				arg = get_arg_in_braces(arg, right, map_table[cnt].rval);

				map_table[cnt].map (ses, left, right);
				break;
			}
		}
		if (*map_table[cnt].name == 0)
		{
			do_map(ses, "");

			return ses;
		}
		if (ses->map && HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP))
		{
			SET_BIT(ses->flags, SES_FLAG_UPDATEVTMAP);
		}
	}
	return ses;
}

DO_MAP(map_color)
{
	CHECK_INSIDE();

	free(ses->map->room_list[ses->map->in_room]->color);
	ses->map->room_list[ses->map->in_room]->color = strdup(left);

	tintin_printf2(ses, "#MAP: Room color set to: %s", left);
}

DO_MAP(map_create)
{
	create_map(ses);

	tintin_printf2(ses, "#MAP: Map created, use #map goto 1, to proceed");
}

DO_MAP(map_delete)
{
	int room;
	struct exit_data *exit;

	CHECK_INSIDE();

	exit = find_exit(ses, ses->map->in_room, left);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: No exit with that name found");

		return;
	}

	room = exit->vnum;

	delete_room(ses, room, TRUE);

	tintin_printf2(ses, "#MAP: Room {%d} deleted", room);
}

DO_MAP(map_destroy)
{
	CHECK_MAP();

	delete_map(ses);

	tintin_printf2(ses, "#MAP: Map destroyed.");
}

DO_MAP(map_dig)
{
	int room;
	char temp[BUFFER_SIZE];
	struct exit_data *exit;
	struct listnode *node;

	CHECK_INSIDE();

	for (room = 1 ; room < MAX_ROOM ; room++)
	{
		if (ses->map->room_list[room] == NULL)
		{
			break;
		}
	}

	exit = find_exit(ses, ses->map->in_room, left);

	if (exit)
	{
		tintin_printf2(ses, "#MAP: There is already a room in that direction.");

		return;
	}

	if (room == MAX_ROOM)
	{
		tintin_printf2(ses, "#MAP: Maximum amount of rooms of %d reached.", MAX_ROOM);
		return;
	}

	if (find_coord(ses, left))
	{
		room = map_search_tar_room;

		show_message(ses, LIST_MAP, "Found linkable room {%d}", room);
	}
	else
	{
		sprintf(temp, "{%d} {0} {<088>} {%d} { }", room, room);
		create_room(ses, temp);
	}
	sprintf(temp, "{%d} {%s} {%s}", room, left, left);
	create_exit(ses, ses->map->in_room, temp);

	if ((node = searchnode_list(ses->list[LIST_PATHDIR], left)) != NULL)
	{
		if (find_exit(ses, room, node->right) == NULL)
		{
			sprintf(temp, "{%d} {%s} {%s}", ses->map->in_room, node->right, node->right);
			create_exit(ses, room, temp);
		}
	}
	tintin_printf2(ses, "#MAP: Created room {%d}.", room);
}

DO_MAP(map_exitcmd)
{
	char temp[BUFFER_SIZE];
	struct exit_data *exit;

	CHECK_INSIDE();

	exit = find_exit(ses, ses->map->in_room, left);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: Exit {%s} not found.", left);

		return;
	}

	sprintf(temp, "{%d} {%s} {%s}", exit->vnum, exit->name, right);

	delete_exit(ses, ses->map->in_room, exit);

	create_exit(ses, ses->map->in_room, temp);

	tintin_printf2(ses, "#MAP: Exit {%s} set to {%s}.", left, right);
}

DO_MAP(map_exitdir)
{
	char temp[BUFFER_SIZE];
	struct exit_data *exit;
	struct listnode *node;

	CHECK_INSIDE();

	exit = find_exit(ses, ses->map->in_room, left);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: Exit {%s} not found.", left);

		return;
	}

	if ((node = searchnode_list(ses->list[LIST_PATHDIR], right)) == NULL)
	{
		tintin_printf2(ses, "#MAP: Pathdir {%s} not found.", right);

		return;
	}

	sprintf(temp, "{%d} {%s} {%s} {%s}", exit->vnum, exit->name, exit->cmd, node->pr);

	delete_exit(ses, ses->map->in_room, exit);

	create_exit(ses, ses->map->in_room, temp);

	tintin_printf2(ses, "#MAP: Exit {%s} set to {%s}.", left, right);
}

DO_MAP(map_explore)
{

	CHECK_INSIDE();

	explore_path(ses, FALSE, left, "");
}

DO_MAP(map_find)
{
	CHECK_INSIDE();

	shortest_path(ses, FALSE, left, "");
}

DO_MAP(map_flag)
{
	int flag;

	CHECK_MAP();

	if (*left)
	{
		if (is_abbrev(left, "static"))
		{
			flag = MAP_FLAG_STATIC;
		}
		else if (is_abbrev(left, "vtgraphics"))
		{
			flag = MAP_FLAG_VTGRAPHICS;
		}
		else if (is_abbrev(left, "vtmap"))
		{
			flag = MAP_FLAG_VTMAP;
		}
		else if (is_abbrev(left, "asciigraphics"))
		{
			flag = MAP_FLAG_ASCIIGRAPHICS;
		}
		else if (is_abbrev(left, "asciivnums"))
		{
			flag = MAP_FLAG_ASCIIVNUMS;
		}
		else
		{
			tintin_printf2(ses, "#MAP: Invalid flag {%s}.", left);

			return;
		}
	}
	else
	{
		tintin_printf2(ses, "#MAP: Static flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) ? "on" : "off");
		tintin_printf2(ses, "#MAP: VTmap flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP) ? "on" : "off");
		tintin_printf2(ses, "#MAP: VTgraphics flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS) ? "on" : "off");
		tintin_printf2(ses, "#MAP: AsciiGraphics flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS) ? "on" : "off");
		tintin_printf2(ses, "#MAP: AsciiVnums flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS) ? "on" : "off");

		return;
	}

	if (is_abbrev("on", right))
	{
		SET_BIT(ses->map->flags, flag);
	}
	else if (is_abbrev("off", right))
	{
		DEL_BIT(ses->map->flags, flag);
	}
	else
	{
		TOG_BIT(ses->map->flags, flag);
	}

	if (is_abbrev(left, "static"))
	{
		tintin_printf2(ses, "#MAP: Static flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) ? "on" : "off");
	}
	else if (is_abbrev(left, "vtgraphics"))
	{
		tintin_printf2(ses, "#MAP: VTgraphics flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS) ? "on" : "off");
	}
	else if (is_abbrev(left, "vtmap"))
	{
		tintin_printf2(ses, "#MAP: VTmap flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP) ? "on" : "off");

		show_vtmap(ses);
	}
	else if (is_abbrev(left, "asciigraphics"))
	{
		tintin_printf2(ses, "#MAP: AsciiGraphics flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS) ? "on" : "off");
	}
	else if (is_abbrev(left, "asciivnums"))
	{
		tintin_printf2(ses, "#MAP: AsciiVnums flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS) ? "on" : "off");
	}
}

DO_MAP(map_get)
{
	CHECK_INSIDE();

	if (*right == 0)
	{
		tintin_printf2(ses, "#MAP: roomcolor is set to:  %s.", ses->map->room_list[ses->map->in_room]->color);
		tintin_printf2(ses, "#MAP: roomflags is set to:  %d.", ses->map->room_list[ses->map->in_room]->flags);
		tintin_printf2(ses, "#MAP: roomname is set to:   %s.", ses->map->room_list[ses->map->in_room]->name);
		tintin_printf2(ses, "#MAP: roomsymbol is set to: %s.", ses->map->room_list[ses->map->in_room]->symbol);
		tintin_printf2(ses, "#MAP: roomvnum is set to:   %d.", ses->map->room_list[ses->map->in_room]->vnum);
	}
	else if (*left)
	{
		if (is_abbrev(left, "roomcolor"))
		{
			internal_variable(ses, "{%s} {%s}", right, ses->map->room_list[ses->map->in_room]->color);
		}
		else if (is_abbrev(left, "roomflags"))
		{
			internal_variable(ses, "{%s} {%d}", right, ses->map->room_list[ses->map->in_room]->flags);
		}
		else if (is_abbrev(left, "roomname"))
		{
			internal_variable(ses, "{%s} {%s}", right, ses->map->room_list[ses->map->in_room]->name);
		}
		else if (is_abbrev(left, "roomsymbol"))
		{
			internal_variable(ses, "{%s} {%s}", right, ses->map->room_list[ses->map->in_room]->symbol);
		}
		else if (is_abbrev(left, "roomvnum"))
		{
			internal_variable(ses, "{%s} {%d}", right, ses->map->room_list[ses->map->in_room]->vnum);
		}
		else
		{
			show_message(ses, LIST_MAP, "#MAP SET: unknown option: %s.", left);
		}
	}
}

DO_MAP(map_goto)
{
	int room;

	CHECK_MAP();

	room = find_room(ses, left);

	if (room != -1)
	{
		insert_undo(ses, "%d %d %d", room, ses->map->in_room, 0);

		goto_room(ses, room);
		show_message(ses, LIST_MAP, "#MAP: Location set to room {%s}.", ses->map->room_list[room]->name);
	}
	else
	{
		show_message(ses, LIST_MAP, "#MAP: Couldn't find room {%s}.", left);
	}
}

DO_MAP(map_info)
{
	int room, cnt, exits;
	struct exit_data *exit;

	CHECK_MAP();

	for (room = cnt = exits = 0 ; room < MAX_ROOM ; room++)
	{
		if (ses->map->room_list[room])
		{
			cnt++;

			exits += get_map_exits(ses, room);
		}
	}

	tintin_printf2(ses, "%-20s %d", "Total rooms:",    cnt);
	tintin_printf2(ses, "%-20s %d", "Total exits:",    exits);
	tintin_printf2(ses, "%-20s %s", "Vtmap:",          HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP) ? "on" : "off");
	tintin_printf2(ses, "%-20s %s", "Static:",         HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) ? "on" : "off");

	if (ses->map->in_room)
	{
		tintin_printf2(ses, "");
		tintin_printf2(ses, "%-20s %d", "Room vnum:",      ses->map->in_room);
		tintin_printf2(ses, "%-20s %s", "Room name:",      ses->map->room_list[ses->map->in_room]->name);
		tintin_printf2(ses, "%-20s %s", "Avoid:",          HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_AVOID) ? "on" : "off");
		tintin_printf2(ses, "%-20s %s", "Hide:",           HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_HIDE) ? "on" : "off");		
		tintin_printf2(ses, "%-20s %s", "Leave:",          HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE) ? "on" : "off");
		tintin_printf2(ses, "%-20s %s", "Void:",           HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID) ? "on" : "off");

		for (exit = ses->map->room_list[ses->map->in_room]->f_exit ; exit ; exit = exit->next)
		{
			tintin_printf2(ses, "%-20s %03d %-3s (%s) to room: %-5d (%s)", "Exit:", exit->dir, exit->name, exit->cmd, exit->vnum, ses->map->room_list[exit->vnum]->name);
		}
	}
}

DO_MAP(map_insert)
{
	int room, in_room, to_room;
	char temp[BUFFER_SIZE];
	struct exit_data *exit;
	struct listnode *node;

	CHECK_INSIDE();

	for (room = 1 ; room < MAX_ROOM ; room++)
	{
		if (ses->map->room_list[room] == NULL)
		{
			break;
		}
	}

	exit = find_exit(ses, ses->map->in_room, left);

	node = searchnode_list(ses->list[LIST_PATHDIR], left);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: There is no room in that direction.");

		return;
	}

	if (room == MAX_ROOM)
	{
		tintin_printf2(ses, "#MAP: Maximum amount of rooms of %d reached.", MAX_ROOM);
		return;
	}

	if (node == NULL)
	{
		tintin_printf2(ses, "#MAP: Given direction must be a pathdir.");
		return;
	}

	in_room = ses->map->in_room;
	to_room = exit->vnum;

	sprintf(temp, "{%d} {0} {<088>} {%d} { }", room, room);
	create_room(ses, temp);

	sprintf(temp, "{%d} {%s} {%s}", to_room, node->left, node->left);
	create_exit(ses, room, temp);

	sprintf(temp, "{%d} {%s} {%s}", in_room, node->right, node->right);
	create_exit(ses, room, temp);

	exit->vnum = room;

	if ((exit = find_exit(ses, to_room, node->right)) != NULL)
	{
		exit->vnum = room;
	}

	if (*right)
	{
		ses->map->in_room = room;
		map_roomflag(ses, right, "");
		ses->map->in_room = in_room;
	}
	tintin_printf2(ses, "#MAP: Inserted room {%d}.", room);
}

DO_MAP(map_jump)
{
	int vnum;

	map_grid_x = 0;
	map_grid_y = 0;
	map_grid_z = 0;

	map_search_ses       = ses;
	map_search_tar_room  = 0;

	map_grid_x += atoi(left);
	map_grid_y += atoi(right);

	for (vnum = 0 ; vnum < MAX_ROOM ; vnum++)
	{
		if (ses->map->room_list[vnum])
		{
			ses->map->room_list[vnum]->size = MAP_SEARCH_SIZE;
		}
	}

	search_coord_bf(ses->map->in_room);

	if (map_search_tar_room)
	{
		insert_undo(ses, "%d %d %d", map_search_tar_room, ses->map->in_room, 0);

		goto_room(ses, map_search_tar_room);

		show_message(ses, LIST_MAP, "#MAP: Location set to room {%s}.", ses->map->room_list[map_search_tar_room]->name);
	}
	else
	{
		show_message(ses, LIST_MAP, "#MAP: Couldn't find room at {%s} {%s}.", left, right);
	}
}

DO_MAP(map_leave)
{
	CHECK_INSIDE();

	if (ses->map->in_room == 0)
	{
		tintin_printf2(ses, "#MAP: You're not currently inside the map.");
	}
	else
	{
		ses->map->last_room = ses->map->in_room;
		ses->map->in_room = 0;

		tintin_printf2(ses, "#MAP: Leaving the map. Use goto to return.");
	}
}

DO_MAP(map_legend)
{
	CHECK_MAP();

	if (*left == 0)
	{
		tintin_printf2(ses, "#MAP: Current legend: %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
			ses->map->legend[0],  ses->map->legend[1],  ses->map->legend[2],
			ses->map->legend[3],  ses->map->legend[4],  ses->map->legend[5],
			ses->map->legend[6],  ses->map->legend[7],  ses->map->legend[8],
			ses->map->legend[9],  ses->map->legend[10], ses->map->legend[11],
			ses->map->legend[12], ses->map->legend[13], ses->map->legend[14],
			ses->map->legend[15], ses->map->legend[16]);
	}
	else
	{
		create_legend(ses, left);
	}
}

DO_MAP(map_link)
{
	char temp[BUFFER_SIZE];
	struct listnode *node;
	struct exit_data *exit;
	int room;

	CHECK_INSIDE();

	if ((room = find_room(ses, right)) == -1)
	{
		tintin_printf2(ses, "#MAP: Couldn't find room {%s}.", right);
		return;
	}

	exit = find_exit(ses, ses->map->in_room, left);

	if (exit)
	{
		delete_exit(ses, ses->map->in_room, exit);
	}

	sprintf(temp, "{%d} {%s} {%s}", room, left, left);
	create_exit(ses, ses->map->in_room, temp);

	if ((node = searchnode_list(ses->list[LIST_PATHDIR], left)) != NULL)
	{
		if (find_exit(ses, room, node->right) == NULL)
		{
			sprintf(temp, "{%d} {%s} {%s}", ses->map->in_room, node->right, node->right);
			create_exit(ses, room, temp);
		}
	}
	tintin_printf2(ses, "#MAP: Linked room {%s} to {%s}.", ses->map->room_list[ses->map->in_room]->name, ses->map->room_list[room]->name);
}

DO_MAP(map_list)
{
	int room;
	struct exit_data *exit;

	CHECK_MAP();

	for (room = 0 ; room < MAX_ROOM ; room++)
	{
		if (ses->map->room_list[room])
		{
			if (*left == 0 || match(ses, ses->map->room_list[room]->name, left))
			{
				tintin_printf2(ses, "[%5d] %s", ses->map->room_list[room]->vnum, ses->map->room_list[room]->name);

				for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
				{
					tintin_printf2(ses, "        [%5d] %s", exit->vnum, exit->cmd);
				}
			}
		}
	}
}

DO_MAP(map_map)
{
	CHECK_INSIDE();

	if (!strcasecmp("vt", left))
	{
		show_vtmap(ses);
	}
	else
	{
		show_map(ses, left, right);
	}
}

DO_MAP(map_move)
{
	CHECK_INSIDE();

	follow_map(ses, left);
}

DO_MAP(map_name)
{
	CHECK_INSIDE();

	free(ses->map->room_list[ses->map->in_room]->name);
	ses->map->room_list[ses->map->in_room]->name = strdup(left);
}

DO_MAP(map_read)
{
	FILE *myfile;
	struct exit_data *exit;
	char buffer[BUFFER_SIZE], *cptr;
	int room = 0;

	if ((myfile = fopen(left, "r")) == NULL)
	{
		tintin_printf(ses, "#MAP: Map file {%s} not found.", left);

		return;
	}

	gtd->quiet++;

	if (ses->map == NULL)
	{
		ses->map = calloc(1, sizeof(struct map_data));
	}

	while (fgets(buffer, BUFFER_SIZE - 1, myfile))
	{
		cptr = strchr(buffer, '\n');

		if (cptr)
		{
			*cptr = 0;
		}

		switch (buffer[0])
		{
			case 'C':
				create_map(ses);
				break;

			case 'E':
				create_exit(ses, room, &buffer[2]);
				break;

			case 'F':
				ses->map->flags = atoi(&buffer[2]);
				break;

			case 'I':
				ses->map->last_room = atoi(&buffer[2]);
				break;

			case 'L':
				create_legend(ses, &buffer[2]);
				break;

			case 'R':
				room = create_room(ses, &buffer[2]);
				break;

			case '#':
				ses = parse_tintin_command(ses, &buffer[1]);
				break;

			case 0:
				break;

			default:
				gtd->quiet--;

				tintin_printf2(ses, "#MAP: Invalid command '%c'. Aborting read.", buffer[0]);

				fclose(myfile);

				delete_map(ses);

				return;				
		}
	}

	gtd->quiet--;

	fclose(myfile);

	for (room = 0 ; room < MAX_ROOM ; room++)
	{
		if (ses->map->room_list[room] == NULL)
		{
			continue;
		}

		for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
		{
			if (ses->map->room_list[exit->vnum] == NULL)
			{
				tintin_printf2(ses, "#MAP READ: Room %d - invalid exit '%s'.", room, exit->name);
			}
		}
	}

	show_message(ses, LIST_MAP, "#MAP READ: Map file {%s} loaded.", left);


}

DO_MAP(map_return)
{
	if (ses->map == NULL || ses->map->room_list[ses->map->last_room] == NULL)
	{
		tintin_printf2(ses, "#MAP RETURN: No known last room.");

		return;
	}

	ses->map->in_room = ses->map->last_room;
}

DO_MAP(map_roomflag)
{
	int flag;

	CHECK_INSIDE();

	if (*left)
	{
		if (is_abbrev(left, "avoid"))
		{
			flag = ROOM_FLAG_AVOID;
		}
		else if (is_abbrev(left, "hide"))
		{
			flag = ROOM_FLAG_HIDE;
		}
		else if (is_abbrev(left, "leave"))
		{
			flag = ROOM_FLAG_LEAVE;
		}
		else if (is_abbrev(left, "void"))
		{
			flag = ROOM_FLAG_VOID;
		}
		else if (is_abbrev(left, "static"))
		{
			flag = ROOM_FLAG_STATIC;
		}
		else
		{
			tintin_printf2(ses, "#MAP: Invalid room flag {%s}.", left);

			return;
		}
	}
	else
	{
			tintin_printf2(ses, "#MAP: Avoid flag is set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_AVOID) ? "on" : "off");
			tintin_printf2(ses, "#MAP: Hide flag is set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_HIDE) ? "on" : "off");
			tintin_printf2(ses, "#MAP: Leave flag is set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE) ? "on" : "off");
			tintin_printf2(ses, "#MAP: Void flag is set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID) ? "on" : "off");
			tintin_printf2(ses, "#MAP: Static flag is set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_STATIC) ? "on" : "off");

			return;
	}

	if (is_abbrev("on", right))
	{
		SET_BIT(ses->map->room_list[ses->map->in_room]->flags, flag);
	}
	else if (is_abbrev("off", right))
	{
		DEL_BIT(ses->map->room_list[ses->map->in_room]->flags, flag);
	}
	else
	{
		TOG_BIT(ses->map->room_list[ses->map->in_room]->flags, flag);
	}	


	if (is_abbrev(left, "avoid"))
	{
		tintin_printf2(ses, "#MAP: Avoid flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_AVOID) ? "on" : "off");
	}
	else if (is_abbrev(left, "hide"))
	{
		tintin_printf2(ses, "#MAP: Hide flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_HIDE) ? "on" : "off");
	}
	else if (is_abbrev(left, "leave"))
	{
		tintin_printf2(ses, "#MAP: Leave flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE) ? "on" : "off");
	}
	else if (is_abbrev(left, "void"))
	{
		tintin_printf2(ses, "#MAP: Void flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID) ? "on" : "off");
	}
	else if (is_abbrev(left, "static"))
	{
		tintin_printf2(ses, "#MAP: Static flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_STATIC) ? "on" : "off");
	}
}

DO_MAP(map_set)
{
	CHECK_INSIDE();

	if (*right == 0)
	{
		tintin_printf2(ses, "#MAP: roomcolor is set to:  %s.", ses->map->room_list[ses->map->in_room]->color);
		tintin_printf2(ses, "#MAP: roomflags is set to:  %d.", ses->map->room_list[ses->map->in_room]->flags);
		tintin_printf2(ses, "#MAP: roomname is set to:   %s.", ses->map->room_list[ses->map->in_room]->name);
		tintin_printf2(ses, "#MAP: roomsymbol is set to: %s.", ses->map->room_list[ses->map->in_room]->symbol);
	}
	else if (*left)
	{
		if (is_abbrev(left, "roomcolor"))
		{
			if (is_color_code(right))
			{
				RESTRING(ses->map->room_list[ses->map->in_room]->color, right);

				show_message(ses, LIST_MAP, "#MAP SET: roomcolor set to: %s.", right);
			}
			else
			{
				tintin_printf2(ses, "#MAP SET: invalid color code '%s'.", right);
			}
		}
		else if (is_abbrev(left, "roomflags"))
		{
			ses->map->room_list[ses->map->in_room]->flags = (int) get_number(ses, right);

			show_message(ses, LIST_MAP, "#MAP SET: roomflags set to: %d.", ses->map->room_list[ses->map->in_room]->flags);
		}
		else if (is_abbrev(left, "roomname"))
		{
			RESTRING(ses->map->room_list[ses->map->in_room]->name, right);

			show_message(ses, LIST_MAP, "#MAP SET: roomname set to: %s.", ses->map->room_list[ses->map->in_room]->name);
		}
		else if (is_abbrev(left, "roomsymbol"))
		{
			RESTRING(ses->map->room_list[ses->map->in_room]->symbol, right);

			show_message(ses, LIST_MAP, "#MAP SET: roomsymbol set to: %s.", ses->map->room_list[ses->map->in_room]->symbol);
		}
		else
		{
			show_message(ses, LIST_MAP, "#MAP SET: unknown option: %s.", left);
		}
	}
}

DO_MAP(map_travel)
{
	CHECK_INSIDE();

	explore_path(ses, TRUE, left, right);
}

DO_MAP(map_undo)
{
	struct listnode *node;
	int undo_flag;
	struct exit_data *exit;

	CHECK_INSIDE();

	node = ses->list[LIST_MAP]->l_node;

	if (node == NULL)
	{
		tintin_printf2(ses, "#MAP UNDO: No known last move.");

		return;
	}

	if (ses->map->in_room != atoi(node->left))
	{
		tintin_printf2(ses, "#MAP UNDO: Invalid last move.");

		return;
	}

	if (ses->map->room_list[atoi(node->right)] == NULL)
	{
		tintin_printf2(ses, "#MAP UNDO: Invalid last move.");

		return;
	}

	undo_flag = atoi(node->pr);

	goto_room(ses, atoi(node->right));

	if (HAS_BIT(undo_flag, MAP_UNDO_CREATE))
	{
		delete_room(ses, atoi(node->left), TRUE);
	}
	else if (HAS_BIT(undo_flag, MAP_UNDO_LINK))
	{
		exit = find_exit(ses, atoi(node->left), node->right);

		if (exit)
		{
			delete_exit(ses, atoi(node->left), exit);
		}

		exit = find_exit(ses, atoi(node->right), node->left);

		if (exit)
		{
			delete_exit(ses, atoi(node->right), exit);
		}
	}
	deletenode_list(ses, node, LIST_MAP);
}

DO_MAP(map_unlink)
{
	struct exit_data *exit1;
	struct exit_data *exit2;
	struct listnode *node;

	CHECK_INSIDE();

	node = searchnode_list(ses->list[LIST_PATHDIR], left);

	exit1 = find_exit(ses, ses->map->in_room, left);

	if (exit1 == NULL)
	{
		tintin_printf2(ses, "#MAP UNLINK: No exit with that name found");

		return;
	}

	if (*right == 'b')
	{
		if (node)
		{
			exit2 = find_exit(ses, exit1->vnum, node->right);

			if (exit2)
			{
				delete_exit(ses, exit1->vnum, exit2);
			}
		}
	}

	delete_exit(ses, ses->map->in_room, exit1);

	tintin_printf2(ses, "#MAP UNLINK: Exit deleted.");
}

DO_MAP(map_run)
{
	CHECK_INSIDE();

	shortest_path(ses, TRUE, left, right);
}

DO_MAP(map_write)
{
	FILE *file;
	struct exit_data *exit;
	int vnum;

	CHECK_MAP();

	if (*left == 0 || (file = fopen(left, "w")) == NULL)
	{
		tintin_printf(ses, "#ERROR: #WRITEMAP {%s} - COULDN'T OPEN FILE TO WRITE.", left);

		return;
	}

	fprintf(file, "C %d\n\n", MAX_ROOM);

	fprintf(file, "F %d\n\n", ses->map->flags);

	fprintf(file, "I %d\n\n", ses->map->in_room);

	fprintf(file, "L %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n\n",
		ses->map->legend[0],  ses->map->legend[1],  ses->map->legend[2],
		ses->map->legend[3],  ses->map->legend[4],  ses->map->legend[5],
		ses->map->legend[6],  ses->map->legend[7],  ses->map->legend[8],
		ses->map->legend[9],  ses->map->legend[10], ses->map->legend[11],
		ses->map->legend[12], ses->map->legend[13], ses->map->legend[14],
		ses->map->legend[15], ses->map->legend[16]);

	for (vnum = 0 ; vnum < MAX_ROOM ; vnum++)
	{
		if (ses->map->room_list[vnum])
		{
			fprintf(file, "\nR {%5d} {%d} {%s} {%s} {%s}\n",
				ses->map->room_list[vnum]->vnum,
				ses->map->room_list[vnum]->flags,
				ses->map->room_list[vnum]->color,
				ses->map->room_list[vnum]->name,
				ses->map->room_list[vnum]->symbol);

			for (exit = ses->map->room_list[vnum]->f_exit ; exit ; exit = exit->next)
			{
				fprintf(file, "E {%5d} {%s} {%s} {%d}\n",
					exit->vnum,
					exit->name,
					exit->cmd,
					exit->dir);
			}
		}
	}

	fclose(file);

	show_message(ses, LIST_MAP, "#MAP: Map file written to {%s}.", left);
}

void create_map(struct session *ses)
{
	int index;

	if (ses->map)
	{
		delete_map(ses);
	}

	ses->map = calloc(1, sizeof(struct map_data));

	ses->map->flags = MAP_FLAG_ASCIIGRAPHICS;

	create_room(ses, "{1} {0} {<178>} {1} { }");

	for (index = 0 ; index < 17 ; index++)
	{
		sprintf(ses->map->legend[index], "%c", map_palet0[index]);
	}
}

void delete_map(struct session *ses)
{
	int index;

	for (index = 1 ; index < MAX_ROOM ; index++)
	{
		if (ses->map->room_list[index])
		{
			delete_room(ses, index, FALSE);
		}
	}
	free(ses->map);

	ses->map = NULL;
}
		
void create_legend(struct session *ses, char *arg)
{
	int cnt;
	char buf[BUFFER_SIZE];

	for (cnt = 0 ; cnt < 17 ; cnt++)
	{
		arg = get_arg_in_braces(arg, buf, FALSE);

		substitute(ses, buf, buf, SUB_ESC);

		if (is_number(buf))
		{
			sprintf(ses->map->legend[cnt], "%c", atoi(buf));
		}
		else
		{
			sprintf(ses->map->legend[cnt], "%s", buf);
		}
	}
}


int create_room(struct session *ses, char *arg)
{
	char vnum[BUFFER_SIZE], flags[BUFFER_SIZE], color[BUFFER_SIZE], name[BUFFER_SIZE], symbol[BUFFER_SIZE];
	struct room_data *newroom;

	arg = get_arg_in_braces(arg, vnum,   FALSE);
	arg = get_arg_in_braces(arg, flags,  FALSE);
	arg = get_arg_in_braces(arg, color,  FALSE);
	arg = get_arg_in_braces(arg, name,   FALSE);
	arg = get_arg_in_braces(arg, symbol, FALSE);

	/* Backward compatbility 1.96.5 */

	if (*symbol == 0) { strcpy(symbol, " "); }


	newroom         = calloc(1, sizeof(struct room_data));

	newroom->color  = strdup(color);
	newroom->name   = strdup(name);
	newroom->symbol = strdup(symbol);

	newroom->flags  = atoi(flags);
	newroom->vnum   = atoi(vnum);

	ses->map->room_list[newroom->vnum] = newroom;

	tintin_printf2(ses, "#READMAP R %5s {%s}.", vnum, name);

	return newroom->vnum;
}

void delete_room(struct session *ses, int room, int exits)
{
	struct exit_data *exit, *exit_next;
	int cnt;

	while (ses->map->room_list[room]->f_exit)
	{
		delete_exit(ses, room, ses->map->room_list[room]->f_exit);
	}

	free(ses->map->room_list[room]->name);
	free(ses->map->room_list[room]->color);
	free(ses->map->room_list[room]);

	ses->map->room_list[room] = NULL;

	if (exits)
	{
		for (cnt = 0 ; cnt < MAX_ROOM ; cnt++)
		{
			if (ses->map->room_list[cnt])
			{
				for (exit = ses->map->room_list[cnt]->f_exit ; exit ; exit = exit_next)
				{
					exit_next = exit->next;

					if (exit->vnum == room)
					{
						delete_exit(ses, cnt, exit);
					}
				}
			}
		}
	}
}

void create_exit(struct session *ses, int room, char *arg)
{
	struct exit_data *newexit;
	struct listnode *node;

	char vnum[BUFFER_SIZE], name[BUFFER_SIZE], cmd[BUFFER_SIZE], dir[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, vnum, FALSE);
	arg = get_arg_in_braces(arg, name, FALSE);
	arg = get_arg_in_braces(arg, cmd,  TRUE);
	arg = get_arg_in_braces(arg, dir,  FALSE);

	newexit       = calloc(1, sizeof(struct exit_data));
	newexit->vnum = atoi(vnum);
	newexit->name = strdup(name);
	newexit->cmd  = strdup(cmd);
	newexit->dir  = atoi(dir);

	if (newexit->dir == 0)
	{
		if ((node = searchnode_list(ses->list[LIST_PATHDIR], name)) != NULL)
		{
			newexit->dir = atoi(node->pr);
		}
	}

	LINK(newexit, ses->map->room_list[room]->f_exit, ses->map->room_list[room]->l_exit);

	tintin_printf2(ses, "#READMAP E %5s {%s} {%s}.", vnum, name, cmd);
}

void delete_exit(struct session *ses, int room, struct exit_data *exit)
{
	free(exit->name);
	free(exit->cmd);

	UNLINK(exit, ses->map->room_list[room]->f_exit, ses->map->room_list[room]->l_exit)

	free(exit);
}

int get_map_exit(struct session *ses, char *arg)
{
	struct listnode *node;

	node = searchnode_list(ses->list[LIST_PATHDIR], arg);

	if (node)
	{
		return atoi(node->pr);
	}
	else
	{
		return 0;
	}
}

int get_map_exits(struct session *ses, int room)
{
	struct exit_data *exit;

	int exits = 0;

	for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
	{
		exits++;
	}

	return exits;
}

void follow_map(struct session *ses, char *argument)
{
	struct exit_data *exit;
	int room;

	exit = find_exit(ses, ses->map->in_room, argument);

	if (exit)
	{
		room = ses->map->in_room;

		insert_undo(ses, "%d %d %d", exit->vnum, room, 0);

		goto_room(ses, exit->vnum);

		if (HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE))
		{
			show_message(ses, LIST_MAP, "#MAP: LEAVE FLAG FOUND IN ROOM {%d}. LEAVING MAP.", ses->map->in_room);

			map_leave(ses, "", "");
		}
		else if (HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID))
		{
			exit = find_exit(ses, exit->vnum, argument);

			if (exit)
			{
				follow_map(ses, exit->name);

				return;
			}

			for (exit = ses->map->room_list[ses->map->in_room]->f_exit ; exit ; exit = exit->next)
			{
				if (exit->vnum != room)
				{
					follow_map(ses, exit->name);
	
					return;
				}
			}
		}

		show_vtmap(ses);

		return;
	}

	if (!HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) && !HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_STATIC))
	{
		char temp[BUFFER_SIZE];
		struct listnode *node;

		if ((node = searchnode_list(ses->list[LIST_PATHDIR], argument)) == NULL)
		{
			return;
		}

		for (room = 1 ; room < MAX_ROOM ; room++)
		{
			if (ses->map->room_list[room] == NULL)
			{
				break;
			}
		}

		if (room == MAX_ROOM)
		{
			tintin_printf2(ses, "#MAP: Maximum amount of rooms of %d reached. Consider updating MAX_ROOM in tintin.h", MAX_ROOM);

			return;
		}

		if (find_coord(ses, argument))
		{
			room = map_search_tar_room;

			show_message(ses, LIST_MAP, "Found linkable room {%d}", room);

			insert_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_LINK);
		}
		else
		{
			sprintf(temp, "{%d} {0} {<088>} {%d} { }", room, room);
			create_room(ses, temp);

			insert_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_CREATE|MAP_UNDO_LINK);

			tintin_printf2(ses, "#MAP: Created room %d.", room);
		}
		sprintf(temp, "{%d} {%s} {%s}", room, node->left, node->left);
		create_exit(ses, ses->map->in_room, temp);

		if (find_exit(ses, room, node->right) == NULL)
		{
			sprintf(temp, "{%d} {%s} {%s}", ses->map->in_room, node->right, node->right);
			create_exit(ses, room, temp);
		}
		goto_room(ses, room);

		show_vtmap(ses);
	}
}

void insert_undo(struct session *ses, char *format, ...)
{
	char buf[BUFFER_SIZE], *arg, dir[BUFFER_SIZE], rev[BUFFER_SIZE], flag[BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	arg = get_arg_in_braces(buf, dir, FALSE);
	arg = get_arg_in_braces(arg, rev, FALSE);
	arg = get_arg_in_braces(arg, flag, FALSE);

	addnode_list(ses->list[LIST_MAP], dir, rev, flag);

	if (ses->list[LIST_MAP]->count > 100)
	{
		deletenode_list(ses, ses->list[LIST_MAP]->f_node, LIST_MAP);
	}
}

/*
	Draws a map on a grid, breadth first improvements by Bryan Turner
*/

#define MAP_BF_SIZE 800

struct grid_node
{
	short vnum;
	short x;
	short y;
	short z;
	short size;
};

void build_map_grid_bf(short vnum, short x, short y, short z)
{
	short head, tail;
	struct grid_node *node, *temp, list[MAP_BF_SIZE];
	struct exit_data *exit;
	struct room_data *room;
	short xx, yy, zz;

	head = 0;
	tail = 1;

	node = &list[head];

	node->vnum = vnum;
	node->x    = x;
	node->y    = y;
	node->z    = z;
	node->size = 0;

	while (head != tail)
	{
		node = &list[head];

		head = (head + 1) % MAP_BF_SIZE;

		room = map_search_ses->map->room_list[node->vnum];

		if (room->size <= node->size)
		{
			continue;
		}

		room->size = node->size;

		if (node->x >= 0 && node->x <= map_grid_x && node->y >= 0 && node->y <= map_grid_y && node->z == 0)
		{
			if (map_grid[node->x][node->y] == NULL)
			{
				map_grid[node->x][node->y] = room;
			}
			else
			{
				continue;
			}
		}

		for (exit = room->f_exit ; exit ; exit = exit->next)
		{
			if (head == (tail + 1) % MAP_BF_SIZE)
			{
				break;
			}

			if (HAS_BIT(map_search_ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_HIDE))
			{
				continue;
			}

			if (exit->dir == 0)
			{
				continue;
			}

			xx = node->x + (HAS_BIT(exit->dir, MAP_EXIT_E) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_W) ? -1 : 0);
			yy = node->y + (HAS_BIT(exit->dir, MAP_EXIT_N) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_S) ? -1 : 0);
			zz = node->z + (HAS_BIT(exit->dir, MAP_EXIT_U) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_D) ? -1 : 0);

			if (xx >= 0 && xx <= map_grid_x && yy >= 0 && yy <= map_grid_y && zz == 0)
			{
				if (map_grid[xx][yy] && map_grid[xx][yy]->size <= room->size + 1)
				{
					continue;
				}
			}

			temp = &list[tail];

			temp->vnum = exit->vnum;
			temp->x    = xx;
			temp->y    = yy;
			temp->z    = zz;
			temp->size = node->size + 1;

			tail = (tail + 1) % MAP_BF_SIZE;
		}
	}
}


void build_map_grid(short room, short x, short y, short z)
{
	struct exit_data *exit;
	short coord, xx = 0, yy = 0, zz = 0;

	map_search_ses->map->room_list[room]->size = TRUE;

	if (x >= 0 && x <= map_grid_x && y >= 0 && y <= map_grid_y && z == 0)
	{
		map_grid[x][y] = map_search_ses->map->room_list[room];
	}

	for (exit = map_search_ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
	{
		if (map_search_ses->map->room_list[exit->vnum]->size)
		{
			continue;
		}

		if (HAS_BIT(map_search_ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_HIDE))
		{
			continue;
		}

		coord = exit->dir;

		if (exit->dir == 0)
		{
			continue;
		}

		xx = x + (HAS_BIT(exit->dir, MAP_EXIT_E) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_W) ? -1 : 0);
		yy = y + (HAS_BIT(exit->dir, MAP_EXIT_N) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_S) ? -1 : 0);
		zz = z + (HAS_BIT(exit->dir, MAP_EXIT_U) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_D) ? -1 : 0);

		if (xx >= 0 && xx <= map_grid_x && yy >= 0 && yy <= map_grid_y && zz == 0)
		{
			if (map_grid[xx][yy])
			{
				continue;
			}
		}

		build_map_grid(exit->vnum, xx, yy, zz);
	}
}

void create_map_grid(struct session *ses, short room, short x, short y)
{
	short vnum;

	for (vnum = 0 ; vnum < MAX_ROOM ; vnum++)
	{
		if (ses->map->room_list[vnum])
		{
			ses->map->room_list[vnum]->size = 500;
		}
	}

	memset(map_grid, 0, sizeof(map_grid));

	map_search_ses = ses;

	map_grid_y = y;
	map_grid_x = x;

	if (ses->map->room_list[room])
	{
		build_map_grid_bf(room, x / 2, y / 2, 0);
	}
}

void show_map(struct session *ses, char *left, char *right)
{
	FILE *logfile = NULL;
	char buf[BUFFER_SIZE], out[BUFFER_SIZE];
	int x, y, line;

	push_call("show_map(%p,%p,%p)",ses,left,right);

	if (sscanf(left, "%dx%d", &map_grid_x, &map_grid_y) == 2)
	{
		logfile = fopen(right, "a");

		map_grid_x = URANGE(1, map_grid_x, MAX_MAP_X);
		map_grid_y = URANGE(1, map_grid_y, MAX_MAP_Y);

		if (HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS))
		{
			map_grid_y = map_grid_y / 3;
			map_grid_x = map_grid_x / 6;
		}
	}
	else if (HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS))
	{
		map_grid_y = (get_scroll_size(ses) / 3) - 1;
		map_grid_x = (ses->cols / 6) - 1;

	}
	else
	{
		map_grid_y = get_scroll_size(ses) - 1;
		map_grid_x = ses->cols - 1;
	}

	create_map_grid(ses, ses->map->in_room, map_grid_x, map_grid_y);

	if (HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS))
	{
		for (y = map_grid_y ; y >= 0 ; y--)
		{
			for (line = 1 ; line <= 3 ; line++)
			{
				strcpy(buf, "");

				for (x = 0 ; x <= map_grid_x ; x++)
				{
					strcat(buf, draw_room(ses, map_grid[x][y], line));
				}

				substitute(ses, buf, out, SUB_COL);

				if (logfile)
				{
					fprintf(logfile, "%s\n", out);
				}
				else
				{
					tintin_puts2(ses, out);
				}
			}
		}
	}
	else
	{
		for (y = map_grid_y ; y >= 0 ; y--)
		{
			strcpy(buf, "");

			for (x = 0 ; x < map_grid_x ; x++)
			{
				strcat(buf, draw_room(ses, map_grid[x][y], 0));
			}

			for (x = strlen(buf) - 1 ; x > 0 ; x--)
			{
				if (buf[x] != ' ')
				{
					break;
				}
			}

			buf[x+1] = 0;

			substitute(ses, buf, out, SUB_COL);

			if (logfile)
			{
				fprintf(logfile, "%s\n", out);
			}
			else
			{
				tintin_puts2(ses, out);
			}
		}
	}

	if (logfile)
	{
		fflush(logfile);
	}

	pop_call();
	return;
}


void show_vtmap(struct session *ses)
{
	char buf[BUFFER_SIZE], out[BUFFER_SIZE];
	int x, y, line;

	push_call("show_vtmap(%p)",ses);

	if (ses->map == NULL || !HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP))
	{
		pop_call();
		return;
	}

	if (ses != gtd->ses || HAS_BIT(gtd->ses->flags, SES_FLAG_READMUD))
	{
		pop_call();
		return;
	}


	if (HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS))
	{
		map_grid_y = (ses->top_row - 2) / 3 - 1;
		map_grid_x = (ses->cols    - 0) / 6 - 1;
	}
	else
	{
		map_grid_y = (ses->top_row - 3);
		map_grid_x = (ses->cols - 1);
	}
	create_map_grid(ses, ses->map->in_room, map_grid_x, map_grid_y);

	save_pos(ses);
	goto_rowcol(ses, 1, 1);

	if (HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS))
	{
		for (y = map_grid_y ; y >= 0 ; y--)
		{
			for (line = 1 ; line <= 3 ; line++)
			{
				strcpy(buf, "");

				for (x = 0 ; x <= map_grid_x ; x++)
				{
					strcat(buf, draw_room(ses, map_grid[x][y], line));
				}
				substitute(ses, buf, out, SUB_COL);

				printf("%s\033[0K\r\n", out);
			}
		}
	}
	else
	{
		for (y = map_grid_y ; y >= 0 ; y--)
		{
			strcpy(buf, "");

			for (x = 0 ; x <= map_grid_x ; x++)
			{
				strcat(buf, draw_room(ses, map_grid[x][y], 0));
			}
			substitute(ses, buf, out, SUB_COL);

			printf("%s\r\n", out);
		}
	}

	restore_pos(ses);

	pop_call();
	return;
}



char *draw_room(struct session *ses, struct room_data *room, int line)
{
	struct exit_data *exit;
	static char buf[101];
	int door, exits = 0;

	push_call("draw_room(%p,%p,%p)",ses,room,line);

	if (line)
	{
		if (room == NULL)
		{
			pop_call();
			return "      ";
		}

		strcpy(buf, "<078>");

		for (exit = room->f_exit ; exit ; exit = exit->next)
		{
			door = exit->dir;

			switch (exit->dir)
			{
				case MAP_EXIT_N|MAP_EXIT_E:
					door = MAP_EXIT_NE;
					break;

				case MAP_EXIT_N|MAP_EXIT_W:
					door = MAP_EXIT_NW;
					break;

				case MAP_EXIT_S|MAP_EXIT_E:
					door = MAP_EXIT_SE;
					break;

				case MAP_EXIT_S|MAP_EXIT_W:
					door = MAP_EXIT_SW;
					break;

				case 0:
					continue;
			}

			SET_BIT(exits, door);
		}

		switch (line)
		{
			case 1:
				strcat(buf, HAS_BIT(exits, MAP_EXIT_NW) ? "\\ " : "  ");
				strcat(buf, HAS_BIT(exits, MAP_EXIT_N)  ? "|"   : " ");
				strcat(buf, HAS_BIT(exits, MAP_EXIT_U)  ? "+"   : " ");
				strcat(buf, HAS_BIT(exits, MAP_EXIT_NE) ? "/ "  : "  ");
				break;

			case 2:
				if (HAS_BIT(room->flags, ROOM_FLAG_VOID) || !HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS))
				{
					strcat(buf, HAS_BIT(exits, MAP_EXIT_W) ? "-" : " ");
				}

				if (room->vnum == ses->map->in_room)
				{
					if (!HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS))
					{
						cat_sprintf(buf, "%s[<118>#%s]<078>", room->color, room->color);
					}
					else
					{
						cat_sprintf(buf, "<118>%05d<078>", room->vnum);
					}
				}
				else
				{
					if (HAS_BIT(room->flags, ROOM_FLAG_VOID))
					{
						switch (exits)
						{
							case MAP_EXIT_N|MAP_EXIT_S:
								cat_sprintf(buf, " | ");
								break;
							case MAP_EXIT_E|MAP_EXIT_W:
								cat_sprintf(buf, "---");
								break;
							case MAP_EXIT_NE|MAP_EXIT_SW:
								cat_sprintf(buf, " / ");
								break;
							case MAP_EXIT_NW|MAP_EXIT_SE:
								cat_sprintf(buf, " \\ ");
								break;
							default:
								cat_sprintf(buf, " * ");
								break;
						}
					}
					else
					{
						if (HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS))
						{
							cat_sprintf(buf, "<128>%05d<078>", room->vnum);
						}
						else if (strip_color_strlen(room->symbol) <= 1)
						{
							cat_sprintf(buf, "%s[%s%s]<078>", room->color, room->symbol, room->color);
						}
						else
						{
							cat_sprintf(buf, "%s%-3s<078>", room->color, room->symbol);
						}
					}
				}

				if (HAS_BIT(room->flags, ROOM_FLAG_VOID) || !HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS))
				{
					strcat(buf, HAS_BIT(exits, MAP_EXIT_E) ? "--" : "  ");
				}
				else
				{
					strcat(buf, HAS_BIT(exits, MAP_EXIT_E) ? "-" : " ");
				}
				break;

			case 3:
				strcat(buf, HAS_BIT(exits, MAP_EXIT_SW) ? "/"   : " ");
				strcat(buf, HAS_BIT(exits, MAP_EXIT_D)  ? "-"   : " ");
				strcat(buf, HAS_BIT(exits, MAP_EXIT_S)  ? "| "  : "  ");
				strcat(buf, HAS_BIT(exits, MAP_EXIT_SE) ? "\\ " : "  ");
				break;
		}
		pop_call();
		return buf;
	}

	if (room == NULL)
	{
		pop_call();
		return " ";
	}

	if (room->vnum == ses->map->in_room)
	{
		if (HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS))
		{
			sprintf(buf, "<118>\033[12m%s\033[10m", ses->map->legend[16]);
		}
		else
		{
			sprintf(buf, "<118>%s", ses->map->legend[16]);
		}
		pop_call();
		return buf;
	}

	for (exit = room->f_exit ; exit ; exit = exit->next)
	{
		if (exit->dir >= MAP_EXIT_N && exit->dir <= MAP_EXIT_W)
		{
			SET_BIT(exits, exit->dir);
		}
	}

	if (HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS))
	{
		sprintf(buf, "%s\033[12m%s\033[10m", room->color, ses->map->legend[exits]);
	}
	else
	{
		sprintf(buf, "%s%s", room->color, ses->map->legend[exits]);
	}
	pop_call();
	return buf;
}


int find_room(struct session *ses, char *arg)
{
	int room, vnum, size;

	room = atoi(arg);

	if (room > 0 && room < MAX_ROOM)
	{
		if (ses->map->room_list[room])
		{
			return room;
		}
		return -1;
	}

	fill_map(ses);

	vnum = -1;
	size = MAP_SEARCH_SIZE + 1;

	for (room = 0 ; room < MAX_ROOM ; room++)
	{
		if (ses->map->room_list[room] && match(ses, ses->map->room_list[room]->name, arg))
		{
			show_debug(ses, LIST_MAP, "%-5d %-20s (%3d)", room, ses->map->room_list[room]->name, ses->map->room_list[room]->size);

			if (ses->map->room_list[room]->size < size)
			{
				size = ses->map->room_list[room]->size;
				vnum = room;
			}
		}
	}
	return vnum;
}

void goto_room(struct session *ses, int room)
{
	ses->map->in_room = room;

	check_all_events(ses, "MAP ENTER ROOM");
}

struct exit_data *find_exit(struct session *ses, int room, char *arg)
{
	struct exit_data *exit;

	for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
	{
		if (!strcmp(exit->name, arg) || exit->vnum == atoi(arg))
		{
			return exit;
		}
	}
	return NULL;
}

int revdir(int dir)
{
	int revdir = 0;

	if (HAS_BIT(dir, MAP_EXIT_N))
	{
		SET_BIT(revdir, MAP_EXIT_S);
	}

	if (HAS_BIT(dir, MAP_EXIT_E))
	{
		SET_BIT(revdir, MAP_EXIT_W);
	}

	if (HAS_BIT(dir, MAP_EXIT_S))
	{
		SET_BIT(revdir, MAP_EXIT_N);
	}

	if (HAS_BIT(dir, MAP_EXIT_W))
	{
		SET_BIT(revdir, MAP_EXIT_E);
	}

	if (HAS_BIT(dir, MAP_EXIT_U))
	{
		SET_BIT(revdir, MAP_EXIT_D);
	}

	if (HAS_BIT(dir, MAP_EXIT_D))
	{
		SET_BIT(revdir, MAP_EXIT_U);
	}

	return revdir;
}

struct exit_data *find_exit_dir(struct session *ses, int room, short dir)
{
	struct exit_data *exit;

	for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
	{
		if (exit->dir == dir)
		{
			return exit;
		}
	}

	if (get_map_exits(ses, room) != 2)
	{
		show_debug(map_search_ses, LIST_MAP, "#ERROR: ROOM {%d} IS AN INVALID VOID ROOM.", room);

		return NULL;
	}

	if (ses->map->room_list[room]->f_exit->dir != revdir(dir))
	{
		return ses->map->room_list[room]->f_exit;
	}
	else
	{
		return ses->map->room_list[room]->l_exit;
	}
}

/*
	Fast, short distance node weighted algorithms - Igor
*/

short follow_void(struct session *ses, short from, short room, short dir)
{
	struct exit_data *exit;

	map_search_last_room = from;

	if (room == map_search_tar_room)
	{
		return room;
	}

	exit = find_exit_dir(ses, room, dir);

	if (exit)
	{
		if (HAS_BIT(ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_VOID))
		{
			return follow_void(ses, room, exit->vnum, exit->dir);
		}
		else
		{
			return exit->vnum;
		}
	}
	else
	{
		return room;
	}
}

void search_path(short room, short size, short dir)
{
	struct exit_data *exit;

	map_search_ses->map->room_list[room]->size = size;

	if (size >= map_search_max_size)
	{
		return;
	}

	if (room == map_search_tar_room)
	{
		map_search_exit_best = map_search_exit_node;
		map_search_max_size  = size;
	}
	else
	{
		if (HAS_BIT(map_search_ses->map->room_list[room]->flags, ROOM_FLAG_AVOID))
		{
			show_debug(map_search_ses, LIST_MAP, "Avoiding room %d.", room);

			return;
		}

		if (HAS_BIT(map_search_ses->map->room_list[room]->flags, ROOM_FLAG_VOID))
		{
			exit = find_exit_dir(map_search_ses, room, dir);

			if (exit && map_search_ses->map->room_list[exit->vnum]->size > size)
			{
				search_path(exit->vnum, size, exit->dir);
			}
		}
		else
		{
			size++;

			for (exit = map_search_ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
			{
				if (map_search_ses->map->room_list[exit->vnum]->size > size)
				{
					search_path(exit->vnum, size, exit->dir);
				}
			}
		}
	}
}

void fill_map(struct session *ses)
{
	int vnum, room;
	struct exit_data *exit;

	map_search_ses       = ses;
	map_search_tar_room  = -1;
	map_search_max_size  = MAP_SEARCH_SIZE;

	for (vnum = 0 ; vnum < MAX_ROOM ; vnum++)
	{
		if (ses->map->room_list[vnum])
		{
			ses->map->room_list[vnum]->size = map_search_max_size;
		}
	}

	if (ses->map->in_room == 0)
	{
		return;
	}

	room = ses->map->in_room;

	ses->map->room_list[room]->size = 0;

	for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
	{
		if (ses->map->room_list[exit->vnum])
		{
			search_path(exit->vnum, 1, exit->dir);
		}
	}

	return;
}

void shortest_path(struct session *ses, int run, char *left, char *right)
{
	char time[BUFFER_SIZE], name[BUFFER_SIZE];
	struct exit_data *exit;
	int room, vnum, wait;

	if (HAS_BIT(ses->flags, SES_FLAG_MAPPING))
	{
		tintin_printf2(ses, "#SHORTEST PATH: You have to stop #PATH TRACKING first.");

		return;
	}

	kill_list(ses, LIST_PATH);

	room = find_room(ses, left);

	if (room == -1)
	{
		tintin_printf2(ses, "Couldn't find the room named '%s'", left);
		return;
	}

	if (room == ses->map->in_room)
	{
		tintin_printf2(ses, "Already there.");
		return;
	}

	map_search_exit_best = NULL;
	map_search_ses       = ses;
	map_search_tar_room  = room;
	map_search_max_size  = MAP_SEARCH_SIZE;

	for (vnum = 0 ; vnum < MAX_ROOM ; vnum++)
	{
		if (ses->map->room_list[vnum])
		{
			ses->map->room_list[vnum]->size = map_search_max_size;
		}
	}

	room = ses->map->in_room;

	ses->map->room_list[room]->size = 0;

	for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
	{
		if (ses->map->room_list[exit->vnum])
		{
			map_search_exit_node = exit;
			search_path(exit->vnum, 1, exit->dir);
		}
	}

	if (map_search_exit_best == NULL)
	{
		tintin_printf2(ses, "#SHORTEST PATH: No path found.");
		return;
	}

	while (TRUE)
	{
		ses->map->room_list[room]->size = 0;

		for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
		{
			if (ses->map->room_list[exit->vnum])
			{
				map_search_exit_node = exit;
				search_path(exit->vnum, 1, exit->dir);
			}
		}

		if (map_search_exit_best == NULL)
		{
			return;
		}

		addnode_list(ses->list[LIST_PATH], map_search_exit_best->cmd, "", "0");

		if (HAS_BIT(ses->map->room_list[map_search_exit_best->vnum]->flags, ROOM_FLAG_VOID))
		{
			room = follow_void(ses, room, map_search_exit_best->vnum, map_search_exit_best->dir);
		}
		else
		{
			room = map_search_exit_best->vnum;
		}

		if (room == map_search_tar_room)
		{
			break;
		}
	}

	if (run)
	{
		if (*right)
		{
			wait = 0;

			while (ses->list[LIST_PATH]->f_node)
			{
				sprintf(time, "%lld", utime() + wait);
				sprintf(name, "%lld", gtd->time);

				wait += (long long) (get_number(ses, right) * 1000000LL);

				updatenode_list(ses, name, ses->list[LIST_PATH]->f_node->left, time, LIST_DELAY);

				deletenode_list(ses, ses->list[LIST_PATH]->f_node, LIST_PATH);
			}
		}
		else
		{
			while (ses->list[LIST_PATH]->f_node)
			{
				pre_parse_input(ses, ses->list[LIST_PATH]->f_node->left, SUB_NONE);

				deletenode_list(ses, ses->list[LIST_PATH]->f_node, LIST_PATH);
			}
		}
	}
}

/*
	Do a coordinate based search.
*/

int find_coord(struct session *ses, char *arg)
{
	int vnum, coord;

	map_grid_x = 0;
	map_grid_y = 0;
	map_grid_z = 0;

	map_search_ses      = ses;
	map_search_tar_room = 0;

	coord = get_map_exit(ses, arg);

	if (coord == 0)
	{
		return 0;
	}

	map_grid_y += (HAS_BIT(coord, MAP_EXIT_N) ? 1 : HAS_BIT(coord, MAP_EXIT_S) ? -1 : 0);
	map_grid_x += (HAS_BIT(coord, MAP_EXIT_E) ? 1 : HAS_BIT(coord, MAP_EXIT_W) ? -1 : 0);
	map_grid_z += (HAS_BIT(coord, MAP_EXIT_U) ? 1 : HAS_BIT(coord, MAP_EXIT_D) ? -1 : 0);

	for (vnum = 0 ; vnum < MAX_ROOM ; vnum++)
	{
		if (ses->map->room_list[vnum])
		{
			ses->map->room_list[vnum]->size = MAP_SEARCH_SIZE;
		}
	}

	search_coord_bf(ses->map->in_room);

	if (ses->map->room_list[map_search_tar_room])
	{
		if (HAS_BIT(ses->map->room_list[map_search_tar_room]->flags, ROOM_FLAG_STATIC))
		{
			show_message(ses, LIST_MAP, "#MAP: Linkable room is marked static. Creating overlapping room instead.");

			return 0;
		}
	}
	return map_search_tar_room;
}

void search_coord_bf(int vnum)
{
	short head, tail;
	struct grid_node *node, *temp, list[MAP_BF_SIZE];
	struct exit_data *exit;
	struct room_data *room;
	short xx, yy, zz;

	head = 0;
	tail = 1;

	node = &list[head];

	node->vnum = vnum;
	node->x    = 0;
	node->y    = 0;
	node->z    = 0;
	node->size = 0;

	while (head != tail)
	{
		node = &list[head];

		head = (head + 1) % MAP_BF_SIZE;

		room = map_search_ses->map->room_list[node->vnum];

		if (room->size <= node->size)
		{
			continue;
		}

		room->size = node->size;

		if (node->x == map_grid_x && node->y == map_grid_y && node->z == map_grid_z)
		{
			map_search_tar_room = node->vnum;

			return;
		}

		for (exit = room->f_exit ; exit ; exit = exit->next)
		{
			if (head == (tail + 1) % MAP_BF_SIZE)
			{
				break;
			}

			if (HAS_BIT(map_search_ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_HIDE))
			{
				continue;
			}

			if (exit->dir == 0)
			{
				continue;
			}

			xx = node->x + (HAS_BIT(exit->dir, MAP_EXIT_E) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_W) ? -1 : 0);
			yy = node->y + (HAS_BIT(exit->dir, MAP_EXIT_N) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_S) ? -1 : 0);
			zz = node->z + (HAS_BIT(exit->dir, MAP_EXIT_U) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_D) ? -1 : 0);

			temp = &list[tail];

			temp->vnum = exit->vnum;
			temp->x    = xx;
			temp->y    = yy;
			temp->z    = zz;
			temp->size = node->size + 1;

			tail = (tail + 1) % MAP_BF_SIZE;
		}
	}
}

void explore_path(struct session *ses, int run, char *left, char *right)
{
	char time[BUFFER_SIZE], name[BUFFER_SIZE];
	struct exit_data *exit;
	int room, vnum, wait;

	if (HAS_BIT(ses->flags, SES_FLAG_MAPPING))
	{
		tintin_printf2(ses, "#MAP EXPLORE: You have to stop #PATH TRACKING first.");

		return;
	}

	kill_list(ses, LIST_PATH);

	room = ses->map->in_room;

	exit = find_exit(ses, room, left);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: There's no exit named '%s'.", left);
		return;
	}

	vnum = exit->vnum;

	addnode_list(ses->list[LIST_PATH], exit->cmd, "", "0");

	while (get_map_exits(ses, vnum) == 2)
	{
		if (ses->map->room_list[vnum]->f_exit->vnum != room)
		{
			exit = ses->map->room_list[vnum]->f_exit;
		}
		else
		{
			exit = ses->map->room_list[vnum]->l_exit;
		}

		addnode_list(ses->list[LIST_PATH], exit->cmd, "", "0");

		if (HAS_BIT(ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_VOID))
		{
			vnum = follow_void(ses, room, exit->vnum, exit->dir);
			room = map_search_last_room;
		}
		else
		{
			room = vnum;
			vnum = exit->vnum;
		}
	}

	if (run)
	{
		if (*right)
		{
			wait = 0;

			while (ses->list[LIST_PATH]->f_node)
			{
				sprintf(time, "%lld", utime() + wait);
				sprintf(name, "%lld", gtd->time);

				wait += (long long) (get_number(ses, right) * 1000000LL);

				updatenode_list(ses, name, ses->list[LIST_PATH]->f_node->left, time, LIST_DELAY);

				deletenode_list(ses, ses->list[LIST_PATH]->f_node, LIST_PATH);
			}
		}
		else
		{
			while (ses->list[LIST_PATH]->f_node)
			{
				pre_parse_input(ses, ses->list[LIST_PATH]->f_node->left, SUB_NONE);

				deletenode_list(ses, ses->list[LIST_PATH]->f_node, LIST_PATH);
			}
		}
	}
}
