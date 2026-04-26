#include <check.h>
#include <stdlib.h>

Suite *test_player_creation_suite(void);
Suite *test_room_creation_suite(void);
Suite *test_world_creation_suite(void);
Suite *test_game_engine_creation_suite(void);

int main(void)
{
    Suite *suites[] = {
        test_player_creation_suite(),
        test_room_creation_suite(),
        test_world_creation_suite(),
        test_game_engine_creation_suite(),
        NULL
    };

    SRunner *runner = srunner_create(suites[0]);
    for (int i = 1; suites[i] != NULL; ++i) {
        srunner_add_suite(runner, suites[i]);
    }

    srunner_run_all(runner, CK_NORMAL);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
