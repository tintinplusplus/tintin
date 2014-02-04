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

	arg = get_arg_in_braces(ses, arg, arg1, FALSE);

	if (*arg1 == 0)
	{
		tintin_printf2(ses, "Available map options");
		tintin_printf2(ses, "");
		tintin_printf2(ses, "#map at       <location>  {command>    (execute command at given location)");
		tintin_printf2(ses, "#map color    <field>     <color>      (set the color for given field)");
		tintin_printf2(ses, "#map create   [size]                   (creates the initial map)");
		tintin_printf2(ses, "#map destroy                           (destroys the map)");
		tintin_printf2(ses, "#map delete   <direction>              (delete the room at given dir)");
		tintin_printf2(ses, "#map dig      <direction> [new] [vnum] (creates a new room)");
		tintin_printf2(ses, "#map exit     <direction>  <command>   (sets the exit command)");
		tintin_printf2(ses, "#map exitflag <direction> <exit flag>  (set the exit direction)");
		tintin_printf2(ses, "#map explore  <direction>              (saves path to #path)");
		tintin_printf2(ses, "#map info                              (info on map and current room)");
		tintin_printf2(ses, "#map insert   <direction>  [room flag] (insert a new room)");
		tintin_printf2(ses, "#map jump     <x> <y>                  (go to given coordinate)");
		tintin_printf2(ses, "#map find     <location> [exits]       (saves path to #path)");
		tintin_printf2(ses, "#map flag     <map flag>               (set map wide flags)");
		tintin_printf2(ses, "#map get      <option>     <variable>  (get various values)");
		tintin_printf2(ses, "#map goto     <location> [exits]       (moves you to given room)");
		tintin_printf2(ses, "#map leave                             (leave the map, return with goto)");
		tintin_printf2(ses, "#map legend   <symbols>                (sets the map legend)");
		tintin_printf2(ses, "#map link     <direction>  <room name> (links 2 rooms)");
		tintin_printf2(ses, "#map list     <location>               (shows list of matching rooms)");
		tintin_printf2(ses, "#map map      <radius>                 (shows an ascii map)");
		tintin_printf2(ses, "#map name     <room name>              (set the room's name)");
		tintin_printf2(ses, "#map read     <filename>               (load your map from a file)");
		tintin_printf2(ses, "#map resize   <size>                   (resize the maximum size)");
		tintin_printf2(ses, "#map roomflag <room flag>              (set room based flags)");
		tintin_printf2(ses, "#map set      <option>     <value>     (set various values)");
		tintin_printf2(ses, "#map return                            (return to last room.)");
		tintin_printf2(ses, "#map run      <location>  <delay>      (run to given room)");
		tintin_printf2(ses, "#map travel   <direction>  <delay>     (run in given direction)");
		tintin_printf2(ses, "#map undo                              (undo last move)");
		tintin_printf2(ses, "#map uninsert <direction>              (opposite of insert)");
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

DO_MAP(map_at)
{
	int new_room, old_room;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_NONE);

	new_room = find_room(ses, arg1);

	if (new_room == -1)
	{
		show_message(ses, -1, "#MAP AT: Couldn't find room {%s}.", arg1);

		return;
	}

	old_room = ses->map->in_room;
	ses->map->in_room = new_room;

	script_driver(ses, -1, arg2);

	if (ses->map)
	{
		ses->map->in_room = old_room;
	}
}

DO_MAP(map_color)
{
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1 && is_abbrev(arg1, "ROOMS"))
	{
		RESTRING(ses->map->room_color, arg2);
	}
	else if (*arg1 && is_abbrev(arg1, "EXITS"))
	{
		RESTRING(ses->map->exit_color, arg2);
	}
	else if (*arg1 && is_abbrev(arg1, "PATHS"))
	{
		RESTRING(ses->map->path_color, arg2);
	}
	else
	{
		tintin_printf2(ses, "#SYNTAX: #MAP COLOR {EXIT|PATH|ROOM} {COLOR CODE}");
		return;
	}

	show_message(ses, -1, "#MAP: %s color set to: %s", arg1, arg2);
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

	if (is_number(arg1))
	{
		room = find_room(ses, arg1);

		if (room == -1)
		{
			tintin_printf2(ses, "#MAP: No room with that vnum found");
			return;
		}
	}
	else
	{
		exit = find_exit(ses, ses->map->in_room, arg1);

		if (exit)
		{
			room = exit->vnum;
		}

		if (exit == NULL)
		{
			tintin_printf2(ses, "#MAP: No exit with that name found");
			return;
		}

		room = exit->vnum;
	}

	if (room == ses->map->in_room)
	{
		tintin_printf2(ses, "#MAP: You must first leave the room you're trying to delete");
		return;
	}

	delete_room(ses, room, TRUE);

	show_message(ses, -1, "#MAP: Room {%d} deleted", room);
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

	if (*arg1 == 0)
	{
		tintin_printf2(ses, "#SYNTAX: #MAP DIG {<DIRECTION>} {NEW|<VNUM>}");

		return;
	}

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
		show_message(ses, -1, "#MAP DIG: There is already a room in that direction.");
		return;
	}

	if (room == ses->map->size)
	{
		tintin_printf2(ses, "#MAP DIG: Maximum amount of rooms of %d reached.", ses->map->size);
		return;
	}

	if (is_number(arg2))
	{
		room = atoi(arg2);

		if (room <= 0 || room >= ses->map->size)
		{
			tintin_printf2(ses, "#MAP DIG: Invalid room vnum: %d.", room);
			return;
		}

		if (ses->map->room_list[room] == NULL)
		{
			sprintf(temp, "{%d} {0} {} {%d} { }", room, room);
			create_room(ses, temp);
		}
		sprintf(temp, "{%d} {%s} {%s}", room, arg1, arg1);
		create_exit(ses, ses->map->in_room, temp);

		return;
	}

	if (strcasecmp(arg2, "new") && find_coord(ses, arg1))
	{
		room = map_search_tar_room;
		show_message(ses, -1, "#MAP CREATE LINK %5d {%s}.", room, ses->map->room_list[room]->name);
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
}

