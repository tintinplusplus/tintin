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

void show_map(struct session *ses, char *argument);
void create_legenda(struct session *ses, char *arg);

short               map_grid_x;
short               map_grid_y;
short               map_grid_z;

struct room_data  *	map_grid[201][101];

struct exit_data  * map_search_exit_best;
struct exit_data  * map_search_exit_node;
struct session    * map_search_ses;
short               map_search_max_size;
short               map_search_tar_room;

/*
	MUD font

	#map legenda {128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143 161}
	#map legenda {144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159 161}
*/

unsigned char	map_palet0[]	= { 42, 35, 35, 43, 35,124, 43, 43, 35, 43, 45, 43, 43, 43, 43, 43, 120};

unsigned char	map_palet1[]	= {126,247,247,109,247,120,108,116,247,106,113,118,107,117,119,110, 88};

#define CHECK_MAP(void)                                                                                            \
{                                                                                                                  \
	if (ses->map == NULL)                                                                                         \
	{                                                                                                             \
		tintin_printf2(ses, "#MAP: This session has no map data. Use #map create or #map load to create on.");   \
                                                                                                                   \
		return;                                                                                                  \
	}                                                                                                             \
}

#define CHECK_INSIDE(void)                                                                                         \
{                                                                                                                  \
	if (ses->map == NULL)                                                                                         \
	{                                                                                                             \
		tintin_printf2(ses, "#MAP: This session has no map data. Use #map create or #map load to create on.");   \
                                                                                                                   \
		return;                                                                                                  \
	}                                                                                                             \
	if (ses->map->room_list[ses->map->in_room] == NULL)                                                           \
	{                                                                                                             \
		tintin_printf2(ses, "#MAP: You are not inside the map. Use #map goto first to enter it.");               \
                                                                                                                   \
		return;                                                                                                  \
	}                                                                                                             \
}

DO_COMMAND(do_map)
{
	int cnt;
	char cmd[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, cmd, FALSE);

	if (*cmd == 0)
	{
		tintin_printf2(ses, "Available map options");
		tintin_printf2(ses, "");
		tintin_printf2(ses, "#map color    <color code>             (set the room's color for map)");
		tintin_printf2(ses, "#map create                            (creates the initial map)");
		tintin_printf2(ses, "#map destroy                           (destroys the map)");
		tintin_printf2(ses, "#map dig      <direction>              (creates a new room)");
		tintin_printf2(ses, "#map exit     <direction> <command>    (sets the exit command)");
		tintin_printf2(ses, "#map info                              (info on map and current room)");
		tintin_printf2(ses, "#map find     <room name>              (saves path to #path)");
		tintin_printf2(ses, "#map flag     <map flag>               (set map wide flags)");
		tintin_printf2(ses, "#map goto     <room name>              (moves you to given room)");
		tintin_printf2(ses, "#map leave                             (leave the map, return with goto)");
		tintin_printf2(ses, "#map legenda  <symbols>                (sets the map legenda)");
		tintin_printf2(ses, "#map link     <direction>  <room name> (links 2 rooms)");
		tintin_printf2(ses, "#map list                              (shows list of created rooms)");
		tintin_printf2(ses, "#map map      <radius>                 (shows an ascii map)");
		tintin_printf2(ses, "#map name     <room name>              (set the room's name)");
		tintin_printf2(ses, "#map read     <filename>               (load your map from a file)");
		tintin_printf2(ses, "#map roomflag <room flag>              (set room based flags)");
		tintin_printf2(ses, "#map set      <option>     <value>     (set various values)");
		tintin_printf2(ses, "#map undo                              (undo last move)");
		tintin_printf2(ses, "#map unlink   <direction>              (deletes an exit)");
		tintin_printf2(ses, "#map walk     <room name>              (walk to given room)");
		tintin_printf2(ses, "#map write    <filename>               (save the map to a file)");

		return ses;
	}
	else
	{
		for (cnt = 0 ; *map_table[cnt].name ; cnt++)
		{
			if (is_abbrev(cmd, map_table[cnt].name))
			{
				map_table[cnt].map (ses, arg);
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
	char left[BUFFER_SIZE];

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, FALSE);

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
	char left[BUFFER_SIZE];
	struct exit_data *exit;

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, FALSE);

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
	char left[BUFFER_SIZE];

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, TRUE);

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

DO_MAP(map_exit)
{
	char temp[BUFFER_SIZE];
	struct exit_data *exit;
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

	exit = find_exit(ses, ses->map->in_room, left);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: Exit {%s} not found.", left);

		return;
	}

	sprintf(temp, "{%d} {%s} {%s}", exit->vnum, left, right);

	delete_exit(ses, ses->map->in_room, exit);

	create_exit(ses, ses->map->in_room, temp);

	tintin_printf2(ses, "#MAP: Exit {%s} set to {%s}.", left, right);
}

