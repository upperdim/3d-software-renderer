#include <string.h>

#include "cli.h"
#include "debug.h"
#include "graphics.h"

bool quit = false;

static void print_version()
{
	printf("3D Software Renderer %s - Author: github.com/upperdim\n", VERSION);
}

static void print_help()
{
	print_version();
	putchar('\n');
	
	printf("Arguments:\n");
	printf("  -h, -H, --help       Prints this hint.\n");
	printf("  -v, -V, --version    Prints the version of the binary.\n");
	printf("  -d, -D, --debug      Runs the binary in debugging mode. Prints\n"
	       "                           time difference and framerate. \n"
	       "                           Indicates upon window resizing\n"
	       "                           and frame limiter toggle. Draws\n"
	       "                           the mesh with wireframes.\n");
	printf("  -l, -L, --limit      Limits framerate to 60.\n");
	putchar('\n');
}

void handle_args(int argc, char **argv)
{
	for (int i = 1; i < argc; ++i) {
		if      (strcmp("--help",    argv[i]) == 0) {print_help();    quit = true;}
		else if (strcmp("--version", argv[i]) == 0) {print_version(); quit = true;}
		else if (strcmp("--limit",   argv[i]) == 0) {limitFrameRate = true;}
		else if (strcmp("--debug",   argv[i]) == 0) {debugMode      = true;}
		else if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'h':
			case 'H':
				print_help();
				quit = true;
				break;
			case 'v':
			case 'V':
				print_version();
				quit = true;
				break;
			case 'd':
			case 'D':
				debugMode = true;
				break;
			case 'l':
			case 'L':
				limitFrameRate = true;
				break;
			default:
				printf("%s: Unrecognized option '%s'\nTry '%s --help' for more information.\n", argv[0], argv[i], argv[0]);
				quit = true;;
			}
		} else {
			printf("%s: Unrecognized option '%s'\nTry '%s --help' for more information.\n", argv[0], argv[i], argv[0]);
			quit = true;;
		}
	}
}
