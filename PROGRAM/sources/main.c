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
		}

		
		while(!close_window_check)
		{
			

			while(SDL_PollEvent(&main_event))
			{
				if(main_event.type == SDL_EVENT_QUIT)
				{
					close_window_check = true;
				}

			}


			SDL_GetWindowSize(main_win,&cfg->w,&cfg->h);
			

			SDL_FRect top_bar = {0,0,cfg->w,50};

			SDL_FRect browse_file = {50,cfg->h/2,cfg->w/2+100,35};

			SDL_FRect button = {50,cfg->h/2+50,100,70};

			//SDL_FRect top_bar = {0,0,cfg->w,50};

			
			SDL_SetRenderDrawColor(main_render,33, 204, 104,255);
			
			SDL_RenderClear(main_render);
		
			SDL_SetRenderDrawColor(main_render,100, 100, 100, 255);
			
			SDL_RenderFillRect(main_render,&top_bar);

			SDL_RenderFillRect(main_render,&browse_file);

			SDL_RenderFillRect(main_render,&button);

			SDL_RenderPresent(main_render);
		

		}
	
		destruct_alloc_mem(1,cfg);

		SDL_DestroyRenderer(main_render);

		SDL_DestroyWindow(main_win);

		SDL_Quit();

		return 0x00;
	}