DO_MAP(map_find)
{
	char left[BUFFER_SIZE];

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, TRUE);

	shortest_path(ses, FALSE, left, "");
}

DO_MAP(map_flag)
{
	char left[BUFFER_SIZE];

	CHECK_MAP();

	arg = get_arg_in_braces(arg, left, TRUE);

	if (*left)
	{
		if (is_abbrev(left, "static"))
		{
			TOG_BIT(ses->map->flags, MAP_FLAG_STATIC);

			tintin_printf2(ses, "#MAP: Static flag toggled %s.", HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) ? "on" : "off");
		}

		if (is_abbrev(left, "vtmap"))
		{
			TOG_BIT(ses->map->flags, MAP_FLAG_VTMAP);

			tintin_printf2(ses, "#MAP: VTmap flag toggled %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP) ? "on" : "off");

			show_vtmap(ses);
		}

		if (is_abbrev(left, "vtgraphics"))
		{
			TOG_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS);

			tintin_printf2(ses, "#MAP: VTgraphics flag toggled %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS) ? "on" : "off");
		}

		if (is_abbrev(left, "asciigraphics"))
		{
			TOG_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS);

			tintin_printf2(ses, "#MAP: AsciiGraphics flag toggled %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS) ? "on" : "off");
		}
	}
	else
	{
		tintin_printf2(ses, "#MAP: Static flag toggled %s.", HAS_BIT(ses->map->flags, MAP_FLAG_STATIC) ? "on" : "off");
		tintin_printf2(ses, "#MAP: VTmap flag toggled %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP) ? "on" : "off");
		tintin_printf2(ses, "#MAP: VTgraphics flag toggled %s.", HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS) ? "on" : "off");
		tintin_printf2(ses, "#MAP: AsciiGraphics flag toggled %s.", HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS) ? "on" : "off");
	}
}

DO_MAP(map_get)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

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
			sprintf(left, "{%s} {%s}", right, ses->map->room_list[ses->map->in_room]->color);

			do_internal_variable(ses, left);
		}
		else if (is_abbrev(left, "roomflags"))
		{
			sprintf(left, "{%s} {%d}", right, ses->map->room_list[ses->map->in_room]->flags);

			do_internal_variable(ses, left);			
		}
		else if (is_abbrev(left, "roomname"))
		{
			sprintf(left, "{%s} {%s}", right, ses->map->room_list[ses->map->in_room]->name);

			do_internal_variable(ses, left);
		}
		else if (is_abbrev(left, "roomsymbol"))
		{
			sprintf(left, "{%s} {%s}", right, ses->map->room_list[ses->map->in_room]->symbol);

			do_internal_variable(ses, left);
		}
		else if (is_abbrev(left, "roomvnum"))
		{
			sprintf(left, "{%s} {%d}", right, ses->map->room_list[ses->map->in_room]->vnum);

			do_internal_variable(ses, left);
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
	char left[BUFFER_SIZE];

	CHECK_MAP();

	arg = get_arg_in_braces(arg, left, TRUE);

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

			for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
			{
				exits++;
			}
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

		for (exit = ses->map->room_list[ses->map->in_room]->f_exit ; exit ; exit = exit->next)
		{
			tintin_printf2(ses, "%-20s %-3s (%s) to room: %-5d (%s).", "Exit:", exit->name, exit->cmd, exit->vnum, ses->map->room_list[exit->vnum]->name);
		}
	}
}

