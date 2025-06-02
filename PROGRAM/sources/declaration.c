#include "../headers/declaration.h"
#include "../headers/nfd.h"
#include "../headers/archiver.h"

//________________VAR_INIT____________________
SDL_Event main_event;
WINDOW_SET *main_win = NULL;
WINDOW_SET *info_win = NULL;
RECTWP *archive_name_field_title = NULL;
RECTWP *archive_name_field_input_rect = NULL;
RECTWP *top_bar = NULL;
RECTWP *exit_button = NULL;
RECTWP *info_button = NULL;
RECTWP *title_arch_name_field = NULL;
RECTWP *file_space = NULL;
RECTWP *enc_button = NULL;
RECTWP *dec_button = NULL;
RECTWP *add_files_button = NULL;
RECTWP *archive_name_field = NULL;
FILE_MAN file_container[FILE_BUF_SIZE];
INPUT *archive_input_text_field = NULL;

//____________LOCAL____________________
		
		int file_scroll_offset;
		unsigned int file_counter;
		float d;
		float e;
		const bool *key_state;
		unsigned int index_to_transfer;
		char input_keyboard[SYMBOL_INPUT_BUF];
		
		//STRUCT NEED ? MAYBE

//_____________FUNCTIONS_REALIZATION_________________

void test_header_write_read() {
    LZ77_Header original = {LZ77_MAGIC, 12345, 6789, 0xABCDEF};
    
    FILE *f = fopen("test_header.bin", "wb");
    fwrite(&original, sizeof(LZ77_Header), 1, f);
    fclose(f);
    
    LZ77_Header read_back;
    f = fopen("test_header.bin", "rb");
    fread(&read_back, sizeof(LZ77_Header), 1, f);
    fclose(f);
    
    printf("Original magic: 0x%08X, Read: 0x%08X\n", original.magic, read_back.magic);
}

void archivator_init(void)
	{	
		
		test_header_write_read();
			
		CONFIG main_cfg = {WIN_WIDTH,WIN_HEIGHT,TITLE_NAME,SDL_WINDOW_RESIZABLE};

		if(SDL_GL_LoadLibrary(NULL) == true)
		{
			main_cfg.flags |= SDL_WINDOW_OPENGL;
			SDL_GL_UnloadLibrary();
		}
		else
		{
			main_cfg.flags |= SDL_WINDOW_VULKAN;
		}
		
		main_win = init_window_set_obj(main_win,main_cfg);

		if(main_win == NULL)
		{
        		ERR_MSG("[archivator_init]MAIN_WIN_INIT: create failed", CMN_ERR);
        		exit(0x01);
    		}	

		top_bar = create_rectwp(main_win,(RGBA){255,64,0,1},NULL,NULL,NULL,0);
		
		exit_button = create_rectwp(main_win,(RGBA){78,54,234,1},DEF_PIC_PATH "cross.svg",DEF_FONT_PATH "PatrickHandSC-Regular.ttf","Exit",500);

		info_button = create_rectwp(main_win,(RGBA){78,54,234,1}, DEF_PIC_PATH "info.svg", DEF_FONT_PATH "PatrickHandSC-Regular.ttf","Info",500);
    		
		file_space = create_rectwp(main_win,(RGBA){255,152,0,1},NULL,NULL,NULL,0);

		add_files_button = create_rectwp(main_win,(RGBA){46,204,113,1},NULL,NULL,NULL,0);
		
		enc_button = create_rectwp(main_win,(RGBA){46,204,113,1},NULL,NULL,NULL,0);

		dec_button = create_rectwp(main_win,(RGBA){46,204,113,1},NULL,NULL,NULL,0);

		
	
		return;
	}

WINDOW_SET* init_window_set_obj(WINDOW_SET *win_obj,CONFIG cfg)
	{
		
		win_obj = (WINDOW_SET*)malloc(sizeof(WINDOW_SET));

		if(win_obj == NULL)
		{
			ERR_MSG("[init_window_set_obj]WINDOW_SET OBJ: alloc failed",CMN_ERR);
			return win_obj;
		}

		win_obj->win = NULL;
   		win_obj->render = NULL;
   		win_obj->engine = NULL;
   		win_obj->check = false;
   		win_obj->cfg = cfg;


		win_obj->win = SDL_CreateWindow(cfg.title,cfg.w,cfg.h,cfg.flags);

		if(win_obj->win == NULL)
		{
			ERR_MSG("(init_window_set]WIN: create failed",SDL_ERR);
			destroy_window_set_obj(&win_obj);
			return win_obj;
		}


		
		win_obj->cfg = cfg;

		win_obj->render = SDL_CreateRenderer(win_obj->win,NULL);

		if(win_obj->render == NULL)
		{
			ERR_MSG("(init_window_set]RENDER: create failed",SDL_ERR);
			destroy_window_set_obj(&win_obj);
			return win_obj;
		}



		win_obj->engine = TTF_CreateRendererTextEngine(win_obj->render);
		
		if(win_obj->engine == NULL)
		{
			ERR_MSG("(init_window_set]ENGINE: create failed",SDL_ERR);
			destroy_window_set_obj(&win_obj);
			return win_obj;
		}

		win_obj->check = true;


		return win_obj;
	}

