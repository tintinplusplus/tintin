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

#define             MAP_SEARCH_DIST 1000
#define             MAP_BF_SIZE 4000

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
		tintin_printf2(ses, "#map at       <location>  <command>    (execute command at given location)");
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
		tintin_printf2(ses, "#map map      <radius> <filename>      (shows an ascii map)");
		tintin_printf2(ses, "#map move     <direction>              (move to given direction)");
		tintin_printf2(ses, "#map read     <filename>               (load a map from file)");
		tintin_printf2(ses, "#map resize   <size>                   (resize the maximum size)");
		tintin_printf2(ses, "#map roomflag <room flag>              (set room based flags)");
		tintin_printf2(ses, "#map set      <option>     <value>     (set various values)");
		tintin_printf2(ses, "#map return                            (return to last room.)");
		tintin_printf2(ses, "#map run      <location>   [delay]     (run to given room)");
		tintin_printf2(ses, "#map travel   <direction>  [delay]     (run in given direction)");
		tintin_printf2(ses, "#map undo                              (undo last move)");
		tintin_printf2(ses, "#map uninsert <direction>              (opposite of insert)");
		tintin_printf2(ses, "#map unlink   <direction> [both]       (deletes an exit)");
		tintin_printf2(ses, "#map vnum     <low vnum> [high vnum]   (change room vnum)");
		tintin_printf2(ses, "#map write    <filename>               (save the map)");

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
					return show_error(ses, LIST_COMMAND, "#MAP: This session has no map data. Use #map create or #map read to create one.");
				}
				if (map_table[cnt].check > 1 && ses->map->room_list[ses->map->in_room] == NULL)
				{
					return show_error(ses, LIST_COMMAND, "#MAP: You are not inside the map. Use #map goto to enter it.");
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
	int new_room;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_NONE);

	new_room = find_room(ses, arg1);

	if (new_room == 0)
	{
		show_message(ses, LIST_COMMAND, "#MAP AT: Couldn't find room {%s}.", arg1);

		return;
	}

	ses->map->at_room = ses->map->in_room;
	ses->map->in_room = new_room;

	script_driver(ses, LIST_COMMAND, arg2);

	if (ses->map)
	{
		ses->map->in_room = ses->map->at_room;
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
	else if (*arg1 && is_abbrev(arg1, "HERE"))
	{
		RESTRING(ses->map->here_color, arg2);
	}
	else if (*arg1 && is_abbrev(arg1, "PATHS"))
	{
		RESTRING(ses->map->path_color, arg2);
	}
	else if (*arg1 && is_abbrev(arg1, "BACKGROUND"))
	{
		RESTRING(ses->map->back_color, arg2);
	}
	else
	{
		show_error(ses, LIST_COMMAND, "#SYNTAX: #MAP COLOR {BACKGROUND|EXIT|HERE|PATH|ROOM} {COLOR CODE}");

		return;
	}

	show_message(ses, LIST_COMMAND, "#MAP: %s color set to: %s", arg1, arg2);
}

DO_MAP(map_create)
{
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	create_map(ses, arg1);

	tintin_printf2(ses, "#MAP: %d room map created, use #map goto 1, to proceed", ses->map->size);
}

DO_MAP(map_debug)
{
	tintin_printf2(ses, "max spatial grid x: %d", ses->map->max_grid_x);
	tintin_printf2(ses, "max spatial grid y: %d", ses->map->max_grid_y);
	tintin_printf2(ses, "     max undo size: %d", ses->map->undo_size);
	tintin_printf2(ses, "           in room: %d", ses->map->in_room);
	tintin_printf2(ses, "           at room: %d", ses->map->at_room);
	tintin_printf2(ses, "         last room: %d", ses->map->last_room);
	tintin_printf2(ses, "             stamp: %d", ses->map->search->stamp);
	tintin_printf2(ses, "            length: %f", ses->map->room_list[ses->map->in_room]->length);
	tintin_printf2(ses, "          nofollow: %d", ses->map->nofollow);

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1)
	{
		if (is_abbrev(arg1, "undo"))
		{
			struct link_data *link;

			for (link = ses->map->undo_head ; link ; link = link->next)
			{
				tintin_printf2(ses, "%05s %05s %s", link->str1, link->str2, link->str3);
			}
		}
	}
}

DO_MAP(map_delete)
{
	int room;
	struct exit_data *exit;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	if (is_number(arg1))
	{
		room = find_room(ses, arg1);

		if (room == 0)
		{
			show_error(ses, LIST_COMMAND, "#MAP DELETE {%d} - No room with that vnum found", arg1);

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
			show_error(ses, LIST_COMMAND, "#MAP: No exit with that name found");
			
			return;
		}

		room = exit->vnum;
	}

	if (room == ses->map->in_room)
	{
		show_error(ses, LIST_COMMAND, "#MAP: You must first leave the room you're trying to delete");
		
		return;
	}

	delete_room(ses, room, TRUE);

	show_message(ses, LIST_COMMAND, "#MAP: Room {%d} deleted", room);
}

DO_MAP(map_destroy)
{
	delete_map(ses);

	tintin_printf2(ses, "#MAP: Map destroyed.");
}

DO_MAP(map_dig)
{
	int room;
	struct exit_data *exit;
	struct listnode *node;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1 == 0)
	{
		show_error(ses, LIST_COMMAND, "#SYNTAX: #MAP DIG {<DIRECTION>|<VNUM>} {NEW|<VNUM>}");
		
		return;
	}

	room = atoi(arg1);

	if (room > 0 && room < ses->map->size)
	{
		if (ses->map->room_list[room] == NULL)
		{
			add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_CREATE);

			create_room(ses, "{%d} {0} {} {} { } {} {} {} {} {} {1.0}", room);
		}
		return;
	}

	exit = find_exit(ses, ses->map->in_room, arg1);

	if (exit)
	{
		show_message(ses, LIST_COMMAND, "#MAP DIG: There is already a room in that direction.");
		return;
	}

	if (is_number(arg2))
	{
		room = atoi(arg2);

		if (room <= 0 || room >= ses->map->size)
		{
			show_error(ses, LIST_COMMAND, "#MAP DIG: Invalid room vnum: %d.", room);

			return;
		}

		if (ses->map->room_list[room] == NULL)
		{
			add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_CREATE|MAP_UNDO_LINK);

			create_room(ses, "{%d} {0} {} {} { } {} {} {} {} {} {1.0}", room);
			create_exit(ses, ses->map->in_room, "{%d} {%s} {%s}", room, arg1, arg1);
		}
		else
		{
			add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_LINK);

			create_exit(ses, ses->map->in_room, "{%d} {%s} {%s}", room, arg1, arg1);
		}
		return;
	}

	room = find_coord(ses, arg1);

	if (room && strcasecmp(arg2, "new"))
	{
		show_message(ses, LIST_COMMAND, "#MAP CREATE LINK %5d {%s}.", room, ses->map->room_list[room]->name);

		add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_LINK);

		create_exit(ses, ses->map->in_room, "{%d} {%s} {%s}", room, arg1, arg1);
	}
	else
	{
		for (room = 1 ; room < ses->map->size ; room++)
		{
			if (ses->map->room_list[room] == NULL)
			{
				break;
			}
		}

		if (room == ses->map->size)
		{
			show_error(ses, LIST_COMMAND, "#MAP DIG: Maximum amount of rooms of %d reached.", ses->map->size);
			
			return;
		}
		add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_CREATE|MAP_UNDO_LINK);

		create_room(ses, "{%d} {0} {} {} { } {} {} {} {} {} {1.0}", room);
		create_exit(ses, ses->map->in_room, "{%d} {%s} {%s}", room, arg1, arg1);
	}

	if ((node = search_node_list(ses->list[LIST_PATHDIR], arg1)) != NULL)
	{
		if (find_exit(ses, room, node->right) == NULL)
		{
			create_exit(ses, room, "{%d} {%s} {%s}", ses->map->in_room, node->right, node->right);
		}
	}
}

