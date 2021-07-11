#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include "logging.h"
#include "message.h"
#include <sys/wait.h>
#include <limits.h>
#ifdef SYSVR4
#define PIPE(a) pipe(a)
#else
#include<sys/socket.h>
#define PIPE(a) socketpair(AF_UNIX, SOCK_STREAM,PF_UNIX,(a))
#endif


typedef struct monster_information {
	coordinate current_position;
	pid_t monster_id;
	bool monster_alive;
	bool process_flag;
	char monster_character;
	int file_descriptor_of_monster;
	monster_response current_response;
} monster_information;


typedef struct player_information {
	coordinate current_position;
	int file_descriptor_of_player;
	pid_t player_id;
} player_information;


bool is_movement_valid(player_information player,monster_information monsters[],int monster_number,coordinate movement,int width,int height,coordinate door_position,bool is_monster) {
	if (movement.x <= 0 || movement.y <= 0 || movement.x >= width-1 || movement.y >= height-1) {					
		return false;
	}
	
	for (int i=0;i<monster_number;i++) {
		if (monsters[i].monster_alive) {
			if (monsters[i].current_position.x == movement.x && monsters[i].current_position.y == movement.y) {	 
				return false;
			}
		}
	}
	
	if (is_monster) {
		if (movement.x == door_position.x && movement.y == door_position.y) {							
			return false;
		}

		if (movement.x == player.current_position.x && movement.y == player.current_position.y) {		
			return false;																				
		}
	}

	return true;

}


void print_map_helper(monster_information monsters[],map_info *map,coordinate player_position,int alive_monsters,int total_monster_count) {
	map->player.x = player_position.x;
	map->player.y = player_position.y;
	map->alive_monster_count = alive_monsters;
	for (int j=0;j<total_monster_count;j++) {
		monsters[j].process_flag = false;
	}
	int min_monster_id;
	coordinate min_coordinate;
	

	for (int i=0;i<alive_monsters;i++) {
		min_coordinate.x = INT_MAX;
		min_coordinate.y = INT_MAX;
		
		for (int j=0;j<total_monster_count;j++) {
			if (monsters[j].monster_alive && (!(monsters[j].process_flag))) {
				if (monsters[j].current_position.x < min_coordinate.x) {
					min_monster_id = monsters[j].monster_id;
					min_coordinate.x = monsters[j].current_position.x;
					min_coordinate.y = monsters[j].current_position.y;
				}
				if (monsters[j].current_position.x == min_coordinate.x && monsters[j].current_position.y < min_coordinate.y) {
					min_monster_id = monsters[j].monster_id;
					min_coordinate.x = monsters[j].current_position.x;
					min_coordinate.y = monsters[j].current_position.y;
				}
			}
		}

		for (int j=0;j<total_monster_count;j++) {
			if (monsters[j].monster_id == min_monster_id) {
				monsters[j].process_flag = true;
				map->monster_types[i] = monsters[j].monster_character;
				map->monster_coordinates[i].x = monsters[j].current_position.x;
				map->monster_coordinates[i].y = monsters[j].current_position.y;
			}
		}
	
	}
	print_map(map);	
	return;
}


void get_coordinates(monster_information monsters[],coordinate monster_coordinates[],int alive_monsters,int total_monster_count) {
	for (int j=0;j<total_monster_count;j++) {
		monsters[j].process_flag = false;
	}
	int min_monster_id;
	coordinate min_coordinate;
	
	for (int i=0;i<alive_monsters;i++) {
		min_coordinate.x = INT_MAX;
		min_coordinate.y = INT_MAX;
		
		for (int j=0;j<total_monster_count;j++) {
			if (monsters[j].monster_alive && (!(monsters[j].process_flag))) {
				if (monsters[j].current_position.x < min_coordinate.x) {
					min_monster_id = monsters[j].monster_id;
					min_coordinate.x = monsters[j].current_position.x;
					min_coordinate.y = monsters[j].current_position.y;
				}
				if (monsters[j].current_position.x == min_coordinate.x && monsters[j].current_position.y < min_coordinate.y) {
					min_monster_id = monsters[j].monster_id;
					min_coordinate.x = monsters[j].current_position.x;
					min_coordinate.y = monsters[j].current_position.y;
				}
			}
		}

		for (int j=0;j<total_monster_count;j++) {
			if (monsters[j].monster_id == min_monster_id) {
				monsters[j].process_flag = true;
				monster_coordinates[i].x = monsters[j].current_position.x;
				monster_coordinates[i].y = monsters[j].current_position.y;
			}
		}
	
	}	
	
}


int find_damage_to_the_monster(coordinate monster_coordinates[],int attack[],coordinate monster_position,int alive_monsters){
	for (int i=0;i<alive_monsters;i++) {
		if (monster_coordinates[i].x == monster_position.x && monster_coordinates[i].y == monster_position.y)
			return attack[i];
	}
	return 0;
}


