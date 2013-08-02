/*
 * =============================================================================
 *
 *       Filename:  autodjtg.c
 *
 *    Description:  Main autodjtg file
 *
 *        Version:  0.0.1
 *        Created:  31/07/13 12:51:12
 *
 *         Author:  Tony Lee (Roganartu), github@roganartu.com
 *
 *          Notes:  
 * =============================================================================
 */
#include "autodjtg.h"

int retcode;

int main(int argc, char** argv) {
    Device* device;

    retcode = 0;

    get_options(argc, argv);

    if (cmd_args.help)
        usage();

    device = (Device*) malloc(sizeof(Device));
    get_device_id(device);

    if (device->id == NULL) {
        retcode = 2;
        goto cleanup;
    }

    printf("Found USB Device: %s\n", device->id);

    get_device_index(device);

    if (device->index < 0) {
        retcode = 3;
        goto cleanup;
    }

    get_file_path(device);

cleanup:
    free(device->id);
    free(device->file);
    free(device);
    return retcode;
}

/* 
 * ===  FUNCTION  ==============================================================
 *         Name:  usage
 *
 *  Description:  Prints a usage message and exits with status 1
 * 
 *      Version:  0.0.1
 *       Params:  void
 *      Returns:  void
 *        Usage:  usage()
 *      Outputs:  Usage message
 *
 *        Notes:  
 * =============================================================================
 */
void usage() {
    char* usage = "Usage:\n\
       autodjtg [-h]\n\
   \n\
   This tool automates the usage of djtgcfg from Digilent.\n\
   Currently does not support file paths as arguments.\n\
   In the meantime just run the tool from the directory\n\
   containing the .bit file you wish to program.";

    printf("%s\n", usage);

    exit(1);
}

/* 
 * ===  FUNCTION  ==============================================================
 *         Name:  get_options
 *
 *  Description:  Parse commandline options into a global struct
 * 
 *      Version:  0.0.1
 *       Params:  int argc
 *                char** argv
 *      Returns:  void
 *        Usage:  get_options( int argc, char** argv )
 *      Outputs:  N/A
 *
 *        Notes:  
 * =============================================================================
 */
void get_options(int argc, char** argv) {
    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 ||
                strcmp(argv[i], "-h") == 0)
            cmd_args.help = 1;
    }

    // Currently --help and -h are the only supported arguments
    if (argc > 2 || (argc == 2 && !cmd_args.help))
        cmd_args.help = 1;
}

/* 
 * ===  FUNCTION  ==============================================================
 *         Name:  get_device_id
 *
 *  Description:  Calls `djtgcfg enum` and looks for device username. Prompts
 *                for user input if more than one device. Exits with error if
 *                no devices found or djtgcfg is not installed.
 * 
 *      Version:  0.0.1
 *       Params:  Device *device
 *      Returns:  void
 *        Usage:  get_device_id(Device *device)
 *      Outputs:  N/A
 *
 *        Notes:  
 * =============================================================================
 */
void get_device_id(Device *device) {
    char *tmp;
    char* args[3];
    char output[80];
    char* combined;
    int pipe_id, exit_status, device_id_len;
    FILE *input;

    pipe_id = pipe_and_fork();

    if (childPid == 0) {
        // Child process
        args[0] = "djtgcfg";
        args[1] = "enum";
        args[2] = (char*) 0;
        execvp("djtgcfg", args);

        // Should never happen normally
        exit(999);
    } else {
        // Parent process
        input = fdopen(pipe_id, "r");
        combined = (char*) malloc(80);

        while (fgets(output, 80, input) != NULL) {
            combined = (char *) realloc(combined, strlen(combined) + 1 +
                    strlen(output));
            combined = strcat(combined, output);
        }

        wait(&exit_status);
        childPid = 0;
    }

    // No devices
    if (strstr(combined, "No devices found") != NULL) {
        printf("%s\n", "Failed to find device");
    } else {
        // Extract the device user name
        tmp = strstr(combined, "Device: ");
        device_id_len = strchr(tmp, '\n') - tmp - 8;
        device->id = (char *) malloc(device_id_len);
        strncpy(device->id, tmp + 8, device_id_len);
    }

    free(combined);
    close(pipe_id);
}

/* 
 * ===  FUNCTION  ==============================================================
 *         Name:  get_device_index
 *
 *  Description:  Calls `djtgcfg init` and looks for device indexes. Prompts
 *                for user input if more than one index. Exits with error if
 *                no devices found or djtgcfg is not installed.
 * 
 *      Version:  0.0.1
 *       Params:  Device *device
 *      Returns:  Device index from djtgcfg init
 *        Usage:  get_device_index(Device *device)
 *      Outputs:  N/A
 *
 *        Notes:  
 * =============================================================================
 */
