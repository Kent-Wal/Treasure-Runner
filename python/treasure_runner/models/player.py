import ctypes
from ..bindings import lib, Player as CPlayer, Status, Direction
from .exceptions import status_to_exception, GameEngineError


class Player:
    def __init__(self, player: CPlayer):
        self._player = player
    def get_room(self) -> int:
        room_id = lib.player_get_room(self._player)
        if room_id < 0:
            raise GameEngineError(f"Failed to get player's current room: {room_id}")
        return room_id
    def get_position(self) -> tuple[int, int]:
        x = ctypes.c_int()
        y = ctypes.c_int()
        status = lib.player_get_position(self._player, ctypes.byref(x), ctypes.byref(y))
        if status != Status.OK:
            raise status_to_exception(status, f"Failed to get player's position: {status}")
        return (x.value, y.value)
    def get_collected_count(self) -> int:
        count = lib.player_get_collected_count(self._player)
        if count < 0:
            raise GameEngineError(f"Failed to get player's collected treasure count: {count}")
        return count
    def has_collected_treasure(self, treasure_id: int) -> bool:
        result = lib.player_has_collected_treasure(self._player, treasure_id)
        return bool(result)

    def get_collected_treasures(self) -> list[dict]:
        count = ctypes.c_int()
        treasures_ptr = lib.player_get_collected_treasures(self._player, ctypes.byref(count))
        if not treasures_ptr:
            return []

        treasures = []
        for i in range(count.value):
            treasure = treasures_ptr[i].contents
            treasures.append({
                "id": treasure.id,
                "name": treasure.name.decode('utf-8') if treasure.name else None,
                "starting_room_id": treasure.starting_room_id,
                "initial_x": treasure.initial_x,
                "initial_y": treasure.initial_y,
                "x": treasure.x,
                "y": treasure.y,
                "collected": bool(treasure.collected),
            })

        return treasures
    