void destroy_window_set_obj(WINDOW_SET **win_obj)
	{
		if(win_obj == NULL || *win_obj == NULL)
		{
			puts("[destroy_window_set_obj]ZERO NOTICED\n");
			return ;
		}

		WINDOW_SET *obj = *win_obj;


		if(obj->engine != NULL)
			TTF_DestroyRendererTextEngine(obj->engine);

		
		if(obj->render != NULL)
			SDL_DestroyRenderer(obj->render);
	
		
		if(obj->win != NULL)
			SDL_DestroyWindow(obj->win);

		
		obj->cfg = (CONFIG){0,0,NULL,0};
		

		
		obj->engine = NULL;
		


		obj->render = NULL;
	
		

		obj->win = NULL;

	
		obj->check = false;

	
		free(obj);

		*win_obj = NULL;
	
		return;
	}

RECTWP *create_rectwp(WINDOW_SET *win_obj,RGBA rgba,char *texture_path,char *ttf_path,char *title,unsigned int text_scale)
	{

		RECTWP *temp_object = (RECTWP*)malloc(sizeof(RECTWP));

		if(temp_object == NULL)
		{
			ERR_MSG("[create_rectwp]RECTW OBJ: alloc failed",CMN_ERR);
			return temp_object;
		}


	

		temp_object->rect = (SDL_FRect){0,0,0,0};

	

		temp_object->main_color = rgba;

		temp_object->texture = NULL;	

		//temp_object->transition_color = (RGBA){temp_object->main_color.red+50,temp_object->main_color.green+50,temp_object->main_color.blue+50,temp_object->main_color.alpha};
	
		if(texture_path != NULL)
		{
			temp_object->texture = IMG_LoadTexture(win_obj->render,texture_path);
			

			if(temp_object->texture == NULL)
			{
				//puts("HUYARA_ERR");	
				ERR_MSG("[create_rectwp]TEXTURE: create failed",SDL_ERR);
				destroy_rectwp(&temp_object);
				return temp_object;
			}

		}
	

		
		
		temp_object->rect_text = NULL;
		
		temp_object->rect_text = (TEXT*)malloc(sizeof(TEXT));

		if(temp_object->rect_text == NULL)
		{
			ERR_MSG("[create_rectwp]TEXT OBJ: alloc failed",CMN_ERR);
			destroy_rectwp(&temp_object);
			return temp_object;
		}
		
		temp_object->rect_text->font = NULL;
	
		temp_object->rect_text->text = NULL;
		
		if((temp_object->rect_text->title = title) != NULL)
		{
			temp_object->rect_text->title = strdup(title);
		}

		temp_object->rect_text->text_texture = NULL;
		
		temp_object->rect_text->title_size = title == NULL ? 0 : strlen(title);
	
		temp_object->rect_text->text_scale = text_scale;
	

		if(ttf_path != NULL)
		{

			temp_object->rect_text->font = TTF_OpenFont(ttf_path,text_scale);

			if(temp_object->rect_text->font == NULL)
			{
				ERR_MSG("[create_rectwp]FONT: create failed",SDL_ERR);
				destroy_rectwp(&temp_object);
				return temp_object;
			}

		}
		
		temp_object->rect_text->text_texture = (TEXTURES*)malloc(sizeof(TEXTURES));

		if(temp_object->rect_text->text_texture ==  NULL)
		{
			ERR_MSG("[create_rectwp]text_texture: alloc failed",CMN_ERR);
			destroy_rectwp(&temp_object);
			return temp_object;

		}
	
		temp_object->rect_text->text_texture->dest_rect = (SDL_FRect){0,0,0,0};
		
		temp_object->rect_text->text_texture->texture = NULL;
		
		temp_object->rect_text->text_texture->surface = NULL;
		
		SDL_Color temp_color = (SDL_Color){0,0,0,0};

	

		if(ttf_path != NULL && title != NULL)
		{
		
			temp_object->rect_text->text = TTF_CreateText(win_obj->engine, temp_object->rect_text->font, temp_object->rect_text->title , temp_object->rect_text->title_size);

			if(temp_object->rect_text->text == NULL)
			{
				ERR_MSG("[create_rectwp]TTF_Text: create failed",SDL_ERR);
				destroy_rectwp(&temp_object);
				return temp_object;
			}

			temp_object->rect_text->text_texture->surface = TTF_RenderText_Solid(temp_object->rect_text->font,title,temp_object->rect_text->title_size,temp_color);

			if(temp_object->rect_text->text_texture->surface == NULL)
			{
				ERR_MSG("[create_rectwp]text_texture_surface: create failed",SDL_ERR);
				destroy_rectwp(&temp_object);
				return temp_object;
			}

			temp_object->rect_text->text_texture->texture = SDL_CreateTextureFromSurface(win_obj->render,temp_object->rect_text->text_texture->surface);

			if(temp_object->rect_text->text_texture->texture == NULL)
			{
				ERR_MSG("[create_rectwp]text_texture_texture: create failed",SDL_ERR);
				destroy_rectwp(&temp_object);
				return temp_object;
			}
	

		}

	
		
		return temp_object;
	}

