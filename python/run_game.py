import curses
import os
import argparse
import json
import time
from datetime import datetime, timezone
from treasure_runner.models.game_engine import GameEngine
from treasure_runner.ui.game_ui import GameUI
from treasure_runner.bindings import Direction
from treasure_runner.models.exceptions import GameError, ImpassableError, NoPortalError

DEFAULT_PROFILE_TEMPLATE = {
    "player_name": "Player",
    "games_played": 0,
    "max_treasure_collected": 0,
    "most_rooms_world_completed": 0,
    "timestamp_last_played": None,
}


def _prompt_player_name(stdscr) -> str:
    stdscr.clear()
    stdscr.addstr(0, 0, "No player profile found.")
    stdscr.addstr(1, 0, "Enter player name: ")
    stdscr.refresh()

    curses.echo()
    try:
        name_bytes = stdscr.getstr(1, len("Enter player name: "), 40)
    finally:
        curses.noecho()

    name = name_bytes.decode("utf-8", errors="ignore").strip() if name_bytes else ""
    if not name:
        return "Player"
    return name


def ensure_player_profile(stdscr, player_path: str) -> dict:
    if os.path.exists(player_path):
        with open(player_path, "r", encoding="utf-8") as profile_file:
            return json.load(profile_file)

    player_name = _prompt_player_name(stdscr)
    profile = dict(DEFAULT_PROFILE_TEMPLATE)
    profile["player_name"] = player_name

    os.makedirs(os.path.dirname(player_path), exist_ok=True)
    with open(player_path, "w", encoding="utf-8") as profile_file:
        json.dump(profile, profile_file, indent=4)

    stdscr.clear()
    stdscr.addstr(0, 0, f"Created profile for {player_name}.")
    stdscr.addstr(1, 0, "Press any key to continue.")
    stdscr.refresh()
    stdscr.getch()
    return profile


def save_player_profile(player_path: str, profile: dict) -> None:
    os.makedirs(os.path.dirname(player_path), exist_ok=True)
    with open(player_path, "w", encoding="utf-8") as profile_file:
        json.dump(profile, profile_file, indent=4)


def update_profile_after_game(profile: dict, treasure_collected: int, did_win: bool, world_room_count: int) -> None:
    profile["games_played"] = int(profile.get("games_played", 0)) + 1
    profile["max_treasure_collected"] = max(int(profile.get("max_treasure_collected", 0)), treasure_collected)
    if did_win:
        profile["most_rooms_world_completed"] = max(
            int(profile.get("most_rooms_world_completed", 0)),
            world_room_count,
        )
    profile["timestamp_last_played"] = datetime.now(timezone.utc).isoformat()

def main(stdscr):
    args = parse_args()
    config_path = os.path.abspath(args.config)
    player_path = os.path.abspath(args.profile)
    profile = ensure_player_profile(stdscr, player_path)
    
    gameEngine = GameEngine(config_path)
    gameUI = GameUI(gameEngine, stdscr)
    
    
    while True:
        stdscr.clear()
        stdscr.addstr(0, 0, "Welcome to Treasure Runner!")
        stdscr.addstr(2, 0, "Player Profile")
        stdscr.addstr(3, 0, f"Name: {profile.get('player_name', 'Player')}")
        stdscr.addstr(4, 0, f"Games Played: {profile.get('games_played', 0)}")
        stdscr.addstr(5, 0, f"Max Treasure Collected: {profile.get('max_treasure_collected', 0)}")
        stdscr.addstr(6, 0, f"Most Rooms World Completed: {profile.get('most_rooms_world_completed', 0)}")
        stdscr.addstr(7, 0, f"Last Played: {profile.get('timestamp_last_played', 'Never')}")
        stdscr.addstr(9, 0, "1. Start New Game")
        stdscr.addstr(10, 0, "2. Exit")
        stdscr.refresh()

        key = stdscr.getch()
        if key == ord('1'):
            return_to_menu, treasure_collected, did_win = start_new_game(stdscr, gameUI, gameEngine, profile)
            update_profile_after_game(profile, treasure_collected, did_win, gameEngine.get_room_count())
            save_player_profile(player_path, profile)
            if not return_to_menu:
                break
        elif key == ord('2'):
            break
        else:
            stdscr.addstr(12, 0, "Invalid option. Please try again.")
            stdscr.refresh()
            stdscr.getch()
            
