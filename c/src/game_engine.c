#include "game_engine.h"
#include "world_loader.h"
#include "graph.h"
#include "player.h"
#include "room.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

/* forward declarations for static helpers implemented later in this file */
static Status handle_portal_transition(GameEngine *eng, const Room *current_room, int new_x, int new_y);
static Status handle_treasure_collection(GameEngine *eng, const Room *current_room, int dest_id);
static Status handle_pushable_move(GameEngine *eng, const Room *current_room, int dest_id, Direction dir, int new_x, int new_y);
static bool is_pushable_on_switch(const Room *room, int pushable_idx);

Status game_engine_create(const char *config_file_path, GameEngine **engine_out){
    if(!config_file_path || !engine_out){
        return INVALID_ARGUMENT;
    }

    //load the world
    Graph *g = NULL;
    Room *first_room = NULL;
    int num_rooms = 0;
    Charset charset;
    Status status = loader_load_world(config_file_path, &g, &first_room, &num_rooms, &charset);
    if(status != OK){
        return status;
    }

    //get starting position in first room
    int x = 0;
    int y = 0;
    status = room_get_start_position(first_room, &x, &y);

    if(status != OK){
        graph_destroy(g);
        return status;
    }

    //create the player in the starting room
    Player *player = NULL;
    status = player_create(first_room->id, x, y, &player);
    if(status != OK){
        graph_destroy(g);
        return status;
    }

    //create the game engine struct
    GameEngine *eng = malloc(sizeof(GameEngine));
    if(!eng){
        player_destroy(player);
        graph_destroy(g);
        return NO_MEMORY;
    }

    //initialize the engine fields
    eng->graph = g;
    eng->player = player;
    eng->charset = charset;
    eng->initial_room_id = first_room->id;
    eng->initial_player_x = x;
    eng->initial_player_y = y;
    eng->room_count = num_rooms;

    //return the engine
    *engine_out = eng;
    return OK;
}

void game_engine_destroy(GameEngine *eng){
    if(!eng){return;}

    //destroy player
    if(eng->player){
        player_destroy(eng->player);
    }

    //destroy graph (which destroys all rooms)
    if(eng->graph){
        graph_destroy(eng->graph);
    }

    //free engine struct
    free(eng);
}

const Player *game_engine_get_player(const GameEngine *eng){
    if(!eng){return NULL;}
    return eng->player;
}

Status game_engine_move_player(GameEngine *eng, Direction dir){
    if(!eng){return INVALID_ARGUMENT;}

    //get current player position
    int x = 0;
    int y = 0;
    Status status = player_get_position(eng->player, &x, &y);
    if(status != OK){
        return INTERNAL_ERROR;
    }

    //determine new position based on direction
    int new_x = x;
    int new_y = y;
    switch(dir){
        case DIR_NORTH: new_y -= 1; break;
        case DIR_SOUTH: new_y += 1; break;
        case DIR_EAST:  new_x += 1; break;
        case DIR_WEST:  new_x -= 1; break;
        default: return INVALID_ARGUMENT;
    }

    //get current room
    int room_id = player_get_room(eng->player);
    Room key = { .id = room_id };
    const Room *current_room = graph_get_payload(eng->graph, &key);
    if(!current_room){
        return GE_NO_SUCH_ROOM;
    }

    // classify the destination tile so we can handle treasures/portals
    int dest_id = -1;
    RoomTileType tile_type = room_classify_tile(current_room, new_x, new_y, &dest_id);

    // dispatch to helpers based on tile type
    if(tile_type == ROOM_TILE_PORTAL){
        status = player_set_position(eng->player, new_x, new_y);
        if(status != OK){
            return INTERNAL_ERROR;
        }
        return OK;
    }

    if(tile_type == ROOM_TILE_TREASURE){
        return handle_treasure_collection(eng, current_room, dest_id);
    }

    if(tile_type == ROOM_TILE_PUSHABLE){
        if(is_pushable_on_switch(current_room, dest_id)){
            status = player_set_position(eng->player, new_x, new_y);
            if(status != OK){
                return INTERNAL_ERROR;
            }
            return OK;
        }
        return handle_pushable_move(eng, current_room, dest_id, dir, new_x, new_y);
    }

    // default: ensure tile is walkable and move
    if(!room_is_walkable(current_room, new_x, new_y)){
        return ROOM_IMPASSABLE;
    }

    status = player_set_position(eng->player, new_x, new_y);
    if(status != OK){
        return INTERNAL_ERROR;
    }

    return OK;
}