DO_MAP(map_exit)
{
	char arg3[BUFFER_SIZE];
	struct exit_data *exit;
	int room, dir;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ONE, SUB_VAR|SUB_FUN);

	exit = find_exit(ses, ses->map->in_room, arg1);

	if (exit == NULL)
	{
		show_error(ses, LIST_COMMAND, "#MAP: Exit {%s} not found.", arg1);
		
		return;
	}
	if (*arg2 == 0)
	{
		tintin_printf2(ses, "  command: %s", exit->cmd);
		tintin_printf2(ses, "direction: %d", exit->dir);
		tintin_printf2(ses, "    flags: %d", exit->flags);
		tintin_printf2(ses, "  get/set: %s", exit->data);
		tintin_printf2(ses, "     name: %s", exit->name);
		tintin_printf2(ses, "     vnum: %d", exit->vnum);
	}
	else if (is_abbrev(arg2, "COMMAND"))
	{
		exit->cmd = restring(exit->cmd, arg3);

		show_message(ses, LIST_COMMAND, "#MAP EXIT {%s} : COMMAND SET TO {%s}.", arg1, exit->cmd);
	}
	else if (is_abbrev(arg2, "DIRECTION"))
	{
		if ((dir = get_exit_dir(ses, arg3)) == 0)
		{
			show_error(ses, LIST_COMMAND, "#MAP EXIT {%s} : DIRECTION {%s} NOT FOUND.", arg1, arg3);
			
			return;
		}

		exit->dir = dir;

		set_room_exits(ses, ses->map->in_room);

		show_message(ses, LIST_COMMAND, "#MAP EXIT {%s} : DIRECTION SET TO {%s}.", arg1, arg3);
	}
	else if (is_abbrev(arg2, "FLAGS"))
	{
		exit->flags = atoi(arg3);

		show_message(ses, LIST_COMMAND, "#MAP EXIT {%s} : FLAGS SET TO {%d}.", arg1, exit->flags);
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

		show_message(ses, LIST_COMMAND, "#MAP EXIT {%s} : NAME SET TO {%s}.", arg1, exit->name);
	}
	else if (is_abbrev(arg2, "SAVE"))
	{
		if (*arg3)
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg3, "{command}{%s}{destination}{%d}{dir}{%d}{flags}{%d}{name}{%s}{vnum}{%d}", exit->cmd, tunnel_void(ses, ses->map->in_room, exit->vnum, exit->dir), exit->dir, exit->flags, exit->name, exit->vnum);
		}
		else
		{
			show_error(ses, LIST_COMMAND, "#MAP EXIT SAVE: No destination variable.");
		}
	}
	else if (is_abbrev(arg2, "SET"))
	{
		exit->data = restring(exit->data, arg3);

		show_message(ses, LIST_COMMAND, "#MAP EXIT {%s} : DATA SET TO {%s}.", arg1, exit->data);
	}
	else if (is_abbrev(arg2, "VNUM"))
	{
		room = atoi(arg3);

		if (room <= 0 || room >= ses->map->size)
		{
			show_error(ses, LIST_COMMAND, "#MAP EXIT VNUM: Invalid room vnum: %d.", room);
			return;
		}

		if (ses->map->room_list[room] == NULL)
		{
			show_error(ses, LIST_COMMAND, "#MAP EXIT VNUM: Non existant room vnum: %d.", room);
			return;
		}
		exit->vnum = room;

		show_message(ses, LIST_COMMAND, "#MAP EXIT {%s} : VNUM SET TO {%s}.", arg1, arg3);
	}
	else
	{
		show_error(ses, LIST_COMMAND, "Syntax: #MAP EXIT {<NAME>} {COMMAND|DIRECTION|GET|NAME|FLAGS|SAVE|SET|VNUM} {<argument>}");
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
		show_error(ses, LIST_COMMAND, "#MAP: Exit {%s} not found.", arg1);

		return;
	}

	if (*arg2)
	{
		if (is_abbrev(arg2, "hide"))
		{
			flag = EXIT_FLAG_HIDE;
		}
		else if (is_abbrev(arg2, "avoid"))
		{
			flag = EXIT_FLAG_AVOID;
		}
		else
		{
			show_error(ses, LIST_COMMAND, "#MAP: Invalid exit flag {%s}.", arg2);

			return;
		}
	}
	else
	{
		tintin_printf2(ses, "#MAP: Hide flag is set to: %s.", HAS_BIT(exit->flags, EXIT_FLAG_HIDE) ? "on" : "off");
		tintin_printf2(ses, "#MAP: Avoid flag is set to: %s.", HAS_BIT(exit->flags, EXIT_FLAG_AVOID) ? "on" : "off");

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
		show_message(ses, LIST_COMMAND, "#MAP: Hide flag set to %s.", HAS_BIT(exit->flags, EXIT_FLAG_HIDE) ? "on" : "off");
	}
	else if (is_abbrev(arg2, "avoid"))
	{
		show_message(ses, LIST_COMMAND, "#MAP: Avoid flag set to %s.", HAS_BIT(exit->flags, EXIT_FLAG_AVOID) ? "on" : "off");
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
		else if (is_abbrev(arg1, "symbolgraphics"))
		{
			flag = MAP_FLAG_SYMBOLGRAPHICS;
			unflag = MAP_FLAG_ASCIIGRAPHICS;
		}
		else
		{
			show_error(ses, LIST_COMMAND, "#MAP: Invalid flag {%s}.", arg1);

			return;
		}
	}
	else
	{
		tintin_printf2(ses, "#MAP: AsciiGraphics flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS) ? "on" : "off");
		tintin_printf2(ses, "#MAP: AsciiVnums flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS) ? "on" : "off");
		tintin_printf2(ses, "#MAP: MudFont flag is set to %s", HAS_BIT(ses->map->flags, MAP_FLAG_MUDFONT) ? "on" : "off");
		tintin_printf2(ses, "#MAP: NoFollow flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW) ? "on" : "off");
		tintin_printf2(ses, "#MAP: Static flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) ? "on" : "off");
		tintin_printf2(ses, "#MAP: SymbolGraphics flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_SYMBOLGRAPHICS) ? "on" : "off");
		tintin_printf2(ses, "#MAP: VTmap flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP) ? "on" : "off");
		tintin_printf2(ses, "#MAP: VTgraphics flag is set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS) ? "on" : "off");



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

	if (is_abbrev(arg1, "asciigraphics"))
	{
		show_message(ses, LIST_COMMAND, "#MAP: AsciiGraphics flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "asciivnums"))
	{
		show_message(ses, LIST_COMMAND, "#MAP: AsciiVnums flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "mudfont"))
	{
		show_message(ses, LIST_COMMAND, "#MAP: MudFont flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_MUDFONT) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "nofollow"))
	{
		show_message(ses, LIST_COMMAND, "#MAP: NoFollow flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "static"))
	{
		show_message(ses, LIST_COMMAND, "#MAP: Static flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "symbolgraphics"))
	{
		show_message(ses, LIST_COMMAND, "#MAP: Symbolgraphics flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_SYMBOLGRAPHICS) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "vtgraphics"))
	{
		show_message(ses, LIST_COMMAND, "#MAP: VTgraphics flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS) ? "on" : "off");
	}
	else if (is_abbrev(arg1, "vtmap"))
	{
		show_message(ses, LIST_COMMAND, "#MAP: VTmap flag set to %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP) ? "on" : "off");
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
		tintin_printf2(ses, " worldflags: %d", ses->map->flags);
		tintin_printf2(ses, "  worldsize: %d", ses->map->size);
		tintin_printf2(ses, "");
		tintin_printf2(ses, "   roomvnum: %d", room->vnum);
		tintin_printf2(ses, "   roomarea: %s", room->area);
		tintin_printf2(ses, "  roomcolor: %s", room->color);
		tintin_printf2(ses, "   roomdata: %s", room->data);
		tintin_printf2(ses, "   roomdesc: %s", room->desc);
		tintin_printf2(ses, "  roomexits: %d", get_room_exits(ses, room->vnum));
		tintin_printf2(ses, "  roomflags: %d", room->flags);
		tintin_printf2(ses, "   roomname: %s", room->name);
		tintin_printf2(ses, "   roomnote: %s", room->note);
		tintin_printf2(ses, " roomsymbol: %s", room->symbol);
		tintin_printf2(ses, "roomterrain: %s", room->terrain);
		tintin_printf2(ses, " roomweight: %.3f", room->weight);
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
		else if (is_abbrev(arg1, "roomweight"))
		{
			set_nest_node(ses->list[LIST_VARIABLE], arg2, "%.3f", room->weight);
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
			show_message(ses, LIST_COMMAND, "#MAP GET: unknown option: %s.", arg1);
		}
	}
}

DO_MAP(map_goto)
{
	int room;

	room = find_room(ses, arg);

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN); // look for dig argument

	if (room == 0 && ses->map->search->vnum > 0 && ses->map->search->vnum < ses->map->size && !strcasecmp(arg2, "dig"))
	{
		room = ses->map->search->vnum;

		create_room(ses, "{%d} {0} {} {} { } {} {} {} {} {} {1.0}", room);
	}

	if (room)
	{
		add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_MOVE);

		goto_room(ses, room);

		show_message(ses, LIST_COMMAND, "#MAP GOTO: MOVED TO ROOM %d {%s}.", room, ses->map->room_list[room]->name);
	}
	else
	{

		show_message(ses, LIST_COMMAND, "#MAP GOTO: COULDN'T FIND ROOM %s.", arg1);
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

			exits += get_room_exits(ses, room);
		}
	}

	tintin_printf2(ses, "%+16s %-7d %+16s %-6d %+16s %-6d", "Total rooms:", cnt, "Total exits:", exits, "World size:", ses->map->size);

	tintin_printf2(ses, "");

	tintin_printf2(ses, "%+16s %-7s %+16s %-7s %+16s %-7s", "Vtmap:", HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP) ? "on" : "off", "Static:", HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) ? "on" : "off", "Vtgraphics:", HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS) ? "on" : "off");
	tintin_printf2(ses, "%+16s %-7s %+16s %-7s %+16s %-7s", "Asciigraphics:", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS) ? "on" : "off", "Asciivnums:", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS) ? "on" : "off", "Nofollow:", HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW) ? "on" : "off");

	if (ses->map->in_room == 0)
	{
		return;
	}



	tintin_printf2(ses, "");

	tintin_printf2(ses, "%+16s %s", "Room area:",    ses->map->room_list[ses->map->in_room]->area);
	tintin_printf2(ses, "%+16s %s", "Room data:",    ses->map->room_list[ses->map->in_room]->data);
	tintin_printf2(ses, "%+16s %s", "Room desc:",    ses->map->room_list[ses->map->in_room]->desc);
	tintin_printf2(ses, "%+16s %s", "Room name:",    ses->map->room_list[ses->map->in_room]->name);
	tintin_printf2(ses, "%+16s %s", "Room note:",    ses->map->room_list[ses->map->in_room]->note);
	tintin_printf2(ses, "%+16s %s", "Room symbol:",  ses->map->room_list[ses->map->in_room]->symbol);
	tintin_printf2(ses, "%+16s %s", "Room terrain:", ses->map->room_list[ses->map->in_room]->terrain);
	tintin_printf2(ses, "%+16s %d", "Room vnum:",    ses->map->in_room);
	tintin_printf2(ses, "%+16s %.3f", "Room weight:", ses->map->room_list[ses->map->in_room]->weight);
	tintin_printf2(ses, "");

	tintin_printf2(ses, "%+16s %-7s %+16s %-7s %+16s %-7s", "Avoid:", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_AVOID) ? "on" : "off", "Hide:", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_HIDE) ? "on" : "off", "Leave:",  HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE) ? "on" : "off");
	tintin_printf2(ses, "%+16s %-7s %+16s %-7s",             "Void:", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID) ? "on" : "off", "Static:", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_STATIC) ? "on" : "off");

	tintin_printf2(ses, "");

	for (exit = ses->map->room_list[ses->map->in_room]->f_exit ; exit ; exit = exit->next)
	{
		tintin_printf2(ses, "%+16s %-3s (%3s)   to room: %-5d (%5s)", "Exit:", exit->name, exit->cmd, exit->vnum, ses->map->room_list[exit->vnum]->name);
	}

	tintin_printf2(ses, "");

	for (room = 0 ; room < ses->map->size ; room++)
	{
		if (ses->map->room_list[room])
		{
			for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
			{
				if (exit->vnum == ses->map->in_room)
				{
					tintin_printf2(ses, "%+16s %-3s (%3s) from room: %-5d (%5s)", "Entrance:", exit->name, exit->cmd, room, ses->map->room_list[room]->name);
				}
			}
		}
	}
/*
	for (exit = ses->map->room_list[ses->map->in_room]->f_exit ; exit ; exit = exit->next)
	{
		tintin_printf2(ses, "%+14s %-4s %+14s %5d %+14s %5d %+14s %5s", "Exit name:", exit->name, "vnum:", exit->vnum, "flags:", exit->flags, "command:", exit->cmd);
		tintin_printf2(ses, "%+14s %s", "Exit data:", exit->data);
	}
*/
}

DO_MAP(map_insert)
{
	int room, in_room, to_room;
	struct exit_data *exit;
	struct listnode *node;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);

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
		show_error(ses, LIST_COMMAND, "#MAP: There is no room in that direction.");

		return;
	}

	if (room == ses->map->size)
	{
		show_error(ses, LIST_COMMAND, "#MAP: Maximum amount of rooms of %d reached.", ses->map->size);
		return;
	}

	if (node == NULL)
	{
		show_error(ses, LIST_COMMAND, "#MAP: Given direction must be a pathdir.");
		return;
	}

	in_room = ses->map->in_room;
	to_room = exit->vnum;

	add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_INSERT);

	create_room(ses, "{%d} {0} {} {} { } {} {} {} {} {} {1.0}", room);

	create_exit(ses, room, "{%d} {%s} {%s}", to_room, node->left, node->left);

	create_exit(ses, room, "{%d} {%s} {%s}", in_room, node->right, node->right);

	exit->vnum = room;

	if ((exit = find_exit(ses, to_room, node->right)) != NULL)
	{
		exit->vnum = room;
	}

	if (*arg)
	{
		ses->map->in_room = room;
		map_roomflag(ses, arg, arg1, arg2);
		ses->map->in_room = in_room;
	}
	show_message(ses, LIST_COMMAND, "#MAP: Inserted room {%d}.", room);
}

DO_MAP(map_jump)
{
	char arg3[BUFFER_SIZE];
	int room, x, y, z;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ALL, SUB_VAR|SUB_FUN);

	x = get_number(ses, arg1);
	y = get_number(ses, arg2);
	z = get_number(ses, arg3);

	room = spatialgrid_find(ses, ses->map->in_room, x, y, z);

	if (room)
	{
		add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_MOVE);

		goto_room(ses, room);

		show_message(ses, LIST_COMMAND, "#MAP JUMP: JUMPED TO ROOM %d {%s}.", room, ses->map->room_list[room]->name);
	}
	else
	{
		show_message(ses, LIST_COMMAND, "#MAP JUMP: Couldn't find a room at {%d} {%d} {%d}.", x, y, z);
	}
}

DO_MAP(map_leave)
{
	if (ses->map->in_room == 0)
	{
		show_error(ses, LIST_COMMAND, "#MAP: You're not currently inside the map.");
	}
	else
	{
		ses->map->last_room = ses->map->in_room;
		ses->map->in_room = 0;

		show_message(ses, LIST_COMMAND, "#MAP: Leaving the map. Use goto or return to return.");

		check_all_events(ses, SUB_ARG|SUB_SEC, 0, 1, "MAP EXIT MAP", ntos(ses->map->in_room));
	}
}

DO_MAP(map_legend)
{
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1 == 0)
	{
		if (HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS))
		{
			tintin_printf2(ses, "#MAP: Current legend: \033[12m%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\033[10m",
				ses->map->legend[0],  ses->map->legend[1],  ses->map->legend[2],
				ses->map->legend[3],  ses->map->legend[4],  ses->map->legend[5],
				ses->map->legend[6],  ses->map->legend[7],  ses->map->legend[8],
				ses->map->legend[9],  ses->map->legend[10], ses->map->legend[11],
				ses->map->legend[12], ses->map->legend[13], ses->map->legend[14],
				ses->map->legend[15], ses->map->legend[16]);
		}
		else
		{
			tintin_printf2(ses, "#MAP: Current legend: %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
				ses->map->legend[0],  ses->map->legend[1],  ses->map->legend[2],
				ses->map->legend[3],  ses->map->legend[4],  ses->map->legend[5],
				ses->map->legend[6],  ses->map->legend[7],  ses->map->legend[8],
				ses->map->legend[9],  ses->map->legend[10], ses->map->legend[11],
				ses->map->legend[12], ses->map->legend[13], ses->map->legend[14],
				ses->map->legend[15], ses->map->legend[16]);
		}
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
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ONE, SUB_VAR|SUB_FUN);

	if (*arg1 == 0 || *arg2 == 0)
	{
		show_error(ses, LIST_COMMAND, "#SYNTAX: #MAP LINK {<DIRECTION>} {<LOCATION>} {BOTH}");
		return;
	}

	room = find_room(ses, arg2);

	if (room == 0)
	{
		show_error(ses, LIST_COMMAND, "#MAP: Couldn't find room {%s}.", arg1);
		return;
	}

	exit = find_exit(ses, ses->map->in_room, arg1);

	if (exit)
	{
		delete_exit(ses, ses->map->in_room, exit);
	}

	create_exit(ses, ses->map->in_room, "{%d} {%s} {%s}", room, arg1, arg1);

	if (is_abbrev(arg3, "both"))
	{
		if ((node = search_node_list(ses->list[LIST_PATHDIR], arg1)) != NULL)
		{
			if (find_exit(ses, room, node->right) == NULL)
			{
				create_exit(ses, room, "{%d} {%s} {%s}", ses->map->in_room, node->right, node->right);
			}
		}
	}
	show_message(ses, LIST_COMMAND, "#MAP LINK: Connected room {%s} to {%s}.", ses->map->room_list[ses->map->in_room]->name, ses->map->room_list[room]->name);
}

