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

		va_start(args,fmt_str);
		
		unsigned int counter;

		for(counter = 0x00; counter < amount; ++counter)
		{
			free(va_arg(args,void*));
		}

		va_end(args);

	}


void interface_appear(void)
	{

		int gap = 0;

		//GET WINDOW SIZE FOR 

		SDL_GetWindowSize(main_win,&cfg->w,&cfg->h);

		printf("w = %d,h = %d\n\r",cfg->w,cfg->h);
		
		//TOP BAR
		SDL_FRect top_bar;
		
		//TOP BAR BUTTON
		SDL_FRect exit_button;

		SDL_FRect info_button;

		//MAIN BAR

		SDL_FRect file_space;

		//FOOTER BAR

		SDL_FRect enc_dec_button;

		SDL_FRect add_files_button;

		gap = cfg->h/40;

		if(cfg->w <= 800 && cfg->h <= 600)
		{	
				
			top_bar.x = 0,top_bar.y = 0,top_bar.w = cfg->w,top_bar.h = cfg->h/8-gap;

			file_space.x = 30 ,file_space.y = top_bar.h+gap, file_space.w = cfg->w-file_space.x*2, file_space.h = cfg->h-(cfg->h/2);

			enc_dec_button.x = file_space.x ,enc_dec_button.y = file_space.y + file_space.h + gap ,enc_dec_button.w = file_space.w, enc_dec_button.h = cfg->h/8;

			add_files_button.x = enc_dec_button.x , add_files_button.y = enc_dec_button.y + enc_dec_button.h + gap, add_files_button.w = enc_dec_button.w , add_files_button.h = enc_dec_button.h;

		}
		else if(cfg->w <= 1024 || cfg->h <= 768)
		{
			
			int div = 8;

			top_bar.x = 0,top_bar.y = 0,top_bar.w = cfg->w/8,top_bar.h = cfg->h;

			if(cfg->w >= 1000)
			{
				top_bar.w = cfg->w/16;
			}

			file_space.x = top_bar.w+gap ,file_space.y = 30 ,file_space.w = cfg->w-(cfg->w/3), file_space.h = cfg->h-file_space.y*2;

			enc_dec_button.x = file_space.x + file_space.w + gap + ((file_space.x + file_space.w + gap)/20) ,enc_dec_button.y = file_space.y ,enc_dec_button.w = cfg->h/8, enc_dec_button.h = file_space.w/2;

			add_files_button.x = enc_dec_button.x, add_files_button.y = enc_dec_button.y, add_files_button.w = enc_dec_button.w , add_files_button.h = enc_dec_button.h;
			//add_files_button.x = enc_dec_button.x , add_files_button.y = enc_dec_button.y - enc_dec_button.h + gap , add_files_button.w = enc_dec_button.w, add_files_button.h = enc_dec_button.h;


		}
//		else if(cfg->w <= 1280 || cfg-> <= 1024)
//		{
//
//		}

		SDL_SetRenderDrawColor(main_render,33, 204, 104,255);
			
		SDL_RenderClear(main_render);	
		
		SDL_SetRenderDrawColor(main_render,100, 100, 100, 255);

		SDL_SetRenderDrawColor(main_render,66,43,21,255);
		
		SDL_RenderFillRect(main_render,&top_bar);

		SDL_SetRenderDrawColor(main_render,76,99,12,255);

		SDL_RenderFillRect(main_render,&file_space);

		SDL_SetRenderDrawColor(main_render,120,32,77,255);

		SDL_RenderFillRect(main_render,&enc_dec_button);

		SDL_RenderFillRect(main_render,&add_files_button);

		SDL_RenderPresent(main_render);


		return;
	}
