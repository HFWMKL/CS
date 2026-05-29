# DO NOT modify or add any import statements
from support import *
from display import KCView

# Name:Chongbin Ge
# Student Number:48297231
# Favorite Author: 
# -----------------------------------------------------------------------------

# Define your classes and functions here
from support import *
from display import KCView

class Tile:
    def __init__(self, blocking=False):
        self.tile_ID = TILE_ID
        self.blocking = blocking

    def __str__(self):
        return self.tile_ID

    def __repr__(self):
        return f"{self.__class__.__name__}()"

    def is_blocking(self):
        return self.blocking
class Floor(Tile):
    def __init__(self):
        super().__init__()
        self.tile_ID = FLOOR_ID
class Wall(Tile):
    def __init__(self):
        super().__init__(blocking=True)
        self.tile_ID = WALL_ID
class Door(Tile):
    def __init__(self, is_open: bool = False):
        super().__init__(blocking=True)
        self._is_open = is_open
        self.tile_ID = DOOR_ID
        if is_open:
            self.tile_ID = OPEN_ID
            self.blocking = False
        else:
            self.tile_ID = DOOR_ID
            self.blocking = True

    def __repr__(self):
        return f"{self.__class__.__name__}({self._is_open})"

    def is_open(self):
        return self._is_open

    def set_open(self, is_open: bool):
        self._is_open = is_open
        self.tile_ID = OPEN_ID if is_open else DOOR_ID
        self.blocking = False if is_open else True
class Entity():
    def __init__(self, pos: Position, speed: int, magic: int):
        self.pos = pos
        self.speed = speed
        self.magic = magic
        self.id = ENTITY_ID

    def __str__(self):
        return f"{self.id},{self.pos[0]},{self.pos[1]},{self.speed},{self.magic}"

    def __repr__(self):
        return f"{self.__class__.__name__}({self.pos!r}, {self.speed!r}, {self.magic!r})"

    def get_id(self) -> str:
        return self.id

    def get_position(self) -> Position:
        return self.pos

    def set_position(self, new_pos: Position):
        self.pos = new_pos

    def get_speed(self) -> int:
        return self.speed

    def set_speed(self, new_speed: int):
        self.speed = new_speed

    def get_magic(self) -> int:
        return self.magic

    def set_magic(self, new_magic: int):
        self.magic = new_magic
class Player(Entity):
    def __init__(self, pos: Position, speed: int, magic: int, health: int):
        super().__init__(pos=pos, speed=speed, magic=magic)
        self.id = PLAYER_ID
        self.health = health

    def __str__(self) -> str:
        return f"{self.id},{self.pos[0]},{self.pos[1]},{self.speed},{self.magic},{self.health}"

    def __repr__(self):
        return f"{self.__class__.__name__}({self.pos!r}, {self.speed!r}, {self.magic!r}, {self.health!r})"

    def get_health(self) -> int:
        return self.health

    def set_health(self, new_health: int):
        self.health = new_health

    def is_alive(self) -> bool:
        return self.health > 0

    def adjacent_positions(self) -> tuple[tuple[int, int], tuple[int, int], tuple[int, int], tuple[int, int]]:
        x, y = self.pos
        return (x - 1, y), (x, y - 1), (x, y + 1), (x + 1, y)
class Creature(Entity):
    def __init__(self, pos: Position, speed: int, magic: int = 0):
        super().__init__(pos=pos, speed=speed,magic = magic)
        self.id = CREATURE_ID

    def __str__(self) -> str:
        return f"{self.id},{self.pos[0]},{self.pos[1]},{self.speed},{self.magic!r}"

    def __repr__(self):
        return f"{self.__class__.__name__}({self.pos!r}, {self.speed!r}, {self.magic!r})"

    def get_id(self) -> str:
        return self.id

    def choose_move(self, target: Entity) -> str:
        x,y = self.pos
        tx,ty = target.pos
        move = {
            "up": (x - 1, y),
            "down": (x + 1, y),
            "left": (x, y - 1),
            "right": (x, y + 1)
        }
        best_distance = euclidean_distance(self.pos, target.get_position())
        best_move = "wait"
        for direction,new_pos in move.items():
            new_distance = euclidean_distance(new_pos, target.get_position())
            if new_distance < best_distance:
                best_distance = new_distance
                best_move = direction
            elif new_distance == best_distance:
                priority = ["up", "left", "right", "down"]
                if priority.index(direction) < priority.index(best_move):
                    best_move = direction
        return best_move
class Item(Tile):
    def __init__(self, consumed: bool = False):
        super().__init__()
        self.consumed = consumed
        self.id = ITEM_ID if not consumed else PICKED_ID

    def __str__(self):
        return self.id

    def __repr__(self):
        return f"{self.__class__.__name__}({self.consumed!r})"

    def is_consumed(self) -> bool:
        return self.consumed

    def pick_up(self, entity: Entity):
        self.consumed = True
        self.id = PICKED_ID
