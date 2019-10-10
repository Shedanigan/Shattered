#pragma once
#include "Audio.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

float find_Distance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow(((float)x2 - (float)x1), 2.0) + pow(((float)y2 - (float)y1), 2.0));
}

namespace Sys
{
	SDL_GameController* find_Any_Controller()
	{
		SDL_GameController* found_controller = NULL;
		int n_found_controllers = SDL_NumJoysticks();
		for (int i = 0; i < n_found_controllers; i++)
		{
			if (SDL_IsGameController(i)) return SDL_GameControllerOpen(i);
		}
		return found_controller;
	}

	SDL_GameController* find_Available_Controller(short taken_product_id)
	{
		SDL_GameController* found_controller = NULL;
		int n_found_controllers = SDL_NumJoysticks();
		for (int i = 0; i < n_found_controllers; i++)
		{
			if (SDL_IsGameController(i))
			{
				found_controller = SDL_GameControllerOpen(i);
				short found_controller_product_id = SDL_GameControllerGetProduct(found_controller);
				int is_same = true;
				//for (int i = 0; i < strlen(taken_controller); i++) if ()
				if (found_controller_product_id != taken_product_id) return found_controller;
				found_controller = NULL;
			}
		}
		return found_controller;
	}

	int check_Controllers(int start_state)	//make sure controllers are connected, and pause game to fix if they're not
	{
		int state = 0;
		if (p1.input.controller == NULL && p2.input.controller == NULL)
		{
			p1.input.controller = Sys::find_Any_Controller();
			p2.input.controller = Sys::find_Available_Controller(SDL_GameControllerGetProduct(p1.input.controller));
		}
		else if (p1.input.controller == NULL) p1.input.controller = Sys::find_Available_Controller(SDL_GameControllerGetProduct(p2.input.controller));
		else if (p2.input.controller == NULL) p2.input.controller = Sys::find_Available_Controller(SDL_GameControllerGetProduct(p1.input.controller));
		else state = start_state;

		if (p1.input.controller == NULL) state = 0;
		else state = start_state;
		if (p2.input.controller == NULL) state = 0;
		else state = start_state;

		return state;
	}

	void update_Controller(Controller* input)
	{
		input->l_stick_x_axis = SDL_GameControllerGetAxis(input->controller, SDL_CONTROLLER_AXIS_LEFTX);
		input->l_stick_y_axis = SDL_GameControllerGetAxis(input->controller, SDL_CONTROLLER_AXIS_LEFTY);

		input->a_button_state = input->a_button_state << 1;
		if (SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_A) == 1) input->a_button_state++;
		input->b_button_state = input->b_button_state << 1;
		if (SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_B) == 1) input->b_button_state++;
		input->x_button_state = input->x_button_state << 1;
		if (SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_X) == 1) input->x_button_state++;
		input->y_button_state = input->y_button_state << 1;
		if (SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_Y) == 1) input->y_button_state++;

		input->d_pad_up_state = input->d_pad_up_state << 1;
		if (SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_DPAD_UP) == 1) input->d_pad_up_state++;
		input->d_pad_down_state = input->d_pad_down_state << 1;
		if (SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == 1) input->d_pad_down_state++;
		input->d_pad_left_state = input->d_pad_left_state << 1;
		if (SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == 1) input->d_pad_left_state++;
		input->d_pad_right_state = input->d_pad_right_state << 1;
		if (SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == 1) input->d_pad_right_state++;
	}

	void update_System(int state)
	{
		prev_space_state = curr_space_state;
		SDL_Event event;
		while (SDL_PollEvent(&event) == 1)
		{
			if (event.type == SDL_QUIT) exit(0);
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE) exit(0);
				else if (event.key.keysym.sym == SDLK_SPACE) curr_space_state = 1;
			}
			else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE) curr_space_state = 0;
			else if (event.type == SDL_CONTROLLERDEVICEREMOVED)
			{
				if (SDL_GameControllerGetAttached(p1.input.controller) == SDL_FALSE)
				{
					SDL_GameControllerClose(p1.input.controller);
					p1.input.controller = NULL;
				}
				else if (SDL_GameControllerGetAttached(p2.input.controller) == SDL_FALSE)
				{
					SDL_GameControllerClose(p2.input.controller);
					p2.input.controller = NULL;
				}
			}
		}
		if (prev_space_state == 0 && curr_space_state == 1)
		{
			if (debug_toggle == 0) debug_toggle = 1;
			else debug_toggle = 0;
		}
		if (state > 0)
		{
			update_Controller(&p1.input);
			update_Controller(&p2.input);
			if (state == 2 && audio_state == 0)
			{
				Audio::play_Audio("Assets/Music/Greece_Level_Music.wav");
				audio_state = 1;
			}
			/*
			if (state == 3 && audio_state == 1)
			{
				Audio::swap_Audio("Assets/Music/Hydra_Fight.wav");
				audio_state = 2;
			}
			*/
		}
	}
}

namespace Phys
{
	void update_Player_Noninput_Velocity(Player* player)
	{
		if (player->instance.world_position.z > 0) player->instance.velocity.z -= gravity;
	}

	void update_Player_Input_Velocity(Player* player)
	{
		if (player->input.l_stick_x_axis > 10000 || player->input.l_stick_x_axis < -10000 || player->input.l_stick_y_axis > 10000 || player->input.l_stick_y_axis < -10000)
		{
			if (player->input.l_stick_x_axis > 10000) player->instance.velocity.x = player->instance.key->phys_data.max_vel.x * (player->input.l_stick_x_axis - 10000) / 22768;
			else if (player->input.l_stick_x_axis < -10000) player->instance.velocity.x = player->instance.key->phys_data.max_vel.x * (player->input.l_stick_x_axis + 10000) / 22768;
			else player->instance.velocity.x = 0;
			if (player->input.l_stick_y_axis > 10000) player->instance.velocity.y = player->instance.key->phys_data.max_vel.x * (player->input.l_stick_y_axis - 10000) / 22768;
			else if (player->input.l_stick_y_axis < -10000) player->instance.velocity.y = player->instance.key->phys_data.max_vel.x * (player->input.l_stick_y_axis + 10000) / 22768;
			else player->instance.velocity.y = 0;
		}
		else
		{
			player->instance.velocity.x = 0;
			player->instance.velocity.y = 0;
		}
		if (player->input.d_pad_left_state) player->instance.velocity.x = -1 * player->instance.key->phys_data.max_vel.x;
		else if (player->input.d_pad_right_state) player->instance.velocity.x = player->instance.key->phys_data.max_vel.x;

		if (player->input.d_pad_up_state) player->instance.velocity.y = -1 * player->instance.key->phys_data.max_vel.x;
		else if (player->input.d_pad_down_state) player->instance.velocity.y = player->instance.key->phys_data.max_vel.x;

		if (player->input.a_button_state << 1 == 2 && player->input.a_button_state >> 1 == 0 && player->instance.world_position.z == 0) player->instance.velocity.z = player->instance.key->phys_data.max_vel.z;
	}

