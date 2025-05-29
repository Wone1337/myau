#include "../headers/declaration.h"
#include "../headers/nfd.h"

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
RECTWP *delete_file_from_list = NULL;
RECTWP *enc_dec_button = NULL;
RECTWP *add_files_button = NULL;
FILE_MAN file_container[FILE_BUF_SIZE];



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
		
		exit_button = create_rectwp(main_win,(RGBA){78,54,234,1},"/home/Bobik/Документы/DIPLOMA_WORK/PROGRAM/pic/cross.svg","/home/Bobik/Документы/DIPLOMA_WORK/PROGRAM/fonts/ChivoMono-VariableFont_wght.ttf","Exit",500);

		info_button = create_rectwp(main_win,(RGBA){78,54,234,1},"/home/Bobik/Документы/DIPLOMA_WORK/PROGRAM/pic/info.svg","/home/Bobik/Документы/DIPLOMA_WORK/PROGRAM/fonts/ChivoMono-VariableFont_wght.ttf","Info",500);
    		
		log_button = create_rectwp(main_win,(RGBA){78,54,234,1},"/home/Bobik/Документы/DIPLOMA_WORK/PROGRAM/pic/log.svg","/home/Bobik/Документы/DIPLOMA_WORK/PROGRAM/fonts/ChivoMono-VariableFont_wght.ttf","Log",500);

		file_space = create_rectwp(main_win,(RGBA){255,152,0,1},NULL,NULL,NULL,0);
			
		//delete_file_from_list = create_rectwp(255, 64, 0, 1,NULL,NULL,NULL,0);
  
		add_files_button = create_rectwp(main_win,(RGBA){46,204,113,1},NULL,NULL,NULL,0);
		
		enc_dec_button = create_rectwp(main_win,(RGBA){46,204,113,1},NULL,NULL,NULL,0);
		
	
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
		temp_object->rect_text->title = NULL;
		temp_object->rect_text->text_texture = NULL;
		temp_object->rect_text->title_size = 0;
		temp_object->rect_text->text_scale = 0;
		//puts("HUYARA3\n");	

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
		if(rect_obj == NULL ||  *rect_obj == NULL)
		{
			
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

		if(obj->rect_text->text_texture->surface != NULL)
		{
			SDL_DestroySurface(obj->rect_text->text_texture->surface);
		}


		obj->rect_text->text_texture->dest_rect = (SDL_FRect){0,0,0,0};

		
		free(obj->rect_text->text_texture);
		
		obj->rect_text->text_texture = NULL;

		free(obj->rect_text);
	
		obj->rect_text = NULL;

		free(obj);
		
		*rect_obj = NULL;

		return;
	}

void add_file(WINDOW_SET *win_obj,RECTWP *rect_obj,const char *file_name)
	{	
		unsigned int counter = 0;

		unsigned int gap_between_files = 10;

		while(file_container[counter].is_busy == true && counter < FILE_BUF_SIZE)
		{
			counter++;
		}

		printf("hui = %u",counter);

		RECTWP *file_rect_obj = create_rectwp(win_obj,(RGBA){0,0,0,0},"/home/Bobik/Документы/DIPLOMA_WORK/PROGRAM/pic/cross.svg","/home/Bobik/Документы/DIPLOMA_WORK/PROGRAM/fonts/ChivoMono-VariableFont_wght.ttf",file_name,60);

		puts("HUYAMBA_start\n");

		if(counter == 0)
		{	
			SDL_FRect temp_rect = rect_obj->rect;
			
			temp_rect.w -= 50;

			temp_rect.h  = 25;

			file_rect_obj->rect_text->text_texture->dest_rect = temp_rect;
	
			temp_rect.x += temp_rect.w;

			temp_rect.w = rect_obj->rect.w - temp_rect.w;
			
			file_rect_obj->rect = temp_rect;
			
			puts("HUYAMBA\n");
		}
		else
		{	
			file_rect_obj->rect = file_container[counter-1].file_rect->rect; 
			file_rect_obj->rect_text->text_texture->dest_rect = file_container[counter-1].file_rect->rect_text->text_texture->dest_rect;
			file_rect_obj->rect.y += file_rect_obj->rect.h + gap_between_files;  
			file_rect_obj->rect_text->text_texture->dest_rect.y += file_rect_obj->rect_text->text_texture->dest_rect.h + gap_between_files;
		
			
			puts("HUYAMBA\n");
		}

		file_container[counter] = (FILE_MAN){file_rect_obj,true};  

		puts("HUYAMBA\n");

		return;
	}

