#include <check.h>
#include <stdlib.h>
#include "room.h"

/* ============================================================
 * Setup and Teardown fixtures
 * ============================================================ */

//declare the new graph
static Room *r = NULL;

static void setup_room(void){
    r = room_create(1, "Test Room", 10, 10);
    ck_assert_ptr_nonnull(r);
}

static void teardown_room(void){
    room_destroy(r);
    r = NULL;
}




/* ============================================================
 * Test 1: creating a valid room
 * Tests that room_create creates a valid room
 * ============================================================ */
START_TEST(test_create_room){
    ck_assert_ptr_nonnull(r);
}
END_TEST

/* ============================================================
 * Test 2: creating a room with null name
 * Tests that room_create handles null name correctly
 * ============================================================ */
START_TEST(test_create_room_null_name){
    Room *r2 = room_create(2, NULL, 10, 10);
    ck_assert_ptr_nonnull(r2);
    ck_assert_ptr_null(r2->name);
    room_destroy(r2);
}
END_TEST

/* ============================================================
 * Test 3: creating a room with negative dimensions
 * Tests that room_create returns NULL for negative dimensions
 * ============================================================ */
START_TEST(test_create_room_negative_dimensions){
    Room *r2 = room_create(3, "Negative Room", -5, -5);
    ck_assert_ptr_nonnull(r2);
}
END_TEST

/* ============================================================
 * Test 4: creating a room with zero dimensions
 * Tests that room_create returns NULL for zero dimensions
 * ============================================================ */
START_TEST(test_create_room_zero_dimensions){
    Room *r2 = room_create(4, "Zero Room", 0, 0);
    ck_assert_ptr_nonnull(r2);
}
END_TEST

/* ============================================================
 * Test 5: getting width of a valid room
 * Tests that room_get_width returns correct width
 * ============================================================ */
START_TEST(test_get_width_valid){
    ck_assert_int_eq(room_get_width(r), 10);
}
END_TEST

/* ============================================================
 * Test 6: getting width of a null room
 * Tests that room_get_width returns 0 for null room
 * ============================================================ */
START_TEST(test_get_width_null){
    ck_assert_int_eq(room_get_width(NULL), 0);
}
END_TEST

/* ============================================================
 * Test 7: getting height of a valid room
 * Tests that room_get_height returns correct height
 * ============================================================ */
START_TEST(test_get_height_valid){
    ck_assert_int_eq(room_get_height(r), 10);
}
END_TEST

/* ============================================================
 * Test 8: getting height of a null room
 * Tests that `room_get_height` returns 0 when room is NULL
 * ============================================================ */
START_TEST(test_get_height_null){
    ck_assert_int_eq(room_get_height(NULL), 0);
}
END_TEST

/* ============================================================
 * Test 9: setting a valid floor grid
 * Tests that `room_set_floor_grid` accepts a valid floor grid
 * ============================================================ */
START_TEST(test_set_floor_valid){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = true;
    }
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);
}
END_TEST

/* ============================================================
 * Test 10: setting floor grid to NULL
 * Tests that `room_set_floor_grid` accepts NULL to indicate implicit walls
 * ============================================================ */
START_TEST(test_set_floor_null_floor){
    ck_assert_int_eq(room_set_floor_grid(r, NULL), OK);
}
END_TEST

/* ============================================================
 * Test 11: set floor with NULL room
 * Tests that `room_set_floor_grid` returns INVALID_ARGUMENT for NULL room
 * ============================================================ */
START_TEST(test_set_floor_null_room){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = true;
    }
    ck_assert_int_eq(room_set_floor_grid(NULL, floor_grid), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 12: set floor with all NULL
 * Tests that `room_set_floor_grid` returns INVALID_ARGUMENT when both args NULL
 * ============================================================ */
START_TEST(test_set_floor_all_null){
    ck_assert_int_eq(room_set_floor_grid(NULL, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 14: replace existing floor grid
 * Tests replacing an existing floor grid with a new one
 * ============================================================ */
START_TEST(test_set_floor_existing_floor){
    bool *floor_grid1 = malloc(10 * 10 * sizeof(bool));
    bool *floor_grid2 = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid1[i] = true;
        floor_grid2[i] = false;
    }
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid1), OK);
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid2), OK);
}
END_TEST

/* ============================================================
 * Test 15: setting portals valid
 * Tests that `room_set_portals` accepts a valid portals array
 * ============================================================ */
START_TEST(test_set_portals_valid){
    Portal *portals = malloc(2 * sizeof(Portal));

    portals[0].id = 1;
    portals[0].name = strdup("Portal 1");
    portals[0].x = 1;
    portals[0].y = 1;
    portals[0].target_room_id = 2;

    portals[1].id = 2;
    portals[1].name = strdup("Portal 2");
    portals[1].x = 2;
    portals[1].y = 2;
    portals[1].target_room_id = 3;
    
    ck_assert_int_eq(room_set_portals(r, portals, 2), OK);
    ck_assert_int_eq(r->portal_count, 2);
    for(int i = 0; i < 2; i++){
        ck_assert_int_eq(r->portals[i].id, portals[i].id);
        ck_assert_str_eq(r->portals[i].name, portals[i].name);
        ck_assert_int_eq(r->portals[i].x, portals[i].x);
        ck_assert_int_eq(r->portals[i].y, portals[i].y);
        ck_assert_int_eq(r->portals[i].target_room_id, portals[i].target_room_id);
    }
}
END_TEST

/* ============================================================
 * Test 16: set portals with NULL room
 * Tests that `room_set_portals` returns INVALID_ARGUMENT for NULL room
 * ============================================================ */
