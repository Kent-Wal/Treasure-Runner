#include "datagen.h"
#include "world_loader.h"
#include "graph.h"
#include "room.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Helper: free internal allocations of a Room struct but do NOT free the struct itself.
static void free_room_contents(Room *r){
    if(!r) return;

    if(r->name){ free(r->name); r->name = NULL; }
    if(r->floor_grid){ free(r->floor_grid); r->floor_grid = NULL; }

    if(r->portals){
        for(int i = 0; i < r->portal_count; i++){
            if(r->portals[i].name){ free(r->portals[i].name); }
        }
        free(r->portals);
        r->portals = NULL;
        r->portal_count = 0;
    }

    if(r->treasures){
        for(int i = 0; i < r->treasure_count; i++){
            if(r->treasures[i].name){ free(r->treasures[i].name); }
        }
        free(r->treasures);
        r->treasures = NULL;
        r->treasure_count = 0;
    }

    if(r->pushables){
        for(int i = 0; i < r->pushable_count; i++){
            if(r->pushables[i].name){ free(r->pushables[i].name); }
        }
        free(r->pushables);
        r->pushables = NULL;
        r->pushable_count = 0;
    }

    if(r->neighbors){ free(r->neighbors); r->neighbors = NULL; r->neighbor_count = 0; }
    if(r->switches){ free(r->switches); r->switches = NULL; r->switch_count = 0; }
}

static void free_rooms_array(Room *rooms, int count){
    if(!rooms) return;
    for(int i = 0; i < count; i++){
        free_room_contents(&rooms[i]);
    }
    free(rooms);
}

/* forward declarations for helper builders */
static int add_portals_to_room(Room *r, const DG_Room *dg);
static int add_treasures_to_room(Room *r, const DG_Room *dg);
static int add_pushables_to_room(Room *r, const DG_Room *dg);
static int add_switches_to_room(Room *r, const DG_Room *dg);
static int add_floor_grid_to_room(Room *r, const DG_Room *dg);
static Graph *build_graph_from_rooms(Room *rooms, int num_rooms);
static int room_compare_id_fn(const void *a, const void *b);

// Build a Room from a DG_Room; returns a newly-allocated Room or NULL on failure.
static Room *build_room_from_dg(const DG_Room *dg){
    if(!dg) return NULL;

    Room *r = room_create(dg->id, "room", dg->width, dg->height);
    if(!r) return NULL;

    if(!add_floor_grid_to_room(r, dg)) { room_destroy(r); return NULL; }

    if(!add_portals_to_room(r, dg)){ room_destroy(r); return NULL; }
    if(!add_treasures_to_room(r, dg)){ room_destroy(r); return NULL; }
    if(!add_pushables_to_room(r, dg)){ room_destroy(r); return NULL; }
    if(!add_switches_to_room(r, dg)){ room_destroy(r); return NULL; }

    return r;
}

static int add_portals_to_room(Room *r, const DG_Room *dg){
    if(dg->portal_count <= 0) return 1;

    Portal *pc = malloc(sizeof(Portal) * dg->portal_count);
    if(!pc) return 0;

    for(int i = 0; i < dg->portal_count; i++){
        pc[i].id = dg->portals[i].id;
        pc[i].x = dg->portals[i].x;
        pc[i].y = dg->portals[i].y;
        pc[i].target_room_id = dg->portals[i].neighbor_id;
        pc[i].name = strdup("portal");
        pc[i].gated = false;
        pc[i].required_switch_id = -1;
    }

    //check for gated portal
    for(int i = 0; i < dg->portal_count; i++){
        for(int j = 0; j < dg->switch_count; j++){
            if(dg->switches[j].portal_id == pc[i].id){
                pc[i].gated = true;
                pc[i].required_switch_id = dg->switches[j].id;
                break;
            }
        }
    }

    if(room_set_portals(r, pc, dg->portal_count) != OK){
        for(int j=0;j<dg->portal_count;j++){
            if(pc[j].name) free(pc[j].name);
        }
        free(pc);
        return 0;
    }
    return 1;
}