DO_MAP(map_list)
{
	char var[BUFFER_SIZE];
	int room;

	map_search_compile(ses, "0", var);

	searchgrid_find(ses, ses->map->in_room, ses->map->search);

	map_search_compile(ses, arg, var);

	for (room = 0 ; room < ses->map->size ; room++)
	{
		if (match_room(ses, room, ses->map->search))
		{
			if (*var)
			{
				add_nest_node(ses->list[LIST_VARIABLE], var, "{%d} {%.3f}", ses->map->room_list[room]->vnum, ses->map->search->stamp == ses->map->room_list[room]->search_stamp ? ses->map->room_list[room]->length  : -1);
			}
			else
			{
				tintin_printf2(ses, "vnum: %5d   distance: %8.3f   name: %s", ses->map->room_list[room]->vnum, ses->map->search->stamp == ses->map->room_list[room]->search_stamp ? ses->map->room_list[room]->length  : -1, ses->map->room_list[room]->name);
			}
		}
	}
}

DO_MAP(map_map)
{
	char arg3[BUFFER_SIZE], buf[BUFFER_SIZE], out[BUFFER_SIZE];
	FILE *logfile = NULL;
	int x, y, line, var = 0;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ONE, SUB_VAR|SUB_FUN);

	push_call("map_map(%p,%p)",ses,arg);

	if (sscanf(arg1, "%dx%d", &map_grid_x, &map_grid_y) == 2)
	{
		if (*arg2)
		{
			if (tolower((int) *arg3) == 'v')
			{
				var = 1;
			}
			else if (tolower((int) *arg3) == 'a')
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

	displaygrid_build(ses, ses->map->in_room, map_grid_x, map_grid_y, 0);

	*arg3 = 0;

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

				substitute(ses, buf, out, SUB_COL|SUB_CMP);

				if (var)
				{
					cat_sprintf(arg3, "%s\n", out);
				}
				else if (logfile)
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

			substitute(ses, buf, out, SUB_COL|SUB_CMP);

			if (var)
			{
				cat_sprintf(arg3, "%s\n", out);
			}
			else if (logfile)
			{
				fprintf(logfile, "%s\n", out);
			}
			else
			{
				tintin_puts2(ses, out);
			}
		}
	}

	if (var)
	{
		set_nest_node(ses->list[LIST_VARIABLE], arg2, "%s", arg3);
	}
	else if (logfile)
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
		show_error(ses, LIST_COMMAND, "#MAP: Map file {%s} not found.", arg1);

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

					case 'B':
						ses->map->back_color = strdup(buffer + 3);
						break;
					case 'E':
						ses->map->exit_color = strdup(buffer + 3);
						break;

					case 'H':
						ses->map->here_color = strdup(buffer + 3);
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
				create_exit(ses, room, "%s", buffer + 2);
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
				room = create_room(ses, "%s", buffer + 2);
				break;

			case '#':
				buffer[0] = gtd->tintin_char;
				ses = script_driver(ses, LIST_COMMAND, buffer);
				break;

			case  0:
			case 13:
				break;

			default:
				gtd->quiet--;

				show_error(ses, LIST_COMMAND, "#MAP: INVALID COMMAND {%d} {%s} ON LINE %d. ABORTING READ..", buffer[0], buffer, line);

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
				show_error(ses, LIST_COMMAND, "#MAP READ: Room %d - invalid exit '%s' to room %d.", room, exit->name, exit->vnum);

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

	show_message(ses, LIST_COMMAND, "#MAP READ: Map file {%s} loaded.", arg1);


}

DO_MAP(map_resize)
{
	int size, vnum, room;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	size = atoi(arg1);

	if (size <= ses->map->size)
	{
		for (room = vnum = 1 ; vnum < ses->map->size ; vnum++)
		{
			if (ses->map->room_list[vnum])
			{
				room = vnum;
			}
		}

		if (room >= size)
		{
			show_error(ses, LIST_COMMAND, "#MAP RESIZE: YOU MUST DELETE ALL ROOMS WITH VNUMS ABOVE (%d) FIRST.", size);
			return;
		}
	}

	ses->map->room_list = (struct room_data **) realloc(ses->map->room_list, size * sizeof(struct room_data *));

	if (ses->map->size < size)
	{
		while (ses->map->size < size)
		{
			ses->map->room_list[ses->map->size++] = NULL;
		}
	}
	else
	{
		ses->map->size = size;
	}

	show_message(ses, LIST_COMMAND, "#MAP RESIZE: MAP RESIZED TO %d ROOMS.", ses->map->size);
}

DO_MAP(map_return)
{
	if (ses->map == NULL || ses->map->room_list[ses->map->last_room] == NULL)
	{
		show_error(ses, LIST_COMMAND, "#MAP RETURN: NO KNOWN LAST ROOM.");

		return;
	}

	if (ses->map->in_room)
	{
		show_error(ses, LIST_COMMAND, "#MAP RETURN: ALREADY IN THE MAP.");
	}
	else
	{
		goto_room(ses, ses->map->last_room);

		show_message(ses, LIST_COMMAND, "#MAP RETURN: RETURNED TO ROOM %d {%s}.", ses->map->in_room, ses->map->room_list[ses->map->in_room]->name);
	}
}

