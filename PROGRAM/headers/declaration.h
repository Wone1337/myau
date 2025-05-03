//______________INCLUDES_____________________
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3_image/SDL_image.h"
#include <stdbool.h>

#if !defined(DECLARATION)
#define DECLARATION

//______________MACROS______________________
#define ERR_MSG(MSG) (SDL_LogError(SDL_LOG_CATEGORY_ERROR,(MSG),SDL_GetError()) );

//_____________DEFINES______________________
#define TITLE_NAME "[NOTWI] [Pre-alpha] ACHIVATOR [BUILD 0.0.0]"

//_____________STRUCTURES_____________________
typedef struct config
	{	
		unsigned int w;
		unsigned int h;
		const char* title;
		SDL_WindowFlags flags;

	} INIT_CFG; 

//_______________DATA________________________
extern SDL_Window *main_win;
extern bool close_window_check;
extern SDL_Event main_event;
extern SDL_Renderer *main_render;
extern INIT_CFG *cfg;


//_____________FUNCTIONS______________________
void archivator_init(void);






#endif
