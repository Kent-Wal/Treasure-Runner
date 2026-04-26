#include "room.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

Room *room_create(int id, const char *name, int width, int height){
    //allocate the room
    Room *r = malloc(sizeof(Room));
    if(!r){return NULL;}

    r->id = id;

    //set width and height with minimum of 10
    if(width <= 0){
        r->width = 1;
    }
    else{
        r->width = width;
    }

    if(height <= 0){
        r->height = 1;
    }
    else{
        r->height = height;
    }

    //initialize room fields
    r->floor_grid = NULL;
    r->portals = NULL;
    r->portal_count = 0;
    r->treasures = NULL;
    r->treasure_count = 0;
    r->pushables = NULL;
    r->pushable_count = 0;
    r->neighbors = NULL;
    r->neighbor_count = 0;
    r->switches = NULL;
    r->switch_count = 0;

    //allocate and initialize default floor grid
    r->floor_grid = malloc(sizeof(bool) * r->width * r->height);
    if(!r->floor_grid){
        free(r);
        return NULL;
    }
    for(int yy = 0; yy < r->height; yy++){
        for(int xx = 0; xx < r->width; xx++){
            if(xx == 0 || xx == r->width - 1 || yy == 0 || yy == r->height - 1){
                r->floor_grid[yy * r->width + xx] = false;
            }
            else{
                r->floor_grid[yy * r->width + xx] = true;
            }
        }
    }

    //copy the name
    if(name){
        r->name = malloc(strlen(name) + 1);
        if(!r->name){
            if(r->floor_grid){
                free(r->floor_grid);
            }
            free(r);
            return NULL;
        }
        strcpy(r->name, name);
    } 
    else {
        r->name = NULL;
    }

    return r;
}

int room_get_width(const Room *r){
    if(!r){return 0;}
    return r->width;
}

int room_get_height(const Room *r){
    if(!r){return 0;}
    return r->height;
}

Status room_set_floor_grid(Room *r, bool *floor_grid){
    if(!r){return INVALID_ARGUMENT;}

    if(floor_grid){
        //free old grid and set the new one
        if(r->floor_grid){
            free(r->floor_grid);
        }
        r->floor_grid = floor_grid;
    }
    else{
        if(r->floor_grid){
            free(r->floor_grid);
        }

        //allocate new space after freeing old grid
        r->floor_grid = malloc(sizeof(bool) * r->width * r->height);
        if(!r->floor_grid){
            return NO_MEMORY;
        }

        for(int yy = 0; yy < r->height; yy++){
            for(int xx = 0; xx < r->width; xx++){
                //set perimeter walls
                if(xx == 0 || xx == r->width - 1 || yy == 0 || yy == r->height - 1){
                    r->floor_grid[yy * r->width + xx] = false;
                }
                else{
                    r->floor_grid[yy * r->width + xx] = true;
                }
            }
        }
    }

    return OK;
}

// helper: free a portals array (names and array)
static void free_portals_array(Portal *p, int count){
    if(!p) return;
    for(int i = 0; i < count; i++){
        if(p[i].name) free(p[i].name);
    }
    free(p);
}

// helper: free a treasures array (names and array)
static void free_treasures_array(Treasure *t, int count){
    if(!t) return;
    for(int i = 0; i < count; i++){
        if(t[i].name) free(t[i].name);
    }
    free(t);
}

Status room_set_portals(Room *r, Portal *portals, int portal_count){
    if(!r || portal_count < 0 || (portal_count > 0 && !portals)){
        return INVALID_ARGUMENT;
    }

    // free any existing portals
    if(r->portals){
        free_portals_array(r->portals, r->portal_count);
        r->portals = NULL;
        r->portal_count = 0;
    }

    if(portal_count == 0){
        r->portals = NULL;
        r->portal_count = 0;
        return OK;
    }

    r->portals = portals;
    r->portal_count = portal_count;
    return OK;
}

