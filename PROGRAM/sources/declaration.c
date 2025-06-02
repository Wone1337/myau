#include "../headers/declaration.h"
#include "../headers/nfd.h"
#include "../headers/archiver.h"

//________________VAR_INIT____________________
SDL_Event main_event;
WINDOW_SET *main_win = NULL;
WINDOW_SET *info_win = NULL;
WINDOW_SET *log_win = NULL;
RECTWP *top_bar = NULL;
RECTWP *exit_button = NULL;
RECTWP *info_button = NULL;
RECTWP *log_button = NULL;
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
void archivator_init(void)
	{	


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
    		
		log_button = create_rectwp(main_win,(RGBA){78,54,234,1}, DEF_PIC_PATH "log.svg", DEF_FONT_PATH "PatrickHandSC-Regular.ttf","Log",500);

		file_space = create_rectwp(main_win,(RGBA){255,152,0,1},NULL,NULL,NULL,0);

		add_files_button = create_rectwp(main_win,(RGBA){46,204,113,1},NULL,NULL,NULL,0);
		
		enc_button = create_rectwp(main_win,(RGBA){46,204,113,1},NULL,NULL,NULL,0);

		dec_button = create_rectwp(main_win,(RGBA){46,204,113,1},NULL,NULL,NULL,0);

		
	
		return;
	}

WINDOW_SET* init_window_set_obj(WINDOW_SET *win_obj,CONFIG cfg)
	{
		
			
		//puts("HUI1\n");

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

		//puts("HUI2\n");

		win_obj->win = SDL_CreateWindow(cfg.title,cfg.w,cfg.h,cfg.flags);

		if(win_obj->win == NULL)
		{
			ERR_MSG("(init_window_set]WIN: create failed",SDL_ERR);
			destroy_window_set_obj(&win_obj);
			return win_obj;
		}


		//puts("HUI3\n");
		
		win_obj->cfg = cfg;

		win_obj->render = SDL_CreateRenderer(win_obj->win,NULL);

		if(win_obj->render == NULL)
		{
			ERR_MSG("(init_window_set]RENDER: create failed",SDL_ERR);
			destroy_window_set_obj(&win_obj);
			return win_obj;
		}


	//	puts("HUI4\n");

		win_obj->engine = TTF_CreateRendererTextEngine(win_obj->render);
		
		if(win_obj->engine == NULL)
		{
			ERR_MSG("(init_window_set]ENGINE: create failed",SDL_ERR);
			destroy_window_set_obj(&win_obj);
			return win_obj;
		}

		win_obj->check = true;

		//puts("HUI5\n");

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

		//puts("HUYASIK1\n");
		
		if(obj->render != NULL)
			SDL_DestroyRenderer(obj->render);
	
		
	//	puts("HUYASIK2\n");
		if(obj->win != NULL)
			SDL_DestroyWindow(obj->win);

		
	//	puts("HUYASIK3\n");
		obj->cfg = (CONFIG){0,0,NULL,0};
		
	//	puts("HUYASIK4\n");
		
		obj->engine = NULL;
		
	//	puts("HUYASIK5\n");

		obj->render = NULL;
	
		
	//	puts("HUYASIK6\n");
		obj->win = NULL;

		//puts("HUYASIK7\n");
		obj->check = false;

	//	puts("HUYASIK8\n");
		free(obj);

		*win_obj = NULL;
	
	//	puts("HUYASIK9\n");
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


		//puts("HUYARA1\n");	

		temp_object->rect = (SDL_FRect){0,0,0,0};

		//puts("HUYARA1.1\n");	

		temp_object->main_color = rgba;

		temp_object->texture = NULL;
		
		//temp_object->current_color = rgba;

		//temp_object->transition_color = (RGBA){temp_object->main_color.red+50,temp_object->main_color.green+50,temp_object->main_color.blue+50,temp_object->main_color.alpha};
		//puts("HUYARA1.2\n");	

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
	

		//puts("HUYARA2\n");	
		
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

		//puts("HUYARA4\n");	

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

	
			//puts("HUYARA5\n");	

		return temp_object;
	}

void destroy_rectwp(RECTWP **rect_obj)
	{
		if(rect_obj == NULL ||  (*rect_obj) == NULL)
		{
			printf("rect = %p\n",rect_obj);	
			puts("[destroy_rectwp]ZERO NOTICED\n");
			return;
		}
		
		RECTWP *obj = *rect_obj;

		obj->rect = (SDL_FRect){0,0,0,0};

		obj->main_color = (RGBA){0,0,0,0};
		
		//obj->transition_color = (RGBA){0,0,0,0};
		
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

		//ПРОБЛЕМЫ	
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
		    
		    //printf("index = %d\n",index); 

		    if (index >= file_counter)
		    {
			    break;
		    }
		
		    //puts("SOS!\n");
			
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
		
		//printf("rect = %p\n",file_obj->file_rect);	
		
		destroy_rectwp(&file_obj[index].file_rect);


		
		//printf("rect = %p\n",file_obj->file_rect);	
		file_obj[index].is_busy = false;
		
		//printf("rect = %p\n",file_obj->file_rect);	

		
		//printf("rect = %p bool = %u\n",file_obj[index].file_rect,file_obj[index].is_busy);	
		
		file_obj[index] = file_obj[*size-1];

		//printf("rect = %p bool = %u\n",file_obj[index].file_rect,file_obj[index].is_busy);	
		
		file_obj[*size-1].is_busy = false;
		
		file_obj[*size-1].file_rect = NULL;

		--(*size);
		
		return;
	}

