/*
 * Copyright 2020-2024, 2026 NXP
 *
 * NXP Proprietary
 *
 * This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or
 * by downloading, installing, activating and/or otherwise using the software, you are
 * agreeing that you have read, and that you agree to comply with and are bound by,
 * such license terms. If you do not agree to be bound by the applicable license terms,
 * then you may not retain, install, activate or otherwise use the software.
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cmdparse.h"
#include "wireless_ranging_cli.h"

#ifndef CMDPARSE_MAX_ARGS
#define CMDPARSE_MAX_ARGS 16
#endif

void cmdparse_worker(int argc, char **argv, const command_t *tab);

/*******************************************************************
 * Execute show command (recursive function)
 *******************************************************************/
static void cmdparse_show(const command_t *tab)
{
    int i;
    for (i = 0; tab[i].syntax != 0; i++) {
        if ((tab[i].flags & eFlagCommandList) && (tab[i].child != NULL)) {
            cmdparse_show(tab[i].child);
        } else if ((tab[i].flags & eFlagRead) && (tab[i].funcptr != NULL)) {
            ((cmd_func_t)tab[i].funcptr)(0, 0);
        }
        else
        {
            /* No action needed */
        }
    }
}

/*******************************************************************
 *
 *******************************************************************/
void cmdparse_worker(int argc, char **argv, const command_t *tab)
{

    int i;
    if(argc == 0 || strncmp(argv[0], "help", 4) == 0)
    {
        printf("-----------commands-------------\n");
        for (i = 0; tab[i].syntax != 0; i++) {
            if(!(tab[i].flags & eFlagHidden)) {
                printf("%s\n", tab[i].syntax);
            }
        }
        printf("-------------end----------------\n");
        return;
    }
    else if(strncmp(argv[0], "show", 4) == 0)
    {
        printf("-----------values---------------\n");
        cmdparse_show(tab);
        printf("-------------end----------------\n");
        return;
    }
    else
    {
        /* No action needed */
    }

    for (i = 0; tab[i].syntax != 0; i++) {
        if (strncmp(tab[i].syntax, argv[0], strlen(argv[0])) == 0) {
            if(tab[i].flags & eFlagCommandList)
            {
                cmdparse_worker(argc - 1, (char**)&argv[1], (const command_t *)tab[i].child);
            }
            else if((tab[i].flags & (eFlagRead | eFlagArgsOpt)) ||
                    ((tab[i].flags & eFlag1Args) && argc >= 2) ||
                    ((tab[i].flags & eFlag2Args) && argc >= 3) ||
                    ((tab[i].flags & eFlag3Args) && argc >= 4) )
            {
                ((cmd_func_t)tab[i].funcptr)(argc - 1, (char**)&argv[1]);
            }
            else
            {
                printf("too few arguments. Command syntax: %s\n", tab[i].syntax);
            }
            return;
        }
    }
    printf("command not found\n");
    return;
}

/*******************************************************************
 * split line in arguments separated by whitespace
 *******************************************************************/
void cmdparse(char * cmdline, const command_t *tab) {

    char *argv[CMDPARSE_MAX_ARGS];
    int argc = 0;
    char *cp = cmdline;

    while (*cp) {
        while (*cp && isspace((int)*cp)) /* remove leading whitespace if any */
        {
            cp++;
        }
        if (*cp == 0)/* no more arguments */
        {
            break;
        }
        argv[argc++] = cp;
        while (*cp && !isspace((int)*cp)) /* find end of argument */
        {
            cp++;
        }
        if (*cp == 0 || argc == CMDPARSE_MAX_ARGS) /* end of command line */
        {
            break;
        }
        *cp = 0;
        cp++;
    }
    cmdparse_worker(argc, argv, tab);
}

/* end */

