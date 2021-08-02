#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

#define VERSION "v0.1"

bool quit;

void handle_args(int argc, char **argv);

#endif