void input_text_on(WINDOW_SET *win_obj,INPUT **text_obj)
	{
		SDL_StartTextInput(win_obj->win);

		(*text_obj) = (INPUT*)malloc(sizeof(INPUT));

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

		if(text_obj || (*text_obj))
		{
			puts("[input_text_off]NOTICED NULL\n");
			return;
		}

		SDL_StopTextInput(win_obj->win);
		
		TTF_CloseFont((*text_obj)->font);

		(*text_obj)->font = NULL;

		if((*text_obj)->texture != NULL)
		{
			SDL_DestroyTexture((*text_obj)->texture);
		}

		(*text_obj)->texture = NULL;
		
		if((*text_obj)->surface != NULL)
		{
			SDL_DestroySurface((*text_obj)->surface);
		}

		(*text_obj)->surface = NULL;
		
		(*text_obj)->rect = (SDL_FRect){0,0,0,0}; 
		
		free((*text_obj));
		
		puts("MYAU2.1\n");
		(*text_obj) = NULL;

		puts("MYAU2.2\n");
		
		text_obj = NULL;

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
   				     
			++current_ptr; // переходим к следующему элементу массива указателей
   		}
   		 
   		snprintf(fn, sizeof(fn), "%s%s%s", DEF_PATH_OUTPUT_TMP, file_name, ".myzip");

   		create_archive(fn, ptr, file_counter);
   		 
   		free(ptr);
    				
		return;
	}