Status room_set_treasures(Room *r, Treasure *treasures, int treasure_count){
    if(!r || treasure_count < 0 || (treasure_count > 0 && !treasures)){
        return INVALID_ARGUMENT;
    }

    // free any existing treasures
    if(r->treasures){
        free_treasures_array(r->treasures, r->treasure_count);
        r->treasures = NULL;
        r->treasure_count = 0;
    }

    if(treasure_count == 0){
        r->treasures = NULL;
        r->treasure_count = 0;
        return OK;
    }

    r->treasures = treasures;
    r->treasure_count = treasure_count;
    return OK;
}

static void render_floor_layer(const Room *r, const Charset *charset, char *buffer){
    for(int yy = 0; yy < r->height; yy++){
        for(int xx = 0; xx < r->width; xx++){
            if(r->floor_grid[yy * r->width + xx] == true){
                buffer[yy * r->width + xx] = charset->floor;
            }
            else{
                buffer[yy * r->width + xx] = charset->wall;
            }
        }
    }
}

static void render_treasure_layer(const Room *r, const Charset *charset, char *buffer){
    for(int i = 0; i < r->treasure_count; i++){
        if(r->treasures[i].collected == false){
            int tx = r->treasures[i].x;
            int ty = r->treasures[i].y;
            buffer[ty * r->width + tx] = charset->treasure;
        }
    }
}

static void render_switch_layer(const Room *r, const Charset *charset, char *buffer){
    for(int i = 0; i < r->switch_count; i++){
        int sx = r->switches[i].x;
        int sy = r->switches[i].y;
        buffer[sy * r->width + sx] = charset->switch_off;
    }
}

static void render_portal_layer(const Room *r, const Charset *charset, char *buffer){
    for(int i = 0; i < r->portal_count; i++){
        int px = r->portals[i].x;
        int py = r->portals[i].y;
        buffer[py * r->width + px] = charset->portal;
    }
}

static bool is_switch_position(const Room *r, int x, int y){
    for(int i = 0; i < r->switch_count; i++){
        if(r->switches[i].x == x && r->switches[i].y == y){
            return true;
        }
    }
    return false;
}

static void render_pushable_layer(const Room *r, const Charset *charset, char *buffer){
    for(int i = 0; i < r->pushable_count; i++){
        int px = r->pushables[i].x;
        int py = r->pushables[i].y;

        if(is_switch_position(r, px, py)){
            buffer[py * r->width + px] = charset->switch_on;
        }
        else{
            buffer[py * r->width + px] = charset->pushable;
        }
    }
}

Status room_place_treasure(Room *r, const Treasure *treasure){
    if(!r || !treasure){return INVALID_ARGUMENT;}

    //get name copy
    char *name_copy = NULL;
    if(treasure->name){
        name_copy = malloc(strlen(treasure->name) + 1);
        if(!name_copy){
            return NO_MEMORY;
        }
        strcpy(name_copy, treasure->name);
    }

    //expand the treasures array
    Treasure *new_treasures = realloc(r->treasures, sizeof(Treasure) * (r->treasure_count + 1));
    if(!new_treasures){
        if(name_copy){ free(name_copy); }
        return NO_MEMORY;
    }

    //add the new treasure
    r->treasures = new_treasures;
    r->treasures[r->treasure_count] = *treasure;
    r->treasures[r->treasure_count].name = name_copy;

    //increase count after adding treasure
    r->treasure_count += 1;

    return OK;
}

int room_get_treasure_at(const Room *r, int x, int y){
    if(!r){return -1;}

    for(int i = 0; i < r->treasure_count; i++){
        if(r->treasures[i].x == x && r->treasures[i].y == y && r->treasures[i].collected == false){
            return r->treasures[i].id;
        }
    }

    return -1;
}

int room_get_portal_destination(const Room *r, int x, int y){
    if(!r){return -1;}

    for(int i = 0; i < r->portal_count; i++){
        if(r->portals[i].x == x && r->portals[i].y == y){
            return r->portals[i].target_room_id;
        }
    }

    return -1;
}

bool room_is_walkable(const Room *r, int x, int y){
    if(!r){return false;}

    //check bounds
    if(x < 0 || x >= r->width || y < 0 || y >= r->height){
        return false;
    }

    //check if wall
    if(r->floor_grid[y * r->width + x] == false){
        return false;
    }  

    //check if pushable
    for(int i = 0; i < r->pushable_count; i++){
        if(r->pushables[i].x == x && r->pushables[i].y == y){
            return false;
        }
    }
    return true;
}

