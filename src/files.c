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
*               (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                  *
*                                                                             *
*                        coded by Peter Unold 1992                            *
*                       New code by Bill Reiss 1993                           *
*                    New code by Joann Ellsworth 1994                         *
*                   New code by Igor van den Hoven 1994                       *
******************************************************************************/

#include "tintin.h"
#include <sys/stat.h>

/*
	read and execute a command file, supports multi lines - Igor
*/

DO_COMMAND(do_read)
{
	FILE *fp;
	struct stat filedata;
	char *bufi, *bufo, filename[BUFFER_SIZE], temp[BUFFER_SIZE], *pti, *pto;
	int lvl, cnt, com, lnc, fix, ok;
	int counter[LIST_MAX];

	get_arg_in_braces(arg, filename, TRUE);

	if ((fp = fopen(filename, "r")) == NULL)
	{
		tintin_printf(ses, "#ERROR: #READ {%s} - COULDN'T OPEN THAT FILE.", filename);
		return ses;
	}

	temp[0] = getc(fp);

	if (!isgraph(temp[0]))
	{
		tintin_printf(ses, "#ERROR: #READ {%s} - INVALID START OF FILE.", filename);

		fclose(fp);

		return ses;
	}

	ungetc(temp[0], fp);

	for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
	{
		if (HAS_BIT(list_table[cnt].flags, LIST_FLAG_READ))
		{
			counter[cnt] = ses->list[cnt]->count;
		}
	}

	stat(filename, &filedata);

	if ((bufi = calloc(1, filedata.st_size + 2)) == NULL || (bufo = calloc(1, filedata.st_size + 2)) == NULL)
	{
		tintin_printf(ses, "#ERROR: #READ {%s} - FAILED TO ALLOCATE MEMORY.", filename);

		fclose(fp);

		return ses;
	}

	fread(bufi, 1, filedata.st_size, fp);

	pti = bufi;
	pto = bufo;

	lvl = com = lnc = fix = ok = 0;

	while (*pti)
	{
		if (com == 0)
		{
#ifdef BIG5
			if (*pti & 0x80)
			{
				*pto++ = *pti++;
				if (*pti)
				{
					*pto++ = *pti++;
				}
				continue;
			}
#endif
			switch (*pti)
			{
				case DEFAULT_OPEN:
					*pto++ = *pti++;
					lvl++;
					break;

				case DEFAULT_CLOSE:
					*pto++ = *pti++;
					lvl--;
					break;

				case '/':
					if (lvl == 0 && pti[1] == '*')
					{
						pti += 2;
						com += 1;
					}
					else
					{
						*pto++ = *pti++;
					}
					break;

				case '\r':
					pti++;
					break;

				case '\n':
					lnc++;

					pto--;

					while (isspace(*pto))
					{
						pto--;
					}

					pto++;

					if (fix == 0 && pti[1] == gtd->tintin_char)
					{
						if (lvl == 0)
						{
							ok = lnc + 1;
						}
						else
						{
							fix = lnc;
						}

					}
					if (lvl)
					{
						pti++;

						while (isspace(*pti))
						{
							if (*pti == '\n')
							{
								lnc++;

								if (fix == 0 && pti[1] == gtd->tintin_char)
								{
									fix = lnc;
								}
							}
							pti++;
						}
					}
					else for (cnt = 1 ; ; cnt++)
					{
						if (pti[cnt] == 0)
						{
							*pto++ = *pti++;
							break;
						}

						if (pti[cnt] == DEFAULT_OPEN || pti[cnt] == DEFAULT_CLOSE)
						{
							pti++;
							while (isspace(*pti))
							{
								pti++;
							}
							*pto++ = ' ';
							break;
						}

						if (!isspace(pti[cnt]))
						{
							*pto++ = *pti++;
							break;
						}
					}
					break;

				default:
					*pto++ = *pti++;
					break;
			}
		}
		else
		{
			switch (*pti)
			{
				case '/':
					if (pti[1] == '*')
					{
						pti += 2;
						com += 1;
					}
					else
					{
						pti += 1;
					}
					break;

				case '*':
					if (pti[1] == '/')
					{
						pti += 2;
						com -= 1;
					}
					else
					{
						pti += 1;
					}
					break;

				case '\n':
					lnc++;
					pti++;
					break;

				default:
					pti++;
					break;
			}
		}
	}
	*pto++ = '\n';
	*pto   = '\0';

	if (lvl)
	{
		tintin_printf(ses, "#ERROR: #READ {%s} - MISSING %d '%c' BETWEEN LINE %d AND %d.", filename, abs(lvl), lvl < 0 ? DEFAULT_OPEN : DEFAULT_CLOSE, fix == 0 ? 1 : ok, fix == 0 ? lnc + 1 : fix);

		fclose(fp);

		free(bufi);
		free(bufo);

		return ses;
	}

	if (com)
	{
		tintin_printf(ses, "#ERROR: #READ {%s} - MISSING %d '%s'", filename, abs(com), com < 0 ? "/*" : "*/");

		fclose(fp);

		free(bufi);
		free(bufo);

		return ses;
	}

	sprintf(temp, "{TINTIN CHAR} {%c}", bufo[0]);

	SET_BIT(gtd->flags, TINTIN_FLAG_QUIET);

	do_configure(ses, temp);

	lvl = 0;
	pti = bufo;
	pto = bufi;

	while (*pti)
	{
		if (*pti != '\n')
		{
			*pto++ = *pti++;
			continue;
		}
		*pto = 0;

		if (strlen(bufi) >= BUFFER_SIZE)
		{
			DEL_BIT(gtd->flags, TINTIN_FLAG_QUIET);

			bufi[20] = 0;

			tintin_printf(ses, "#ERROR: #READ {%s} - BUFFER OVERFLOW AT COMMAND: %s.", filename, bufi);

			fclose(fp);

			free(bufi);
			free(bufo);

			return ses;
		}

		if (bufi[0])
		{
			ses = parse_input(ses, bufi);
		}
		pto = bufi;
		pti++;
	}

	DEL_BIT(gtd->flags, TINTIN_FLAG_QUIET);

	if (!HAS_BIT(ses->flags, SES_FLAG_VERBOSE))
	{
		for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
		{
			if (HAS_BIT(list_table[cnt].flags, LIST_FLAG_READ))
			{
				switch (ses->list[cnt]->count - counter[cnt])
				{
					case 0:
						break;

					case 1:
						tintin_printf(ses, "#OK: %3d %s LOADED.", ses->list[cnt]->count - counter[cnt], list_table[cnt].name);
						break;

					default:
						tintin_printf(ses, "#OK: %3d %s LOADED.", ses->list[cnt]->count - counter[cnt], list_table[cnt].name_multi);
						break;
				}
			}
		}
	}
	fclose(fp);

	free(bufi);
	free(bufo);

	return ses;
}


DO_COMMAND(do_write)
{
	FILE *file;
	char temp[BUFFER_SIZE], filename[BUFFER_SIZE];
	struct listnode *node;
	int cnt;

	get_arg_in_braces(arg, filename, TRUE);

	if (*filename == 0 || (file = fopen(filename, "w")) == NULL)
	{
		tintin_printf(ses, "#ERROR: #write {%s} - COULDN'T OPEN FILE TO WRITE.", filename);
		return ses;
	}

	for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
	{
		if (!HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_WRITE))
		{
			continue;
		}

		for (node = ses->list[cnt]->f_node ; node ; node = node->next)
		{
			prepare_for_write(cnt, node, temp);

			fputs(temp, file);
		}
	}

	fclose(file);

	show_message(ses, -1, "#OK. COMMAND FILE WRITTEN.", filename);

	return ses;
}