DO_MAP(map_leave)
{
	CHECK_INSIDE();

	ses->map->in_room = 0;
	tintin_printf2(ses, "#MAP: Leaving the map. Use goto to return.");
}

DO_MAP(map_legenda)
{
	char left[BUFFER_SIZE];

	CHECK_MAP();

	arg = get_arg_in_braces(arg, left, TRUE);

	if (*left == 0)
	{
		tintin_printf2(ses, "#MAP: Current legenda: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
			ses->map->legenda[0],  ses->map->legenda[1],  ses->map->legenda[2],
			ses->map->legenda[3],  ses->map->legenda[4],  ses->map->legenda[5],
			ses->map->legenda[6],  ses->map->legenda[7],  ses->map->legenda[8],
			ses->map->legenda[9],  ses->map->legenda[10], ses->map->legenda[11],
			ses->map->legenda[12], ses->map->legenda[13], ses->map->legenda[14],
			ses->map->legenda[15], ses->map->legenda[16]);
	}
	else
	{
		create_legenda(ses, left);
	}
}

DO_MAP(map_link)
{
	char temp[BUFFER_SIZE], left[BUFFER_SIZE], right[BUFFER_SIZE];
	struct listnode *node;
	struct exit_data *exit;
	int room;

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

	CHECK_INSIDE();

	if ((room = find_room(ses, right)) == -1)
	{
		tintin_printf2(ses, "#MAP: Couldn't find room {%s}", right);
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
			tintin_printf2(ses, "[%5d] %s", ses->map->room_list[room]->vnum, ses->map->room_list[room]->name);

			for (exit = ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
			{
				tintin_printf2(ses, "        [%5d] %s", exit->vnum, exit->cmd);
			}
		}
	}
}


DO_MAP(map_map)
{
	char left[BUFFER_SIZE];

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, TRUE);

	if (!strcasecmp("vt", left))
	{
		show_vtmap(ses);
	}
	else
	{
		show_map(ses, left);
	}
}

DO_MAP(map_move)
{
	char left[BUFFER_SIZE];

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, TRUE);

	follow_map(ses, left);
}

DO_MAP(map_name)
{
	char left[BUFFER_SIZE];

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, TRUE);

	free(ses->map->room_list[ses->map->in_room]->name);
	ses->map->room_list[ses->map->in_room]->name = strdup(left);
}

DO_MAP(map_read)
{
	FILE *myfile;
	char temp[BUFFER_SIZE], left[BUFFER_SIZE], *cptr;
	int room = 0;

	arg = get_arg_in_braces(arg, left, TRUE);

	if ((myfile = fopen(left, "r")) == NULL)
	{
		tintin_printf(ses, "#MAP: Map file {%s} not found.", left);

		return;
	}

	SET_BIT(gtd->flags, TINTIN_FLAG_QUIET);

	if (ses->map == NULL)
	{
		ses->map = calloc(1, sizeof(struct map_data));
	}

	while (fgets(temp, BUFFER_SIZE - 1, myfile))
	{
		for (cptr = temp ; *cptr && *cptr != '\n' ; cptr++);

		*cptr = 0;

		switch (temp[0])
		{
			case 'C':
				create_map(ses);
				break;

			case 'E':
				create_exit(ses, room, &temp[2]);
				break;

			case 'F':
				ses->map->flags = atoi(&temp[2]);
				break;

			case 'L':
				create_legenda(ses, &temp[2]);
				break;

			case 'R':
				room = create_room(ses, &temp[2]);
				break;

			case '#':
				ses = parse_input(ses, temp);
				break;

			case 0:
				break;

			default:
				DEL_BIT(gtd->flags, TINTIN_FLAG_QUIET);

				tintin_printf2(ses, "#MAP: Invalid file, aborting.");

				fclose(myfile);

				delete_map(ses);

				return;				
		}
	}

	DEL_BIT(gtd->flags, TINTIN_FLAG_QUIET);

	fclose(myfile);

	show_message(ses, LIST_MAP, "#MAP: Map file {%s} loaded.", left);
}

