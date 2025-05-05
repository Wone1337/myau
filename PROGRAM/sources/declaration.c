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
