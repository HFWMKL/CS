# DO NOT modify or add any import statements
from support import *

# Name: Chongbin Ge
# Student Number:48297231
# Favorite Tree:
# -----------------------------------------------------------------------------

# Define your classes and functions here
player1 = PLAYER_1_PIECE
player2 = PLAYER_2_PIECE
PLAYER_1_DISPLAY = "Player 1"
PLAYER_2_DISPLAY = "Player 2"
player = player1
global restart
# task1
def num_hours() ->float:
    return 40.0

# task2
def create_empty_board(board_size: int) -> list[list[str]]:
    """
nest for to show '○'
1  2  3  4  5  6  7  8  9
1 ○  ○  ○  ○  ○  ○  ○  ○  ○
2 ○  ○  ○  ○  ○  ○  ○  ○  ○
To make a board to running game
    :param  int board_size
    :return: list[list[str]] board
        """
    board = []
    for i in range(board_size):
        row = []
        '''这里卡了很久，每次循环都要创造一个数组，不然就在一个数组内部'''
        '''为什么不能在同一个数组内部？这里可以深挖'''
        for j in range(board_size):
            row.append(EMPTY)
        '''print放的位置，决定了最后显示的形状，因为放在了最外面，一开始只有一行'''
        board.append(row)
    return board
# task3
def display_board(board: list[list[str]]) -> None:
    size = len(board)

    # 打印列标题
    header = "  "
    for i in range(size):
        header += str(i + 1)
        if i != size - 1:
            header += "  "
    print(header)

    # 打印每一行
    for i in range(size):
        line = ""

        # 打印行号（左对齐）
        if i + 1 < 10:
            line += str(i + 1) + " "
        else:
            line += str(i + 1)

        # 打印每个单元格
        for j in range(size):
            line += board[i][j]
            if j != size - 1:
                line += "  "
        print(line)
        print()  # 空行分隔

#task4
def add_piece(board: list[list[str]], piece: str, pos: tuple[int, int]) -> bool:
    x, y = pos
    board_size = len(board)
    if 0 < x <= board_size and 0 < y <= board_size:
        # 这里用的是构造的没加数字的棋盘,这个在判断落子是否在棋盘
        # here use is not add the number's board and charge if pierce in board
            if board[x-1][y-1] == EMPTY:
                # here to charge if this location have piece before
                board[x-1][y-1] = piece
                return True

            else:
                print(INVALID_PLACEMENT_MESSAGE)
                return False


    else:
        print(INVALID_PLACEMENT_MESSAGE)
        return False

def move_piece(board: list[list[str]], piece: str,
               current_pos: tuple[int, int],
               target_pos: tuple[int, int]) -> bool:
    '''
        :param board:list[list[str]]
        :param piece:str
        :param current_pos: tuple[int, int],
               target_pos: tuple[int, int])
        :return:bool
        here to move piece which continue to move until win or lost
    '''
    # 怎么定位元素，现在不知道
    x, y = current_pos
    tx, ty = target_pos

    size = (len(board))
    if not (0 < x <= size and  0 < y <= size
        and 0 < tx <= size and 0 < ty <= size):
        print(INVALID_MOVEMENT_MESSAGE)
        return False
    if (board[x-1][y-1] == piece) and (board[tx-1][ty-1] ==  EMPTY):
        if abs(tx - x) <= 1 and abs(ty - y) <= 1:
            board[x-1][y-1] = EMPTY
            board[tx-1][ty-1] = piece
            return True
        else:
            print(INVALID_MOVEMENT_MESSAGE)
            return False
    else:
        print(INVALID_MOVEMENT_MESSAGE)
        return False
# board = create_empty_board(5)
# display_board(board)
# add_piece(board, player1, (1, 1))
# print(move_piece(board, player1, (1, 1), (2, 2)))
def check_input(command: str) -> bool:
    command = command.lower()
    if not command:
        return False
    "only choose right command"
    length = len(command)
    firrCommand = command[0]
    if (length == 1) and (firrCommand == 'q' or firrCommand == 'h'):
            return True
    elif (length == 3) and (firrCommand == 'p'):
        # here to check if input command[1:3] are numbers
        for mov_char in command[1:3]:
            if not ("0" <= mov_char <= "9"):
                print(INVALID_FORMAT_MESSAGE)
                return False
            return True
    elif (length == 5) and (firrCommand == 'm'):
        for mov_char in command[1:5]:
            # here to check if input command[1:5] are numbers
            if not ("0" <= mov_char <= "9"):
                print(INVALID_FORMAT_MESSAGE)
                return False
            # here to take off the location out
        x = int(command[1])
        y = int(command[2])
        z = int(command[3])
        u = int(command[4])
        dx = x - z
        dy = y - u
        # check the distance if is one step
        if abs(dx) <= 1 and abs(dy) <= 1:
            return True
        else:
            return False
    else:
        return False
# print(check_input("m4321"))
# task7
def get_command() -> str:
    while True:
        command = input(f"{ENTER_COMMAND_PROMPT}").strip().lower()
        if check_input(command):
            return command
        else:
            print(INVALID_FORMAT_MESSAGE)