DO_MAP(map_roomflag)
{
	char left[BUFFER_SIZE];

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, TRUE);

	if (*left)
	{
		if (is_abbrev(left, "avoid"))
		{
			TOG_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_AVOID);

			tintin_printf2(ses, "#MAP: Avoid flag toggled to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_AVOID) ? "on" : "off");
		}
		else if (is_abbrev(left, "hide"))
		{
			TOG_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_HIDE);

			tintin_printf2(ses, "#MAP: Hide flag toggled to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_HIDE) ? "on" : "off");
		}
		else if (is_abbrev(left, "leave"))
		{
			TOG_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE);

			tintin_printf2(ses, "#MAP: Leave flag toggled to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE) ? "on" : "off");
		}
		else if (is_abbrev(left, "void"))
		{
			TOG_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID);

			tintin_printf2(ses, "#MAP: Void flag toggled to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID) ? "on" : "off");
		}
	}
	else
	{
			tintin_printf2(ses, "#MAP: Avoid flag toggled to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_AVOID) ? "on" : "off");
			tintin_printf2(ses, "#MAP: Hide flag toggled to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_HIDE) ? "on" : "off");
			tintin_printf2(ses, "#MAP: Leave flag toggled to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE) ? "on" : "off");		
			tintin_printf2(ses, "#MAP: Void flag toggled to %s.", HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID) ? "on" : "off");
	}
}

DO_MAP(map_set)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

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
			RESTRING(ses->map->room_list[ses->map->in_room]->color, right);

			show_message(ses, LIST_MAP, "#MAP SET: roomcolor set to: %s.", right);
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
	struct exit_data *exit;
	char left[BUFFER_SIZE];

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, TRUE);

	exit = find_exit(ses, ses->map->in_room, left);

	if (exit == NULL)
	{
		tintin_printf2(ses, "#MAP: No exit with that name found");

		return;
	}

	delete_exit(ses, ses->map->in_room, exit);

	tintin_printf2(ses, "#MAP: Exit deleted.");
}

DO_MAP(map_walk)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	CHECK_INSIDE();

	arg = get_arg_in_braces(arg, left, TRUE);
	arg = get_arg_in_braces(arg, right, TRUE);

	shortest_path(ses, TRUE, left, right);
}

DO_MAP(map_write)
{
	FILE *file;
	char left[BUFFER_SIZE], temp[BUFFER_SIZE];
	struct exit_data *exit;
	int vnum;

	CHECK_MAP();

	arg = get_arg_in_braces(arg, left, TRUE);

	if (*left == 0 || (file = fopen(left, "w")) == NULL)
	{
		tintin_printf(ses, "#ERROR: #WRITEMAP {%s} - COULDN'T OPEN FILE TO WRITE.", left);

		return;
	}

	sprintf(temp, "C %d\n\n", MAX_ROOM);
	fputs(temp, file);

	sprintf(temp, "F %d\n\n", ses->map->flags);
	fputs(temp, file);

	sprintf(temp, "L %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n\n",
		ses->map->legenda[0],  ses->map->legenda[1],  ses->map->legenda[2],
		ses->map->legenda[3],  ses->map->legenda[4],  ses->map->legenda[5],
		ses->map->legenda[6],  ses->map->legenda[7],  ses->map->legenda[8],
		ses->map->legenda[9],  ses->map->legenda[10], ses->map->legenda[11],
		ses->map->legenda[12], ses->map->legenda[13], ses->map->legenda[14],
		ses->map->legenda[15], ses->map->legenda[16]);
	fputs(temp, file);

	for (vnum = 0 ; vnum < MAX_ROOM ; vnum++)
	{
		if (ses->map->room_list[vnum])
		{
			sprintf(temp, "\nR {%5d} {%d} {%s} {%s} {%s}\n",
				ses->map->room_list[vnum]->vnum,
				ses->map->room_list[vnum]->flags,
				ses->map->room_list[vnum]->color,
				ses->map->room_list[vnum]->name,
				ses->map->room_list[vnum]->symbol);

			fputs(temp, file);

			for (exit = ses->map->room_list[vnum]->f_exit ; exit ; exit = exit->next)
			{
				sprintf(temp, "E {%5d} {%s} {%s}\n",
					exit->vnum,
					exit->name,
					exit->cmd);

				fputs(temp, file);
			}
		}
	}

	fclose(file);

	show_message(ses, -1, "#MAP: Map file written to {%s}.", left);
}

