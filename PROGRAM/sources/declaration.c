#include "../headers/declaration.h"

//________________VAR_DEFINITIONS____________________
SDL_Window *main_win = NULL;
bool close_window_check = false;
SDL_Event main_event;
SDL_Renderer *main_render = NULL;
INIT_CFG *cfg = NULL;

//_____________FUNCTIONS_REALIZATION_________________
void archivator_init(void)
	{	

		cfg = (INIT_CFG*)malloc(sizeof(INIT_CFG));
		
		cfg->w = 640;

		cfg->h = 480;

		cfg->title = TITLE_NAME;

		cfg->flags = SDL_WINDOW_RESIZABLE;

		if(SDL_GL_LoadLibrary(NULL) == true)
		{
			cfg->flags |= SDL_WINDOW_OPENGL;

			SDL_GL_UnloadLibrary();
		}
		else
		{
			cfg->flags |= SDL_WINDOW_VULKAN;
		}

		return;
	}
