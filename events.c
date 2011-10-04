
#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "common.h"

static void event_listener(void);

static void event_listener(void)
{
	SDL_Event event;
	s32 quit = 0;

	/* Loop until an SDL_QUIT event is found */
	while(!quit) {
		/* Poll for events */
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_LEFT) {
				}
				if(event.key.keysym.sym == SDLK_RIGHT) {
				}
				if(event.key.keysym.sym == SDLK_UP) {
				}
				if(event.key.keysym.sym == SDLK_DOWN) {
				}
				break;
			case SDL_KEYUP:
				break;
			/* SDL_QUIT event (window close) */
			case SDL_QUIT:
				quit = 1;
				break;

			default:
				break;
			}
		}
	}
}