void create_map(struct session *ses)
{
	int index;

	if (ses->map)
	{
		delete_map(ses);
	}

	ses->map = calloc(1, sizeof(struct map_data));

	create_room(ses, "{1} {0} {<078>} {1} { }");

	for (index = 0 ; index < 17 ; index++)
	{
		ses->map->legenda[index] = map_palet0[index];
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
		
void create_legenda(struct session *ses, char *arg)
{
	int cnt;
	char buf[BUFFER_SIZE];

	for (cnt = 0 ; cnt < 17 ; cnt++)
	{
		arg = get_arg_in_braces(arg, buf,  FALSE);

		if (*buf)
		{
			ses->map->legenda[cnt] = atoi(buf);
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

	tintin_printf2(ses, "#READMAP R %5s {%s}", vnum, name);

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
	char vnum[BUFFER_SIZE], name[BUFFER_SIZE], cmd[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, vnum, FALSE);
	arg = get_arg_in_braces(arg, name, FALSE);
	arg = get_arg_in_braces(arg, cmd,  TRUE);

	newexit       = calloc(1, sizeof(struct exit_data));
	newexit->vnum = atoi(vnum);
	newexit->name = strdup(name);
	newexit->cmd  = strdup(cmd);

	LINK(newexit, ses->map->room_list[room]->f_exit, ses->map->room_list[room]->l_exit);

	tintin_printf2(ses, "#READMAP E %5s {%s} {%s}", vnum, name, cmd);
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


void follow_map(struct session *ses, char *argument)
{
	struct listnode *node;
	struct exit_data *exit;
	int room;

	exit = find_exit(ses, ses->map->in_room, argument);

	if (exit)
	{
		insert_undo(ses, "%d %d %d", exit->vnum, ses->map->in_room, 0);

		goto_room(ses, exit->vnum);

		if (HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_LEAVE))
		{
			show_message(ses, LIST_MAP, "#MAP: LEAVE FLAG FOUND IN ROOM {%d}. LEAVING MAP.", ses->map->in_room);

			map_leave(ses, argument);
		}

		if (HAS_BIT(ses->map->room_list[ses->map->in_room]->flags, ROOM_FLAG_VOID))
		{
			node = searchnode_list(ses->list[LIST_PATHDIR], argument);

			if (node)
			{
				for (exit = ses->map->room_list[ses->map->in_room]->f_exit ; exit ; exit = exit->next)
				{
					if (get_map_exit(ses, node->right) != get_map_exit(ses, exit->name))
					{
						follow_map(ses, exit->name);
	
						return;
					}
				}
			}
		}

		show_vtmap(ses);

		return;
	}

	if (!HAS_BIT(ses->map->flags, MAP_FLAG_STATIC))
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
	char buf[BUFFER_SIZE], dir[BUFFER_SIZE], rev[BUFFER_SIZE], flag[BUFFER_SIZE];
	char *arg;
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

		coord = get_map_exit(map_search_ses, exit->name);

		if (coord == 0)
		{
			continue;
		}

		
		xx = x + (HAS_BIT(coord, MAP_EXIT_E) ?  1 : HAS_BIT(coord, MAP_EXIT_W) ? -1 : 0);
		yy = y + (HAS_BIT(coord, MAP_EXIT_N) ?  1 : HAS_BIT(coord, MAP_EXIT_S) ? -1 : 0);
		zz = z + (HAS_BIT(coord, MAP_EXIT_U) ?  1 : HAS_BIT(coord, MAP_EXIT_D) ? -1 : 0);

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
			ses->map->room_list[vnum]->size = FALSE;
		}
	}

	memset(map_grid, 0, sizeof(map_grid));

	map_search_ses = ses;

	map_grid_y = y;
	map_grid_x = x;

	if (ses->map->room_list[room])
	{
		build_map_grid(room, x / 2, y / 2, 0);
	}
}

void show_map(struct session *ses, char *argument)
{
	char buf[BUFFER_SIZE], out[BUFFER_SIZE];
	int x, y, line, size;

	push_call("show_map(%p,%p)",ses,argument);

	size = atoi(argument);

	if (HAS_BIT(ses->map->flags, MAP_FLAG_ASCIIGRAPHICS))
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

				tintin_printf2(ses, "%s", out);
			}
		}
		pop_call();
		return;
	}
		
	for (y = map_grid_y ; y >= 0 ; y--)
	{
		strcpy(buf, "");

		for (x = 0 ; x <= map_grid_x ; x++)
		{
			strcat(buf, draw_room(ses, map_grid[x][y], 0));
		}

		substitute(ses, buf, out, SUB_COL);

		tintin_printf2(ses, "%s", out);
	}

	pop_call();
	return;
}


