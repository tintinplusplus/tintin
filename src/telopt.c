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
#include "telnet.h"

/* what we see from the remote end */

char iac_do_naws[]            = { IAC, DO,    TELOPT_NAWS            };
char iac_will_naws[]          = { IAC, WILL,  TELOPT_NAWS            };

char iac_do_sga[]             = { IAC, DO,    TELOPT_SGA             };
char iac_will_sga[]           = { IAC, WILL,  TELOPT_SGA             };

char iac_sb_tspeed[]          = { IAC, SB,    TELOPT_TSPEED, ENV_SEND, IAC, SE };
char iac_sb_ttype[]           = { IAC, SB,    TELOPT_TTYPE,  ENV_SEND, IAC, SE  };



char iac_do_echo[]            = { IAC, DO,    TELOPT_ECHO            };
char iac_wont_echo[]          = { IAC, WONT,  TELOPT_ECHO            };
char iac_will_echo[]          = { IAC, WILL,  TELOPT_ECHO            };

char iac_will_mssp[]          = { IAC, WILL,  TELOPT_MSSP            };
char iac_sb_mssp[]            = { IAC, SB,    TELOPT_MSSP            };

char iac_sb_msdp[]            = { IAC, SB,    TELOPT_MSDP            };

char iac_sb_new_environ[]     = { IAC, SB,    TELOPT_NEW_ENVIRON     };

char iac_sb_zmp[]             = { IAC, SB,    TELOPT_ZMP             };

char iac_sb_mccp1[]           = { IAC, SB,    TELOPT_MCCP1, WILL, SE };
char iac_will_mccp2[]         = { IAC, WILL,  TELOPT_MCCP2           };
char iac_sb_mccp2[]           = { IAC, SB,    TELOPT_MCCP2, IAC, SE  };

char iac_eor[]                = { IAC, EOR                           };
char iac_ga[]                 = { IAC, GA                            };


struct iac_type
{
	int      size;
	char   * code;
	int  (* func) (struct session *ses, int cplen, unsigned char *cpsrc);
};

struct iac_type iac_table [] =
{
	{   3,  iac_do_sga,           &send_will_sga           },
	{   3,  iac_will_sga,         &send_do_sga             },
	{   3,  iac_do_naws,          &send_sb_naws            },
	{   3,  iac_will_naws,        &send_sb_naws            },
	{   3,  iac_do_echo,          &send_echo_will          },
	{   3,  iac_will_echo,        &send_echo_off           },
	{   3,  iac_wont_echo,        &send_echo_on            },
	{   3,  iac_will_mccp2,       &send_do_mccp2           },
	{   3,  iac_will_mssp,        &send_do_mssp            },
	{   3,  iac_sb_mssp,          &recv_sb_mssp            },
	{   3,  iac_sb_msdp,          &recv_sb_msdp            },
	{   3,  iac_sb_new_environ,   &recv_sb_new_environ     },
	{   6,  iac_sb_tspeed,        &recv_sb_tspeed          },
	{   6,  iac_sb_ttype,         &recv_sb_ttype           },
	{   3,  iac_sb_zmp,           &recv_sb_zmp             },
	{   5,  iac_sb_mccp1,         &init_mccp               },
	{   5,  iac_sb_mccp2,         &init_mccp               },
	{   2,  iac_eor,              &mark_prompt             },
	{   2,  iac_ga,               &mark_prompt             },
	{   0,  NULL,                 NULL                     }
};


void telopt_debug(struct session *ses, char *format, ...)
{
	char buf[BUFFER_SIZE];
	va_list args;

	if (HAS_BIT(ses->telopts, TELOPT_FLAG_DEBUG))
	{
		va_start(args, format);
		vsprintf(buf, format, args);
		va_end(args);

		tintin_puts(ses, buf);
	}
}