RoomTileType room_classify_tile(const Room *r, int x, int y, int *out_id){
    if(!r){return ROOM_TILE_INVALID;}

    //check if tile is outside of room
    if(x < 0 || x >= r->width || y < 0 || y >= r->height){
        return ROOM_TILE_INVALID;
    }

    //check for treasure
    for(int i = 0; i < r->treasure_count; i++){
        if(r->treasures[i].x == x && r->treasures[i].y == y && r->treasures[i].collected == false){
            if(out_id){
                *out_id = r->treasures[i].id;
            }
            return ROOM_TILE_TREASURE;
        }
    }

    //check for switch
    // for(int i = 0; i < r->switch_count; i++){
    //     if(r->switches[i].x == x && r->switches[i].y == y){
    //         if(out_id){
    //             *out_id = r->switches[i].id;
    //         }
    //         return ROOM_TILE_SWITCH;
    //     }
    // }

    //check fo portal
    for(int i = 0; i < r->portal_count; i++){
        if(r->portals[i].x == x && r->portals[i].y == y){
            if(out_id){
                *out_id = r->portals[i].target_room_id;
            }
            return ROOM_TILE_PORTAL;
        }
    }

    //check for pushable
    for(int i = 0; i < r->pushable_count; i++){
        if(r->pushables[i].x == x && r->pushables[i].y == y){
            if(out_id){
                *out_id = i;
            }
            return ROOM_TILE_PUSHABLE;
        }
    }

    //check if tile is a wall
    if(room_is_walkable(r, x, y) == false){
        return ROOM_TILE_WALL;
    }

    //if it gets to here it must just be an empty floor tile
    return ROOM_TILE_FLOOR;
}

Status room_render(const Room *r, const Charset *charset, char *buffer, int buffer_width, int buffer_height){
    if(!r || !charset || !buffer){
        return INVALID_ARGUMENT;
    }

    //check buffer dimensions
    if(buffer_width != r->width || buffer_height != r->height){
        return INVALID_ARGUMENT;
    }

    render_floor_layer(r, charset, buffer);
    render_treasure_layer(r, charset, buffer);
    render_switch_layer(r, charset, buffer);
    render_portal_layer(r, charset, buffer);
    render_pushable_layer(r, charset, buffer);

    return OK;
}

Status room_get_start_position(const Room *r, int *x_out, int *y_out){
    if(!r || !x_out || !y_out){
        return INVALID_ARGUMENT;
    }

    //find first walkable tile
    if(r->portal_count == 0){
        for(int yy = 0; yy < r->height; yy++){
            for(int xx = 0; xx < r->width; xx++){
                if(room_is_walkable(r, xx, yy)){
                    *x_out = xx;
                    *y_out = yy;
                    return OK;
                }
            }
        }
        return ROOM_NOT_FOUND;
    }
    
    //get first portal position
    *x_out = r->portals[0].x;
    *y_out = r->portals[0].y;
    return OK;
}

void room_destroy(Room *r){
    if(!r){return;}

    //free name
    if(r->name){
        free(r->name);
    }

    //free floor grid
    if(r->floor_grid){
        free(r->floor_grid);
    }

    //free portals
    if(r->portals){
        for(int i = 0; i < r->portal_count; i++){
            if(r->portals[i].name){
                free(r->portals[i].name);
            }
        }
        free(r->portals);
    }

    //free treasures
    if(r->treasures){
        for(int i = 0; i < r->treasure_count; i++){
            if(r->treasures[i].name){
                free(r->treasures[i].name);
            }
        }
        free(r->treasures);
    }

    //free pushables
    if(r->pushables){
        for(int i = 0; i < r->pushable_count; i++){
            if(r->pushables[i].name) free(r->pushables[i].name);
        }
        free(r->pushables);
    }

    //free neighbors
    if(r->neighbors){
        free(r->neighbors);
    }

    //free switches
    if(r->switches){
        free(r->switches);
    }

    //free room itself
    free(r);
}

// *****************************************************************
//                      A2 Functions
// *****************************************************************

