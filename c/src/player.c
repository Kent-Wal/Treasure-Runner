#include "player.h"
#include <stdlib.h>

Status player_create(int initial_room_id, int initial_x, int initial_y, Player **player_out){
    if(!player_out){
        return INVALID_ARGUMENT;
    }

    Player *p = *player_out;
    if(!p){
        p = malloc(sizeof(Player));
        if(!p){
            return NO_MEMORY;
        }
        *player_out = p;
    }

    p->room_id = initial_room_id;
    p->x = initial_x;
    p->y = initial_y;

    // Initialize collected treasures array
    p->collected_treasures = NULL;
    p->collected_count = 0;

    return OK;
}

void player_destroy(Player *p){
    if(p){
        if(p->collected_treasures){
            //set treasures to uncollected since player is being destroyed
            for(int i = 0; i < p->collected_count; i++){                
                p->collected_treasures[i]->collected = false;
            }
            free(p->collected_treasures);
        }
        p->collected_treasures = NULL;
        p->collected_count = 0;
        free(p);
    }
}

int player_get_room(const Player *p){
    if(!p){return -1;}
    return p->room_id;
}

Status player_get_position(const Player *p, int *x_out, int *y_out){
    if(!p || !x_out || !y_out){
        return INVALID_ARGUMENT;
    }

    *x_out = p->x;
    *y_out = p->y;
    return OK;
}

Status player_set_position(Player *p, int x, int y){
    if(!p){
        return INVALID_ARGUMENT;
    }

    p->x = x;
    p->y = y;
    return OK;
}

Status player_move_to_room(Player *p, int new_room_id){
    if(!p){
        return INVALID_ARGUMENT;
    }

    p->room_id = new_room_id;
    return OK;
}

Status player_reset_to_start(Player *p, int starting_room_id, int start_x, int start_y){
    if(!p){
        return INVALID_ARGUMENT;
    }

    p->room_id = starting_room_id;
    p->x = start_x;
    p->y = start_y;

    //reset collected treasures
    if(p->collected_treasures){
        //set treasures to uncollected since player is being reset
        for(int i = 0; i < p->collected_count; i++){
            p->collected_treasures[i]->collected = false;
        }
        free(p->collected_treasures);
    }
    p->collected_treasures = NULL;
    p->collected_count = 0;
    return OK;
}


// *****************************************************************
//                      A2 Functions
// *****************************************************************

Status player_try_collect(Player *p, Treasure *treasure){
    if(!p || !treasure){return NULL_POINTER;}

    // Check if player already has collected this treasure
    if(player_has_collected_treasure(p, treasure->id)){return INVALID_ARGUMENT;}

    //expand collected treasures array for player
    Treasure **new_collected = realloc(p->collected_treasures, sizeof(Treasure *) * (p->collected_count + 1));
    if(!new_collected){
        return NO_MEMORY;
    }

    //add treasure to player's collected array and set collected status
    p->collected_treasures = new_collected;
    p->collected_treasures[p->collected_count] = treasure;
    p->collected_count++;
    treasure->collected = true;
    return OK;
}

bool player_has_collected_treasure(const Player *p, int treasure_id){
    if(!p || treasure_id < 0){return false;}

    for(int i = 0; i < p->collected_count; i++){
        if(p->collected_treasures[i]->id == treasure_id){
            return true;
        }
    }
    return false;
}

int player_get_collected_count(const Player *p){
    if(!p){return 0;}
    return p->collected_count;
}

const Treasure * const * player_get_collected_treasures(const Player *p, int *count_out){
    if(!p || !count_out){return NULL;}

    *count_out = p->collected_count;
    return (const Treasure * const *)p->collected_treasures;
}