class SpeedPotion(Item):
    def __init__(self, consumed: bool):
        super().__init__(consumed)
        self.id = SPEED_ID if not consumed else PICKED_ID
        self.blocking = False

    def __str__(self):
        return self.id

    def __repr__(self):
        return f"{self.__class__.__name__}({self.consumed!r})"

    def pick_up(self, entity: Entity):
        if self.consumed:
            return
        self.consumed = True
        self.id = PICKED_ID
        entity.speed += 1
class MagicPotion(Item):
    def __init__(self, consumed: bool):
        super().__init__(consumed)
        self.blocking = False
        self.id = MAGIC_ID if not consumed else PICKED_ID

    def __str__(self):
        return self.id

    def __repr__(self):
        return f"{self.__class__.__name__}({self.consumed!r})"

    def pick_up(self, entity: Entity):
        if self.consumed:
            return
        self.consumed = True
        self.id = PICKED_ID
        entity.magic += 1
class Key(Item):
    def __init__(self, consumed: bool):
        super().__init__(consumed)
        self.blocking = False
        self.id = KEY_ID if not consumed else PICKED_ID

    def __str__(self):
        return self.id

    def __repr__(self):
        return f"{self.__class__.__name__}({self.consumed!r})"

    def pick_up(self, entity: Player):
        if isinstance(entity, Player):
            if not self.consumed:
                self.consumed = True
                self.id = PICKED_ID
                entity.magic += 1
class Dungeon():
    def __init__(self, tiles: list[list[Tile]]):
        self.tiles = tiles
        self.doors = {}
        self.items = {}
        for r, row in enumerate(tiles):
            for c, tile in enumerate(row):
                pos = (r, c)
                if isinstance(tile, Door):
                    self.doors[pos] = tile
                elif isinstance(tile, Item):
                    self.items[pos] = tile

    def __repr__(self) -> str:
        return f"Dungeon({self.tiles!r})"

    def __str__(self) -> str:
        return '\n'.join(''.join(str(tile) for tile in row) for row in self.tiles)

    def get_tiles(self) -> list[list[Tile]]:
        return self.tiles

    def get_doors(self) -> dict[Position, Door]:
        return self.doors

    def get_items(self) -> dict[Position, Item]:
        return self.items

    def keys_remaining(self) -> int:
        return sum(1 for item in self.items.values()
                   if isinstance(item, Key) and not item.is_consumed())

    def open_doors(self):
        for door in self.doors.values():
            door.set_open(True)
class KCModel():
    def __init__(self, dungeon: Dungeon, player: Player, creatures: list[Creature]):
        self._dungeon = dungeon
        self._player = player
        self._creatures = creatures
    def __str__(self) -> str:
        grid = [list(row) for row in str(self._dungeon).split('\n')]
        row, col = self._player.get_position()
        grid[row][col] = self._player.id
        for c in self._creatures:
            cr, cc = c.get_position()
            grid[cr][cc] = c.id
        return "\n".join("".join(row) for row in grid)
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({repr(self._dungeon)}, {repr(self._player)}, {repr(self._creatures)})"
    def get_dungeon(self) -> Dungeon:
        return self._dungeon
    def get_player(self) -> Player:
        return self._player
    def get_creatures(self) -> list[Creature]:
        return self._creatures
    def has_won(self) -> bool:
        player_pos = self._player.get_position()
        doors = self._dungeon.get_doors()
        return player_pos in doors and doors[player_pos].is_open() and self._player.is_alive() > 0
    def has_lost(self) -> bool:
        return self._player.get_health() <= 0
    def move_entity(self, entity: Entity, move: str):
        move = move.lower()
        if move == "wait":
            return
        dx, dy = 0, 0
        if move == "up":
            dx, dy = -1, 0
        if move == "down":
            dx, dy = 1, 0
        if move == "left":
            dx, dy = 0, -1
        if move == "right":
            dx, dy = 0, 1

        row, col = entity.get_position()
        for _ in range(entity.get_speed()):
            new_r = row + dx
            new_c = col + dy
            if new_r <= 0 or new_c <= 0 or new_r > len(self._dungeon.get_tiles()) or new_c > len(
                    self._dungeon.get_tiles()[0]):
                break
            tile = self._dungeon.get_tiles()[new_r][new_c]
            if tile.is_blocking():
                break
            if (new_r, new_c) == self._player.get_position() and entity != self._player:
                break
            if any((new_r, new_c) == c.get_position() for c in self._creatures if c != entity):
                break
            row, col = new_r, new_c
                # 移动
        entity.set_position((row, col))

            # 移动结束，尝试拾取物品
        items = self._dungeon.get_items()
        if entity.get_position() in items:
            items[entity.get_position()].pick_up(entity)
            if isinstance(entity, Player) and self._dungeon.keys_remaining() == 0:
                self._dungeon.open_doors()
    def make_move(self, move: str):
        # 1.改变玩家位置
        self.move_entity(self._player, move)
        # 2.让怪兽追逐玩家
        for creature in self._creatures[:]:
            # 逻辑是先判断是否相邻，如果相邻立刻攻击
            # 攻击玩家
                # 判读是否相邻
            if creature.get_position() in self._player.adjacent_positions():
                # 玩家依据魔法改变怪物

                if self._player.get_magic() > creature.get_magic():
                    self._player.set_magic(self._player.get_magic() - 1)
                    self._creatures.remove(creature)
                else:
                    self._player.set_health(self._player.get_health() - 1)

                # 移动
            direction = creature.choose_move(self._player)
            self.move_entity(creature, direction)
                # 判读是否相邻
            if creature.get_position() in self._player.adjacent_positions():
                # 玩家依据魔法改变怪物
                if self._player.get_magic() > creature.get_magic():
                    self._player.set_magic(self._player.get_magic() - 1)
                    self._creatures.remove(creature)
                else:
                    self._player.set_health(self._player.get_health() - 1)