static int add_treasures_to_room(Room *r, const DG_Room *dg){
    if(dg->treasure_count <= 0) return 1;

    Treasure *tc = malloc(sizeof(Treasure) * dg->treasure_count);
    if(!tc) return 0;

    for(int i = 0; i < dg->treasure_count; i++){
        tc[i].id = dg->treasures[i].global_id;
        tc[i].x = dg->treasures[i].x;
        tc[i].y = dg->treasures[i].y;
        tc[i].collected = false;
        tc[i].name = strdup("treasure");
        tc[i].starting_room_id = dg->id;
        tc[i].initial_x = dg->treasures[i].x;
        tc[i].initial_y = dg->treasures[i].y;
    }
    if(room_set_treasures(r, tc, dg->treasure_count) != OK){
        for(int j=0;j<dg->treasure_count;j++){
            if(tc[j].name) free(tc[j].name);
        }
        free(tc);
        return 0;
    }
    return 1;
}

static int add_pushables_to_room(Room *r, const DG_Room *dg){
    if(dg->pushable_count <= 0){ 
        r->pushables = NULL; 
        r->pushable_count = 0; 
        return 1; 
    }

    Pushable *pc = malloc(sizeof(Pushable) * dg->pushable_count);
    if(!pc) return 0;

    for(int i = 0; i < dg->pushable_count; i++){
        pc[i].id = dg->pushables[i].id;
        pc[i].x = dg->pushables[i].x;
        pc[i].y = dg->pushables[i].y;
        pc[i].initial_x = dg->pushables[i].x;
        pc[i].initial_y = dg->pushables[i].y;
        pc[i].name = strdup("pushable");
    }
    r->pushables = pc;
    r->pushable_count = dg->pushable_count;
    return 1;
}

static int add_floor_grid_to_room(Room *r, const DG_Room *dg){
    if(!r || !dg) return 0;

    bool *floor_grid_copy = malloc(sizeof(bool) * dg->width * dg->height);
    if(!floor_grid_copy) return 0;

    memcpy(floor_grid_copy, dg->floor_grid, sizeof(bool) * dg->width * dg->height);

    if(room_set_floor_grid(r, floor_grid_copy) != OK){ 
        free(floor_grid_copy); 
        return 0; 
    }
    return 1;
}

static int add_switches_to_room(Room *r, const DG_Room *dg){
    if(dg->switch_count <= 0){ 
        r->switches = NULL; 
        r->switch_count = 0; 
        return 1; 
    }

    Switch *sc = malloc(sizeof(Switch) * dg->switch_count);
    if(!sc) return 0;

    for(int i = 0; i < dg->switch_count; i++){
        sc[i].id = dg->switches[i].id;
        sc[i].x = dg->switches[i].x;
        sc[i].y = dg->switches[i].y;
        sc[i].portal_id = dg->switches[i].portal_id;
    }
    r->switches = sc;
    r->switch_count = dg->switch_count;
    return 1;
}

