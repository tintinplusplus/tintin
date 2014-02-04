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
*   file: mapper.c - funtions related to auto mapping                         *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                     coded by Igor van den Hoven 2004                        *
******************************************************************************/

#include "tintin.h"

void show_map(struct session *ses, char *argument);
void show_vtmap(struct session *ses);

short               temp_map_x;
short               temp_map_y;
short               temp_map_z;

struct room_data  *	temp_map[201][101];

struct exit_data  * temp_exit_best;
struct exit_data  * temp_exit_node;
struct session    * temp_ses;
short               temp_max_size;
short               temp_tar_room;

unsigned char	temp_map_color[]	= {0,1,0,1,1,0,0,0,0,1,1,0,0,0,1,0,0,1,0,1,0,1,
							   7,5,3,2,7,6,4,4,4,6,3,1,2,3,4,6,6,0,2,3,7,0};

unsigned char	temp_map_palet0[]	= { 42, 35, 35, 43, 35,124, 43, 43,
							    35, 43, 45, 43, 43, 43, 43, 43};

unsigned char	temp_map_palet1[]	= {126,247,247,109,247,120,108,116,
							   247,106,113,118,107,117,119,110};


DO_COMMAND(do_redit)
{
	int room, new_room;
	struct exit_data *exit;
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

	if (!strcasecmp(left, "goto"))
	{
		room = find_room(ses, right);

		if (room != -1)
		{
			ses->in_room = room;
			tintin_printf2(ses, "Set to room %s", ses->room_list[room]->name);
		}
		return ses;
	}

	if (!strcasecmp(left, "firstroom"))
	{
		create_room(ses, "{1} {0} {<078>} {firstroom}");

		tintin_printf2(ses, "First room created, used #redit goto firstroom, to proceed");

		return ses;
	}

	if (ses->room_list[ses->in_room] == NULL)
	{
		tintin_printf2(ses, "Use #redit goto first, or use #redit firstroom.");

		return ses;
	}

	if (!strcasecmp(left, "map"))
	{
		show_map(ses, right);
	}

	if (!strcasecmp(left, "vtmap"))
	{
		show_vtmap(ses);
	}

	if (!strcasecmp(left, "find"))
	{
		shortest_path(ses, right);
	}

	if (!strcasecmp(left, "delete"))
	{
		for (exit = ses->room_list[ses->in_room]->f_exit ; exit ; exit = exit->next)
		{
			if (!strcmp(exit->name, right))
			{
				break;
			}
		}

		if (exit == NULL)
		{
			tintin_printf2(ses, "No exit with that name found");

			return ses;
		}

		room = exit->vnum;

		for (new_room = 0 ; new_room < MAX_ROOM ; new_room++)
		{
			if (ses->room_list[new_room])
			{
				for (exit = ses->room_list[new_room]->f_exit ; exit ; exit = exit->next)
				{
					if (exit->vnum == room)
					{
						UNLINK(exit, ses->room_list[new_room]->f_exit, ses->room_list[new_room]->l_exit);
						break;
					}
				}
			}
		}
		tintin_printf2(ses, "Room deleted");
	}

	if (!strcasecmp(left, "create"))
	{
		struct listnode *node;

		if ((node = searchnode_list(ses->list[LIST_PATHDIR], right)) == NULL)
		{
			tintin_printf2(ses, "Use: #redit create {pathdir}");
			return ses;
		}

		for (new_room = 1 ; new_room < MAX_ROOM ; new_room++)
		{
			if (ses->room_list[new_room] == NULL)
			{
				break;
			}
		}

		for (exit = ses->room_list[ses->in_room]->f_exit ; exit ; exit = exit->next)
		{
			if (!strcmp(exit->name, right))
			{
				tintin_printf2(ses, "There is already a room in that direction.");
				return ses;
			}
		}

		if (new_room == MAX_ROOM)
		{
			tintin_printf2(ses, "Maximum amount of rooms of %d reached.", MAX_ROOM);
			return ses;
		}

		find_coord(ses, right);

		if (temp_tar_room)
		{
			new_room = temp_tar_room;
		}
		else
		{
			sprintf(left, "{%d} {0} {<088>} {}", new_room);
			create_room(ses, left);
		}
		sprintf(left, "{%d} {%s} {%s}", new_room, right, right);
		create_exit(ses, left);

		room = ses->in_room;
		ses->in_room = new_room;

		
		sprintf(left, "{%d} {%s} {%s}", room, node->right, node->right);
		create_exit(ses, left);

		ses->in_room = room;

		tintin_printf2(ses, "Created new room.");
	}

	if (!strcasecmp(left, "list"))
	{
		for (room = 0 ; room < MAX_ROOM ; room++)
		{
			if (ses->room_list[room])
			{
				tintin_printf2(ses, "[%5d] %10s", ses->room_list[room]->vnum, ses->room_list[room]->name);

				for (exit = ses->room_list[room]->f_exit ; exit ; exit = exit->next)
				{
					tintin_printf2(ses, " - %5s %10s [%5d] %10s", exit->name, exit->cmd, exit->vnum, ses->room_list[exit->vnum]->name);
				}
			}
		}
	}

	if (!strcasecmp(left, "name"))
	{
		free(ses->room_list[ses->in_room]->name);
		ses->room_list[ses->in_room]->name = strdup(right);
	}

	if (!strcasecmp(left, "color"))
	{
		free(ses->room_list[ses->in_room]->color);
		ses->room_list[ses->in_room]->color = strdup(right);
	}

	if (!strcasecmp(left, "link"))
	{
		struct listnode *node;

		arg = get_arg_in_braces(right, left, FALSE);
		arg = get_arg_in_braces(arg, right, TRUE);

		if ((node = searchnode_list(ses->list[LIST_PATHDIR], left)) == NULL)
		{
			tintin_printf2(ses, "Use: #redit link {pathdir} {room name}");
			return ses;
		}

		if ((new_room = find_room(ses, right)) == -1)
		{
			tintin_printf2(ses, "Couldn't find room {%s}", right);
			return ses;
		}

		sprintf(right, "{%d} {%s} {%s}", new_room, left, left);
		create_exit(ses, right);

		room = ses->in_room;
		ses->in_room = new_room;

		sprintf(right, "{%d} {%s} {%s}", room, node->right, node->right);
		create_exit(ses, right);

		ses->in_room = room;

		tintin_printf2(ses, "Created new room.");
	}

	if (!strcasecmp(left, ""))
	{
		tintin_printf2(ses, "Available redit options");
		tintin_printf2(ses, "#redit list                          (shows list of created rooms)");
		tintin_printf2(ses, "#redit goto <room name>              (moves you to given room)");
		tintin_printf2(ses, "#redit map <radius>                  (shows an ascii map)");
		tintin_printf2(ses, "#redit create <direction>            (creates a new room)");
		tintin_printf2(ses, "#redit link <direction> <room name>  (links 2 rooms)");
		tintin_printf2(ses, "#redit name <room name>              (set the room's name)");
		tintin_printf2(ses, "#redit color <color code>            (set the room's color for map)");
		tintin_printf2(ses, "#redit find <room name>              (#map and #walk to use)");
	}

	return ses;
}

