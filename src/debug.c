#include <stdio.h>

#include "debug.h"

bool debugMode = false;
bool debugFlagOutOfScreen = false;

void print_debug_info(int triToRasterCount, double deltaTime)
{
	if (debugMode) {
		printf("Rendering %d triangles - dt: %lf - FPS: %d\n", triToRasterCount, deltaTime, (int) (1000 / deltaTime));
	}
}
