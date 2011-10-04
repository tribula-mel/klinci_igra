#include <stdio.h>
#include <stdlib.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "renderer.h"
#include "common.h"

static s32 sprite_renderer(sprite *sp, SDL_Surface *area);
static s32 sprite_constructor(sprite *sp);
static s32 sprite_collision(sprite *sp);
static s32 *xml_graphics_processor(xmlNode *node, sprite *sp);
static void xml_contour_processor(xmlNode *node, sprite *sp);
static void xml_frame_processor(xmlNode *node, sprite *sp);
static sprite *xml_sprite_processor(xmlNode *node);

static void xml_contour_processor(xmlNode *node, sprite_frame *frame)
{
	xmlNode *cur_node = NULL;
	xmlChar *property;
	sprite_contour *contour = NULL;
	sprite_contour *contour_prev = NULL;

	contour = malloc(sizeof(sprite_contour));
	if (contour == NULL) {
		printf("malloc failure\n");
		return;
	}
	frame->contour = contour;
	contour_prev = contour;
	contour->next = NULL;

	for (cur_node = node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (!xmlStrcmp(cur_node->name, (u8 *)"rectangle")) {
				property = xmlGetProp(cur_node, (u8 *)"x");
				contour->x_offset = atoi((char *)property);
				printf("x_offset[%d]\n", contour->x_offset);
				xmlFree(property);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"rectangle")) {
				property = xmlGetProp(cur_node, (u8 *)"y");
				contour->y_offset = atoi((char *)property);
				printf("y_offset[%d]\n", contour->y_offset);
				xmlFree(property);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"rectangle")) {
				property = xmlGetProp(cur_node, (u8 *)"width");
				contour->width = atoi((char *)property);
				printf("width[%d]\n", contour->width);
				xmlFree(property);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"rectangle")) {
				property = xmlGetProp(cur_node, (u8 *)"height");
				contour->height = atoi((char *)property);
				printf("height[%d]\n", contour->height);
				xmlFree(property);
			}
		}

		if (cur_node->next != NULL) {
			contour = malloc(sizeof(sprite_contour));
			if (contour == NULL) {
				/* TODO: fix the leak */
				printf("malloc failure\n");
				return;
			}
			contour_prev->next = contour;
			contour->next = NULL;
			contour_prev = contour;
		}
	}
}

static void xml_frame_processor(xmlNode *node, sprite *sp)
{
	xmlNode *cur_node = NULL;
	xmlChar *property;
	sprite_frame *frame = NULL;
	sprite_frame *pframe = NULL;

	frame = malloc(sp->frame_number * sizeof(sprite_frame));
	if (frame == NULL) {
		printf("malloc failure\n");
		return;
	}
	sp->frame = frame;
	frame->contour = NULL;
	pframe = frame;

	for (cur_node = node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (!xmlStrcmp(cur_node->name, (u8 *)"file")) {
				frame->file_name = xmlGetProp(cur_node, (u8 *)"name");
				printf("file_name[%s]\n", frame->file_name);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"contour")) {
				xml_contour_processor(cur_node->children, frame);
			}
		}
	}
}

static sprite *xml_sprite_processor(xmlNode *node)
{
	xmlNode *cur_node = NULL;
	xmlChar *property;
	sprite *sp = NULL;

	sp = malloc(sizeof(sprite));
	if (sp == NULL) {
		printf("malloc failure\n");
		return NULL;
	}

	sp->frame = NULL;

	for (cur_node = node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (!xmlStrcmp(cur_node->name, (u8 *)"position")) {
				property = xmlGetProp(cur_node, (u8 *)"x");
				sp->x_pos = atoi((char *)property);
				printf("x[%d]\n", sp->x_pos);
				xmlFree(property);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"position")) {
				property = xmlGetProp(cur_node, (u8 *)"y");
				sp->y_pos = atoi((char *)property);
				printf("y[%d]\n", sp->y_pos);
				xmlFree(property);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"attributes")) {
				property = xmlGetProp(cur_node, (u8 *)"moveable");
				sp->attrs.moveable = atoi((char *)property);
				printf("moveable[%d]\n", sp->attrs.moveable);
				xmlFree(property);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"attributes")) {
				property = xmlGetProp(cur_node, (u8 *)"intelligence");
				sp->attrs.intelligence = atoi((char *)property);
				printf("intelligence[%d]\n", sp->attrs.intelligence);
				xmlFree(property);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"attributes")) {
				property = xmlGetProp(cur_node, (u8 *)"frame_rate");
				sp->attrs.frame_rate = atoi((char *)property);
				printf("frame_rate[%d]\n", sp->attrs.frame_rate);
				xmlFree(property);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"frame")) {
				xml_frame_processor(cur_node->children, sp);
			}
		}
	}

	return sp;
}

