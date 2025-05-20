#include "../headers/declaration.h"
#include "../headers/nfd.h"

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

		if(cfg == NULL)
		{
			ERR_MSG("[archivator_init]cfg[declaration.h]: alloc failed",CMN_ERR);
			exit(0x01);
		}
		
		cfg->w = 0x280;

		cfg->h = 0x1E0;

		cfg->title = TITLE_NAME;

		cfg->flags = SDL_WINDOW_RESIZABLE;

		if(tcgetattr(STDIN_FILENO,&cfg->origin) == ERRNO_FAILURE)
		{
			ERR_MSG("[archivator_init]tsgetattr[termios.h]: failed",CMN_ERR);
			exit(0x01);
		}
		
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

void enable_raw_mode(void)
	{
		
		struct termios raw;

		raw = cfg->origin;

	   	raw.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP| INLCR | IGNCR | ICRNL | IXON);
           	raw.c_oflag &= ~OPOST;
           	raw.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
           	raw.c_cflag &= ~(CSIZE | PARENB);
           	raw.c_cflag |= CS8;
		raw.c_cc[VMIN] = 0x00;
		raw.c_cc[VTIME] = 0x1E;

		if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw) == ERRNO_FAILURE)
		{
			ERR_MSG("[enable_raw_mode]tcsetattr[termios.h]: failed",CMN_ERR);
			exit(0x01);
		}


		return;
	}

void disable_raw_mode(void)
	{
		if(tcsetattr(STDIN_FILENO,TCSANOW,&cfg->origin) == ERRNO_FAILURE)
		{
			ERR_MSG("[disable_raw_mode]tssetattr[termios.h]: failed",CMN_ERR);
			exit(0x01);
		}
		
		return;
	}

void destruct_alloc_mem(unsigned int amount,...)
	{

		va_list args;

		va_start(args);
		
		unsigned int counter;

		for(counter = 0x00; counter < amount; ++counter)
		{
			free(va_arg(args,void*));
		}

		va_end(args);

	}