int create_room(struct session *ses, const char *arg)
{
	char vnum[BUFFER_SIZE], flags[BUFFER_SIZE], color[BUFFER_SIZE], name[BUFFER_SIZE];
	struct room_data *newroom;

	arg = get_arg_in_braces(arg, vnum,  FALSE);
	arg = get_arg_in_braces(arg, flags, FALSE);
	arg = get_arg_in_braces(arg, color, FALSE);
	arg = get_arg_in_braces(arg, name,  TRUE);

	newroom        = calloc(1, sizeof(struct room_data));

	newroom->name  = strdup(name);
	newroom->color = strdup(color);

	newroom->flags = atoi(flags);
	newroom->vnum  = atoi(vnum);

	ses->room_list[newroom->vnum] = newroom;

	tintin_printf2(ses, "#READMAP R %5s {%s}", vnum, name);

	return newroom->vnum;
}

void create_exit(struct session *ses, const char *arg)
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

	LINK(newexit, ses->room_list[ses->in_room]->f_exit, ses->room_list[ses->in_room]->l_exit);

	tintin_printf2(ses, "#READMAP E %5s {%s} {%s}", vnum, name, cmd);
}


int get_map_exit(struct exit_data *exit)
{
	if (exit->name[0])
	{
		if (exit->name[1] == 0)
		{
			switch (exit->name[0])
			{
				case 'n':
					return 0;
				case 'e':
					return 1;
				case 's':
					return 2;
				case 'w':
					return 3;
				default:
					return -1;
			}
		}

		if (exit->name[2] == 0 && exit->name[0] == 'n')
		{
			switch (exit->name[1])
			{
				case 'e':
					return 4;
				case 'w':
					return 5;
				default:
					return -1;
			}
		}

		if (exit->name[2] == 0 && exit->name[0] == 's')
		{
			switch (exit->name[1])
			{
				case 'e':
					return 6;
				case 'w':
					return 7;
				default:
					return -1;
			}
		}
	}
	return -1;
}