static s32 *xml_graphics_processor(xmlNode *node, sprite *sp)
{
	xmlNode *cur_node = NULL;

	for (cur_node = node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			printf("node name[%s]\n", cur_node->name);
			if (!xmlStrcmp(cur_node->name, (u8 *)"sprite")) {
				property = xmlGetProp(cur_node, (u8 *)"frame_number");
				sp->frame_number = atoi((char *)property);
				printf("frame_number[%d]\n", sp->frame_number);
				xmlFree(property);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"sprite")) {
				sp->name = xmlGetProp(cur_node, (u8 *)"name");
				printf("name[%d]\n", sp->name);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"sprite")) {
				sp->next = xml_sprite_processor(cur_node->children);
				sp = sp->next;
				continue;
			}
		}

		xml_node_processor(cur_node->children, sp);
	}

	return EXIT_SUCCESS;
}

static s32 sprite_renderer(sprite *sp, SDL_Surface *area)
{
	SDL_Surface *image, *image_format;
	SDL_Rect DestR;
	sprite *next = NULL;

	for (next = sp->next; next != NULL; next = next->next) {
		image = IMG_Load((char *)next->file_name);
		if (image == NULL) {
			fprintf(stderr, "Couldn't load %s: %s\n", next->file_name, SDL_GetError());
			exit(EXIT_FAILURE);
		}

		/*
		 * Palettized screen modes will have a default palette (a standard
		 * 8*8*4 colour cube), but if the image is palettized as well we can
		 * use that palette for a nicer colour matching
		 */
		if (image->format->palette && area->format->palette) {
			SDL_SetColors(area, image->format->palette->colors, 0,
					image->format->palette->ncolors);
		}

		image_format = SDL_DisplayFormat(image);
		if (image_format == NULL) {
			fprintf(stderr, "Couldn't convert to display format: out of memory\n");
			SDL_FreeSurface(image);
			exit(EXIT_FAILURE);
		}

		DestR.x = next->pos_x;
		DestR.y = next->pos_y;

		/* Blit onto the screen surface */
		if(SDL_BlitSurface(image_format, NULL, area, &DestR) < 0) {
			fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}

		SDL_UpdateRect(area, next->pos_x, next->pos_y, image_format->w, image_format->h);
	}

	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	sprite root_node;
	sprite *next = NULL;
	background bg;
	SDL_Surface *screen, *background, *background_format;
	SDL_Rect DestR;
	SDL_Event event;
	s32 quit = 0;

	if (argc != 2) {
		printf("args != 2\n");
		return(EXIT_FAILURE);
	}

	/*
	 * this initialize the library and check potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */
	LIBXML_TEST_VERSION

	/*parse the file and get the DOM */
	doc = xmlReadFile(argv[1], NULL, 0);
	if (doc == NULL) {
		printf("error: could not parse file %s\n", argv[1]);
	}

	/*Get the root element node */
	root_element = xmlDocGetRootElement(doc);

	xml_node_processor(root_element, &root_node, &bg);
	for (next = root_node.next; next != NULL; next = next->next)
		printf("file_name[%s] pos_x[%d] pos_y[%d]\n", next->file_name, next->pos_x, next->pos_y);

	/*free the document */
	xmlFreeDoc(doc);

	/*
	 *Free the global variables that may
	 *have been allocated by the parser.
	 */
	xmlCleanupParser();

	/* Initialize defaults, Video and Audio */
	if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) {
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	/* Clean up on exit */
	atexit(SDL_Quit);

	/* Have a preference for 8-bit, but accept any depth */
	screen = SDL_SetVideoMode(bg.size_x, bg.size_y, 8, SDL_SWSURFACE|SDL_ANYFORMAT);
	if (screen == NULL) {
		fprintf(stderr, "Couldn't set 800x400x8 video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	background = IMG_Load((char *)bg.file_name);
	if (background == NULL) {
		fprintf(stderr, "Couldn't load %s: %s\n", argv[1], SDL_GetError());
		exit(EXIT_FAILURE);
	}

	/*
	 * Palettized screen modes will have a default palette (a standard
	 * 8*8*4 colour cube), but if the image is palettized as well we can
	 * use that palette for a nicer colour matching
	 */
	if (background->format->palette && screen->format->palette) {
		SDL_SetColors(screen, background->format->palette->colors, 0,
				background->format->palette->ncolors);
	}

	background_format = SDL_DisplayFormat(background);
	if (background_format == NULL) {
		fprintf(stderr, "Couldn't convert to display format: out of memory\n");
		SDL_FreeSurface(background);
		exit(EXIT_FAILURE);
	}

	DestR.x = 0;
	DestR.y = 0;

	/* Blit onto the screen surface */
	if(SDL_BlitSurface(background_format, NULL, screen, &DestR) < 0) {
		fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_UpdateRect(screen, 0, 0, background->w, background->h);

	sprite_renderer(&root_node, screen);

	/* Enable Unicode translation */
	SDL_EnableUNICODE(1);

	/* Loop until an SDL_QUIT event is found */
	while(!quit) {
		/* Poll for events */
		while(SDL_PollEvent( &event) ) {
			switch(event.type) {
			/* SDL_QUIT event (window close) */
			case SDL_QUIT:
				quit = 1;
				break;

			default:
				break;
			}
		}
	}

	return EXIT_SUCCESS;
}
