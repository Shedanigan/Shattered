#include "Headers/Core.h"

int main(int argc, char** argv)
{
	Draw::init_Window(1280, 720, "Shattered");
	unsigned int curr_time = 0;
	unsigned int last_update_time = 0;
	char max_framerate = 60;
	int state = 0;
	int state_before_controllers_lost = 1;
	Sys::prev_space_state = 0;
	Sys::curr_space_state = 0;
	Sys::debug_toggle = 0;
	audio_state = 0;

	Draw::load_Image_To_Texture(&Draw::title_screen, Draw::renderer, "Assets/Images/UI/Title_Screen_Fin.png");
	Draw::load_Image_To_Texture(&attackbox, Draw::renderer, "Assets/Images/UI/attack_fade.png");
	Draw::load_Image_To_Texture(&collisionbox, Draw::renderer, "Assets/Images/UI/collision_fade.png");

	Draw::load_Image_To_Texture(&Draw::a_unlit, Draw::renderer, "Assets/Images/UI/A_Unlit.png");
	Draw::load_Image_To_Texture(&Draw::a_lit, Draw::renderer, "Assets/Images/UI/A_Lit.png");
	Draw::load_Image_And_Size_To_Texture(&bar_set, &bar_set_size.x, &bar_set_size.y, Draw::renderer, "Assets/Images/UI/UI_Frame.png");
	Draw::load_Image_And_Size_To_Texture(&hp_bar, &bar_size.x, &bar_size.y, Draw::renderer, "Assets/Images/UI/UI_HP.png");

	Draw::load_Image_To_Texture(&Draw::background[0], Draw::renderer, "Assets/Images/Setting/Greece_Map_01.png");
	Draw::load_Image_To_Texture(&Draw::background[1], Draw::renderer, "Assets/Images/Setting/Greece_Map_02.png");
	Draw::load_Image_To_Texture(&Draw::foreground[0], Draw::renderer, "Assets/Images/Setting/Greece_Map_01_Foreground.png");
	Draw::load_Image_To_Texture(&Draw::foreground[1], Draw::renderer, "Assets/Images/Setting/Greece_Map_02_Foreground.png");

	p1 = { 0 };
	p2 = { 0 };
	p1.input.controller = NULL;
	p2.input.controller = NULL;
	Phys::gravity = 1;

	rizer = Draw::load_EDS(Draw::renderer, "Assets/EDS/rizer.eds");
	rizer.phys_data.max_vel = { 7, 3, 20 };
	rizer.phys_data.max_health = 10;
	rizer.phys_data.i_time = 1000;
	torch = Draw::load_EDS(Draw::renderer, "Assets/EDS/torch.eds");
	torch.phys_data.max_vel = { 7, 3, 20 };
	torch.phys_data.max_health = 10;
	torch.phys_data.i_time = 1000;

	p1.instance.key = &rizer;
	p1.instance.direction_flip = SDL_FLIP_NONE;
	p1.instance.world_position = { 150, 400, 0 };
	p1.instance.key->shared_data.scale = 0.5;
	p1.instance.state = Draw::IDLE;
	p1.instance.curr_frame = rizer.draw_data.idle_start;
	p1.instance.health = p1.instance.key->phys_data.max_health;
	p1.instance.last_hit_time = 0;

	p2.instance.key = &torch;
	p2.instance.direction_flip = SDL_FLIP_NONE;
	p2.instance.world_position = { 250, 550, 0 };
	p2.instance.key->shared_data.scale = 0.5;
	p2.instance.state = Draw::IDLE;
	p2.instance.curr_frame = torch.draw_data.idle_start;
	p2.instance.health = p2.instance.key->phys_data.max_health;
	p2.instance.last_hit_time = 0;


	mob = Draw::load_EDS(Draw::renderer, "Assets/EDS/melee.eds");
	mob.phys_data.max_vel = { 4, 2, 0 };
	mob.shared_data.scale = 0.6;

	//initialize waves

	curr_wave = { 0 };
	curr_wave.n_remaining = 0;
	curr_wave.n_instances = 0;
	curr_wave.curr_frame = (char*)calloc(100, sizeof(char));
	curr_wave.depth_order = (int*)calloc(100, sizeof(int));
	curr_wave.depth_order_pos = (int*)calloc(100, sizeof(int));
	curr_wave.direction_flip = (SDL_RendererFlip*)calloc(100, sizeof(SDL_RendererFlip));
	curr_wave.key = &mob;
	curr_wave.last_frame_update = (unsigned int*)calloc(100, sizeof(unsigned int));
	curr_wave.world_position = (ISet*)calloc(100, sizeof(ISet));
	curr_wave.draw_position = (IPair*)calloc(100, sizeof(IPair));
	curr_wave.state = (char*)calloc(100, sizeof(char));
	curr_wave.velocity = (ISet*)calloc(100, sizeof(ISet));

	min_range = { 100, 10 };
	max_range = { 150, 30 };
	aim_range.x = (min_range.x + max_range.x) * 0.5;
	aim_range.y = (min_range.y + max_range.y) * 0.5;


	for (;;) //main system loop
	{
		curr_time = SDL_GetTicks();
		if (1000.0 / (curr_time - last_update_time) <= max_framerate || max_framerate <= 0)
		{
			last_update_time = curr_time;
			//game reset
			if (p1.instance.state == Draw::DEAD && p2.instance.state == Draw::DEAD)
			{
				p1.instance.state = Draw::IDLE;
				p1.instance.health = p1.instance.key->phys_data.max_health;
				p1.instance.world_position = { 150, 400, 0 };

				p2.instance.state = Draw::IDLE;
				p2.instance.health = p2.instance.key->phys_data.max_health;
				p2.instance.world_position = { 250, 550, 0 };

				Draw::cam_pos.x = 0;
				state = 1;
				state_before_controllers_lost = 1;
				curr_wave.n_instances = 0;
				curr_wave.n_remaining = 0;
			}

			//check for controllers
			if (state > 0) state_before_controllers_lost = state;
			state = Sys::check_Controllers(state_before_controllers_lost);

			if (state > 0)
			{
				if (state == 1 && p1.input.a_button_state % 2 == 1 && p2.input.a_button_state % 2 == 1) state = 2;
				Phys::update_Physics(&state, curr_time);
				Draw::update_Entity_Animation(&p1.instance, curr_time);
				Draw::update_Entity_Animation(&p2.instance, curr_time);
				if (state > 2) Draw::update_Entity_Set_Animation(&curr_wave, curr_time);
			}
			Sys::update_System(state);
			Draw::draw_Window(state);
		}
	}
	return 0;
}