	void update_Player_Position(Player* player)
	{
		player->instance.world_position.x += player->instance.velocity.x;
		player->instance.world_position.y += player->instance.velocity.y;
		player->instance.world_position.z += player->instance.velocity.z;
		player->instance.draw_position.x = player->instance.world_position.x - Draw::cam_pos.x - (player->instance.key->shared_data.sprite_size.x * player->instance.key->shared_data.scale * 0.5);
		player->instance.draw_position.y = player->instance.world_position.y - player->instance.world_position.z - (player->instance.key->shared_data.sprite_size.y * player->instance.key->shared_data.scale);
	}

	void update_Player_State(Player* player)
	{
		if (player->instance.state == Draw::DAMAGED || player->instance.state == Draw::DYING || player->instance.state == Draw::DEAD)
		{

		}
		else if (player->instance.velocity.z > 0) player->instance.state = Draw::JUMP;
		else if (player->instance.velocity.z < 0)
		{
			if (player->instance.world_position.z <= 0)
			{
				player->instance.state = Draw::LAND;
				player->instance.world_position.z = 0;
				player->instance.velocity.z = 0;
			}
			else player->instance.state = Draw::FALL;
		}
		else if (player->instance.state != Draw::LAND)
		{
			if (player->input.x_button_state << 1 == 2 && player->input.x_button_state >> 1 == 0 && player->instance.world_position.z == 0)
			{
				if (player->instance.curr_frame == player->instance.key->draw_data.atk_1_end) player->instance.state = Draw::ATTACK2;
				else if (player->instance.state != Draw::ATTACK2) player->instance.state = Draw::ATTACK1;
			}
			else if (player->instance.state != Draw::ATTACK1 && player->instance.state != Draw::ATTACK2)
			{
				if (player->instance.velocity.x != 0)
				{
					player->instance.state = Draw::WALK;
					if (player->instance.velocity.x > 0) player->instance.direction_flip = SDL_FLIP_NONE;
					else player->instance.direction_flip = SDL_FLIP_HORIZONTAL;
				}
				else if (player->instance.velocity.y != 0) player->instance.state = Draw::WALK;
				else player->instance.state = Draw::IDLE;
			}
		}
	}

	void check_Collision(Entity_Instance* player, int instance_id, unsigned int curr_time)
	{
		if (curr_wave.state[instance_id] == Draw::DAMAGED || curr_wave.state[instance_id] == Draw::DYING || curr_wave.state[instance_id] == Draw::DEAD ||
			player->state == Draw::DAMAGED || player->state == Draw::DYING || player->state == Draw::DEAD) return;

		Entity_Key* player_key = player->key;
		IPair player_pos = player->draw_position;
		IPair enemy_pos = curr_wave.draw_position[instance_id];

		//player attack
		if (player->state == Draw::ATTACK1 || player->state == Draw::ATTACK2)
		{
			for (int i = 0; i < player_key->phys_data.n_attack_boxes[player->curr_frame]; i++)
			{
				//printf("%d\n", i);
				SDL_Rect b1 = player_key->phys_data.attack_boxes[player->curr_frame][i];

				if (player->direction_flip == SDL_FLIP_HORIZONTAL) b1.x = b1.x = player_key->shared_data.sprite_size.x - (player_key->phys_data.attack_boxes[player->curr_frame][i].x + player_key->phys_data.attack_boxes[player->curr_frame][i].w);
				
				b1.x *= player_key->shared_data.scale;
				b1.y *= player_key->shared_data.scale;
				b1.w *= player_key->shared_data.scale;
				b1.h *= player_key->shared_data.scale;

				b1.x += player_pos.x;
				b1.y += player_pos.y;

				for (int j = 0; j < curr_wave.key->phys_data.n_collision_boxes[curr_wave.curr_frame[instance_id]]; j++)
				{
					SDL_Rect b2 = curr_wave.key->phys_data.collision_boxes[curr_wave.curr_frame[instance_id]][j];

					if (curr_wave.direction_flip[i] == SDL_FLIP_HORIZONTAL) b2.x = curr_wave.key->shared_data.sprite_size.x - (curr_wave.key->phys_data.collision_boxes[curr_wave.curr_frame[instance_id]][j].x + curr_wave.key->phys_data.collision_boxes[curr_wave.curr_frame[instance_id]][j].w);

					b2.x *= curr_wave.key->shared_data.scale;
					b2.y *= curr_wave.key->shared_data.scale;
					b2.w *= curr_wave.key->shared_data.scale;
					b2.h *= curr_wave.key->shared_data.scale;

					b2.x += enemy_pos.x;
					b2.y += enemy_pos.y;
					if (b1.x < b2.x + b2.w && b1.x + b1.w > b2.x && b1.y < b2.y + b2.h && b1.y + b1.h > b2.y)
					{
						curr_wave.state[instance_id] = Draw::DYING;
						return;
					}
				}
			}
		}

		if (curr_time - player->last_hit_time < player_key->phys_data.i_time) return;
		//enemy attack
		if ((curr_wave.state[instance_id] == Draw::ATTACK1 || curr_wave.state[instance_id] == Draw::ATTACK2) && player->world_position.y - curr_wave.world_position[instance_id].y < max_range.y && player->world_position.y - curr_wave.world_position[instance_id].y > -1 * max_range.y)
		{
			for (int i = 0; i < curr_wave.key->phys_data.n_attack_boxes[curr_wave.curr_frame[instance_id]]; i++)
			{
				SDL_Rect b1 = curr_wave.key->phys_data.attack_boxes[curr_wave.curr_frame[instance_id]][i];

				if (curr_wave.direction_flip[i] == SDL_FLIP_HORIZONTAL) b1.x = curr_wave.key->shared_data.sprite_size.x - (curr_wave.key->phys_data.attack_boxes[curr_wave.curr_frame[instance_id]][i].x + curr_wave.key->phys_data.attack_boxes[curr_wave.curr_frame[instance_id]][i].w);

				b1.x *= curr_wave.key->shared_data.scale;
				b1.y *= curr_wave.key->shared_data.scale;
				b1.w *= curr_wave.key->shared_data.scale;
				b1.h *= curr_wave.key->shared_data.scale;

				b1.x += enemy_pos.x;
				b1.y += enemy_pos.y;
				for (int j = 0; j < player_key->phys_data.n_collision_boxes[player->curr_frame]; j++)
				{
					SDL_Rect b2 = player_key->phys_data.collision_boxes[player->curr_frame][j];

					if (player->direction_flip == SDL_FLIP_HORIZONTAL) b2.x = player_key->shared_data.sprite_size.x - (player_key->phys_data.collision_boxes[player->curr_frame][j].x + player_key->phys_data.collision_boxes[player->curr_frame][j].w);

					b2.x *= player_key->shared_data.scale;
					b2.y *= player_key->shared_data.scale;
					b2.w *= player_key->shared_data.scale;
					b2.h *= player_key->shared_data.scale;

					b2.x += player_pos.x;
					b2.y += player_pos.y;
					if (b1.x < b2.x + b2.w && b1.x + b1.w > b2.x && b1.y < b2.y + b2.h && b1.y + b1.h > b2.y)
					{
						if (player_pos.x < enemy_pos.x) player->direction_flip = SDL_FLIP_NONE;
						else player->direction_flip = SDL_FLIP_HORIZONTAL;
						player->last_hit_time = curr_time;
						player->health--;
						player->state = Draw::DAMAGED;
						if (player->health <= 0) player->state = Draw::DYING;
						return;
					}
				}
			}
		}
	}

