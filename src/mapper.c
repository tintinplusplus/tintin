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

void show_map(struct session *ses, char *arg1, char *arg2);
void create_legend(struct session *ses, char *arg);

int                 map_grid_x;
int                 map_grid_y;
int                 map_grid_z;

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


DO_COMMAND(do_map)
{
	int cnt;
	char arg1[BUFFER_SIZE], arg2[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, arg1, FALSE);

	if (*arg1 == 0)
	{
		tintin_printf2(ses, "Available map options");
		tintin_printf2(ses, "");
		tintin_printf2(ses, "#map color    <field>     <color>      (set the color for given field)");
		tintin_printf2(ses, "#map create                            (creates the initial map)");
		tintin_printf2(ses, "#map destroy                           (destroys the map)");
		tintin_printf2(ses, "#map delete   <direction>              (delete the room at given dir)");
		tintin_printf2(ses, "#map dig      <direction> [new]        (creates a new room)");
		tintin_printf2(ses, "#map exit     <direction>  <command>   (sets the exit command)");
		tintin_printf2(ses, "#map exitdir  <direction> <pathdir>    (set the exit direction)");
		tintin_printf2(ses, "#map explore  <direction>              (saves path to #path)");
		tintin_printf2(ses, "#map info                              (info on map and current room)");
		tintin_printf2(ses, "#map insert   <direction>  [roomflag]  (insert a new room)");
		tintin_printf2(ses, "#map jump     <x> <y>                  (go to given coordinate)");
		tintin_printf2(ses, "#map find     <room name> [exits]      (saves path to #path)");
		tintin_printf2(ses, "#map flag     <map flag>               (set map wide flags)");
		tintin_printf2(ses, "#map get      <option>     <variable>  (get various values)");
		tintin_printf2(ses, "#map goto     <room name> [exits]      (moves you to given room)");
		tintin_printf2(ses, "#map leave                             (leave the map, return with goto)");
		tintin_printf2(ses, "#map legend   <symbols>                (sets the map legend)");
		tintin_printf2(ses, "#map link     <direction>  <room name> (links 2 rooms)");
		tintin_printf2(ses, "#map list     <location>               (shows list of matching rooms)");
		tintin_printf2(ses, "#map map      <radius>                 (shows an ascii map)");
		tintin_printf2(ses, "#map name     <room name>              (set the room's name)");
		tintin_printf2(ses, "#map read     <filename>               (load your map from a file)");
		tintin_printf2(ses, "#map roomflag <room flag>              (set room based flags)");
		tintin_printf2(ses, "#map set      <option>     <value>     (set various values)");
		tintin_printf2(ses, "#map return                            (return to last room.)");
		tintin_printf2(ses, "#map run      <room name>  <delay>     (run to given room)");
		tintin_printf2(ses, "#map travel   <direction>  <delay>     (run in given direction)");
		tintin_printf2(ses, "#map undo                              (undo last move)");
		tintin_printf2(ses, "#map unlink   <direction> [both]       (deletes an exit)");
		tintin_printf2(ses, "#map write    <filename>               (save the map to a file)");

		return ses;
	}
	else
	{
		for (cnt = 0 ; *map_table[cnt].name ; cnt++)
		{
			if (is_abbrev(arg1, map_table[cnt].name))
			{
				if (map_table[cnt].check > 0 && ses->map == NULL)
				{
					tintin_printf2(ses, "#MAP: This session has no map data. Use #map create or #map read to create one.");

					return ses;
				}
				if (map_table[cnt].check > 1 && ses->map->room_list[ses->map->in_room] == NULL)
				{
					tintin_printf2(ses, "#MAP: You are not inside the map. Use #map goto to enter it.");

					return ses;
				}
				*arg1 = *arg2 = 0;

				map_table[cnt].map (ses, arg, arg1, arg2);
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
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	if (is_abbrev(arg1, "ROOMS"))
	{
		RESTRING(ses->map->room_color, arg2);
	}
	else if (is_abbrev(arg1, "EXITS"))
	{
		RESTRING(ses->map->exit_color, arg2);
	}
	else
	{
		tintin_printf2(ses, "#SYNTAX: #MAP COLOR {ROOM|EXIT} {COLOR CODE}");
		return;
	}

	tintin_printf2(ses, "#MAP: %s color set to: %s", arg1, arg2);
}

DO_MAP(map_create)
{
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	create_map(ses, arg1);

	tintin_printf2(ses, "#MAP: %d room map created, use #map goto 1, to proceed", ses->map->size);
}

DO_MAP(map_delete)
{
	int room;
	struct exit_data *exit;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	exit = find_exit(ses, ses->map->in_room, arg1);

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
	delete_map(ses);

	tintin_printf2(ses, "#MAP: Map destroyed.");
}

DO_MAP(map_dig)
{
	int room;
	char temp[BUFFER_SIZE];
	struct exit_data *exit;
	struct listnode *node;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	for (room = 1 ; room < ses->map->size ; room++)
	{
		if (ses->map->room_list[room] == NULL)
		{
			break;
		}
	}

	exit = find_exit(ses, ses->map->in_room, arg1);

	if (exit)
	{
		tintin_printf2(ses, "#MAP: There is already a room in that direction.");

		return;
	}

	if (room == ses->map->size)
	{
		tintin_printf2(ses, "#MAP: Maximum amount of rooms of %d reached.", ses->map->size);
		return;
	}

	if (strcasecmp(arg2, "new") && find_coord(ses, arg1))
	{
		room = map_search_tar_room;

		show_message(ses, -1, "Found linkable room {%d}", room);
	}
	else
	{
		sprintf(temp, "{%d} {0} {} {%d} { }", room, room);
		create_room(ses, temp);
	}
	sprintf(temp, "{%d} {%s} {%s}", room, arg1, arg1);
	create_exit(ses, ses->map->in_room, temp);

	if ((node = search_node_list(ses->list[LIST_PATHDIR], arg1)) != NULL)
	{
		if (find_exit(ses, room, node->right) == NULL)
		{
			sprintf(temp, "{%d} {%s} {%s}", ses->map->in_room, node->right, node->right);
			create_exit(ses, room, temp);
		}
	}
	tintin_printf(ses, "#MAP: Created room {%d}.", room);
}


DO_MAP(map_exitcmd)
{
	char temp[BUFFER_SIZE];
	struct exit_data *exit;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	exit = find_exit(ses, ses->map->in_room, arg1);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: Exit {%s} not found.", arg1);

		return;
	}

	sprintf(temp, "{%d} {%s} {%s}", exit->vnum, exit->name, arg2);

	delete_exit(ses, ses->map->in_room, exit);

	create_exit(ses, ses->map->in_room, temp);

	tintin_printf2(ses, "#MAP: Exit {%s} set to {%s}.", arg1, arg2);
}

DO_MAP(map_exitdir)
{
	struct exit_data *exit;
	struct listnode *node;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	exit = find_exit(ses, ses->map->in_room, arg1);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: Exit {%s} not found.", arg1);

		return;
	}

	if ((node = search_node_list(ses->list[LIST_PATHDIR], arg2)) == NULL)
	{
		tintin_printf2(ses, "#MAP: Pathdir {%s} not found.", arg2);

		return;
	}

	exit->dir = atoi(node->pr);

	tintin_printf(ses, "#MAP: The direction of exit {%s} has been set to {%s}.", arg1, arg2);
}

DO_MAP(map_explore)
{
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	explore_path(ses, FALSE, arg1, "");
}

DO_MAP(map_find)
{
	shortest_path(ses, FALSE, arg1, arg);
}

DO_MAP(map_flag)
{
	int flag;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1)
	{
		if (is_abbrev(arg1, "static"))
		{
			flag = MAP_FLAG_STATIC;
		}
		else if (is_abbrev(arg1, "vtgraphics"))
		{
			flag = MAP_FLAG_VTGRAPHICS;
		}
		else if (is_abbrev(arg1, "vtmap"))
		{
			flag = MAP_FLAG_VTMAP;
		}
		else if (is_abbrev(arg1, "asciigraphics"))
		{
			flag = MAP_FLAG_ASCIIGRAPHICS;
		}
		else if (is_abbrev(arg1, "asciivnums"))
		{
			flag = MAP_FLAG_ASCIIVNUMS;
		}
		else
		{
			tintin_printf2(ses, "#MAP: Invalid flag {%s}.", arg1);

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

	if (is_abbrev("on", arg2))
	{
		SET_BIT(ses->map->flags, flag);
	}
	else if (is_abbrev("off", arg2))
	{
		DEL_BIT(ses->map->flags, flag);
	}
	else
	{
		TOG_BIT(ses->map->flags, flag);
	}

	if (is_abbrev(arg1, "static"))
	{
		tintin_printf2(ses, "#MAP: Static flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "vtgraphics"))
	{
		tintin_printf2(ses, "#MAP: VTgraphics flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "vtmap"))
	{
		tintin_printf2(ses, "#MAP: VTmap flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP) ? "on" : "off");

		show_vtmap(ses);
	}
	else if (is_abbrev(arg1, "asciigraphics"))
	{
		tintin_printf2(ses, "#MAP: AsciiGraphics flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "asciivnums"))
	{
		tintin_printf2(ses, "#MAP: AsciiVnums flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS) ? "on" : "off");
	}
}

DO_MAP(map_get)
{
	struct room_data *room = ses->map->room_list[ses->map->in_room];
	struct exit_data *exit;
	char buf[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1 == 0 || *arg2 == 0)
	{
		tintin_printf2(ses, "  roomvnum: %d", room->vnum);
		tintin_printf2(ses, "  roomarea: %s", room->area);
		tintin_printf2(ses, " roomcolor: %s", room->color);
		tintin_printf2(ses, "  roomdesc: %s", room->desc);
		tintin_printf2(ses, " roomflags: %d", room->flags);
		tintin_printf2(ses, "  roomname: %s", room->name);
		tintin_printf2(ses, "  roomnote: %s", room->note);
		tintin_printf2(ses, "roomsymbol: %s", room->symbol);
	}
	else
	{
		if (is_abbrev(arg1, "roomarea"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%s", room->area);
		}
		else if (is_abbrev(arg1, "roomcolor"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%s", room->color);
		}
		else if (is_abbrev(arg1, "roomdesc"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%s", room->desc);
		}
		else if (is_abbrev(arg1, "roomflags"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%d", room->flags);
		}
		else if (is_abbrev(arg1, "roomname"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%s", room->name);
		}
		else if (is_abbrev(arg1, "roomnote"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%s", room->note);
		}
		else if (is_abbrev(arg1, "roomsymbol"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%s", room->symbol);
		}
		else if (is_abbrev(arg1, "roomvnum"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%d", room->vnum);
		}
		else if (is_abbrev(arg1, "roomexits"))
		{
			buf[0] = 0;

			for (exit = room->f_exit ; exit ; exit = exit->next)
			{
				cat_sprintf(buf, "{%s}{%d}", exit->name, exit->vnum);
			}
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%s", buf);
		}
		else
		{
			show_message(ses, -1, "#MAP GET: unknown option: %s.", arg1);
		}
	}
}

DO_MAP(map_goto)
{
	int room;

	room = find_room(ses, arg);

	if (room != -1)
	{
		add_undo(ses, "%d %d %d", room, ses->map->in_room, 0);

		goto_room(ses, room);
		show_message(ses, -1, "#MAP: Location set to room {%s}.", ses->map->room_list[room]->name);
	}
	else
	{
		show_message(ses, -1, "#MAP: Couldn't find room {%s}.", arg1);
	}
}

DO_MAP(map_info)
{
	int room, cnt, exits;
	struct exit_data *exit;

	for (room = cnt = exits = 0 ; room < ses->map->size ; room++)
	{
		if (ses->map->room_list[room])
		{
			cnt++;

			exits += get_map_exits(ses, room);
		}
	}

	tintin_printf2(ses, "%-20s %d/%d", "Total rooms:",    cnt, ses->map->size);
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

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	for (room = 1 ; room < ses->map->size ; room++)
	{
		if (ses->map->room_list[room] == NULL)
		{
			break;
		}
	}

	exit = find_exit(ses, ses->map->in_room, arg1);

	node = search_node_list(ses->list[LIST_PATHDIR], arg1);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: There is no room in that direction.");

		return;
	}

	if (room == ses->map->size)
	{
		tintin_printf2(ses, "#MAP: Maximum amount of rooms of %d reached.", ses->map->size);
		return;
	}

	if (node == NULL)
	{
		tintin_printf2(ses, "#MAP: Given direction must be a pathdir.");
		return;
	}

	in_room = ses->map->in_room;
	to_room = exit->vnum;

	sprintf(temp, "{%d} {0} {} {%d} { }", room, room);
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

	if (*arg2)
	{
		char buf[BUFFER_SIZE];

		strcpy(buf, arg2);
		ses->map->in_room = room;
		map_roomflag(ses, buf, arg1, arg2);
		ses->map->in_room = in_room;
	}
	tintin_printf2(ses, "#MAP: Inserted room {%d}.", room);
}

DO_MAP(map_jump)
{
	int vnum;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	map_grid_x = 0;
	map_grid_y = 0;
	map_grid_z = 0;

	map_search_ses       = ses;
	map_search_tar_room  = 0;

	map_grid_x += atoi(arg1);
	map_grid_y += atoi(arg2);

	for (vnum = 0 ; vnum < ses->map->size ; vnum++)
	{
		if (ses->map->room_list[vnum])
		{
			ses->map->room_list[vnum]->size = MAP_SEARCH_SIZE;
		}
	}

	search_coord_bf(ses->map->in_room);

	if (map_search_tar_room)
	{
		add_undo(ses, "%d %d %d", map_search_tar_room, ses->map->in_room, 0);

		goto_room(ses, map_search_tar_room);

		show_message(ses, -1, "#MAP: Location set to room {%s}.", ses->map->room_list[map_search_tar_room]->name);
	}
	else
	{
		show_message(ses, -1, "#MAP: Couldn't find room at {%s} {%s}.", arg1, arg2);
	}
}

DO_MAP(map_leave)
{
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
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1 == 0)
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
		create_legend(ses, arg1);
	}
}

DO_MAP(map_link)
{
	char arg3[BUFFER_SIZE];
	struct listnode *node;
	struct exit_data *exit;
	int room;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);

	if ((room = find_room(ses, arg)) == -1)
	{
		tintin_printf2(ses, "#MAP: Couldn't find room {%s}.", arg2);
		return;
	}

	exit = find_exit(ses, ses->map->in_room, arg1);

	if (exit)
	{
		delete_exit(ses, ses->map->in_room, exit);
	}

	sprintf(arg3, "{%d} {%s} {%s}", room, arg1, arg1);
	create_exit(ses, ses->map->in_room, arg3);

	if ((node = search_node_list(ses->list[LIST_PATHDIR], arg1)) != NULL)
	{
		if (find_exit(ses, room, node->right) == NULL)
		{
			sprintf(arg3, "{%d} {%s} {%s}", ses->map->in_room, node->right, node->right);
			create_exit(ses, room, arg3);
		}
	}
	tintin_printf2(ses, "#MAP: Linked room {%s} to {%s}.", ses->map->room_list[ses->map->in_room]->name, ses->map->room_list[room]->name);
}

DO_MAP(map_list)
{
	char arg3[BUFFER_SIZE], arg4[BUFFER_SIZE], arg5[BUFFER_SIZE];
	int room;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg4, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg5, GET_ALL, SUB_VAR|SUB_FUN);

	fill_map(ses);

	for (room = 0 ; room < ses->map->size ; room++)
	{
		if (match_room(ses, room, arg1, arg2, arg3, arg4, arg5))
		{
			tintin_printf2(ses, "vnum: %5d distance: %5d name: %s", ses->map->room_list[room]->vnum, ses->map->room_list[room]->size, ses->map->room_list[room]->name);
		}
	}
}

DO_MAP(map_map)
{
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	if (!strcasecmp("vt", arg1))
	{
		show_vtmap(ses);
	}
	else
	{
		show_map(ses, arg1, arg2);
	}
}

DO_MAP(map_move)
{
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	follow_map(ses, arg1);
}

DO_MAP(map_name)
{
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	free(ses->map->room_list[ses->map->in_room]->name);
	ses->map->room_list[ses->map->in_room]->name = strdup(arg1);
}

DO_MAP(map_read)
{
	FILE *myfile;
	struct exit_data *exit;
	char buffer[BUFFER_SIZE], *cptr;
	int room = 0;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	if ((myfile = fopen(arg1, "r")) == NULL)
	{
		tintin_printf(ses, "#MAP: Map file {%s} not found.", arg1);

		return;
	}

	gtd->quiet++;

	if (ses->map == NULL)
	{
		ses->map = (struct map_data *) calloc(1, sizeof(struct map_data));
	}

	while (fgets(buffer, BUFFER_SIZE - 1, myfile))
	{
		cptr = strchr(buffer, '\r'); /* For map files editor on Windows systems. */

		if (!cptr)
		{
			cptr = strchr(buffer, '\n');
		}

		if (cptr)
		{
			*cptr = 0;
		}

		switch (buffer[0])
		{
			case 'C':
				switch (buffer[1])
				{
					case ' ':
						create_map(ses, buffer + 2);
						break;

					case 'E':
						ses->map->exit_color = strdup(buffer + 3);
						break;

					case 'R':
						ses->map->room_color = strdup(buffer + 3);
						break;
				}
				break;

			case 'E':
				create_exit(ses, room, buffer + 2);
				break;

			case 'F':
				ses->map->flags = atoi(buffer + 2);
				break;

			case 'I':
				ses->map->last_room = atoi(buffer + 2);
				break;

			case 'L':
				create_legend(ses, buffer + 2);
				break;

			case 'R':
				room = create_room(ses, buffer + 2);
				break;

			case '#':
				buffer[0] = gtd->tintin_char;
				ses = script_driver(ses, -1, buffer);
				break;

			case  0:
			case 13:
				break;

			default:
				gtd->quiet--;

				tintin_printf2(ses, "#MAP: Invalid command '%d'. Aborting read.", buffer[0]);

				fclose(myfile);

				delete_map(ses);

				return;				
		}
	}

	gtd->quiet--;

	fclose(myfile);

	for (room = 0 ; room < ses->map->size ; room++)
	{
		if (ses->map->room_list[room] == NULL)
		{
			continue;
		}

		for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
		{
			if (exit->vnum < 0 || exit->vnum >= ses->map->size || ses->map->room_list[exit->vnum] == NULL)
			{
				tintin_printf2(ses, "#MAP READ: Room %d - invalid exit '%s' to room %d.", room, exit->name, exit->vnum);

				delete_exit(ses, room, exit);

				if (ses->map->room_list[room]->f_exit)
				{
					exit = ses->map->room_list[room]->f_exit;
				}
				else
				{
					break;
				}
			}
		}
	}

	show_message(ses, -1, "#MAP READ: Map file {%s} loaded.", arg1);


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

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1)
	{
		if (is_abbrev(arg1, "avoid"))
		{
			flag = ROOM_FLAG_AVOID;
		}
		else if (is_abbrev(arg1, "hide"))
		{
			flag = ROOM_FLAG_HIDE;
		}
		else if (is_abbrev(arg1, "leave"))
		{
			flag = ROOM_FLAG_LEAVE;
		}
		else if (is_abbrev(arg1, "void"))
		{
			flag = ROOM_FLAG_VOID;
		}
		else if (is_abbrev(arg1, "static"))
		{
			flag = ROOM_FLAG_STATIC;
		}
		else
		{
			tintin_printf2(ses, "#MAP: Invalid room flag {%s}.", arg1);

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

	if (is_abbrev("on", arg2))
	{
		SET_BIT(ses->map->room_list[ses->map->in_room]->flags, flag);
	}
	else if (is_abbrev("off", arg2))
	{
		DEL_BIT(ses->map->room_list[ses->map->in_room]->flags, flag);
	}
	else
	{
		TOG_BIT(ses->map->room_list[ses->map->in_room]->flags, flag);
	}	


	if (is_abbrev(arg1, "avoid"))
	{
		tintin_printf2(ses, "#MAP: Avoid flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_AVOID) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "hide"))
	{
		tintin_printf2(ses, "#MAP: Hide flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_HIDE) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "leave"))
	{
		tintin_printf2(ses, "#MAP: Leave flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "void"))
	{
		tintin_printf2(ses, "#MAP: Void flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "static"))
	{
		tintin_printf2(ses, "#MAP: Static flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_STATIC) ? "on" : "off");
	}
}

DO_MAP(map_set)
{
	struct room_data *room = ses->map->room_list[ses->map->in_room];

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1 == 0)
	{
		tintin_printf2(ses, "  roomarea: %s", room->area);
		tintin_printf2(ses, " roomcolor: %s", room->color);
		tintin_printf2(ses, "  roomdesc: %s", room->desc);
		tintin_printf2(ses, " roomflags: %d", room->flags);
		tintin_printf2(ses, "  roomname: %s", room->name);
		tintin_printf2(ses, "  roomnote: %s", room->note);
		tintin_printf2(ses, "roomsymbol: %s", room->symbol);
	}
	else
	{
		if (is_abbrev(arg1, "roomarea"))
		{
			RESTRING(room->area, arg2);
			show_message(ses, -1, "#MAP SET: roomarea set to: %s", room->area);
		}
		else if (is_abbrev(arg1, "roomcolor"))
		{
			RESTRING(room->color, arg2);
			show_message(ses, -1, "#MAP SET: roomcolor set to: %s", arg2);
		}
		else if (is_abbrev(arg1, "roomdesc"))
		{
			RESTRING(room->desc, arg2);
			show_message(ses, -1, "#MAP SET: roomdesc set to: %s", arg2);
		}
		else if (is_abbrev(arg1, "roomflags"))
		{
			room->flags = (int) get_number(ses, arg2);

			show_message(ses, -1, "#MAP SET: roomflags set to: %d", room->flags);
		}
		else if (is_abbrev(arg1, "roomname"))
		{
			RESTRING(room->name, arg2);

			show_message(ses, -1, "#MAP SET: roomname set to: %s", room->name);
		}
		else if (is_abbrev(arg1, "roomnote"))
		{
			RESTRING(room->note, arg2);
			show_message(ses, -1, "#MAP SET: roomnote set to: %s", arg2);
		}
		else if (is_abbrev(arg1, "roomsymbol"))
		{
			RESTRING(room->symbol, arg2);

			show_message(ses, -1, "#MAP SET: roomsymbol set to: %s", room->symbol);
		}
		else
		{
			show_message(ses, -1, "#MAP SET: unknown option: %s", arg1);
		}
	}
}

DO_MAP(map_travel)
{
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	explore_path(ses, TRUE, arg1, arg2);
}

DO_MAP(map_undo)
{
	struct link_data *link;
	int undo_flag;
	struct exit_data *exit;

	link = ses->map->undo_tail;

	if (link == NULL)
	{
		tintin_printf2(ses, "#MAP UNDO: No known last move.");

		return;
	}

	if (ses->map->in_room != atoi(link->str1))
	{
		tintin_printf2(ses, "#MAP UNDO: Invalid last move.");

		return;
	}

	if (ses->map->room_list[atoi(link->str2)] == NULL)
	{
		tintin_printf2(ses, "#MAP UNDO: Invalid last move.");

		return;
	}

	undo_flag = atoi(link->str3);

	goto_room(ses, atoi(link->str2));

	if (HAS_BIT(undo_flag, MAP_UNDO_CREATE))
	{
		delete_room(ses, atoi(link->str1), TRUE);
	}
	else if (HAS_BIT(undo_flag, MAP_UNDO_LINK))
	{
		exit = find_exit(ses, atoi(link->str1), link->str2);

		if (exit)
		{
			delete_exit(ses, atoi(link->str1), exit);
		}

		exit = find_exit(ses, atoi(link->str2), link->str1);

		if (exit)
		{
			delete_exit(ses, atoi(link->str2), exit);
		}
	}
	del_undo(ses, link);
}

DO_MAP(map_unlink)
{
	struct exit_data *exit1;
	struct exit_data *exit2;
	struct listnode *node;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	node = search_node_list(ses->list[LIST_PATHDIR], arg1);

	exit1 = find_exit(ses, ses->map->in_room, arg1);

	if (exit1 == NULL)
	{
		tintin_printf2(ses, "#MAP UNLINK: No exit with that name found");

		return;
	}

	if (*arg2 == 'b')
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
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	shortest_path(ses, TRUE, arg2, arg1);
}

DO_MAP(map_write)
{
	FILE *file;
	struct exit_data *exit;
	int vnum;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1 == 0 || (file = fopen(arg1, "w")) == NULL)
	{
		tintin_printf(ses, "#ERROR: #WRITEMAP {%s} - COULDN'T OPEN FILE TO WRITE.", arg1);

		return;
	}

	fprintf(file, "C %d\n\n", ses->map->size);

	fprintf(file, "CE %s\nCR %s\n\n", ses->map->exit_color, ses->map->room_color);

	fprintf(file, "F %d\n\n", ses->map->flags);

	fprintf(file, "I %d\n\n", ses->map->in_room ? ses->map->in_room : ses->map->last_room);

	fprintf(file, "L %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n\n",
		ses->map->legend[0],  ses->map->legend[1],  ses->map->legend[2],
		ses->map->legend[3],  ses->map->legend[4],  ses->map->legend[5],
		ses->map->legend[6],  ses->map->legend[7],  ses->map->legend[8],
		ses->map->legend[9],  ses->map->legend[10], ses->map->legend[11],
		ses->map->legend[12], ses->map->legend[13], ses->map->legend[14],
		ses->map->legend[15], ses->map->legend[16]);

	for (vnum = 0 ; vnum < ses->map->size ; vnum++)
	{
		if (ses->map->room_list[vnum])
		{
			fprintf(file, "\nR {%5d} {%d} {%s} {%s} {%s} {%s} {%s} {%s}\n",
				ses->map->room_list[vnum]->vnum,
				ses->map->room_list[vnum]->flags,
				ses->map->room_list[vnum]->color,
				ses->map->room_list[vnum]->name,
				ses->map->room_list[vnum]->symbol,
				ses->map->room_list[vnum]->desc,
				ses->map->room_list[vnum]->area,
				ses->map->room_list[vnum]->note);
                        
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

	show_message(ses, -1, "#MAP: Map file written to {%s}.", arg1);
}

void create_map(struct session *ses, char *arg)
{
	int index;

	if (ses->map)
	{
		delete_map(ses);
	}

	ses->map = (struct map_data *) calloc(1, sizeof(struct map_data));
	ses->map->size = atoi(arg) > 0 ? atoi(arg) : 15000;

	ses->map->room_list = (struct room_data **) calloc(ses->map->size, sizeof(struct room_data *));

	ses->map->max_grid_x = 255;
	ses->map->max_grid_y = 101;

	ses->map->grid = (struct room_data **) calloc(ses->map->max_grid_x * ses->map->max_grid_y, sizeof(struct room_data *));

	ses->map->flags = MAP_FLAG_ASCIIGRAPHICS;

	ses->map->room_color = strdup("<178>");
	ses->map->exit_color = strdup("<078>");

	create_room(ses, "{1} {0} {} {1} { }");

	for (index = 0 ; index < 17 ; index++)
	{
		sprintf(ses->map->legend[index], "%c", map_palet0[index]);
	}
}

void delete_map(struct session *ses)
{
	int index;

	for (index = 1 ; index < ses->map->size ; index++)
	{
		if (ses->map->room_list[index])
		{
			delete_room(ses, index, FALSE);
		}
	}
	free(ses->map->room_list);

	while (ses->map->undo_head)
	{
		del_undo(ses, ses->map->undo_head);
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
	char vnum[BUFFER_SIZE], flags[BUFFER_SIZE], color[BUFFER_SIZE], name[BUFFER_SIZE], symbol[BUFFER_SIZE], desc[BUFFER_SIZE], area[BUFFER_SIZE], note[BUFFER_SIZE];
	struct room_data *newroom;

	arg = get_arg_in_braces(arg, vnum,   FALSE);
	arg = get_arg_in_braces(arg, flags,  FALSE);
	arg = get_arg_in_braces(arg, color,  FALSE);
	arg = get_arg_in_braces(arg, name,   FALSE);
	arg = get_arg_in_braces(arg, symbol, FALSE);
	arg = get_arg_in_braces(arg, desc,   FALSE);
	arg = get_arg_in_braces(arg, area,   FALSE);
	arg = get_arg_in_braces(arg, note,   FALSE);

	/* Backward compatbility 1.96.5 */

	if (*symbol == 0) { strcpy(symbol, " "); }

	newroom         = (struct room_data *) calloc(1, sizeof(struct room_data));

	newroom->color  = strdup(color);
	newroom->name   = strdup(name);
	newroom->symbol = strdup(symbol);
	newroom->desc   = strdup(desc);
	newroom->area   = strdup(area);
	newroom->note   = strdup(note);

	newroom->flags  = atoi(flags);
	newroom->vnum   = atoi(vnum);

	ses->map->room_list[newroom->vnum] = newroom;

	tintin_printf(ses, "#READMAP R %5s {%s}.", vnum, name);

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
	free(ses->map->room_list[room]->symbol);
	free(ses->map->room_list[room]->desc);
	free(ses->map->room_list[room]->area);
	free(ses->map->room_list[room]->note);

	free(ses->map->room_list[room]);

	ses->map->room_list[room] = NULL;

	if (exits)
	{
		for (cnt = 0 ; cnt < ses->map->size ; cnt++)
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

	newexit       = (struct exit_data *) calloc(1, sizeof(struct exit_data));
	newexit->vnum = atoi(vnum);
	newexit->name = strdup(name);
	newexit->cmd  = strdup(cmd);
	newexit->dir  = atoi(dir);

	if (newexit->dir == 0)
	{
		if ((node = search_node_list(ses->list[LIST_PATHDIR], name)) != NULL)
		{
			newexit->dir = atoi(node->pr);
		}
	}

	LINK(newexit, ses->map->room_list[room]->f_exit, ses->map->room_list[room]->l_exit);

	tintin_printf(ses, "#READMAP E %5s {%s} {%s}.", vnum, name, cmd);
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

	node = search_node_list(ses->list[LIST_PATHDIR], arg);

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

	push_call("follow_map(%p,%p)",ses,argument);

	exit = find_exit(ses, ses->map->in_room, argument);

	if (exit)
	{
		room = ses->map->in_room;

		add_undo(ses, "%d %d %d", exit->vnum, room, 0);

		goto_room(ses, exit->vnum);

		if (HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE))
		{
			show_message(ses, -1, "#MAP: LEAVE FLAG FOUND IN ROOM {%d}. LEAVING MAP.", ses->map->in_room);

			map_leave(ses, "", "", "");
		}
		else if (HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID))
		{
			exit = find_exit(ses, exit->vnum, argument);

			if (exit)
			{
				follow_map(ses, exit->name);

				pop_call();
				return;
			}

			for (exit = ses->map->room_list[ses->map->in_room]->f_exit ; exit ; exit = exit->next)
			{
				if (exit->vnum != room)
				{
					follow_map(ses, exit->name);
	
					pop_call();
					return;
				}
			}
		}

		show_vtmap(ses);

		pop_call();
		return;
	}

	if (!HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) && !HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_STATIC))
	{
		char temp[BUFFER_SIZE];
		struct listnode *node;

		if ((node = search_node_list(ses->list[LIST_PATHDIR], argument)) == NULL)
		{
			pop_call();
			return;
		}

		for (room = 1 ; room < ses->map->size ; room++)
		{
			if (ses->map->room_list[room] == NULL)
			{
				break;
			}
		}

		if (room == ses->map->size)
		{
			tintin_printf2(ses, "#MAP: Maximum amount of rooms of %d reached. Consider updating updating the C value in your map file.", ses->map->size);

			pop_call();
			return;
		}

		if (find_coord(ses, argument))
		{
			room = map_search_tar_room;

			show_message(ses, -1, "Found linkable room {%d}", room);

			add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_LINK);
		}
		else
		{
			sprintf(temp, "{%d} {0} {} {%d} { }", room, room);
			create_room(ses, temp);

			add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_CREATE|MAP_UNDO_LINK);

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

		pop_call();
		return;
	}
}

void add_undo(struct session *ses, char *format, ...)
{
	struct link_data *link;
	char buf[BUFFER_SIZE], *arg, dir[BUFFER_SIZE], rev[BUFFER_SIZE], flag[BUFFER_SIZE];
	va_list args;

	push_call("add_undo(%s,%s)",ses->name, format);

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	arg = get_arg_in_braces(buf, dir, FALSE);
	arg = get_arg_in_braces(arg, rev, FALSE);
	arg = get_arg_in_braces(arg, flag, FALSE);

	link = (struct link_data *) calloc(1, sizeof(struct link_data));

	link->str1 = strdup(dir);
	link->str2 = strdup(rev);
	link->str3 = strdup(flag);

	LINK(link, ses->map->undo_head, ses->map->undo_tail);

	ses->map->undo_size++;

	if (ses->map->undo_size > 100)
	{
		del_undo(ses, ses->map->undo_head);
	}
	pop_call();
	return;
}

void del_undo(struct session *ses, struct link_data *link)
{
	UNLINK(link, ses->map->undo_head, ses->map->undo_tail);

	free(link->str1);
	free(link->str2);
	free(link->str3);

	free(link);

	ses->map->undo_size--;
}

/*
	Draws a map on a grid, breadth first improvements by Bryan Turner
*/

#define MAP_BF_SIZE 800

struct grid_node
{
	int vnum;
	int x;
	int y;
	int z;
	int size;
};

void build_map_grid_bf(int vnum, int x, int y, int z)
{
	int head, tail;
	struct grid_node *node, *temp, list[MAP_BF_SIZE];
	struct exit_data *exit;
	struct room_data *room;
	int xx, yy, zz;

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

		if (node->x >= 0 && node->x < map_grid_x && node->y >= 0 && node->y < map_grid_y && node->z == 0)
		{
			if (map_search_ses->map->grid[node->x + map_grid_x * node->y] == NULL)
			{
				map_search_ses->map->grid[node->x + map_grid_x * node->y] = room;
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

			if (xx >= 0 && xx < map_grid_x && yy >= 0 && yy < map_grid_y && zz == 0)
			{
				if (map_search_ses->map->grid[xx + map_grid_x * yy] && map_search_ses->map->grid[xx + map_grid_x * yy]->size <= room->size + 1)
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



void create_map_grid(struct session *ses, int room, int x, int y)
{
	int vnum;

	for (vnum = 0 ; vnum < ses->map->size ; vnum++)
	{
		if (ses->map->room_list[vnum])
		{
			ses->map->room_list[vnum]->size = 500;
		}
	}

	for (vnum = 0 ; vnum < x * y ; vnum++)
	{
		ses->map->grid[vnum] = NULL;
	}

	map_search_ses = ses;

	map_grid_y = y;
	map_grid_x = x;

	if (ses->map->room_list[room])
	{
		build_map_grid_bf(room, x / 2, y / 2, 0);
	}
}

void show_map(struct session *ses, char *arg1, char *arg2)
{
	FILE *logfile = NULL;
	char buf[BUFFER_SIZE], out[BUFFER_SIZE];
	int x, y, line;

	push_call("show_map(%p,%p,%p)",ses,arg1,arg2);

	if (sscanf(arg1, "%dx%d", &map_grid_x, &map_grid_y) == 2)
	{
		logfile = fopen(arg2, "a");

		if (HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS))
		{
			map_grid_y = map_grid_y / 3;
			map_grid_x = map_grid_x / 6;
		}

		if (map_grid_x > ses->map->max_grid_x)
		{
			ses->map->max_grid_x = map_grid_x;

			ses->map->grid = (struct room_data **) realloc(ses->map->grid, ses->map->max_grid_x * ses->map->max_grid_y);
		}

		if (map_grid_y > ses->map->max_grid_y)
		{
			ses->map->max_grid_y = map_grid_y;

			ses->map->grid = (struct room_data **) realloc(ses->map->grid, ses->map->max_grid_x * ses->map->max_grid_y);
		}

	}
	else if (HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS))
	{
		map_grid_y = (get_scroll_size(ses) / 3);
		map_grid_x = (ses->cols / 6);

	}
	else
	{
		map_grid_y = get_scroll_size(ses);
		map_grid_x = ses->cols;
	}

	create_map_grid(ses, ses->map->in_room, map_grid_x, map_grid_y);

	if (HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS))
	{
		for (y = map_grid_y - 1 ; y >= 0 ; y--)
		{
			for (line = 1 ; line <= 3 ; line++)
			{
				strcpy(buf, "");

				for (x = 0 ; x < map_grid_x ; x++)
				{
					strcat(buf, draw_room(ses, ses->map->grid[x + map_grid_x * y], line));
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
				strcat(buf, draw_room(ses, ses->map->grid[x + map_grid_x * y], 0));
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
		fclose(logfile);
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

	if (ses->map->room_list[ses->map->in_room] == NULL)
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
		map_grid_y = (ses->top_row - 2) / 3;
		map_grid_x = (ses->cols    - 0) / 6;
	}
	else
	{
		map_grid_y = (ses->top_row - 2);
		map_grid_x = (ses->cols);
	}
	create_map_grid(ses, ses->map->in_room, map_grid_x, map_grid_y);

	save_pos(ses);
	goto_rowcol(ses, 1, 1);

	if (HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS))
	{
		for (y = map_grid_y - 1 ; y >= 0 ; y--)
		{
			for (line = 1 ; line <= 3 ; line++)
			{
				strcpy(buf, "");

				for (x = 0 ; x < map_grid_x ; x++)
				{
					strcat(buf, draw_room(ses, ses->map->grid[x + map_grid_x * y], line));
				}
				substitute(ses, buf, out, SUB_COL);

				printf("%s\033[0K\n", out);
			}
		}
	}
	else
	{
		for (y = map_grid_y - 1 ; y >= 0 ; y--)
		{
			strcpy(buf, "");

			for (x = 0 ; x < map_grid_x ; x++)
			{
				strcat(buf, draw_room(ses, ses->map->grid[x + map_grid_x * y], 0));
			}
			substitute(ses, buf, out, SUB_COL);

			printf("%s\n", out);
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

		sprintf(buf, "%s", ses->map->exit_color);

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
						cat_sprintf(buf, "%s[<118>#%s]%s", *room->color ? room->color : ses->map->room_color, *room->color ? room->color : ses->map->room_color, ses->map->exit_color);
					}
					else
					{
						cat_sprintf(buf, "<118>%05d%s", room->vnum, ses->map->exit_color);
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
							cat_sprintf(buf, "<128>%05d%s", room->vnum, ses->map->exit_color);
						}
						else if (strip_color_strlen(room->symbol) <= 1)
						{
							cat_sprintf(buf, "%s[%s%s]%s", *room->color ? room->color : ses->map->room_color, room->symbol, *room->color ? room->color : ses->map->room_color, ses->map->exit_color);
						}
						else
						{
							cat_sprintf(buf, "%s%-3s%s", *room->color ? room->color : ses->map->room_color, room->symbol, ses->map->exit_color);
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
		door = exit->dir;

		switch (exit->dir)
		{
			case MAP_EXIT_N|MAP_EXIT_E:
			case MAP_EXIT_N|MAP_EXIT_W:
			case MAP_EXIT_S|MAP_EXIT_E:
			case MAP_EXIT_S|MAP_EXIT_W:
				continue;
		}

		if (exit->dir >= MAP_EXIT_N && exit->dir <= MAP_EXIT_W)
		{
			SET_BIT(exits, exit->dir);
		}
	}

	if (HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS))
	{
		sprintf(buf, "%s\033[12m%s\033[10m", *room->color ? room->color : ses->map->room_color, ses->map->legend[exits]);
	}
	else
	{
		sprintf(buf, "%s%s", *room->color ? room->color : ses->map->room_color, ses->map->legend[exits]);
	}
	pop_call();
	return buf;
}

int match_room(struct session *ses, int room, char *arg1, char *arg2, char *arg3, char *arg4, char *arg5)
{
	if (ses->map->room_list[room] == NULL)
	{
		return 0;
	}

	if (*arg1)
	{
		if (!match(ses, ses->map->room_list[room]->name, arg1))
		{
			return 0;
		}
	}

	if (*arg2)
	{
		char exit[BUFFER_SIZE];
		int size = 0;
		char *arg = arg2;

		while (*arg)
		{
			arg = get_arg_in_braces(arg, exit, GET_ONE);

			size++;

			if (!find_exit(ses, room, exit))
			{
				size = -1;
				break;
			}
		}

		if (size != get_map_exits(ses, room))
		{
			return 0;
		}
	}

	if (*arg3)
	{
		if (!match(ses, ses->map->room_list[room]->desc, arg3))
		{
			return 0;
		}
	}

	if (*arg4)
	{
		if (!match(ses, ses->map->room_list[room]->area, arg4))
		{
			return 0;
		}
	}

	if (*arg5)
	{
		if (!match(ses, ses->map->room_list[room]->note, arg5))
		{
			return 0;
		}
	}

	return 1;
}

int find_room(struct session *ses, char *arg)
{
	char arg1[BUFFER_SIZE], arg2[BUFFER_SIZE], arg3[BUFFER_SIZE], arg4[BUFFER_SIZE], arg5[BUFFER_SIZE];
	int room, vnum, size;

	push_call("find_room(%p,%s)",ses,arg);

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg4, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg5, GET_ALL, SUB_VAR|SUB_FUN);

	room = atoi(arg1);

	if (room > 0 && room < ses->map->size)
	{
		if (ses->map->room_list[room])
		{
			pop_call();
			return room;
		}
		pop_call();
		return -1;
	}

	fill_map(ses);

	vnum = -1;
	size = MAP_SEARCH_SIZE + 1;

	for (room = 0 ; room < ses->map->size ; room++)
	{
		if (!match_room(ses, room, arg1, arg2, arg3, arg4, arg5))
		{
			continue;
		}

		if (ses->map->room_list[room]->size < size)
		{
			size = ses->map->room_list[room]->size;
			vnum = room;
		}
	}
	pop_call();
	return vnum;
}

void goto_room(struct session *ses, int room)
{
	push_call("goto_room(%p,%d)",ses,room);

	if (ses->map->in_room)
	{
		check_all_events(ses, 0, 1, "MAP EXIT ROOM", ntos(room));
	}

	ses->map->in_room = room;

	check_all_events(ses, 0, 1, "MAP ENTER ROOM", ntos(room));

	pop_call();
	return;
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

struct exit_data *find_exit_dir(struct session *ses, int room, int dir)
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
		show_message(map_search_ses, -1, "#ERROR: ROOM {%d} IS AN INVALID VOID ROOM.", room);

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

int follow_void(struct session *ses, int from, int room, int dir)
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

void search_path(int room, int size, int dir)
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

	push_call("fill_map(%p)",ses);

	map_search_ses       = ses;
	map_search_tar_room  = -1;
	map_search_max_size  = MAP_SEARCH_SIZE;

	for (vnum = 0 ; vnum < ses->map->size ; vnum++)
	{
		if (ses->map->room_list[vnum])
		{
			ses->map->room_list[vnum]->size = map_search_max_size;
		}
	}

	if (ses->map->in_room == 0)
	{
		pop_call();
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

	pop_call();
	return;
}

void shortest_path(struct session *ses, int run, char *delay, char *arg)
{
	struct exit_data *exit;
	int room, vnum;

	if (HAS_BIT(ses->flags, SES_FLAG_MAPPING))
	{
		tintin_printf2(ses, "#SHORTEST PATH: You have to stop #PATH TRACKING first.");

		return;
	}

	kill_list(ses->list[LIST_PATH]);

	room = find_room(ses, arg);

	if (room == -1)
	{
		tintin_printf2(ses, "#SHORTEST PATH: Couldn't find the room %s", arg);
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

	for (vnum = 0 ; vnum < ses->map->size ; vnum++)
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

		insert_node_list(ses->list[LIST_PATH], map_search_exit_best->cmd, "", "0");

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
		path_run(ses, delay);
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

	for (vnum = 0 ; vnum < ses->map->size ; vnum++)
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
			show_message(ses, -1, "#MAP: Linkable room is marked static. Creating overlapping room instead.");

			return 0;
		}
	}
	return map_search_tar_room;
}

void search_coord_bf(int vnum)
{
	int head, tail;
	struct grid_node *node, *temp, list[MAP_BF_SIZE];
	struct exit_data *exit;
	struct room_data *room;
	int xx, yy, zz;

	push_call("search_coord_bf(%d)",vnum);

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

			pop_call();
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
	pop_call();
	return;
}

void explore_path(struct session *ses, int run, char *arg1, char *arg2)
{
	struct exit_data *exit;
	int room, vnum;

	if (HAS_BIT(ses->flags, SES_FLAG_MAPPING))
	{
		tintin_printf2(ses, "#MAP EXPLORE: You have to stop #PATH TRACKING first.");

		return;
	}

	kill_list(ses->list[LIST_PATH]);

	room = ses->map->in_room;

	exit = find_exit(ses, room, arg1);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: There's no exit named '%s'.", arg1);
		return;
	}

	vnum = exit->vnum;

	insert_node_list(ses->list[LIST_PATH], exit->cmd, "", "0");

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

		insert_node_list(ses->list[LIST_PATH], exit->cmd, "", "0");

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
		path_run(ses, arg2);
	}
}