Status game_engine_enter_portal(GameEngine *eng){
    if(!eng){
        return INVALID_ARGUMENT;
    }

    int room_id = player_get_room(eng->player);
    Room key = { .id = room_id };
    const Room *current_room = graph_get_payload(eng->graph, &key);
    if(!current_room){
        return GE_NO_SUCH_ROOM;
    }

    int x = 0;
    int y = 0;
    Status status = player_get_position(eng->player, &x, &y);
    if(status != OK){
        return INTERNAL_ERROR;
    }

    int dest_id = -1;
    RoomTileType tile_type = room_classify_tile(current_room, x, y, &dest_id);
    if(tile_type != ROOM_TILE_PORTAL){
        return ROOM_NO_PORTAL;
    }

    return handle_portal_transition(eng, current_room, x, y);
}

// Helper: perform portal transition (move player to destination room start)
static Status handle_portal_transition(GameEngine *eng, const Room *current_room, int new_x, int new_y){
    // Find the portal at the destination tile
    const Portal *portal = NULL;
    for(int i = 0; i < current_room->portal_count; i++){
        if(current_room->portals[i].x == new_x && current_room->portals[i].y == new_y){
            portal = &current_room->portals[i];
            break;
        }
    }
    if(!portal){
        return ROOM_NO_PORTAL;
    }

    //portal is locked
    if(portal->gated){
        return ROOM_IMPASSABLE;
    }

    // Portal with no valid destination cannot be entered.
    if(portal->target_room_id < 0){
        return ROOM_NO_PORTAL;
    }

    

    // Perform the room transition
    Room dest = { .id = portal->target_room_id };
    const Room *target_room = graph_get_payload(eng->graph, &dest);
    if(!target_room){
        return GE_NO_SUCH_ROOM;
    }

    int sx = 0;
    int sy = 0;
    Status status = room_get_start_position(target_room, &sx, &sy);
    if(status != OK){
        return status;
    }

    status = player_move_to_room(eng->player, portal->target_room_id);
    if(status != OK){
        return INTERNAL_ERROR;
    }

    status = player_set_position(eng->player, sx, sy);
    if(status != OK){
        return INTERNAL_ERROR;
    }

    return OK;
}

// Helper: collect treasure at dest_id in current room
static Status handle_treasure_collection(GameEngine *eng, const Room *current_room, int dest_id){
    Room *room_mut = (Room *)current_room;
    Treasure *treasure = NULL;
    Status rstat = room_pick_up_treasure(room_mut, dest_id, &treasure);
    if(rstat != OK || treasure == NULL){
        return ROOM_IMPASSABLE;
    }

    Status pstat = player_try_collect(eng->player, treasure);
    if(pstat != OK){
        return INTERNAL_ERROR;
    }

    return OK;
}

// Helper: push object and move player into its former spot
static Status handle_pushable_move(GameEngine *eng, const Room *current_room, int dest_id, Direction dir, int new_x, int new_y){
    Room *room_mut = (Room *)current_room;
    Status pr = room_try_push(room_mut, dest_id, dir);
    if(pr != OK){
        return ROOM_IMPASSABLE;
    }

    Status status = player_set_position(eng->player, new_x, new_y);
    if(status != OK){
        return INTERNAL_ERROR;
    }

    return OK;
}

// A pushable occupying a switch tile should not block player movement.
static bool is_pushable_on_switch(const Room *room, int pushable_idx){
    if(!room || pushable_idx < 0 || pushable_idx >= room->pushable_count){
        return false;
    }

    int px = room->pushables[pushable_idx].x;
    int py = room->pushables[pushable_idx].y;

    for(int i = 0; i < room->switch_count; i++){
        if(room->switches[i].x == px && room->switches[i].y == py){
            return true;
        }
    }

    return false;
}

Status game_engine_get_room_count(const GameEngine *eng,int *count_out){
    if(!eng){return INVALID_ARGUMENT;}
    if(!count_out){return NULL_POINTER;}

    *count_out = eng->room_count;
    return OK;
}

