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

int main(int argc, char** argv) {
    get_options(argc, argv);

    if (cmd_args.help)
        usage();

    return 0;
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