	void check_Collision_Vs_Wave(Entity_Instance* player, unsigned int curr_time)
	{
		for (int i = 0; i < curr_wave.n_instances; i++) if (player->world_position.y - curr_wave.world_position[i].y < 20 && player->world_position.y - curr_wave.world_position[i].y > -20) check_Collision(player, i, curr_time);
	}

	void update_Player(Player* player, unsigned int curr_time)
	{
		if (player->instance.state != Draw::ATTACK1 && player->instance.state != Draw::ATTACK2 && player->instance.state != Draw::LAND)
		{
			update_Player_Noninput_Velocity(player);
			if (player->instance.state != Draw::DAMAGED && player->instance.state != Draw::DYING && player->instance.state != Draw::DEAD) update_Player_Input_Velocity(player);
			else player->instance.velocity = { 0 };
		}
		else
		{
			player->instance.velocity = { 0 };
			if (player->instance.state == Draw::ATTACK1 || player->instance.state == Draw::ATTACK2)
				check_Collision_Vs_Wave(&player->instance, curr_time);
		}

		update_Player_Position(player);
		update_Player_State(player);
	}

	void update_Physics(int* state, unsigned int curr_time)
	{
		int curr_state = *state;

		if (curr_state > 1)
		{
			if (curr_state == 2 && (p1.instance.world_position.x >= Draw::window_w || p2.instance.world_position.x >= Draw::window_w))
			{
				p1.instance.velocity.y = 0;
				p2.instance.velocity.y = 0;

				Phys::update_Player_Noninput_Velocity(&p1);
				Phys::update_Player_Position(&p1);
				Phys::update_Player_State(&p1);
				Phys::update_Player_Noninput_Velocity(&p2);
				Phys::update_Player_Position(&p2);
				Phys::update_Player_State(&p2);

				if (p1.instance.world_position.x < 1500)
				{
					if (p1.instance.state != Draw::ATTACK1 && p1.instance.state != Draw::ATTACK2 && p1.instance.state != Draw::FALL)
						p1.instance.velocity.x = p1.instance.key->phys_data.max_vel.x;
					if (p1.instance.world_position.x >= 1500) p1.instance.velocity.x = 0;
				}
				else p1.instance.velocity.x = 0;

				if (p2.instance.world_position.x < 1500)
				{
					if (p2.instance.state != Draw::ATTACK1 && p2.instance.state != Draw::ATTACK2 && p2.instance.state != Draw::FALL)
						p2.instance.velocity.x = p2.instance.key->phys_data.max_vel.x;
					if (p2.instance.world_position.x >= 1500) p2.instance.velocity.x = 0;
				}
				else p2.instance.velocity.x = 0;

				if (Draw::cam_pos.x < Draw::window_w)
				{
					Draw::cam_pos.x += 5;
					if (Draw::cam_pos.x > Draw::window_w) Draw::cam_pos.x = Draw::window_w;
				}

				if (Draw::cam_pos.x == Draw::window_w && p1.instance.world_position.x >= 1500 && p2.instance.world_position.x >= 1500) curr_state = 3;
			}
			else
			{
				Phys::update_Player(&p1, curr_time);
				Phys::update_Player(&p2, curr_time);
				if (curr_state % 2 == 0) //if not in combat
				{
					if (p1.instance.world_position.x > Draw::cam_pos.x + Draw::window_w - 250) Draw::cam_pos.x = p1.instance.world_position.x - Draw::window_w + 250;
					if (p2.instance.world_position.x > Draw::cam_pos.x + Draw::window_w - 250) Draw::cam_pos.x = p2.instance.world_position.x - Draw::window_w + 250;
				}

				if (curr_state == 3)
				{
					if (curr_wave.n_remaining <= 0) //start new wave
					{
						curr_wave.n_instances++;
						curr_wave.n_remaining = curr_wave.n_instances;
						for (int i = 0; i < 100; i++)
						{
							if (i >= curr_wave.n_instances) break;
							curr_wave.curr_frame[i] = curr_wave.key->draw_data.idle_start;
							curr_wave.state[i] = Draw::IDLE;
							curr_wave.world_position[i] = { 2560, 400 + (int)((float)i * (220.0 / (float)curr_wave.n_instances)), 0 };
							curr_wave.draw_position[i].x = curr_wave.world_position[i].x - curr_wave.key->shared_data.scale * curr_wave.key->shared_data.sprite_size.x * 0.5;
							curr_wave.draw_position[i].y = curr_wave.world_position[i].y - curr_wave.world_position[i].z - curr_wave.key->shared_data.scale * curr_wave.key->shared_data.sprite_size.y;
							curr_wave.velocity[i] = { 0 };
							curr_wave.depth_order[i] = i;
							curr_wave.depth_order_pos[i] = i;
							curr_wave.direction_flip[i] = SDL_FLIP_NONE;
							curr_wave.last_frame_update[i] = 0;
						}
					}
					for (int i = 0; i < curr_wave.n_instances; i++)
					{
						//printf("%d %d %d\n", i, curr_wave.state[i], curr_wave.world_position[i].y);
						if (curr_wave.state[i] == Draw::DEAD || curr_wave.state[i] == Draw::DYING) continue;
						float distance_1 = find_Distance(p1.instance.world_position.x, p1.instance.world_position.y, curr_wave.world_position[i].x, curr_wave.world_position[i].y);
						float distance_2 = find_Distance(p2.instance.world_position.x, p2.instance.world_position.y, curr_wave.world_position[i].x, curr_wave.world_position[i].y);
						Player* closest_player = &p1;
						if (distance_1 > distance_2 || p1.instance.state == Draw::DEAD) closest_player = &p2;
						if (p2.instance.state == Draw::DEAD) closest_player = &p1;

						IPair distance;
						distance.x = closest_player->instance.world_position.x - curr_wave.world_position[i].x;
						distance.y = closest_player->instance.world_position.y - curr_wave.world_position[i].y;
						if (curr_wave.state[i] == Draw::IDLE)
						{
							if (distance.x > max_range.x || distance.x < max_range.x * -1)
							{
								curr_wave.state[i] = Draw::WALK;
								if (closest_player->instance.world_position.x < curr_wave.world_position[i].x) curr_wave.world_position[i].x -= curr_wave.key->phys_data.max_vel.x;
								else curr_wave.world_position[i].x += curr_wave.key->phys_data.max_vel.x;
							}

							if (distance.y > max_range.y || distance.y < max_range.y * -1) curr_wave.state[i] = Draw::WALK;
						}
						else if (curr_wave.state[i] == Draw::WALK)
						{
							if (distance.x > aim_range.x || distance.x < aim_range.x * -1)
							{
								curr_wave.state[i] = Draw::WALK;
								if (closest_player->instance.world_position.x < curr_wave.world_position[i].x) curr_wave.world_position[i].x -= curr_wave.key->phys_data.max_vel.x;
								else curr_wave.world_position[i].x += curr_wave.key->phys_data.max_vel.x;
							}

							if (distance.y > aim_range.y || distance.y < aim_range.y * -1)
							{
								curr_wave.state[i] = Draw::WALK;
								if (closest_player->instance.world_position.y < curr_wave.world_position[i].y)
								{
									if (curr_wave.depth_order_pos[i] > 0)
									{
										if (curr_wave.world_position[i].y - curr_wave.world_position[curr_wave.depth_order[curr_wave.depth_order_pos[i] - 1]].y - curr_wave.key->phys_data.max_vel.y < min_range.y &&
											curr_wave.world_position[i].x - curr_wave.world_position[curr_wave.depth_order[curr_wave.depth_order_pos[i] - 1]].x < min_range.x &&
											curr_wave.world_position[i].x - curr_wave.world_position[curr_wave.depth_order[curr_wave.depth_order_pos[i] - 1]].x > -1 * min_range.x);
										else curr_wave.world_position[i].y -= curr_wave.key->phys_data.max_vel.y;
									}
									else curr_wave.world_position[i].y -= curr_wave.key->phys_data.max_vel.y;
									while (curr_wave.depth_order_pos[i] > 0 && curr_wave.world_position[i].y < curr_wave.world_position[curr_wave.depth_order[curr_wave.depth_order_pos[i] - 1]].y)
									{
										int order_1_id = i;
										int pos_1_id = curr_wave.depth_order_pos[order_1_id];
										int order_2_id = curr_wave.depth_order[pos_1_id - 1];
										int pos_2_id = curr_wave.depth_order_pos[order_2_id];

										curr_wave.depth_order[pos_1_id] = order_2_id;
										curr_wave.depth_order[pos_2_id] = order_1_id;
										curr_wave.depth_order_pos[order_1_id] = pos_2_id;
										curr_wave.depth_order_pos[order_2_id] = pos_1_id;
									}
								}
								else if (closest_player->instance.world_position.y > curr_wave.world_position[i].y)
								{
									if (curr_wave.depth_order_pos[i] < curr_wave.n_instances - 1)
									{
										if (curr_wave.world_position[curr_wave.depth_order[curr_wave.depth_order_pos[i] + 1]].y - curr_wave.world_position[i].y - curr_wave.key->phys_data.max_vel.y < min_range.y &&
											curr_wave.world_position[curr_wave.depth_order[curr_wave.depth_order_pos[i] + 1]].x - curr_wave.world_position[i].x < min_range.x &&
											curr_wave.world_position[curr_wave.depth_order[curr_wave.depth_order_pos[i] + 1]].x - curr_wave.world_position[i].x > -1 * min_range.x);
										else curr_wave.world_position[i].y += curr_wave.key->phys_data.max_vel.y;
									}
									else curr_wave.world_position[i].y += curr_wave.key->phys_data.max_vel.y;
									while (curr_wave.depth_order_pos[i] < curr_wave.n_instances - 1 && curr_wave.world_position[i].y > curr_wave.world_position[curr_wave.depth_order[curr_wave.depth_order_pos[i] + 1]].y)
									{
										int order_1_id = i;
										int pos_1_id = curr_wave.depth_order_pos[order_1_id];
										int order_2_id = curr_wave.depth_order[pos_1_id + 1];
										int pos_2_id = curr_wave.depth_order_pos[order_2_id];

										curr_wave.depth_order[pos_1_id] = order_2_id;
										curr_wave.depth_order[pos_2_id] = order_1_id;
										curr_wave.depth_order_pos[order_1_id] = pos_2_id;
										curr_wave.depth_order_pos[order_2_id] = pos_1_id;
									}
								}
							}

							curr_wave.draw_position[i].x = curr_wave.world_position[i].x - Draw::cam_pos.x - (curr_wave.key->shared_data.sprite_size.x * curr_wave.key->shared_data.scale * 0.5);
							curr_wave.draw_position[i].y = curr_wave.world_position[i].y - curr_wave.world_position[i].z - (curr_wave.key->shared_data.sprite_size.x * curr_wave.key->shared_data.scale);
						}
						else if (curr_wave.state[i] == Draw::ATTACK1) check_Collision(&closest_player->instance, i, curr_time);

						curr_wave.direction_flip[i] = SDL_FLIP_HORIZONTAL;
						if (distance.x < 0)
						{
							curr_wave.direction_flip[i] = SDL_FLIP_NONE;
							distance.x *= -1;
						}
						if (distance.y < 0) distance.y *= -1;
						if (distance.x < max_range.x && distance.y < max_range.y) //attack
						{
							curr_wave.state[i] = Draw::ATTACK1;
						}
					}
				}

				//player movement boundaries
				if (p1.instance.world_position.x < Draw::cam_pos.x + p1.instance.key->shared_data.sprite_size.x * p1.instance.key->shared_data.scale * 0.25) p1.instance.world_position.x = Draw::cam_pos.x + p1.instance.key->shared_data.sprite_size.x * p1.instance.key->shared_data.scale * 0.25;
				else if (p1.instance.world_position.x > Draw::cam_pos.x + Draw::window_w - p1.instance.key->shared_data.sprite_size.x * p1.instance.key->shared_data.scale * 0.25) p1.instance.world_position.x = Draw::cam_pos.x + Draw::window_w - p1.instance.key->shared_data.sprite_size.x * p1.instance.key->shared_data.scale * 0.25;

				if (p2.instance.world_position.x < Draw::cam_pos.x + p2.instance.key->shared_data.sprite_size.x * p2.instance.key->shared_data.scale * 0.25) p2.instance.world_position.x = Draw::cam_pos.x + p2.instance.key->shared_data.sprite_size.x * p2.instance.key->shared_data.scale * 0.25;
				else if (p2.instance.world_position.x > Draw::cam_pos.x + Draw::window_w - p2.instance.key->shared_data.sprite_size.x * p2.instance.key->shared_data.scale * 0.25) p2.instance.world_position.x = Draw::cam_pos.x + Draw::window_w - p2.instance.key->shared_data.sprite_size.x * p2.instance.key->shared_data.scale * 0.25;

				if (p1.instance.world_position.y > Draw::window_h - 75) p1.instance.world_position.y = Draw::window_h - 75;
				else if (p1.instance.world_position.y < 330) p1.instance.world_position.y = 330;

				if (p2.instance.world_position.y > Draw::window_h - 75) p2.instance.world_position.y = Draw::window_h - 75;
				else if (p2.instance.world_position.y < 330) p2.instance.world_position.y = 330;
			}
		}
		*state = curr_state;
	}
}