int translate_telopts(struct session *ses, unsigned char *src, int cplen)
{
	int skip, cnt;
	unsigned char *cpdst;
	unsigned char *cpsrc;

	if (cplen == 0)
	{
		return 0;
	}

	if (ses->mccp)
	{
		ses->mccp->next_in   = src;
		ses->mccp->avail_in  = cplen;

		ses->mccp->next_out  = gtd->mccp_buf;
		ses->mccp->avail_out = gtd->mccp_len;

		inflate:

		switch (inflate(ses->mccp, Z_SYNC_FLUSH))
		{
			case Z_OK:
				if (ses->mccp->avail_out == 0)
				{
					gtd->mccp_len *= 2;
					gtd->mccp_buf  = (unsigned char *) realloc(gtd->mccp_buf, gtd->mccp_len);

					ses->mccp->avail_out = gtd->mccp_len / 2;
					ses->mccp->next_out  = gtd->mccp_buf + gtd->mccp_len / 2;

					goto inflate;
				}
				cplen = ses->mccp->next_out - gtd->mccp_buf;
				cpsrc = gtd->mccp_buf;
				break;

			case Z_STREAM_END:
				cplen = ses->mccp->next_out - gtd->mccp_buf;
				cpsrc = gtd->mccp_buf;

				tintin_puts2(ses, "");
				tintin_puts2(ses, "#COMPRESSION END, DISABLING MCCP.");
				inflateEnd(ses->mccp);
				free(ses->mccp);
				ses->mccp = NULL;
				break;

			default:
				tintin_puts2(ses, "");
				tintin_puts2(ses, "#COMPRESSION ERROR, DISABLING MCCP.");
				send_dont_mccp2(ses, 0, NULL);
				inflateEnd(ses->mccp);
				free(ses->mccp);
				ses->mccp = NULL;
				cpsrc = src;
				cplen = 0;
				break;
		}
	}
	else
	{
		cpsrc = src;
	}

	if (ses->read_len + cplen >= ses->read_max)
	{
		ses->read_max = ses->read_len + cplen + 1000;
		ses->read_buf = (unsigned char *) realloc(ses->read_buf, ses->read_max);
	}

	memcpy(ses->read_buf + ses->read_len, cpsrc, cplen);

	cpsrc = ses->read_buf;


	if (gtd->mud_output_len + cplen >= gtd->mud_output_max)
	{
		gtd->mud_output_max = gtd->mud_output_len + cplen + 1000;
		gtd->mud_output_buf = (char *) realloc(gtd->mud_output_buf, gtd->mud_output_max);
	}

	cpdst = (unsigned char *) gtd->mud_output_buf + gtd->mud_output_len;


	if (HAS_BIT(ses->flags, SES_FLAG_LOGLEVEL) && ses->logfile)
	{
		fwrite(cpsrc, 1, cplen, ses->logfile);
		fflush(ses->logfile);
	}

	while (cplen > 0)
	{
		if (*cpsrc == IAC)
		{
			skip = 2;

			if (HAS_BIT(ses->telopts, TELOPT_FLAG_DEBUG))
			{
				switch(cpsrc[1])
				{
					case NOP:   
					case DM:
					case BREAK: 
					case IP:    
					case AO:    
					case AYT:   
					case EC:    
					case EL:
					case IAC:
					case GA:
					case EOR:
						tintin_printf2(ses, "RCVD IAC %s", TELCMD(cpsrc[1]));
						break;

					case DO:
					case DONT:
					case WILL:
					case WONT:
						if (cplen > 2)
						{
							tintin_printf2(ses, "RCVD IAC %s %s", TELCMD(cpsrc[1]), telopt_table[cpsrc[2]].name);
						}
						else
						{
							tintin_printf2(ses, "RCVD IAC %s %d (BAD TELOPT)", TELCMD(cpsrc[1]), cpsrc[2]);
						}
						break;

					case SB:
						if (cplen > 2)
						{
							tintin_printf2(ses, "RCVD IAC SB %s", telopt_table[cpsrc[2]].name);
						}
						break;

					default:
						if (TELCMD_OK(cpsrc[1]))
						{
							tintin_printf2(ses, "RCVD IAC %s %d", TELCMD(cpsrc[1]), cpsrc[2]);
						}
						else
						{
							tintin_printf2(ses, "RCVD IAC %d %d", cpsrc[1], cpsrc[2]);
						}
						break;
				}
			}

			for (cnt = 0 ; iac_table[cnt].code ; cnt++)
			{
				if (cplen < iac_table[cnt].size && !memcmp(cpsrc, iac_table[cnt].code, cplen))
				{
					skip = iac_table[cnt].size;

					break;
				}

				if (cplen >= iac_table[cnt].size && !memcmp(cpsrc, iac_table[cnt].code, iac_table[cnt].size))
				{
					skip = iac_table[cnt].func(ses, cplen, cpsrc);

					if (iac_table[cnt].func == init_mccp)
					{
						return translate_telopts(ses, cpsrc + skip, cplen - skip);
					}
					break;
				}
			}

			if (iac_table[cnt].code == NULL && cplen > 1)
			{
				switch (cpsrc[1])
				{
					case SE:
					case NOP:
					case DM:
					case BREAK:
					case IP:
					case AO:
					case AYT:
					case EC:
					case EL:
					case GA:
					case EOR:
					     skip = 2;
					     break;

					case WILL:
						if (cplen > 2)
						{
							if (!check_all_events(ses, 1, 0, "IAC WILL %s", telopt_table[cpsrc[2]].name))
							{
								if (!HAS_BIT(ses->telopt_flag[cpsrc[2] / 32], 1 << cpsrc[2] % 32))
								{
									if (telopt_table[cpsrc[2]].want)
									{
										skip = send_do_telopt(ses, cplen, cpsrc);
									}
									else
									{
										skip = send_dont_telopt(ses, cplen, cpsrc);
									}
									SET_BIT(ses->telopt_flag[cpsrc[2] / 32], 1 << cpsrc[2] % 32);
								}
							}
						}
						skip = 3;
						break;

					case DO:
						if (cplen > 2)
						{
							if (!check_all_events(ses, 1, 0, "IAC DO %s", telopt_table[cpsrc[2]].name))
							{
								if (!HAS_BIT(ses->telopt_flag[cpsrc[2] / 32], 1 << cpsrc[2] % 32))
								{
									if (telopt_table[cpsrc[2]].want)
									{
										skip = send_will_telopt(ses, cplen, cpsrc);
									}
									else
									{
										skip = send_wont_telopt(ses, cplen, cpsrc);
									}
									SET_BIT(ses->telopt_flag[cpsrc[2] / 32], 1 << cpsrc[2] % 32);
								}
							}
						}
						skip = 3;
						break;

					case WONT:
					case DONT:
						if (cplen > 2)
						{
							check_all_events(ses, 2, 0, "IAC %s %s", TELCMD(cpsrc[1]), telopt_table[cpsrc[2]].name);
						}
						skip = 3;
						break;

					case SB:
						skip = recv_sb(ses, cplen, cpsrc);
						break;

					case IAC:
						gtd->mud_output_len++;
						*cpdst++ = 0xFF;
						skip = 2;
						break;

					default:
						tintin_printf(NULL, "#IAC BAD TELOPT %d", cpsrc[1]);
						skip = 1;
						break;
				}
			}
			if (skip <= cplen)
			{
				cplen -= skip;
				cpsrc += skip;
			}
			else
			{
				memcpy(ses->read_buf, cpsrc, cplen);

				gtd->mud_output_buf[gtd->mud_output_len] = 0;

				return cplen;
			}
		}
#ifdef BIG5
		else if (*cpsrc & 0x80)
		{
			*cpdst++ = *cpsrc++;
			gtd->mud_output_len++;
			cplen--;

			if (cplen)
			{
				*cpdst++ = *cpsrc++;
				gtd->mud_output_len++;
				cplen--;
			}
		}
#endif
		else
		{
			/*
				skip '\0' and '\r' in text input
			*/

			switch (*cpsrc)
			{
				case '\0':
					cpsrc++;
					cplen--;
					continue;

				case '\r':
					if (cplen < 2 || cpsrc[1] != 0)
					{
						cpsrc++;
						cplen--;
						continue;
					}
					break;

				case '\n':
					if (HAS_BIT(ses->telopts, TELOPT_FLAG_PROMPT))
					{
						DEL_BIT(ses->telopts, TELOPT_FLAG_PROMPT);
					}

					*cpdst++ = *cpsrc++;
					gtd->mud_output_len++;
					cplen--;

					while (*cpsrc == '\r')
					{
						cpsrc++;
						cplen--;
					}
					continue;

				default:
					if (HAS_BIT(ses->telopts, TELOPT_FLAG_PROMPT))
					{
						DEL_BIT(ses->telopts, TELOPT_FLAG_PROMPT);

						/*
							Fix up non vt muds
						*/

						if (HAS_BIT(ses->flags, SES_FLAG_SPLIT) || !IS_SPLIT(ses))
						{
							*cpdst++ = '\n';
							gtd->mud_output_len++;
						}
					}
					break;
			}
			*cpdst++ = *cpsrc++;
			gtd->mud_output_len++;
			cplen--;
		}
	}

	gtd->mud_output_buf[gtd->mud_output_len] = 0;

	return 0;
}