void destroy_rectwp(RECTWP **rect_obj)
	{
		if(rect_obj == NULL ||  (*rect_obj) == NULL)
		{
			
			puts("[destroy_rectwp]ZERO NOTICED\n");
			return;
		}
		
		RECTWP *obj = *rect_obj;

		obj->rect = (SDL_FRect){0,0,0,0};

		obj->main_color = (RGBA){0,0,0,0};
		
			
		if(obj->texture != NULL)
		{
			SDL_DestroyTexture(obj->texture);
		}
		
		obj->texture = NULL;

		if(obj->rect_text->title != NULL)
		{
			free(obj->rect_text->title );
		}

		obj->rect_text->title = NULL;

		obj->rect_text->title_size = 0;
		
		obj->rect_text->text_scale = 0;

		if(obj->rect_text->font != NULL)
		{
		
			TTF_CloseFont(obj->rect_text->font);
		
		}

		obj->rect_text->font = NULL;
		
		if(obj->rect_text->text != NULL)
		{
			TTF_DestroyText(obj->rect_text->text);
		}


		obj->rect_text->text = NULL;
		
		if(obj->rect_text->text_texture->texture != NULL)
		{
			SDL_DestroyTexture(obj->rect_text->text_texture->texture);
		}

		obj->rect_text->text_texture->texture = NULL;

		if(obj->rect_text->text_texture->surface != NULL)
		{
			SDL_DestroySurface(obj->rect_text->text_texture->surface);
		}

		obj->rect_text->text_texture->surface = NULL;


		obj->rect_text->text_texture->dest_rect = (SDL_FRect){0,0,0,0};

		
		free(obj->rect_text->text_texture);
		
		obj->rect_text->text_texture = NULL;

		free(obj->rect_text);
	
		obj->rect_text = NULL;

		free(obj);
		
		*rect_obj = NULL;

		return;
	}

void add_file(WINDOW_SET *win_obj,RECTWP *rect_obj,char *file_name)
	{	
			
		struct stat is_dir;

		int check_is_dir = stat(file_name,&is_dir);

		if(check_is_dir == 0 && S_ISDIR(is_dir.st_mode))
		{
			ERR_MSG("[add_file]check dir: file expected",CMN_ERR);
			return;
		}


		if(file_counter >= FILE_BUF_SIZE)
		{
			ERR_MSG("[add_file]file_container: array is overflow",CMN_ERR);
			return;
		}

		
		RECTWP *file_rect_obj = create_rectwp(win_obj,(RGBA){0,0,0,0}, DEF_PIC_PATH "cross.svg",DEF_FONT_PATH "PatrickHandSC-Regular.ttf",file_name,24);
		
		puts("HUYAMBA_start\n");

		file_container[file_counter] = (FILE_MAN){file_rect_obj,true};  

		++file_counter;

		puts("HUYAMBA\n");

		return;
	}

void add_file_from_dialog(WINDOW_SET *win_obj,RECTWP *rect_obj)
	{
			nfdpathset_t outpath;

			nfdresult_t result = NFD_OpenDialogMultiple(NULL,NULL,&outpath);

			if(result == NFD_ERROR)
			{
				puts("[add_file_from_dialog]outpath:");
				ERR_MSG(NFD_GetError(),CMN_ERR);
			}
			else if(result == NFD_CANCEL)
			{
				puts("[add_file_from_dialog]outpath: file doesn't select from dialog\n");	
			}

			for(unsigned int count = 0; count < NFD_PathSet_GetCount(&outpath);++count)
			{
				nfdchar_t *file_name =  NFD_PathSet_GetPath(&outpath,count);
				add_file(win_obj,rect_obj,file_name);
			}

			if(outpath.buf == NULL  && outpath.indices == 0)
			{
				NFD_PathSet_Free(&outpath);
			}

			return;
	}

