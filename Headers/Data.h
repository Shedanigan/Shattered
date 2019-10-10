#pragma once
#pragma warning(disable:4996)
#pragma comment(linker, "/subsystem:console")

#include "SDL2-2.0.9/include/SDL.h"
#pragma comment(lib, "Headers\\SDL2-2.0.9\\lib\\x86\\SDL2.lib")
#pragma comment(lib, "Headers\\SDL2-2.0.9\\lib\\x86\\SDL2main.lib")
#include "SDL2-2.0.9/include/SDL_image.h"
#pragma comment (lib, "Headers\\SDL2-2.0.9\\lib\\x86\\SDL2_image.lib")

struct IPair
{
	int x, y;
};

struct ISet
{
	int x, y, z;
};

struct FSet
{
	float x, y, z;
};

namespace Sys									//keystates & core SDL
{
	struct Controller							//dpad, face buttons, and left stick
	{
		SDL_GameController* controller;
		int controller_id;
		unsigned int d_pad_up_state : 2;		//2 bit variable, left bit previous, right bit current
		unsigned int d_pad_down_state : 2;		//2 bit variable, left bit previous, right bit current
		unsigned int d_pad_left_state : 2;		//2 bit variable, left bit previous, right bit current
		unsigned int d_pad_right_state : 2;		//2 bit variable, left bit previous, right bit current
		unsigned int a_button_state : 2;		//2 bit variable, left bit previous, right bit current
		unsigned int b_button_state : 2;		//2 bit variable, left bit previous, right bit current
		unsigned int x_button_state : 2;		//2 bit variable, left bit previous, right bit current
		unsigned int y_button_state : 2;		//2 bit variable, left bit previous, right bit current
		short l_stick_x_axis;					//-32k to 32k range
		short l_stick_y_axis;					//-32k to 32k range
	};

	char curr_space_state;
	char prev_space_state;
	char debug_toggle;
}

namespace Phys									//motion and collision
{
	struct Entity_Key_Phys_Data					//physics components shared between instances
	{
		FSet max_vel;
		int max_health;
		int i_time;
		int* n_collision_boxes;
		int* n_attack_boxes;
		SDL_Rect** collision_boxes;
		SDL_Rect** attack_boxes;
	};
	int gravity;
}

namespace Draw									//printing things to the game window
{
	enum { NA = -1, DEAD = 0, IDLE, WALK, JUMP, FALL, LAND, ATTACK1, ATTACK2, CAST, DAMAGED, DYING };

	struct Entity_Key_Draw_Data					//drawing components shared between instances
	{
		SDL_Texture* sprite_sheet;
		IPair sheet_size;
		char n_cols;
		unsigned short
			idle_start,		idle_end,	idle_rate,
			jump_start,		jump_end,	jump_rate,
			fall_start,		fall_end,	fall_rate,
			land_start,		land_end,	land_rate,
			walk_start,		walk_end,	walk_rate,
			atk_1_start,	atk_1_end,	atk_1_rate,
			atk_2_start,	atk_2_end,	atk_2_rate,
			recoil_start,	recoil_end,	recoil_rate,
			die_start,		die_end,	die_rate,
			cast_start,		cast_end,	cast_rate;
	};

	short window_w;
	short window_h;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* title_screen;
	SDL_Texture* fontsheet;
	SDL_Texture* a_lit;
	SDL_Texture* a_unlit;
	SDL_Texture* background[2];
	SDL_Texture* foreground[2];
	IPair fontsheet_size, font_size, cam_pos;
}

struct Entity_Key_Shared_Data					//components used for both physics and drawing shared between instances
{
	ISet sprite_size;
	float scale;
};

struct Entity_Key								//collection of components shared between instances
{
	Phys::Entity_Key_Phys_Data phys_data;
	Draw::Entity_Key_Draw_Data draw_data;
	Entity_Key_Shared_Data shared_data;
};

struct Entity_Instance							//key and components different per instance
{
	Entity_Key* key;
	char state;
	int health;
	int last_hit_time;
	char curr_frame;
	unsigned int last_frame_update;
	IPair draw_position;
	ISet world_position;
	SDL_RendererFlip direction_flip;
	ISet velocity;
};

struct Player
{
	Entity_Instance instance;
	Sys::Controller input;
};

struct Entity_Instance_Set						//arrays of components different per instance of same entity type
{
	Entity_Key* key;
	int n_instances;
	int n_remaining;
	int *health;
	int* depth_order;
	int* depth_order_pos;
	Player** closeset_player;
	unsigned int* last_frame_update;
	char* state;
	char* curr_frame;
	IPair* draw_position;
	ISet* world_position;
	SDL_RendererFlip* direction_flip;
	ISet* velocity;
};

Entity_Key rizer;
Entity_Key torch;
Entity_Key mob;
Entity_Key boss;
Entity_Instance_Set curr_wave;
IPair min_range;
IPair max_range;
IPair aim_range;
SDL_Texture* attackbox;
SDL_Texture* collisionbox;
IPair bar_set_size;
IPair bar_size;
SDL_Texture* bar_set;
SDL_Texture* hp_bar;
char audio_state;

Player p1;
Player p2;