namespace Draw
{
	void update_Entity_Set_Animation(Entity_Instance_Set* set, unsigned int curr_time)
	{
		for (int i = 0; i < set->n_instances; i++)
		{
			if (set->state[i] == IDLE)
			{
				if (1000.0 / (curr_time - set->last_frame_update[i]) <= set->key->draw_data.idle_rate || set->key->draw_data.idle_rate < 0)
				{
					set->last_frame_update[i] = curr_time;
					if (set->curr_frame[i] < set->key->draw_data.idle_start || set->curr_frame[i] >= set->key->draw_data.idle_end) set->curr_frame[i] = set->key->draw_data.idle_start;
					else set->curr_frame[i]++;
				}
			}
			else if (set->state[i] == WALK)
			{
				if (1000.0 / (curr_time - set->last_frame_update[i]) <= set->key->draw_data.walk_rate || set->key->draw_data.walk_rate < 0)
				{
					set->last_frame_update[i] = curr_time;
					if (set->curr_frame[i] < set->key->draw_data.walk_start || set->curr_frame[i] >= set->key->draw_data.walk_end) set->curr_frame[i] = set->key->draw_data.walk_start;
					else set->curr_frame[i]++;
				}
			}
			else if (set->state[i] == ATTACK1)
			{
				if (1000.0 / (curr_time - set->last_frame_update[i]) <= set->key->draw_data.atk_1_rate || set->key->draw_data.atk_1_rate < 0)
				{
					set->last_frame_update[i] = curr_time;
					if (set->curr_frame[i] < set->key->draw_data.atk_1_start || set->curr_frame[i] > set->key->draw_data.atk_1_end) set->curr_frame[i] = set->key->draw_data.atk_1_start;
					else if (set->curr_frame[i] != set->key->draw_data.atk_1_end) set->curr_frame[i]++;
					else
					{
						set->curr_frame[i] = set->key->draw_data.idle_start;
						set->state[i] = IDLE;
					}
				}
			}
			else if (set->state[i] == DYING)
			{
				if (1000.0 / (curr_time - set->last_frame_update[i]) <= set->key->draw_data.die_rate || set->key->draw_data.atk_1_rate < 0)
				{
					set->last_frame_update[i] = curr_time;
					if (set->curr_frame[i] < set->key->draw_data.die_start || set->curr_frame[i] > set->key->draw_data.die_end) set->curr_frame[i] = set->key->draw_data.die_start;
					else if (set->curr_frame[i] != set->key->draw_data.die_end) set->curr_frame[i]++;
					else
					{
						set->world_position[i].y = 0;
						set->draw_position[i].y = 0 - set->key->shared_data.sprite_size.y * set->key->shared_data.scale;
						set->n_remaining--;
						set->state[i] = DEAD;
					}
				}
			}
			else if (set->state[i] == DEAD)
			{

			}
		}
	}

