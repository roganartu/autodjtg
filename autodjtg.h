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
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    int help;
    int clean;
} CmdArgs;
CmdArgs cmd_args;

typedef struct {
    char* id;
    char* file;
    char* index;
} Device;

pid_t childPid;

void usage(void);

int pipe_and_fork(void);

void get_options(int argc, char** argv);

void get_device_id(Device *device);

void get_device_index(Device *device);

void get_file_path(Device *device);

void program_device(Device *device);