/*
	SGA
*/

int send_do_sga(struct session *ses, int cplen, unsigned char *cpsrc)
{
	SET_BIT(ses->telopts, TELOPT_FLAG_SGA);

	if (!HAS_BIT(ses->telopt_flag[TELOPT_SGA / 32], 1 << TELOPT_SGA % 32))
	{
		SET_BIT(ses->telopt_flag[TELOPT_SGA / 32], 1 << TELOPT_SGA % 32);

		socket_printf(ses, 3, "%c%c%c", IAC, DO, TELOPT_SGA);

		telopt_debug(ses, "SENT IAC DO %s", telopt_table[TELOPT_SGA].name);
	}
	return 3;
}

int send_will_sga(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (!HAS_BIT(ses->telopt_flag[TELOPT_SGA / 32], 1 << TELOPT_SGA % 32))
	{
		SET_BIT(ses->telopt_flag[TELOPT_SGA / 32], 1 << TELOPT_SGA % 32);

		socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_SGA);

		telopt_debug(ses, "SENT IAC WILL %s", telopt_table[TELOPT_SGA].name);
	}
	return 3;
}

int mark_prompt(struct session *ses, int cplen, unsigned char *cpsrc)
{
	SET_BIT(ses->telopts, TELOPT_FLAG_PROMPT);

	return 2;
}