void decode_files(void)
	{
		if(file_counter <= 0)
		{
			return;
		}

		extract_archive(file_container[0].file_rect->rect_text->title,"/");

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
			log_button->rect.x = info_button->rect.x + info_button->rect.w + gap, log_button->rect.y = top_bar->rect.y, log_button->rect.w = info_button->rect.w, log_button->rect.h = top_bar->rect.h; 

			file_space->rect.x = 30 ,file_space->rect.y = top_bar->rect.h+gap, file_space->rect.w = main_win->cfg.w-file_space->rect.x*2, file_space->rect.h = main_win->cfg.h-(main_win->cfg.h/2);

			enc_button->rect.x = file_space->rect.x ,enc_button->rect.y = file_space->rect.y + file_space->rect.h + gap ,enc_button->rect.w = file_space->rect.w/2, enc_button->rect.h = main_win->cfg.h/8;
			
			dec_button->rect.x = enc_button->rect.x + enc_button->rect.w + gap, dec_button->rect.y = enc_button->rect.y, dec_button->rect.w = enc_button->rect.w - gap, dec_button->rect.h = enc_button->rect.h;

			add_files_button->rect.x = enc_button->rect.x , add_files_button->rect.y = enc_button->rect.y + enc_button->rect.h + gap, add_files_button->rect.w = enc_button->rect.w + dec_button->rect.w + gap, add_files_button->rect.h = enc_button->rect.h;
			
			if(archive_name_field != NULL)
			archive_name_field->rect.x = archive_name_field->rect.x ,archive_name_field->rect.y = file_space->rect.y + file_space->rect.h + gap ,archive_name_field->rect.w = file_space->rect.w/2, archive_name_field->rect.h = main_win->cfg.h/8;

		}
		else if(main_win->cfg.w <= 1920 || main_win->cfg.h <= 1080)
		{
			gap = main_win->cfg.h/40;
			
			top_bar->rect.x = 0,top_bar->rect.y = 0,top_bar->rect.w = main_win->cfg.w/gap,top_bar->rect.h = main_win->cfg.h;
			
			exit_button->rect.x = top_bar->rect.x , exit_button->rect.y = top_bar->rect.y , exit_button->rect.w = top_bar->rect.w ,exit_button->rect.h = 100;
			
			info_button->rect.x = top_bar->rect.x , info_button->rect.y = top_bar->rect.y + exit_button->rect.h + gap , info_button->rect.w = exit_button->rect.w, info_button->rect.h = exit_button->rect.h;
			
			log_button->rect.x = top_bar->rect.x, log_button->rect.y = info_button->rect.y + info_button->rect.h + gap, log_button->rect.w = info_button->rect.w, log_button->rect.h = info_button->rect.h; 

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
					puts("pisya\n");
						
					if(main_win && main_win->win && main_event.window.windowID == SDL_GetWindowID(main_win->win))
					{
						main_win->check = false;
					}

					if(info_win && info_win->win && main_event.window.windowID == SDL_GetWindowID(info_win->win))
					{
						destroy_window_set_obj(&info_win);

					}

					if(log_win && log_win->win && main_event.window.windowID == SDL_GetWindowID(log_win->win))
					{
						destroy_window_set_obj(&log_win);		
						
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

				if( (key_state[SDL_SCANCODE_LCTRL]) && (key_state[SDL_SCANCODE_L]))
				{
					if(log_win == NULL)
					{	
						CONFIG log_cfg = {WIN_WIDTH,WIN_HEIGHT,"LOG WIN",0};
						log_win = init_window_set_obj(log_win,log_cfg);
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

						//delete_file(file_container,&file_counter,count);

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

					puts("MYAU\n");
					if(strlen(input_keyboard) > 0)
					{

						puts("MYAU1\n");
						destroy_rectwp(&archive_name_field);
							
						puts("MYAU2\n");
						
						input_text_off(main_win,&archive_input_text_field);

						encode_files(input_keyboard);
						
						puts("MYAU3\n");
						//encode();
						memset(&input_keyboard,0,sizeof(input_keyboard));
					}

					
				}

						
			}

			

			if(main_event.type == SDL_EVENT_TEXT_INPUT)
			{
				strncat(input_keyboard, main_event.text.text, sizeof(input_keyboard) - strlen(input_keyboard) - 1);
				printf("str = %s\n",input_keyboard);
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

				if(((log_button->rect.x <= d) && (d <= log_button->rect.w + log_button->rect.x))  &&  ( (log_button->rect.y <= e) && (e <= log_button->rect.h + log_button->rect.y ))  )
				{
					if(log_win == NULL)
					{	
						CONFIG log_cfg = {WIN_WIDTH,WIN_HEIGHT,"LOG WIN",0};
						log_win = init_window_set_obj(log_win,log_cfg);
					}	
				}

				if(index_to_transfer != -1)				
				{		
					delete_file(file_container,&file_counter,index_to_transfer);

					printf("fc = %d\n",index_to_transfer);
				}

				if(((add_files_button->rect.x <= d) && (d <= add_files_button->rect.w + add_files_button->rect.x))  &&  ( (add_files_button->rect.y <= e) && (e <= add_files_button->rect.h + add_files_button->rect.y)))
				{
					puts("MYAUUUUUU\n");

					add_file_from_dialog(main_win,file_space);
				}

				if(((enc_button->rect.x <= d) && (d <= enc_button->rect.w + enc_button->rect.x))  &&  ( (enc_button->rect.y <= e) && (e <= enc_button->rect.h + enc_button->rect.y)))
				{
												
					archive_name_field = create_rectwp(main_win,(RGBA){65,76,198,1},NULL,NULL,NULL,0);
					input_text_on(main_win,&archive_input_text_field);
					
				}

				if(((dec_button->rect.x <= d) && (d <= dec_button->rect.w + dec_button->rect.x))  &&  ( (dec_button->rect.y <= e) && (e <= dec_button->rect.h + dec_button->rect.y)))
				{
					decode_files();	
					puts("DECODE\n");
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

		fill_rect(main_win,log_button,log_button->main_color);
	
		fill_rect(main_win,file_space,file_space->main_color);			
		
		fill_rect(main_win,enc_button,enc_button->main_color);
		
		if(archive_name_field != NULL)
			fill_rect(main_win,archive_name_field,archive_name_field->main_color);
		
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

		if(log_win && log_win->check && log_win->render)
		{
			SDL_RenderClear(log_win->render);
			SDL_RenderPresent(log_win->render);

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
	
		puts("HUI1\n");
		
		destroy_window_set_obj(&main_win);
			
		puts("HUI2\n");
		destroy_window_set_obj(&info_win);
			
		puts("HUI3\n");
		destroy_window_set_obj(&log_win);
		
		puts("HUI4\n");
		destroy_rectwp(&top_bar);

		puts("HUI5\n");
		//puts("MEEEEEEEOW_HUIIIIII 1\n");

		destroy_rectwp(&exit_button);
		
		puts("HUI6\n");
	//	puts("MEEEEEEEOW_HUIIIIII 2\n");
		destroy_rectwp(&info_button);
		
		puts("HUI7\n");
	//	puts("MEEEEEEEOW_HUIIIIII 3\n");
		destroy_rectwp(&log_button);
		
		puts("HUI8\n");
		destroy_rectwp(&file_space);
		
		puts("HUI9\n");
	//	puts("MEEEEEEEOW_HUIIIIII 4\n");
		
		
		destroy_rectwp(&enc_button);

		destroy_rectwp(&dec_button);

		destroy_rectwp(&archive_name_field);
		
		puts("HUI10\n");
	//	puts("MEEEEEEEOW_HUIIIIII 5\n");
		destroy_rectwp(&add_files_button);

		puts("HUI_GO\n");	
				
		for(unsigned int count = 0; file_container[count].is_busy; ++count)
		{
			printf("cnt = %d file_counter_b = %u\n",count,file_container[count].is_busy);
			delete_file(file_container,&file_counter,count);
		}
		
		input_text_off(main_win,&archive_input_text_field);	

		puts("SOS\n");

	//	puts("MEEEEEEEOW_HUIIIIII 6\n");
		TTF_Quit();

		SDL_Quit();

		
		return;
	}
