#include "../headers/declaration.h"




int main([[maybe_unused]] int argc ,[[maybe_unused]] char *argv[])
	{
	
		SDL_Init(SDL_INIT_VIDEO);

		archivator_init();

		main_win = SDL_CreateWindow(cfg->title,cfg->w,cfg->h,cfg->flags);

		if(main_win == NULL)
		{
			ERR_MSG("MAIN WIN: %s\n")
			return 0x01;
		}	
		
		main_render = SDL_CreateRenderer(main_win,NULL);
		
		if(main_render == NULL)
		{
			ERR_MSG("RENDER: %s\n");
			SDL_DestroyWindow(main_win);
			SDL_Quit();
			return 0x01;
		}


		SDL_FRect background = {0,0,100,100};

		while(!close_window_check)
		{

			while(SDL_PollEvent(&main_event))
			{
				if(main_event.type == SDL_EVENT_QUIT)
				{
					close_window_check = true;
				}

			}
			
			SDL_SetRenderDrawColor(main_render,33, 204, 104,255);
			
			SDL_RenderClear(main_render);
		
			SDL_SetRenderDrawColor(main_render,100, 100, 100, 255);
			
			SDL_RenderFillRect(main_render,&background);

			SDL_RenderPresent(main_render);
		

		}
	
	
		SDL_DestroyRenderer(main_render);

		SDL_DestroyWindow(main_win);

		SDL_Quit();

		return 0x00;
	}