void show_vtmap(struct session *ses)
{
	char buf[BUFFER_SIZE], out[BUFFER_SIZE];
	int x, y, line;

	push_call("show_vtmap(%p,%p)",ses);

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
			door = get_map_exit(ses, exit->name);

			switch (door)
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
				strcat(buf, HAS_BIT(exits, MAP_EXIT_W) ? "-" : " ");
				if (room->vnum == ses->map->in_room)
				{
					cat_sprintf(buf, "%s[<118>#%s]<078>", room->color, room->color);
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
						cat_sprintf(buf, "%s[%s%s]<078>", room->color, room->symbol, room->color);
					}
				}
				strcat(buf, HAS_BIT(exits, MAP_EXIT_E) ? "--" : "  ");
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
			sprintf(buf, "<118>\033[12m%c\033[10m", ses->map->legenda[16]);
		}
		else
		{
			sprintf(buf, "<118>%c", ses->map->legenda[16]);
		}
		pop_call();
		return buf;
	}

	for (exit = room->f_exit ; exit ; exit = exit->next)
	{
		door = get_map_exit(ses, exit->name);

		if (door >= MAP_EXIT_N && door <= MAP_EXIT_W)
		{
			SET_BIT(exits, door);
		}
	}

	if (HAS_BIT(ses->map->flags, MAP_FLAG_VTGRAPHICS))
	{
		sprintf(buf, "%s\033[12m%c\033[10m", room->color, ses->map->legenda[exits]);
	}
	else
	{
		sprintf(buf, "%s%c", room->color, ses->map->legenda[exits]);
	}
	pop_call();
	return buf;
}


int find_room(struct session *ses, char *arg)
{
	int room;

	room = atoi(arg);

	if (room > 0 && room < MAX_ROOM)
	{
		if (ses->map->room_list[room])
		{
			return room;
		}
		return -1;
	}

	for (room = 0 ; room < MAX_ROOM ; room++)
	{
		if (ses->map->room_list[room] && regexp(ses->map->room_list[room]->name, arg, FALSE))
		{
			return room;
		}
	}
	return -1;
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

/*
	Fast, short distance node weighted algorithm - Igor
*/

void search_void(short from, short room)
{
	if (map_search_ses->map->room_list[room]->f_exit == NULL)
	{
		show_message(map_search_ses, LIST_MAP, "#SHORTEST PATH: ROOM {%d} IS AN INVALID VOID ROOM.", room);
		map_search_exit_best = NULL;

		return;
	}

	if (map_search_ses->map->room_list[room]->f_exit == map_search_ses->map->room_list[room]->l_exit)
	{
		show_message(map_search_ses, LIST_MAP, "#SHORTEST PATH: ROOM {%d} IS AN INVALID VOID ROOM.", room);
		map_search_exit_best = NULL;

		return;
	}

	if (map_search_ses->map->room_list[room]->f_exit->next != map_search_ses->map->room_list[room]->l_exit)
	{
		show_message(map_search_ses, LIST_MAP, "#SHORTEST PATH: ROOM {%d} IS AN INVALID VOID ROOM.", room);
		map_search_exit_best = NULL;

		return;
	}

	if (map_search_ses->map->room_list[room]->f_exit->vnum != from)
	{
		map_search_exit_best = map_search_ses->map->room_list[room]->f_exit;
	}
	else
	{
		map_search_exit_best = map_search_ses->map->room_list[room]->l_exit;
	}

	if (HAS_BIT(map_search_ses->map->room_list[map_search_exit_best->vnum]->flags, ROOM_FLAG_VOID))
	{
		search_void(room, map_search_exit_best->vnum);
	}
}

void search_path(short room, short size)
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

		if (!HAS_BIT(map_search_ses->map->room_list[room]->flags, ROOM_FLAG_VOID))
		{
			size++;
		}

		for (exit = map_search_ses->map->room_list[room]->f_exit ; exit ; exit = exit->next)
		{
			if (map_search_ses->map->room_list[exit->vnum] == NULL)
			{
				continue;
			}

			if (map_search_ses->map->room_list[exit->vnum]->size > size)
			{
				search_path(exit->vnum, size);
			}
		}
	}
}