Status game_engine_get_room_dimensions(const GameEngine *eng, int *width_out, int *height_out){
    if(!eng){return INVALID_ARGUMENT;}
    if(!width_out || !height_out){return NULL_POINTER;}

    //get current room
    int room_id = player_get_room(eng->player);
    Room key = { .id = room_id };
    const Room *current_room = graph_get_payload(eng->graph, &key);
    if(!current_room){
        return GE_NO_SUCH_ROOM;
    }

    *width_out = room_get_width(current_room);
    *height_out = room_get_height(current_room);
    return OK;
}

Status game_engine_get_charset(const GameEngine *eng, Charset *charset_out){
    if(!eng){
        return INVALID_ARGUMENT;
    }
    if(!charset_out){
        return NULL_POINTER;
    }

    *charset_out = eng->charset;
    return OK;
}

Status game_engine_get_current_room_name(const GameEngine *eng, char **name_out){
    if(!eng || !name_out){
        return INVALID_ARGUMENT;
    }

    int room_id = player_get_room(eng->player);
    Room key = { .id = room_id };
    const Room *current_room = graph_get_payload(eng->graph, &key);
    if(!current_room){
        return GE_NO_SUCH_ROOM;
    }

    const char *src = current_room->name ? current_room->name : "";
    size_t len = strlen(src);
    char *copy = malloc(len + 1);
    if(!copy){
        return NO_MEMORY;
    }

    memcpy(copy, src, len + 1);
    *name_out = copy;
    return OK;
}

Status game_engine_can_enter_portal_at(const GameEngine *eng, int x, int y, bool *can_enter_out){
    if(!eng || !can_enter_out){
        return INVALID_ARGUMENT;
    }

    int room_id = player_get_room(eng->player);
    Room key = { .id = room_id };
    const Room *current_room = graph_get_payload(eng->graph, &key);
    if(!current_room){
        return GE_NO_SUCH_ROOM;
    }

    const Portal *portal = NULL;
    for(int i = 0; i < current_room->portal_count; i++){
        if(current_room->portals[i].x == x && current_room->portals[i].y == y){
            portal = &current_room->portals[i];
            break;
        }
    }

    if(!portal){
        return ROOM_NO_PORTAL;
    }

    *can_enter_out = !portal->gated;
    return OK;
}

Status game_engine_reset(GameEngine *eng){
    if(!eng){return INVALID_ARGUMENT;}

    //reset player to initial state
    Status status = player_reset_to_start(eng->player,
                                          eng->initial_room_id,
                                          eng->initial_player_x,
                                          eng->initial_player_y);
    if(status != OK){
        return INTERNAL_ERROR;
    }

    //reset treasures in all rooms
    const void * const *payloads = NULL;
    int room_count = 0;
    GraphStatus gstat = graph_get_all_payloads(eng->graph, &payloads, &room_count);
    if(gstat != GRAPH_STATUS_OK){
        return INTERNAL_ERROR;
    }

    for(int i = 0; i < room_count; i++){
        Room *room = (Room *)payloads[i];
        for(int j = 0; j < room->treasure_count; j++){
            room->treasures[j].collected = false;
            room->treasures[j].x = room->treasures[j].initial_x;
            room->treasures[j].y = room->treasures[j].initial_y;
        }
    }

    //reset pushables in all rooms
    for(int i = 0; i < room_count; i++){
        Room *room = (Room *)payloads[i];
        for(int j = 0; j < room->pushable_count; j++){
            room->pushables[j].x = room->pushables[j].initial_x;
            room->pushables[j].y = room->pushables[j].initial_y;
        }
    }

    return OK;
}

