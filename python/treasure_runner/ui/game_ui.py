import curses
from treasure_runner.models.game_engine import GameEngine


class GameUI:
    CONTROLS_MESSAGE = "Game Controls: Arrows/WASD to move | . or > enter portal | r reset world | q quit"
    GAME_NAME_MESSAGE = "Treasure Runner - A Maze Exploration Game"
    EMAIL_MESSAGE = "waldenk@uoguelph.ca"

    def __init__(self, game_engine: GameEngine, stdscr) -> None:
        self.game_engine = game_engine
        charset = self.game_engine.get_charset()
        self._player_symbol = charset["player"]
        self._wall_symbol = charset["wall"]
        self._floor_symbol = charset["floor"]
        self._treasure_symbol = charset["treasure"]
        self._portal_symbol = charset["portal"]
        self._pushable_symbol = charset["pushable"]
        self._switch_off_symbol = charset["switch_off"]
        self._switch_on_symbol = charset["switch_on"]
        self._stdscr = stdscr
        self._status_message = "Welcome to Treasure Runner!"
        self._message_attr = curses.A_NORMAL
        self._completed_room_ids: set[int] = set()
        self._default_attr = curses.A_NORMAL
        self._player_attr = curses.A_NORMAL
        self._treasure_attr = curses.A_NORMAL
        self._pushable_attr = curses.A_NORMAL
        self._switch_attr = curses.A_NORMAL
        self._portal_passable_attr = curses.A_NORMAL
        self._portal_blocked_attr = curses.A_NORMAL

    # outline taken from lesson overview
    def init_screen(self) -> None:
        curses.cbreak()             #dont wait for Enter to send input
        curses.noecho()             #dont show typed characters
        self._stdscr.keypad(True)    #enable special keys (arrows, etc)

        if curses.has_colors():
            curses.start_color()        #enable color support
            curses.use_default_colors() #use terminal's default background
            curses.init_pair(1, -1, -1)
            curses.init_pair(2, curses.COLOR_WHITE, -1)
            curses.init_pair(3, curses.COLOR_GREEN, -1)
            curses.init_pair(4, curses.COLOR_YELLOW, -1)
            curses.init_pair(5, curses.COLOR_BLUE, -1)
            curses.init_pair(6, curses.COLOR_RED, -1)
            curses.init_pair(7, curses.COLOR_MAGENTA, -1)
            curses.init_pair(8, curses.COLOR_CYAN, -1)
            self._stdscr.bkgd(' ', curses.color_pair(1))
            self._message_attr = curses.color_pair(2)
            self._default_attr = curses.color_pair(1)
            self._treasure_attr = curses.color_pair(3)
            self._pushable_attr = curses.color_pair(4)
            self._portal_passable_attr = curses.color_pair(5)
            self._portal_blocked_attr = curses.color_pair(6)
            self._player_attr = curses.color_pair(7)
            self._switch_attr = curses.color_pair(8)

        else:
            self._stdscr.bkgd(' ', curses.A_NORMAL)
            self._message_attr = curses.A_NORMAL
            self._default_attr = curses.A_NORMAL
            self._treasure_attr = curses.A_NORMAL
            self._pushable_attr = curses.A_NORMAL
            self._switch_attr = curses.A_NORMAL
            self._portal_passable_attr = curses.A_NORMAL
            self._portal_blocked_attr = curses.A_NORMAL
            self._player_attr = curses.A_NORMAL

        self._clear_screen()

    def _clear_screen(self) -> None:
        self._stdscr.clear()
        self._stdscr.refresh()

    def set_status_message(self, message: str) -> None:
        self._status_message = message

    def reset_progress_tracking(self) -> None:
        self._completed_room_ids.clear()

    def get_completed_rooms_count(self) -> int:
        return len(self._completed_room_ids)

    def refresh_current_room_completion(self) -> None:
        room_id = self.game_engine.player.get_room()
        room_lines = self.game_engine.render_current_room().splitlines()
        self._update_completed_rooms(room_id, room_lines)

    def _update_completed_rooms(self, room_id: int, room_lines: list[str]) -> None:
        room_snapshot = "\n".join(room_lines)
        if self._treasure_symbol not in room_snapshot:
            self._completed_room_ids.add(room_id)

    def _draw_bar(self, row: int, text: str, max_x: int) -> None:
        if row < 0:
            return

        try:
            self._stdscr.move(row, 0)
            self._stdscr.clrtoeol()
            self._stdscr.addnstr(row, 0, text, max(0, max_x - 1), self._message_attr)
        except curses.error:
            pass

    def _legend_x(self, room_lines: list[str]) -> int:
        return max((len(line) for line in room_lines), default=0) + 6

    def _legend_lines(self) -> list[str]:
        return ["Legend"] + [f"{symbol} = {label}" for symbol, label in self._legend_items()]

    def _legend_items(self) -> list[tuple[str, str]]:
        return [
            (self._player_symbol, "Player"),
            (self._wall_symbol, "Wall"),
            (self._floor_symbol, "Floor"),
            (self._treasure_symbol, "Treasure"),
            (self._portal_symbol, "Portal"),
            (self._pushable_symbol, "Pushable"),
            (self._switch_off_symbol, "Switch (Off)"),
            (self._switch_on_symbol, "Switch (On)"),
        ]

    def _legend_symbol_attr(self, symbol: str) -> int:
        symbol_attr_map = {
            self._player_symbol: self._player_attr,
            self._treasure_symbol: self._treasure_attr,
            self._pushable_symbol: self._pushable_attr,
            self._switch_off_symbol: self._switch_attr,
            self._switch_on_symbol: self._switch_attr,
            self._portal_symbol: self._portal_passable_attr,
        }
        return symbol_attr_map.get(symbol, self._default_attr)

    def _draw_legend(self, room_lines: list[str], max_y: int, max_x: int) -> None:
        legend_x = self._legend_x(room_lines)

        if legend_x >= max_x - 1:
            return

        heading_y = 3
        if heading_y < max_y:
            try:
                self._stdscr.addnstr(heading_y, legend_x, "Legend", max(0, max_x - legend_x - 1), self._message_attr)
            except curses.error:
                pass

        for idx, (symbol, label) in enumerate(self._legend_items(), start=1):
            y = 3 + idx
            if y >= max_y:
                break
            try:
                self._stdscr.addch(y, legend_x, symbol, self._legend_symbol_attr(symbol))
                self._stdscr.addnstr(y, legend_x + 2, f"= {label}", max(0, max_x - (legend_x + 2) - 1), self._message_attr)
            except curses.error:
                pass

    def _portal_attr(self, x: int, y: int) -> int:
        try:
            can_enter = self.game_engine.can_enter_portal_at(x, y)
        except Exception:
            can_enter = True

        if can_enter:
            return self._portal_passable_attr
        return self._portal_blocked_attr

    def _char_attr(self, char_symbol: str, x: int, y: int) -> int:
        attr = self._default_attr
        if char_symbol == self._player_symbol:
            attr = self._player_attr
        elif char_symbol == self._treasure_symbol:
            attr = self._treasure_attr
        elif char_symbol == self._pushable_symbol:
            attr = self._pushable_attr
        elif char_symbol in (self._switch_off_symbol, self._switch_on_symbol):
            attr = self._switch_attr
        elif char_symbol == self._portal_symbol:
            attr = self._portal_attr(x, y)
        return attr

    def _draw_map(self, room_lines: list[str], max_y: int, max_x: int, start_y: int = 3) -> None:
        for y, line in enumerate(room_lines):
            draw_y = y + start_y
            if draw_y >= max_y:
                break
            for x, char_symbol in enumerate(line):
                if x >= max_x - 1:
                    break
                try:
                    self._stdscr.addch(draw_y, x, char_symbol, self._char_attr(char_symbol, x, y))
                except curses.error:
                    pass

    def _draw_controls(self, room_lines: list[str], max_y: int, max_x: int, start_y: int = 3) -> int:
        controls_y = start_y + len(room_lines) + 1
        if controls_y < max_y:
            try:
                self._stdscr.move(controls_y, 0)
                self._stdscr.clrtoeol()
                self._stdscr.addnstr(controls_y, 0, self.CONTROLS_MESSAGE, max(0, max_x - 1), self._message_attr)
            except curses.error:
                pass
        return controls_y

    def _progress_text(self) -> str:
        total, collected = self.game_engine.get_treasure_stats()
        completed_rooms = len(self._completed_room_ids)
        total_rooms = self.game_engine.get_room_count()
        rooms_left = max(0, total_rooms - completed_rooms)
        return (
            f"Treasures Collected: {collected}/{total} | "
            f"Rooms Completed: {completed_rooms} | "
            f"Rooms Left: {rooms_left}"
        )

    def _draw_footer(self, footer_y: int, max_x: int) -> None:
        email_x = max_x - len(self.EMAIL_MESSAGE) - 1
        left_max = max(0, email_x - 1) if email_x > 0 else max(0, max_x - 1)

        try:
            self._stdscr.move(footer_y, 0)
            self._stdscr.clrtoeol()
            self._stdscr.addnstr(footer_y, 0, self.GAME_NAME_MESSAGE, left_max, self._message_attr)

            if email_x >= 0:
                self._stdscr.addnstr(
                    footer_y,
                    email_x,
                    self.EMAIL_MESSAGE,
                    max(0, max_x - email_x - 1),
                    self._message_attr,
                )
        except curses.error:
            pass

    def _draw_progress_and_footer(self, controls_y: int, max_y: int, max_x: int) -> None:
        stats_y = controls_y + 2
        if stats_y >= max_y:
            return

        self._draw_bar(stats_y, self._progress_text(), max_x)

        footer_y = stats_y + 1
        if footer_y >= max_y:
            return
        self._draw_footer(footer_y, max_x)

    def _required_dimensions(self, room_lines: list[str]) -> tuple[int, int]:
        room_height = len(room_lines)
        room_width = max((len(line) for line in room_lines), default=0)
        legend_width = max((len(text) for text in self._legend_lines()), default=0)
        progress_width = len(self._progress_text())

        min_height = room_height + 8
        min_width = max(
            room_width + 1,
            self._legend_x(room_lines) + legend_width + 1,
            len(self.CONTROLS_MESSAGE) + 1,
            progress_width + 1,
            len(self.GAME_NAME_MESSAGE) + len(self.EMAIL_MESSAGE) + 2,
        )
        return min_height, min_width

    def _draw_terminal_too_small(self, max_y: int, max_x: int, min_y: int, min_x: int) -> None:
        lines = [
            "Terminal window is too small for Treasure Runner.",
            f"Current size: {max_x}x{max_y} (WxH)",
            f"Required size: {min_x}x{min_y} (WxH)",
            "Resize the terminal to continue playing.",
        ]

        self._stdscr.erase()
        for idx, text in enumerate(lines):
            y = idx
            if y >= max_y:
                break
            try:
                self._stdscr.addnstr(y, 0, text, max(0, max_x - 1), self._message_attr)
            except curses.error:
                pass
        self._stdscr.refresh()

    def render(self) -> None:
        room_lines = self.game_engine.render_current_room().splitlines()
        self._stdscr.erase()

        max_y, max_x = self._stdscr.getmaxyx()
        min_y, min_x = self._required_dimensions(room_lines)
        if max_y < min_y or max_x < min_x:
            self._draw_terminal_too_small(max_y, max_x, min_y, min_x)
            return

        room_id = self.game_engine.player.get_room()
        room_name = self.game_engine.get_current_room_name()
        self._update_completed_rooms(room_id, room_lines)

        self._draw_bar(0, self._status_message, max_x)
        self._draw_bar(1, f"Room Number: {room_id} | Room Name: {room_name}", max_x)
        self._draw_map(room_lines, max_y, max_x, start_y=3)
        self._draw_legend(room_lines, max_y, max_x)
        controls_y = self._draw_controls(room_lines, max_y, max_x, start_y=3)
        self._draw_progress_and_footer(controls_y, max_y, max_x)

        self._stdscr.refresh()
