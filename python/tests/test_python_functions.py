import unittest
import os
from treasure_runner.models.game_engine import GameEngine
from treasure_runner.bindings import Direction
from treasure_runner.models.exceptions import ImpassableError, GameEngineError, NoPortalError

class TestSuite(unittest.TestCase):
    ######################################################################################
    # Setup and Teardown
    ######################################################################################
    DIR = os.path.abspath("../assets/example_integration_run.ini")
    
    def setUp(self):
        self.engine = GameEngine(self.DIR)

    def tearDown(self):
        if not self.engine.destroyed:
            self.engine.destroy()

    ######################################################################################
    # Game Engine Tests
    ######################################################################################
    def test_engine_create_valid_config(self):
        engine = GameEngine(self.DIR)
        self.assertIsNotNone(engine)
        engine.destroy()
    
    def test_engine_create_invalid_config(self):
        with self.assertRaises(GameEngineError):
            GameEngine("../assets/nonexistent_config.ini")
    
    def test_engine_destroy(self):
        engine = GameEngine(self.DIR)
        engine.destroy()
        with self.assertRaises(Exception):
            engine.move_player(Direction.NORTH)  # should fail since engine is destroyed
        
    def test_move_player_valid(self):
        original_room = self.engine.player.get_room()
        original_position = self.engine.player.get_position()

        # Attempt to move in all directions and check for valid movement
        for d in [Direction.NORTH, Direction.SOUTH, Direction.EAST, Direction.WEST]:
            try:
                self.engine.move_player(d)
                new_room = self.engine.player.get_room()
                new_position = self.engine.player.get_position()

                # Check if player moved to a different room or changed position within the same room
                if new_room != original_room or new_position != original_position:
                    return  # Valid move found, test passes

                # Reset for next attempt
                self.engine.reset()
            except ImpassableError:
                continue  # Try next direction

        self.fail("No valid moves found for player at starting position")
    
    def test_move_player_invalid(self):
        dimensions = self.engine.get_room_dimensions()
        current_position = self.engine.player.get_position()
        
        while current_position[1] < dimensions[1] - 1:
            try:
                self.engine.move_player(Direction.SOUTH)
                current_position = self.engine.player.get_position()
            except ImpassableError:
                break  # Found a direction that is blocked, test can proceed
        with self.assertRaises(ImpassableError):
            self.engine.move_player(Direction.SOUTH)  # Should raise since it's blocked
    
    def test_render_current_room(self):
        room_str = self.engine.render_current_room()
        self.assertIsInstance(room_str, str)
        self.assertGreater(len(room_str), 0)
        
    def test_get_room_count(self):
        count = self.engine.get_room_count()
        self.assertIsInstance(count, int)
        self.assertGreater(count, 0)
    
    def test_get_room_dimensions(self):
        dimensions = self.engine.get_room_dimensions()
        self.assertIsInstance(dimensions, tuple)
        self.assertEqual(len(dimensions), 2)
        self.assertIsInstance(dimensions[0], int)
        self.assertIsInstance(dimensions[1], int)
        self.assertGreater(dimensions[0], 0)
        self.assertGreater(dimensions[1], 0)
        
    def test_get_room_ids(self):
        ids = self.engine.get_room_ids()
        self.assertIsInstance(ids, list)
        self.assertGreater(len(ids), 0)
        for room_id in ids:
            self.assertIsInstance(room_id, int)
            
    def test_reset(self):
        original_room = self.engine.player.get_room()
        original_position = self.engine.player.get_position()

        # Move player to change state
        try:
            self.engine.move_player(Direction.SOUTH)
        except ImpassableError:
            pass  # Ignore if move is blocked, we just want to change state if possible

        # Reset and check if player is back to original state
        self.engine.reset()
        reset_room = self.engine.player.get_room()
        reset_position = self.engine.player.get_position()
        self.assertEqual(reset_room, original_room)
        self.assertEqual(reset_position, original_position)
    
    ######################################################################################
    # Player Tests
    ######################################################################################
    def test_get_player_position(self):
        position = self.engine.player.get_position()
        self.assertIsInstance(position, tuple)
        self.assertEqual(len(position), 2)
        self.assertIsInstance(position[0], int)
        self.assertIsInstance(position[1], int)
        
    def test_get_player_room(self):
        room_id = self.engine.player.get_room()
        self.assertIsInstance(room_id, int)
        
    def test_player_collected_count(self):
        count = self.engine.player.get_collected_count()
        self.assertIsInstance(count, int)
        self.assertGreaterEqual(count, 0)
    
    def test_players_collected_treasures(self):
        treasures = self.engine.player.get_collected_treasures()
        self.assertIsInstance(treasures, list)
        for treasure in treasures:
            self.assertIsInstance(treasure, dict)
            self.assertIn("id", treasure)
            self.assertIn("name", treasure)
            self.assertIn("starting_room_id", treasure)
            self.assertIn("initial_x", treasure)
            self.assertIn("initial_y", treasure)
            self.assertIn("x", treasure)
            self.assertIn("y", treasure)
            self.assertIn("collected", treasure)
    
    def test_player_has_treasure(self):
        def try_enter_portal() -> None:
            try:
                self.engine.enter_portal()
            except NoPortalError:
                pass
            except ImpassableError:
                pass

        # Deterministic bounded exploration: repeated sweeps plus portal attempts.
        # A single sweep can miss treasure due to gated portals and pushable layouts.
        for _ in range(20):
            for d in [Direction.SOUTH, Direction.WEST, Direction.NORTH, Direction.EAST]:
                while True:
                    pre_room = self.engine.player.get_room()
                    pre_pos = self.engine.player.get_position()
                    pre_collected = self.engine.player.get_collected_count()

                    try:
                        self.engine.move_player(d)
                    except ImpassableError:
                        break

                    # If the move lands on a portal, try to traverse it.
                    try_enter_portal()

                    post_room = self.engine.player.get_room()
                    post_pos = self.engine.player.get_position()
                    post_collected = self.engine.player.get_collected_count()

                    if post_collected > pre_collected:
                        break

                    # No state change indicates no forward progress in this direction.
                    if post_room == pre_room and post_pos == pre_pos:
                        break

                if self.engine.player.get_collected_count() > 0:
                    break

            if self.engine.player.get_collected_count() > 0:
                break

        treasures = self.engine.player.get_collected_treasures()
        self.assertGreater(len(treasures), 0, "Expected to collect at least one treasure")
        treasure_id = treasures[0]["id"]
        has_treasure = self.engine.player.has_collected_treasure(treasure_id)
        self.assertIsInstance(has_treasure, bool)
        self.assertTrue(has_treasure)
    
    
    
if __name__ == "__main__":
    unittest.main()
