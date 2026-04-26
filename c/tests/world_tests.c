#include <check.h>
#include <stdlib.h>
#include "room.h"
#include "world_loader.h"
#include "graph.h"

/* ============================================================
    * Test 1: creating a valid world loader
    * Tests that room_create creates a valid room
 * ============================================================ */
START_TEST(test_world_loader_valid){
    Graph *g = NULL;
    Room *first_room = NULL;
    int num_rooms = 0;
    Charset charset;
    ck_assert_int_eq(loader_load_world("../assets/starter.ini", &g, &first_room, &num_rooms, &charset), OK);
}
END_TEST

/* ============================================================
    * Test 2: charset switch fields loaded
    * Tests that loader_load_world propagates switch charset chars from INI
 * ============================================================ */
START_TEST(test_world_loader_charset_switch_fields){
    Graph *g = NULL;
    Room *first_room = NULL;
    int num_rooms = 0;
    Charset charset;

    ck_assert_int_eq(loader_load_world("../assets/starter.ini", &g, &first_room, &num_rooms, &charset), OK);
    ck_assert_int_eq(charset.switch_off, '-');
    ck_assert_int_eq(charset.switch_on, '+');

    if(g){
        graph_destroy(g);
    }
}
END_TEST

/* ============================================================
    * Test 3: creating a world loader with null config
    * Tests that loader_load_world returns WL_ERR_CONFIG when given a NULL config file path
 * ============================================================ */
START_TEST(test_world_loader_null_config){
    Graph *g = NULL;
    Room *first_room = NULL;
    int num_rooms = 0;
    Charset charset;
    ck_assert_int_eq(loader_load_world(NULL, &g, &first_room, &num_rooms, &charset), WL_ERR_CONFIG);
}
END_TEST

/* ============================================================
    * Test 4: creating a world loader with invalid config
    * Tests that loader_load_world returns WL_ERR_CONFIG when given an invalid config file path
 * ============================================================ */
START_TEST(test_world_loader_invalid_config){
    Graph *g = NULL;
    Room *first_room = NULL;
    int num_rooms = 0;
    Charset charset;
    ck_assert_int_eq(loader_load_world("invalid_config.ini", &g, &first_room, &num_rooms, &charset), WL_ERR_CONFIG);
}
END_TEST

//suite creation
Suite *test_world_creation_suite(void){
    //create the new test suite and test case
    Suite *s = suite_create("TestWorldCreation");;
    TCase *tc = tcase_create("Core");

    //add each test to the test case
    tcase_add_test(tc, test_world_loader_valid);
    tcase_add_test(tc, test_world_loader_charset_switch_fields);
    tcase_add_test(tc, test_world_loader_null_config);
    tcase_add_test(tc, test_world_loader_invalid_config);

    //add the test case to the suite
    suite_add_tcase(s, tc);

    //return the suite
    return s;
}