DO_MAP(map_exit)
{
	char arg3[BUFFER_SIZE];
	struct exit_data *exit;
	struct listnode *node;
	int room;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ONE, SUB_VAR|SUB_FUN);

	exit = find_exit(ses, ses->map->in_room, arg1);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: Exit {%s} not found.", arg1);

		return;
	}
	if (*arg2 == 0)
	{
		tintin_printf2(ses, "  command: %s", exit->cmd);
		tintin_printf2(ses, "direction: %d", exit->dir);
		tintin_printf2(ses, "    flags: %d", exit->flags);
		tintin_printf2(ses, "     name: %s", exit->name);
		tintin_printf2(ses, "     vnum: %d", exit->vnum);
	}
	else if (is_abbrev(arg2, "COMMAND"))
	{
		exit->cmd = restring(exit->cmd, arg3);

		show_message(ses, -1, "#MAP EXIT {%s} : COMMAND SET TO {%s}.", arg1, exit->cmd);
	}
	else if (is_abbrev(arg2, "DIRECTION"))
	{
		if ((node = search_node_list(ses->list[LIST_PATHDIR], arg3)) == NULL)
		{
			show_message(ses, -1, "#MAP EXIT {%s} : DIRECTION {%s} NOT FOUND.", arg1, arg3);

			return;
		}

		exit->dir = atoi(node->pr);

		show_message(ses, -1, "#MAP EXIT {%s} : DIRECTION SET TO {%s}.", arg1, arg3);
	}
	else if (is_abbrev(arg2, "FLAGS"))
	{
		exit->flags = atoi(arg3);

		show_message(ses, -1, "#MAP EXIT {%s} : FLAGS SET TO {%d}.", arg1, exit->flags);
	}
	else if (is_abbrev(arg2, "GET"))
	{
		if (*arg3)
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg3, "%s", exit->data);
		}
		else
		{
			tintin_printf2(ses, "#MAP EXIT GET: No destination variable.");
		}
	}
	else if (is_abbrev(arg2, "NAME"))
	{
		exit->name = restring(exit->name, arg3);

		show_message(ses, -1, "#MAP EXIT {%s} : NAME SET TO {%s}.", arg1, exit->name);
	}
	else if (is_abbrev(arg2, "SET"))
	{
		exit->data = restring(exit->data, arg3);

		show_message(ses, -1, "#MAP EXIT {%s} : DATA SET TO {%s}.", arg1, exit->data);
	}
	else if (is_abbrev(arg2, "VNUM"))
	{
		room = atoi(arg3);

		if (room <= 0 || room >= ses->map->size)
		{
			tintin_printf2(ses, "#MAP EXIT VNUM: Invalid room vnum: %d.", room);
			return;
		}

		if (ses->map->room_list[room] == NULL)
		{
			tintin_printf2(ses, "#MAP EXIT VNUM: Non existant room vnum: %d.", room);
			return;
		}
		exit->vnum = room;

		show_message(ses, -1, "#MAP EXIT {%s} : VNUM SET TO {%s}.", arg1, arg3);
	}
	else
	{
		tintin_printf2(ses, "Syntax: #MAP EXIT {<NAME>} {COMMAND|DIRECTION|NAME|FLAGS|VNUM} {<argument>}");
	}
}