/*
	TTYPE
*/

int recv_sb_ttype(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (!check_all_events(ses, 0, 1, "IAC SB TERMINAL TYPE", ntos(cpsrc[3])))
	{        
		if (HAS_BIT(ses->telopts, TELOPT_FLAG_TSPEED) && atoi(ses->port) == TELNET_PORT && getenv("TERM"))
		{
			socket_printf(ses, 6 + strlen(getenv("TERM")), "%c%c%c%c%s%c%c", IAC, SB, TELOPT_TTYPE, 0, getenv("TERM"), IAC, SE);

			telopt_debug(ses, "SENT IAC SB TTYPE %s", getenv("TERM"));
		}
		else
		{
			socket_printf(ses, 14, "%c%c%c%c%s%c%c", IAC, SB, TELOPT_TTYPE, 0, "TINTIN++", IAC, SE);

			telopt_debug(ses, "SENT IAC SB TTYPE %s", "TINTIN++");
		}
	}
	return 6;
}


/*
	TSPEED
*/

int recv_sb_tspeed(struct session *ses, int cplen, unsigned char *cpsrc)
{
	SET_BIT(ses->telopts, TELOPT_FLAG_TSPEED);

	if (!check_all_events(ses, 0, 1, "IAC SB TSPEED", ntos(cpsrc[3])))
	{
		socket_printf(ses, 17, "%c%c%c%c%s%c%c", IAC, SB, TELOPT_TSPEED, 0, "38400,38400", IAC, SE);

		telopt_debug(ses, "SENT IAC SB 0 %s 38400,38400 IAC SB", telopt_table[TELOPT_TSPEED].name);
	}
	return 6;
}


/*
	NAWS
*/

int send_will_naws(struct session *ses, int cplen, unsigned char *cpsrc)
{
	SET_BIT(ses->telopt_flag[TELOPT_NAWS / 32], 1 << TELOPT_NAWS % 32);

	socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_NAWS);

	telopt_debug(ses, "SENT IAC WILL NAWS");

	return 3;
}

