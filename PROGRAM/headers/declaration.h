//______________INCLUDES_____________________
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3_image/SDL_image.h"
#include <stdbool.h>
#include <sys/stat.h>

#if !defined(DECLARATION)
#define DECLARATION

//_______________ENUMS______________________________


enum ERROR_MSG_MODE
{
	SDL_ERR = 1001,
	CMN_ERR = 1000
};

//______________MACROS______________________
#define ERR_MSG(MSG,ERROR_MODE) ( (ERROR_MODE) == SDL_ERR ? SDL_LogError(SDL_LOG_CATEGORY_ERROR,"%s: %s\n",( MSG),SDL_GetError()) : printf("%s\n",(MSG) ) )
				 

//_____________DEFINES______________________
#define TITLE_NAME "[NOTWI] [Alpha] ACHIVATOR [BUILD 1.0.0]"

#define WIN_WIDTH 0x280

#define WIN_HEIGHT 0x1E0

#define FILE_BUF_SIZE 512

#define ITEM_HEIGHT 30

#define SYMBOL_INPUT_BUF 1024

#define DEF_PIC_PATH "../PROGRAM/pic/"

#define DEF_FONT_PATH "../PROGRAM/fonts/"

#define DEF_ZIP_NAME ".ntwzip"



//_____________STRUCTURES_____________________
typedef struct CONFIG	
	{	
		unsigned int w;
		unsigned int h;
		const char* title;
		SDL_WindowFlags flags;
				
	} CONFIG;


typedef struct WINDOW_SET
	{
		SDL_Window *win;
		CONFIG cfg;
		SDL_Renderer *render;
		TTF_TextEngine *engine;
		bool check;
		
	} WINDOW_SET;

typedef struct RGBA
	{
		unsigned int red;
		unsigned int green;
		unsigned int blue;
		unsigned int alpha;
	
	} RGBA;

typedef struct TEXTURES
	{
		SDL_FRect dest_rect;
		SDL_Texture *texture;
		SDL_Surface *surface;

	} TEXTURES;


typedef struct TEXT
	{
		TTF_Font *font;
		TTF_Text *text;
		char *title;
		TEXTURES *text_texture;
		unsigned int title_size;
		unsigned int text_scale;

	} TEXT;

typedef struct RECTWP
	{
		SDL_FRect rect;
		RGBA main_color;
		SDL_Texture *texture;
		TEXT *rect_text;
	} RECTWP;

typedef struct FILE_MAN
	{
		RECTWP *file_rect;
		bool is_busy;

	} FILE_MAN;

typedef struct INPUT_TEXT
	{

           SDL_FRect rect;
	   SDL_Texture *texture;
	   TTF_Font *font;
	   SDL_Surface *surface;
	   
	} INPUT;

//_______________DATA________________________

//EVENT
extern SDL_Event main_event;

//CUSTOM
extern WINDOW_SET *main_win;
extern WINDOW_SET *info_win;
extern RECTWP *top_bar;
extern RECTWP *exit_button;
extern RECTWP *info_button;
extern RECTWP *file_space;
extern RECTWP *enc_button;
extern RECTWP *dec_button;
extern RECTWP *add_files_button;
extern RECTWP *archive_name_field;
extern RECTWP *archive_name_field_title;
extern RECTWP *archive_name_field_input_rect;
extern FILE_MAN file_container[];

//_____________FUNCTIONS______________________
void archivator_init(void);
void interface_appear(void);
void exit_from_program(void);
RECTWP* create_rectwp(WINDOW_SET *,RGBA,char*,char*,char*,unsigned int);
void destroy_rectwp(RECTWP **);
WINDOW_SET * init_window_set_obj(WINDOW_SET*,CONFIG);
void destroy_window_set_obj(WINDOW_SET**);
void fill_rect(WINDOW_SET *,RECTWP *,RGBA);
void add_file(WINDOW_SET*, RECTWP*, char*);
void file_location_update(RECTWP *,RECTWP *, unsigned int);
int render_file_container(WINDOW_SET *,RECTWP *);
void change_scroll_offset(int *);
void delete_file(FILE_MAN [],unsigned int *,unsigned int);
void add_file_from_dialog(WINDOW_SET *,RECTWP *);
void encode_files(char *);
void decode_files(void);
void input_text_on(WINDOW_SET *,INPUT **);
void input_text_off(WINDOW_SET *,INPUT **);
void input_text_render(WINDOW_SET *,INPUT *,RECTWP *,char *);

#endif
