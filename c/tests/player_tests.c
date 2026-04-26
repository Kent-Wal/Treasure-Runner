#include <check.h>
#include <stdlib.h>
#include "player.h"


/* ============================================================
 * Test 1: creating a valid player
 * Tests that player_create creates a valid player
 * ============================================================ */
START_TEST(test_create_player){
    Player *p = malloc(sizeof(Player));
    ck_assert_ptr_nonnull(p);
    ck_assert_int_eq(player_create(1, 0, 0, &p), OK);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 2: creating a null player
 * Tests that player_create returns INVALID_ARGUMENT when player_out is null
 * ============================================================ */
START_TEST(test_null_player_out){
    ck_assert_int_eq(player_create(1, 0, 0, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 3: getting the room of a valid player
 * Tests that player_get_room returns the correct room ID
 * ============================================================ */
START_TEST(test_player_get_room_valid){
    Player *p = malloc(sizeof(Player));
    player_create(1, 0, 0, &p);
    ck_assert_int_eq(player_get_room(p), 1);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 4: getting the room of a null player
 * Tests that player_get_room returns -1 when player is null
 * ============================================================ */
START_TEST(test_player_get_room_null){
    Player *p = NULL;
    ck_assert_int_eq(player_get_room(p), -1);
}
END_TEST

/* ============================================================
 * Test 5: getting the position of a valid player
 * Tests that player_get_position returns the correct position
 * ============================================================ */
START_TEST(test_player_get_position_valid){
    Player *p = malloc(sizeof(Player));
    ck_assert_int_eq(player_create(1, 5, 10, &p), OK);
    int x, y;
    ck_assert_int_eq(player_get_position(p, &x, &y), OK);
    ck_assert_int_eq(x, 5);
    ck_assert_int_eq(y, 10);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 6: getting the position of a null player
 * Tests that player_get_position returns INVALID_ARGUMENT when player is null  
 * ============================================================ */
START_TEST(test_player_get_position_null_player){
    Player *p = NULL;
    int x, y;
    ck_assert_int_eq(player_get_position(p, &x, &y), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 7: getting the position of a valid player with null x pointer
 * Tests that player_get_position returns INVALID_ARGUMENT when x is null
 * ============================================================ */
START_TEST(test_player_get_position_null_x){
    Player *p = malloc(sizeof(Player));
    ck_assert_int_eq(player_create(1, 5, 10, &p), OK);
    int y;
    ck_assert_int_eq(player_get_position(p, NULL, &y), INVALID_ARGUMENT);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 8: getting the position of a valid player with null y pointer
 * Tests that player_get_position returns INVALID_ARGUMENT when y is null
 * ============================================================ */
START_TEST(test_player_get_position_null_y){
    Player *p = malloc(sizeof(Player));
    ck_assert_int_eq(player_create(1, 5, 10, &p), OK);
    int x;
    ck_assert_int_eq(player_get_position(p, &x, NULL), INVALID_ARGUMENT);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 9: getting the position of a null player with null x and y pointers
 * Tests that player_get_position returns INVALID_ARGUMENT when all pointers are null
 * ============================================================ */
START_TEST(test_player_get_position_all_null){
    Player *p = NULL;
    ck_assert_int_eq(player_get_position(p, NULL, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 10: setting the position of a valid player
 * Tests that player_set_position sets the correct position
 * ============================================================ */
START_TEST(test_player_set_position_valid){
    Player *p = malloc(sizeof(Player));
    ck_assert_int_eq(player_create(1, 0, 0, &p), OK);
    ck_assert_int_eq(player_set_position(p, 7, 14), OK);
    int x, y;
    ck_assert_int_eq(player_get_position(p, &x, &y), OK);
    ck_assert_int_eq(x, 7);
    ck_assert_int_eq(y, 14);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 11: setting the position of a null player
 * Tests that player_set_position returns INVALID_ARGUMENT when player is null
 * ============================================================ */
START_TEST(test_player_set_position_null){
    Player *p = NULL;
    ck_assert_int_eq(player_set_position(p, 7, 14), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 12: moving to a different room with a valid player
 * Tests that player_move_to_room sets the correct room ID
 * ============================================================ */
START_TEST(test_player_move_room_valid){
    Player *p = malloc(sizeof(Player));
    ck_assert_int_eq(player_create(1, 0, 0, &p), OK);
    ck_assert_int_eq(player_move_to_room(p, 2), OK);
    ck_assert_int_eq(player_get_room(p), 2);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 13: moving to a different room with a null player
 * Tests that player_move_to_room returns INVALID_ARGUMENT when player is null
 * ============================================================ */
START_TEST(test_player_move_room_null){
    Player *p = NULL;
    ck_assert_int_eq(player_move_to_room(p, 2), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 14: resetting a valid player
 * Tests that player_reset_to_start resets the player to the correct room and position
 * ============================================================ */
START_TEST(test_player_reset_valid){
    Player *p = malloc(sizeof(Player));
    ck_assert_int_eq(player_create(2, 5, 10, &p), OK);
    ck_assert_int_eq(player_reset_to_start(p, 4, 0, 0), OK);

    int x, y;
    ck_assert_int_eq(player_get_position(p, &x, &y), OK);
    ck_assert_int_eq(x, 0);
    ck_assert_int_eq(y, 0);
    ck_assert_int_eq(player_get_room(p), 4);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 15: resetting a null player
 * Tests that player_reset_to_start returns INVALID_ARGUMENT when player is null
 * ============================================================ */
START_TEST(test_player_reset_null){
    Player *p = NULL;
    ck_assert_int_eq(player_reset_to_start(p, 4, 0, 0), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 16: trying to collect a treasure with a valid player and treasure
 * Tests that player_try_collect works correctly when both player and treasure are valid
 * ============================================================ */
START_TEST(test_player_try_collect_valid){
    Player *p = malloc(sizeof(Player));
    Treasure *t = malloc(sizeof(Treasure));
    t->id = 1;
    t->collected = false;

    ck_assert_int_eq(player_create(1, 0, 0, &p), OK);
    ck_assert_int_eq(player_try_collect(p, t), OK);
    ck_assert_int_eq(player_has_collected_treasure(p, 1), true);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 17: trying to collect a treasure with a null player
 * Tests that player_try_collect returns NULL_POINTER when player is null
 * ============================================================ */
START_TEST(test_player_try_collect_null_player){
    Treasure *t = malloc(sizeof(Treasure));
    t->id = 1;
    t->collected = false;

    ck_assert_int_eq(player_try_collect(NULL, t), NULL_POINTER);
}
END_TEST

/* ============================================================
 * Test 18: trying to collect a null treasure
 * Tests that player_try_collect returns NULL_POINTER when treasure is null
 * ============================================================ */
START_TEST(test_player_try_collect_null_treasure){
    Player *p = malloc(sizeof(Player));
    ck_assert_int_eq(player_create(1, 0, 0, &p), OK);

    ck_assert_int_eq(player_try_collect(p, NULL), NULL_POINTER);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 19: trying to collect a treasure that has already been collected
 * Tests that player_try_collect returns INVALID_ARGUMENT when the treasure has already been collected
 * ============================================================ */
START_TEST(test_player_try_collect_already_collected){
    Player *p = malloc(sizeof(Player));
    Treasure *t = malloc(sizeof(Treasure));
    t->id = 1;
    t->collected = false;

    ck_assert_int_eq(player_create(1, 0, 0, &p), OK);
    ck_assert_int_eq(player_try_collect(p, t), OK);
    ck_assert_int_eq(player_try_collect(p, t), INVALID_ARGUMENT);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 20: trying to collect a treasure with the same ID as an already collected treasure
 * Tests that player_try_collect returns INVALID_ARGUMENT when the treasure has the same ID as an already collected treasure
 * ============================================================ */
START_TEST(test_player_try_collect_already_collected_in_player){
    Player *p = malloc(sizeof(Player));
    Treasure *t1 = malloc(sizeof(Treasure));
    t1->id = 1;
    t1->collected = false;

    Treasure *t2 = malloc(sizeof(Treasure));
    t2->id = 1; // same ID as t1
    t2->collected = false;

    ck_assert_int_eq(player_create(1, 0, 0, &p), OK);
    ck_assert_int_eq(player_try_collect(p, t1), OK);
    ck_assert_int_eq(player_try_collect(p, t2), INVALID_ARGUMENT);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 21: checking if a player has collected a treasure with null arguments
 * Tests that player_has_collected_treasure returns false when player is null or treasure_id is negative
 * ============================================================ */
START_TEST(test_player_has_treasure_null_args){
    Player *p = NULL;

    ck_assert_int_eq(player_has_collected_treasure(NULL, 1), false);
    ck_assert_int_eq(player_has_collected_treasure(p, -1), false);
    ck_assert_int_eq(player_has_collected_treasure(NULL, -1), false);
}
END_TEST

/* ============================================================
 * Test 22: checking if a player has collected a treasure that has not been collected
 * Tests that player_has_collected_treasure returns false when the treasure has not been collected
 * ============================================================ */
START_TEST(test_player_has_treasure_not_collected){
    Player *p = malloc(sizeof(Player));
    ck_assert_int_eq(player_create(1, 0, 0, &p), OK);
    ck_assert_int_eq(player_has_collected_treasure(p, 1), false);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 23: getting the collected count of a null player
 * Tests that player_get_collected_count returns 0 when player is null
 * ============================================================ */
START_TEST(test_player_get_collected_count_null){
    Player *p = NULL;
    ck_assert_int_eq(player_get_collected_count(p), 0);
}
END_TEST

/* ============================================================
 * Test 24: getting the collected count of a valid player
 * Tests that player_get_collected_count returns the correct count
 * ============================================================ */
START_TEST(test_player_get_collected_count_valid){
    Player *p = malloc(sizeof(Player));
    Treasure *t1 = malloc(sizeof(Treasure));
    t1->id = 1;
    t1->collected = false;

    Treasure *t2 = malloc(sizeof(Treasure));
    t2->id = 2;
    t2->collected = false;

    ck_assert_int_eq(player_create(1, 0, 0, &p), OK);
    ck_assert_int_eq(player_get_collected_count(p), 0);
    ck_assert_int_eq(player_try_collect(p, t1), OK);
    ck_assert_int_eq(player_get_collected_count(p), 1);
    ck_assert_int_eq(player_try_collect(p, t2), OK);
    ck_assert_int_eq(player_get_collected_count(p), 2);
    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 25: getting the collected treasures of a null player
 * Tests that player_get_collected_treasures returns NULL when player is null or count_out is null
 * ============================================================ */
START_TEST(test_player_get_treasures_null_args){
    Player *p = NULL;
    int count;
    ck_assert_ptr_null(player_get_collected_treasures(NULL, &count));

    ck_assert_ptr_null(player_get_collected_treasures(NULL, NULL));
    ck_assert_ptr_null(player_get_collected_treasures(p, NULL));
}
END_TEST

/* ============================================================
 * Test 26: getting the collected treasures of a valid player
 * Tests that player_get_collected_treasures returns the correct treasures and count
 * ============================================================ */
START_TEST(test_player_get_treasures_valid){
    Player *p = malloc(sizeof(Player));
    Treasure *t1 = malloc(sizeof(Treasure));
    t1->id = 1;
    t1->collected = false;

    Treasure *t2 = malloc(sizeof(Treasure));
    t2->id = 2;
    t2->collected = false;

    ck_assert_int_eq(player_create(1, 0, 0, &p), OK);
    int count;
    const Treasure * const * treasures = player_get_collected_treasures(p, &count);
    ck_assert_ptr_null(treasures);
    ck_assert_int_eq(count, 0);

    ck_assert_int_eq(player_try_collect(p, t1), OK);
    treasures = player_get_collected_treasures(p, &count);
    ck_assert_ptr_nonnull(treasures);
    ck_assert_int_eq(count, 1);
    ck_assert_int_eq(treasures[0]->id, 1);

    ck_assert_int_eq(player_try_collect(p, t2), OK);
    treasures = player_get_collected_treasures(p, &count);
    ck_assert_ptr_nonnull(treasures);
    ck_assert_int_eq(count, 2);
    ck_assert_int_eq(treasures[0]->id, 1);
    ck_assert_int_eq(treasures[1]->id, 2);

    player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 27: getting the collected treasures of a valid player with no treasures
 * Tests that player_get_collected_treasures returns a non-null pointer and count of 0 when player has not collected any treasures
 * ============================================================ */
START_TEST(test_player_get_treasures_empty){
    Player *p = malloc(sizeof(Player));
    ck_assert_int_eq(player_create(1, 0, 0, &p), OK);
    int count;
    const Treasure * const * treasures = player_get_collected_treasures(p, &count);
    ck_assert_ptr_null(treasures);
    ck_assert_int_eq(count, 0);
    player_destroy(p);
}
END_TEST

START_TEST(test_reset_player_with_treasures){
    Player *p = malloc(sizeof(Player));

    Treasure *t1 = malloc(sizeof(Treasure));
    t1->id = 1;
    t1->collected = false;

    Treasure *t2 = malloc(sizeof(Treasure));
    t2->id = 2;
    t2->collected = false;

    ck_assert_int_eq(player_create(1, 0, 0, &p), OK);
    ck_assert_int_eq(player_try_collect(p, t1), OK);
    ck_assert_int_eq(player_try_collect(p, t2), OK);

    ck_assert_int_eq(player_reset_to_start(p, 4, 0, 0), OK);

    ck_assert_int_eq(player_get_collected_count(p), 0);
    ck_assert_int_eq(t1->collected, false);
    ck_assert_int_eq(t2->collected, false);
    player_destroy(p);
}

//suite creation
Suite *test_player_creation_suite(void){
    //create the new test suite and test case
    Suite *s = suite_create("TestPlayerCreation");;
    TCase *tc = tcase_create("Core");

    //add each test to the test case
    tcase_add_test(tc, test_create_player);
    tcase_add_test(tc, test_null_player_out);
    tcase_add_test(tc, test_player_get_room_valid);
    tcase_add_test(tc, test_player_get_room_null);
    tcase_add_test(tc, test_player_get_position_valid);
    tcase_add_test(tc, test_player_get_position_null_player);
    tcase_add_test(tc, test_player_get_position_null_x);
    tcase_add_test(tc, test_player_get_position_null_y);
    tcase_add_test(tc, test_player_get_position_all_null);
    tcase_add_test(tc, test_player_set_position_valid);
    tcase_add_test(tc, test_player_set_position_null);
    tcase_add_test(tc, test_player_move_room_valid);
    tcase_add_test(tc, test_player_move_room_null);
    tcase_add_test(tc, test_player_reset_valid);
    tcase_add_test(tc, test_player_reset_null);
    tcase_add_test(tc, test_player_try_collect_valid);
    tcase_add_test(tc, test_player_try_collect_null_player);
    tcase_add_test(tc, test_player_try_collect_null_treasure);
    tcase_add_test(tc, test_player_try_collect_already_collected);
    tcase_add_test(tc, test_player_try_collect_already_collected_in_player);
    tcase_add_test(tc, test_player_has_treasure_null_args);
    tcase_add_test(tc, test_player_has_treasure_not_collected);
    tcase_add_test(tc, test_player_get_collected_count_null);
    tcase_add_test(tc, test_player_get_collected_count_valid);
    tcase_add_test(tc, test_player_get_treasures_null_args);
    tcase_add_test(tc, test_player_get_treasures_valid);
    tcase_add_test(tc, test_player_get_treasures_empty);
    tcase_add_test(tc, test_reset_player_with_treasures);

    //add the test case to the suite
    suite_add_tcase(s, tc);

    //return the suite
    return s;
}
