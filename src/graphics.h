#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "vector.h"
#include "util.h"


#define FRAME_LIMIT_FPS       60
#define FRAME_LIMIT_MS        (1000 / FRAME_LIMIT_FPS)

#define ROTATION_SPEED        0.001
#define Z_OFFSET              5.0
#define FOV                   90.0

#define DEBUG_WIREFRAME_COLOR ((color) {0, 255, 0, 255})

typedef struct {
	Uint8 r, g, b, a;
} color;

typedef struct {
	unsigned int *pixelsArr;
	int width;
	int height;
	SDL_PixelFormat *format;
} screen;

typedef struct {
	vector3d p[3];
	color color;
} triangle;

typedef struct {
	triangle *tris;
	int triCount;
} mesh;


bool  limitFrameRate;
color backgroundColor;


void load_object(mesh *mesh, char *fileName);
void draw_pixel(screen *, int x, int y, color);
void draw_line(screen *, int x0, int y0, int x1, int y1, color);
void draw_triangle(screen *, triangle, color);
void draw_line_horizontal(screen *, int startx, int endx, int consty, color);
void fill_triangle(screen *, triangle, color);
int  triangle_compare_z(const void *t1, const void *t2);
void draw_rect(screen *, int xstart, int ystart, int xend, int yend, color);
void screen_fill(screen *, color);
void screen_clear(screen *);
triangle triangle_new();
void cap_framerate(double deltaTime);


#endif