void interface_appear(void)
	{
		//INIT VAR

		int gap = 0;

		//GET WINDOW SIZE FOR 

		SDL_GetWindowSize(main_win,&cfg->w,&cfg->h);

		//printf("w = %d,h = %d\n\r",cfg->w,cfg->h);
		
		//TOP BAR
		SDL_FRect top_bar;


		SDL_FRect exit_button;

		SDL_FRect info_button;

		//MAIN BAR

		SDL_FRect file_space;

		SDL_FRect delete_file_from_list;

		//FOOTER BAR

		SDL_FRect enc_dec_button;

		SDL_FRect add_files_button;

		//INTERFACE 

		gap = cfg->h/40;

		if(cfg->w <= 854 ||  cfg->h <= 458)
		{	
				

			top_bar.x = 0,top_bar.y = 0,top_bar.w = cfg->w,top_bar.h = cfg->h/8-gap;
			
			exit_button.x = top_bar.x , exit_button.y = top_bar.y , exit_button.w = 100 ,exit_button.h = top_bar.h ;

			info_button.x = top_bar.x + exit_button.w + gap , info_button.y = top_bar.y , info_button.w = exit_button.w,info_button.h = top_bar.h;

			file_space.x = 30 ,file_space.y = top_bar.h+gap, file_space.w = cfg->w-file_space.x*2, file_space.h = cfg->h-(cfg->h/2);

			enc_dec_button.x = file_space.x ,enc_dec_button.y = file_space.y + file_space.h + gap ,enc_dec_button.w = file_space.w, enc_dec_button.h = cfg->h/8;

			add_files_button.x = enc_dec_button.x , add_files_button.y = enc_dec_button.y + enc_dec_button.h + gap, add_files_button.w = enc_dec_button.w , add_files_button.h = enc_dec_button.h;

		}
		else if(cfg->w <= 1920 || cfg->h <= 1080)
		{

			top_bar.x = 0,top_bar.y = 0,top_bar.w = cfg->w/gap,top_bar.h = cfg->h;
			
			exit_button.x = top_bar.x , exit_button.y = top_bar.y , exit_button.w = top_bar.w ,exit_button.h = 100;
			
			info_button.x = top_bar.x , info_button.y = top_bar.y + exit_button.h + gap , info_button.w = exit_button.w, info_button.h = exit_button.h;

			file_space.x = top_bar.w+gap ,file_space.y = 30 ,file_space.w = cfg->w-(cfg->w/3), file_space.h = cfg->h-file_space.y*2;

			enc_dec_button.x = file_space.x + file_space.w + gap + gap ,enc_dec_button.y = file_space.y ,enc_dec_button.w = file_space.w/3-gap, enc_dec_button.h = file_space.h/2;

			add_files_button.x = enc_dec_button.x, add_files_button.y = enc_dec_button.y+enc_dec_button.h+gap, add_files_button.w = enc_dec_button.w , add_files_button.h = enc_dec_button.h-gap;


		}
		else if(cfg->w <= 1280 || cfg->h <= 1024)
		{

		}
		else if(cfg->w <= 1600 || cfg->h <= 900)
		{

		}
		else
		{

		}

		//FILL ALL OBJECT COLOR

		//BACKGROUND FILL

		SDL_SetRenderDrawColor(main_render,41, 41, 40, 1);
		
		//CLEAR	
		SDL_RenderClear(main_render);

		//TOP_BAR FILL
		SDL_SetRenderDrawColor(main_render, 255, 64, 0, 1);
		
		SDL_RenderFillRect(main_render,&top_bar);

		//TOP_BAR BUTTONS FILL
		
		SDL_SetRenderDrawColor(main_render, 78, 54, 234, 1);
		
		SDL_RenderFillRect(main_render,&exit_button);

		SDL_RenderFillRect(main_render,&info_button);			

		//FILE SPACE FILL
		SDL_SetRenderDrawColor(main_render,255, 152, 0, 1);

		SDL_RenderFillRect(main_render,&file_space);

		//ENC_DEC_BUTTON AND ADD_FILES_BUTTON FILL
		SDL_SetRenderDrawColor(main_render,46, 204, 113,1 );

		SDL_RenderFillRect(main_render,&enc_dec_button);

		SDL_SetRenderDrawColor(main_render,46, 204, 113, 1);

		SDL_RenderFillRect(main_render,&add_files_button);
	
		
		float d = 0;
		float e = 0;

		SDL_GetGlobalMouseState(&d,&e);
		
		//printf("d = %f e = %f\n\r",d,e);

		//printf("x = %f y = %f w = %f h = %f\n\r",info_button.x,info_button.y,info_button.w+info_button.x,info_button.h+info_button.y);
		//ALL OBJECT FUNCTIONAL
		
		while(SDL_PollEvent(&main_event))
		{

			//if(main_event.type == SDL_EVENT_QUIT)
			//{
			//	close_window_check = true;
			//}


			//puts("HUI\n\r");
				
			if(main_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
			{

				if( ( (exit_button.x <= d) && (d <= exit_button.w + exit_button.x))  &&  ( (exit_button.y <= e) && (e <= exit_button.h + exit_button.y )))
				{
					close_window_check = true;
				}

					
				if( ( (info_button.x <= d) && (d <= info_button.w + info_button.x))  &&  ( (info_button.y <= e) && (e <= info_button.h + info_button.y )))
				{
					puts("HUI1\n\r");
				}


			}

			if(main_event.type = SDL_EVENT_DROP_FILE)
			{	
				
				if( ( (file_space.x <= d) && (d <= file_space.w + file_space.x))  &&  ( (file_space.y <= e) && (e <= file_space.h + file_space.y )))
				{
					puts("HUI2\n\r");
			
				}
			}

		}
		
		// RENDER ALL OBJECT
		SDL_RenderPresent(main_render);


		return;
	}

void exit_from_program(void)
	{
		disable_raw_mode();

		destruct_alloc_mem(1,cfg);

		SDL_DestroyRenderer(main_render);

		SDL_DestroyWindow(main_win);

		TTF_Quit();

		SDL_Quit();

		
		return;
	}
