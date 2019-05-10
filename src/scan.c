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
*                     coded by Igor van den Hoven 2005                        *
******************************************************************************/

#include "tintin.h"

#ifdef HAVE_PTY_H
#include <pty.h>
#else
#ifdef HAVE_UTIL_H
#include <util.h>
#endif
#endif

/* support routines for comma separated value files */

char *get_arg_in_quotes(struct session *ses, char *string, char *result, int flag)
{
	char *pti, *pto;
	int nest = 0;

	pti = space_out(string);
	pto = result;

	if (*pti == '"')
	{
		nest = TRUE;
		pti++;
	}

	while (*pti)
	{
		if (HAS_BIT(ses->flags, SES_FLAG_BIG5) && *pti & 128 && pti[1] != 0)
		{
			*pto++ = *pti++;
			*pto++ = *pti++;
			continue;
		}

		if (*pti == '"')
		{
			if (pti[1] == '"')
			{
				*pto++ = *pti++;
			}
			else if (nest)
			{
				nest = FALSE;
			}
			pti++;
			continue;
		}
		else if (nest == TRUE)
		{
			*pto++ = *pti++;
		}
		else if (*pti == ' ' || *pti == '\t')
		{
			pti++;
		}
		else if (*pti == ',')
		{
			pti++;
			break;
		}
		else
		{
			*pto++ = *pti++;
		}
	}

	if (nest)
	{
		tintin_printf2(ses, "#SCAN CSV: GET QUOTED ARGUMENT: UNMATCHED QUOTE.");
	}
	*pto = 0;

	return pti;
}

struct session *scan_csv_file(struct session *ses, FILE *fp, char *filename)
{
	char line[STRING_SIZE], temp[BUFFER_SIZE], *arg;
	int i, header = FALSE;

	SET_BIT(ses->flags, SES_FLAG_SCAN);

	while (fgets(line, BUFFER_SIZE, fp))
	{
		arg = strchr(line, '\r');

		if (arg)
		{
			*arg = 0;
		}
		else
		{
		
			arg = strchr(line, '\n');

			if (arg)
			{
				*arg = 0;
			}
		}

		RESTRING(gtd->vars[0], line);

		arg = line;

		for (i = 1 ; i < 100 ; i++)
		{
			arg = get_arg_in_quotes(ses, arg, temp, FALSE);

			RESTRING(gtd->vars[i], temp);

			if (*arg == 0)
			{
				while (++i < 100)
				{
					if (*gtd->vars[i])
					{
						RESTRING(gtd->vars[i], "");
					}
				}
				break;
			}
		}

		if (header == FALSE)
		{
			header = TRUE;

			check_all_events(ses, SUB_ARG|SUB_SEC, 0, 0, "SCAN CSV HEADER");
		}
		else
		{
			check_all_events(ses, SUB_ARG|SUB_SEC, 0, 0, "SCAN CSV LINE");
		}

		if (HAS_BIT(ses->flags, SES_FLAG_SCANABORT))
		{
			break;
		}
	}

	DEL_BIT(ses->flags, SES_FLAG_SCAN);

	if (HAS_BIT(ses->flags, SES_FLAG_SCANABORT))
	{
		DEL_BIT(ses->flags, SES_FLAG_SCANABORT);

		show_message(ses, LIST_COMMAND, "#SCAN CSV: FILE {%s} PARTIALLY SCANNED.", filename);
	}
	else
	{
		show_message(ses, LIST_COMMAND, "#SCAN CSV: FILE {%s} SCANNED.", filename);
	}
	fclose(fp);

	return ses;
}

/* support routines for tab separated value files */

char *get_arg_stop_tabs(struct session *ses, char *string, char *result, int flag)
{
	char *pti, *pto;

	pti = string;
	pto = result;

	while (*pti)
	{
		if (HAS_BIT(ses->flags, SES_FLAG_BIG5) && *pti & 128 && pti[1] != 0)
		{
			*pto++ = *pti++;
			*pto++ = *pti++;
			continue;
		}

		if (*pti == '\t')
		{
			pti++;
			break;
		}
		*pto++ = *pti++;
	}
	*pto = 0;

	return pti;
}

struct session *scan_tsv_file(struct session *ses, FILE *fp, char *filename)
{
	char line[STRING_SIZE], temp[BUFFER_SIZE], *arg;
	int i, header = FALSE;

	SET_BIT(ses->flags, SES_FLAG_SCAN);

