The system consists of three types of components: game world,player and monsters, all running as separate processes.
The game is turn-based. At the beginning of each turn, the game world informs the player
about its current position, the accumulated damage inflicted on the player by the monsters
(which will be zero in the first turn), the number of alive monsters, the positions of the alive
monsters and whether the game is over or not.
In return, the player responds to the game world by either moving, attacking one or more
monsters, acknowledging its death or leaving the game. Then the game world sends each of
the monsters, their new position, damage inflicted on them by the player, the player’s new
position, and whether the game is over or not. In return, every monster sends a response to the
game world which is either a movement, an attack to the player, or its death announcement.
This loop continues until the game is over.
Both the player and the monsters should interact with the game world using fixed sized
binary messages. They should receive these messages from their standard input and send their
responses via the standard output.
The game world should store the state of the monsters and the player, and update them
according to the messages it receives. At every turn, the game world should first communicate
with the player, then the alive monsters in sequence. The actions of the monsters are carried
out in an order based on their coordinates (so the game state will be deterministic).
None of the game world, the player and the monsters have the complete information about
the game. The game world only stores the information about the positions and the types
of the monsters and the position of the player. It is oblivious to (does not remember) their
internal states like health. On the other hand, the player and the monsters only receive the
external information like the position of other entities via game world messages. So each of
the game world, the player and the monsters should treat each other as black boxes and only
rely on the information they get from the messages/responses.



World tasks: 

1- Send turn message to and receive response from the player,
2- Process the response
3- Send turn message to the every monster,
4- Collect monster responses in a loop, checking for any of the pipe ends for a new responses
and reading them.
5- After all monster responses are received, sort them based on processing order, their
current coordinates. The coordinates on the map will be ordered left to right (first
dimension) and top to bottom (second dimension) in increasing order.
6- For every response:
• Determine the type of the response. The response types and contents are explained
in the Communication subsection.
• Process the response and update the world information.
7- After all responses are processed, print the map to the standard output.



Player
The sample player takes x and y positions of the door, maximum attacks it can do in one turn,
range of its attacks and in which turn to leave after receiving a message from the game world:
> ./player 6 1 4 3 2
where door is at (6,1) , 4 is the maximum number of monsters the player can attack in one
turn, 3 is maximum the distance between it and the monsters it can attack, and 2 is the turn
number the player will leave after receiving a message.
Turn number starts from 1. When the last argument is 0, the player will play the game
without leaving.
The health of the sample player is 10, its defence is 5 and its attack is 7. It uses the same
formula as the sample monster below to calculate its health.
The strategy of the sample player is relatively simple. If there are monsters in its range,
it will attack as many monsters it can (with the maximum of given number) starting from
the one having lowest coordinates. Otherwise it will try to move closer to the door. If there
are multiple directions which may or may not be optimal but still gets the player closer to
the door, it tries to choose different directions as much as it can. 




Monsters
Monsters’ main objective is to kill the player before reaching the door. Beside making sure the
monsters and the game world communicating properly with the correct message and responses,
you will implement a sample monster program. Monsters will always take four command-line
arguments. An example execution is:
./monster 10 1 4 3
• 10 is the health of the monster
• 1 is the damage induced to the player when the monster attacks
• 4 is the defence of the monster. When the player attacks the monster, this value will be
reduced from the damage before removing from the health
• 3 is the range of the attack
When the player is in the range of the monster, if the distance (no diagonal moves) between
two is less than or equal to the range, the monster will attack. If not, the monster will
try to get closer to the player by moving one of the adjacent cells: up,upper-right,right,bottom-
right,down,bottom-left,left,upper-left. When there are more than one directions getting monster
to the best distance, the monster should choose its direction according the list above with up
being the first choice and upper-left being the last. Both range check and distance measure-
ments are based on Manhattan distance: If coordinate of the player is (x, y) and the coordinate
of the monster is (x 0 , y 0 ) the Manhattan distance between two is |x − x 0 | + |y − y 0 |.

The game world, player and monster executables rely on each others’ messages and do not
make consistency assumptions. The only controls enforced by the game world are:
• Entities cannot move to wall cells.
• Entities cannot move out of boundaries.
• Monsters cannot move to the door.
• Entities cannot move to a position occupied by another entity
• If the move is invalid game control returns a different position from target (your imple-
mentation will return the current cell position).







Input & Output
The game world reads the map information from the standard input and prints game infor-
mation to the standard output.


Input :
<width_of_the_room> <height_of_the_room>
<x_position_of_the_door> <y_position_of_the_door>
<x_position_of_the_player> <y_position_of_the_player>
<executable_of_the_player> <argument_3> <argument_4> <argument_5>
<number_of_monsters>
<first_monster_executable> <symbol> <x_position> <y_position> [<arguments>]
<second_monster_executable> <symbol> <x_position> <y_position> [<arguments>]
...
...
<last_monster_executable> <symbol> <x_position> <y_position> [<arguments>]
The monster symbol is an ASCII letter to print in map output.

A sample input is:
6 8
2 0
3 3
./player 3 2 0
2
./monster s 3 5 5 5 1 2
./monster J 1 1 10 6 4 2
With this input the game world will execute:
• ./player 2 0 3 2 0
• ./monster 5 5 1 2
• ./monster 10 6 4 2
processes.



Output :

The header file is given in below:
#ifndef LOGGING_H
#define LOGGING_H
#include "message.h"


typedef struct map_info {
    int map_width, map_height;
    coordinate door;
    coordinate player;
    int alive_monster_count;
    char monster_types[MONSTER_LIMIT];
    coordinate monster_coordinates[MONSTER_LIMIT];
} map_info;


void print_map(map_info *mi);
    typedef enum game_over_status {
    go_reached,
    go_survived,
    go_died,
    go_left,
} game_over_status;

void print_game_over(game_over_status go);
#endif


The function print_map is used to print the map. In the output ‘@’ is the player, ‘#’ is the
wall, ‘+’ is the door, ‘.’ is the empty space and all other ASCII letters are possible monsters.

For the example in 3.1 section, the initial output of the function should be:
10##+###
#J...#
#....#
#..@.#
#....#
#..s.#
#....#


There are four different types of game over messages:
1- go_reached, when the player reached the door
2- go_survived, when the player eliminated all monsters
3- go_left, when the player left the game
4- go_died, when the player died



Sample input can be run with :
> make all
> ./world < input.txt