DO_MAP(map_roomflag)
{
	char buf[BUFFER_SIZE];
	int flag = 0;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1 == 0)
	{
		tintin_printf2(ses, "#MAP: Avoid flag is set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_AVOID) ? "on" : "off");
		tintin_printf2(ses, "#MAP: Hide flag is set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_HIDE) ? "on" : "off");
		tintin_printf2(ses, "#MAP: Leave flag is set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE) ? "on" : "off");
		tintin_printf2(ses, "#MAP: Void flag is set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID) ? "on" : "off");
		tintin_printf2(ses, "#MAP: Static flag is set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_STATIC) ? "on" : "off");

		return;
	}

	while (*arg1)
	{
		arg1 = get_arg_in_braces(ses, arg1, buf, GET_ONE);

		if (is_abbrev(buf, "avoid"))
		{
			SET_BIT(flag, ROOM_FLAG_AVOID);
		}
		else if (is_abbrev(buf, "hide"))
		{
			SET_BIT(flag, ROOM_FLAG_HIDE);
		}
		else if (is_abbrev(buf, "leave"))
		{
			SET_BIT(flag, ROOM_FLAG_LEAVE);
		}
		else if (is_abbrev(buf, "void"))
		{
			SET_BIT(flag, ROOM_FLAG_VOID);
		}
		else if (is_abbrev(buf, "static"))
		{
			SET_BIT(flag, ROOM_FLAG_STATIC);
		}
		else if (is_abbrev(buf, "river"))
		{
			SET_BIT(flag, ROOM_FLAG_RIVER);
		}
		else
		{
			show_error(ses, LIST_COMMAND, "#MAP: Invalid room flag {%s}.", buf);

			return;
		}
		
		if (*arg1 == COMMAND_SEPARATOR) arg1++;
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


	if (HAS_BIT(flag, ROOM_FLAG_AVOID))
	{
		show_message(ses, LIST_COMMAND, "#MAP: Avoid flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_AVOID) ? "on" : "off");
	}
	if (HAS_BIT(flag, ROOM_FLAG_HIDE))
	{
		show_message(ses, LIST_COMMAND, "#MAP: Hide flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_HIDE) ? "on" : "off");
	}
	if (HAS_BIT(flag, ROOM_FLAG_LEAVE))
	{
		show_message(ses, LIST_COMMAND, "#MAP: Leave flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE) ? "on" : "off");
	}
	if (HAS_BIT(flag, ROOM_FLAG_VOID))
	{
		show_message(ses, LIST_COMMAND, "#MAP: Void flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID) ? "on" : "off");
	}
	if (HAS_BIT(flag, ROOM_FLAG_STATIC))
	{
		show_message(ses, LIST_COMMAND, "#MAP: Static flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_STATIC) ? "on" : "off");
	}
	if (HAS_BIT(flag, ROOM_FLAG_RIVER))
	{
		show_message(ses, LIST_COMMAND, "#MAP: River flag set to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_RIVER) ? "on" : "off");
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
		show_message(ses, LIST_COMMAND, "#MAP SET: invalid room vnum: %s", arg3);
	}
	else if (*arg1 == 0)
	{
		tintin_printf2(ses, "   roomarea: %s", room->area);
		tintin_printf2(ses, "  roomcolor: %s", room->color);
		tintin_printf2(ses, "   roomdata: %s", room->data);
		tintin_printf2(ses, "   roomdesc: %s", room->desc);
		tintin_printf2(ses, "  roomflags: %d", room->flags);
		tintin_printf2(ses, "   roomname: %s", room->name);
		tintin_printf2(ses, "   roomnote: %s", room->note);
		tintin_printf2(ses, " roomsymbol: %s", room->symbol);
		tintin_printf2(ses, "roomterrain: %s", room->terrain);
		tintin_printf2(ses, " roomweight: %.3f", room->weight);
	}
	else
	{
		if (is_abbrev(arg1, "roomarea"))
		{
			RESTRING(room->area, arg2);
			show_message(ses, LIST_COMMAND, "#MAP SET: roomarea set to: %s", room->area);
		}
		else if (is_abbrev(arg1, "roomcolor"))
		{
			RESTRING(room->color, arg2);
			show_message(ses, LIST_COMMAND, "#MAP SET: roomcolor set to: %s", arg2);
		}
		else if (is_abbrev(arg1, "roomdata"))
		{
			RESTRING(room->data, arg2);
			show_message(ses, LIST_COMMAND, "#MAP SET: roomdata set to: %s", arg2);
		}
		else if (is_abbrev(arg1, "roomdesc"))
		{
			RESTRING(room->desc, arg2);
			show_message(ses, LIST_COMMAND, "#MAP SET: roomdesc set to: %s", arg2);
		}
		else if (is_abbrev(arg1, "roomflags"))
		{
			room->flags = (int) get_number(ses, arg2);

			show_message(ses, LIST_COMMAND, "#MAP SET: roomflags set to: %d", room->flags);
		}
		else if (is_abbrev(arg1, "roomname"))
		{
			RESTRING(room->name, arg2);

			show_message(ses, LIST_COMMAND, "#MAP SET: roomname set to: %s", room->name);
		}
		else if (is_abbrev(arg1, "roomnote"))
		{
			RESTRING(room->note, arg2);
			show_message(ses, LIST_COMMAND, "#MAP SET: roomnote set to: %s", arg2);
		}
		else if (is_abbrev(arg1, "roomsymbol"))
		{
			RESTRING(room->symbol, arg2);

			show_message(ses, LIST_COMMAND, "#MAP SET: roomsymbol set to: %s", room->symbol);
		}
		else if (is_abbrev(arg1, "roomterrain"))
		{
			RESTRING(room->terrain, arg2);
			show_message(ses, LIST_COMMAND, "#MAP SET: roomterrain set to: %s", arg2);
		}
		else if (is_abbrev(arg1, "roomweight"))
		{
			room->weight = (float) get_number(ses, arg2);

			show_message(ses, LIST_COMMAND, "#MAP SET: roomweight set to: %.3f", room->weight);
		}
		else
		{
			show_message(ses, LIST_COMMAND, "#MAP SET: unknown option: %s", arg1);
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
		show_error(ses, LIST_COMMAND, "#MAP UNINSERT: There is no room in that direction.");

		return;
	}

	if (node == NULL)
	{
		show_error(ses, LIST_COMMAND, "#MAP UNINSERT: Given direction must be a pathdir.");
		return;
	}

	room2 = exit1->vnum;
	exit2 = find_exit(ses, room2, node->left);

	if (exit2 == NULL)
	{
		show_error(ses, LIST_COMMAND, "#MAP UNINSERT: Unable to find backlink room.");
		return;
	}

	room3 = exit2->vnum;
	exit3 = find_exit(ses, room3, node->right);

	if (exit3 == NULL)
	{
		show_error(ses, LIST_COMMAND, "#MAP UNINSERT: Unable to find backlink exit.");

		return;
	}

	exit1->vnum = room3;
	exit3->vnum = room1;

	delete_room(ses, room2, TRUE);

	show_message(ses, LIST_COMMAND, "#MAP UNINSERT: Uninserted room {%d}.", room2);
}

// 1) timestamp 2) type 3) data

DO_MAP(map_undo)
{
	struct link_data *link;
	struct room_data *room;
	int undo_flag;
	struct exit_data *exit1, *exit2, *exit3;

	link = ses->map->undo_tail;

	if (link == NULL)
	{
		show_error(ses, LIST_COMMAND, "#MAP UNDO: No known last move.");

		return;
	}

	room = ses->map->room_list[atoi(link->str1)];

	if (room == NULL)
	{
		show_error(ses, LIST_COMMAND, "#MAP UNDO: Room %s does not exist.", link->str2);
		return;
	}

	if (ses->map->room_list[atoi(link->str2)] == NULL)
	{
		show_error(ses, LIST_COMMAND, "#MAP UNDO: Invalid last move.");
		return;
	}

	undo_flag = atoi(link->str3);

	if (HAS_BIT(undo_flag, MAP_UNDO_MOVE))
	{
 		if (ses->map->in_room != room->vnum)
		{
			show_error(ses, LIST_COMMAND, "#MAP UNDO: Invalid last move.");
			return;
		}
		show_message(ses, LIST_COMMAND, "#MAP UNDO: Moving to room %s.", link->str2);

		goto_room(ses, atoi(link->str2));
	}

	if (HAS_BIT(undo_flag, MAP_UNDO_CREATE))
	{
		show_message(ses, LIST_COMMAND, "#MAP UNDO: Deleting room %d.", room->vnum);
		delete_room(ses, room->vnum, TRUE);
	}
	else if (HAS_BIT(undo_flag, MAP_UNDO_LINK))
	{
		exit1 = find_exit(ses, room->vnum, link->str2);

		if (exit1)
		{
			show_message(ses, LIST_COMMAND, "#MAP UNDO: Deleting exit leading %s.", exit1->name);
			delete_exit(ses, room->vnum, exit1);
		}

		exit2 = find_exit(ses, atoi(link->str2), link->str1);

		if (exit2)
		{
			show_message(ses, LIST_COMMAND, "#MAP UNDO: Deleting exit leading %s.", exit2->name);
			delete_exit(ses, atoi(link->str2), exit2);
		}
	}
	else if (HAS_BIT(undo_flag, MAP_UNDO_INSERT))
	{
		exit1 = find_exit(ses, atoi(link->str2), link->str1);

		if (exit1 == NULL)
		{
			show_error(ses, LIST_COMMAND, "#MAP UNDO: Can't find exit between %s and %s.", link->str2, link->str1);
			return;
		}

		exit2 = find_exit(ses, room->vnum, exit1->name);

		if (exit2 == NULL)
		{
			show_error(ses, LIST_COMMAND, "#MAP UNDO: No valid exit found in room %d.", room->vnum);
			return;
		}

		exit3 = find_exit(ses, exit2->vnum, ntos(room->vnum));

		if (exit3 == NULL)
		{
			show_error(ses, LIST_COMMAND, "#MAP UNDO: Can't find exit between %d and %d.", room->vnum, exit2->vnum);
			return;
		}

		exit1->vnum = exit2->vnum;
		exit3->vnum = atoi(link->str2);

		delete_room(ses, room->vnum, TRUE);

		show_message(ses, LIST_COMMAND, "#MAP UNDO: Uninserting room %s.", link->str1);
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
		show_error(ses, LIST_COMMAND, "#MAP UNLINK: No exit with that name found");

		return;
	}

	if (*arg2 == 'b' || *arg == 'B')
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

	show_message(ses, LIST_COMMAND, "#MAP UNLINK: Exit deleted.");
}

DO_MAP(map_run)
{
	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	shortest_path(ses, TRUE, arg2, arg1);
}

DO_MAP(map_vnum)
{
	int vnum, vnum1, vnum2, old_room, new_room;
	struct exit_data *exit;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ONE, SUB_VAR|SUB_FUN);

	vnum1 = atoi(arg1);

	if (*arg2)
	{
		vnum2 = atoi(arg2);
	}
	else
	{
		vnum2 = vnum1;
	}
	
	if (vnum1 <= 0 || vnum1 >= ses->map->size || vnum2 <= 0 || vnum2 >= ses->map->size)
	{
		show_error(ses, LIST_COMMAND, "#MAP VNUM {%s} {%s} - VNUMS MUST BE BETWEEN {1} and {%d}", arg1, arg2, ses->map->size - 1);
		return;
	}

	for (vnum = vnum1 ; vnum <= vnum2 ; vnum++)
	{
		if (ses->map->room_list[vnum] == NULL)
		{
			break;
		}
	}

	if (vnum > vnum2)
	{
		show_error(ses, LIST_COMMAND, "#MAP VNUM {%s} {%s} - NO FREE VNUM FOUND.", arg1, arg2);
		return;
	}

	old_room = ses->map->in_room;
	new_room = vnum;

	ses->map->room_list[new_room] = ses->map->room_list[old_room];
	ses->map->room_list[new_room]->vnum = new_room;
	ses->map->room_list[old_room] = NULL;
	ses->map->in_room = new_room;

	if (ses->map->at_room == old_room)
	{
		ses->map->at_room = new_room;
	}

	for (vnum = 1 ; vnum < ses->map->size ; vnum++)
	{
		if (ses->map->room_list[vnum] == NULL)
		{
			continue;
		}

		for (exit = ses->map->room_list[vnum]->f_exit ; exit ; exit = exit->next)
		{
			if (exit->vnum == old_room)
			{
				exit->vnum = new_room;
			}
		}
	}

	tintin_printf(ses, "#MAP VNUM: MOVED ROOM %d TO %d.", old_room, new_room);
}

DO_MAP(map_write)
{
	FILE *file;
	struct exit_data *exit;
	int vnum;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1 == 0 || (file = fopen(arg1, "w")) == NULL)
	{
		show_error(ses, LIST_COMMAND, "#MAP WRITE {%s} - COULDN'T OPEN FILE TO WRITE.", arg1);

		return;
	}

	fprintf(file, "C %d\n\n", ses->map->size);

	fprintf(file, "CE %s\nCH %s\nCP %s\nCR %s\nCB %s\n\n", ses->map->exit_color, ses->map->here_color, ses->map->path_color, ses->map->room_color, ses->map->back_color);

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
			fprintf(file, "\nR {%5d} {%d} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%.3f}\n",
				ses->map->room_list[vnum]->vnum,
				ses->map->room_list[vnum]->flags,
				ses->map->room_list[vnum]->color,
				ses->map->room_list[vnum]->name,
				ses->map->room_list[vnum]->symbol,
				ses->map->room_list[vnum]->desc,
				ses->map->room_list[vnum]->area,
				ses->map->room_list[vnum]->note,
				ses->map->room_list[vnum]->terrain,
				ses->map->room_list[vnum]->data,
				ses->map->room_list[vnum]->weight);

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

	show_message(ses, LIST_COMMAND, "#MAP: Map file written to {%s}.", arg1);
}

void create_map(struct session *ses, char *arg)
{
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

	ses->map->search = (struct search_data *) calloc(1, sizeof(struct search_data));

	ses->map->flags = MAP_FLAG_ASCIIGRAPHICS;

	ses->map->exit_color = strdup("<278>");
	ses->map->here_color = strdup("<118>");
	ses->map->path_color = strdup("<138>");
	ses->map->room_color = strdup("<178>");
	ses->map->back_color = strdup("");

	create_room(ses, "%s", "{1} {0} {} {} { } {} {} {} {} {} {1.0}");

	if (!HAS_BIT(ses->flags, SES_FLAG_UTF8))
	{
		create_legend(ses, "{*}{#}{#}{+}{#}{|}{+}{+}{#}{+}{-}{+}{+}{+}{+}{+}{x}");
	}
	else
	{
		create_legend(ses, "{\xE2\x80\xA2}{\xE2\x95\xB9}{\xE2\x95\xBA}{\xE2\x94\x97}{\xE2\x95\xBB}{\xE2\x94\x83}{\xE2\x94\x8F}{\xE2\x94\xA3}{\xE2\x95\xB8}{\xE2\x94\x9B}{\xE2\x94\x81}{\xE2\x94\xBB}{\xE2\x94\x93}{\xE2\x94\xAB}{\xE2\x94\xB3}{\xE2\x95\x8B}{X}");
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


int create_room(struct session *ses, char *format, ...)
{
	char *arg, buf[BUFFER_SIZE];
	struct room_data *newroom;
	va_list args;

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	newroom          = (struct room_data *) calloc(1, sizeof(struct room_data));

	arg = buf;

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
	arg = get_arg_in_braces(ses, arg, buf, FALSE); newroom->weight  = atof(buf);

	if (newroom->weight <= 0)
	{
		newroom->weight = 1;
	}

	ses->map->room_list[newroom->vnum] = newroom;

	show_message(ses, LIST_COMMAND, "#MAP CREATE ROOM %5d {%s}.", newroom->vnum, newroom->name);

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
	free(ses->map->room_list[room]->terrain);
	free(ses->map->room_list[room]->data); 

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

void create_exit(struct session *ses, int room, char *format, ...)
{
	struct exit_data *newexit;
	va_list args;

	char *arg, buf[BUFFER_SIZE], vnum[BUFFER_SIZE], name[BUFFER_SIZE], cmd[BUFFER_SIZE], dir[BUFFER_SIZE], flags[BUFFER_SIZE], data[BUFFER_SIZE];

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	arg = buf;

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
		newexit->dir = get_exit_dir(ses, name);
	}

	LINK(newexit, ses->map->room_list[room]->f_exit, ses->map->room_list[room]->l_exit);

	ses->map->room_list[room]->exit_size++;
	SET_BIT(ses->map->room_list[room]->exit_dirs, (1LL << newexit->dir));

	show_message(ses, LIST_COMMAND, "#MAP CREATE EXIT %5s {%s} {%s}.", vnum, name, cmd);
}

void delete_exit(struct session *ses, int room, struct exit_data *exit)
{
	free(exit->name);
	free(exit->cmd);
	free(exit->data);

	UNLINK(exit, ses->map->room_list[room]->f_exit, ses->map->room_list[room]->l_exit)

	set_room_exits(ses, room);

	free(exit);
}

int get_exit_dir(struct session *ses, char *arg)
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

int get_room_exits(struct session *ses, int room)
{
	return ses->map->room_list[room]->exit_size;
}

void set_room_exits(struct session *ses, int room)
{
	struct exit_data *exit;

	ses->map->room_list[room]->exit_dirs = 0;
	ses->map->room_list[room]->exit_size = 0;

	for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
	{
		SET_BIT(ses->map->room_list[room]->exit_dirs, 1LL << exit->dir);

		ses->map->room_list[room]->exit_size++;
	}
}

int follow_map(struct session *ses, char *argument)
{
	struct exit_data *exit;
	int room, dir;

	push_call("follow_map(%p,%p)",ses,argument);

	exit = find_exit(ses, ses->map->in_room, argument);

	if (exit)
	{
		room = ses->map->in_room;

		add_undo(ses, "%d %d %d", exit->vnum, room, MAP_UNDO_MOVE);

		if (ses->map->nofollow == 0)
		{
			ses->map->nofollow++;

			script_driver(ses, LIST_COMMAND, exit->cmd);

			ses->map->nofollow--;
		}
		goto_room(ses, exit->vnum);

		if (HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE))
		{
			show_message(ses, LIST_COMMAND, "#MAP: LEAVE FLAG FOUND IN ROOM {%d}. LEAVING MAP.", ses->map->in_room);

			map_leave(ses, "", "", "");
		}
		else if (HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID))
		{
			if (get_room_exits(ses, ses->map->in_room) == 2)
			{
				ses->map->nofollow++;

				if (ses->map->room_list[ses->map->in_room]->f_exit && ses->map->room_list[ses->map->in_room]->f_exit->vnum != room)
				{
					follow_map(ses, ses->map->room_list[ses->map->in_room]->f_exit->name);
				}
				else if (ses->map->room_list[ses->map->in_room]->l_exit && ses->map->room_list[ses->map->in_room]->l_exit->vnum != room)
				{
					follow_map(ses, ses->map->room_list[ses->map->in_room]->l_exit->name);
				}

				ses->map->nofollow--;

				pop_call();
				return 1;
			}
			else
			{
				dir = exit->dir;

				for (exit = ses->map->room_list[ses->map->in_room]->f_exit ; exit ; exit = exit->next)
				{
					if (exit->dir == dir)
					{
						ses->map->nofollow++;

						follow_map(ses, exit->name);

						ses->map->nofollow--;

						pop_call();
						return 1;
					}
				}
			}
		}

		if (HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP))
		{
			SET_BIT(ses->flags, SES_FLAG_UPDATEVTMAP);
		}

		pop_call();
		return 1;
	}

	if (!HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) && !HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_STATIC))
	{
		struct listnode *node;

		if ((node = search_node_list(ses->list[LIST_PATHDIR], argument)) == NULL)
		{
			pop_call();
			return 0;
		}

		room = find_coord(ses, argument);

		if (room)
		{
			show_message(ses, LIST_COMMAND, "#MAP CREATE LINK %5d {%s}.", room, ses->map->room_list[room]->name);

			add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_MOVE|MAP_UNDO_LINK);
		}
		else
		{
			for (room = 1 ; room < ses->map->size ; room++)
			{
				if (ses->map->room_list[room] == NULL)
				{
					break;
				}
			}

			if (room == ses->map->size)
			{
				show_error(ses, LIST_COMMAND, "#MAP: Maximum amount of rooms of %d reached. Use #MAP RESIZE to increase the maximum.", ses->map->size);

				pop_call();
				return 1;
			}
			add_undo(ses, "%d %d %d", room, ses->map->in_room, MAP_UNDO_MOVE|MAP_UNDO_CREATE|MAP_UNDO_LINK);

			create_room(ses, "{%d} {0} {} {} { } {} {} {} {} {} {1.0}", room);
		}
		create_exit(ses, ses->map->in_room, "{%d} {%s} {%s}", room, node->left, node->left);

		if (find_exit(ses, room, node->right) == NULL)
		{
			create_exit(ses, room, "{%d} {%s} {%s}", ses->map->in_room, node->right, node->right);
		}

		if (ses->map->nofollow == 0)
		{
			ses->map->nofollow++;

			script_driver(ses, LIST_COMMAND, argument);

			ses->map->nofollow--;
		}
		goto_room(ses, room);

		if (HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP))
		{
			SET_BIT(ses->flags, SES_FLAG_UPDATEVTMAP);
		}

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



struct grid_node
{
	int vnum;
	float length;
	int x;
	int y;
	int z;
};

struct breadth_node
{
	int vnum;
	float length;
};

void displaygrid_build(struct session *ses, int vnum, int x, int y, int z)
{
	int head, tail;
	struct grid_node *node, *temp, list[MAP_BF_SIZE];
	struct exit_data *exit;
	struct room_data *room;

	map_grid_x = x;
	map_grid_y = y;

	head = 0;
	tail = 1;

	node = &list[head];

	node->vnum   = vnum;
	node->x      = x / 2;
	node->y      = y / 2;
	node->z      = z / 2;
	node->length = 0;

	ses->map->display_stamp++;

	for (vnum = 0 ; vnum < x * y ; vnum++)
	{
		ses->map->grid[vnum] = NULL;
	}

	while (head != tail)
	{
		node = &list[head];

		head = (head + 1) % MAP_BF_SIZE;

		room = ses->map->room_list[node->vnum];

		if (ses->map->display_stamp != room->display_stamp)
		{
			room->display_stamp = ses->map->display_stamp;
		}
		else if (room->length <= node->length)
		{
			continue;
		}

		room->length = node->length;

		if (node->x >= 0 && node->x < map_grid_x && node->y >= 0 && node->y < map_grid_y && node->z == 0)
		{
			if (ses->map->grid[node->x + map_grid_x * node->y] == NULL)
			{
				ses->map->grid[node->x + map_grid_x * node->y] = room;
			}
			else
			{
				continue;
			}
		}

		for (exit = room->f_exit ; exit ; exit = exit->next)
		{
			if (ses->map->display_stamp == ses->map->room_list[exit->vnum]->display_stamp)
			{
				if (room->length >= ses->map->room_list[exit->vnum]->length)
				{
					continue;
				}
			}

			if (exit->dir == 0)
			{
				continue;
			}

			if (HAS_BIT(exit->flags, EXIT_FLAG_HIDE) || HAS_BIT(ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_HIDE))
			{
				continue;
			}

			if (head == (tail + 1) % MAP_BF_SIZE)
			{
				break;
			}

			temp = &list[tail];

			temp->vnum   = exit->vnum;
			temp->x      = node->x + (HAS_BIT(exit->dir, MAP_EXIT_E) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_W) ? -1 : 0);
			temp->y      = node->y + (HAS_BIT(exit->dir, MAP_EXIT_N) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_S) ? -1 : 0);
			temp->z      = node->z + (HAS_BIT(exit->dir, MAP_EXIT_U) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_D) ? -1 : 0);
			temp->length = node->length + 1;

			tail = (tail + 1) % MAP_BF_SIZE;
		}
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
	displaygrid_build(ses, ses->map->in_room, map_grid_x, map_grid_y, 0);

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
				substitute(ses, buf, out, SUB_COL|SUB_CMP);

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
			substitute(ses, buf, out, SUB_COL|SUB_CMP);

			printf("%s\n", out);
		}
	}

	restore_pos(ses);

	pop_call();
	return;
}



char *draw_room(struct session *ses, struct room_data *room, int line)
{
	static char buf[101];
	int exits, exit1, exit2, room1, room2;

	push_call("draw_room(%p,%p,%p)",ses,room,line);

	if (line)
	{
		if (room == NULL)
		{
			sprintf(buf, "%s      ", ses->map->back_color);
			pop_call();
			return buf;
		}

		sprintf(buf, "%s%s", ses->map->back_color, ses->map->exit_color);

		switch (line)
		{
			case 1:
				strcat(buf, HAS_BIT(room->exit_dirs, MAP_DIR_NW) ? "\\ " : "  ");
				strcat(buf, HAS_BIT(room->exit_dirs, MAP_DIR_N)  ? "|"   : " ");
				strcat(buf, HAS_BIT(room->exit_dirs, MAP_DIR_U)  ? "+"   : " ");
				strcat(buf, HAS_BIT(room->exit_dirs, MAP_DIR_NE) ? "/ "  : "  ");
				break;

			case 2:
				if (HAS_BIT(room->flags, ROOM_FLAG_VOID) || !HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS))
				{
					strcat(buf, HAS_BIT(room->exit_dirs, MAP_DIR_W) ? "-" : " ");
				}

				if (room->vnum == ses->map->in_room)
				{
					if (!HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS))
					{
						cat_sprintf(buf, "%s[%s#%s]%s", *room->color ? room->color : ses->map->room_color, ses->map->here_color, *room->color ? room->color : ses->map->room_color, ses->map->exit_color);
					}
					else
					{
						cat_sprintf(buf, "%s%05d%s", ses->map->here_color, room->vnum, ses->map->exit_color);
					}
				}
				else
				{
					if (HAS_BIT(room->flags, ROOM_FLAG_VOID))
					{
						switch (room->exit_dirs)
						{
							case MAP_DIR_N|MAP_DIR_S:
								cat_sprintf(buf, " | ");
								break;
							case MAP_DIR_E|MAP_DIR_W:
								cat_sprintf(buf, "---");
								break;
							case MAP_DIR_NE|MAP_DIR_SW:
								cat_sprintf(buf, " / ");
								break;
							case MAP_DIR_NW|MAP_DIR_SE:
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
							cat_sprintf(buf, "%s%05d%s", HAS_BIT(room->flags, ROOM_FLAG_PATH) && room->search_stamp == ses->map->search->stamp ? ses->map->path_color : *room->color ? room->color : ses->map->room_color, room->vnum, ses->map->exit_color);
						}
						else if (strip_color_strlen(ses, room->symbol) <= 1)
						{
							cat_sprintf(buf, "%s[%-1s%s]%s", HAS_BIT(room->flags, ROOM_FLAG_PATH) && room->search_stamp == ses->map->search->stamp ? ses->map->path_color : *room->color ? room->color : ses->map->room_color, room->symbol, HAS_BIT(room->flags, ROOM_FLAG_PATH) && room->search_stamp == ses->map->search->stamp ? ses->map->path_color : *room->color ? room->color : ses->map->room_color, ses->map->exit_color);
						}
						else
						{
							cat_sprintf(buf, "%s%-3s%s", HAS_BIT(room->flags, ROOM_FLAG_PATH) && room->search_stamp == ses->map->search->stamp ? ses->map->path_color : *room->color ? room->color : ses->map->room_color, room->symbol, ses->map->exit_color);
						}
					}
				}

				if (HAS_BIT(room->flags, ROOM_FLAG_VOID) || !HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIVNUMS))
				{
					strcat(buf, HAS_BIT(room->exit_dirs, MAP_DIR_E) ? "--" : "  ");
				}
				else
				{
					strcat(buf, HAS_BIT(room->exit_dirs, MAP_DIR_E) ? "-" : " ");
				}
				break;

			case 3:
				strcat(buf, HAS_BIT(room->exit_dirs, MAP_DIR_SW) ? "/"   : " ");
				strcat(buf, HAS_BIT(room->exit_dirs, MAP_DIR_D)  ? "-"   : " ");
				strcat(buf, HAS_BIT(room->exit_dirs, MAP_DIR_S)  ? "| "  : "  ");
				strcat(buf, HAS_BIT(room->exit_dirs, MAP_DIR_SE) ? "\\ " : "  ");
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
			sprintf(buf, "%s%c%c", ses->map->here_color, 199, 207);
		}
		else if (HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS))
		{
			sprintf(buf, "%s\033[12m%s\033[10m", ses->map->here_color, ses->map->legend[16]);
		}
		else
		{
			sprintf(buf, "%s%s", ses->map->here_color, ses->map->legend[16]);
		}
		pop_call();
		return buf;
	}

	exit1 = 0;
	exit2 = 0;
	exits = 0;

	if (HAS_BIT(room->exit_dirs, MAP_DIR_N))
	{
		SET_BIT(exit1, 1 << 0);
		SET_BIT(exit2, 1 << 0);
		SET_BIT(exits, MAP_EXIT_N);
	}

	if (HAS_BIT(room->exit_dirs, MAP_DIR_W))
	{
		SET_BIT(exit1, 1 << 2);
		SET_BIT(exits, MAP_EXIT_W);
	}

	if (HAS_BIT(room->exit_dirs, MAP_DIR_E))
	{
		SET_BIT(exit2, 1 << 2);
		SET_BIT(exits, MAP_EXIT_E);
	}

	if (HAS_BIT(room->exit_dirs, MAP_DIR_S))
	{
		SET_BIT(exit1, 1 << 4);
		SET_BIT(exit2, 1 << 4);
		SET_BIT(exits, MAP_EXIT_S);
	}

	if (HAS_BIT(ses->map->flags, MAP_FLAG_MUDFONT))
	{
		if (HAS_BIT(room->exit_dirs, MAP_DIR_NW))
		{
			SET_BIT(exit1, 1 << 1);
			SET_BIT(exits, MAP_EXIT_NW);
		}
		if (HAS_BIT(room->exit_dirs, MAP_DIR_NE))
		{
			SET_BIT(exit2, 1 << 1);
			SET_BIT(exits, MAP_EXIT_NE);
		}
		if (HAS_BIT(room->exit_dirs, MAP_DIR_SW))
		{
			SET_BIT(exit1, 1 << 3);
			SET_BIT(exits, MAP_EXIT_SW);
		}
		if (HAS_BIT(room->exit_dirs, MAP_DIR_SE))
		{
			SET_BIT(exit2, 1 << 3);
			SET_BIT(exits, MAP_EXIT_SE);
		}

		room1 = exit1 + 128;
		room2 = exit2 + 160;

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
		if (HAS_BIT(ses->map->flags, MAP_FLAG_SYMBOLGRAPHICS) && room->symbol[0] && room->symbol[0] != ' ')
		{
			sprintf(buf, "%s%-1s", *room->color ? room->color : ses->map->room_color, room->symbol);
		}
		else
		{
			sprintf(buf, "%s\033[12m%s\033[10m", *room->color ? room->color : ses->map->room_color, ses->map->legend[exits]);
		}
	}
	else
	{
		if (HAS_BIT(ses->map->flags, MAP_FLAG_SYMBOLGRAPHICS) && room->symbol[0] && room->symbol[0] != ' ')
		{
			sprintf(buf, "%s%-1s", *room->color ? room->color : ses->map->room_color, room->symbol);
		}
		else
		{
			sprintf(buf, "%s%s", *room->color ? room->color : ses->map->room_color, ses->map->legend[exits]);
		}
	}
	pop_call();
	return buf;
}