	void update_Entity_Animation(Entity_Instance* instance, unsigned int curr_time)
	{
		if (instance->state == IDLE)
		{
			if (1000.0 / (curr_time - instance->last_frame_update) <= instance->key->draw_data.idle_rate || instance->key->draw_data.idle_rate < 0)
			{
				instance->last_frame_update = curr_time;
				if (instance->curr_frame < instance->key->draw_data.idle_start || instance->curr_frame >= instance->key->draw_data.idle_end) instance->curr_frame = instance->key->draw_data.idle_start;
				else instance->curr_frame++;
			}
		}
		else if (instance->state == WALK)
		{
			if (1000.0 / (curr_time - instance->last_frame_update) <= instance->key->draw_data.walk_rate || instance->key->draw_data.walk_rate < 0)
			{
				instance->last_frame_update = curr_time;
				if (instance->curr_frame < instance->key->draw_data.walk_start || instance->curr_frame >= instance->key->draw_data.walk_end) instance->curr_frame = instance->key->draw_data.walk_start;
				else instance->curr_frame++;
			}
		}
		else if (instance->state == JUMP)
		{
			if (1000.0 / (curr_time - instance->last_frame_update) <= instance->key->draw_data.jump_rate || instance->key->draw_data.jump_rate < 0)
			{
				instance->last_frame_update = curr_time;
				if (instance->curr_frame < instance->key->draw_data.jump_start || instance->curr_frame > instance->key->draw_data.jump_end) instance->curr_frame = instance->key->draw_data.jump_start;
				else if (instance->curr_frame != instance->key->draw_data.jump_end) instance->curr_frame++;
			}
		}
		else if (instance->state == FALL)
		{
			if (1000.0 / (curr_time - instance->last_frame_update) <= instance->key->draw_data.fall_rate || instance->key->draw_data.fall_rate < 0)
			{
				instance->last_frame_update = curr_time;
				if (instance->curr_frame < instance->key->draw_data.fall_start || instance->curr_frame >= instance->key->draw_data.fall_end) instance->curr_frame = instance->key->draw_data.fall_start;
				else instance->curr_frame++;
			}
		}
		else if (instance->state == LAND)
		{
			if (1000.0 / (curr_time - instance->last_frame_update) <= instance->key->draw_data.land_rate || instance->key->draw_data.land_rate < 0)
			{
				instance->last_frame_update = curr_time;
				if (instance->curr_frame < instance->key->draw_data.land_start || instance->curr_frame > instance->key->draw_data.land_end) instance->curr_frame = instance->key->draw_data.land_start;
				else if (instance->curr_frame != instance->key->draw_data.land_end) instance->curr_frame++;
				else
				{
					instance->state = IDLE;
					instance->curr_frame = instance->key->draw_data.idle_start;
				}
			}
		}
		else if (instance->state == ATTACK1)
		{
			if (1000.0 / (curr_time - instance->last_frame_update) <= instance->key->draw_data.atk_1_rate || instance->key->draw_data.atk_1_rate < 0)
			{
				instance->last_frame_update = curr_time;
				if (instance->curr_frame < instance->key->draw_data.atk_1_start || instance->curr_frame > instance->key->draw_data.atk_1_end) instance->curr_frame = instance->key->draw_data.atk_1_start;
				else if (instance->curr_frame != instance->key->draw_data.atk_1_end) instance->curr_frame++;
				else
				{
					instance->state = IDLE;
				}
			}
		}
		else if (instance->state == ATTACK2)
		{
			if (1000.0 / (curr_time - instance->last_frame_update) <= instance->key->draw_data.atk_2_rate || instance->key->draw_data.atk_2_rate < 0)
			{
				instance->last_frame_update = curr_time;
				if (instance->curr_frame < instance->key->draw_data.atk_2_start || instance->curr_frame > instance->key->draw_data.atk_2_end) instance->curr_frame = instance->key->draw_data.atk_2_start;
				else if (instance->curr_frame != instance->key->draw_data.atk_2_end) instance->curr_frame++;
				else instance->state = IDLE;
			}
		}
		else if (instance->state == CAST)
		{
			if (1000.0 / (curr_time - instance->last_frame_update) <= instance->key->draw_data.cast_rate || instance->key->draw_data.cast_rate < 0)
			{
				instance->last_frame_update = curr_time;
				if (instance->curr_frame < instance->key->draw_data.cast_start || instance->curr_frame > instance->key->draw_data.cast_end) instance->curr_frame = instance->key->draw_data.cast_start;
				else if (instance->curr_frame != instance->key->draw_data.cast_end) instance->curr_frame++;
				else instance->state = IDLE;
			}
		}
		else if (instance->state == DAMAGED)
		{
			if (1000.0 / (curr_time - instance->last_frame_update) <= instance->key->draw_data.recoil_rate || instance->key->draw_data.recoil_rate < 0)
			{
				instance->last_frame_update = curr_time;
				if (instance->curr_frame < instance->key->draw_data.recoil_start || instance->curr_frame > instance->key->draw_data.recoil_end) instance->curr_frame = instance->key->draw_data.recoil_start;
				else if (instance->curr_frame != instance->key->draw_data.recoil_end) instance->curr_frame++;
				else instance->state = IDLE;
			}
		}
		else if (instance->state == DYING)
		{
			if (instance->direction_flip == SDL_FLIP_NONE) instance->world_position.x -= instance->key->phys_data.max_vel.x * 0.5;
			else instance->world_position.x += instance->key->phys_data.max_vel.x * 0.5;

			if (1000.0 / (curr_time - instance->last_frame_update) <= instance->key->draw_data.die_rate || instance->key->draw_data.die_rate < 0)
			{
				instance->last_frame_update = curr_time;
				if (instance->curr_frame < instance->key->draw_data.die_start || instance->curr_frame > instance->key->draw_data.die_end) instance->curr_frame = instance->key->draw_data.die_start;
				else if (instance->curr_frame != instance->key->draw_data.die_end) instance->curr_frame++;
				else instance->state = DEAD;
			}
		}
	}

