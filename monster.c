#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "logging.h"
#include "message.h"



int max(int x,int y){
	return x > y ? x : y;
}

int calculate_manhattan_distance(coordinate first_coordinate,coordinate second_coordinate) {
	return abs(first_coordinate.x - second_coordinate.x) + abs(first_coordinate.y - second_coordinate.y);  
}

int calculate_new_health(int old_health,int damage,int defence) {
	return old_health-max(0,damage-defence);
}

coordinate calculate_next_move(coordinate current_position,coordinate player_position) {
	coordinate next_move,template_next_move;																//  Firstly, set next move to up adjacent.																					
	next_move.x = current_position.x;																		//	Then, we will check other adjacents.															
	next_move.y = current_position.y-1;																		//																						
	int distance_from_next_move = calculate_manhattan_distance(next_move,player_position);					//																		
	
	template_next_move.x = current_position.x+1;															// Upper - Right
	template_next_move.y = current_position.y-1;															// Calculation.
	if (calculate_manhattan_distance(template_next_move,player_position) < distance_from_next_move) {		//
		next_move = template_next_move;																		//	
		distance_from_next_move = calculate_manhattan_distance(template_next_move,player_position);			//
	}


	template_next_move.x = current_position.x+1;															// Right
	template_next_move.y = current_position.y;																// Calculation.
	if (calculate_manhattan_distance(template_next_move,player_position) < distance_from_next_move) {		//
		next_move = template_next_move;																		//	
		distance_from_next_move = calculate_manhattan_distance(template_next_move,player_position);			//
	}


	template_next_move.x = current_position.x+1;															// Bottom - Right
	template_next_move.y = current_position.y+1;															// Calculation.
	if (calculate_manhattan_distance(template_next_move,player_position) < distance_from_next_move) {		//
		next_move = template_next_move;																		//	
		distance_from_next_move = calculate_manhattan_distance(template_next_move,player_position);			//
	}



	template_next_move.x = current_position.x;																// Down
	template_next_move.y = current_position.y+1;															// Calculation.
	if (calculate_manhattan_distance(template_next_move,player_position) < distance_from_next_move) {		//
		next_move = template_next_move;																		//	
		distance_from_next_move = calculate_manhattan_distance(template_next_move,player_position);			//
	}



	template_next_move.x = current_position.x-1;															// Bottom - Left
	template_next_move.y = current_position.y+1;															// Calculation.
	if (calculate_manhattan_distance(template_next_move,player_position) < distance_from_next_move) {		//
		next_move = template_next_move;																		//	
		distance_from_next_move = calculate_manhattan_distance(template_next_move,player_position);			//
	}



	template_next_move.x = current_position.x-1;															// Left
	template_next_move.y = current_position.y;																// Calculation.
	if (calculate_manhattan_distance(template_next_move,player_position) < distance_from_next_move) {		//
		next_move = template_next_move;																		//	
		distance_from_next_move = calculate_manhattan_distance(template_next_move,player_position);			//
	}


	template_next_move.x = current_position.x-1;															// Upper - Left
	template_next_move.y = current_position.y-1;															// Calculation.
	if (calculate_manhattan_distance(template_next_move,player_position) < distance_from_next_move) {		//
		next_move = template_next_move;																		//	
		distance_from_next_move = calculate_manhattan_distance(template_next_move,player_position);			//
	}


	return next_move;
}


int main(int argc, char *argv[]) {
	int health=atoi(argv[1]),monster_damage=atoi(argv[2]),monster_defence=atoi(argv[3]),range_of_attack=atoi(argv[4]);
	coordinate current_coordinate;
	coordinate player_coordinate;
	monster_message message_from_world;
	monster_response message_to_world;
	message_to_world.mr_type = mr_ready;
	write(1,&message_to_world,sizeof(message_to_world));

	while(true) {
		if (read (0,&message_from_world,sizeof(message_from_world)) >= 0) {
			if (message_from_world.game_over == true) {
				break;
			}
			else if ((health = calculate_new_health(health,message_from_world.damage,monster_defence)) <= 0) {
				message_to_world.mr_type = mr_dead;
				write(1,&message_to_world,sizeof(message_to_world));
				break;
			}
			else {
				current_coordinate = message_from_world.new_position;
				player_coordinate = message_from_world.player_coordinate;
				if (calculate_manhattan_distance(current_coordinate,player_coordinate) <= range_of_attack) {
					message_to_world.mr_type = mr_attack;
					message_to_world.mr_content.attack = monster_damage;
					write(1,&message_to_world,sizeof(message_to_world));
				}
				else {
					coordinate next_move = calculate_next_move(current_coordinate,player_coordinate);
					message_to_world.mr_type = mr_move;
					message_to_world.mr_content.move_to = next_move;
					write(1,&message_to_world,sizeof(message_to_world));
				}
			}
		}


	}
	close(0);
	close(1);	
	exit(1);
}