void shortest_path(struct session *ses, int walk, char *left, char *right)
{
	char time[BUFFER_SIZE], delay[BUFFER_SIZE];
	struct exit_data *exit;
	int room, vnum;

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
	map_search_max_size  = 500;

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
			search_path(exit->vnum, 1);
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
				search_path(exit->vnum, 1);
			}
		}

		if (HAS_BIT(ses->map->room_list[map_search_exit_best->vnum]->flags, ROOM_FLAG_VOID))
		{
			search_void(room, map_search_exit_best->vnum);
		}

		if (map_search_exit_best == NULL)
		{
			return;
		}

		addnode_list(ses->list[LIST_PATH], map_search_exit_best->cmd, "", "0");

		if (map_search_exit_best->vnum == map_search_tar_room)
		{
			break;
		}

		room = map_search_exit_best->vnum;
	}

	if (walk)
	{
		if (*right)
		{
			strcpy(delay, "0");

			while (ses->list[LIST_PATH]->f_node)
			{
				sprintf(time, "%lld", utime());

				cat_sprintf(delay, " + %s", right);
				get_number_string(ses, delay, delay);

				updatenode_list(ses, time, ses->list[LIST_PATH]->f_node->left, delay, LIST_DELAY);

				deletenode_list(ses, ses->list[LIST_PATH]->f_node, LIST_PATH);
			}
		}
		else
		{
			while (ses->list[LIST_PATH]->f_node)
			{
				parse_input(ses, ses->list[LIST_PATH]->f_node->left);

				deletenode_list(ses, ses->list[LIST_PATH]->f_node, LIST_PATH);
			}
		}
	}
}

/*
	Do a coordinate based search
*/

int find_coord(struct session *ses, char *arg)
{
	int vnum, coord;

	map_grid_x = 0;
	map_grid_y = 0;
	map_grid_z = 0;

	map_search_ses       = ses;
	map_search_tar_room  = 0;

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
			ses->map->room_list[vnum]->size = 0;
		}
	}

	search_coord(ses->map->in_room, 0, 0, 0);

	return map_search_tar_room;
}

void search_coord(int vnum, short x, short y, short z)
{
	short xx, yy, zz, coord;

	map_search_ses->map->room_list[vnum]->size = 1;

	if (map_grid_x == x && map_grid_y == y && map_grid_z == z)
	{
		tintin_printf2(map_search_ses, "#MAP: Found linkable room: %d.", vnum);
		map_search_tar_room = vnum;
	}
	else
	{
		struct exit_data *exit;

		for (exit = map_search_ses->map->room_list[vnum]->f_exit ; exit ; exit = exit->next)
		{
			if (HAS_BIT(map_search_ses->map->room_list[exit->vnum]->flags, ROOM_FLAG_HIDE))
			{
				continue;
			}

			if (map_search_ses->map->room_list[exit->vnum] && map_search_ses->map->room_list[exit->vnum]->size == 0)
			{
				coord = get_map_exit(map_search_ses, exit->name);

				if (coord == 0)
				{
					continue;
				}

				yy = y + (HAS_BIT(coord, MAP_EXIT_N) ?  1 : HAS_BIT(coord, MAP_EXIT_S) ? -1 : 0);
				xx = x + (HAS_BIT(coord, MAP_EXIT_E) ?  1 : HAS_BIT(coord, MAP_EXIT_W) ? -1 : 0);
				zz = z + (HAS_BIT(coord, MAP_EXIT_U) ?  1 : HAS_BIT(coord, MAP_EXIT_D) ? -1 : 0);

				search_coord(exit->vnum, xx, yy, zz);
			}
		}
	}
}