def show_win_screen(stdscr, run_stats: dict, profile_stats: dict) -> bool:
    while True:
        stdscr.clear()
        stdscr.addstr(0, 0, "Congratulations! You collected all the treasures!")

        stdscr.addstr(2, 0, "Player Profile")
        stdscr.addstr(3, 0, f"Name: {profile_stats.get('player_name', 'Player')}")
        stdscr.addstr(4, 0, f"Games Played: {profile_stats.get('games_played', 0)}")
        stdscr.addstr(5, 0, f"Max Treasure Collected: {profile_stats.get('max_treasure_collected', 0)}")
        stdscr.addstr(6, 0, f"Most Rooms World Completed: {profile_stats.get('most_rooms_world_completed', 0)}")
        stdscr.addstr(7, 0, f"Last Played: {profile_stats.get('timestamp_last_played', 'Never')}")

        stdscr.addstr(9, 0, "Run Stats")
        stdscr.addstr(10, 0, f"Treasures Collected: {run_stats['treasures_collected']}")
        stdscr.addstr(11, 0, f"Rooms Completed: {run_stats['rooms_completed']}")
        stdscr.addstr(12, 0, f"Rooms Left: {run_stats['rooms_left']}")
        stdscr.addstr(13, 0, f"Total Steps: {run_stats['total_steps']}")
        stdscr.addstr(14, 0, f"Total Rooms Entered: {run_stats['total_rooms_entered']}")
        stdscr.addstr(15, 0, f"Time Elapsed: {run_stats['time_elapsed_seconds']:.2f}s")

        stdscr.addstr(17, 0, "1. Return to Main Menu")
        stdscr.addstr(18, 0, "2. Exit Game")
        stdscr.refresh()

        key = stdscr.getch()
        if key == ord('1'):
            return True
        if key == ord('2'):
            return False


def start_new_game(stdscr, gameUI, gameEngine, profile):
    stdscr.clear()
    gameEngine.reset()
    gameUI.init_screen()
    gameUI.reset_progress_tracking()
    gameUI.set_status_message("Welcome to Treasure Runner!")
    did_win = False
    session_max_treasures = 0
    session_steps = 0
    rooms_entered = 1
    previous_room_id = gameEngine.player.get_room()
    run_start_time = time.monotonic()

    def session_stats() -> tuple[int, bool]:
        return session_max_treasures, did_win

    while True:
        gameUI.render()
        key = stdscr.getch()
        try:
            collected_before = gameEngine.player.get_collected_count()
            action_message = None

            if key in (curses.KEY_UP, ord('w'), ord('W')):
                gameEngine.move_player(Direction.NORTH)
                action_message = "Moved north"
            elif key in (curses.KEY_DOWN, ord('s'), ord('S')):
                gameEngine.move_player(Direction.SOUTH)
                action_message = "Moved south"
            elif key in (curses.KEY_LEFT, ord('a'), ord('A')):
                gameEngine.move_player(Direction.WEST)
                action_message = "Moved west"
            elif key in (curses.KEY_RIGHT, ord('d'), ord('D')):
                gameEngine.move_player(Direction.EAST)
                action_message = "Moved east"
            elif key in (ord('>'), ord('.')):
                gameEngine.enter_portal()
                action_message = "Entered portal"
            elif key in (ord('r'), ord('R')):
                gameEngine.reset()
                gameUI.reset_progress_tracking()
                previous_room_id = gameEngine.player.get_room()
                session_steps = 0
                rooms_entered = 1
                run_start_time = time.monotonic()
                gameUI.set_status_message("World reset")
            elif key == ord('q'):
                return True, *session_stats()

            if action_message is not None:
                collected_after = gameEngine.player.get_collected_count()
                if collected_after > collected_before:
                    gameUI.set_status_message("Treasure collected!")
                else:
                    gameUI.set_status_message(action_message)

                session_steps += 1
                current_room_id = gameEngine.player.get_room()
                if current_room_id != previous_room_id:
                    rooms_entered += 1
                    previous_room_id = current_room_id

            current_collected = gameEngine.player.get_collected_count()
            session_max_treasures = max(session_max_treasures, current_collected)
                    
            #check win condition
            if current_collected == gameEngine.get_treasure_stats()[0]:
                did_win = True
                total_rooms = gameEngine.get_room_count()
                gameUI.refresh_current_room_completion()
                rooms_completed = gameUI.get_completed_rooms_count()
                run_stats = {
                    "treasures_collected": current_collected,
                    "rooms_completed": rooms_completed,
                    "rooms_left": max(0, total_rooms - rooms_completed),
                    "total_steps": session_steps,
                    "total_rooms_entered": rooms_entered,
                    "time_elapsed_seconds": time.monotonic() - run_start_time,
                }
                profile_preview = dict(profile)
                update_profile_after_game(profile_preview, session_max_treasures, True, total_rooms)
                return show_win_screen(stdscr, run_stats, profile_preview), *session_stats()
        except (ImpassableError, NoPortalError):
            gameUI.set_status_message("Action blocked")
     
def parse_args():
    parser = argparse.ArgumentParser(description="Treasure Runner integration test logger")
    parser.add_argument(
        "--config",
        required=True,
        help="Path to generator config file",
    )
    parser.add_argument(
        "--profile",
        required=True,
        help="Path to player profile file",
    )
    return parser.parse_args()

if __name__ == "__main__":
    curses.wrapper(main)