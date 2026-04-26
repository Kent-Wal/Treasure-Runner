#include <check.h>
#include <stdlib.h>
#include "room.h"
#include "world_loader.h"
#include "graph.h"
#include "game_engine.h"

//APIs are implemented in game_engine.c but arenty in header files
bool game_engine_all_treasures_collected(const GameEngine *eng);
Status game_engine_get_treasure_stats(const GameEngine *eng, int *total_treasures_out, int *collected_treasures_out);

/* ============================================================
 * Setup and Teardown fixtures
 * ============================================================ */

//declare the new graph
static GameEngine *eng = NULL;

static void setup_eng(void){
    Status status = game_engine_create("../assets/starter.ini", &eng);
    ck_assert_int_eq(status, OK);
    ck_assert_ptr_nonnull(eng);
}

static void teardown_eng(void){
    game_engine_destroy(eng);
    eng = NULL;
}

/* ============================================================
 * Test 1: creating a valid game engine
 * Tests that game_engine_create creates a valid engine
 * ============================================================ */
START_TEST(test_game_engine_creation_valid){
    ck_assert_ptr_nonnull(eng);
}
END_TEST

/* ============================================================
 * Test 2: creating a game engine with null config path
 * Tests that game_engine_create returns INVALID_ARGUMENT for null config path
 * ============================================================ */
START_TEST(test_game_engine_creation_null_config){
    GameEngine *eng2 = NULL;
    ck_assert_int_eq(game_engine_create(NULL, &eng2), INVALID_ARGUMENT);
    ck_assert_ptr_null(eng2);
}
END_TEST

/* ============================================================
 * Test 3: creating a game engine with null engine out
 * Tests that game_engine_create returns INVALID_ARGUMENT for null engine out
 * ============================================================ */