void search_keywords(struct session *ses, char *arg, char *out, char *var)
{
	char buf[MAP_SEARCH_MAX][BUFFER_SIZE], tmp[BUFFER_SIZE];
	int type, max;

	push_call("search_keywords(%p,%p,%p,%p,%p)");

	for (type = 0 ; type < MAP_SEARCH_MAX ; type++)
	{
		buf[type][0] = 0;
	}

	var[0] = 0;

	type = 0;

	while (*arg && type < MAP_SEARCH_MAX)
	{
		arg = sub_arg_in_braces(ses, arg, tmp, GET_ALL, SUB_VAR|SUB_FUN);

		if (!strcasecmp(tmp, "roomname"))
		{
			arg = sub_arg_in_braces(ses, arg, buf[MAP_SEARCH_NAME], GET_ALL, SUB_VAR|SUB_FUN);
		}
		else if (!strcasecmp(tmp, "roomexits"))
		{
			arg = sub_arg_in_braces(ses, arg, buf[MAP_SEARCH_EXITS], GET_ALL, SUB_VAR|SUB_FUN);
		}
		else if (!strcasecmp(tmp, "roomdesc"))
		{
			arg = sub_arg_in_braces(ses, arg, buf[MAP_SEARCH_DESC], GET_ALL, SUB_VAR|SUB_FUN);
		}
		else if (!strcasecmp(tmp, "roomarea"))
		{
			arg = sub_arg_in_braces(ses, arg, buf[MAP_SEARCH_AREA], GET_ALL, SUB_VAR|SUB_FUN);
		}
		else if (!strcasecmp(tmp, "roomnote"))
		{
			arg = sub_arg_in_braces(ses, arg, buf[MAP_SEARCH_NOTE], GET_ALL, SUB_VAR|SUB_FUN);
		}
		else if (!strcasecmp(tmp, "roomterrain"))
		{
			arg = sub_arg_in_braces(ses, arg, buf[MAP_SEARCH_TERRAIN], GET_ALL, SUB_VAR|SUB_FUN);
		}
		else if (!strcasecmp(tmp, "roomflag"))
		{
			arg = sub_arg_in_braces(ses, arg, buf[MAP_SEARCH_FLAG], GET_ALL, SUB_VAR|SUB_FUN);
		}
		else if (!strcasecmp(tmp, "variable"))
		{
			arg = sub_arg_in_braces(ses, arg, var, GET_ALL, SUB_VAR|SUB_FUN);
		}
		else
		{
			strcpy(buf[type++], tmp);
		}
	}

	for (max = MAP_SEARCH_MAX - 1 ; max >= 0 ; max--)
	{
		if (*buf[max])
		{
			break;
		}
	}

	out[0] = 0;

	for (type = 0 ; type <= max ; type++)
	{
		cat_sprintf(out, "{%s}", buf[type]);
	}
	pop_call();
	return;
}