void interface_appear(void)
	{
	
		int gap = 0;

		if(main_win && main_win->win)
		{
			SDL_GetWindowSize(main_win->win,&main_win->cfg.w,&main_win->cfg.h);
		}
		//INTERFACE 

		if(main_win->cfg.w <= 854 || main_win->cfg.h <= 458)
		{	
				
			gap = main_win->cfg.w/40;
			
			top_bar->rect.x = 0,top_bar->rect.y = 0,top_bar->rect.w = main_win->cfg.w,top_bar->rect.h = main_win->cfg.h/8-gap;
			
			exit_button->rect.x = top_bar->rect.x , exit_button->rect.y = top_bar->rect.y , exit_button->rect.w = 100 ,exit_button->rect.h = top_bar->rect.h ;

			info_button->rect.x = top_bar->rect.x + exit_button->rect.w + gap , info_button->rect.y = top_bar->rect.y , info_button->rect.w = exit_button->rect.w,info_button->rect.h = top_bar->rect.h;
			log_button->rect.x = info_button->rect.x + info_button->rect.w + gap, log_button->rect.y = top_bar->rect.y, log_button->rect.w = info_button->rect.w, log_button->rect.h = top_bar->rect.h; 

			file_space->rect.x = 30 ,file_space->rect.y = top_bar->rect.h+gap, file_space->rect.w = main_win->cfg.w-file_space->rect.x*2, file_space->rect.h = main_win->cfg.h-(main_win->cfg.h/2);

			enc_dec_button->rect.x = file_space->rect.x ,enc_dec_button->rect.y = file_space->rect.y + file_space->rect.h + gap ,enc_dec_button->rect.w = file_space->rect.w, enc_dec_button->rect.h = main_win->cfg.h/8;

			add_files_button->rect.x = enc_dec_button->rect.x , add_files_button->rect.y = enc_dec_button->rect.y + enc_dec_button->rect.h + gap, add_files_button->rect.w = enc_dec_button->rect.w , add_files_button->rect.h = enc_dec_button->rect.h;
			
		}
		else if(main_win->cfg.w <= 1920 || main_win->cfg.h <= 1080)
		{
			gap = main_win->cfg.h/40;
			
			top_bar->rect.x = 0,top_bar->rect.y = 0,top_bar->rect.w = main_win->cfg.w/gap,top_bar->rect.h = main_win->cfg.h;
			
			exit_button->rect.x = top_bar->rect.x , exit_button->rect.y = top_bar->rect.y , exit_button->rect.w = top_bar->rect.w ,exit_button->rect.h = 100;
			
			info_button->rect.x = top_bar->rect.x , info_button->rect.y = top_bar->rect.y + exit_button->rect.h + gap , info_button->rect.w = exit_button->rect.w, info_button->rect.h = exit_button->rect.h;
			
			log_button->rect.x = top_bar->rect.x, log_button->rect.y = info_button->rect.y + info_button->rect.h + gap, log_button->rect.w = info_button->rect.w, log_button->rect.h = info_button->rect.h; 

			file_space->rect.x = top_bar->rect.w+gap ,file_space->rect.y = 30 ,file_space->rect.w = main_win->cfg.w-(main_win->cfg.w/3), file_space->rect.h = main_win->cfg.h-file_space->rect.y*2;

			enc_dec_button->rect.x = file_space->rect.x + file_space->rect.w + gap + gap ,enc_dec_button->rect.y = file_space->rect.y ,enc_dec_button->rect.w = file_space->rect.w/3-gap, enc_dec_button->rect.h = file_space->rect.h/2;

			add_files_button->rect.x = enc_dec_button->rect.x, add_files_button->rect.y = enc_dec_button->rect.y+enc_dec_button->rect.h+gap, add_files_button->rect.w = enc_dec_button->rect.w , add_files_button->rect.h = enc_dec_button->rect.h-gap;


		}
	//	else if(cfg->w <= 1280 || cfg->h <= 1024)
	//	{

	//	}
	//	else if(cfg->w <= 1600 || cfg->h <= 900)
	//	{

	//	}
	//	else
	//	{

	//	}

	
	
		
		float d = 0;
		float e = 0;
		bool *key_state;

		while(SDL_PollEvent(&main_event))
		{
			
								


			//printf("d = %f e = %f\n\r",d,e);
	

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
puts("pisya\n");
					if(info_win && info_win->win && main_event.window.windowID == SDL_GetWindowID(info_win->win))
					{
						destroy_window_set_obj(&info_win);

					}
puts("pisya\n");
					if(log_win && log_win->win &&main_event.window.windowID == SDL_GetWindowID(log_win->win))
					{
						destroy_window_set_obj(&log_win);		
						
					}

puts("pisya\n");
				}
			
			}


			if(main_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
			{
				
				SDL_GetMouseState(&d,&e);


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


			}	
		

			if(main_event.type == SDL_EVENT_DROP_FILE)
			{
				
				printf("HUI = %s\n",main_event.drop.data);
				add_file(main_win,file_space,main_event.drop.data);
				puts("HUYAMBA_end\n");
			
			}

		}

	
		SDL_SetRenderDrawColor(main_win->render,41, 41, 40, 1);
		
		SDL_RenderClear(main_win->render);
		
		
		fill_rect(main_win,top_bar,top_bar->main_color);	
		
		fill_rect(main_win,exit_button,exit_button->main_color);	
		
		fill_rect(main_win,info_button,info_button->main_color);

		fill_rect(main_win,log_button,log_button->main_color);
	
		fill_rect(main_win,file_space,file_space->main_color);			
		
		fill_rect(main_win,enc_dec_button,enc_dec_button->main_color);	
		
		fill_rect(main_win,add_files_button,add_files_button->main_color);
		
		int s = 0;

		while(file_container[s].is_busy == true)
		{
			SDL_RenderTexture(main_win->render,file_container[s].file_rect->texture,NULL,&file_container[s].file_rect->rect);
			SDL_RenderTexture(main_win->render,file_container[s].file_rect->rect_text->text_texture->texture,NULL,&file_container[s].file_rect->rect_text->text_texture->dest_rect);
			++s;
		}
		
		//puts("pisya\n");
	
		// RENDER ALL OBJECT
		SDL_RenderPresent(main_win->render);
		//SDL_Delay(16);
		
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

		//puts("pisya\n");

		return;
	}