int main() {
	monster_information monsters[MONSTER_LIMIT];
	player_information player;
	player_message message_to_player;
	player_response response_of_player;
	monster_message message_to_monster;
	monster_response response_of_monster;
	map_info map;	
	char player_executable[PATH_MAX];
	char monster_executable[PATH_MAX];
	char monster_symbol;
	int width,height,x_of_door,y_of_door,x_of_player,y_of_player;
	char third_player_argument[PATH_MAX],fourth_player_argument[PATH_MAX],fifth_player_argument[PATH_MAX];
	int x_of_monster,y_of_monster,monster_number_at_beginning,number_of_alive_monsters,monster_id;
	char first_monster_argument[PATH_MAX],second_monster_argument[PATH_MAX],third_monster_argument[PATH_MAX],fourth_monster_argument[PATH_MAX];
	scanf("%d %d",&width,&height);
	scanf("%d %d",&x_of_door,&y_of_door);
	scanf("%d %d",&x_of_player,&y_of_player);
	scanf("%s %s %s %s",player_executable,third_player_argument,fourth_player_argument,fifth_player_argument);
	scanf("%d",&monster_number_at_beginning);
	player.current_position.x = x_of_player;
	player.current_position.y = y_of_player;
	number_of_alive_monsters = monster_number_at_beginning;
	map.map_width = width;
	map.map_height = height;
	map.door.x = x_of_door;
	map.door.y = y_of_door;
	int file_descriptors[2],player_id;
	PIPE(file_descriptors);
	player_id = fork();

	if (player_id) {
		close(file_descriptors[1]);
		player.current_position.x = x_of_player;
		player.current_position.y = y_of_player;
		player.file_descriptor_of_player = file_descriptors[0];
		player.player_id = player_id;
		if (read(player.file_descriptor_of_player,&response_of_player,sizeof(response_of_player)) > 0) {
			if (response_of_player.pr_type != pr_ready)
				return 0;
		}
		else {
			return 0;
		}
	}


	else {
		char arg1[30],arg2[30];
		close(file_descriptors[0]);
		dup2(file_descriptors[1],1);
		dup2(file_descriptors[1],0);
		close(file_descriptors[1]);
		sprintf(arg1,"%d",x_of_door);
		sprintf(arg2,"%d",y_of_door);
		execl(player_executable,player_executable,arg1,arg2,third_player_argument,fourth_player_argument,fifth_player_argument,NULL);
	}



	for (int i=0;i<monster_number_at_beginning;i++) {
		scanf("%s %c %d %d %s %s %s %s",monster_executable,&monster_symbol,&x_of_monster,&y_of_monster,first_monster_argument,second_monster_argument,third_monster_argument,fourth_monster_argument);
		int fd[2];
		PIPE(fd);
		monster_id = fork();
		if (monster_id) {
			close(fd[1]);
			monsters[i].file_descriptor_of_monster = fd[0];
			monsters[i].current_position.x = x_of_monster;
			monsters[i].current_position.y = y_of_monster;
			monsters[i].monster_alive = true;
			monsters[i].monster_character = monster_symbol;
			monsters[i].monster_id = monster_id;
			if (read(monsters[i].file_descriptor_of_monster,&response_of_monster,sizeof(response_of_monster)) > 0) {
				if (response_of_monster.mr_type != mr_ready)
					return 0;
			}
			else {
				return 0;
			}
		}
		else {
			close(fd[0]);
			dup2(fd[1],1);
			dup2(fd[1],0);
			close(fd[1]);
			execl(monster_executable,monster_executable,first_monster_argument,second_monster_argument,third_monster_argument,fourth_monster_argument,NULL);
		}
	}
	print_map_helper(monsters,&map,player.current_position,number_of_alive_monsters,monster_number_at_beginning);
	int total_damage_to_the_player = 0;
	bool reached=false,survived=false,died=false,left=false,is_player_attacked=false;
	coordinate door;
	door.x = x_of_door;
	door.y = y_of_door;
	
	while(true) {
		is_player_attacked=false;
		get_coordinates(monsters,message_to_player.monster_coordinates,number_of_alive_monsters,monster_number_at_beginning);
		message_to_player.alive_monster_count = number_of_alive_monsters;
		message_to_player.game_over = false;
		message_to_player.total_damage = total_damage_to_the_player;
		total_damage_to_the_player = 0;
		message_to_player.new_position.x = player.current_position.x;
		message_to_player.new_position.y = player.current_position.y;
		write(player.file_descriptor_of_player,&message_to_player,sizeof(message_to_player));

		if (read(player.file_descriptor_of_player,&response_of_player,sizeof(response_of_player)) == 0) {
			waitpid(player.player_id,NULL,0);
			close(player.file_descriptor_of_player);
			left = true;
			break;
		}

		else if (response_of_player.pr_type == pr_dead) {
			waitpid(player.player_id,NULL,0);
			close(player.file_descriptor_of_player);
			died = true;
			break;
		}

		else if (response_of_player.pr_type == pr_move) {
			if (response_of_player.pr_content.move_to.x == x_of_door && response_of_player.pr_content.move_to.y == y_of_door) {
				player.current_position.x = x_of_door;
				player.current_position.y = y_of_door;
				reached = true;
				break;
			}
			else if (is_movement_valid(player,monsters,monster_number_at_beginning,response_of_player.pr_content.move_to,width,height,door,false)) {
				player.current_position.x = response_of_player.pr_content.move_to.x;
				player.current_position.y = response_of_player.pr_content.move_to.y;
			}
		}

		else if (response_of_player.pr_type == pr_attack) {
			is_player_attacked = true;
		}


		for (int i=0;i<monster_number_at_beginning;i++) {
			if (monsters[i].monster_alive) {
				message_to_monster.game_over = false;
				message_to_monster.player_coordinate.x = player.current_position.x;
				message_to_monster.player_coordinate.y = player.current_position.y;
				message_to_monster.new_position.x = monsters[i].current_position.x;
				message_to_monster.new_position.y = monsters[i].current_position.y;
				if (!is_player_attacked)
					message_to_monster.damage = 0;
				else 
					message_to_monster.damage = find_damage_to_the_monster(message_to_player.monster_coordinates,response_of_player.pr_content.attacked,monsters[i].current_position,number_of_alive_monsters);
				write(monsters[i].file_descriptor_of_monster,&message_to_monster,sizeof(message_to_monster));
				read(monsters[i].file_descriptor_of_monster,&response_of_monster,sizeof(response_of_monster));
				monsters[i].current_response = response_of_monster;
			}
		}

		for (int j=0;j<monster_number_at_beginning;j++) {
			monsters[j].process_flag = false;
		}
		int min_monster_id,death_at_this_turn = 0;
		coordinate min_coordinate;
		for (int i=0;i<number_of_alive_monsters;i++) {
			min_coordinate.x = INT_MAX;
			min_coordinate.y = INT_MAX;
			
			for (int j=0;j<monster_number_at_beginning;j++) {
				if (monsters[j].monster_alive && (!(monsters[j].process_flag))) {
					if (monsters[j].current_position.x < min_coordinate.x) {
						min_monster_id = monsters[j].monster_id;
						min_coordinate.x = monsters[j].current_position.x;
						min_coordinate.y = monsters[j].current_position.y;
					}
					if (monsters[j].current_position.x == min_coordinate.x && monsters[j].current_position.y < min_coordinate.y) {
						min_monster_id = monsters[j].monster_id;
						min_coordinate.x = monsters[j].current_position.x;
						min_coordinate.y = monsters[j].current_position.y;
					}
				}
			}
			for (int j=0;j<monster_number_at_beginning;j++) {
				if (monsters[j].monster_id == min_monster_id) {
					monsters[j].process_flag = true;
					if (monsters[j].current_response.mr_type == mr_dead) {
						death_at_this_turn++;
						monsters[j].monster_alive = false;
						waitpid(monsters[j].monster_id,NULL,0);
						close(monsters[j].file_descriptor_of_monster);
					}

					else if (monsters[j].current_response.mr_type == mr_attack) {
						total_damage_to_the_player += monsters[j].current_response.mr_content.attack;
					}

					else if (monsters[j].current_response.mr_type == mr_move) {
						if (is_movement_valid(player,monsters,monster_number_at_beginning,monsters[j].current_response.mr_content.move_to,width,height,door,true)) {
							monsters[j].current_position.x = monsters[j].current_response.mr_content.move_to.x;
							monsters[j].current_position.y = monsters[j].current_response.mr_content.move_to.y;
						}
					}


				}
			}
		}
		number_of_alive_monsters = number_of_alive_monsters-death_at_this_turn;
		if (number_of_alive_monsters <= 0) {
			survived = true;
			break;
		}
		print_map_helper(monsters,&map,player.current_position,number_of_alive_monsters,monster_number_at_beginning);
	}

	

	for (int i=0;i<monster_number_at_beginning;i++) {
		if (monsters[i].monster_alive) {
			message_to_monster.game_over = true;
			write(monsters[i].file_descriptor_of_monster,&message_to_monster,sizeof(message_to_monster));
			waitpid(monsters[i].monster_id,NULL,0);
			close(monsters[i].file_descriptor_of_monster);
		}
	}
	
	if (reached || survived) {
		message_to_player.game_over = true;
		write(player.file_descriptor_of_player,&message_to_player,sizeof(message_to_player));
		waitpid(player.player_id,NULL,0);
		close(player.file_descriptor_of_player);
	}

	
	print_map_helper(monsters,&map,player.current_position,number_of_alive_monsters,monster_number_at_beginning);
	if (died)
		print_game_over(go_died);
	if (left)
		print_game_over(go_left);
	if (reached)
		print_game_over(go_reached);
	if (survived)
		print_game_over(go_survived);
	


}