void map_search_compile(struct session *ses, char *arg, char *var)
{
	char tmp[BUFFER_SIZE], buf[BUFFER_SIZE];

	push_call("map_search_compile(%p,%p,%p)",ses,arg,var);

	search_keywords(ses, arg, tmp, var);

	arg = sub_arg_in_braces(ses, tmp, buf, GET_ALL, SUB_VAR|SUB_FUN); // name

	ses->map->search->vnum = (int) tintoi(buf);

	if (ses->map->search->vnum)
	{
		pop_call();
		return;
	}

	if (ses->map->search->name)
	{
		free(ses->map->search->name);
	}

	if (*buf)
	{
		strcat(buf, "$");

		ses->map->search->name = tintin_regexp_compile(ses, NULL, buf, PCRE_ANCHORED);
	}
	else
	{
		ses->map->search->name = NULL;
	}

	arg = sub_arg_in_braces(ses, arg, buf, GET_ALL, SUB_VAR|SUB_FUN); // exits

	ses->map->search->exit_dirs = 0;
	ses->map->search->exit_size = 0;

	if (ses->map->search->exit_list)
	{
		free(ses->map->search->exit_list);
	}

	if (*buf)
	{
		struct listnode *node;
		char exit[BUFFER_SIZE];
		char *str = buf;

		tmp[0] = 0;

		while (*str)
		{
			str = get_arg_in_braces(ses, str, exit, GET_ONE);

			node = search_node_list(ses->list[LIST_PATHDIR], exit);

			ses->map->search->exit_size++;

			if (node)
			{
				SET_BIT(ses->map->search->exit_dirs, 1LL << atoi(node->pr));
			}
			else
			{
				SET_BIT(ses->map->search->exit_dirs, 1); // flag indicates no exits

				cat_sprintf(tmp, "{%s}", exit);
			}

			if (*str == COMMAND_SEPARATOR)
			{
				str++;
			}
		}
		ses->map->search->exit_list = strdup(tmp);
	}
	else
	{
		ses->map->search->exit_list = strdup("");
	}

	arg = sub_arg_in_braces(ses, arg, buf, GET_ALL, SUB_VAR|SUB_FUN); // desc

	if (ses->map->search->desc)
	{
		free(ses->map->search->desc);
	}

	if (*buf)
	{
		ses->map->search->desc = tintin_regexp_compile(ses, NULL, buf, PCRE_ANCHORED);
	}
	else
	{
		ses->map->search->desc = NULL;
	}

	arg = sub_arg_in_braces(ses, arg, buf, GET_ALL, SUB_VAR|SUB_FUN); // area

	if (ses->map->search->area)
	{
		free(ses->map->search->area);
	}

	if (*buf)
	{
		ses->map->search->area = tintin_regexp_compile(ses, NULL, buf, PCRE_ANCHORED);
	}
	else
	{
		ses->map->search->area = NULL;
	}

	arg = sub_arg_in_braces(ses, arg, buf, GET_ALL, SUB_VAR|SUB_FUN); // note

	if (ses->map->search->note)
	{
		free(ses->map->search->note);
	}

	if (*buf)
	{
		ses->map->search->note = tintin_regexp_compile(ses, NULL, buf, PCRE_ANCHORED);
	}
	else
	{
		ses->map->search->note = NULL;
	}

	arg = sub_arg_in_braces(ses, arg, buf, GET_ALL, SUB_VAR|SUB_FUN); // terrain

	if (ses->map->search->terrain)
	{
		free(ses->map->search->terrain);
	}

	if (*buf)
	{
		ses->map->search->terrain = tintin_regexp_compile(ses, NULL, buf, PCRE_ANCHORED);
	}
	else
	{
		ses->map->search->terrain = NULL;
	}

	arg = sub_arg_in_braces(ses, arg, buf, GET_ALL, SUB_VAR|SUB_FUN); // flag

	if (*buf)
	{
		ses->map->search->flag = tintoi(buf);

		if (is_abbrev(buf, "avoid"))
		{
			ses->map->search->flag = ROOM_FLAG_AVOID;
		}
		else if (is_abbrev(buf, "hide"))
		{
			ses->map->search->flag = ROOM_FLAG_HIDE;
		}
		else if (is_abbrev(buf, "leave"))
		{
			ses->map->search->flag = ROOM_FLAG_LEAVE;
		}
		else if (is_abbrev(buf, "void"))
		{
			ses->map->search->flag = ROOM_FLAG_VOID;
		}
		else if (is_abbrev(buf, "static"))
		{
			ses->map->search->flag = ROOM_FLAG_STATIC;
		}
		else if (is_abbrev(buf, "river"))
		{
			ses->map->search->flag = ROOM_FLAG_RIVER;
		}
	}
	else
	{
		ses->map->search->flag = 0;
	}

	pop_call();
	return;
}

