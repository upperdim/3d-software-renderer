#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "util.h"
#include "graphics.h"
#include "debug.h"
#include "vector.h"
#include "matrix.h"
#include "cli.h"

int init(SDL_Window **window, SDL_Surface **surface, screen *scr)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Error initializing SDL: %s\n", SDL_GetError());
		return 1;
	}

	*window = SDL_CreateWindow("3D Software Renderer",
	                           SDL_WINDOWPOS_CENTERED,
	                           SDL_WINDOWPOS_CENTERED,
	                           800, 600,
	                           SDL_WINDOW_RESIZABLE);
	if (!(*window)) {
		printf("Error creating window: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	*surface = SDL_GetWindowSurface(*window);
	if (!(*surface)) {
		printf("Error creating surface: %s\n", SDL_GetError());
		SDL_DestroyWindow(*window);
		SDL_Quit();
		return 1;
	}

	scr->pixelsArr = (*surface)->pixels;
	scr->width     = (*surface)->w;
	scr->height    = (*surface)->h;
	scr->format    = (*surface)->format;

	return 0;
}

int main(int argc, char *argv[])
{
	handle_args(argc, argv);
	SDL_Window  *window        = NULL;
	SDL_Surface *windowSurface = NULL;
	screen screen = { 0 };

	if (init(&window, &windowSurface, &screen))
		return 1;

	mesh mesh = { NULL };
	load_object(&mesh, "teapot.obj");

	vector3d  camera  = {0.0, 0.0, 0.0, VECTOR_DEFAULT_W};
	matrix4x4 matProj = matrix_projection(FOV, (double) screen.height / (double) screen.width, 0.1, 1000.0);

	clock_t oldtime = clock();
	double theta = 1.0;
	double dt    = 1.0;

	while (!quit) {
		dt = dt_ms(oldtime);
		theta += ROTATION_SPEED * dt;
		oldtime = clock();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;

			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					windowSurface = SDL_GetWindowSurface(window);
					screen.pixelsArr = windowSurface->pixels;
					screen.width = windowSurface->w;
					screen.height = windowSurface->h;
					// TODO: Update aspect ratio and other dependent vars
					//printf("Size changed: %d, %d\n", screen.width, screen.height); // debug
				}
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_Q:
				case SDL_SCANCODE_ESCAPE:
					quit = true;
					break;
				default:
					break;
				}
				break;

			default:
				break;
			}
		}

		matrix4x4 matRotZ = matrix_rotation_z( theta / 2);
		matrix4x4 matRotX = matrix_rotation_x(-theta);

		matrix4x4 matTrans = matrix_translation(0.0, 0.0, 5.0);
		matrix4x4 matWorld = matrix_identity();
		matWorld = matrix_mult_matrix(&matRotZ,  &matRotX);
		matWorld = matrix_mult_matrix(&matWorld, &matTrans);

		triangle *trianglesToRaster = NULL;
		int triToRasterCount = 0;
		screen_clear(&screen);

		// Draw mesh
		for (int i = 0; i < mesh.triCount; ++i) {
			triangle triProjected   = triangle_new();
			triangle triTransformed = triangle_new();

			triTransformed.p[0] = matrix_mult_vector(&matWorld, &mesh.tris[i].p[0]);
			triTransformed.p[1] = matrix_mult_vector(&matWorld, &mesh.tris[i].p[1]);
			triTransformed.p[2] = matrix_mult_vector(&matWorld, &mesh.tris[i].p[2]);

			// Calculate triangle normal
			vector3d normal = {0.0, 0.0, 0.0, VECTOR_DEFAULT_W};
			vector3d side1  = {0.0, 0.0, 0.0, VECTOR_DEFAULT_W};
			vector3d side2  = {0.0, 0.0, 0.0, VECTOR_DEFAULT_W};

			side1 = vector_sub(&triTransformed.p[1], &triTransformed.p[0]);
			side2 = vector_sub(&triTransformed.p[2], &triTransformed.p[0]);

			normal = vector_cross_prod(&side1, &side2);
			normal = vector_normalise(&normal);

			vector3d cameraRay = vector_sub(&triTransformed.p[0], &camera);
			
			if (vector_dot_prod(&normal, &cameraRay) < 0.0) {
				vector3d lightDirection = {0.0, 1.0, -1.0, VECTOR_DEFAULT_W};
				lightDirection = vector_normalise(&lightDirection);

				double dp = max_double(0.1, vector_dot_prod(&lightDirection, &normal));
				uint8_t pixVal = (int) (255 * dp);
				triProjected.color = (color) {pixVal, pixVal, pixVal, 255};

				triProjected.p[0] = matrix_mult_vector(&matProj, &triTransformed.p[0]);
				triProjected.p[1] = matrix_mult_vector(&matProj, &triTransformed.p[1]);
				triProjected.p[2] = matrix_mult_vector(&matProj, &triTransformed.p[2]);

				triProjected.p[0] = vector_div(&triProjected.p[0], triProjected.p[0].w);
				triProjected.p[1] = vector_div(&triProjected.p[1], triProjected.p[1].w);
				triProjected.p[2] = vector_div(&triProjected.p[2], triProjected.p[2].w);

				vector3d offsetView = {1.0, 1.0, 0.0, VECTOR_DEFAULT_W};
				triangle_add_vector(&triProjected, &offsetView);

				triangle_mult_xy(&triProjected, 0.5 * (double) screen.width, 0.5 * (double) screen.height);

				// Add this triangle to "triangles to raster"
				++triToRasterCount;
				trianglesToRaster = realloc(trianglesToRaster, triToRasterCount * sizeof(triangle));
				trianglesToRaster[triToRasterCount - 1] = triProjected;
				//trianglesToRaster[triToRasterCount - 1].color = c; // We handled color above, it's already set	
			}
		}
		
		// Painters algorithm, sort from back to front
		qsort(trianglesToRaster, triToRasterCount, sizeof(triangle), triangle_compare_z);

		for (int i = 0; i < triToRasterCount; ++i) {
			triangle tri = trianglesToRaster[i];

			fill_triangle(&screen, tri, tri.color);

			// Wireframe
			if (debugMode) {
				draw_triangle(&screen, tri, DEBUG_WIREFRAME_COLOR);
			}
		}

		SDL_UpdateWindowSurface(window);
		cap_framerate(dt);
		print_debug_info(triToRasterCount, dt);
	}

	// Display flags
	if (debugMode && debugFlagOutOfScreen) {
		printf("Out of screen!\n");
	}

	SDL_FreeSurface(windowSurface);
	windowSurface = NULL;

	SDL_DestroyWindow(window);
	window = NULL;

	SDL_Quit();

	return 0;
}
