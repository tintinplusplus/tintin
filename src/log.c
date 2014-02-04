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
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                      coded by Igor van den Hoven 2004                       *
******************************************************************************/

#include "tintin.h"


void logit(struct session *ses, char *txt, FILE *file)
{
	char out[BUFFER_SIZE];

	if (HAS_BIT(ses->flags, SES_FLAG_LOGPLAIN))
	{
		strip_vt102_codes(txt, out);
		strcat(out, "\n");
	}
	else if (HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
	{
		vt102_to_html(ses, txt, out);
	}
	else
	{
		sprintf(out, "%s\n", txt);
	}
	fputs(out, file);

	fflush(file);
}


DO_COMMAND(do_log)
{
	FILE *logfile;

	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, FALSE);

	if (ses->logfile)
	{
		fclose(ses->logfile);
		ses->logfile = NULL;
		tintin_printf(ses, "#OK: LOGGING TURNED OFF.");
	}
	else if (*left == 0 || *right == 0 || (!is_abbrev(left, "APPEND") && !is_abbrev(left, "OVERWRITE")))
	{
		tintin_printf(ses, "#SYNTAX: #LOG [<APPEND|OVERWRITE> <FILENAME>]");
	}
	else
	{
		if (is_abbrev(left, "APPEND"))
		{
			if ((logfile = fopen(right, "a")))
			{
				ses->logfile = logfile;

				tintin_printf(ses, "#OK: LOGGING OUTPUT TO '%s' FILESIZE: %ld", right, fseek(ses->logfile, 0, SEEK_END));

				if (fseek(ses->logfile, 0, SEEK_END) == 0 && HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
				{
					write_html_header(ses->logfile);
				}
			}
			else
			{
				tintin_printf(ses, "#ERROR: #LOG {%s} {%s} - COULDN'T OPEN FILE.", left, right);
			}
		}
		else
		{
			if ((logfile = fopen(right, "w")))
			{
				tintin_printf(ses, "#OK: LOGGING OUTPUT TO '%s'", right);

				ses->logfile = logfile;

				if (HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
				{
					write_html_header(ses->logfile);
				}
			}
			else
			{
				tintin_printf(ses, "#ERROR: #LOG {%s} {%s} - COULDN'T OPEN FILE.", left, right);
			}
		}
	}
	return ses;
}


DO_COMMAND(do_logline)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left, FALSE);

	arg = get_arg_in_braces(arg, temp, TRUE);
	substitute(ses, temp, right, SUB_ESC|SUB_COL);

	if (ses->logline)
	{
		return ses;
	}

	if ((ses->logline = fopen(left, "a")))
	{
		if (fseek(ses->logline, 0, SEEK_END) == 0 && HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
		{
			write_html_header(ses->logline);
		}

		if (*right)
		{
			logit(ses, right, ses->logline);

			fclose(ses->logline);
			ses->logline = NULL;
		}
	}
	else
	{
		tintin_printf(ses, "#ERROR: #LOGLINE {%s} - COULDN'T OPEN FILE.", left);
	}
	return ses;
}


DO_COMMAND(do_writebuffer)
{
	FILE *fp;
	char left[BUFFER_SIZE], out[BUFFER_SIZE];
	int cnt;

	arg = get_arg_in_braces(arg, left, FALSE);

	if (*left == 0)
	{
		tintin_printf(ses, "#SYNTAX: #BUFFER WRITE <FILENAME>]");
	}
	else
	{
		if ((fp = fopen(left, "w")))
		{
			tintin_printf(ses, "#OK: WRITING BUFFER TO '%s'", left);

			if (HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
			{
				write_html_header(fp);
			}

			cnt = ses->scroll_row;

			do
			{
				if (cnt == 0)
				{
					cnt = ses->scroll_max - 1;
				}
				else
				{
					cnt--;
				}

				if (ses->buffer[cnt] == NULL)
				{
					continue;
				}

				if (HAS_BIT(ses->flags, SES_FLAG_LOGPLAIN))
				{
					strip_vt102_codes(ses->buffer[cnt], out);
					strcat(out, "\n");
				}
				else if (HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
				{
					vt102_to_html(ses, ses->buffer[cnt], out);
				}
				else
				{
					sprintf(out, "%s\n", ses->buffer[cnt]);
				}
				fputs(out, fp);
			}
			while (cnt != ses->scroll_row);

			fclose(fp);
		}
		else
		{
			tintin_printf(ses, "#ERROR: #BUFFER WRITE {%s} - FAILED TO OPEN FILE.", left);
		}
	}
	return ses;
}

void write_html_header(FILE *fp)
{
	char *header =
		"<html>\n"
		"<head>\n"
		"<meta http-equiv=\"content-type\" content=\"text/html; charset=iso-8859-1\">\n"
		"<meta name=\"description\" content=\"Generated by "VERSION_NUM" - http://tintin.sourceforge.net\">\n"
		"<style type=\"text/css\">\n"
		"{\n\tfont-family: Courier;\n\tfont-size: 10pt;\n}\n"
		".d30{ color: #000; } .l30{ color: #555; } .b40{ background-color: #000; }\n"
		".d31{ color: #B00; } .l31{ color: #F55; } .b41{ background-color: #B00; }\n"
		".d32{ color: #0B0; } .l32{ color: #5F5; } .b42{ background-color: #0B0; }\n"
		".d33{ color: #BB0; } .l33{ color: #FF5; } .b43{ background-color: #BB0; }\n"
		".d34{ color: #00B; } .l34{ color: #55F; } .b44{ background-color: #00B; }\n"
		".d35{ color: #B0B; } .l35{ color: #F5F; } .b45{ background-color: #B0B; }\n"
		".d36{ color: #0BB; } .l36{ color: #5FF; } .b46{ background-color: #0BB; }\n"
		".d37{ color: #BBB; } .l37{ color: #FFF; } .b47{ background-color: #BBB; }\n"
		".d38{ color: #FFF; } .l38{ color: #FFF; } .b48{ background-color: #000; }\n"
		".d39{ color: #FFF; } .l39{ color: #FFF; } .b49{ background-color: #000; }\n"
		"</STYLE>\n"
		"<body bgcolor=\"#000000\">\n"
		"</head>\n"
		"<pre>\n"
		"<span class=\"b49\"><span class=\"d39\">\n";
	fputs(header, fp);
}


void vt102_to_html(struct session *ses, char *txt, char *out)
{
	char tmp[BUFFER_SIZE], *pti, *pto;
	int vtc, fgc, bgc, cnt;

	vtc = ses->vtc;
	fgc = ses->fgc;
	bgc = ses->bgc;

	pti = txt;
	pto = out;

	while (*pti)
	{
		while (skip_vt102_codes_non_graph(pti))
		{
			pti += skip_vt102_codes_non_graph(pti);
		}

		switch (*pti)
		{
			case 27:
				pti += 2;

				for (cnt = 0 ; pti[cnt] ; cnt++)
				{
					tmp[cnt] = pti[cnt];

					if (pti[cnt] == ';' || pti[cnt] == 'm')
					{
						tmp[cnt] = 0;

						cnt = -1;
						pti += 1 + strlen(tmp);

						switch (atoi(tmp))
						{
							case 0:
								vtc = 0;
								fgc = 39;
								bgc = 49;
								break;
							case 1:
								SET_BIT(vtc, 1);
								break;
							case 4:
								SET_BIT(vtc, 2);
								break;
							case 5:
								break;
							case 7:
								SET_BIT(vtc, 4);
								break;
							case  2:
							case 21:
							case 22:
								DEL_BIT(vtc, 1);
								break;
							case 24:
								DEL_BIT(vtc, 2);
								break;
							case 27:
								DEL_BIT(vtc, 4);
								break;
							case 38:
								SET_BIT(vtc, 2);
								fgc = 38;
								break;
							case 39:
								DEL_BIT(vtc, 2);
								fgc = 39;
								break;
							default:
								if (atoi(tmp) >= 40 && atoi(tmp) < 50)
								{
									bgc = atoi(tmp);
								}
								if (atoi(tmp) >= 30 && atoi(tmp) < 40)
								{
									fgc = atoi(tmp);
								}
								break;
						}
					}
					if (pti[-1] == 'm')
					{
						break;
					}
				}
				if (HAS_BIT(vtc, 2) && !HAS_BIT(ses->vtc, 2))
				{
					strcpy(pto, "<u>");
					pto += strlen(pto);
				}
				if (!HAS_BIT(vtc, 2) && HAS_BIT(ses->vtc, 2))
				{
					strcpy(pto, "</u>");
					pto += strlen(pto);
				}

				if (HAS_BIT(vtc, 4))
				{
					cnt = fgc;
					fgc = bgc - 10;
					bgc = cnt + 10;
				}

				if (bgc != ses->bgc || fgc != ses->fgc || vtc != ses->vtc)
				{
					sprintf(pto, "</span>");
					pto += strlen(pto);

					if (bgc != ses->bgc)
					{
						sprintf(pto, "</span>");
						pto += strlen(pto);
					}
				}

				if (bgc != ses->bgc || fgc != ses->fgc || vtc != ses->vtc)
				{
					if (bgc != ses->bgc)
					{
						sprintf(pto, "<span class=\"b%d\">", bgc);
						pto += strlen(pto);
					}
					if (HAS_BIT(vtc, 1))
					{
						sprintf(pto, "<span class=\"l%d\">", fgc);
						pto += strlen(pto);
					}
					else
					{
						sprintf(pto, "<span class=\"d%d\">", fgc);
						pto += strlen(pto);
					}
				}
				ses->vtc = vtc;
				ses->fgc = fgc;
				ses->bgc = bgc;
				break;

			case '>':
				sprintf(pto, "&gt;");
				pto += strlen(pto);
				pti++;
				break;

			case '<':
				sprintf(pto, "&lt;");
				pto += strlen(pto);
				pti++;
				break;

			case '\r':
				pti++;
				break;

			case '"':
				sprintf(pto, "&quot;");
				pto += strlen(pto);
				pti++;
				break;

			case '&':
				sprintf(pto, "&amp;");
				pto += strlen(pto);
				pti++;
				break;

			case 0:
				break;

			default:
				*pto++ = *pti++;
				break;
		}
	}
	*pto++ = '\n';
	*pto = 0;
}
