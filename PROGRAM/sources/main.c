#include "../headers/declaration.h"

int main([[maybe_unused]] int argc ,[[maybe_unused]] char *argv[])
	{
	
		if(!SDL_Init(SDL_INIT_VIDEO))
		{			
			ERR_MSG("[main]SDL_INIT: Init failed",SDL_ERR);
			return 0x01;
		}

		if(!TTF_Init())
		{	
			ERR_MSG("[main]TTF_INIT: Init failed",SDL_ERR);
			return 0x01;
		}
		
		archivator_init();

		if(!SDL_SetWindowMinimumSize(main_win->win,WIN_WIDTH,WIN_HEIGHT))
		{
			ERR_MSG("[main]SET_MIN_WIN_SIZE: error on settin up",SDL_ERR);
		}
		
		while(main_win->check)
		{
			
			interface_appear();
				
		}

		exit_from_program();	

		return 0x00;
	}