void follow_map(struct session *ses, const char *argument)
{
	char buf[BUFFER_SIZE];
	struct exit_data *exit;
	int room, new_room;

	for (exit = ses->room_list[ses->in_room]->f_exit ; exit ; exit = exit->next)
	{
		if (!strcmp(exit->name, argument))
		{
			ses->in_room = exit->vnum;
			return;
		}
	}

	{
		struct listnode *node;

		if ((node = searchnode_list(ses->list[LIST_PATHDIR], argument)) == NULL)
		{
			return;
		}

		for (new_room = 1 ; new_room < MAX_ROOM ; new_room++)
		{
			if (ses->room_list[new_room] == NULL)
			{
				break;
			}
		}

		if (new_room == MAX_ROOM)
		{
			tintin_printf2(ses, "Maximum amount of rooms of %d reached.", MAX_ROOM);
			return;
		}

		if (find_coord(ses, (char *) argument))
		{
			new_room = temp_tar_room;
		}
		else
		{
			sprintf(buf, "{%d} {0} {<088>} {}", new_room);
			create_room(ses, buf);
			tintin_printf2(ses, "Created new room vnum: %d", new_room);
		}
		sprintf(buf, "{%d} {%s} {%s}", new_room, node->left, node->left);
		create_exit(ses, buf);

		room = ses->in_room;
		ses->in_room = new_room;

		sprintf(buf, "{%d} {%s} {%s}", room, node->right, node->right);
		create_exit(ses, buf);

		ses->in_room = room;



		follow_map(ses, argument);
	}
}


void build_map(short room, short x, short y)
{
	struct exit_data *exit;
	short xx = 0, yy = 0;

	temp_map[x][y] = temp_ses->room_list[room];

	for (exit = temp_ses->room_list[room]->f_exit ; exit ; exit = exit->next)
	{
		if (temp_ses->room_list[exit->vnum]->size)
		{
			continue;
		}
		else
		{
			temp_ses->room_list[exit->vnum]->size = TRUE;
		}

		switch (get_map_exit(exit))
		{
			case 0: xx = x;	yy = y+1;	break;
			case 1: xx = x+1;	yy = y;	break;
			case 2: xx = x;	yy = y-1;	break;
			case 3: xx = x-1;	yy = y;	break;
			case 4: xx = x+1;   yy = y+1; break;
			case 5: xx = x-1;   yy = y+1; break;
			case 6: xx = x+1;   yy = y-1; break;
			case 7: xx = x-1;   yy = y-1; break;
			default:                      continue;
		}

		if (xx < 0 || xx > temp_map_x || yy < 0 || yy > temp_map_y)
		{
			continue;
		}

		if (temp_map[xx][yy])
		{
			continue;
		}
		build_map(exit->vnum, xx, yy);
	}
}

void create_map(struct session *ses, short room, short x, short y)
{
	short vnum;

	for (vnum = 0 ; vnum < MAX_ROOM ; vnum++)
	{
		if (ses->room_list[vnum])
		{
			ses->room_list[vnum]->size = FALSE;
		}
	}

	temp_ses = ses;

	temp_map_y = y;
	temp_map_x = x;

	build_map(room, x / 2, y / 2);
}