Status game_engine_render_current_room(const GameEngine *eng, char **str_out){
    if(!eng || !str_out){
        return INVALID_ARGUMENT;
    }

    //get current room
    int room_id = player_get_room(eng->player);
    Room key = { .id = room_id };
    const Room *current_room = graph_get_payload(eng->graph, &key);
    if(!current_room){
        return GE_NO_SUCH_ROOM;
    }

    //render the room into a buffer
    int width = room_get_width(current_room);
    int height = room_get_height(current_room);
    char *buffer = malloc(sizeof(char) * width * height);
    if(!buffer){
        return NO_MEMORY;
    }

    Status status = room_render(current_room, &eng->charset, buffer, width, height);
    if(status != OK){
        free(buffer);
        return status;
    }

    //overlay the player character
    int x = 0;
    int y = 0;
    status = player_get_position(eng->player, &x, &y);
    if(status != OK){
        free(buffer);
        return INTERNAL_ERROR;
    }
    buffer[y * width + x] = eng->charset.player;

    //convert buffer to multi line string
    int line_length = width + 1; // +1 for newline
    int total_length = line_length * height + 1; // +1 for null terminator
    char *output_str = malloc(sizeof(char) * total_length);
    if(!output_str){
        free(buffer);
        return NO_MEMORY;
    }

    //copy buffer to output string with new lines and null terminator
    for(int row = 0; row < height; row++){
        memcpy(&output_str[(ptrdiff_t)(row * line_length)], &buffer[(ptrdiff_t)(row * width)], width);
        output_str[row * line_length + width] = '\n';
    }
    output_str[total_length - 1] = '\0';

    free(buffer);
    *str_out = output_str;
    return OK;
}

Status game_engine_render_room(const GameEngine *eng, int room_id, char **str_out){
    if(!eng || !str_out){
        return INVALID_ARGUMENT;
    }

    //get current room
    Room key = { .id = room_id };
    const Room *current_room = graph_get_payload(eng->graph, &key);
    if(!current_room){
        return GE_NO_SUCH_ROOM;
    }

    //render the room into a buffer
    int width = room_get_width(current_room);
    int height = room_get_height(current_room);
    char *buffer = malloc((long)width * (long)height);
    if(!buffer){
        return NO_MEMORY;
    }

    Status status = room_render(current_room, &eng->charset, buffer, width, height);
    if(status != OK){
        free(buffer);
        return status;
    }

    //convert buffer to multi line string
    int line_length = width + 1; // +1 for newline
    int total_length = line_length * height + 1; // +1 for null terminator
    char *output_str = malloc(sizeof(char) * total_length);
    if(!output_str){
        free(buffer);
        return NO_MEMORY;
    }

    //copy buffer to output string with new lines and null terminator
    for(int row = 0; row < height; row++){
        memcpy(&output_str[(ptrdiff_t)(row * line_length)], &buffer[(ptrdiff_t)(row * width)], width);
        output_str[row * line_length + width] = '\n';
    }
    output_str[total_length - 1] = '\0';

    free(buffer);
    *str_out = output_str;
    return OK;
}

Status game_engine_get_room_ids(const GameEngine *eng, int **ids_out, int *count_out){
    if(!eng){return INVALID_ARGUMENT;}
    if(!ids_out || !count_out){return NULL_POINTER;}

    int room_count = 0;

    //get all room payloads (array owned by graph)
    const void * const *payloads = NULL;
    GraphStatus gstat = graph_get_all_payloads(eng->graph, &payloads, &room_count);
    if(gstat != GRAPH_STATUS_OK){
        return INTERNAL_ERROR;
    }

    //get ids from the rooms array
    int *ids = malloc(sizeof(int) * room_count);
    if(!ids){
        return NO_MEMORY;
    }

    for(int i = 0; i < room_count; i++){
        const Room *r = (const Room *)payloads[i];
        ids[i] = r->id;
    }

    //set outputs
    *ids_out = ids;
    *count_out = room_count;
    return OK;
}

// *****************************************************************
//                      A2 Functions
// *****************************************************************

void game_engine_free_string(void *ptr){
    free(ptr);
}

//get total treasures and collected treasures in the game (helper for score and win condition)
Status game_engine_get_treasure_stats(const GameEngine *eng,
                                 int *total_treasures_out,
                                 int *collected_treasures_out){
    if(!eng){return INVALID_ARGUMENT;}
    if(!total_treasures_out || !collected_treasures_out){return NULL_POINTER;}

    const void * const *payloads = NULL;
    int room_count = 0;
    GraphStatus gstat = graph_get_all_payloads(eng->graph, &payloads, &room_count);
    if(gstat != GRAPH_STATUS_OK){
        return INTERNAL_ERROR;
    }

    int total = 0;
    int collected = 0;

    for(int i = 0; i < room_count; i++){
        const Room *r = (const Room *)payloads[i];
        for(int j = 0; j < r->treasure_count; j++){
            total += 1;
            if(r->treasures[j].collected){
                collected += 1;
            }
        }
    }

    *total_treasures_out = total;
    *collected_treasures_out = collected;
    return OK;
}