int render_file_container(WINDOW_SET *win_obj,RECTWP *rect_obj)
	{
		int delete_file_index = -1;

		int visible_items = rect_obj->rect.h / ITEM_HEIGHT;
		
		int max_scroll = file_counter - visible_items;

		if(max_scroll < 0) 
		{
			max_scroll = 0;
		}

		if(file_scroll_offset < 0)
		{		
			file_scroll_offset = 0;
		}

		if (file_scroll_offset > max_scroll) 
		{
			file_scroll_offset = max_scroll;
		}

	
		for (int i = 0; i < visible_items; ++i)
		{
			
		    int index = file_scroll_offset + i;
		    
		   
		    if (index >= file_counter)
		    {
			    break;
		    }
		
				
		    float offsety = i * ITEM_HEIGHT;

		

		    if(file_container[index].file_rect)
		    {
			file_location_update(rect_obj,file_container[index].file_rect, offsety);
		    	
			SDL_RenderTexture(win_obj->render, file_container[index].file_rect->texture, NULL, &file_container[index].file_rect->rect);
		    	
			SDL_RenderTexture(win_obj->render, file_container[index].file_rect->rect_text->text_texture->texture, NULL, &file_container[index].file_rect->rect_text->text_texture->dest_rect);

  		 	if(((file_container[index].file_rect->rect.x <= d) && (d <= file_container[index].file_rect->rect.x  + file_container[index].file_rect->rect.w ))  
		        &&  ((file_container[index].file_rect->rect.y <= e) && (e <= file_container[index].file_rect->rect.y  + file_container[index].file_rect->rect.h)) )
		    	{
		        	delete_file_index = index;	
		   	}

		    }
		
		}	

		return delete_file_index < 0 ? -1 : delete_file_index;

	}

void delete_file(FILE_MAN file_obj[],unsigned int *size , unsigned int index)
	{	
		
		destroy_rectwp(&file_obj[index].file_rect);
	
		file_obj[index].is_busy = false;
		
		file_obj[index] = file_obj[*size-1];

			
		file_obj[*size-1].is_busy = false;
		
		file_obj[*size-1].file_rect = NULL;

		--(*size);
		
		return;
	}

void input_text_on(WINDOW_SET *win_obj,INPUT **text_obj)
	{
		SDL_StartTextInput(win_obj->win);

		(*text_obj) = (INPUT*)malloc(sizeof(INPUT));

		(*text_obj)->font  = NULL ;

		(*text_obj)->surface = NULL; 

		(*text_obj)->texture = NULL;
		
		(*text_obj)->rect = (SDL_FRect){0,0,0,0};

		if((*text_obj) == NULL)
		{
			ERR_MSG("[input_text_on]text_obj: create failed",CMN_ERR);
			input_text_off(win_obj,text_obj);
			return;

		}
		
		(*text_obj)->font = TTF_OpenFont(DEF_FONT_PATH "PatrickHandSC-Regular.ttf",24);

		if((*text_obj)->font == NULL)
		{	
			ERR_MSG("[input_text_on]font: create failed",SDL_ERR);
			input_text_off(win_obj,text_obj);
			return;
		}
		
		return;
		
	}

void input_text_off(WINDOW_SET *win_obj,INPUT **text_obj)
	{

		if( text_obj == NULL || (*text_obj) == NULL)
		{
			puts("[input_text_off]NOTICED NULL\n");
			return;
		}
		
		if(win_obj && win_obj->win)
		{
			SDL_StopTextInput(win_obj->win);
		}

		if((*text_obj)->font != NULL)
		{
			TTF_CloseFont((*text_obj)->font);

			(*text_obj)->font = NULL;

		}

		if((*text_obj)->texture != NULL)
		{
			SDL_DestroyTexture((*text_obj)->texture);
			
			(*text_obj)->texture = NULL;

		}

		if((*text_obj)->surface != NULL)
		{
			SDL_DestroySurface((*text_obj)->surface);
		
			(*text_obj)->surface = NULL;

		}
		
		(*text_obj)->rect = (SDL_FRect){0,0,0,0}; 
		
		free((*text_obj));
		
		(*text_obj) = NULL;

			
		return;
		
	}