int send_sb_naws(struct session *ses, int cplen, unsigned char *cpsrc)
{
	int rows;

	if (!HAS_BIT(ses->telopt_flag[TELOPT_NAWS / 32], 1 << TELOPT_NAWS % 32))
	{
		send_will_naws(ses, cplen, cpsrc);
	}

	rows = HAS_BIT(ses->flags, SES_FLAG_SPLIT) ? ses->bot_row - ses->top_row + 1 : ses->rows;

	socket_printf(ses, 9, "%c%c%c%c%c%c%c%c%c", IAC, SB, TELOPT_NAWS, ses->cols / 256, ses->cols % 256, rows / 256, rows % 256, IAC, SE);

	SET_BIT(ses->telopts, TELOPT_FLAG_NAWS);

	telopt_debug(ses, "SENT IAC SB NAWS %d %d %d %d", ses->cols / 256, ses->cols % 256, ses->rows / 256, ses->rows % 256);

	return 3;
}

/*
	ECHO
*/

int send_echo_on(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DONT, TELOPT_ECHO);

	SET_BIT(ses->telopts, TELOPT_FLAG_ECHO);

	telopt_debug(ses, "SENT IAC DONT ECHO");

	return 3;
}

int send_echo_off(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DO, TELOPT_ECHO);

	DEL_BIT(ses->telopts, TELOPT_FLAG_ECHO);

	telopt_debug(ses, "SENT IAC DO ECHO");

	return 3;
}

int send_echo_will(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_ECHO);

	DEL_BIT(ses->telopts, TELOPT_FLAG_ECHO);

	telopt_debug(ses, "SENT IAC WILL ECHO");

	return 3;
}

/*
	IP
*/

int send_ip(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 5, "%c%c%c%c%c", IAC, IP, IAC, DO, TELOPT_TIMINGMARK);

	telopt_debug(ses, "SENT IAC IP");
	telopt_debug(ses, "SENT IAC DO TIMING MARK");

	return 3;
}

/*
	Automatic telopt handling
*/

int send_wont_telopt(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WONT, cpsrc[2]);

	telopt_debug(ses, "SENT IAC WONT %s", telopt_table[cpsrc[2]].name);

	return 3;
}

int send_dont_telopt(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DONT, cpsrc[2]);

	telopt_debug(ses, "SENT IAC DONT %s", telopt_table[cpsrc[2]].name);

	return 3;
}

int send_will_telopt(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WILL, cpsrc[2]);

	telopt_debug(ses, "SENT IAC WILL %s", telopt_table[cpsrc[2]].name);

	return 3;
}

int send_do_telopt(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DO, cpsrc[2]);

	telopt_debug(ses, "SENT IAC DO %s", telopt_table[cpsrc[2]].name);

	return 3;
}

/*
	MSSP (Mud Server Status Protocol)
*/

int send_do_mssp(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (!check_all_events(ses, 0, 0, "IAC WILL MSSP"))
	{
		if (HAS_BIT(ses->telopts, TELOPT_FLAG_DEBUG))
		{
			socket_printf(ses, 3, "%c%c%c", IAC, DO, TELOPT_MSSP);

			telopt_debug(ses, "SENT IAC DO MSSP");
		}
	}
	return 3;
}

int recv_sb_mssp(struct session *ses, int cplen, unsigned char *src)
{
	char var[BUFFER_SIZE], val[BUFFER_SIZE];
	char *pto;
	int i;

	var[0] = val[0] = i = 0;

	if (skip_sb(ses, cplen, src) > cplen)
	{
		return cplen + 1;
	}

	while (i < cplen && src[i] != SE)
	{
		switch (src[i])
		{
			case MSSP_VAR:
				i++;
				pto = var;

				while (i < cplen && src[i] >= 3 && src[i] != IAC)
				{
					*pto++ = src[i++];
				}
				*pto = 0;
				break;

			case MSSP_VAL:
				i++;
				pto = val;

				while (i < cplen && src[i] >= 3 && src[i] != IAC)
				{
					*pto++ = src[i++];
				}
				*pto = 0;

				telopt_debug(ses, "MSSP VAR %-20s VAL %s", var, val);

				check_all_events(ses, 0, 2, "IAC SB MSSP", var, val);
				break;

			default:
				i++;
				break;
		}
	}

	telopt_debug(ses, "IAC SB MSSP IAC SE");

	check_all_events(ses, 0, 0, "IAC SB MSSP IAC SE");

	return UMIN(i + 1, cplen);
}