void prepare_for_write(int list, struct listnode *node, char *result)
{
	int llen = strlen(node->left)  > 20 ? 20 : strlen(node->left);
	int rlen = strlen(node->right) > 25 ? 25 : strlen(node->right);

	switch (list_table[list].args)
	{
		case 0:
			result[0] = 0;
			break;
		case 1:
			sprintf(result, "%c%-16s {%s}\n", gtd->tintin_char, list_table[list].name, node->left);
			break;
		case 2:
			sprintf(result, "%c%-16s {%s} %*s {%s}\n", gtd->tintin_char, list_table[list].name, node->left, 20 - llen, "", node->right);
			break;
		case 3:
			sprintf(result, "%c%-16s {%s} %*s {%s} %*s {%s}\n", gtd->tintin_char, list_table[list].name, node->left, 20 - llen, "", node->right, 25 - rlen, "", node->pr);
			break;
	}
}


DO_COMMAND(do_textin)
{
	FILE *fp;
	char filename[BUFFER_SIZE], buffer[BUFFER_SIZE], *cptr;

	get_arg_in_braces(arg, filename, TRUE);

	if ((fp = fopen(filename, "r")) == NULL)
	{
		tintin_printf(ses, "#ERROR: #TEXTIN {%s} - FILE NOT FOUND.", filename);
		return ses;
	}

	while (fgets(buffer, sizeof(buffer), fp))
	{
		for (cptr = buffer ; *cptr && *cptr != '\n' ; cptr++)
		{
			;
		}
		*cptr++ = '\r';
		*cptr++ = '\n';
		*cptr++ = '\0';

		write_line_mud(buffer, ses);
	}
	fclose(fp);

	show_message(ses, -1, "#OK. FILE READ.");

	return ses;
}


DO_COMMAND(do_scan)
{
	FILE *fp;
	char filename[BUFFER_SIZE];

	get_arg_in_braces(arg, filename, TRUE);

	if ((fp = fopen(filename, "r")) == NULL)
	{
		tintin_printf(ses, "#ERROR: #SCAN {%s} - FILE NOT FOUND.", filename);
		return ses;
	}

	SET_BIT(ses->flags, SES_FLAG_SCAN);

	while (BUFFER_SIZE >= gtd->mud_output_max)
	{
		gtd->mud_output_max *= 2;
		gtd->mud_output_buf  = realloc(gtd->mud_output_buf, gtd->mud_output_max);
	}

	while (fgets(gtd->mud_output_buf, BUFFER_SIZE, fp))
	{
		readmud(ses);
	}

	DEL_BIT(ses->flags, SES_FLAG_SCAN);

	show_message(ses, -1, "#OK. FILE READ.", filename);

	fclose(fp);

	return ses;
}