START_TEST(test_set_portals_null_room){
    Portal *portals = malloc(2 * sizeof(Portal));

    portals[0].id = 1;
    portals[0].name = strdup("Portal 1");
    portals[0].x = 1;
    portals[0].y = 1;
    portals[0].target_room_id = 2;

    portals[1].id = 2;
    portals[1].name = strdup("Portal 2");
    portals[1].x = 2;
    portals[1].y = 2;
    portals[1].target_room_id = 3;

    ck_assert_int_eq(room_set_portals(NULL, portals, 2), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 17: set portals with NULL portals
 * Tests that `room_set_portals` rejects NULL portals with positive count
 * ============================================================ */
START_TEST(test_set_portals_null_portals){
    ck_assert_int_eq(room_set_portals(r, NULL, 2), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 18: set portals with null arguments
 * Tests that `room_set_portals` returns INVALID_ARGUMENT for inconsistent args
 * ============================================================ */
START_TEST(test_set_portals_null_arguments){
    ck_assert_int_eq(room_set_portals(NULL, NULL, 0), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 19: set portals invalid count
 * Tests that `room_set_portals` rejects negative counts
 * ============================================================ */
START_TEST(test_set_portals_invalid_count){
    Portal *portals = malloc(2 * sizeof(Portal));

    portals[0].id = 1;
    portals[0].name = strdup("Portal 1");
    portals[0].x = 1;
    portals[0].y = 1;
    portals[0].target_room_id = 2;

    portals[1].id = 2;
    portals[1].name = strdup("Portal 2");
    portals[1].x = 2;
    portals[1].y = 2;
    portals[1].target_room_id = 3;

    ck_assert_int_eq(room_set_portals(r, portals, -1), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 20: replace old portals
 * Tests that setting new portals frees and replaces old portals
 * ============================================================ */
START_TEST(test_set_portals_old_portals){
    Portal *portals1 = malloc(1 * sizeof(Portal));
    Portal *portals2 = malloc(2 * sizeof(Portal));

    portals1[0].id = 1;
    portals1[0].name = strdup("Portal 1");
    portals1[0].x = 1;
    portals1[0].y = 1;
    portals1[0].target_room_id = 2;

    portals2[0].id = 2;
    portals2[0].name = strdup("Portal 2");
    portals2[0].x = 2;
    portals2[0].y = 2;
    portals2[0].target_room_id = 3;

    portals2[1].id = 3;
    portals2[1].name = strdup("Portal 3");
    portals2[1].x = 3;
    portals2[1].y = 3;
    portals2[1].target_room_id = 4;

    ck_assert_int_eq(room_set_portals(r, portals1, 1), OK);
    ck_assert_int_eq(room_set_portals(r, portals2, 2), OK);
    ck_assert_int_eq(r->portal_count, 2);
    for(int i = 0; i < 2; i++){
        ck_assert_int_eq(r->portals[i].id, portals2[i].id);
        ck_assert_str_eq(r->portals[i].name, portals2[i].name);
        ck_assert_int_eq(r->portals[i].x, portals2[i].x);
        ck_assert_int_eq(r->portals[i].y, portals2[i].y);
        ck_assert_int_eq(r->portals[i].target_room_id, portals2[i].target_room_id);
    }
}
END_TEST

/* ============================================================
 * Test 21: setting treasures valid
 * Tests that `room_set_treasures` accepts a valid treasures array
 * ============================================================ */
START_TEST(test_set_trasures_valid){
    Treasure *treasures = malloc(2 * sizeof(Treasure));

    treasures[0].id = 1;
    treasures[0].name = strdup("Treasure 1");
    treasures[0].x = 1;
    treasures[0].y = 1;
    treasures[0].collected = false;

    treasures[1].id = 2;
    treasures[1].name = strdup("Treasure 2");
    treasures[1].x = 2;
    treasures[1].y = 2;
    treasures[1].collected = false;

    ck_assert_int_eq(room_set_treasures(r, treasures, 2), OK);
    ck_assert_int_eq(r->treasure_count, 2);
    for(int i = 0; i < 2; i++){
        ck_assert_int_eq(r->treasures[i].id, treasures[i].id);
        ck_assert_str_eq(r->treasures[i].name, treasures[i].name);
        ck_assert_int_eq(r->treasures[i].x, treasures[i].x);
        ck_assert_int_eq(r->treasures[i].y, treasures[i].y);
        ck_assert_int_eq(r->treasures[i].collected, treasures[i].collected);
    }
}
END_TEST

/* ============================================================
 * Test 22: set treasures with NULL room
 * Tests that `room_set_treasures` returns INVALID_ARGUMENT for NULL room
 * ============================================================ */
START_TEST(test_set_treasures_null_room){
    Treasure *treasures = malloc(2 * sizeof(Treasure));

    treasures[0].id = 1;
    treasures[0].name = strdup("Treasure 1");
    treasures[0].x = 1;
    treasures[0].y = 1;
    treasures[0].collected = false;

    treasures[1].id = 2;
    treasures[1].name = strdup("Treasure 2");
    treasures[1].x = 2;
    treasures[1].y = 2;
    treasures[1].collected = false;

    ck_assert_int_eq(room_set_treasures(NULL, treasures, 2), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 23: set treasures with NULL treasures
 * Tests that `room_set_treasures` rejects NULL treasures with positive count
 * ============================================================ */
START_TEST(test_set_treasures_null_treasures){
    ck_assert_int_eq(room_set_treasures(r, NULL, 2), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 24: set treasures all null
 * Tests that `room_set_treasures` returns INVALID_ARGUMENT for NULL args
 * ============================================================ */
START_TEST(test_set_treasures_all_null){
    ck_assert_int_eq(room_set_treasures(NULL, NULL, 0), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 25: set treasures invalid count
 * Tests that `room_set_treasures` rejects negative counts
 * ============================================================ */
START_TEST(test_set_treasures_invalid_count){
    Treasure *treasures = malloc(2 * sizeof(Treasure));

    treasures[0].id = 1;
    treasures[0].name = strdup("Treasure 1");
    treasures[0].x = 1;
    treasures[0].y = 1;
    treasures[0].collected = false;

    treasures[1].id = 2;
    treasures[1].name = strdup("Treasure 2");
    treasures[1].x = 2;
    treasures[1].y = 2;
    treasures[1].collected = false;

    ck_assert_int_eq(room_set_treasures(r, treasures, -1), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 26: replace old treasures
 * Tests that setting new treasures frees and replaces old treasures
 * ============================================================ */
START_TEST(test_set_treasures_old_treasures){
    Treasure *treasures1 = malloc(1 * sizeof(Treasure));
    Treasure *treasures2 = malloc(2 * sizeof(Treasure));

    treasures1[0].id = 1;
    treasures1[0].name = strdup("Treasure 1");
    treasures1[0].x = 1;
    treasures1[0].y = 1;
    treasures1[0].collected = false;

    treasures2[0].id = 2;
    treasures2[0].name = strdup("Treasure 2");
    treasures2[0].x = 2;
    treasures2[0].y = 2;
    treasures2[0].collected = false;

    treasures2[1].id = 3;
    treasures2[1].name = strdup("Treasure 3");
    treasures2[1].x = 3;
    treasures2[1].y = 3;
    treasures2[1].collected = false;

    ck_assert_int_eq(room_set_treasures(r, treasures1, 1), OK);
    ck_assert_int_eq(room_set_treasures(r, treasures2, 2), OK);
    ck_assert_int_eq(r->treasure_count, 2);
    for(int i = 0; i < 2; i++){
        ck_assert_int_eq(r->treasures[i].id, treasures2[i].id);
        ck_assert_str_eq(r->treasures[i].name, treasures2[i].name);
        ck_assert_int_eq(r->treasures[i].x, treasures2[i].x);
        ck_assert_int_eq(r->treasures[i].y, treasures2[i].y);
        ck_assert_int_eq(r->treasures[i].collected, treasures2[i].collected);
    }
}
END_TEST

/* ============================================================
 * Test 27: place treasure valid
 * Tests that `room_place_treasure` adds a treasure to the room
 * ============================================================ */
START_TEST(test_place_treasure_valid){
    Treasure t;
    t.id = 1;
    t.name = strdup("Treasure 1");
    t.x = 1;
    t.y = 1;
    t.collected = false;

    ck_assert_int_eq(room_place_treasure(r, &t), OK);
}
END_TEST

/* ============================================================
 * Test 28: place treasure null room
 * Tests that `room_place_treasure` returns INVALID_ARGUMENT for NULL room
 * ============================================================ */
START_TEST(test_place_treasure_null_room){
    Treasure t;
    t.id = 1;
    t.name = strdup("Treasure 1");
    t.x = 1;
    t.y = 1;
    t.collected = false;

    ck_assert_int_eq(room_place_treasure(NULL, &t), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 29: place treasure null treasure
 * Tests that `room_place_treasure` returns INVALID_ARGUMENT for NULL treasure
 * ============================================================ */
START_TEST(test_place_treasure_null_treasure){
    ck_assert_int_eq(room_place_treasure(r, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 30: place treasure all null
 * Tests that `room_place_treasure` returns INVALID_ARGUMENT when both args NULL
 * ============================================================ */
START_TEST(test_place_treasure_all_null){
    ck_assert_int_eq(room_place_treasure(NULL, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 31: place treasure after set treasures
 * Tests placing a treasure after `room_set_treasures` has initialized the array
 * ============================================================ */
START_TEST(test_place_treasure_after_set_treasure){
    Treasure *treasures = malloc(1 * sizeof(Treasure));

    treasures[0].id = 1;
    treasures[0].name = strdup("Treasure 1");
    treasures[0].x = 1;
    treasures[0].y = 1;
    treasures[0].collected = false;

    ck_assert_int_eq(room_set_treasures(r, treasures, 1), OK);

    Treasure t2;
    t2.id = 2;
    t2.name = strdup("Treasure 2");
    t2.x = 2;
    t2.y = 2;
    t2.collected = false;

    ck_assert_int_eq(room_place_treasure(r, &t2), OK);
    ck_assert_int_eq(r->treasure_count, 2);
}
END_TEST

/* ============================================================
 * Test 32: set treasures after place treasure
 * Tests setting treasures after placing individual treasures
 * ============================================================ */
START_TEST(test_set_treasure_after_place_treasure){
    Treasure t;
    t.id = 1;
    t.name = strdup("Treasure 1");
    t.x = 1;
    t.y = 1;
    t.collected = false;

    ck_assert_int_eq(room_place_treasure(r, &t), OK);

    Treasure *treasures = malloc(2 * sizeof(Treasure));

    treasures[0].id = 2;
    treasures[0].name = strdup("Treasure 2");
    treasures[0].x = 2;
    treasures[0].y = 2;
    treasures[0].collected = false;

    treasures[1].id = 3;
    treasures[1].name = strdup("Treasure 3");
    treasures[1].x = 3;
    treasures[1].y = 3;
    treasures[1].collected = false;

    ck_assert_int_eq(room_set_treasures(r, treasures, 2), OK);
    ck_assert_int_eq(r->treasure_count, 2);
}
END_TEST

/* ============================================================
 * Test 33: get treasure valid
 * Tests that `room_get_treasure_at` returns the treasure id when present
 * ============================================================ */
START_TEST(test_get_treasure_valid){
    Treasure t;
    t.id = 1;
    t.name = strdup("Treasure 1");
    t.x = 1;
    t.y = 1;
    t.collected = false;

    ck_assert_int_eq(room_place_treasure(r, &t), OK);
    ck_assert_int_eq(room_get_treasure_at(r, 1, 1), 1);
}
END_TEST

/* ============================================================
 * Test 34: get treasure not found
 * Tests that `room_get_treasure_at` returns -1 when no treasure at position
 * ============================================================ */
START_TEST(test_get_treasure_not_found){
    ck_assert_int_eq(room_get_treasure_at(r, 5, 5), -1);
}
END_TEST

/* ============================================================
 * Test 35: get treasure null room
 * Tests that `room_get_treasure_at` returns -1 for NULL room
 * ============================================================ */
START_TEST(test_get_treasure_null_room){
    ck_assert_int_eq(room_get_treasure_at(NULL, 1, 1), -1);
}
END_TEST

/* ============================================================
 * Test 36: get treasure wrong position
 * Tests that `room_get_treasure_at` returns -1 for wrong coordinates
 * ============================================================ */
START_TEST(test_get_treasure_wrong_position){
    Treasure t;
    t.id = 1;
    t.name = strdup("Treasure 1");
    t.x = 1;
    t.y = 1;
    t.collected = false;

    ck_assert_int_eq(room_place_treasure(r, &t), OK);
    ck_assert_int_eq(room_get_treasure_at(r, 2, 2), -1);
}
END_TEST

/* ============================================================
 * Test 37: get portal valid
 * Tests that `room_get_portal_destination` returns correct destination
 * ============================================================ */
START_TEST(test_get_portal_valid){
    Portal *p = malloc(sizeof(Portal));
    p->id = 1;
    p->name = strdup("Portal 1");
    p->x = 1;
    p->y = 1;
    p->target_room_id = 2;

    ck_assert_int_eq(room_set_portals(r, p, 1), OK);
    ck_assert_int_eq(room_get_portal_destination(r, 1, 1), 2);
}
END_TEST

/* ============================================================
 * Test 38: get portal not found
 * Tests that `room_get_portal_destination` returns -1 when no portal
 * ============================================================ */
START_TEST(test_get_portal_not_found){
    ck_assert_int_eq(room_get_portal_destination(r, 5, 5), -1);
}
END_TEST

/* ============================================================
 * Test 39: get portal null room
 * Tests that `room_get_portal_destination` returns -1 for NULL room
 * ============================================================ */
START_TEST(test_get_portal_null_room){
    ck_assert_int_eq(room_get_portal_destination(NULL, 1, 1), -1);
}
END_TEST

/* ============================================================
 * Test 40: get portal wrong position
 * Tests that `room_get_portal_destination` returns -1 for wrong coords
 * ============================================================ */
START_TEST(test_get_portal_wrong_position){
    Portal *p = malloc(sizeof(Portal));
    p->id = 1;
    p->name = strdup("Portal 1");
    p->x = 1;
    p->y = 1;
    p->target_room_id = 2;

    ck_assert_int_eq(room_set_portals(r, p, 1), OK);
    ck_assert_int_eq(room_get_portal_destination(r, 2, 2), -1);
}
END_TEST

/* ============================================================
 * Test 41: walkable valid
 * Tests that `room_is_walkable` reports true for walkable tiles
 * ============================================================ */
START_TEST(test_walkable_valid){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = true;
    }
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);
    ck_assert_int_eq(room_is_walkable(r, 5, 5), true);
}
END_TEST

/* ============================================================
 * Test 42: walkable wall
 * Tests that `room_is_walkable` reports false for walls
 * ============================================================ */
START_TEST(test_walkable_wall){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = true;
    }
    //set a wall
    floor_grid[5 * 10 + 5] = false;

    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);
    ck_assert_int_eq(room_is_walkable(r, 5, 5), false);
}
END_TEST

/* ============================================================
 * Test 43: walkable invalid position
 * Tests that `room_is_walkable` returns false for out-of-bounds coords
 * ============================================================ */
START_TEST(test_walkable_invalid_position){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = true;
    }
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);
    ck_assert_int_eq(room_is_walkable(r, -1, 5), false);
    ck_assert_int_eq(room_is_walkable(r, 5, -1), false);
    ck_assert_int_eq(room_is_walkable(r, 10, 5), false);
    ck_assert_int_eq(room_is_walkable(r, 5, 10), false);
}
END_TEST

/* ============================================================
 * Test 44: walkable null room
 * Tests that `room_is_walkable` returns false for NULL room
 * ============================================================ */
START_TEST(test_walkable_null_room){
    ck_assert_int_eq(room_is_walkable(NULL, 5, 5), false);
}
END_TEST

/* ============================================================
 * Test 45: classify valid floor
 * Tests that `room_classify_tile` identifies floor tiles correctly
 * ============================================================ */
START_TEST(test_classify_valid_floor){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = true;
    }
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    int out_id;
    ck_assert_int_eq(room_classify_tile(r, 5, 5, &out_id), ROOM_TILE_FLOOR);
}
END_TEST

/* ============================================================
 * Test 46: classify valid wall
 * Tests that `room_classify_tile` identifies wall tiles correctly
 * ============================================================ */
START_TEST(test_classify_valid_wall){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = true;
    }
    //set a wall
    floor_grid[5 * 10 + 5] = false;

    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    int out_id;
    ck_assert_int_eq(room_classify_tile(r, 5, 5, &out_id), ROOM_TILE_WALL);
}
END_TEST


/* ============================================================
 * Test 47: classify invalid position
 * Tests that `room_classify_tile` returns TILE_INVALID for out-of-bounds
 * ============================================================ */
START_TEST(test_classify_invalid_position){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = true;
    }
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    int out_id;
    ck_assert_int_eq(room_classify_tile(r, -1, 5, &out_id), ROOM_TILE_INVALID);
    ck_assert_int_eq(room_classify_tile(r, 5, -1, &out_id), ROOM_TILE_INVALID);
    ck_assert_int_eq(room_classify_tile(r, 10, 5, &out_id), ROOM_TILE_INVALID);
    ck_assert_int_eq(room_classify_tile(r, 5, 10, &out_id), ROOM_TILE_INVALID);
}
END_TEST


/* ============================================================
 * Test 48: classify treasure valid
 * Tests that `room_classify_tile` identifies treasures
 * ============================================================ */
START_TEST(test_classify_treasure_valid){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = true;
    }
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    Treasure t;
    t.id = 1;
    t.name = strdup("Treasure 1");
    t.x = 5;
    t.y = 5;
    t.collected = false;

    ck_assert_int_eq(room_place_treasure(r, &t), OK);

    int out_id;
    ck_assert_int_eq(room_classify_tile(r, 5, 5, &out_id), ROOM_TILE_TREASURE);
    ck_assert_int_eq(out_id, 1);
}
END_TEST


/* ============================================================
 * Test 49: classify portal valid
 * Tests that `room_classify_tile` identifies portals
 * ============================================================ */
START_TEST(test_classify_portal_valid){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = true;
    }
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    Portal *p = malloc(sizeof(Portal));
    p->id = 1;
    p->name = strdup("Portal 1");
    p->x = 5;
    p->y = 5;
    p->target_room_id = 2;

    ck_assert_int_eq(room_set_portals(r, p, 1), OK);

    int out_id;
    ck_assert_int_eq(room_classify_tile(r, 5, 5, &out_id), ROOM_TILE_PORTAL);
    ck_assert_int_eq(out_id, 2);
}
END_TEST


/* ============================================================
 * Test 50: classify null room
 * Tests that `room_classify_tile` returns TILE_INVALID for NULL room
 * ============================================================ */
START_TEST(test_classify_null_room){
    int out_id;
    ck_assert_int_eq(room_classify_tile(NULL, 5, 5, &out_id), ROOM_TILE_INVALID);
}
END_TEST


/* ============================================================
 * Test 51: classify null out id
 * Tests that `room_classify_tile` handles NULL out_id gracefully
 * ============================================================ */
START_TEST(test_classify_null_out_id){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = true;
    }
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    Treasure t;
    t.id = 1;
    t.name = strdup("Treasure 1");
    t.x = 5;
    t.y = 5;
    t.collected = false;

    ck_assert_int_eq(room_place_treasure(r, &t), OK);

    ck_assert_int_eq(room_classify_tile(r, 5, 5, NULL), ROOM_TILE_TREASURE);
}
END_TEST


/* ============================================================
 * Test 52: render valid
 * Tests that `room_render` produces the expected ASCII output
 * ============================================================ */
START_TEST(test_render_valid){
    char *buffer = malloc(10 * 10 * sizeof(char));

    Charset c;
    c.floor = '.';
    c.wall = '#';
    c.portal = 'X';
    c.treasure = '$';
    c.player = '@';
    c.pushable = 'O';
    c.switch_off = '-';
    c.switch_on = '+';

    ck_assert_int_eq(room_render(r, &c, buffer, 10, 10), OK);
    free(buffer);
}
END_TEST


/* ============================================================
 * Test 53: render pushable on switch as switch_on
 * Tests that `room_render` displays switch_on instead of pushable when overlapped
 * ============================================================ */
START_TEST(test_render_pushable_on_switch_shows_switch_on){
    char *buffer = malloc(10 * 10 * sizeof(char));
    ck_assert_ptr_nonnull(buffer);

    bool *floor_grid = malloc(10 * 10 * sizeof(bool));
    ck_assert_ptr_nonnull(floor_grid);
    for(int i = 0; i < 100; i++){
        floor_grid[i] = true;
    }
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    r->switches = malloc(sizeof(Switch));
    ck_assert_ptr_nonnull(r->switches);
    r->switch_count = 1;
    r->switches[0].id = 1;
    r->switches[0].x = 4;
    r->switches[0].y = 4;
    r->switches[0].portal_id = -1;

    r->pushables = malloc(sizeof(Pushable));
    ck_assert_ptr_nonnull(r->pushables);
    r->pushable_count = 1;
    r->pushables[0].id = 1;
    r->pushables[0].name = strdup("Pushable");
    r->pushables[0].x = 4;
    r->pushables[0].y = 4;
    r->pushables[0].initial_x = 4;
    r->pushables[0].initial_y = 4;

    Charset c;
    c.floor = '.';
    c.wall = '#';
    c.portal = 'X';
    c.treasure = '$';
    c.player = '@';
    c.pushable = 'O';
    c.switch_off = '-';
    c.switch_on = '+';

    ck_assert_int_eq(room_render(r, &c, buffer, 10, 10), OK);
    ck_assert_int_eq(buffer[4 * 10 + 4], '+');

    free(buffer);
}
END_TEST


/* ============================================================
 * Test 53: render null room
 * Tests that `room_render` returns INVALID_ARGUMENT for NULL room
 * ============================================================ */
START_TEST(test_render_null_room){
    char *buffer = malloc(10 * 10 * sizeof(char));

    Charset c;
    c.floor = '.';
    c.wall = '#';
    c.portal = 'X';
    c.treasure = '$';
    c.player = '@';
    c.pushable = 'O';
    c.switch_off = '-';
    c.switch_on = '+';

    ck_assert_int_eq(room_render(NULL, &c, buffer, 10, 10), INVALID_ARGUMENT);
    free(buffer);
}
END_TEST


/* ============================================================
 * Test 54: render null charset
 * Tests that `room_render` returns INVALID_ARGUMENT for NULL charset
 * ============================================================ */
START_TEST(test_render_null_charset){
    char *buffer = malloc(10 * 10 * sizeof(char));

    ck_assert_int_eq(room_render(r, NULL, buffer, 10, 10), INVALID_ARGUMENT);
    free(buffer);
}
END_TEST


/* ============================================================
 * Test 55: render null buffer
 * Tests that `room_render` returns INVALID_ARGUMENT for NULL buffer
 * ============================================================ */
START_TEST(test_render_null_buffer){
    Charset c;
    c.floor = '.';
    c.wall = '#';
    c.portal = 'X';
    c.treasure = '$';
    c.player = '@';
    c.pushable = 'O';
    c.switch_off = '-';
    c.switch_on = '+';

    ck_assert_int_eq(room_render(r, &c, NULL, 10, 10), INVALID_ARGUMENT);
}
END_TEST


/* ============================================================
 * Test 56: render invalid buffer size
 * Tests that `room_render` returns INVALID_ARGUMENT for too-small buffer
 * ============================================================ */
START_TEST(test_render_invalid_buffer_size){
    char *buffer = malloc(5 * 5 * sizeof(char));

    Charset c;
    c.floor = '.';
    c.wall = '#';
    c.portal = 'X';
    c.treasure = '$';
    c.player = '@';
    c.pushable = 'O';
    c.switch_off = '-';
    c.switch_on = '+';

    ck_assert_int_eq(room_render(r, &c, buffer, 5, 5), INVALID_ARGUMENT);
    free(buffer);
}
END_TEST


/* ============================================================
 * Test 57: start position valid
 * Tests that `room_get_start_position` returns valid start coords
 * ============================================================ */
START_TEST(test_start_position_valid){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = false;
    }
    //set a walkable tile
    floor_grid[2 * 10 + 3] = true;

    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    int x, y;
    ck_assert_int_eq(room_get_start_position(r, &x, &y), OK);
    ck_assert_int_eq(x, 3);
    ck_assert_int_eq(y, 2);

}
END_TEST


/* ============================================================
 * Test 58: start position valid portal
 * Tests start position when it's a portal
 * ============================================================ */
START_TEST(test_start_position_valid_portal){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = false;
    }
    //set a walkable tile
    floor_grid[2 * 10 + 3] = true;

    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    Portal *p = malloc(sizeof(Portal));
    p->id = 1;
    p->name = strdup("Portal 1");
    p->x = 5;
    p->y = 5;
    p->target_room_id = 2;

    ck_assert_int_eq(room_set_portals(r, p, 1), OK);

    int x, y;
    ck_assert_int_eq(room_get_start_position(r, &x, &y), OK);
    ck_assert_int_eq(x, 5);
    ck_assert_int_eq(y, 5);
}
END_TEST


/* ============================================================
 * Test 59: start position multiple portal
 * Tests behavior with multiple portals defined
 * ============================================================ */
START_TEST(test_start_position_multiple_portal){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = false;
    }
    //set a walkable tile
    floor_grid[2 * 10 + 3] = true;

    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    Portal *portals = malloc(2 * sizeof(Portal));
    portals[0].id = 1;
    portals[0].name = strdup("Portal 1");
    portals[0].x = 5;
    portals[0].y = 5;
    portals[0].target_room_id = 2;

    portals[1].id = 2;
    portals[1].name = strdup("Portal 2");
    portals[1].x = 1;
    portals[1].y = 1;
    portals[1].target_room_id = 3;

    ck_assert_int_eq(room_set_portals(r, portals, 2), OK);

    int x, y;
    ck_assert_int_eq(room_get_start_position(r, &x, &y), OK);
    ck_assert_int_eq(x, 5);
    ck_assert_int_eq(y, 5);
}
END_TEST


/* ============================================================
 * Test 60: start position null room
 * Tests that `room_get_start_position` returns INVALID_ARGUMENT for NULL room
 * ============================================================ */
START_TEST(test_start_position_null_room){
    int x, y;
    ck_assert_int_eq(room_get_start_position(NULL, &x, &y), INVALID_ARGUMENT);
}
END_TEST


/* ============================================================
 * Test 61: start position null x
 * Tests that `room_get_start_position` returns INVALID_ARGUMENT for NULL x
 * ============================================================ */
START_TEST(test_start_position_null_x){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = false;
    }
    //set a walkable tile
    floor_grid[2 * 10 + 3] = true;

    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    int y;
    ck_assert_int_eq(room_get_start_position(r, NULL, &y), INVALID_ARGUMENT);
}
END_TEST


/* ============================================================
 * Test 62: start position null y
 * Tests that `room_get_start_position` returns INVALID_ARGUMENT for NULL y
 * ============================================================ */
START_TEST(test_start_position_null_y){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = false;
    }
    //set a walkable tile
    floor_grid[2 * 10 + 3] = true;

    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    int x;
    ck_assert_int_eq(room_get_start_position(r, &x, NULL), INVALID_ARGUMENT);
}
END_TEST    


/* ============================================================
 * Test 63: start position all null
 * Tests that `room_get_start_position` returns INVALID_ARGUMENT for all NULLs
 * ============================================================ */
START_TEST(test_start_position_all_null){
    ck_assert_int_eq(room_get_start_position(NULL, NULL, NULL), INVALID_ARGUMENT);
}
END_TEST


/* ============================================================
 * Test 64: start position no start position
 * Tests that `room_get_start_position` returns NO_START_POSITION when none present
 * ============================================================ */
START_TEST(test_start_position_no_start_position){
    bool *floor_grid = malloc(10 * 10 * sizeof(bool));

    for(int i = 0; i < 100; i++){
        floor_grid[i] = false;
    }

    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    int x, y;
    ck_assert_int_eq(room_get_start_position(r, &x, &y), ROOM_NOT_FOUND);
}
END_TEST

START_TEST(test_get_room_id_valid){
    ck_assert_int_eq(room_get_id(r), 1);
}
END_TEST

START_TEST(test_get_room_id_null){
    ck_assert_int_eq(room_get_id(NULL), -1);
}
END_TEST

START_TEST(test_pickup_treasure_valid){
    Treasure t;
    t.id = 1;
    t.name = strdup("Treasure 1");
    t.x = 1;
    t.y = 1;
    t.collected = false;

    Treasure *treasure_out = malloc(1 * sizeof(Treasure));

    ck_assert_int_eq(room_place_treasure(r, &t), OK);
    ck_assert_int_eq(room_pick_up_treasure(r, 1, &treasure_out), OK);
    ck_assert_int_eq(treasure_out->id, t.id);
}
END_TEST

START_TEST(test_pickup_treasure_invalid_args){
    Treasure t;
    t.id = 1;
    t.name = strdup("Treasure 1");
    t.x = 1;
    t.y = 1;
    t.collected = false;

    Treasure *treasure_out = malloc(1 * sizeof(Treasure));

    ck_assert_int_eq(room_place_treasure(r, &t), OK);
    ck_assert_int_eq(room_pick_up_treasure(NULL, 1, &treasure_out), INVALID_ARGUMENT);
    ck_assert_int_eq(room_pick_up_treasure(r, 1, NULL), INVALID_ARGUMENT);
}
END_TEST

START_TEST(test_pickup_treasure_not_found){
    Treasure t;
    t.id = 1;
    t.name = strdup("Treasure 1");
    t.x = 1;
    t.y = 1;
    t.collected = false;

    Treasure *treasure_out = malloc(1 * sizeof(Treasure));

    ck_assert_int_eq(room_place_treasure(r, &t), OK);
    ck_assert_int_eq(room_pick_up_treasure(r, 2, &treasure_out), ROOM_NOT_FOUND);
}
END_TEST

START_TEST(test_has_pushable_valid){
    Pushable p;
    p.id = 1;
    p.name = strdup("Pushable 1");
    p.x = 1;
    p.y = 1;
    p.initial_x = 1;
    p.initial_y = 1;

    int pushable_idx_out;
    r->pushables = malloc(1 * sizeof(Pushable));
    r->pushables[0] = p;
    r->pushable_count = 1;

    ck_assert_int_eq(room_has_pushable_at(r, 1, 1, &pushable_idx_out), true);
    ck_assert_int_eq(pushable_idx_out, 0);
}
END_TEST

START_TEST(test_has_pushable_not_found){
    int pushable_idx_out;
    ck_assert_int_eq(room_has_pushable_at(r, 1, 1, &pushable_idx_out), false);
}
END_TEST

START_TEST(test_has_pushable_null_args){
    int pushable_idx_out;
    ck_assert_int_eq(room_has_pushable_at(NULL, 1, 1, &pushable_idx_out), false);
    ck_assert_int_eq(room_has_pushable_at(r, 1, 1, NULL), false);
    ck_assert_int_eq(room_has_pushable_at(NULL, 1, 1, NULL), false);
}
END_TEST

START_TEST(test_try_push_valid){
    Pushable p;
    p.id = 1;
    p.name = strdup("Pushable 1");
    p.x = 1;
    p.y = 1;
    p.initial_x = 1;
    p.initial_y = 1;

    //set pushables
    r->pushables = malloc(1 * sizeof(Pushable));
    r->pushables[0] = p;
    r->pushable_count = 1;

    ck_assert_int_eq(room_try_push(r, 0, DIR_SOUTH), OK);
    ck_assert_int_eq(r->pushables[0].x, 1);
    ck_assert_int_eq(r->pushables[0].y, 2);
}
END_TEST

START_TEST(test_try_push_invalid_args){
    ck_assert_int_eq(room_try_push(NULL, 0, DIR_SOUTH), INVALID_ARGUMENT);
    ck_assert_int_eq(room_try_push(r, -1, DIR_SOUTH), INVALID_ARGUMENT);
    ck_assert_int_eq(room_try_push(r, 0, -1), INVALID_ARGUMENT);
    ck_assert_int_eq(room_try_push(NULL, 0, 5), INVALID_ARGUMENT);
}
END_TEST

START_TEST(test_try_push_not_pushable){
    Pushable p;
    p.id = 1;
    p.name = strdup("Pushable 1");
    p.x = 1;
    p.y = 1;
    p.initial_x = 1;
    p.initial_y = 1;

    //set pushables
    r->pushables = malloc(1 * sizeof(Pushable));
    r->pushables[0] = p;
    r->pushable_count = 1;

    //place treasire in front of pushable
    Treasure t;
    t.id = 1;
    t.name = strdup("Treasure 1");
    t.x = 1;
    t.y = 2;
    t.collected = false;

    ck_assert_int_eq(room_place_treasure(r, &t), OK);
    ck_assert_int_eq(room_try_push(r, 0, DIR_SOUTH), ROOM_IMPASSABLE);
    ck_assert_int_eq(r->pushables[0].x, 1);
    ck_assert_int_eq(r->pushables[0].y, 1);
}
END_TEST

START_TEST(test_try_push_unlocks_linked_portal_on_switch_press){
    Portal *portals = malloc(sizeof(Portal));
    ck_assert_ptr_nonnull(portals);
    portals[0].id = 42;
    portals[0].name = NULL;
    portals[0].x = 8;
    portals[0].y = 8;
    portals[0].target_room_id = 2;
    portals[0].gated = true;
    portals[0].required_switch_id = 7;
    ck_assert_int_eq(room_set_portals(r, portals, 1), OK);

    r->switches = malloc(sizeof(Switch));
    ck_assert_ptr_nonnull(r->switches);
    r->switch_count = 1;
    r->switches[0].id = 7;
    r->switches[0].x = 2;
    r->switches[0].y = 1;
    r->switches[0].portal_id = 42;

    r->pushables = malloc(sizeof(Pushable));
    ck_assert_ptr_nonnull(r->pushables);
    r->pushable_count = 1;
    r->pushables[0].id = 1;
    r->pushables[0].name = NULL;
    r->pushables[0].x = 1;
    r->pushables[0].y = 1;
    r->pushables[0].initial_x = 1;
    r->pushables[0].initial_y = 1;

    ck_assert_int_eq(room_try_push(r, 0, DIR_EAST), OK);
    ck_assert_int_eq(r->pushables[0].x, 2);
    ck_assert_int_eq(r->pushables[0].y, 1);
    ck_assert_int_eq(r->portals[0].gated, false);
}
END_TEST

START_TEST(test_try_push_relocks_linked_portal_on_switch_release){
    Portal *portals = malloc(sizeof(Portal));
    ck_assert_ptr_nonnull(portals);
    portals[0].id = 42;
    portals[0].name = NULL;
    portals[0].x = 8;
    portals[0].y = 8;
    portals[0].target_room_id = 2;
    portals[0].gated = false;
    portals[0].required_switch_id = 7;
    ck_assert_int_eq(room_set_portals(r, portals, 1), OK);

    r->switches = malloc(sizeof(Switch));
    ck_assert_ptr_nonnull(r->switches);
    r->switch_count = 1;
    r->switches[0].id = 7;
    r->switches[0].x = 2;
    r->switches[0].y = 1;
    r->switches[0].portal_id = 42;

    r->pushables = malloc(sizeof(Pushable));
    ck_assert_ptr_nonnull(r->pushables);
    r->pushable_count = 1;
    r->pushables[0].id = 1;
    r->pushables[0].name = NULL;
    r->pushables[0].x = 2;
    r->pushables[0].y = 1;
    r->pushables[0].initial_x = 2;
    r->pushables[0].initial_y = 1;

    ck_assert_int_eq(room_try_push(r, 0, DIR_EAST), OK);
    ck_assert_int_eq(r->pushables[0].x, 3);
    ck_assert_int_eq(r->pushables[0].y, 1);
    ck_assert_int_eq(r->portals[0].gated, true);
}
END_TEST

//suite creation
Suite *test_room_creation_suite(void){
    //create the new test suite and test case
    Suite *s = suite_create("TestRoomCreation");;
    TCase *tc = tcase_create("Core");

    tcase_add_checked_fixture(tc, setup_room, teardown_room);

    //add each test to the test case
    tcase_add_test(tc, test_create_room);
    tcase_add_test(tc, test_create_room_null_name);
    tcase_add_test(tc, test_create_room_negative_dimensions);
    tcase_add_test(tc, test_create_room_zero_dimensions);
    tcase_add_test(tc, test_get_width_valid);
    tcase_add_test(tc, test_get_width_null);
    tcase_add_test(tc, test_get_height_valid);
    tcase_add_test(tc, test_get_height_null);
    tcase_add_test(tc, test_set_floor_valid);
    tcase_add_test(tc, test_set_floor_null_floor);
    tcase_add_test(tc, test_set_floor_null_room);
    tcase_add_test(tc, test_set_floor_all_null);
    tcase_add_test(tc, test_set_floor_existing_floor);
    tcase_add_test(tc, test_set_portals_valid);
    tcase_add_test(tc, test_set_portals_null_room);
    tcase_add_test(tc, test_set_portals_null_portals);
    tcase_add_test(tc, test_set_portals_null_arguments);
    tcase_add_test(tc, test_set_portals_invalid_count);
    tcase_add_test(tc, test_set_portals_old_portals);
    tcase_add_test(tc, test_set_trasures_valid);
    tcase_add_test(tc, test_set_treasures_null_room);
    tcase_add_test(tc, test_set_treasures_null_treasures);
    tcase_add_test(tc, test_set_treasures_all_null);
    tcase_add_test(tc, test_set_treasures_invalid_count);
    tcase_add_test(tc, test_set_treasures_old_treasures);
    tcase_add_test(tc, test_place_treasure_valid);
    tcase_add_test(tc, test_place_treasure_null_room);
    tcase_add_test(tc, test_place_treasure_null_treasure);
    tcase_add_test(tc, test_place_treasure_all_null);
    tcase_add_test(tc, test_place_treasure_after_set_treasure);
    tcase_add_test(tc, test_set_treasure_after_place_treasure);
    tcase_add_test(tc, test_get_treasure_valid);
    tcase_add_test(tc, test_get_treasure_not_found);
    tcase_add_test(tc, test_get_treasure_null_room);
    tcase_add_test(tc, test_get_treasure_wrong_position);
    tcase_add_test(tc, test_get_portal_valid);
    tcase_add_test(tc, test_get_portal_not_found);
    tcase_add_test(tc, test_get_portal_null_room);
    tcase_add_test(tc, test_get_portal_wrong_position);
    tcase_add_test(tc, test_walkable_valid);
    tcase_add_test(tc, test_walkable_wall);
    tcase_add_test(tc, test_walkable_invalid_position);
    tcase_add_test(tc, test_walkable_null_room);
    tcase_add_test(tc, test_classify_valid_floor);
    tcase_add_test(tc, test_classify_valid_wall);
    tcase_add_test(tc, test_classify_invalid_position);
    tcase_add_test(tc, test_classify_treasure_valid);
    tcase_add_test(tc, test_classify_portal_valid);
    tcase_add_test(tc, test_classify_null_room);
    tcase_add_test(tc, test_classify_null_out_id);
    tcase_add_test(tc, test_render_valid);
    tcase_add_test(tc, test_render_pushable_on_switch_shows_switch_on);
    tcase_add_test(tc, test_render_null_room);
    tcase_add_test(tc, test_render_null_charset);
    tcase_add_test(tc, test_render_null_buffer);
    tcase_add_test(tc, test_render_invalid_buffer_size);
    tcase_add_test(tc, test_start_position_valid);
    tcase_add_test(tc, test_start_position_valid_portal);
    tcase_add_test(tc, test_start_position_multiple_portal);
    tcase_add_test(tc, test_start_position_null_room);
    tcase_add_test(tc, test_start_position_null_x);
    tcase_add_test(tc, test_start_position_null_y);
    tcase_add_test(tc, test_start_position_all_null);
    tcase_add_test(tc, test_start_position_no_start_position);
    tcase_add_test(tc, test_get_room_id_valid);
    tcase_add_test(tc, test_get_room_id_null);
    tcase_add_test(tc, test_pickup_treasure_valid);
    tcase_add_test(tc, test_pickup_treasure_invalid_args);
    tcase_add_test(tc, test_pickup_treasure_not_found);
    tcase_add_test(tc, test_has_pushable_valid);
    tcase_add_test(tc, test_has_pushable_not_found);
    tcase_add_test(tc, test_has_pushable_null_args);
    tcase_add_test(tc, test_try_push_valid);
    tcase_add_test(tc, test_try_push_invalid_args);
    tcase_add_test(tc, test_try_push_not_pushable);
    tcase_add_test(tc, test_try_push_unlocks_linked_portal_on_switch_press);
    tcase_add_test(tc, test_try_push_relocks_linked_portal_on_switch_release);
    
    //add the test case to the suite
    suite_add_tcase(s, tc);

    //return the suite
    return s;
}