/*
	MSDP (Mud Server Data Protocol)
*/

int recv_sb_msdp(struct session *ses, int cplen, unsigned char *src)
{
	char var[BUFFER_SIZE], val[BUFFER_SIZE];
	char *pto;
	int i;

	var[0] = val[0] = i = 0;

	if (skip_sb(ses, cplen, src) > cplen)
	{
		return cplen + 1;
	}

	while (i < cplen && src[i] != SE)
	{
		switch (src[i])
		{
			case MSSP_VAR:
				i++;
				pto = var;

				while (i < cplen && src[i] >= 3 && src[i] != IAC)
				{
					*pto++ = src[i++];
				}
				*pto = 0;
				break;

			case MSSP_VAL:
				i++;
				pto = val;

				while (i < cplen && src[i] >= 3 && src[i] != IAC)
				{
					*pto++ = src[i++];
				}
				*pto = 0;

				telopt_debug(ses, "IAC SB MSDP (VAR %-20s VAL %s)", var, val);

				check_all_events(ses, 0, 2, "IAC SB MSDP", var, val);
				break;

			default:
				i++;
				break;
		}
	}

	telopt_debug(ses, "IAC SB MSDP IAC SE");

	check_all_events(ses, 0, 0, "IAC SB MSDP IAC SE");
	return UMIN(i + 1, cplen);
}

/*
	NEW-ENVIRON
*/

int recv_sb_new_environ(struct session *ses, int cplen, unsigned char *src)
{
	char var[BUFFER_SIZE], val[BUFFER_SIZE];
	char *pto;
	int i, x;

	var[0] = val[0] = 0;

	if (skip_sb(ses, cplen, src) > cplen)
	{
		return cplen + 1;
	}

	telopt_debug(ses, "IAC SB NEW-ENVIRON %d %d", src[3], src[4]);

	i = 4;
	x = -1;

	while (i < cplen && src[i] != SE)
	{
		switch (src[i])
		{
			case ENV_VAR:
			case ENV_USR:
				x = src[i];
				i++;
				pto = var;

				while (i < cplen && src[i] >= 4 && src[i] != IAC)
				{
					*pto++ = src[i++];
				}
				*pto = 0;

				if (src[3] == ENV_SEND)
				{
					telopt_debug(ses, "IAC SB NEW-ENVIRON SEND %s", x ? "VAR" : "USR");

					check_all_events(ses, 1, 2, "IAC SB NEW-ENVIRON SEND %s", x ? "VAR" : "USR", var, "");
				}
				break;

			case ENV_VAL:
				i++;
				pto = val;

				while (i < cplen && src[i] >= 3 && src[i] != IAC)
				{
					*pto++ = src[i++];
				}
				*pto = 0;

				telopt_debug(ses, "IAC SB NEW-ENVIRON %s %s", src[3] ? "IS" : "INFO", x ? "VAR" : "USR");

				check_all_events(ses, 2, 2, "IAC SB NEW-ENVIRON %s %s", src[i] ? "IS" : "INFO", x ? "VAR" : "USR", var, val);
				break;

			case IAC:
				if (x == -1)
				{
					telopt_debug(ses, "IAC SB NEW-ENVIRON %s", src[3] == 0 ? "IS" : src[3] == 1 ? "SEND" : "INFO");

					check_all_events(ses, 1, 2, "IAC SB NEW-ENVIRON %s", src[3] == 0 ? "IS" : src[3] == 1 ? "SEND" : "INFO", var, val);
				}
				i++;
				break;
			default:
				i++;
				break;
		}
	}

	telopt_debug(ses, "IAC SB NEW-ENVIRON IAC SE");

	check_all_events(ses, 0, 0, "IAC SB NEW-ENVIRON IAC SE");

	return i + 1;
}