static Graph *build_graph_from_rooms(Room *rooms, int num_rooms){
    Graph *g = NULL;
    GraphStatus gs = graph_create(room_compare_id_fn, (GraphDestroyFn)room_destroy, &g);
    if(gs != GRAPH_STATUS_OK){ return NULL; }

    // insert rooms
    for(int i = 0; i < num_rooms; i++){
        Room *payload = malloc(sizeof(Room));
        if(!payload){
            graph_destroy(g);
            return NULL;
        }
        *payload = rooms[i];

        gs = graph_insert(g, payload);
        if(gs != GRAPH_STATUS_OK){
            free(payload);
            graph_destroy(g);
            return NULL;
        }

        // Clear the source entry so the caller won't double-free
        // the internal pointers that the graph payload now owns.
        memset(&rooms[i], 0, sizeof(Room));
        rooms[i].id = payload->id; // preserve id for portal-connection loop
    }

    // connect rooms based on portals (use graph payloads since rooms[] was cleared)
    for(int i = 0; i < num_rooms; i++){
        Room key_src = { .id = rooms[i].id };
        const Room *src_payload = graph_get_payload(g, &key_src);
        if(!src_payload) continue;

        for(int j = 0; j < src_payload->portal_count; j++){
            int target_id = src_payload->portals[j].target_room_id;
            if(target_id < 0) continue;

            Room key_tgt = { .id = target_id };
            const Room *tgt_payload = graph_get_payload(g, &key_tgt);
            if(tgt_payload){ (void)graph_connect(g, src_payload, tgt_payload); }
        }
    }

    return g;
}

//comparison function to create graph
static int room_compare_id_fn(const void *a, const void *b){
    if(a == b) return 0;
    if(!a) return -1;
    if(!b) return 1;
    const Room *ra = a;
    const Room *rb = b;
    return ra->id - rb->id;
}

Status loader_load_world(const char *config_file, Graph **graph_out, 
                        Room **first_room_out, int  *num_rooms_out, Charset *charset_out)
{                        
    if(!config_file){return WL_ERR_CONFIG;}

    // treat empty path as config error
    if(config_file[0] == '\0'){
        return WL_ERR_CONFIG;
    }

    //start datagen
    int dg_status = start_datagen(config_file);
    if(dg_status != DG_OK){
        stop_datagen();
        if(dg_status == DG_ERR_CONFIG){
            return WL_ERR_CONFIG;
        }
        return WL_ERR_DATAGEN;
    }

    //create array of rooms
    *num_rooms_out = 0;
    Room *rooms = NULL;
    

    //get the room structs
    while(has_more_rooms()){
        DG_Room dg = get_next_room();

        // Build Room from DG_Room (centralized builder handles allocation and failures)
        Room *r = build_room_from_dg(&dg);
        if(!r){
            free_rooms_array(rooms, *num_rooms_out);
            stop_datagen();
            return NO_MEMORY;
        }

        //add room to array
        //realloc for next room
        Room *tmp = realloc(rooms, sizeof(Room) * (*num_rooms_out + 1));
        if(!tmp){
            // free the Room we just built and any previously accumulated rooms
            free_room_contents(r);
            if(r) free(r);
            free_rooms_array(rooms, *num_rooms_out);
            stop_datagen();
            return NO_MEMORY;
        }
        rooms = tmp;

        rooms[*num_rooms_out] = *r;
        free(r); // free only the temporary struct

        (*num_rooms_out)++;
    }

    // build graph from rooms (graph will own payloads)
    Graph *g = build_graph_from_rooms(rooms, *num_rooms_out);
    if(!g){
        free_rooms_array(rooms, *num_rooms_out);
        stop_datagen();
        return NO_MEMORY;
    }

    //set charset
    const DG_Charset *dg_charset = dg_get_charset();
    if(dg_charset){
        charset_out->wall = dg_charset->wall;
        charset_out->floor = dg_charset->floor;
        charset_out->player = dg_charset->player;
        charset_out->treasure = dg_charset->treasure;
        charset_out->portal = dg_charset->portal;
        charset_out->pushable = dg_charset->pushable;
        charset_out->switch_off = dg_charset->switch_off;
        charset_out->switch_on = dg_charset->switch_on;
    }

    stop_datagen();

    //set graph_out
    *graph_out = g;


    //get the first room in graph (graph owns the payloads)
    if(*num_rooms_out > 0){
        Room key = { .id = rooms[0].id };
        const Room *found = graph_get_payload(g, &key);
        *first_room_out = (Room *)found;
    }
    else{
        *first_room_out = NULL;
    }

    //rooms array is no longer needed so we can free it
    free(rooms);
    return OK;
}