void show_map(struct session *ses, char *argument)
{
	char buf[BUFFER_SIZE];
	int x, y, size;

	push_call("show_map(%p,%p)",ses,argument);

	memset(temp_map, 0, sizeof(temp_map));
	memset(buf, 0, BUFFER_SIZE);

	size = atoi(argument);

	temp_map_y = get_scroll_size(ses) - 1;
	temp_map_x = ses->cols - 1;

	temp_map_y = URANGE(12, size * 2, temp_map_y);
	temp_map_x = URANGE(28, size * 4, temp_map_x);

	create_map(ses, ses->in_room, temp_map_x, temp_map_y);

	for (y = temp_map_y ; y >= 0 ; y--)
	{
		strcpy(buf, "{");

		for (x = 0 ; x <= temp_map_x ; x++)
		{
			strcat(buf, draw_room(ses, temp_map[x][y]));
		}
		strcat(buf, "}");

		do_showme(ses, buf);
	}

	/*
		the ESC ( toggles vt100 graphics mapping
	*/

	switch (0)
	{
		case 0:
			break;

		default:
			tintin_printf2(ses, "\033(0%s\033(B", buf);
			break;
	}
	pop_call();
	return;
}


void show_vtmap(struct session *ses)
{
	char buf[BUFFER_SIZE], out[BUFFER_SIZE];
	int x, y;

	push_call("show_vtmap(%p,%p)",ses);

	memset(temp_map, 0, sizeof(temp_map));
	memset(buf, 0, BUFFER_SIZE);

	temp_map_y = (ses->top_row - 3);
	temp_map_x = (ses->cols - 1);

	create_map(ses, ses->in_room, temp_map_x, temp_map_y);

	save_pos(ses);
	goto_rowcol(ses, 1, 1);

	for (y = temp_map_y ; y >= 0 ; y--)
	{
		buf[0] = 0;

		for (x = 0 ; x <= temp_map_x ; x++)
		{
			strcat(buf, draw_room(ses, temp_map[x][y]));
		}
		substitute(ses, buf, out, SUB_COL);

		printf("%s\n\r", out);
	}

	restore_pos(ses);

	/*
		the ESC ( toggles vt100 graphics mapping
	*/

	switch (0)
	{
		case 0:
			break;
		default:
			tintin_printf2(ses, "\033(0%s\033(B", buf);
			break;
	}
	pop_call();
	return;
}

char *draw_room(struct session *ses, struct room_data *room)
{
	struct exit_data *exit;
	static char buf[11];
	int door, exits = 0;

	push_call("draw_room(%p,%p)",ses,room);

	if (room == NULL)
	{
		pop_call();
		return " ";
	}

	if (room->vnum == ses->in_room)
	{
		pop_call();
		return "<118>X<088>";
	}

	for (exit = room->f_exit ; exit ; exit = exit->next)
	{
		door = get_map_exit(exit);

		if (door >= 0 && door <= 3)
		{
			SET_BIT(exits, 1 << get_map_exit(exit));
		}
	}

	sprintf(buf, "%s%c", room->color, temp_map_palet0[exits]);

/*
	sprintf(buf, "%s%c", room->color, temp_map_palet0[exits]);
*/
	pop_call();
	return buf;
}


int find_room(struct session *ses, char *arg)
{
	int room;

	for (room = 0 ; room < MAX_ROOM ; room++)
	{
		if (ses->room_list[room] && !strcasecmp(arg, ses->room_list[room]->name))
		{
			return room;
		}
	}
	return -1;
}

/*
	Fast, short distance node weighted algorithm - Scandum
*/

void search_path(short room, short size)
{
	if (size < temp_max_size && temp_ses->room_list[room]->size > size)
	{
		struct exit_data *exit;

		temp_ses->room_list[room]->size = size;

		if (room == temp_tar_room)
		{
			temp_exit_best = temp_exit_node;
			temp_max_size  = size;
		}
		else
		{
			size++;

			for (exit = temp_ses->room_list[room]->f_exit ; exit ; exit = exit->next)
			{
				if (temp_ses->room_list[exit->vnum])
				{
					search_path(exit->vnum, size);
				}
			}
		}
	}
}