int recv_sb_zmp(struct session *ses, int cplen, unsigned char *src)
{
	char var[BUFFER_SIZE], val[BUFFER_SIZE];
	char *pto;
	int i, x;

	var[0] = val[0] = x = 0;

	if (skip_sb(ses, cplen, src) > cplen)
	{
		return cplen + 1;
	}

	i = 3;

	while (i < cplen && src[i] != SE)
	{
		switch (src[i])
		{
			case IAC:
				i++;
				break;

			default:
				pto = x ? val : var;

				while (i < cplen && src[i])
				{
					*pto++ = src[i++];
				}
				*pto = src[i++];

				if (x++)
				{
					telopt_debug(ses, "IAC SB ZMP %s", var);

					check_all_events(ses, 1, 1, "IAC SB ZMP %s", var, val);
				}
				break;
		}
	}

	telopt_debug(ses, "IAC SB ZMP %s IAC SE", var);

	check_all_events(ses, 1, 0, "IAC SB ZMP %s IAC SE", var);

	return UMIN(i + 1, cplen);
}


/*
	MCCP
*/

int send_do_mccp2(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (HAS_BIT(ses->flags, SES_FLAG_MCCP))
	{
		socket_printf(ses, 3, "%c%c%c", IAC, DO, TELOPT_MCCP2);

		telopt_debug(ses, "SENT IAC DO MCCP2");
	}
	else
	{
		socket_printf(ses, 3, "%c%c%c", IAC, WONT, TELOPT_MCCP2);

		telopt_debug(ses, "SENT IAC WONT MCCP2 (MCCP HAS BEEN DISABLED)");
	}
	return 3;
}

int send_dont_mccp2(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DONT, TELOPT_MCCP2);

	telopt_debug(ses, "SENT DONT MCCP2");

	return 3;
}


int init_mccp(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (ses->mccp)
	{
		return 5;
	}

	ses->mccp = (z_stream *) calloc(1, sizeof(z_stream));

	ses->mccp->data_type = Z_ASCII;
	ses->mccp->zalloc    = zlib_alloc;
	ses->mccp->zfree     = zlib_free;
	ses->mccp->opaque    = NULL;

	if (inflateInit(ses->mccp) != Z_OK)
	{
		tintin_puts2(ses, "#FAILED TO INITIALIZE MCCP.");
		send_dont_mccp2(ses, 0, NULL);
		free(ses->mccp);
		ses->mccp = NULL;
	}
	else
	{
		telopt_debug(ses, "MCCP INITIALIZED.");
	}
	return 5;
}


void *zlib_alloc(void *opaque, unsigned int items, unsigned int size)
{
	return calloc(items, size);
}


void zlib_free(void *opaque, void *address)
{
	free(address);
}

void init_telnet_session(struct session *ses)
{
//	send_do_sga(ses, 0, NULL);
//	send_will_naws(ses, 0, NULL);
}


/*
	Returns the length of a telnet subnegotiation
*/

int skip_sb(struct session *ses, int cplen, unsigned char *cpsrc)
{
	int i;

	for (i = 1 ; i < cplen ; i++)
	{
		if (cpsrc[i] == SE && cpsrc[i-1] == IAC)
		{
			return i + 1;
		}
	}

	return cplen + 1;
}

int recv_sb(struct session *ses, int cplen, unsigned char *cpsrc)
{
	char *pt1, *pt2, var1[BUFFER_SIZE], var2[BUFFER_SIZE];
	int i;

	if (skip_sb(ses, cplen, cpsrc) > cplen)
	{
		return cplen + 1;
	}

	pt1 = var1;
	pt2 = var2;

	for (i = 3 ; i < cplen ; i++)
	{
		if (cpsrc[i] == IAC && i + 1 < cplen && cpsrc[i+1] == SE)
		{
			break;
		}
		else
		{
			*pt1++ = cpsrc[i];

			sprintf(pt2, "%03d ", cpsrc[i]);

			pt2 += 4;
		}
	}

	*pt1 = 0;
	*pt2 = 0;

	check_all_events(ses, 1, 2, "IAC SB %s", telopt_table[cpsrc[2]].name, var1, var2);

	return i + 2;
}