void input_text_render(WINDOW_SET *win_obj,INPUT *text_obj,RECTWP *rect_obj,char *text)
	{
		if(text_obj == NULL)
		{
			return;
		}
		
		text_obj->rect = rect_obj->rect;

		if(text_obj->texture != NULL)
		{
			SDL_DestroyTexture(text_obj->texture);	
		}

		if(strlen(text) <= 0)
		{
			return;
		}


		text_obj->surface = TTF_RenderText_Blended_Wrapped(text_obj->font,text,strlen(text),(SDL_Color){0,0,0,0},200);

		if(text_obj->surface == NULL)
		{
			ERR_MSG("[input_text_render]surface: create failed",SDL_ERR);
			return;
		}

		text_obj->texture = SDL_CreateTextureFromSurface(win_obj->render,text_obj->surface);
		
	
		if(text_obj->texture == NULL)
		{
			ERR_MSG("[input_text_render]texture: create failed",SDL_ERR);
			return;
		}


		SDL_DestroySurface(text_obj->surface);

		SDL_GetTextureSize(text_obj->texture,&text_obj->rect.w,&text_obj->rect.h);

		SDL_RenderTexture(win_obj->render,text_obj->texture,NULL,&text_obj->rect);

		return;
	}

void file_location_update(RECTWP *rect_obj_src,RECTWP *rect_obj_dest,unsigned int offsety)
	{	

		rect_obj_dest->rect_text->text_texture->dest_rect.y = rect_obj_src->rect.y + offsety; 

		rect_obj_dest->rect_text->text_texture->dest_rect.x = rect_obj_src->rect.x + 5;
		
		SDL_GetTextureSize(rect_obj_dest->rect_text->text_texture->texture,&rect_obj_dest->rect_text->text_texture->dest_rect.w,&rect_obj_dest->rect_text->text_texture->dest_rect.h);

		rect_obj_dest->rect.x = rect_obj_src->rect.x + rect_obj_src->rect.w - 25;

		rect_obj_dest->rect.y = rect_obj_src->rect.y + offsety + 2;
	
		rect_obj_dest->rect.w = 20;

		rect_obj_dest->rect.h = 25;
		
		return;
	}

void encode_files(char * file_name)
	{
		
		if(file_counter <= 0)
    		{
        		return;
    		}

   		 char fn[150];
   		 
   		 char **ptr = malloc(file_counter * sizeof(char*));
   		 
		 if(ptr == NULL) 
		 {
   		     return;
   		 }

   		 char **current_ptr = ptr;

   		 int counter = 0;

   		 while(file_counter > counter)
   		 {
   		
			*current_ptr = file_container[counter].file_rect->rect_text->title;
	
			++counter;
   				     
			++current_ptr;    		
		 }

   		 
   		snprintf(fn, sizeof(fn), "%s%s%s", DEF_PATH_OUTPUT_TMP, file_name, DEF_ZIP_NAME);

   		create_archive(fn, ptr, file_counter);

		while(file_counter)
		{
			delete_file(file_container,&file_counter,file_counter-1);	
		}
   		 
   		free(ptr);
    				
		return;
	}

void decode_files(void)
	{
		if(file_counter <= 0)
		{
			return;
		}
		

		while(file_counter)
		{

			
			
			extract_archive(file_container[file_counter-1].file_rect->rect_text->title,DEF_PATH_OUTPUT_TMP);
			
			delete_file(file_container,&file_counter,file_counter-1);
			
						
		}

		return;
	}

