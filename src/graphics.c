#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "debug.h"

bool limitFrameRate = false;
color backgroundColor = {0, 0, 0, 255};


void load_object(mesh *mesh, char *fileName)
{
	FILE *objFile = fopen(fileName, "r");
	if (!objFile) {
		printf("Error! load_object(): could not open the file '%s'\n", fileName);
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	// Store points in an array
	vector3d *points = NULL;
	int pointCount = 0;

	char *line = NULL;
	size_t len = 0;
	while ((get_line(&line, &len, objFile) != -1)) {
		double x, y, z;

		if (line[0] == 'v' && sscanf(line, " v %lf %lf %lf ", &x, &y, &z)) {
			++pointCount;
			points = realloc(points, pointCount * sizeof(vector3d));

			points[pointCount - 1].x = x;
			points[pointCount - 1].y = y;
			points[pointCount - 1].z = z;
			points[pointCount - 1].w = VECTOR_DEFAULT_W;
		}
	}

	// Read points to the actual mesh accordingly
	int triangleCount = 0;

	rewind(objFile);
	line = NULL;
	len = 0;
	int maxLineLen = 0;
	while ( (get_line(&line, &len, objFile) != -1) ) {
		if (len > maxLineLen) {
			maxLineLen = len;
		}

		int point1index, point2index, point3index;
		if (line[0] == 'f' && sscanf(line, " f %d %d %d ", &point1index, &point2index, &point3index)) {
			// Erase texture and normal info
			for (int i = 0; i < maxLineLen; ++i) {
				if (line[i] == '/') {
					while (line[i] != ' ' && line[i] != '\n' && line[i] != '\0') {
						line[i++] = ' ';
					}
				}
			}

			++triangleCount;
			mesh->tris = realloc(mesh->tris, triangleCount * sizeof(triangle));
			mesh->triCount = triangleCount;

			// Read points from points array into the actual mesh
			mesh->tris[triangleCount - 1].p[0] = points[point1index - 1]; // obj file starts from line 1
			mesh->tris[triangleCount - 1].p[1] = points[point2index - 1];
			mesh->tris[triangleCount - 1].p[2] = points[point3index - 1];
		}
	}

	free(points);
}

void draw_pixel(screen *scr, int x, int y, color c)
{
	if (x < 0 || x > scr->width - 1 || y < 0 || y > scr->height - 1) {
		debugFlagOutOfScreen = true;
		return;
	}

	scr->pixelsArr[x + y * scr->width] = SDL_MapRGBA(scr->format, c.r, c.g, c.b, c.a);
}

void draw_line(screen *screen, int x0, int y0, int x1, int y1, color color)
{
	int isSteep = false;

	if (abs(x0 - x1) < abs(y0 - y1)) {
		swap_int(&x0, &y0);
		swap_int(&x1, &y1);

		isSteep = true;
	}

	if (x0 > x1) {
		swap_int(&x0, &x1);
		swap_int(&y0, &y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = abs(dy) * 2;
	int error2 = 0;
	int y = y0;

	for (int x = x0; x <= x1; ++x) {
		if (isSteep) {
			draw_pixel(screen, y, x, color);
		} else {
			draw_pixel(screen, x, y, color);
		}

		error2 += derror2;

		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

void draw_triangle(screen *s, triangle t, color c)
{
	draw_line(s, (int)t.p[0].x, (int)t.p[0].y, (int)t.p[1].x, (int)t.p[1].y, c);
	draw_line(s, (int)t.p[1].x, (int)t.p[1].y, (int)t.p[2].x, (int)t.p[2].y, c);
	draw_line(s, (int)t.p[2].x, (int)t.p[2].y, (int)t.p[0].x, (int)t.p[0].y, c);
}

void draw_line_horizontal(screen *scr, int startx, int endx, int consty, color c)
{
	for (int i = startx; i <= endx; ++i)
		draw_pixel(scr, i, consty, c);
}

// https://www.avrfreaks.net/sites/default/files/triangles.c
void fill_triangle(screen *scr, triangle t, color c)
{
	int x1 = t.p[0].x;	int y1 = t.p[0].y;
	int x2 = t.p[1].x;	int y2 = t.p[1].y;
	int x3 = t.p[2].x;	int y3 = t.p[2].y;

	int t1x, t2x, y, minx, maxx, t1xp, t2xp;
	int changed1 = false;
	int changed2 = false;
	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;
	// Sort vertices
	if (y1 > y2) { swap_int(&y1, &y2); swap_int(&x1, &x2); }
	if (y1 > y3) { swap_int(&y1, &y3); swap_int(&x1, &x3); }
	if (y2 > y3) { swap_int(&y2, &y3); swap_int(&x2, &x3); }

	t1x = t2x = x1; y = y1;   // Starting points
	dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
	else signx2 = 1;
	dy2 = (int)(y3 - y1);

	if (dy1 > dx1) {   // swap values
		swap_int(&dx1, &dy1);
		changed1 = true;
	}
	if (dy2 > dx2) {   // swap values
		swap_int(&dy2, &dx2);
		changed2 = true;
	}

	e2 = (int)(dx2 >> 1);
	// Flat top, just process the second half
	if (y1 == y2) goto next;
	e1 = (int)(dx1 >> 1);

	for (int i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			i++;
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) t1xp = signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
		// Move line
		next1:
		// process second line until y value is about to change
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
		next2:
		if (minx > t1x) minx = t1x;
		if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x;
		if (maxx < t2x) maxx = t2x;
		draw_line_horizontal(scr, minx, maxx, y, c);    // Draw line from min to max points found on the y
		                                              // Now increase y
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == y2) break;

	}
	next:
	// Second half
	dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y3 - y2);
	t1x = x2;

	if (dy1 > dx1) {   // swap values
		swap_int(&dy1, &dx1);
		changed1 = true;
	}
	else changed1 = false;

	e1 = (int)(dx1 >> 1);

	for (int i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) { t1xp = signx1; break; }//t1x += signx1;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if (i < dx1) i++;
		}
		next3:
		// process second line until y value is about to change
		while (t2x != x3) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
		next4:

		if (minx > t1x) minx = t1x;
		if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x;
		if (maxx < t2x) maxx = t2x;
		draw_line_horizontal(scr, minx, maxx, y, c);
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y > y3) return;
	}
}

// For qsort()
int triangle_compare_z(const void *a, const void *b)
{
	const triangle *t1 = a;
	const triangle *t2 = b;

	double z1 = t1->p[0].z + t1->p[1].z + t1->p[2].z;
	double z2 = t2->p[0].z + t2->p[1].z + t2->p[2].z;

	return (z2 > z1) - (z1 > z2);
}

void draw_rect(screen *scr, int xstart, int ystart, int xend, int yend, color c)
{
	for (int i = ystart; i < yend; ++i) {
		for (int j = xstart; j < xend; ++j) {
			draw_pixel(scr, i, j, c);
		}
	}
}

void screen_fill(screen *scr, color c)
{
	draw_rect(scr, 0, 0, scr->height, scr->width, c);
}

void screen_clear(screen *scr)
{
	screen_fill(scr, backgroundColor);
}

triangle triangle_new()
{
	return (triangle) {
		{
			{0.0, 0.0, 0.0, VECTOR_DEFAULT_W},
			{0.0, 0.0, 0.0, VECTOR_DEFAULT_W},
			{0.0, 0.0, 0.0, VECTOR_DEFAULT_W}
		},
		{255, 255, 255, 255}
	};
}