int match_room(struct session *ses, int vnum, struct search_data *search)
{
	struct room_data *room = ses->map->room_list[vnum];

	if (room == NULL)
	{
		return 0;
	}

	if (search->name)
	{
		if (!regexp_compare(search->name, room->name, "", 0, 0))
		{
			return 0;
		}
	}

	if (search->exit_dirs)
	{
		char *arg, exit[BUFFER_SIZE];

		if (search->exit_dirs != room->exit_dirs)
		{
			return 0;
		}
		if (search->exit_size != room->exit_size)
		{
			return 0;
		}

		arg = search->exit_list;

		while (*arg)
		{
			arg = get_arg_in_braces(ses, arg, exit, GET_ONE);

			if (!find_exit(ses, vnum, exit))
			{
				return 0;
			}

			if (*arg == COMMAND_SEPARATOR)
			{
				arg++;
			}
		}
	}

	if (search->desc)
	{
		if (!regexp_compare(search->desc, room->desc, "", 0, 0))
		{
			return 0;
		}
	}

	if (search->area)
	{
		if (!regexp_compare(search->area, room->area, "", 0, 0))
		{
			return 0;
		}
	}

	if (search->note)
	{
		if (!regexp_compare(search->note, room->note, "", 0, 0))
		{
			return 0;
		}
	}

	if (search->terrain)
	{
		if (!regexp_compare(search->terrain, room->terrain, "", 0, 0))
		{
			return 0;
		}
	}

	if (search->flag)
	{
		if ((room->flags & search->flag) != search->flag)
		{
			return 0;
		}
	}
	return 1;
}