void get_device_index(Device *device) {
    char* tmp, *cmp;
    char* args[5];
    char output[80];
    char* combined;
    int pipe_id, exit_status, i, response;
    FILE *input;

    pipe_id = pipe_and_fork();

    if (childPid == 0) {
        // Child process
        args[0] = "djtgcfg";
        args[1] = "init";
        args[2] = "-d";
        args[3] = device->id;
        args[4] = (char*) 0;
        execvp("djtgcfg", args);

        // Should never happen normally
        exit(999);
    } else {
        // Parent process
        input = fdopen(pipe_id, "r");
        combined = (char*) malloc(80);

        while (fgets(output, 80, input) != NULL) {
            combined = (char *) realloc(combined, strlen(combined) + 1 +
                    strlen(output));
            combined = strcat(combined, output);
        }

        wait(&exit_status);
        childPid = 0;
    }

    cmp = malloc(22);
    for (i = 1; i < 1000; i++) {
        sprintf(cmp, "Found %d device(s):", i);
        tmp = strstr(combined, cmp);
        if (tmp != NULL)
            break;
    }
    free(cmp);
    free(combined);
    close(pipe_id);

    if (tmp == NULL) {
        printf("%s\n", "No indexes found. Is device switched on?");
        exit(5);
    }

    printf("%s", tmp);

    response = -1;
    while (response < 0 || response > i - 1) {
        printf("Which device would you like to program?: [0] ");
        tmp = fgets(output, 80, stdin);
        response = atoi(output);
    }

    device->index = response;
}

/* 
 * ===  FUNCTION  ==============================================================
 *         Name:  get_file_path
 *
 *  Description:  Traverses the current directory for .bit files and prompts
 *                the user to choose which one to program the board with.
 * 
 *      Version:  0.0.1
 *       Params:  Device *device
 *      Returns:  void
 *        Usage:  get_file_path( Device *device )
 *      Outputs:  Prompts user to choose which file
 *
 *        Notes:  
 * =============================================================================
 */
void get_file_path(Device *device) {
    DIR           *d;
    struct dirent *dir;
    char cwd[1024], *bstring, output[80];
    char** files;
    int i, response;

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("%s\n", "Error getting cwd");
        exit(9);
    }

    files = (char **) malloc(sizeof(char *) * 1);

    i = 0;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                if ((bstring = strstr(dir->d_name, ".bit")) != NULL) {
                    if (strcmp(bstring, ".bit") == 0) {
                        files = (char **) realloc(files, sizeof(char *) * (i + 1));
                        files[i] = (char *) malloc(strlen(dir->d_name) + 1);
                        strcpy(files[i++], dir->d_name);
                    }
                }
            }
        }

        closedir(d);
    }

    if (i == 0) {
        printf("%s\n", "No .bit files found");
        free(files);
        exit(4);
    }

    // Prompt to choose file
    printf("Found %d file(s):\n", i);
    for (int x = 0; x < i; x++) {
        printf("    [%d] %s/%s\n", x, cwd, files[x]);
    }

    response = -1;
    while (response < 0 || response > i - 1) {
        printf("Which file would you like to program onto the device?: [0] ");
        if (fgets(output, 80, stdin) != NULL)
            response = atoi(output);
    }

    device->file = (char *) malloc(strlen(files[response]) + strlen(cwd) + 2);
    strcpy(device->file, cwd);
    strcat(device->file, "/");
    strcat(device->file, files[response]);

    // Free files array
    for (int x = 0; x < i; x++) {
        free(files[x]);
    }
    free(files);
}

/* 
 * ===  FUNCTION  =============================================================
 *         Name:  pipe_and_fork
 *
 *  Description:  Opens input pipe, forks and then closes parent/child pipes as
 *                necessary. Also sets up input/output redirection to
 *                stdin/stderr/stdout as necessary.
 *
 *      Version:  0.0.1
 *       Params:  void
 *      Returns:  void
 *        Usage:  pipe_and_fork()
 *      Outputs:  N/A
 * ============================================================================
 */
int pipe_and_fork() {
    int pipes[2];
    int output;
    pid_t pid;

    if (pipe(pipes) != 0) {
        //TODO throw_error(SYS_ERROR);
    }

    if ((pid = fork()) == -1) {
        //TODO throw_error(SYS_ERROR);
    }
    // Global var
    childPid = pid;

    if (pid == 0) {
        // Child closes input side
        close(pipes[0]);
        output = STDOUT_FILENO;
        if (dup2(pipes[1], output) == -1) {
            //TODO throw_error(SYS_ERROR);
        }

        return pipes[1];
    } else {
        // Parent closes output side
        close(pipes[1]);
        return pipes[0];
    }
}
