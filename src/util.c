#include "util.h"

create_swap_func(int)
create_max_func(double)

// Reads a line from file pointer. Returns the line and its length via pass by reference.
// Returns  0 if it successfully read a line.
// Returns -1 if it could not (EOF).
// Returns -2 if it could not ('\n').
int get_line(char **lineptr, size_t *len, FILE *objFile)
{
	int returnVal = 0; // success initially

	// Copy a line from file into the buffer
	int line_len = 128;
	int curr_length = 0;

	char *buff = (char*) malloc(line_len * sizeof(char)); // line buffer
	char *pc = buff;

	while (1) {
		char ch = fgetc(objFile);

		if (ch == '\n') {
			returnVal = -2;
			break;
		} else if (ch == EOF) {
			returnVal = -1;
			break;
		}

		*pc++ = ch;
		curr_length++;

		if ((curr_length + 1) == line_len) {
			line_len *= 2;
			buff = realloc(buff, line_len * sizeof(char));
		}
	}

	// If a line was read, return value isn't actually -2 or -1
	if (curr_length != 0)
		returnVal = 0;

	// Put '\0' at the end
	*pc = '\0';
	curr_length++;

	// We didn't necessarily use all the allocated memory in the first buffer
	// Construct a proper return string with actualy length
	char *resultStr = (char*) malloc(curr_length * sizeof(char));

	char *from = buff;
	char *to = resultStr;
	for (int i = 0; i < curr_length; ++i)
		*to++ = *from++;

	free(buff);

	*lineptr = resultStr;
	*len = curr_length;

	return returnVal;
}