int find_room(struct session *ses, char *arg)
{
	char var[BUFFER_SIZE];
	int room;

	push_call("find_room(%p,%s)",ses,arg);

	map_search_compile(ses, arg, var);

	if (ses->map->search->vnum > 0 && ses->map->search->vnum < ses->map->size)
	{
		if (ses->map->room_list[ses->map->search->vnum])
		{
			pop_call();
			return ses->map->search->vnum;
		}
		pop_call();
		return 0;
	}

	if (ses->map->in_room)
	{
		room = searchgrid_find(ses, ses->map->in_room, ses->map->search);

		if (room)
		{
			pop_call();
			return room;
		}
	}

	for (room = 0 ; room < ses->map->size ; room++)
	{
		if (ses->map->room_list[room] == NULL)
		{
			continue;
		}

		if (!match_room(ses, room, ses->map->search))
		{
			continue;
		}
		pop_call();
		return room;
	}
	pop_call();
	return 0;
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

int tunnel_void(struct session *ses, int from, int room, int dir)
{
	if (!HAS_BIT(ses->map->room_list[room]->flags, ROOM_FLAG_VOID))
	{
		return room;
	}

	if (get_room_exits(ses, room) != 2)
	{
		struct exit_data *exit;

		for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
		{
			if (exit->dir == dir)
			{
				return tunnel_void(ses, room, exit->vnum, exit->dir);
			}
		}
		return room;
	}

	if (ses->map->room_list[room]->f_exit->vnum != from)
	{
		return tunnel_void(ses, room, ses->map->room_list[room]->f_exit->vnum, ses->map->room_list[room]->f_exit->dir);
	}
	else
	{
		return tunnel_void(ses, room, ses->map->room_list[room]->l_exit->vnum, ses->map->room_list[room]->l_exit->dir);
	}
}

// shortest_path() utilities

int searchgrid_find(struct session *ses, int from, struct search_data *search)
{
	int vnum, head, tail, index;
	float length;
	struct breadth_node *node, *temp, list[MAP_BF_SIZE];
	struct exit_data *exit;
	struct room_data *room;

	search->stamp++;

	head = 0;
	tail = 1;

	node = &list[head];

	node->vnum   = from;
	node->length = ses->map->room_list[from]->weight;

	while (head != tail)
	{
		node = &list[head];

		room = ses->map->room_list[node->vnum];

		length = node->length;

		head = (head + 1) % MAP_BF_SIZE;

		if (search->stamp != room->search_stamp)
		{
			room->search_stamp = search->stamp;

			if (HAS_BIT(room->flags, ROOM_FLAG_PATH))
			{
				DEL_BIT(room->flags, ROOM_FLAG_PATH);
			}
		}
		else if (length >= room->length)
		{
			continue;
		}

		room->length = length;

		if (search->vnum)
		{
			if (room->vnum == search->vnum)
			{
				return room->vnum;
			}
		}
		else
		{
			if (match_room(ses, room->vnum, search))
			{
				return room->vnum;
			}
		}

		for (exit = room->f_exit ; exit ; exit = exit->next)
		{
			vnum = tunnel_void(ses, room->vnum, exit->vnum, exit->dir);

			if (HAS_BIT(exit->flags, EXIT_FLAG_AVOID) || HAS_BIT(ses->map->room_list[vnum]->flags, ROOM_FLAG_AVOID))
			{
				continue;
			}

			length = room->length + ses->map->room_list[vnum]->weight;

			if (search->stamp == ses->map->room_list[vnum]->search_stamp)
			{
				if (length >= ses->map->room_list[vnum]->length)
				{
					continue;
				}
			}

			temp = &list[tail];

			temp->vnum   = vnum;
			temp->length = length;

			/*
				list must remain ordered by length
			*/

			index = tail;

			while (index != head)
			{
				temp = &list[index];

				node = &list[index ? index - 1 : MAP_BF_SIZE - 1];

				if (temp->length >= node->length)
				{
					break;
				}

				vnum         = temp->vnum;
				length       = temp->length;

				temp->vnum   = node->vnum;
				temp->length = node->length;

				node->vnum   = vnum;
				node->length = length;

				index = index ? index - 1 : MAP_BF_SIZE - 1;
			}

			tail = (tail + 1) % MAP_BF_SIZE;

			if (tail == head)
			{
				show_error(ses, LIST_COMMAND, "#SHORTEST PATH: MAP TOO BIG FOR BF STACK OF %d", MAP_BF_SIZE);
				break;
			}
		}
	}
	return 0;
}

int searchgrid_walk(struct session *ses, int offset, int from, int dest)
{
	int vnum, trim, head, tail, index;
	float length;
	struct breadth_node *node, *temp, list[MAP_BF_SIZE];
	struct exit_data *exit;
	struct room_data *room;

	head = 0;
	tail = 1;

	node = &list[head];

	node->vnum   = from;
	node->length = ses->map->room_list[from]->weight;

	while (head != tail)
	{
		node = &list[head];

		room = ses->map->room_list[node->vnum];

		length = node->length;

		head = (head + 1) % MAP_BF_SIZE;

		if (length >= room->length)
		{
			continue;
		}

		room->length = length;

		if (room->vnum == dest)
		{
			return room->vnum;
		}

		trim = 1;

		for (exit = room->f_exit ; exit ; exit = exit->next)
		{
			vnum = tunnel_void(ses, room->vnum, exit->vnum, exit->dir);

			if (HAS_BIT(exit->flags, EXIT_FLAG_AVOID) || HAS_BIT(ses->map->room_list[vnum]->flags, ROOM_FLAG_AVOID))
			{
				continue;
			}

			length = room->length + ses->map->room_list[vnum]->weight;

			if (ses->map->search->stamp != ses->map->room_list[vnum]->search_stamp)
			{
				continue;
			}

			if (length >= ses->map->room_list[vnum]->length || length >= ses->map->room_list[dest]->length)
			{
				continue;
			}

			temp = &list[tail];

			temp->vnum   = vnum;
			temp->length = length;

			/*
				list must remain ordered by length
			*/

			index = tail;

			while (index != head)
			{
				temp = &list[index];

				node = &list[index ? index - 1 : MAP_BF_SIZE - 1];

				if (temp->length >= node->length)
				{
					break;
				}

				vnum = temp->vnum;
				length = temp->length;

				temp->vnum = node->vnum;
				temp->length = node->length;

				node->vnum = vnum;
				node->length = length;

				index = index ? index - 1 : MAP_BF_SIZE - 1;
			}

			tail = (tail + 1) % MAP_BF_SIZE;

			if (tail == head)
			{
				show_error(ses, LIST_COMMAND, "#SHORTEST PATH: MAP TOO BIG FOR BF STACK OF %d", MAP_BF_SIZE);
				break;
			}
			trim = 0;
		}

		if (trim)
		{
			room->length = 0;
		}
	}
	return 0;
}

void shortest_path(struct session *ses, int run, char *delay, char *arg)
{
	char var[BUFFER_SIZE];
	struct exit_data *exit;
	struct room_data *room;
	int vnum, dest;

	if (HAS_BIT(ses->flags, SES_FLAG_MAPPING))
	{
		show_error(ses, LIST_COMMAND, "#SHORTEST PATH: You have to use #PATH END first.");

		return;
	}

	kill_list(ses->list[LIST_PATH]);

	map_search_compile(ses, arg, var);

	dest = searchgrid_find(ses, ses->map->in_room, ses->map->search);

	if (dest == 0)
	{
		show_error(ses, LIST_COMMAND, "#SHORTEST PATH: NO PATH FOUND TO %s.", arg);
		return;
	}

	if (dest == ses->map->in_room)
	{
		show_error(ses, LIST_COMMAND, "#SHORTEST PATH: Already there.");
		return;
	}

	vnum = ses->map->in_room;

	// Slower than a backtrace, but works with mazes.

	while (TRUE)
	{
		room = ses->map->room_list[vnum];

		for (exit = room->f_exit ; exit ; exit = exit->next)
		{
			if (HAS_BIT(exit->flags, EXIT_FLAG_AVOID) || HAS_BIT(ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_AVOID))
			{
				continue;
			}

			vnum = tunnel_void(ses, room->vnum, exit->vnum, exit->dir);

			if (searchgrid_walk(ses, room->length, vnum, dest))
			{
				break;
			}
		}

		if (exit == NULL)
		{
			show_error(ses, LIST_COMMAND, "#SHORTEST PATH: UNKNOWN ERROR.");
			return;
		}

		if (HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW))
		{
			insert_node_list(ses->list[LIST_PATH], exit->cmd, "", "0");
		}
		else
		{
			insert_node_list(ses->list[LIST_PATH], exit->name, "", "0");
		}

		SET_BIT(ses->map->room_list[vnum]->flags, ROOM_FLAG_PATH);

		if (ses->map->room_list[vnum]->search_stamp != ses->map->search->stamp)
		{
			show_error(ses, LIST_COMMAND, "%d bad search stamp %d vs %d", vnum, ses->map->room_list[vnum]->search_stamp, ses->map->search->stamp);
		}

		if (vnum == dest)
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
	Virtual coordinate search for linkable rooms when creating a new room.
*/

int find_coord(struct session *ses, char *arg)
{
	int dir, x, y, z, room;

	dir = get_exit_dir(ses, arg);

	if (dir == 0)
	{
		return 0;
	}

	x = (HAS_BIT(dir, MAP_EXIT_E) ? 1 : HAS_BIT(dir, MAP_EXIT_W) ? -1 : 0);
	y = (HAS_BIT(dir, MAP_EXIT_N) ? 1 : HAS_BIT(dir, MAP_EXIT_S) ? -1 : 0);
	z = (HAS_BIT(dir, MAP_EXIT_U) ? 1 : HAS_BIT(dir, MAP_EXIT_D) ? -1 : 0);

	room = spatialgrid_find(ses, ses->map->in_room, x, y, z);

	if (ses->map->room_list[room])
	{
		if (HAS_BIT(ses->map->room_list[room]->flags, ROOM_FLAG_STATIC))
		{
			show_message(ses, LIST_COMMAND, "#MAP: Linkable room is marked static. Creating overlapping room instead.");

			return 0;
		}
	}
	return room;
}

// Used by #map jump and the auto linker

int spatialgrid_find(struct session *ses, int from, int x, int y, int z)
{
	int head, tail;
	struct grid_node *node, *temp, list[MAP_BF_SIZE];
	struct exit_data *exit;
	struct room_data *room;
	int xx, yy, zz;

	push_call("spatialgrid_find(%s,%d,%d,%d,%d)",ses->name,from,x,y,z);

	head = 0;
	tail = 1;

	node = &list[head];

	node->vnum   = from;
	node->x      = 0;
	node->y      = 0;
	node->z      = 0;
	node->length = 0;

	ses->map->display_stamp++;

	while (head != tail)
	{
		node = &list[head];

		head = (head + 1) % MAP_BF_SIZE;

		room = ses->map->room_list[node->vnum];

		if (ses->map->display_stamp != room->display_stamp)
		{
			room->display_stamp = ses->map->display_stamp;
		}
		else if (room->length <= node->length)
		{
			continue;
		}

		room->length = node->length;

		if (node->x == x && node->y == y && node->z == z)
		{
			pop_call();
			return node->vnum;
		}

		for (exit = room->f_exit ; exit ; exit = exit->next)
		{
			if (ses->map->display_stamp == ses->map->room_list[exit->vnum]->display_stamp)
			{
				if (room->length >= ses->map->room_list[exit->vnum]->length)
				{
					continue;
				}
			}

			if (exit->dir == 0)
			{
				continue;
			}

			if (HAS_BIT(exit->flags, EXIT_FLAG_HIDE) || HAS_BIT(ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_HIDE))
			{
				continue;
			}

			if (head == (tail + 1) % MAP_BF_SIZE)
			{
				break;
			}

			xx = node->x + (HAS_BIT(exit->dir, MAP_EXIT_E) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_W) ? -1 : 0);
			yy = node->y + (HAS_BIT(exit->dir, MAP_EXIT_N) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_S) ? -1 : 0);
			zz = node->z + (HAS_BIT(exit->dir, MAP_EXIT_U) ?  1 : HAS_BIT(exit->dir, MAP_EXIT_D) ? -1 : 0);

			temp = &list[tail];

			temp->vnum   = exit->vnum;
			temp->x      = xx;
			temp->y      = yy;
			temp->z      = zz;
			temp->length = node->length + 1;

			tail = (tail + 1) % MAP_BF_SIZE;
		}
	}
	pop_call();
	return 0;
}

void explore_path(struct session *ses, int run, char *arg1, char *arg2)
{
	struct exit_data *exit;
	int room, vnum;

	for (vnum = 0 ; vnum < ses->map->size ; vnum++)
	{
		if (ses->map->room_list[vnum])
		{
			DEL_BIT(ses->map->room_list[vnum]->flags, ROOM_FLAG_PATH);
		}
	}

	if (HAS_BIT(ses->flags, SES_FLAG_MAPPING))
	{
		show_error(ses, LIST_COMMAND, "#MAP EXPLORE: You have to use #PATH END first.");

		return;
	}

	kill_list(ses->list[LIST_PATH]);

	room = ses->map->in_room;

	exit = find_exit(ses, room, arg1);

	if (exit == NULL)
	{
		show_error(ses, LIST_COMMAND, "#MAP: There's no exit named '%s'.", arg1);
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

	SET_BIT(ses->map->room_list[room]->flags, ROOM_FLAG_PATH);
	SET_BIT(ses->map->room_list[vnum]->flags, ROOM_FLAG_PATH);

	while (get_room_exits(ses, vnum) == 2)
	{
		exit = ses->map->room_list[vnum]->f_exit;

		if (HAS_BIT(ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_PATH))
		{
			exit = ses->map->room_list[vnum]->l_exit;

			if (HAS_BIT(ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_PATH))
			{
				break;
			}
		}

		if (!HAS_BIT(ses->map->room_list[vnum]->flags, ROOM_FLAG_VOID))
		{
			if (HAS_BIT(ses->map->flags, MAP_FLAG_NOFOLLOW))
			{
				insert_node_list(ses->list[LIST_PATH], exit->cmd, "", "0");
			}
			else
			{
				insert_node_list(ses->list[LIST_PATH], exit->name, "", "0");
			}
		}

		vnum = exit->vnum;

		SET_BIT(ses->map->room_list[vnum]->flags, ROOM_FLAG_PATH);
	}

	DEL_BIT(ses->map->room_list[room]->flags, ROOM_FLAG_PATH);

	if (run)
	{
		path_run(ses, arg2);
	}
}