DO_MAP(map_exitflag)
{
	struct exit_data *exit;
	char arg3[BUFFER_SIZE];
	int flag;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ONE, SUB_VAR|SUB_FUN);

	exit = find_exit(ses, ses->map->in_room, arg1);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: Exit {%s} not found.", arg1);

		return;
	}

	if (*arg2)
	{
		if (is_abbrev(arg2, "hide"))
		{
			flag = EXIT_FLAG_HIDE;
		}
		else
		{
			tintin_printf2(ses, "#MAP: Invalid exit flag {%s}.", arg2);

			return;
		}
	}
	else
	{
		tintin_printf2(ses, "#MAP: Hide flag is set to %s.", HAS_BIT(exit->flags, EXIT_FLAG_HIDE) ? "on" : "off");

		return;
	}

	if (is_abbrev("on", arg3))
	{
		SET_BIT(exit->flags, flag);
	}
	else if (is_abbrev("off", arg3))
	{
		DEL_BIT(exit->flags, flag);
	}
	else
	{
		TOG_BIT(exit->flags, flag);
	}	


	if (is_abbrev(arg2, "hide"))
	{
		show_message(ses, -1, "#MAP: Hide flag set to %s.", HAS_BIT(exit->flags, EXIT_FLAG_HIDE) ? "on" : "off");
	}
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
	int flag = 0, unflag = 0;

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
			unflag = MAP_FLAG_MUDFONT|MAP_FLAG_ASCIIGRAPHICS;
		}
		else if (is_abbrev(arg1, "vtmap"))
		{
			flag = MAP_FLAG_VTMAP;
		}
		else if (is_abbrev(arg1, "asciigraphics"))
		{
			flag = MAP_FLAG_ASCIIGRAPHICS;
			unflag = MAP_FLAG_MUDFONT|MAP_FLAG_VTGRAPHICS;
		}
		else if (is_abbrev(arg1, "asciivnums"))
		{
			flag = MAP_FLAG_ASCIIVNUMS;
		}
		else if (is_abbrev(arg1, "mudfont"))
		{
			flag = MAP_FLAG_MUDFONT;
			unflag = MAP_FLAG_ASCIIGRAPHICS|MAP_FLAG_VTGRAPHICS;
		}
		else if (is_abbrev(arg1, "nofollow"))
		{
			flag = MAP_FLAG_NOFOLLOW;
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
		tintin_printf2(ses, "#MAP: NoFollow flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW) ? "on" : "off");

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

	if (unflag)
	{
		DEL_BIT(ses->map->flags, unflag);
	}

	if (is_abbrev(arg1, "static"))
	{
		show_message(ses, -1, "#MAP: Static flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "vtgraphics"))
	{
		show_message(ses, -1, "#MAP: VTgraphics flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "vtmap"))
	{
		show_message(ses, -1, "#MAP: VTmap flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP) ? "on" : "off");

		show_vtmap(ses);
	}
	else if (is_abbrev(arg1, "asciigraphics"))
	{
		show_message(ses, -1, "#MAP: AsciiGraphics flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "asciivnums"))
	{
		show_message(ses, -1, "#MAP: AsciiVnums flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "mudfont"))
	{
		show_message(ses, -1, "#MAP: MudFont flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_MUDFONT) ? "on" : "off");
	}
	else if (is_abbrev(arg, "nofollow"))
	{
		show_message(ses, -1, "#MAP: NoFollow flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW) ? "on" : "off");

		ses->map->nofollow = HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW) ? 1 : 0;
	}
}

DO_MAP(map_get)
{
	struct room_data *room = ses->map->room_list[ses->map->in_room];
	struct exit_data *exit;
	char buf[BUFFER_SIZE], arg3[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg3)
	{
		if (atoi(arg3) > 0 && atoi(arg3) < ses->map->size)
		{
			room = ses->map->room_list[atoi(arg3)];
		}
		else
		{
			room = NULL;
		}
	}

	if (room == NULL)
	{
		set_nest_node(ses->list[LIST_VARIABLE], arg2, "0");
	}
	else if (*arg1 == 0 || *arg2 == 0)
	{
		tintin_printf2(ses, "   roomvnum: %d", room->vnum);
		tintin_printf2(ses, "   roomarea: %s", room->area);
		tintin_printf2(ses, "  roomcolor: %s", room->color);
		tintin_printf2(ses, "   roomdata: %s", room->data);
		tintin_printf2(ses, "   roomdesc: %s", room->desc);
		tintin_printf2(ses, "  roomexits: %d", get_map_exits(ses, room->vnum));
		tintin_printf2(ses, "  roomflags: %d", room->flags);
		tintin_printf2(ses, "   roomname: %s", room->name);
		tintin_printf2(ses, "   roomnote: %s", room->note);
		tintin_printf2(ses, " roomsymbol: %s", room->symbol);
		tintin_printf2(ses, "roomterrain: %s", room->terrain);
		tintin_printf2(ses, " worldflags: %d", ses->map->flags);
		tintin_printf2(ses, "  worldsize: %d", ses->map->size);
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
		else if (is_abbrev(arg1, "roomdata"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%s", room->data);
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
		else if (is_abbrev(arg1, "roomterrain"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%s", room->terrain);
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
		else if (is_abbrev(arg1, "worldflags"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%d", ses->map->flags);
		}
		else if (is_abbrev(arg1, "worldsize"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%d", ses->map->size);
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
		show_message(ses, -1, "#MAP: Couldn't find room {%s}.", arg);
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

	tintin_printf2(ses, "%-20s%-20d%-20s%-20d", "Total rooms:", cnt, "Total exits:", exits);
	tintin_printf2(ses, "%-20s%-20d%-20s%-20d", "World size:", ses->map->size, "Current room:", ses->map->in_room);

	tintin_printf2(ses, "");

	tintin_printf2(ses, "%-14s%5s %-14s%5s %-14s%5s %-14s%5s", "Vtmap:", HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP) ? "on" : "off", "Static:", HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) ? "on" : "off", "Vtgraphics", HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS) ? "on" : "off", "Asciigraphics", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS) ? "on" : "off");
	tintin_printf2(ses, "%-14s%5s %-14s%5s", "Asciivnums:", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS) ? "on" : "off", "Nofollow:", HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW) ? "on" : "off");

	if (ses->map->in_room)
	{
		tintin_printf2(ses, "");

		tintin_printf2(ses, "%-20s%s", "Room area:",    ses->map->room_list[ses->map->in_room]->area);
		tintin_printf2(ses, "%-20s%s", "Room data:",    ses->map->room_list[ses->map->in_room]->data);
		tintin_printf2(ses, "%-20s%s", "Room desc:",    ses->map->room_list[ses->map->in_room]->desc);
		tintin_printf2(ses, "%-20s%s", "Room name:",    ses->map->room_list[ses->map->in_room]->name);
		tintin_printf2(ses, "%-20s%s", "Room note:",    ses->map->room_list[ses->map->in_room]->note);
		tintin_printf2(ses, "%-20s%s", "Room symbol:",  ses->map->room_list[ses->map->in_room]->symbol);
		tintin_printf2(ses, "%-20s%s", "Room terrain:", ses->map->room_list[ses->map->in_room]->terrain);
		tintin_printf2(ses, "%-20s%d", "Room vnum:",    ses->map->in_room);

		tintin_printf2(ses, "");

		tintin_printf2(ses, "%-14s%5s %-14s%5s %-14s%5s %-14s%5s", "Avoid:", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_AVOID) ? "on" : "off", "Hide:", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_HIDE) ? "on" : "off", "Leave:",  HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE) ? "on" : "off", "Static:", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_STATIC) ? "on" : "off");
		tintin_printf2(ses, "%-14s%5s",                             "Void:", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID) ? "on" : "off");

		tintin_printf2(ses, "");

		for (exit = ses->map->room_list[ses->map->in_room]->f_exit ; exit ; exit = exit->next)
		{
			tintin_printf2(ses, "%-14s%5s %-14s%5d %-14s%5d %-14s%5s", "Exit name:", exit->name, "Exit vnum:", exit->vnum, "Exit flags:", exit->flags, "Exit command:", exit->cmd);
			tintin_printf2(ses, "%-14s%s", "Exit data:", exit->data);
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
	show_message(ses, -1, "#MAP: Inserted room {%d}.", room);
}

DO_MAP(map_jump)
{
	char arg3[BUFFER_SIZE];
	int vnum;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ALL, SUB_VAR|SUB_FUN);

	map_grid_x = 0;
	map_grid_y = 0;
	map_grid_z = 0;

	map_search_ses       = ses;
	map_search_tar_room  = 0;

	map_grid_x += get_number(ses, arg1);
	map_grid_y += get_number(ses, arg2);
	map_grid_z += get_number(ses, arg3);

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
		show_message(ses, -1, "#MAP: Couldn't find room at {%d} {%d} {%d}.", map_grid_x, map_grid_y, map_grid_z);
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

		show_message(ses, -1, "#MAP: Leaving the map. Use goto or return to return.");

		check_all_events(ses, SUB_ARG|SUB_SEC, 0, 1, "MAP EXIT MAP", ntos(ses->map->in_room));
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
	show_message(ses, -1, "#MAP LINK: Connected room {%s} to {%s}.", ses->map->room_list[ses->map->in_room]->name, ses->map->room_list[room]->name);
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
	char arg3[BUFFER_SIZE], buf[BUFFER_SIZE], out[BUFFER_SIZE];
	FILE *logfile = NULL;
	int x, y, line;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ONE, SUB_VAR|SUB_FUN);

	push_call("map_map(%p,%p)",ses,arg);

	if (sscanf(arg1, "%dx%d", &map_grid_x, &map_grid_y) == 2)
	{
		if (*arg2)
		{
			if (tolower((int) *arg3) == 'a')
			{
				logfile = fopen(arg2, "a");
			}
			else
			{
				logfile = fopen(arg2, "w");
			}
		}

		if (HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS))
		{
			map_grid_y = map_grid_y / 3;
			map_grid_x = map_grid_x / 6;
		}
		else if (HAS_BIT(ses->map->flags, MAP_FLAG_MUDFONT))
		{
			map_grid_x = map_grid_x / 2;
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
		map_grid_y = get_scroll_size(ses) / 3;
		map_grid_x = ses->cols / 6;

	}
	else if (HAS_BIT(ses->map->flags, MAP_FLAG_MUDFONT))
	{
		map_grid_y = get_scroll_size(ses);
		map_grid_x = ses->cols / 2;
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

DO_MAP(map_move)
{
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	ses->map->nofollow++;

	follow_map(ses, arg1);

	ses->map->nofollow--;
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
	int line = 0, room = 0;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	if ((myfile = fopen(arg1, "r")) == NULL)
	{
		tintin_printf2(ses, "#MAP: Map file {%s} not found.", arg1);

		return;
	}

	gtd->quiet++;

	if (ses->map == NULL)
	{
		ses->map = (struct map_data *) calloc(1, sizeof(struct map_data));
	}

	while (fgets(buffer, BUFFER_SIZE - 1, myfile))
	{
		line++;

		cptr = strchr(buffer, '\r'); /* For map files editor on Windows systems. */

		if (cptr)
		{
			*cptr = 0;
		}

		cptr = strchr(buffer, '\n');

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

					case 'P':
						ses->map->path_color = strdup(buffer + 3);
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

				if (HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW))
				{
					ses->map->nofollow = 1;
				}
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

				tintin_printf2(ses, "#MAP: INVALID COMMAND {%d} {%s} ON LINE %d. ABORTING READ..", buffer[0], buffer, line);

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

DO_MAP(map_resize)
{
	int size;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	size = atoi(arg1);

	if (size <= ses->map->size)
	{
		tintin_printf2(ses, "#MAP RESIZE: NEW SIZE (%d) MUST BE GREATER THAN CURRENT SIZE (%d).", size, ses->map->size);
		return;
	}

	ses->map->size = size;

	ses->map->room_list = (struct room_data **) realloc(ses->map->room_list, ses->map->size * sizeof(struct room_data *));

	show_message(ses, -1, "#MAP RESIZE: MAP RESIZED TO %d ROOMS.", size);
}

DO_MAP(map_return)
{
	if (ses->map == NULL || ses->map->room_list[ses->map->last_room] == NULL)
	{
		tintin_printf2(ses, "#MAP RETURN: No known last room.");

		return;
	}

	if (ses->map->in_room)
	{
		tintin_printf2(ses, "#MAP RETURN: Already in the map.");
	}
	else
	{
		goto_room(ses, ses->map->last_room);
	}
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
		else if (is_abbrev(arg1, "river"))
		{
			flag = ROOM_FLAG_RIVER;
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
		show_message(ses, -1, "#MAP: Avoid flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_AVOID) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "hide"))
	{
		show_message(ses, -1, "#MAP: Hide flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_HIDE) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "leave"))
	{
		show_message(ses, -1, "#MAP: Leave flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "void"))
	{
		show_message(ses, -1, "#MAP: Void flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "static"))
	{
		show_message(ses, -1, "#MAP: Static flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_STATIC) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "river"))
	{
		show_message(ses, -1, "#MAP: River flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_RIVER) ? "on" : "off");
	}
}

DO_MAP(map_set)
{
	struct room_data *room = ses->map->room_list[ses->map->in_room];
	char arg3[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg3)
	{
		if (atoi(arg3) > 0 && atoi(arg3) < ses->map->size)
		{
			room = ses->map->room_list[atoi(arg3)];
		}
		else
		{
			room = NULL;
		}
	}

	if (room == NULL)
	{
		show_message(ses, -1, "#MAP SET: invalid room vnum: %s", arg3);
	}
	else if (*arg1 == 0)
	{
		tintin_printf2(ses, "  roomarea: %s", room->area);
		tintin_printf2(ses, " roomcolor: %s", room->color);
		tintin_printf2(ses, "  roomdata: %s", room->data);
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
		else if (is_abbrev(arg1, "roomdata"))
		{
			RESTRING(room->data, arg2);
			show_message(ses, -1, "#MAP SET: roomdata set to: %s", arg2);
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
		else if (is_abbrev(arg1, "roomterrain"))
		{
			RESTRING(room->terrain, arg2);
			show_message(ses, -1, "#MAP SET: roomterrain set to: %s", arg2);
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


DO_MAP(map_uninsert)
{
	int room1, room2, room3;
	struct exit_data *exit1, *exit2, *exit3;
	struct listnode *node;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);

	room1 = ses->map->in_room;
	exit1 = find_exit(ses, room1, arg1);

	node = search_node_list(ses->list[LIST_PATHDIR], arg1);

	if (exit1 == NULL)
	{
		tintin_printf2(ses, "#MAP UNINSERT: There is no room in that direction.");

		return;
	}

	if (node == NULL)
	{
		tintin_printf2(ses, "#MAP UNINSERT: Given direction must be a pathdir.");
		return;
	}

	room2 = exit1->vnum;
	exit2 = find_exit(ses, room2, node->left);

	if (exit2 == NULL)
	{
		tintin_printf2(ses, "#MAP UNINSERT: Unable to find backlink room.");
		return;
	}

	room3 = exit2->vnum;
	exit3 = find_exit(ses, room3, node->right);

	if (exit3 == NULL)
	{
		tintin_printf2(ses, "#MAP UNINSERT: Unable to find backlink exit.");

		return;
	}

	exit1->vnum = room3;
	exit3->vnum = room1;

	delete_room(ses, room2, TRUE);

	show_message(ses, -1, "#MAP UNINSERT: Uninserted room {%d}.", room2);
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

	show_message(ses, -1, "#MAP UNLINK: Exit deleted.");
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
		tintin_printf2(ses, "#ERROR: #MAP WRITE {%s} - COULDN'T OPEN FILE TO WRITE.", arg1);

		return;
	}

	fprintf(file, "C %d\n\n", ses->map->size);

	fprintf(file, "CE %s\nCR %s\nCP %s\n\n", ses->map->exit_color, ses->map->room_color, ses->map->path_color);

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
			fprintf(file, "\nR {%5d} {%d} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s}\n",
				ses->map->room_list[vnum]->vnum,
				ses->map->room_list[vnum]->flags,
				ses->map->room_list[vnum]->color,
				ses->map->room_list[vnum]->name,
				ses->map->room_list[vnum]->symbol,
				ses->map->room_list[vnum]->desc,
				ses->map->room_list[vnum]->area,
				ses->map->room_list[vnum]->note,
				ses->map->room_list[vnum]->terrain,
				ses->map->room_list[vnum]->data);

			for (exit = ses->map->room_list[vnum]->f_exit ; exit ; exit = exit->next)
			{
				fprintf(file, "E {%5d} {%s} {%s} {%d} {%d} {%s}\n",
					exit->vnum,
					exit->name,
					exit->cmd,
					exit->dir,
					exit->flags,
					exit->data);
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
	ses->map->size = atoi(arg) > 0 ? atoi(arg) : 50000;

	ses->map->room_list = (struct room_data **) calloc(ses->map->size, sizeof(struct room_data *));

	ses->map->max_grid_x = 255;
	ses->map->max_grid_y = 101;

	ses->map->grid = (struct room_data **) calloc(ses->map->max_grid_x * ses->map->max_grid_y, sizeof(struct room_data *));

	ses->map->flags = MAP_FLAG_ASCIIGRAPHICS;

	ses->map->room_color = strdup("<178>");
	ses->map->exit_color = strdup("<078>");
	ses->map->path_color = strdup("<138>");

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
		arg = get_arg_in_braces(ses, arg, buf, FALSE);

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
	char buf[BUFFER_SIZE];
	struct room_data *newroom;

	newroom          = (struct room_data *) calloc(1, sizeof(struct room_data));

	arg = get_arg_in_braces(ses, arg, buf, FALSE); newroom->vnum    = atoi(buf);
	arg = get_arg_in_braces(ses, arg, buf, FALSE); newroom->flags   = atoi(buf);
	arg = get_arg_in_braces(ses, arg, buf, FALSE); newroom->color   = strdup(buf);
	arg = get_arg_in_braces(ses, arg, buf, FALSE); newroom->name    = strdup(buf);
	arg = get_arg_in_braces(ses, arg, buf, FALSE); newroom->symbol  = strdup(buf);
	arg = get_arg_in_braces(ses, arg, buf, FALSE); newroom->desc    = strdup(buf);
	arg = get_arg_in_braces(ses, arg, buf, FALSE); newroom->area    = strdup(buf);
	arg = get_arg_in_braces(ses, arg, buf, FALSE); newroom->note    = strdup(buf);
	arg = get_arg_in_braces(ses, arg, buf, FALSE); newroom->terrain = strdup(buf);
	arg = get_arg_in_braces(ses, arg, buf, FALSE); newroom->data    = strdup(buf);

	ses->map->room_list[newroom->vnum] = newroom;

	show_message(ses, -1, "#MAP CREATE ROOM %5d {%s}.", newroom->vnum, newroom->name);

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

	char vnum[BUFFER_SIZE], name[BUFFER_SIZE], cmd[BUFFER_SIZE], dir[BUFFER_SIZE], flags[BUFFER_SIZE], data[BUFFER_SIZE];

	arg = get_arg_in_braces(ses, arg, vnum,  FALSE);
	arg = get_arg_in_braces(ses, arg, name,  FALSE);
	arg = get_arg_in_braces(ses, arg, cmd,   TRUE);
	arg = get_arg_in_braces(ses, arg, dir,   FALSE);
	arg = get_arg_in_braces(ses, arg, flags, FALSE);
	arg = get_arg_in_braces(ses, arg, data,  TRUE);

	newexit        = (struct exit_data *) calloc(1, sizeof(struct exit_data));

	newexit->vnum  = atoi(vnum);
	newexit->name  = strdup(name);
	newexit->cmd   = strdup(cmd);
	newexit->dir   = atoi(dir);
	newexit->flags = atoi(flags);
	newexit->data  = strdup(data);

	if (newexit->dir == 0)
	{
		if ((node = search_node_list(ses->list[LIST_PATHDIR], name)) != NULL)
		{
			newexit->dir = atoi(node->pr);
		}
	}

	LINK(newexit, ses->map->room_list[room]->f_exit, ses->map->room_list[room]->l_exit);

	show_message(ses, -1, "#MAP CREATE EXIT %5s {%s} {%s}.", vnum, name, cmd);
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

	int count = 0;

	for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
	{
		count++;
	}

	return count;
}

int follow_map(struct session *ses, char *argument)
{
	struct exit_data *exit;
	int room;

	push_call("follow_map(%p,%p)",ses,argument);

	exit = find_exit(ses, ses->map->in_room, argument);

	if (exit)
	{
		room = ses->map->in_room;

		add_undo(ses, "%d %d %d", exit->vnum, room, 0);

		if (ses->map->nofollow == 0)
		{
			ses->map->nofollow++;

			script_driver(ses, -1, exit->cmd);

			ses->map->nofollow--;
		}
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
				ses->map->nofollow++;

				follow_map(ses, exit->name);

				ses->map->nofollow--;

				pop_call();
				return 1;
			}

			for (exit = ses->map->room_list[ses->map->in_room]->f_exit ; exit ; exit = exit->next)
			{
				if (exit->vnum != room)
				{
					ses->map->nofollow++;

					follow_map(ses, exit->name);

					ses->map->nofollow--;

					pop_call();
					return 1;
				}
			}
		}

		show_vtmap(ses);

		pop_call();
		return 1;
	}

	if (!HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) && !HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_STATIC))
	{
		char temp[BUFFER_SIZE];
		struct listnode *node;

		if ((node = search_node_list(ses->list[LIST_PATHDIR], argument)) == NULL)
		{
			pop_call();
			return 0;
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
			tintin_printf2(ses, "#MAP: Maximum amount of rooms of %d reached. Consider updating the C value in your map file.", ses->map->size);

			pop_call();
			return 1;
		}

		if (find_coord(ses, argument))
		{
			room = map_search_tar_room;

			show_message(ses, -1, "#MAP CREATE LINK %5d {%s}.", room, ses->map->room_list[room]->name);

			add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_LINK);
		}
		else
		{
			sprintf(temp, "{%d} {0} {} {%d} { }", room, room);
			create_room(ses, temp);

			add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_CREATE|MAP_UNDO_LINK);
		}
		sprintf(temp, "{%d} {%s} {%s}", room, node->left, node->left);
		create_exit(ses, ses->map->in_room, temp);

		if (find_exit(ses, room, node->right) == NULL)
		{
			sprintf(temp, "{%d} {%s} {%s}", ses->map->in_room, node->right, node->right);
			create_exit(ses, room, temp);
		}

		if (ses->map->nofollow == 0)
		{
			ses->map->nofollow++;

			script_driver(ses, -1, argument);

			ses->map->nofollow--;
		}
		goto_room(ses, room);

		show_vtmap(ses);

		pop_call();
		return 1;
	}
	pop_call();
	return 0;
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

	arg = get_arg_in_braces(ses, buf, dir, FALSE);
	arg = get_arg_in_braces(ses, arg, rev, FALSE);
	arg = get_arg_in_braces(ses, arg, flag, FALSE);

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

			if (exit->dir == 0)
			{
				continue;
			}

			if (HAS_BIT(exit->flags, EXIT_FLAG_HIDE))
			{
				continue;
			}

			if (HAS_BIT(map_search_ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_HIDE))
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
	else if (HAS_BIT(ses->map->flags, MAP_FLAG_MUDFONT))
	{
		map_grid_y = (ses->top_row - 2);
		map_grid_x = (ses->cols / 2);
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
	int door, exits = 0, exit1 = 0, exit2 = 0;

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
							cat_sprintf(buf, "%s[%-1s%s]%s", HAS_BIT(room->flags, ROOM_FLAG_PATH) ? ses->map->path_color : *room->color ? room->color : ses->map->room_color, room->symbol, HAS_BIT(room->flags, ROOM_FLAG_PATH) ? ses->map->path_color : *room->color ? room->color : ses->map->room_color, ses->map->exit_color);
						}
						else
						{
							cat_sprintf(buf, "%s%-3s%s", HAS_BIT(room->flags, ROOM_FLAG_PATH) ? ses->map->path_color : *room->color ? room->color : ses->map->room_color, room->symbol, ses->map->exit_color);
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
		if (HAS_BIT(ses->map->flags, MAP_FLAG_MUDFONT))
		{
			pop_call();
			return "  ";
		}
		else
		{
			pop_call();
			return " ";
		}
	}

	if (room->vnum == ses->map->in_room)
	{
		if (HAS_BIT(ses->map->flags, MAP_FLAG_MUDFONT))
		{
			sprintf(buf, "<118>%c%c", 199, 207);
		}
		else if (HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS))
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
		if (HAS_BIT(ses->map->flags, MAP_FLAG_MUDFONT))
		{
			switch (exit->dir)
			{
				case MAP_EXIT_N:
					SET_BIT(exit1, 1 << 0);
					SET_BIT(exit2, 1 << 0);
					SET_BIT(exits, MAP_EXIT_N);
					break;
				case MAP_EXIT_N|MAP_EXIT_W:
					SET_BIT(exit1, 1 << 1);
					SET_BIT(exits, MAP_EXIT_NW);
					break;
				case MAP_EXIT_N|MAP_EXIT_E:
					SET_BIT(exit2, 1 << 1);
					SET_BIT(exits, MAP_EXIT_NE);
					break;
				case MAP_EXIT_W:
					SET_BIT(exit1, 1 << 2);
					SET_BIT(exits, MAP_EXIT_W);
					break;
				case MAP_EXIT_E:
					SET_BIT(exit2, 1 << 2);
					SET_BIT(exits, MAP_EXIT_E);
					break;
				case MAP_EXIT_W|MAP_EXIT_S:
					SET_BIT(exit1, 1 << 3);
					SET_BIT(exits, MAP_EXIT_SW);
					break;
				case MAP_EXIT_E|MAP_EXIT_S:
					SET_BIT(exit2, 1 << 3);
					SET_BIT(exits, MAP_EXIT_SE);
					break;
				case MAP_EXIT_S:
					SET_BIT(exit1, 1 << 4);
					SET_BIT(exit2, 1 << 4);
					SET_BIT(exits, MAP_EXIT_S);
					break;
			}
			continue;
		}

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

	if (HAS_BIT(ses->map->flags, MAP_FLAG_MUDFONT))
	{
		int room1 = exit1 + 128;
		int room2 = exit2 + 160;

		if (HAS_BIT(room->flags, ROOM_FLAG_RIVER))
		{
			switch (exits)
			{
				case 0:
					room1 = 192;
					room2 = 192 + 8;
					break;
				case MAP_EXIT_N:
					room1 = 192 + 1;
					room2 = 192 + 1 + 8;
					break;
				case MAP_EXIT_N|MAP_EXIT_W:
					room1 = 192 + 2;
					room2 = 192 + 1 + 8;
					break;
				case MAP_EXIT_S:
					room1 = 192 + 3;
					room2 = 192 + 3 + 8;
					break;
				case MAP_EXIT_W|MAP_EXIT_S:
					room1 = 192 + 5;
					room2 = 192 + 3 + 8;
					break;
				case MAP_EXIT_N|MAP_EXIT_W|MAP_EXIT_S:
					room1 = 192 + 6;
					room2 = 192 + 4 + 8;
					break;
				case MAP_EXIT_N|MAP_EXIT_E:
					room1 = 192 + 1;
					room2 = 192 + 2 + 8;
					break;
				case MAP_EXIT_N|MAP_EXIT_E|MAP_EXIT_W:
					room1 = 192 + 2;
					room2 = 192 + 2 + 8;
					break;
				case MAP_EXIT_E|MAP_EXIT_S:
					room1 = 192 + 3;
					room2 = 192 + 5 + 8;
					break;
				case MAP_EXIT_N|MAP_EXIT_E|MAP_EXIT_S:
					room1 = 192 + 4;
					room2 = 192 + 6 + 8;
					break;
				case MAP_EXIT_E|MAP_EXIT_S|MAP_EXIT_W:
					room1 = 192 + 5;
					room2 = 192 + 5 + 8;
					break;
			}
		}
		sprintf(buf, "%s%c%c", *room->color ? room->color : ses->map->room_color, room1, room2);
	}
	else if (HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS))
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
		if (!match(ses, ses->map->room_list[room]->name, arg1, SUB_VAR|SUB_FUN))
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
			arg = get_arg_in_braces(ses, arg, exit, GET_ONE);

			size++;

			if (!find_exit(ses, room, exit))
			{
				size = -1;
				break;
			}

			if (*arg == COMMAND_SEPARATOR)
			{
				arg++;
			}
		}

		if (size != get_map_exits(ses, room))
		{
			return 0;
		}
	}

	if (*arg3)
	{
		if (!match(ses, ses->map->room_list[room]->desc, arg3, SUB_VAR|SUB_FUN))
		{
			return 0;
		}
	}

	if (*arg4)
	{
		if (!match(ses, ses->map->room_list[room]->area, arg4, SUB_VAR|SUB_FUN))
		{
			return 0;
		}
	}

	if (*arg5)
	{
		if (!match(ses, ses->map->room_list[room]->note, arg5, SUB_VAR|SUB_FUN))
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

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN); // name
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN); // exits
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ALL, SUB_VAR|SUB_FUN); // desc
	arg = sub_arg_in_braces(ses, arg, arg4, GET_ALL, SUB_VAR|SUB_FUN); // area
	arg = sub_arg_in_braces(ses, arg, arg5, GET_ALL, SUB_VAR|SUB_FUN); // note

	room = atoi(arg1);

	if (room > 0 && room < ses->map->size)
	{
		if (ses->map->room_list[room])
		{
			pop_call();
			return room;
		}

		if (!strcmp(arg2, "dig"))
		{
			sprintf(arg3, "{%d} {0} {} {%d} { }", room, room);
			create_room(ses, arg3);

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
	int last_room = ses->map->in_room;

	push_call("goto_room(%p,%d)",ses,room);

	if (ses->map->in_room)
	{
		check_all_events(ses, SUB_ARG|SUB_SEC, 0, 2, "MAP EXIT ROOM", ntos(last_room), ntos(room));
		check_all_events(ses, SUB_ARG|SUB_SEC, 1, 2, "MAP EXIT ROOM %d", last_room, ntos(last_room), ntos(room));
	}

	ses->map->in_room = room;

	DEL_BIT(ses->map->room_list[room]->flags, ROOM_FLAG_PATH);

	if (last_room == 0)
	{
		check_all_events(ses, SUB_ARG|SUB_SEC, 0, 1, "MAP ENTER MAP", ntos(room));
	}

	check_all_events(ses, SUB_ARG|SUB_SEC, 0, 2, "MAP ENTER ROOM", ntos(room), ntos(last_room));
	check_all_events(ses, SUB_ARG|SUB_SEC, 1, 2, "MAP ENTER ROOM %d", room, ntos(room), ntos(last_room));

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

			DEL_BIT(ses->map->room_list[vnum]->flags, ROOM_FLAG_PATH);
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

		if (HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW))
		{
		
			insert_node_list(ses->list[LIST_PATH], map_search_exit_best->cmd, "", "0");
		}
		else
		{
			insert_node_list(ses->list[LIST_PATH], map_search_exit_best->name, "", "0");
		}

		if (HAS_BIT(ses->map->room_list[map_search_exit_best->vnum]->flags, ROOM_FLAG_VOID))
		{
			room = follow_void(ses, room, map_search_exit_best->vnum, map_search_exit_best->dir);
		}
		else
		{
			room = map_search_exit_best->vnum;
		}

		SET_BIT(ses->map->room_list[room]->flags, ROOM_FLAG_PATH);

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

			if (exit->dir == 0)
			{
				continue;
			}

			if (HAS_BIT(exit->flags, EXIT_FLAG_HIDE))
			{
				continue;
			}

			if (HAS_BIT(map_search_ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_HIDE))
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

	if (HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW))
	{
	
		insert_node_list(ses->list[LIST_PATH], exit->cmd, "", "0");
	}
	else
	{
		insert_node_list(ses->list[LIST_PATH], exit->name, "", "0");
	}

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

		if (HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW))
		{
			insert_node_list(ses->list[LIST_PATH], exit->cmd, "", "0");
		}
		else
		{
			insert_node_list(ses->list[LIST_PATH], exit->name, "", "0");
		}

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