	while (fgets(line, BUFFER_SIZE, fp))
	{
		arg = strchr(line, '\r');

		if (arg)
		{
			*arg = 0;
		}
		else
		{
		
			arg = strchr(line, '\n');

			if (arg)
			{
				*arg = 0;
			}
		}

		RESTRING(gtd->vars[0], line);

		arg = line;

		for (i = 1 ; i < 100 ; i++)
		{
			arg = get_arg_stop_tabs(ses, arg, temp, FALSE);

			RESTRING(gtd->vars[i], temp);

			if (*arg == 0)
			{
				while (++i < 100)
				{
					if (*gtd->vars[i])
					{
						RESTRING(gtd->vars[i], "");
					}
				}
				break;
			}
		}

		if (header == FALSE)
		{
			header = TRUE;

			check_all_events(ses, SUB_ARG|SUB_SEC, 0, 0, "SCAN TSV HEADER");
		}
		else
		{
			check_all_events(ses, SUB_ARG|SUB_SEC, 0, 0, "SCAN TSV LINE");
		}

		if (HAS_BIT(ses->flags, SES_FLAG_SCANABORT))
		{
			break;
		}
	}

	DEL_BIT(ses->flags, SES_FLAG_SCAN);

	if (HAS_BIT(ses->flags, SES_FLAG_SCANABORT))
	{
		DEL_BIT(ses->flags, SES_FLAG_SCANABORT);

		show_message(ses, LIST_COMMAND, "#SCAN TSV: FILE {%s} PARTIALLY SCANNED.", filename);
	}
	else
	{
		show_message(ses, LIST_COMMAND, "#SCAN TSV: FILE {%s} SCANNED.", filename);
	}
	return ses;
}

/* support routines for text files */

struct session *scan_txt_file(struct session *ses, FILE *fp, char *filename)
{
	char line[STRING_SIZE], *arg;

	SET_BIT(ses->flags, SES_FLAG_SCAN);

	while (fgets(line, BUFFER_SIZE - 1, fp))
	{
		arg = strchr(line, '\r');

		if (arg)
		{
			*arg = 0;
		}
		else
		{
		
			arg = strchr(line, '\n');

			if (arg)
			{
				*arg = 0;
			}
		}

		process_mud_output(ses, line, FALSE);

		if (HAS_BIT(ses->flags, SES_FLAG_SCANABORT))
		{
			break;
		}
	}

	DEL_BIT(ses->flags, SES_FLAG_SCAN);

	if (HAS_BIT(ses->flags, SES_FLAG_SCANABORT))
	{
		DEL_BIT(ses->flags, SES_FLAG_SCANABORT);

		show_message(ses, LIST_COMMAND, "#SCAN TXT: FILE {%s} PARTIALLY SCANNED.", filename);
	}
	else
	{
		show_message(ses, LIST_COMMAND, "#SCAN TXT: FILE {%s} SCANNED.", filename);
	}
	return ses;
}

DO_COMMAND(do_scan)
{
	FILE *fp;
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, right, GET_ONE, SUB_VAR|SUB_FUN);

	if (*left == 0)
	{
		show_error(ses, LIST_COMMAND, "#SYNTAX: #SCAN {ABORT|CSV|TXT} {<FILENAME>}");

		return ses;
	}

	if (is_abbrev(left, "ABORT"))
	{
		if (!HAS_BIT(ses->flags, SES_FLAG_SCAN))
		{
			show_error(ses, LIST_COMMAND, "#SCAN ABORT: NOT CURRENTLY SCANNING.");
		}
		else
		{
			SET_BIT(ses->flags, SES_FLAG_SCANABORT);
		}
		return ses;
	}

	if (*right == 0)
	{
		show_error(ses, LIST_COMMAND, "#SYNTAX: #SCAN {ABORT|CSV|TXT} {<FILENAME>}");

		return ses;
	}

	if ((fp = fopen(right, "r")) == NULL)
	{
		show_error(ses, LIST_COMMAND, "#ERROR: #SCAN - FILE {%s} NOT FOUND.", right);

		return ses;
	}

	SET_BIT(ses->flags, SES_FLAG_SCAN);


	if (is_abbrev(left, "CSV"))
	{
		ses = scan_csv_file(ses, fp, right);
	}
	else if (is_abbrev(left, "TSV"))
	{
		ses = scan_tsv_file(ses, fp, right);
	}
	else if (is_abbrev(left, "TXT"))
	{
		ses = scan_txt_file(ses, fp, right);
	}
	else
	{
		DEL_BIT(ses->flags, SES_FLAG_SCAN);

		show_error(ses, LIST_COMMAND, "#SYNTAX: #SCAN {ABORT|CSV|TXT} {<FILENAME>}");
	}

	DEL_BIT(ses->flags, SES_FLAG_SCAN);

	fclose(fp);

	return ses;
}