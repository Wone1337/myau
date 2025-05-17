#include "../headers/declaration.h"

int main([[maybe_unused]] int argc ,[[maybe_unused]] char *argv[])
	{
	
		SDL_Init(SDL_INIT_VIDEO);

		archivator_init();

		main_win = SDL_CreateWindow(cfg->title,cfg->w,cfg->h,cfg->flags);

		if(main_win == NULL)
		{
			ERR_MSG("[main]MAIN WIN: create failed",SDL_ERR);
			return 0x01;
		}	
		
		main_render = SDL_CreateRenderer(main_win,NULL);
		
		if(main_render == NULL)
		{
			ERR_MSG("[main]RENDER: create failed",SDL_ERR);
			SDL_DestroyWindow(main_win);
			SDL_Quit();
			return 0x01;
		}

		if(SDL_SetWindowMinimumSize(main_win,cfg->w,cfg->h) == false)
		{
			ERR_MSG("[main]SET_WIN_MIN_SIZE: failed",CMN_ERR);
			return 0x01;
		}


		//if(SDL_SetWindowMaximumSize(main_win,cfg->w,cfg->h) == false)
		//{
		//	ERR_MSG("[main]SET_WIN_MAZ_SIZE: failed",CMN_ERR);
		//	return 0x01;
		//}



		enable_raw_mode();

		
		while(!close_window_check)
		{
			

			while(SDL_PollEvent(&main_event))
			{
				if(main_event.type == SDL_EVENT_QUIT)
				{
					close_window_check = true;
				}

			}

			
		
			
			interface_appear();
				
		}

		disable_raw_mode();
	
		destruct_alloc_mem(1,cfg);

		SDL_DestroyRenderer(main_render);

		SDL_DestroyWindow(main_win);

		SDL_Quit();

		return 0x00;
	}

