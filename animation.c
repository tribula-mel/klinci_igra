#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "common.h"

static u32 animation_renderer(u32 interval, void *param);

static void animation_dispatch(sprite *anim_sp)
{
	SDL_TimerID timer_id;

	timer_id = SDL_AddTimer(100, animation_renderer, (void *)anim_sp);
}

static u32 animation_renderer(u32 interval, void *param)
{
}