//float linear_interpolation(float color_first,float color_second,float trans_speed)
//	{
//		
//		return color_first + (color_second - color_first) * trans_speed; 
//	}
//
//void color_transition(RECTWP *rect_obj,float speed)
//	{
//		float r,g,b,a;
//
//		r = linear_interpolation(rect_obj->main_color.red,rect_obj->transition_color.red,speed);
//		
//		g = linear_interpolation(rect_obj->main_color.green,rect_obj->transition_color.green,speed);
//
//		b = linear_interpolation(rect_obj->main_color.blue,rect_obj->transition_color.blue,speed);
// 
//		a = linear_interpolation(rect_obj->main_color.alpha,rect_obj->transition_color.alpha,speed);
//		
//		rect_obj->main_color = (RGBA){r,g,b,a};
//						
//	}


void fill_rect(WINDOW_SET *win_obj,RECTWP *rect_obj, RGBA obj_color)
	{
		//bool check;
		//SDL_FRect temp_rect;

	
		SDL_SetRenderDrawColor(win_obj->render,obj_color.red,obj_color.green,obj_color.blue,obj_color.alpha);
			
		SDL_RenderFillRect(win_obj->render,&rect_obj->rect);


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
		destroy_window_set_obj(&main_win);
			
		destroy_window_set_obj(&info_win);
			
		destroy_window_set_obj(&log_win);
		
		destroy_rectwp(&top_bar);

		//puts("MEEEEEEEOW_HUIIIIII 1\n");

		destroy_rectwp(&exit_button);
		
	//	puts("MEEEEEEEOW_HUIIIIII 2\n");
		destroy_rectwp(&info_button);
		
	//	puts("MEEEEEEEOW_HUIIIIII 3\n");
		destroy_rectwp(&log_button);
		
		destroy_rectwp(&file_space);
		
	//	puts("MEEEEEEEOW_HUIIIIII 4\n");
		//destroy_rectwp(delete_file_from_list);
		
		destroy_rectwp(&enc_dec_button);
		
	//	puts("MEEEEEEEOW_HUIIIIII 5\n");
		destroy_rectwp(&add_files_button);

		for(unsigned int count = 0; file_container[count].is_busy; ++count)
		{
			destroy_rectwp(&file_container[count].file_rect);
		}

	//	puts("MEEEEEEEOW_HUIIIIII 6\n");
		TTF_Quit();

		SDL_Quit();

		
		return;
	}
