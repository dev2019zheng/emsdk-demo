#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <emscripten.h>

#define DIM 1024
#define DIM1 (DIM - 1)
#define _sq(x) ((x) * (x))
#define _cb(x) abs((x) * (x) * (x))
#define _cr(x) (unsigned char)(pow((x), 1.0 / 3.0))

static FILE *fp = NULL;

unsigned char GR(int, int);
unsigned char BL(int, int);
unsigned char RD(int, int);

int center_x = 1024 / 2;
int center_y = 1024 / 2;

unsigned char GR(int i, int j)
{
	return BL(i, j);
}
unsigned char BL(int i, int j)
{
	const int n = 3;
	float r = sqrt(0.f + _sq(i - center_y) + _sq(j - center_x));
	float s = r / ((DIM / 2) / 2);

	if (s < 1)
	{
		i -= center_y;
		j -= center_x;
		float k = (sqrt(1. - _sq(s)) * sqrt(1. - _sq(s / n)) + _sq(s) / n) * n;
		i = i / k + center_y;
		j = j / k + center_x;
	}

	return (i / ((DIM / 2) / 8) + j / ((DIM / 2) / 8)) % 2 * 255;
}

unsigned char RD(int i, int j)
{
	return BL(i, j);
}

void pixel_write(int i, int j)
{
	static unsigned char color[3];
	color[0] = RD(i, j) & 255;
	color[1] = GR(i, j) & 255;
	color[2] = BL(i, j) & 255;
	fwrite(color, 1, 3, fp);
}

SDL_Surface *screen = NULL;

void pixel_write_screen(int i, int j)
{
	*((Uint32 *)screen->pixels + i * DIM + j) = SDL_MapRGBA(screen->format, RD(i, j) & 255, GR(i, j) & 255, BL(i, j) & 255, 255);
}

void render_loop()
{
	SDL_Flip(screen);
	if (SDL_MUSTLOCK(screen))
	{
		SDL_LockSurface(screen);
	}

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			break;

		case SDL_MOUSEMOTION:
			if (center_x != event.button.x || center_y != event.button.y)
			{
				center_x = event.button.x;
				center_y = event.button.y;
				printf("mouse(%d, %d)", center_x, center_y);

				// 重新生成图像
				for (int j = 0; j < DIM; j++)
				{
					for (int i = 0; i < DIM; i++)
					{
						pixel_write(i, j);
					}
				}
			}
			break;
		}
	}
	if (SDL_MUSTLOCK(screen))
	{
		SDL_UnlockSurface(screen);
	}
}

int main()
{
	printf("Hello World!\n");

	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(DIM, DIM, 32, SDL_ANYFORMAT);

	if (screen == NULL)
	{
		SDL_Quit();
		return 1;
	}

	emscripten_set_main_loop(render_loop, 0, 0);
	// fp = fopen("MathPic.ppm", "wb");

	// fprintf(fp, "P6\n%d %d\n255\n", DIM, DIM);

	// for (int j = 0; j < DIM; j++)
	// {
	// 	for (int i = 0; i < DIM; i++)
	// 	{
	// 		pixel_write(i, j);
	// 	}
	// }

	// fclose(fp);

	return (0);
}