	void load_Image_And_Size_To_Texture(SDL_Texture** dest, int* width, int* height, SDL_Renderer* renderer, const char* src)
	{
		SDL_Surface* temp = IMG_Load(src);
		assert(temp);
		SDL_LockSurface(temp);
		*width = temp->w;
		*height = temp->h;
		SDL_UnlockSurface(temp);
		*dest = SDL_CreateTextureFromSurface(renderer, temp);
		assert(*dest);
		SDL_FreeSurface(temp);
	}

	void load_Image_To_Texture(SDL_Texture** dest, SDL_Renderer* renderer, const char* src)
	{
		SDL_Surface* temp = IMG_Load(src);
		assert(temp);
		*dest = SDL_CreateTextureFromSurface(renderer, temp);
		assert(*dest);
		SDL_FreeSurface(temp);
	}

	Entity_Key load_EDS(SDL_Renderer* renderer, const char* eds_path)
	{
		Entity_Key entity = { 0 };
		FILE* f_in = fopen(eds_path, "r");
		char image_path[128];
		fscanf(f_in, "Sprite Sheet Path: %[^\n]\n", image_path);
		int n_frames = 0;
		int items_grabbed = 0;
		fscanf(f_in, "Width: %d\tHeight: %d\t# Frames: %d\n", &entity.shared_data.sprite_size.x, &entity.shared_data.sprite_size.y, &n_frames);
		load_Image_And_Size_To_Texture(&entity.draw_data.sprite_sheet, &entity.draw_data.sheet_size.x, &entity.draw_data.sheet_size.y, renderer, image_path);
		entity.draw_data.n_cols = entity.draw_data.sheet_size.x / entity.shared_data.sprite_size.x;
		fscanf(f_in, "Idle Start: %d\tIdle End: %d\tIdle Rate: %d\n", &entity.draw_data.idle_start, &entity.draw_data.idle_end, &entity.draw_data.idle_rate);
		fscanf(f_in, "Jump Start: %d\tJump End: %d\tJump Rate: %d\n", &entity.draw_data.jump_start, &entity.draw_data.jump_end, &entity.draw_data.jump_rate);
		fscanf(f_in, "Fall Start: %d\tFall End: %d\tFall Rate: %d\n", &entity.draw_data.fall_start, &entity.draw_data.fall_end, &entity.draw_data.fall_rate);
		fscanf(f_in, "Land Start: %d\tLand End: %d\tLand Rate: %d\n", &entity.draw_data.land_start, &entity.draw_data.land_end, &entity.draw_data.land_rate);
		fscanf(f_in, "Walk Start: %d\tWalk End: %d\tWalk Rate: %d\n", &entity.draw_data.walk_start, &entity.draw_data.walk_end, &entity.draw_data.walk_rate);
		fscanf(f_in, "Atk1 Start: %d\tAtk1 End: %d\tAtk1 Rate: %d\n", &entity.draw_data.atk_1_start, &entity.draw_data.atk_1_end, &entity.draw_data.atk_1_rate);
		fscanf(f_in, "Atk2 Start: %d\tAtk2 End: %d\tAtk2 Rate: %d\n", &entity.draw_data.atk_2_start, &entity.draw_data.atk_2_end, &entity.draw_data.atk_2_rate);
		fscanf(f_in, "Recoil Start: %d\tRecoil End: %d\tRecoil Rate: %d\n", &entity.draw_data.recoil_start, &entity.draw_data.recoil_end, &entity.draw_data.recoil_rate);
		fscanf(f_in, "Death Start: %d\tDeath End: %d\tDeath Rate: %d\n", &entity.draw_data.die_start, &entity.draw_data.die_end, &entity.draw_data.die_rate);
		fscanf(f_in, "Cast Start: %d\tCast End: %d\tCast Rate: %d\nHitboxes:\n", &entity.draw_data.cast_start, &entity.draw_data.cast_end, &entity.draw_data.cast_rate);
		//fscanf(f_in, "Hitboxes:\n");
		entity.phys_data.n_collision_boxes = (int*)calloc(n_frames, sizeof(int));
		entity.phys_data.collision_boxes = (SDL_Rect**)calloc(n_frames, sizeof(SDL_Rect*));
		int dump = 0;
		for (int i = 0; i < n_frames; i++)
		{
			fscanf(f_in, "Frame: %*d\t# Hitboxes: %d\n", &entity.phys_data.n_collision_boxes[i]);
			entity.phys_data.collision_boxes[i] = (SDL_Rect*)calloc(entity.phys_data.n_collision_boxes[i], sizeof(SDL_Rect));
			for (int j = 0; j < entity.phys_data.n_collision_boxes[i]; j++) fscanf(f_in, "X: %d\tY: %d\tW: %d\tH: %d\n", &entity.phys_data.collision_boxes[i][j].x, &entity.phys_data.collision_boxes[i][j].y, &entity.phys_data.collision_boxes[i][j].w, &entity.phys_data.collision_boxes[i][j].h);
		}
		fscanf(f_in, "Atkboxes:\n", &image_path);
		entity.phys_data.n_attack_boxes = (int*)calloc(n_frames, sizeof(int));
		entity.phys_data.attack_boxes = (SDL_Rect**)calloc(n_frames, sizeof(SDL_Rect*));
		for (int i = 0; i < n_frames; i++)
		{
			fscanf(f_in, "Frame: %*d\t# Atkboxes: %d\n", &entity.phys_data.n_attack_boxes[i]);
			entity.phys_data.attack_boxes[i] = (SDL_Rect*)calloc(entity.phys_data.n_attack_boxes[i], sizeof(SDL_Rect));
			for (int j = 0; j < entity.phys_data.n_attack_boxes[i]; j++) fscanf(f_in, "X: %d\tY: %d\tW: %d\tH: %d\n", &entity.phys_data.attack_boxes[i][j].x, &entity.phys_data.attack_boxes[i][j].y, &entity.phys_data.attack_boxes[i][j].w, &entity.phys_data.attack_boxes[i][j].h);
		}
		fclose(f_in);
		return entity;
	}