def has_unbroken_line(board: list[list[str]], piece: str) -> bool:
    rows = len(board)
    cols = len(board[0])  # 假设每行列数一致

    for row in range(rows):
        for col in range(cols):
            # 横向 →
            if col <= cols - 4:
                count = 0
                for i in range(4):
                    if board[row][col + i] == piece:
                        count += 1
                    else:
                        break
                if count == 4:
                    return True

            # 纵向 ↓
            if row <= rows - 4:
                count = 0
                for i in range(4):
                    if board[row + i][col] == piece:
                        count += 1
                    else:
                        break
                if count == 4:
                    return True

            # 主对角线 ↘
            if row <= rows - 4 and col <= cols - 4:
                count = 0
                for i in range(4):
                    if board[row + i][col + i] == piece:
                        count += 1
                    else:
                        break
                if count == 4:
                    return True

            # 副对角线 ↙
            if row >= 3 and col <= cols - 4:
                count = 0
                for i in range(4):
                    if board[row - i][col + i] == piece:
                        count += 1
                    else:
                        break
                if count == 4:
                    return True
    return False
def has_square(board: list[list[str]], piece: str) -> bool:
    n = len(board)

    for row in range(n - 1):
        for col in range(n - 1):
            # 取出 2x2 的 4 个角
            if (board[row][col] == piece and
                    board[row][col + 1] == piece and
                    board[row + 1][col] == piece and
                    board[row + 1][col + 1] == piece):
                return True

    return False
def check_win(board: list[list[str]]) -> str:
    if has_unbroken_line(board,player1) or has_square(board, player1):
        return PLAYER_1_PIECE
    elif has_unbroken_line(board,player2) or has_square(board, player2):
        return PLAYER_2_PIECE
    else:
        return EMPTY
def play_game() -> None:
    print(WELCOME_MESSAGE)
    print()
    board_size = 5
    board = create_empty_board(board_size)
    piece = player1
    piece1number = 0
    piece2number = 0
    piece_number = 0
    game_state = True
    while game_state:

        piece_number = piece1number + piece2number
        show_number1 = 4 - piece1number
        if (4 - piece1number) < 0:
            show_number1 = 0
        show_number2 = 4 - piece2number
        if (4 - piece2number) < 0:
            show_number2 = 0

        display_board(board)
        # build board

        # show who's turn
        if player == player1:
            print(PLAYER_1_DISPLAY+turn_message(show_number1))
        else:
            print(PLAYER_2_DISPLAY+turn_message(show_number2))
        # show who will place
        command = get_command()
        if command == "q":
            game_state = False
        elif command == "h":
            print(HELP_MESSAGE)

        elif command[0] == "p":
            x = int(command[1])
            y = int(command[2])
            add_storge = True
            while add_storge:
                if piece_number < 8:
                    if piece == player1:
                        add_storge = add_piece(board, piece, (x, y))
                        # 立即判断是否获胜
                        winner = check_win(board)
                        if winner != EMPTY:
                            display_board(board)
                            print(f"{winner}{VICTORY_MESSAGE}")
                            again = input(AGAIN_PROMPT).strip().lower()
                            if again == 'y'or again == 'yes':
                                restart = True
                                game_state = False
                            else:
                                restart = False
                        if add_storge:
                            piece = player2
                            print(f"{piece}{PLACE_MESSAGE}")
                            piece1number += 1
                    else:
                        add_storge = add_piece(board, piece, (x, y))
                        # 立即判断是否获胜
                        winner = check_win(board)
                        if winner != EMPTY:
                            display_board(board)
                            print(f"{winner}{VICTORY_MESSAGE}")
                            again = input(AGAIN_PROMPT).strip().lower()
                            if again[0] == 'y':
                                restart = True
                                game_state = False
                            else:
                                restart = False
                        if add_storge:
                            piece = player1
                            print(f"{piece}{PLACE_MESSAGE}")
                            piece2number += 1
                else:
                    add_storge = False
                    print(ALREADY_PLACED_MESSAGE)
        elif (piece_number == 8) and (command[0] == "m"):
            x = int(command[1])
            y = int(command[2])
            z = int(command[3])
            u = int(command[4])
            moving = True
            while moving :
                if board[x - 1][y - 1] == piece:
                    if piece == player1:
                        moving = move_piece(board, piece, (x, y), (z, u))
                        winner = check_win(board)
                        if winner != EMPTY:
                            display_board(board)
                            print(f"{winner}{VICTORY_MESSAGE}")
                            again = input(AGAIN_PROMPT).strip().lower()
                            if again == 'y'or again == 'yes':
                                game_state = False
                            else:
                                restart = False
                        piece = player2
                        print(f"{piece}{MOVE_MESSAGE}")

                    else:
                        moving = move_piece(board, piece, (x, y), (z, u))
                        winner = check_win(board)
                        if winner != EMPTY:
                            display_board(board)
                            print(f"{winner}{VICTORY_MESSAGE}")
                            again = input(AGAIN_PROMPT).strip().lower()
                            if again == 'y' or again == 'yes':
                                game_state = False
                            else:
                                restart = False
                        piece = player1
                        print(f"{piece}{MOVE_MESSAGE}")
                else:
                    print(INVALID_MOVEMENT_MESSAGE)
                    moving = False

        elif piece_number < 8 and (command[0] == "m"):
            print(MUST_PLACE_MESSAGE)
        else:
            print(INVALID_FORMAT_MESSAGE)

# play_game()
# def main():
#     restart = True
#     while True:
#         restart = play_game()
#         if not restart:
#             break





def main() -> None:
    play_game()

if __name__ == "__main__":
    main()



