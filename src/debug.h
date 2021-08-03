#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>


bool debugMode;
bool debugFlagOutOfScreen;


void print_debug_info(int triToRasterCount, double deltaTime);


#endif