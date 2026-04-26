import ctypes
from ..bindings import lib, GameEngine as CGameEngine, Status, Direction, Charset as CCharset
from .exceptions import status_to_exception, GameEngineError
from .player import Player

# creates the simple python functions that wrap the C library for GameEngine
class GameEngine:
    def __init__(self, config_path: str):
        if not isinstance(config_path, str):
            raise ValueError("config_path must be a string")

        self._eng = CGameEngine()
        status = lib.game_engine_create(config_path.encode('utf-8'), ctypes.byref(self._eng))
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to create game engine: {status}")

        self._player = lib.game_engine_get_player(self._eng)
        if not self._player:
            raise GameEngineError("Failed to get player from game engine")

        # Create a persistent Python wrapper for the engine-owned player
        self.player = Player(self._player)

    @property
    def destroyed(self) -> bool:
        return self._eng is None

    def _check_destroyed(self):
        if self.destroyed:
            raise GameEngineError("Game engine has been destroyed")

    def destroy(self) -> None:
        self._check_destroyed()
        lib.game_engine_destroy(self._eng)
        self._eng = None

    def move_player(self, direction: Direction) -> None:
        self._check_destroyed()
        pre_room = self.player.get_room()
        pre_position = self.player.get_position()
        pre_collected = self.player.get_collected_count()

        status = lib.game_engine_move_player(self._eng, direction)
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to move player: {status}")

        post_room = self.player.get_room()
        post_position = self.player.get_position()
        post_collected = self.player.get_collected_count()

        # If treasure was collected but C movement kept the player in place,
        # issue a second move call from Python to advance onto that tile.
        if post_collected > pre_collected and post_room == pre_room and post_position == pre_position:
            second_status = lib.game_engine_move_player(self._eng, direction)
            if second_status != Status.OK:
                raise status_to_exception(
                    second_status,
                    f"Failed to move player onto collected treasure: {second_status}",
                )

    def enter_portal(self) -> None:
        self._check_destroyed()
        status = lib.game_engine_enter_portal(self._eng)
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to enter portal: {status}")

    def render_current_room(self) -> str:
        self._check_destroyed()
        room_str_ptr = ctypes.c_char_p()
        status = lib.game_engine_render_current_room(self._eng, ctypes.byref(room_str_ptr))
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to render current room: {status}")

        room_str = room_str_ptr.value.decode('utf-8')
        lib.game_engine_free_string(room_str_ptr)  # Free the allocated string from C
        return room_str

    def get_room_count(self) -> int:
        self._check_destroyed()
        count = ctypes.c_int()
        status = lib.game_engine_get_room_count(self._eng, ctypes.byref(count))
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to get room count: {status}")
        return count.value

    def get_room_dimensions(self) -> tuple[int, int]:
        self._check_destroyed()
        width = ctypes.c_int()
        height = ctypes.c_int()
        status = lib.game_engine_get_room_dimensions(self._eng, ctypes.byref(width), ctypes.byref(height))
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to get room dimensions: {status}")
        return (width.value, height.value)

    def get_charset(self) -> dict[str, str]:
        self._check_destroyed()
        charset = CCharset()
        status = lib.game_engine_get_charset(self._eng, ctypes.byref(charset))
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to get charset: {status}")

        def _char(value: bytes) -> str:
            return value.decode("utf-8") if value else ""

        return {
            "wall": _char(charset.wall),
            "floor": _char(charset.floor),
            "player": _char(charset.player),
            "pushable": _char(charset.pushable),
            "treasure": _char(charset.treasure),
            "portal": _char(charset.portal),
            "switch_off": _char(charset.switch_off),
            "switch_on": _char(charset.switch_on),
        }

    def get_current_room_name(self) -> str:
        self._check_destroyed()
        name_ptr = ctypes.c_char_p()
        status = lib.game_engine_get_current_room_name(self._eng, ctypes.byref(name_ptr))
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to get current room name: {status}")

        name_bytes = name_ptr.value
        if name_bytes is not None:
            room_name = name_bytes.decode("utf-8")
        else:
            room_name = ""
        lib.game_engine_free_string(name_ptr)
        return room_name

    def can_enter_portal_at(self, x: int, y: int) -> bool:
        self._check_destroyed()
        can_enter_out = ctypes.c_bool()
        status = lib.game_engine_can_enter_portal_at(self._eng, x, y, ctypes.byref(can_enter_out))
        if status == Status.ROOM_NO_PORTAL:
            return False
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to check portal state at ({x}, {y}): {status}")
        return bool(can_enter_out.value)

    def get_room_ids(self) -> list[int]:
        self._check_destroyed()
        ids_ptr = ctypes.POINTER(ctypes.c_int)()
        count = ctypes.c_int()
        status = lib.game_engine_get_room_ids(self._eng, ctypes.byref(ids_ptr), ctypes.byref(count))
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to get room IDs: {status}")
        ids = [ids_ptr[i] for i in range(count.value)]
        lib.game_engine_free_string(ids_ptr)  # Free the allocated array from C
        return ids

    def get_treasure_stats(self) -> tuple[int, int]:
        self._check_destroyed()
        total = ctypes.c_int()
        collected = ctypes.c_int()
        status = lib.game_engine_get_treasure_stats(self._eng, ctypes.byref(total), ctypes.byref(collected))
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to get treasure stats: {status}")
        return (total.value, collected.value)

    def reset(self) -> None:
        self._check_destroyed()
        status = lib.game_engine_reset(self._eng)
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to reset game engine: {status}")
        