	void init_Window(int w, int h, const char* window_name)
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		window_w = w;
		window_h = h;
		window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		cam_pos = { 0 };
		load_Image_And_Size_To_Texture(&fontsheet, &fontsheet_size.x, &fontsheet_size.y, renderer, "Assets/Images/UI/fontsheet.png");
		font_size = { 64, 96 };
	}

	void draw_Text(const char* input, int x, int y, int size)
	{
		SDL_Rect dest;
		dest.x = x;
		dest.y = y;
		dest.w = size;
		dest.h = size * 1.5;

		SDL_Rect src;
		src.w = font_size.x;
		src.h = font_size.y;

		int n_data = strlen(input);
		for (int i = 0; i < n_data; i++)
		{
			src.x = src.w * (input[i] % (fontsheet_size.x / font_size.x));
			src.y = src.h * (input[i] / (fontsheet_size.x / font_size.x));

			SDL_RenderCopyEx(renderer, fontsheet, &src, &dest, 0, NULL, SDL_FLIP_NONE);
			dest.x += dest.w;
			if (dest.x + dest.w > window_w)
			{
				dest.x = x;
				dest.y += size;
			}
		}
	}

	void draw_Entity(Entity_Instance* entity)
	{
		SDL_Rect src = { 0 };
		src.x = entity->key->shared_data.sprite_size.x * (entity->curr_frame % entity->key->draw_data.n_cols);
		src.y = entity->key->shared_data.sprite_size.y * (entity->curr_frame / entity->key->draw_data.n_cols);
		src.w = entity->key->shared_data.sprite_size.x;
		src.h = entity->key->shared_data.sprite_size.y;

		SDL_Rect dest = { 0 };
		dest.x = entity->draw_position.x;
		dest.y = entity->draw_position.y;
		dest.w = entity->key->shared_data.sprite_size.x * entity->key->shared_data.scale;
		dest.h = entity->key->shared_data.sprite_size.y * entity->key->shared_data.scale;

		SDL_RenderCopyEx(renderer, entity->key->draw_data.sprite_sheet, &src, &dest, 0, NULL, entity->direction_flip);
		if (Sys::debug_toggle == 1)
		{
			SDL_Rect debug;
			for (int i = 0; i < entity->key->phys_data.n_collision_boxes[entity->curr_frame]; i++)
			{
				debug = entity->key->phys_data.collision_boxes[entity->curr_frame][i];

				if (entity->direction_flip == SDL_FLIP_HORIZONTAL) debug.x = entity->key->shared_data.sprite_size.x - (entity->key->phys_data.collision_boxes[entity->curr_frame][i].x + entity->key->phys_data.collision_boxes[entity->curr_frame][i].w);

				debug.x *= entity->key->shared_data.scale;
				debug.y *= entity->key->shared_data.scale;
				debug.w *= entity->key->shared_data.scale;
				debug.h *= entity->key->shared_data.scale;

				debug.x += entity->draw_position.x;
				debug.y += entity->draw_position.y;

				SDL_RenderCopyEx(Draw::renderer, collisionbox, NULL, &debug, 0, NULL, SDL_FLIP_NONE);
			}
			for (int i = 0; i < entity->key->phys_data.n_attack_boxes[entity->curr_frame]; i++)
			{
				debug = entity->key->phys_data.attack_boxes[entity->curr_frame][i];

				if (entity->direction_flip == SDL_FLIP_HORIZONTAL) debug.x = entity->key->shared_data.sprite_size.x - (entity->key->phys_data.attack_boxes[entity->curr_frame][i].x + entity->key->phys_data.attack_boxes[entity->curr_frame][i].w);

				debug.x *= entity->key->shared_data.scale;
				debug.y *= entity->key->shared_data.scale;
				debug.w *= entity->key->shared_data.scale;
				debug.h *= entity->key->shared_data.scale;

				debug.x += entity->draw_position.x;
				debug.y += entity->draw_position.y;

				SDL_RenderCopyEx(Draw::renderer, attackbox, NULL, &debug, 0, NULL, SDL_FLIP_NONE);
			}
		}
	}

	void draw_Instance(const Entity_Key key, const IPair position, char curr_frame, const SDL_RendererFlip flip)
	{
		SDL_Rect src = { 0 };
		src.x = key.shared_data.sprite_size.x * (curr_frame % key.draw_data.n_cols);
		src.y = key.shared_data.sprite_size.y * (curr_frame / key.draw_data.n_cols);
		src.w = key.shared_data.sprite_size.x;
		src.h = key.shared_data.sprite_size.y;

		SDL_Rect dest = { 0 };
		dest.x = position.x;
		dest.y = position.y;
		dest.w = key.shared_data.sprite_size.x * key.shared_data.scale;
		dest.h = key.shared_data.sprite_size.y * key.shared_data.scale;

		SDL_RenderCopyEx(renderer, key.draw_data.sprite_sheet, &src, &dest, 0, NULL, flip);
		if (Sys::debug_toggle == 1)
		{
			SDL_Rect debug;
			for (int i = 0; i < key.phys_data.n_collision_boxes[curr_frame]; i++)
			{
				debug = key.phys_data.collision_boxes[curr_frame][i];
				if (flip == SDL_FLIP_HORIZONTAL) debug.x = key.shared_data.sprite_size.x - (key.phys_data.collision_boxes[curr_frame][i].x + key.phys_data.collision_boxes[curr_frame][i].w);
				debug.x *= key.shared_data.scale;
				debug.y *= key.shared_data.scale;
				debug.w *= key.shared_data.scale;
				debug.h *= key.shared_data.scale;

				debug.x += position.x;
				debug.y += position.y;

				SDL_RenderCopyEx(Draw::renderer, collisionbox, NULL, &debug, 0, NULL, SDL_FLIP_NONE);
			}
			for (int i = 0; i < key.phys_data.n_attack_boxes[curr_frame]; i++)
			{
				debug = key.phys_data.attack_boxes[curr_frame][i];
				if (flip == SDL_FLIP_HORIZONTAL) debug.x = key.shared_data.sprite_size.x - (key.phys_data.attack_boxes[curr_frame][i].x + key.phys_data.attack_boxes[curr_frame][i].w);
				debug.x *= key.shared_data.scale;
				debug.y *= key.shared_data.scale;
				debug.w *= key.shared_data.scale;
				debug.h *= key.shared_data.scale;

				debug.x += position.x;
				debug.y += position.y;

				SDL_RenderCopyEx(Draw::renderer, attackbox, NULL, &debug, 0, NULL, SDL_FLIP_NONE);
			}
		}
	}

	void draw_Wave()
	{
		if (curr_wave.n_instances == 0)
		{
			if (p1.instance.world_position.y < p2.instance.world_position.y)
			{
				draw_Entity(&p1.instance);
				draw_Entity(&p2.instance);
			}
			else
			{
				draw_Entity(&p2.instance);
				draw_Entity(&p1.instance);
			}
		}
		else
		{
			int p1_drawn = 0;
			int p2_drawn = 0;
			for (int i = 0; i < curr_wave.n_instances; i++)
			{
				if (p1_drawn == 0 && curr_wave.world_position[curr_wave.depth_order[i]].y > p1.instance.world_position.y)
				{
					if (p2_drawn == 0 && curr_wave.world_position[curr_wave.depth_order[i]].y > p2.instance.world_position.y)
					{
						if (p1.instance.world_position.y < p2.instance.world_position.y)
						{
							p2_drawn = 1;
							draw_Entity(&p1.instance);
							draw_Entity(&p2.instance);
						}
						else
						{
							p2_drawn = 1;
							draw_Entity(&p2.instance);
							draw_Entity(&p1.instance);
						}
					}
					else draw_Entity(&p1.instance);
					p1_drawn = 1;
				}
				if (p2_drawn == 0 && curr_wave.world_position[curr_wave.depth_order[i]].y > p2.instance.world_position.y)
				{
					p2_drawn = 1;
					draw_Entity(&p2.instance);
				}
				draw_Instance(*curr_wave.key, curr_wave.draw_position[curr_wave.depth_order[i]], curr_wave.curr_frame[curr_wave.depth_order[i]], curr_wave.direction_flip[curr_wave.depth_order[i]]);
			}
			if (p1_drawn == 0 && p2_drawn == 0)
			{
				if (p1.instance.world_position.y < p2.instance.world_position.y)
				{
					draw_Entity(&p1.instance);
					draw_Entity(&p2.instance);
				}
				else
				{
					draw_Entity(&p2.instance);
					draw_Entity(&p1.instance);
				}
			}
			else if (p1_drawn == 0) draw_Entity(&p1.instance);
			else if (p2_drawn == 0) draw_Entity(&p2.instance);
		}
	}

	void draw_Window(int state)
	{
		if (Sys::debug_toggle == 1) draw_Text("Debug", 1230, 10, 10);
		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);
		if (state < 2)
		{
			SDL_RenderCopyEx(renderer, title_screen, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
			if (state == 0)
			{
				draw_Text("Game will play", 780, 200, 30);
				draw_Text("once 2 controllers", 725, 245, 30);
				draw_Text("are connected", 800, 290, 30);
			}
			else if (state == 1)
			{
				draw_Text("Controllers connected", 740, 245, 24);
				draw_Text("Hold A to start", 810, 281, 25);

				SDL_Rect player_status = { 760, 450, 150, 150 };
				if (p1.input.a_button_state % 2 == 1) SDL_RenderCopyEx(renderer, Draw::a_lit, NULL, &player_status, 0, NULL, SDL_FLIP_NONE);
				else SDL_RenderCopyEx(renderer, Draw::a_unlit, NULL, &player_status, 0, NULL, SDL_FLIP_NONE);
				draw_Text("A", player_status.x + 25, player_status.y + 20, 100);

				player_status.x = 1060;
				if (p2.input.a_button_state % 2 == 1) SDL_RenderCopyEx(renderer, Draw::a_lit, NULL, &player_status, 0, NULL, SDL_FLIP_NONE);
				else SDL_RenderCopyEx(renderer, Draw::a_unlit, NULL, &player_status, 0, NULL, SDL_FLIP_NONE);
				draw_Text("A", player_status.x + 25, player_status.y + 20, 100);
			}
		}
		else
		{
			for (int i = 0; i < 2; i++)
			{
				if (i * window_w > cam_pos.x + window_w) break;
				else if ((i + 1) * window_w < cam_pos.x) continue;
				else
				{
					SDL_Rect dest = { i * window_w - cam_pos.x, 0, window_w, window_h };
					SDL_RenderCopyEx(renderer, background[i], NULL, &dest, 0, NULL, SDL_FLIP_NONE);
				}
			}
			if (state % 2 == 1)
			{
				draw_Wave();
			}
			else
			{
				if (p1.instance.world_position.y < p2.instance.world_position.y)
				{
					draw_Entity(&p1.instance);
					draw_Entity(&p2.instance);
				}
				else
				{
					draw_Entity(&p2.instance);
					draw_Entity(&p1.instance);
				}
			}
			for (int i = 0; i < 2; i++)
			{
				if (i * window_w > cam_pos.x + window_w) break;
				else if ((i + 1) * window_w < cam_pos.x) continue;
				else
				{
					SDL_Rect dest = { i * window_w - cam_pos.x, 0, window_w, window_h };
					SDL_RenderCopyEx(renderer, foreground[i], NULL, &dest, 0, NULL, SDL_FLIP_NONE);
				}
			}
			//draw health
			float ui_scale = 0.5;
			SDL_Rect ui_dest = { 10, 0, (int)((float)bar_set_size.x * ui_scale), (int)((float)bar_set_size.y * ui_scale) };
			SDL_RenderCopyEx(renderer, bar_set, NULL, &ui_dest, 0, NULL, SDL_FLIP_NONE);
			draw_Text("Player 1", ui_dest.x + 85, 3, 14);

			ui_dest.x += 68 * ui_scale;
			ui_dest.y += 52 * ui_scale;
			SDL_Rect ui_src = { 0, 0, (int)((float)bar_size.x * ((float)p1.instance.health / (float)p1.instance.key->phys_data.max_health)), bar_size.y };
			ui_dest.w = ui_src.w * ui_scale;
			ui_dest.h = ui_src.h * ui_scale;
			SDL_RenderCopyEx(renderer, hp_bar, &ui_src, &ui_dest, 0, NULL, SDL_FLIP_NONE);

			ui_dest = { 310, 0, (int)((float)bar_set_size.x * ui_scale), (int)((float)bar_set_size.y * ui_scale) };
			SDL_RenderCopyEx(renderer, bar_set, NULL, &ui_dest, 0, NULL, SDL_FLIP_NONE);
			draw_Text("Player 2", ui_dest.x + 85, 3, 14);

			ui_dest.x += 68 * ui_scale;
			ui_dest.y += 52 * ui_scale;
			ui_src = { 0, 0, (int)((float)bar_size.x * ((float)p2.instance.health / (float)p2.instance.key->phys_data.max_health)), bar_size.y };
			ui_dest.w = ui_src.w * ui_scale;
			ui_dest.h = ui_src.h * ui_scale;
			SDL_RenderCopyEx(renderer, hp_bar, &ui_src, &ui_dest, 0, NULL, SDL_FLIP_NONE);
		}
	}
}