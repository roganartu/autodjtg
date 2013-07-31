/*
 * =============================================================================
 *
 *       Filename:  autodjtg.h
 *
 *    Description:  Main autodjtg header file
 *
 *        Version:  0.0.1
 *        Created:  31/07/13 12:52:38
 *
 *         Author:  Tony Lee (Roganartu), github@roganartu.com
 *
 *          Notes:  
 * =============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int help;
    int clean;
} CmdArgs;
CmdArgs cmd_args;

void get_options(int argc, char** argv);

void usage();
