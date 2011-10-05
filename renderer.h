#ifndef __RENDERER_H
#define __RENDERER_H

#include "common.h"

enum intelligence_types {
	NO_INTELLIGENCE = 0,
	ARTIFICIAL_INTELLIGENCE,
	HUMAN_INTELLIGENCE,
};

typedef struct graphics_attributes {
	u8 moveable; /* TODO: bit field in future? */
	u8 intelligence;
} sprite_attrs;

typedef struct graphics_contour {
	u32 x_offset;
	u32 y_offset;
	u32 width;
	u32 height;
	struct graphics_contour *next;
} sprite_contour;

typedef struct graphics_frame {
	SDL_TimerID timer_id;
	sprite_contour *contour;
	u8 *file_name;
} sprite_frame;

typedef struct graphics_sprite {
	struct graphics_sprite *next;
	u32 x_pos; /* relative to the background graphics */
	u32 y_pos; /* coordinate start is at bottom left */
	sprite_attrs attrs;
	sprite_frame *frame;
	u8 *name;
	u8 frame_number;
	u8 frame_rate;
} sprite;

typedef struct graphics_scene {
	sprite *sp_static;
	sprite *sp_dynamic;
} scene;

#endif /* __RENDERER_H */
