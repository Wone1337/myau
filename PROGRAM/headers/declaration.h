//______________INCLUDES_____________________
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3_image/SDL_image.h"
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <stdarg.h>

#if !defined(DECLARATION)
#define DECLARATION

//_______________ENUMS______________________________

enum ERROR_MSG_MODE
{
	ERRNO_FAILURE = -1,
	SDL_ERR = 1001,
	CMN_ERR = 1000
};

//______________MACROS______________________
#define ERR_MSG(MSG,ERROR_MODE) ( (ERROR_MODE) == SDL_ERR ? SDL_LogError(SDL_LOG_CATEGORY_ERROR,"%s: %s\n",(MSG),SDL_GetError()) : printf("%s\n",(MSG)))
				 
//ERR_MSG(MSG,ERROR_MODE), where ERROR_MODE says us about a [CMN] - regular error output, and [SDL] - SDL error logs output

//_____________DEFINES______________________
#define TITLE_NAME "[NOTWI] [Pre-alpha] ACHIVATOR [BUILD 0.0.3]"
#define CTRL_KEY(KEY) (0x1F & (KEY)) 

//_____________STRUCTURES_____________________
typedef struct config
	{	
		unsigned int w;
		unsigned int h;
		const char* title;
		SDL_WindowFlags flags;
		struct termios origin;		

	} INIT_CFG; 

//_______________DATA________________________
extern SDL_Window *main_win;
extern bool close_window_check;
extern SDL_Event main_event;
extern SDL_Renderer *main_render;
extern INIT_CFG *cfg;


//_____________FUNCTIONS______________________
void enable_raw_mode(void);
void disable_raw_mode(void);
void archivator_init(void);
void destruct_alloc_mem(unsigned int, ...);
void interface_appear(void);
void exit_from_program(void);

#endif