START_TEST(test_game_engine_creation_null_engine_out){
    ck_assert_int_eq(game_engine_create("../assets/starter.ini", NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 4: creating a game engine with null config and engine out
 * Tests that game_engine_create returns INVALID_ARGUMENT for null config and engine out
 * ============================================================ */
START_TEST(test_game_engine_creation_null_all){
    ck_assert_int_eq(game_engine_create(NULL, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 5: creating a game engine with invalid config path
 * Tests that game_engine_create returns WL_ERR_DATAGEN for invalid config path
 * ============================================================ */
// START_TEST(test_game_engine_creation_invalid_config){
//     GameEngine *eng2 = NULL;
//     ck_assert_int_eq(game_engine_create("invalid_config.ini", &eng2), WL_ERR_DATAGEN);
//     ck_assert_ptr_null(eng2);
// }
// END_TEST

/* ============================================================
 * Test 6: getting player from valid engine
 * Tests that game_engine_get_player returns a valid player
 * ============================================================ */
START_TEST(test_game_engine_get_player_valid){
    const Player *p = game_engine_get_player(eng);
    ck_assert_ptr_nonnull(p);
}
END_TEST

/* ============================================================
 * Test 7: getting player from null engine
 * Tests that game_engine_get_player returns NULL for null engine
 * ============================================================ */
START_TEST(test_game_engine_get_player_null){
    const Player *p = game_engine_get_player(NULL);
    ck_assert_ptr_null(p);
}
END_TEST

/* ============================================================
 * Test 8: moving player valid direction
 * Tests that game_engine_move_player moves the player in a valid direction
 * ============================================================ */
START_TEST(test_game_engine_move_player_valid){
    ck_assert_int_eq(game_engine_move_player(eng, DIR_EAST), OK);
}
END_TEST

/* ============================================================
 * Test 9: moving player with null engine
 * Tests that game_engine_move_player returns INVALID_ARGUMENT for null engine
 * ============================================================ */
START_TEST(test_game_engine_move_player_null_eng){
    ck_assert_int_eq(game_engine_move_player(NULL, DIR_SOUTH), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 10: moving player with invalid direction
 * Tests that game_engine_move_player returns INVALID_ARGUMENT for invalid direction
 * ============================================================ */
START_TEST(test_game_engine_move_player_invalid_direction){
    ck_assert_int_eq(game_engine_move_player(eng, 999), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 11: moving player into wall
 * Tests that game_engine_move_player returns ROOM_IMPASSABLE when moving into a wall
 * ============================================================ */
START_TEST(test_game_engine_move_player_into_wall){

    int w = 0;
    int h = 0;
    Status status = game_engine_get_room_dimensions(eng, &w, &h);
    ck_assert_int_eq(status, OK);

    for(int i = 0; i < h; i++){
        status = game_engine_move_player(eng, DIR_NORTH);
        if(status != OK){
            break;
        }
    }
    ck_assert_int_eq(status, ROOM_IMPASSABLE);
}
END_TEST

/* ============================================================
 * Test 12: getting room count valid
 * Tests that game_engine_get_room_count returns valid count
 * ============================================================ */
START_TEST(test_game_engine_room_count_valid){
    int count = 0;
    ck_assert_int_eq(game_engine_get_room_count(eng, &count), OK);
    ck_assert_int_gt(count, 0);
}
END_TEST

/* ============================================================
 * Test 13: getting room count with null engine
 * Tests that game_engine_get_room_count returns INVALID_ARGUMENT for null engine
 * ============================================================ */
START_TEST(test_game_engine_room_count_null_eng){
    int count = 0;
    ck_assert_int_eq(game_engine_get_room_count(NULL, &count), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 14: getting room count with null count out
 * Tests that game_engine_get_room_count returns NULL_POINTER for null count out
 * ============================================================ */
START_TEST(test_game_engine_room_count_null_count_out){
    ck_assert_int_eq(game_engine_get_room_count(eng, NULL), NULL_POINTER);
}
END_TEST

/* ============================================================
 * Test 15: getting room dimensions valid
 * Tests that game_engine_get_room_dimensions returns valid dimensions
 * ============================================================ */
START_TEST(test_game_engine_room_dimensions_valid){
    int width = 0;
    int height = 0;
    ck_assert_int_eq(game_engine_get_room_dimensions(eng, &width, &height), OK);
    ck_assert_int_gt(width, 0);
    ck_assert_int_gt(height, 0);
}
END_TEST

/* ============================================================
 * Test 16: getting room dimensions with null engine
 * Tests that game_engine_get_room_dimensions returns INVALID_ARGUMENT for null engine
 * ============================================================ */
START_TEST(test_game_engine_room_dimensions_null_eng){
    int width = 0;
    int height = 0;
    ck_assert_int_eq(game_engine_get_room_dimensions(NULL, &width, &height), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 17: getting room dimensions with null width out
 * Tests that game_engine_get_room_dimensions returns NULL_POINTER for null width out
 * ============================================================ */
START_TEST(test_game_engine_room_dimensions_null_width_out){
    int height = 0;
    ck_assert_int_eq(game_engine_get_room_dimensions(eng, NULL, &height), NULL_POINTER);
}
END_TEST

/* ============================================================
 * Test 18: getting room dimensions with null height out
 * Tests that game_engine_get_room_dimensions returns NULL_POINTER for null height out
 * ============================================================ */
START_TEST(test_game_engine_room_dimensions_null_height_out){
    int width = 0;
    ck_assert_int_eq(game_engine_get_room_dimensions(eng, &width, NULL), NULL_POINTER);
}
END_TEST

/* ============================================================
 * Test 19: getting room dimensions with all null
 * Tests that game_engine_get_room_dimensions returns INVALID_ARGUMENT for all null
 * ============================================================ */
START_TEST(test_game_engine_room_dimensions_null_all){
    ck_assert_int_eq(game_engine_get_room_dimensions(NULL, NULL, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 20: resetting game engine valid
 * Tests that game_engine_reset resets the game engine successfully
 * ============================================================ */
START_TEST(test_game_engine_reset_valid){
    ck_assert_int_eq(game_engine_reset(eng), OK);
}
END_TEST

/* ============================================================
 * Test 21: resetting game engine with null engine
 * Tests that game_engine_reset returns INVALID_ARGUMENT for null engine
 * ============================================================ */
START_TEST(test_game_engine_reset_null_eng){
    ck_assert_int_eq(game_engine_reset(NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 22: rendering current room valid
 * Tests that game_engine_render_current_room returns a valid string
 * ============================================================ */
START_TEST(test_game_engine_render_current_room_valid){
    char *str = NULL;
    ck_assert_int_eq(game_engine_render_current_room(eng, &str), OK);
    ck_assert_ptr_nonnull(str);
    free(str);
}
END_TEST

/* ============================================================
 * Test 23: rendering current room with null engine
 * Tests that game_engine_render_current_room returns INVALID_ARGUMENT for null engine
 * ============================================================ */
START_TEST(test_game_engine_render_current_room_null_eng){
    char *str = NULL;
    ck_assert_int_eq(game_engine_render_current_room(NULL, &str), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 24: rendering current room with null str out
 * Tests that game_engine_render_current_room returns INVALID_ARGUMENT for null str out
 * ============================================================ */
START_TEST(test_game_engine_render_current_room_null_str_out){
    ck_assert_int_eq(game_engine_render_current_room(eng, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 25: rendering current room with all null
 * Tests that game_engine_render_current_room returns INVALID_ARGUMENT for all null
 * ============================================================ */
START_TEST(test_game_engine_render_current_room_null_all){
    ck_assert_int_eq(game_engine_render_current_room(NULL, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 26: rendering room by ID valid
 * Tests that game_engine_render_room returns a valid string for a room ID
 * ============================================================ */
START_TEST(test_game_engine_render_room_valid){
    char *str = NULL;
    ck_assert_int_eq(game_engine_render_room(eng, eng->initial_room_id, &str), OK);
    ck_assert_ptr_nonnull(str);
    free(str);
}
END_TEST

/* ============================================================
 * Test 27: rendering room by ID with null engine
 * Tests that game_engine_render_room returns INVALID_ARGUMENT for null engine
 * ============================================================ */
START_TEST(test_game_engine_render_room_null_eng){
    char *str = NULL;
    ck_assert_int_eq(game_engine_render_room(NULL, eng->initial_room_id, &str), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 28: rendering room by ID with null str out
 * Tests that game_engine_render_room returns INVALID_ARGUMENT for null str out
 * ============================================================ */
START_TEST(test_game_engine_render_room_null_str_out){
    ck_assert_int_eq(game_engine_render_room(eng, eng->initial_room_id, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 29: rendering room by ID with all null
 * Tests that game_engine_render_room returns INVALID_ARGUMENT for all null
 * ============================================================ */
START_TEST(test_game_engine_render_room_null_all){
    ck_assert_int_eq(game_engine_render_room(NULL, eng->initial_room_id, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 30: rendering room by invalid ID
 * Tests that game_engine_render_room returns GE_NO_SUCH_ROOM for invalid room ID
 * ============================================================ */
START_TEST(test_game_engine_render_room_invalid_room_id){
    char *str = NULL;
    ck_assert_int_eq(game_engine_render_room(eng, 9999, &str), GE_NO_SUCH_ROOM);
}
END_TEST

/* ============================================================
 * Test 31: getting room IDs valid
 * Tests that game_engine_get_room_ids returns valid IDs
 * ============================================================ */
START_TEST(test_game_engine_get_room_ids_valid){
    int *ids = NULL;
    int count = 0;
    ck_assert_int_eq(game_engine_get_room_ids(eng, &ids, &count), OK);
    ck_assert_ptr_nonnull(ids);
    ck_assert_int_gt(count, 0);
    free(ids);
}
END_TEST

/* ============================================================
 * Test 32: getting room IDs with null engine
 * Tests that game_engine_get_room_ids returns INVALID_ARGUMENT for null engine
 * ============================================================ */
START_TEST(test_game_engine_get_room_ids_null_eng){
    int *ids = NULL;
    int count = 0;
    ck_assert_int_eq(game_engine_get_room_ids(NULL, &ids, &count), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 33: getting room IDs with null ids out
 * Tests that game_engine_get_room_ids returns NULL_POINTER for null ids out
 * ============================================================ */
START_TEST(test_game_engine_get_room_ids_null_ids_out){
    int count = 0;
    ck_assert_int_eq(game_engine_get_room_ids(eng, NULL, &count), NULL_POINTER);
}
END_TEST

/* ============================================================
 * Test 34: getting room IDs with null count out
 * Tests that game_engine_get_room_ids returns NULL_POINTER for null count out
 * ============================================================ */
START_TEST(test_game_engine_get_room_ids_null_count_out){
    int *ids = NULL;
    ck_assert_int_eq(game_engine_get_room_ids(eng, &ids, NULL), NULL_POINTER);
}
END_TEST

/* ============================================================
 * Test 35: getting room IDs with all null
 * Tests that game_engine_get_room_ids returns INVALID_ARGUMENT for all null
 * ============================================================ */
START_TEST(test_game_engine_get_room_ids_null_all){
    ck_assert_int_eq(game_engine_get_room_ids(NULL, NULL, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 37: all-collected after forcing completion
 * Tests that if all treasure flags are set true, API reports completion.
 * ============================================================ */
START_TEST(test_game_engine_all_treasures_collected){
    const void * const *payloads = NULL;
    int room_count = 0;
    GraphStatus gstat = graph_get_all_payloads(eng->graph, &payloads, &room_count);
    ck_assert_int_eq(gstat, GRAPH_STATUS_OK);

    int total = 0;
    for(int i = 0; i < room_count; i++){
        Room *room = (Room *)payloads[i];
        for(int j = 0; j < room->treasure_count; j++){
            room->treasures[j].collected = true;
            total += 1;
        }
    }

    int collected = 0;
    int total_out = 0;

    ck_assert_int_gt(total, 0);
    ck_assert_int_eq(game_engine_get_treasure_stats(eng, &total_out, &collected), OK);
    ck_assert_int_eq(total, total_out);
    ck_assert_int_eq(collected, total);
}
END_TEST

//suite creation
Suite *test_game_engine_creation_suite(void){
    //create the new test suite and test case
    Suite *s = suite_create("TestGameEngineCreation");;
    TCase *tc = tcase_create("Core");

    tcase_add_checked_fixture(tc, setup_eng, teardown_eng);

    //add each test to the test case
    tcase_add_test(tc, test_game_engine_get_room_ids_null_all);
    tcase_add_test(tc, test_game_engine_get_room_ids_null_count_out);
    tcase_add_test(tc, test_game_engine_get_room_ids_null_ids_out);
    tcase_add_test(tc, test_game_engine_get_room_ids_null_eng);
    tcase_add_test(tc, test_game_engine_get_room_ids_valid);
    tcase_add_test(tc, test_game_engine_all_treasures_collected);
    tcase_add_test(tc, test_game_engine_render_room_invalid_room_id);
    tcase_add_test(tc, test_game_engine_render_room_null_all);
    tcase_add_test(tc, test_game_engine_render_room_null_str_out);  
    tcase_add_test(tc, test_game_engine_render_room_null_eng);
    tcase_add_test(tc, test_game_engine_render_room_valid);
    tcase_add_test(tc, test_game_engine_render_current_room_null_all);
    tcase_add_test(tc, test_game_engine_render_current_room_null_str_out);
    tcase_add_test(tc, test_game_engine_render_current_room_null_eng);
    tcase_add_test(tc, test_game_engine_render_current_room_valid);
    tcase_add_test(tc, test_game_engine_reset_null_eng);
    tcase_add_test(tc, test_game_engine_reset_valid);
    tcase_add_test(tc, test_game_engine_room_dimensions_null_all);
    tcase_add_test(tc, test_game_engine_room_dimensions_null_height_out);
    tcase_add_test(tc, test_game_engine_room_dimensions_null_width_out);
    tcase_add_test(tc, test_game_engine_room_dimensions_null_eng);
    tcase_add_test(tc, test_game_engine_room_dimensions_valid);
    tcase_add_test(tc, test_game_engine_room_count_null_count_out);
    tcase_add_test(tc, test_game_engine_room_count_null_eng);
    tcase_add_test(tc, test_game_engine_room_count_valid);
    tcase_add_test(tc, test_game_engine_move_player_into_wall);
    tcase_add_test(tc, test_game_engine_move_player_invalid_direction);
    tcase_add_test(tc, test_game_engine_move_player_null_eng);
    tcase_add_test(tc, test_game_engine_move_player_valid);
    tcase_add_test(tc, test_game_engine_get_player_null);
    tcase_add_test(tc, test_game_engine_get_player_valid);
    //tcase_add_test(tc, test_game_engine_creation_invalid_config);
    tcase_add_test(tc, test_game_engine_creation_null_all);
    tcase_add_test(tc, test_game_engine_creation_null_engine_out);
    tcase_add_test(tc, test_game_engine_creation_null_config);
    tcase_add_test(tc, test_game_engine_creation_valid);

    //add the test case to the suite
    suite_add_tcase(s, tc);

    //return the suite
    return s;
}