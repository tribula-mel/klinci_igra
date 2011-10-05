#include <stdio.h>
#include <stdlib.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "renderer.h"
#include "common.h"

static s32 sprite_renderer(scene *sc, SDL_Surface *area);
static s32 sprite_constructor(sprite *sp);
static s32 sprite_collision(sprite *sp);
static s32 xml_contour_processor(xmlNode *node, sprite_frame *frame);
static s32 xml_frame_processor(xmlNode *node, sprite *sp);
static s32 xml_sprite_processor(xmlNode *node, sprite *sp);
static s32 xml_graphics_processor(xmlNode *node, scene *sc);
static scene *xml_node_processor(xmlNode *node);

static s32 xml_contour_processor(xmlNode *node, sprite_frame *frame)
{
	xmlNode *cur_node = NULL;
	xmlChar *property;
	sprite_contour *contour = NULL;
	sprite_contour *contour_prev = NULL;

	contour = malloc(sizeof(sprite_contour));
	if (contour == NULL) {
		printf("malloc failure\n");
		exit(EXIT_FAILURE);
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
				exit(EXIT_FAILURE);
			}
			contour_prev->next = contour;
			contour->next = NULL;
			contour_prev = contour;
		}
	}

	return EXIT_SUCCESS;
}

static s32 xml_frame_processor(xmlNode *node, sprite *sp)
{
	xmlNode *cur_node = NULL;
	sprite_frame *frame = NULL;
	sprite_frame *pframe = NULL;

	frame = malloc(sp->frame_number * sizeof(sprite_frame));
	if (frame == NULL) {
		printf("malloc failure\n");
		exit(EXIT_FAILURE);
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

	return EXIT_SUCCESS;
}

static s32 xml_sprite_processor(xmlNode *node, sprite *sp)
{
	xmlNode *cur_node = NULL;
	xmlChar *property;

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
			if (!xmlStrcmp(cur_node->name, (u8 *)"frame")) {
				xml_frame_processor(cur_node->children, sp);
			}
		}
	}

	return EXIT_SUCCESS;
}

static s32 xml_graphics_processor(xmlNode *node, scene *sc)
{
	xmlNode *cur_node = NULL;
	xmlChar *property;
	sprite *sp = NULL;
	sprite **_static = &sc->sp_static;
	sprite **_dynamic = &sc->sp_dynamic;
	u32 frame_rate = 0;

	for (cur_node = node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			printf("node name[%s]\n", cur_node->name);
			if (!xmlStrcmp(cur_node->name, (u8 *)"sprite")) {
				property = xmlGetProp(cur_node, (u8 *)"frame_rate");
				frame_rate = atoi((char *)property);
				printf("frame_rate[%d]\n", frame_rate);
				xmlFree(property);
				if (frame_rate != 0) {
					if (*_dynamic == NULL) {
						*_dynamic = malloc(sizeof(sprite));
						if (*_dynamic == NULL) {
							printf("malloc failure\n");
							exit(EXIT_FAILURE);
						}
					}
					else {
						(*_dynamic)->next = malloc(sizeof(sprite));
						if ((*_dynamic)->next == NULL) {
							printf("malloc failure\n");
							exit(EXIT_FAILURE);
						}
						*_dynamic = (*_dynamic)->next;
					}
					memset(*_dynamic, 0, sizeof(sprite));
					sp = *_dynamic;
				}
				else {
					if (*_static == NULL) {
						*_static = malloc(sizeof(sprite));
						if (*_static == NULL) {
							printf("malloc failure\n");
							exit(EXIT_FAILURE);
						}
					}
					else {
						(*_static)->next = malloc(sizeof(sprite));
						if ((*_static)->next == NULL) {
							printf("malloc failure\n");
							exit(EXIT_FAILURE);
						}
						*_static = (*_static)->next;
					}
					memset(*_static, 0, sizeof(sprite));
					sp = *_static;
				}
				sp->frame_rate = frame_rate;
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"sprite")) {
				property = xmlGetProp(cur_node, (u8 *)"frame_number");
				sp->frame_number = atoi((char *)property);
				printf("frame_number[%d]\n", sp->frame_number);
				xmlFree(property);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"sprite")) {
				sp->name = xmlGetProp(cur_node, (u8 *)"name");
				printf("name[%s]\n", sp->name);
			}
			if (!xmlStrcmp(cur_node->name, (u8 *)"sprite")) {
				xml_sprite_processor(cur_node->children, sp);
			}
		}
	}

	return EXIT_SUCCESS;
}

static scene *xml_node_processor(xmlNode *node)
{
	xmlNode *cur_node = NULL;
	scene *sc;

	sc = malloc(sizeof(scene));
	if (sc == NULL) {
		printf("malloc failure\n");
		return NULL;
	}
	memset(sc, 0, sizeof(scene));

	for (cur_node = node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			printf("node name[%s]\n", cur_node->name);
			if (!xmlStrcmp(cur_node->name, (u8 *)"graphics")) {
				xml_graphics_processor(cur_node->children, sc);
			}
		}
	}

	return sc;
}

static s32 sprite_renderer(scene *sc, SDL_Surface *area)
{
	SDL_Surface *image, *image_format;
	SDL_Rect DestR;
	sprite *sp;

	sp = sc->sp_static;
	for (; sp != NULL; sp = sp->next) {
		image = IMG_Load((char *)sp->frame->file_name);
		if (image == NULL) {
			fprintf(stderr, "Couldn't load %s: %s\n", sp->frame->file_name, SDL_GetError());
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

		DestR.x = sp->x_pos;
		DestR.y = sp->y_pos;

		/* Blit onto the screen surface */
		if(SDL_BlitSurface(image_format, NULL, area, &DestR) < 0) {
			fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}

		SDL_UpdateRect(area, sp->x_pos, sp->y_pos, image_format->w, image_format->h);
	}

	sp = sc->sp_dynamic;
	for (; sp != NULL; sp = sp->next) {
		image = IMG_Load((char *)sp->frame->file_name);
		if (image == NULL) {
			fprintf(stderr, "Couldn't load %s: %s\n", sp->frame->file_name, SDL_GetError());
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

		DestR.x = sp->x_pos;
		DestR.y = sp->y_pos;

		/* Blit onto the screen surface */
		if(SDL_BlitSurface(image_format, NULL, area, &DestR) < 0) {
			fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}

		SDL_UpdateRect(area, sp->x_pos, sp->y_pos, image_format->w, image_format->h);
	}

	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	scene *sc = NULL;
	SDL_Surface *screen;
	SDL_Event event;
	s32 quit = 0;
	u32 width = 0, height = 0;

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

	sc = xml_node_processor(root_element);

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
	width = sc->sp_static->frame->contour->width;
	height = sc->sp_static->frame->contour->height;
	printf("name[%s]\n", sc->sp_static->name);
	printf("next name[%s]\n", sc->sp_static->next->name);
	exit(EXIT_FAILURE);
	screen = SDL_SetVideoMode(width, height, 8, SDL_SWSURFACE|SDL_ANYFORMAT);
	if (screen == NULL) {
		fprintf(stderr, "Couldn't set %dx%dx8 video mode: %s\n", width, height, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	sprite_renderer(sc, screen);

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