int room_get_id(const Room *r){
    if(!r){return -1;}
    return r->id;
}

Status room_pick_up_treasure(Room *r, int treasure_id, Treasure **treasure_out){
    if(!r || !treasure_out || treasure_id < 0){
        return INVALID_ARGUMENT;
    }

    for(int i = 0; i < r->treasure_count; i++){
        if(r->treasures[i].id == treasure_id){
            if(r->treasures[i].collected == true){
                return INVALID_ARGUMENT;
            }

            r->treasures[i].collected = true;
            *treasure_out = &r->treasures[i];
            return OK;
        }
    }

    return ROOM_NOT_FOUND;
}

void destroy_treasure(Treasure *t){
    if(!t){return;}

    if(t->name){
        free(t->name);
    }

    free(t);
}

bool room_has_pushable_at(const Room *r, int x, int y, int *pushable_idx_out){
    if(!r){return false;}

    for(int i = 0; i < r->pushable_count; i++){
        if(r->pushables[i].x == x && r->pushables[i].y == y){
            if(pushable_idx_out){
                *pushable_idx_out = i;
            }
            return true;
        }
    }

    return false;
}

// helper: map direction to (dx, dy)
static Status direction_to_delta(Direction dir, int *dx, int *dy){
    if(!dx || !dy){
        return INVALID_ARGUMENT;
    }

    switch(dir){
        case DIR_NORTH:
            *dx = 0;
            *dy = -1;
            return OK;
        case DIR_SOUTH:
            *dx = 0;
            *dy = 1;
            return OK;
        case DIR_EAST:
            *dx = 1;
            *dy = 0;
            return OK;
        case DIR_WEST:
            *dx = -1;
            *dy = 0;
            return OK;
        default:
            return INVALID_ARGUMENT;
    }
}

// helper: true when a portal occupies (x, y)
static bool has_portal_at(const Room *r, int x, int y){
    if(!r){
        return false;
    }

    for(int i = 0; i < r->portal_count; i++){
        if(r->portals[i].x == x && r->portals[i].y == y){
            return true;
        }
    }

    return false;
}

// helper: find switch at position and return linked portal id, -1 when absent
static int get_switch_portal_id_at(const Room *r, int x, int y){
    if(!r){
        return -1;
    }

    for(int i = 0; i < r->switch_count; i++){
        if(r->switches[i].x == x && r->switches[i].y == y){
            return r->switches[i].portal_id;
        }
    }

    return -1;
}

// helper: set gate state for a portal id if present
static void set_portal_gate_state(Room *r, int portal_id, bool gated){
    if(!r || portal_id < 0){
        return;
    }

    for(int i = 0; i < r->portal_count; i++){
        if(r->portals[i].id == portal_id){
            r->portals[i].gated = gated;
            return;
        }
    }
}

Status room_try_push(Room *r, int pushable_idx, Direction dir){
    if(!r || pushable_idx < 0 || pushable_idx >= r->pushable_count || dir < DIR_NORTH || dir > DIR_WEST){
        return INVALID_ARGUMENT;
    }

    Pushable *p = &r->pushables[pushable_idx];

    int dx = 0;
    int dy = 0;
    if(direction_to_delta(dir, &dx, &dy) != OK){
        return INVALID_ARGUMENT;
    }

    int old_x = p->x;
    int old_y = p->y;
    int new_x = old_x + dx;
    int new_y = old_y + dy;

    if(room_is_walkable(r, new_x, new_y) == false){
        return ROOM_IMPASSABLE;
    }

    if(room_get_treasure_at(r, new_x, new_y) >= 0){
        return ROOM_IMPASSABLE;
    }

    if(has_portal_at(r, new_x, new_y)){
        return ROOM_IMPASSABLE;
    }

    int pressed_portal_id = get_switch_portal_id_at(r, new_x, new_y);
    if(pressed_portal_id >= 0){
        set_portal_gate_state(r, pressed_portal_id, false);
    }

    int released_portal_id = get_switch_portal_id_at(r, old_x, old_y);
    if(released_portal_id >= 0){
        set_portal_gate_state(r, released_portal_id, true);
    }

    p->x = new_x;
    p->y = new_y;

    return OK;
}