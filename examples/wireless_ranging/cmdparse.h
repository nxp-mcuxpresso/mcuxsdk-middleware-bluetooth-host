/*
 * Copyright 2020-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CMDPARSE_H_
#define CMDPARSE_H_

typedef void (*cmd_func_t)(int, char**);

typedef struct command_tag {
    const char *syntax;                   /* command syntax: <name> [<arguments>] | description */
    union {
        void (*funcptr)(int, char**);     /* pointer to function */
        struct command_tag const *child;  /* pointer to sub command list */
    };
    const int flags;                      /* see parse_flags_t */

} command_t;

typedef enum {
    eFlag1Args = 0x01,      /* 1 argument */
    eFlag2Args = 0x02,      /* 2 arguments */
    eFlag3Args = 0x04,      /* 3 arguments */
    eFlagArgsOpt = 0x08,    /* arguments are optional */
    eFlagRead = 0x10,       /* current value can be shown, also implies that command arguments are optional */
    eFlagHidden = 0x20,     /* command hidden from help menu */
    eFlagCommandList = 0x40,
} parse_flags_t;

#ifdef __cplusplus
extern "C" {
#endif

void cmdparse(char * cmdline, const command_t *tab);

#ifdef __cplusplus
}
#endif

#endif /* CMDPARSE_H_ */