void interface_appear(void)
	{
	
		int gap = 0;

		if(main_win && main_win->win)
		{
			SDL_GetWindowSize(main_win->win,&main_win->cfg.w,&main_win->cfg.h);
		}
	
		if(main_win->cfg.w <= 854 || main_win->cfg.h <= 458)
		{	
				
			gap = main_win->cfg.w/40;
			
			top_bar->rect.x = 0,top_bar->rect.y = 0,top_bar->rect.w = main_win->cfg.w,top_bar->rect.h = main_win->cfg.h/8-gap;
			
			exit_button->rect.x = top_bar->rect.x , exit_button->rect.y = top_bar->rect.y , exit_button->rect.w = 100 ,exit_button->rect.h = top_bar->rect.h ;

			info_button->rect.x = top_bar->rect.x + exit_button->rect.w + gap , info_button->rect.y = top_bar->rect.y , info_button->rect.w = exit_button->rect.w,info_button->rect.h = top_bar->rect.h;
		
			file_space->rect.x = 30 ,file_space->rect.y = top_bar->rect.h+gap, file_space->rect.w = main_win->cfg.w-file_space->rect.x*2, file_space->rect.h = main_win->cfg.h-(main_win->cfg.h/2);

			enc_button->rect.x = file_space->rect.x ,enc_button->rect.y = file_space->rect.y + file_space->rect.h + gap ,enc_button->rect.w = file_space->rect.w/2, enc_button->rect.h = main_win->cfg.h/8;
			
			dec_button->rect.x = enc_button->rect.x + enc_button->rect.w + gap, dec_button->rect.y = enc_button->rect.y, dec_button->rect.w = enc_button->rect.w - gap, dec_button->rect.h = enc_button->rect.h;

			add_files_button->rect.x = enc_button->rect.x , add_files_button->rect.y = enc_button->rect.y + enc_button->rect.h + gap, add_files_button->rect.w = enc_button->rect.w + dec_button->rect.w + gap, add_files_button->rect.h = enc_button->rect.h;
			
			if(archive_name_field != NULL)
			{
				archive_name_field->rect.x = archive_name_field->rect.x ,archive_name_field->rect.y = file_space->rect.y + file_space->rect.h + gap ,archive_name_field->rect.w = file_space->rect.w/2, archive_name_field->rect.h = main_win->cfg.h/8;
			
				//archive_name_field_input_rect

				//archive_name_field_title

			}

		
		}
		else if(main_win->cfg.w <= 1920 || main_win->cfg.h <= 1080)
		{
			gap = main_win->cfg.h/40;
			
			top_bar->rect.x = 0,top_bar->rect.y = 0,top_bar->rect.w = main_win->cfg.w/gap,top_bar->rect.h = main_win->cfg.h;
			
			exit_button->rect.x = top_bar->rect.x , exit_button->rect.y = top_bar->rect.y , exit_button->rect.w = top_bar->rect.w ,exit_button->rect.h = 100;
			
			info_button->rect.x = top_bar->rect.x , info_button->rect.y = top_bar->rect.y + exit_button->rect.h + gap , info_button->rect.w = exit_button->rect.w, info_button->rect.h = exit_button->rect.h;
			file_space->rect.x = top_bar->rect.w+gap ,file_space->rect.y = 30 ,file_space->rect.w = main_win->cfg.w-(main_win->cfg.w/3), file_space->rect.h = main_win->cfg.h-file_space->rect.y*2;

			enc_button->rect.x = file_space->rect.x + file_space->rect.w + gap + gap,enc_button->rect.y = file_space->rect.y ,enc_button->rect.w = file_space->rect.w/3-gap, enc_button->rect.h = file_space->rect.h/3;
			dec_button->rect.x = enc_button->rect.x , dec_button->rect.y = enc_button->rect.y + enc_button->rect.h + gap ,dec_button->rect.w = enc_button->rect.w,dec_button->rect.h = enc_button->rect.h -gap;

			add_files_button->rect.x = dec_button->rect.x, add_files_button->rect.y = dec_button->rect.y+dec_button->rect.h+gap, add_files_button->rect.w = dec_button->rect.w , add_files_button->rect.h = dec_button->rect.h;
		
			if(archive_name_field != NULL)
			archive_name_field->rect.x = file_space->rect.x + file_space->rect.w + gap + gap,archive_name_field->rect.y = file_space->rect.y ,archive_name_field->rect.w = file_space->rect.w/3-gap, archive_name_field->rect.h = file_space->rect.h/3;


		}
	
		
		while(SDL_PollEvent(&main_event))
		{

			SDL_GetMouseState(&d,&e);
			
			key_state = SDL_GetKeyboardState(NULL);
		


			if(main_event.type == SDL_EVENT_KEY_DOWN)
			{	
				
				key_state = SDL_GetKeyboardState(NULL);
	

				if( (key_state[SDL_SCANCODE_LCTRL]) && (key_state[SDL_SCANCODE_Q]))
				{
											
					if(main_win && main_win->win && main_event.window.windowID == SDL_GetWindowID(main_win->win))
					{
						main_win->check = false;
					}

					if(info_win && info_win->win && main_event.window.windowID == SDL_GetWindowID(info_win->win))
					{
						destroy_window_set_obj(&info_win);

					}

				}

				if( (key_state[SDL_SCANCODE_LCTRL]) && (key_state[SDL_SCANCODE_I]))
				{
					if(info_win == NULL)
					{
						CONFIG info_cfg = {WIN_WIDTH,WIN_HEIGHT,"INFO WIN",0};
						info_win = init_window_set_obj(info_win,info_cfg);
					}

				}

			
					
				if((key_state[SDL_SCANCODE_LCTRL]) && (key_state[SDL_SCANCODE_C]))
				{
					while(file_container[0].is_busy == true)
					{
						delete_file(file_container,&file_counter,0);
					}

				}

				if((key_state[SDL_SCANCODE_S]))
				{		
					for(unsigned int count = 0; file_container[count].is_busy == true; ++count)
					{	

						printf("f_cnt= %d bool = %u pointer = %p str = %s\n",count,file_container[count].is_busy,file_container[count].file_rect,file_container[count].file_rect->rect_text->title);

					}
				}	

				if( ( (key_state[SDL_SCANCODE_DOWN]) || (key_state[SDL_SCANCODE_UP])) && (main_event.window.windowID == SDL_GetWindowID(main_win->win)))
				{	
					main_event.wheel.y = key_state[SDL_SCANCODE_DOWN] ? -main_event.wheel.y : +main_event.wheel.y ;						
						
					change_scroll_offset(&file_scroll_offset);
				}

				if (main_event.key.key == SDLK_BACKSPACE && strlen(input_keyboard) > 0) 
				{
                        		input_keyboard[strlen(input_keyboard) - 1] = '\0';
                    		}

				if( (main_event.key.key == SDLK_RETURN) && (archive_name_field) && ((archive_name_field->rect.x <= d) && (d <= archive_name_field->rect.w + archive_name_field->rect.x))  &&  ( (archive_name_field->rect.y <= e) && (e <= archive_name_field->rect.h + archive_name_field->rect.y )))
				{

					if(strlen(input_keyboard) > 0)
					{

						destroy_rectwp(&archive_name_field);	
											
						input_text_off(main_win,&archive_input_text_field);

						encode_files(input_keyboard);
						
						memset(&input_keyboard,0,sizeof(input_keyboard));
					
					}

					
				}

						
			}

			

			if(main_event.type == SDL_EVENT_TEXT_INPUT)
			{
				if(strlen(input_keyboard) <= 15)
				{
					strncat(input_keyboard, main_event.text.text, sizeof(input_keyboard) - strlen(input_keyboard) - 1);
					printf("str = %s\n",input_keyboard);
				}
			}


			if(main_event.type == SDL_EVENT_MOUSE_WHEEL)
			{
							
				if((main_event.window.windowID == SDL_GetWindowID(main_win->win) ) && ((file_space->rect.x <= d) && (d <= file_space->rect.w + file_space->rect.x))  &&  ( (file_space->rect.y <= e) && (e <= file_space->rect.h + file_space->rect.y )))
				{
					change_scroll_offset(&file_scroll_offset);					
				}
			}


			if(main_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
			{
				

				if( ( (exit_button->rect.x <= d) && (d <= exit_button->rect.w + exit_button->rect.x))  &&  ( (exit_button->rect.y <= e) && (e <= exit_button->rect.h + exit_button->rect.y )))
				{
					main_win->check = false;
				}

					
				if(((info_button->rect.x <= d) && (d <= info_button->rect.w + info_button->rect.x))  &&  ( (info_button->rect.y <= e) && (e <= info_button->rect.h + info_button->rect.y )))
				{
					if(info_win == NULL)
					{
						CONFIG info_cfg = {WIN_WIDTH,WIN_HEIGHT,"INFO WIN",0};
						info_win = init_window_set_obj(info_win,info_cfg);
					}
				}

			

				if(index_to_transfer != -1)				
				{		
					delete_file(file_container,&file_counter,index_to_transfer);

				
				}

				if(((add_files_button->rect.x <= d) && (d <= add_files_button->rect.w + add_files_button->rect.x))  &&  ( (add_files_button->rect.y <= e) && (e <= add_files_button->rect.h + add_files_button->rect.y)))
				{
				
					add_file_from_dialog(main_win,file_space);
				}

				if(((enc_button->rect.x <= d) && (d <= enc_button->rect.w + enc_button->rect.x))  &&  ( (enc_button->rect.y <= e) && (e <= enc_button->rect.h + enc_button->rect.y)))
				{
												
					archive_name_field = create_rectwp(main_win,(RGBA){65,76,198,1},NULL,NULL,NULL,0);
					
					//archive_name_field_title = create_rectwp(main_win,(RGBA){98,123,45,1},DEF_FONT_PATH "PatrickHandSC-Regular.ttf",NULL,"Введите имя файла:(для продолжения нажмите enter)",36);	
					//archive_name_field_input_rect = create_rectwp(main_win,(RGBA){85,11,93,1},NULL,NULL,NULL,0);	
					input_text_on(main_win,&archive_input_text_field);
					
				}

				if(((dec_button->rect.x <= d) && (d <= dec_button->rect.w + dec_button->rect.x))  &&  ( (dec_button->rect.y <= e) && (e <= dec_button->rect.h + dec_button->rect.y)))
				{
					decode_files();	
				
				}




			}	
		

			if(main_event.type == SDL_EVENT_DROP_FILE)
			{
								
				add_file(main_win,file_space,main_event.drop.data);	
				
			}

		}

	
		SDL_SetRenderDrawColor(main_win->render,41, 41, 40, 1);
		
		SDL_RenderClear(main_win->render);
		
		
		fill_rect(main_win,top_bar,top_bar->main_color);	
		
		fill_rect(main_win,exit_button,exit_button->main_color);	
		
		fill_rect(main_win,info_button,info_button->main_color);

		
	
		fill_rect(main_win,file_space,file_space->main_color);			
		
		fill_rect(main_win,enc_button,enc_button->main_color);
		
		if(archive_name_field != NULL && archive_name_field_input_rect != NULL && archive_name_field_title != NULL)
		{
			fill_rect(main_win,archive_name_field,archive_name_field->main_color);
			fill_rect(main_win,archive_name_field_input_rect,archive_name_field_input_rect->main_color);
			fill_rect(main_win,archive_name_field_title,archive_name_field_title->main_color);
		}

		fill_rect(main_win,dec_button,enc_button->main_color);	
		
		fill_rect(main_win,add_files_button,add_files_button->main_color);
			

		index_to_transfer = render_file_container(main_win,file_space);

		input_text_render(main_win,archive_input_text_field,enc_button,input_keyboard);
	
		
		SDL_RenderPresent(main_win->render);
	
		
		if(info_win && info_win->check && info_win->render)
		{
			SDL_RenderClear(info_win->render);
			SDL_RenderPresent(info_win->render);
		}


		return;
	}

void change_scroll_offset(int *scroll_offset)
	{

		int y_scroll = main_event.wheel.y;
    					
		if(main_event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
		{
			y_scroll = -y_scroll;
		}
					
   		*scroll_offset -= y_scroll;
					
		return;

	}

void fill_rect(WINDOW_SET *win_obj,RECTWP *rect_obj, RGBA obj_color)
	{
	
		if(rect_obj)
		{
			SDL_SetRenderDrawColor(win_obj->render,obj_color.red,obj_color.green,obj_color.blue,obj_color.alpha);
			
			SDL_RenderFillRect(win_obj->render,&rect_obj->rect);
		}
	
	//	if(rect_obj->rect_text->text_texture->texture != NULL)
	//	{	
	//		
	//		temp_rect = rect_obj->rect;
	//		
	//		if(temp_rect.w > temp_rect.h)
	//		{
	//		
	//			temp_rect.x += (temp_rect.w/2);

	//			temp_rect.w /= 2;
	//		
	//		}
	//		else
	//		{
	//			temp_rect.y += (temp_rect.h/2);

	//			temp_rect.h /= 2;

	//		}

	//		check = SDL_RenderTexture(win_obj->render,rect_obj->rect_text->text_texture->texture,NULL,&temp_rect);

	//		if(!check)
	//		{
	//			ERR_MSG("[fill_rect]text_texture_texture: render failed",SDL_ERR);
	//		}
	//		
	//	}

	//	if(rect_obj->texture != NULL)
	//	{

	//		temp_rect = rect_obj->rect;
	//		
	//		if(temp_rect.w > temp_rect.h)
	//		{

	//			temp_rect.w /= 2;
	//		}
	//		else
	//		{

	//			temp_rect.h /= 2;
	//		}

	//		check = SDL_RenderTexture(win_obj->render,rect_obj->texture,NULL,&temp_rect);

	//		if(!check)
	//		{
	//			ERR_MSG("[fill_rect]texture: render failed",SDL_ERR);
	//		}
	//	}

		return;
	}

void exit_from_program(void)
	{
	
		input_text_off(main_win,&archive_input_text_field);	
		
		destroy_window_set_obj(&main_win);
			
		destroy_window_set_obj(&info_win);
			
		destroy_rectwp(&top_bar);

		destroy_rectwp(&exit_button);

		destroy_rectwp(&info_button);
		
		destroy_rectwp(&file_space);
		
		destroy_rectwp(&enc_button);

		destroy_rectwp(&dec_button);

		destroy_rectwp(&archive_name_field);

		destroy_rectwp(&archive_name_field_title);

		destroy_rectwp(&archive_name_field_input_rect);

		destroy_rectwp(&add_files_button);

				
		for(unsigned int count = 0; file_container[count].is_busy; ++count)
		{
			
			delete_file(file_container,&file_counter,count);
		}
		

		TTF_Quit();

		SDL_Quit();

		
		return;
	}