def load_model(file: str) -> KCModel:
    with open(file, "r") as f:
        content = f.read().strip()

    try:
        dungeon_str, entities_str = content.split("\n\n", 1)
    except ValueError:
        raise ValueError(FILE_NOT_FOUND_MSG)
    dungeon_tiles = []
    for r, row in enumerate(dungeon_str.split("\n")):
        tile_row = []
        for ch in row:
            if ch == FLOOR_ID:
                tile_row.append(Floor())
            elif ch == WALL_ID:
                tile_row.append(Wall())
            elif ch == DOOR_ID:
                tile_row.append(Door(False))
            elif ch == OPEN_ID:
                tile_row.append(Door(True))
            elif ch == ITEM_ID or ch == PICKED_ID:
                tile_row.append(Item(ch == PICKED_ID))
            elif ch == SPEED_ID:
                tile_row.append(SpeedPotion(False))
            elif ch == MAGIC_ID:
                tile_row.append(MagicPotion(False))
            elif ch == KEY_ID:
                tile_row.append(Key(False))
            else:
                raise ValueError(INVALID_TILE_MSG)
        dungeon_tiles.append(tile_row)
    dungeon = Dungeon(dungeon_tiles)
    "地下城和实体分开"
    lines = entities_str.strip().split("\n")
    if len(lines) < 1:
        raise ValueError(FILE_NOT_FOUND_MSG)
    "玩家从实体群分开"
    p_parts = lines[0].split(",")
    try:
        if p_parts[0] != PLAYER_ID:
            raise ValueError(INVALID_PLAYER_MSG)
        x, y, hp, spd, mag = map(int, p_parts[1:])
        player = Player((x, y), hp, spd, mag)
    except Exception:
        raise ValueError(INVALID_PLAYER_MSG)

    creatures = []
    "怪物从实体群分开"
    for line in lines[1:]:
        parts = line.split(",")
        try:
            if parts[0] != CREATURE_ID:
                raise ValueError(INVALID_CREATURE_MSG)
            x, y, spd, mag = map(int, parts[1:])
            # 这里解包的时候要注意，按照参数传入的顺序去赋值
            creatures.append(Creature((x, y), spd, mag))
        except Exception:
            raise ValueError(INVALID_CREATURE_MSG)
    return KCModel(dungeon, player, creatures)
class KCController():
    def __init__(self, model: KCModel):
        self.model = model
        self.view = KCView()
    def __repr__(self):
        return f"KCController({repr(self.model)})"
    def __str__(self):
        return str(self.model)
    def print_game(self):
        player = self.model.get_player()
        creatures = self.model.get_creatures()
        dungeon = self.model.get_dungeon().get_tiles()
        self.view.draw_game(
            tiles=dungeon,
            player=player,
            creatures=creatures
        )
    def load_game(self, file: str):
        try:
            new_mode = load_model(file)
            self.model = new_mode
            self.print_game()
        except ValueError as e:
            raise e
    def get_command(self):
        while True:
            direction = ["left","right","up","down"]
            cmd = input(COMMAND_PROMPT)
            if cmd in ("help", "wait", "quit"):
                return cmd
            if cmd.lower().startswith("move "):
                "这个部分应该没必要使用字典，单纯使用列表就够了"
                parts = cmd.split(" ")
                if len(parts) == 2 and parts[1] in direction:
                    return parts[1]
            if cmd.startswith("load "):
                parts = cmd.split( )
                if len(parts) == 2 and parts[1].endswith(".txt"):
                    return parts[0]
            print(INVALID_COMMAND_MSG)

    def play(self):
        while not (self.model.has_won() or self.model.has_lost()):
            self.print_game()
            cmd = self.get_command()
            if cmd == "help":
                print(HELP_MSG)
            elif cmd == "quit":
                break
            elif cmd == "wait":
                self.model.make_move(cmd)
            elif cmd == "load":
                self.print_game()
            elif cmd in ("left","right","up","down"):
                self.model.make_move(cmd)
        if self.model.has_won():
            print(WIN_MSG )
        else:
            print(LOSE_MSG)
def play_game(file: str):
    model = load_model(file)           # 1. Load KCModel from file
    controller = KCController(model)   # 2. Create controller with the model
    controller.play()                  # 3. Play the game

def main():
    play_game("Clark's_python_game/file.txt")

if __name__ == "__main__":
    main()