void shortest_path(struct session *ses, char *arg)
{
	struct exit_data *exit;
	int room, vnum;

	kill_list(ses, LIST_PATH);

	room = find_room(ses, arg);

	if (room == -1)
	{
		tintin_printf2(ses, "Couldn't find the room named '%s'", arg);
		return;
	}

	if (room == ses->in_room)
	{
		tintin_printf2(ses, "Already there.");
		return;
	}

	temp_exit_best = NULL;
	temp_ses       = ses;
	temp_tar_room  = room;
	temp_max_size  = 500;

	for (vnum = 0 ; vnum < MAX_ROOM ; vnum++)
	{
		if (ses->room_list[vnum])
		{
			ses->room_list[vnum]->size = temp_max_size;
		}
	}

	room = ses->in_room;

	ses->room_list[room]->size = 0;

	for (exit = ses->room_list[room]->f_exit ; exit ; exit = exit->next)
	{
		if (ses->room_list[exit->vnum])
		{
			temp_exit_node = exit;
			search_path(exit->vnum, 0);
		}
	}

	if (temp_exit_best == NULL)
	{
		tintin_printf2(ses, "No path not found.");
		return;
	}

	addnode_list(ses->list[LIST_PATH], temp_exit_best->cmd, "", "0");

	while (TRUE)
	{
		if (temp_exit_best->vnum == temp_tar_room)
		{
			break;
		}

		room = temp_exit_best->vnum;

		ses->room_list[room]->size = 0;

		for (exit = ses->room_list[room]->f_exit ; exit ; exit = exit->next)
		{
			if (ses->room_list[exit->vnum])
			{
				temp_exit_node = exit;
				search_path(exit->vnum, 0);


			}
		}
		addnode_list(ses->list[LIST_PATH], temp_exit_best->cmd, "", "0");
	}
}

/*
	Do a coordinate based search
*/

int find_coord(struct session *ses, char *arg)
{
	int vnum;

	temp_map_x = 0;
	temp_map_y = 0;
	temp_map_z = 0;

	switch (arg[0])
	{
		case 'n':
			temp_map_y += 1;
			break;
		case 'e':
			temp_map_x += 1;
			break;
		case 's':
			temp_map_y -= 1;
			break;
		case 'w':
			temp_map_x -= 1;
			break;
		case 'u':
			temp_map_z += 1;
			break;
		case 'd':
			temp_map_z -= 1;
			break;
		default:
			return 0;
	}

	temp_ses       = ses;
	temp_tar_room  = 0;

	for (vnum = 0 ; vnum < MAX_ROOM ; vnum++)
	{
		if (ses->room_list[vnum])
		{
			ses->room_list[vnum]->size = 0;
		}
	}

	search_coord(ses->in_room, 0, 0, 0);

	return temp_tar_room;
}

void search_coord(int vnum, short x, short y, short z)
{
	temp_ses->room_list[vnum]->size = 1;

	if (temp_map_x == x && temp_map_y == y && temp_map_z == z)
	{
		tintin_printf2(temp_ses, "found linking room: %d %d %d", x, y, z);
		temp_tar_room = vnum;
	}
	else
	{
		struct exit_data *exit;

		for (exit = temp_ses->room_list[vnum]->f_exit ; exit ; exit = exit->next)
		{
			if (temp_ses->room_list[exit->vnum] && temp_ses->room_list[exit->vnum]->size == 0)
			{
				switch (exit->name[0])
				{
					case 'n':
						search_coord(exit->vnum, x, y+1, z);
						break;
					case 'e':
						search_coord(exit->vnum, x+1, y, z);
						break;
					case 's':
						search_coord(exit->vnum, x, y-1, z);
						break;
					case 'w':
						search_coord(exit->vnum, x-1, y, z);
						break;
					case 'u':
						search_coord(exit->vnum, x, y, z+1);
						break;
					case 'd':
						search_coord(exit->vnum, x, y, z-1);
						break;
				}
			}
		}
	}
}
