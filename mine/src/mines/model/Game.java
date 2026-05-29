package mines.model;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

/**
* 【类功能】管理整个游戏的运行状态。
* [Class Function] Manages the overall running state of the game.
* * 【主要用途】负责初始化玩家和矿井、管理回合切换、以及执行购买和收集动作。
* [Purpose] Responsible for initializing players/mines, managing turn transitions,
* and executing purchase and collection actions.
*/
public class Game {
   //少了comment
    private final List<Player> players;
    private final Mine[][] grid;
    private int currentPlayerIndex = 0;
    private final Random random = new Random();
    private final List<Position> positions;
    //function:Attempts to have the current player collect gems from the GemPool.
    //@param GemPool(object)
    //@Returns a boolean indicating whether the gem collection was valid and successfully executed.

    /**
     * 【方法功能】初始化一个新游戏。
     * [Function] Initialise a new game.
     * * 【主要用途】创建 4 名玩家（0-3），并生成 4x3 的随机矿井矩阵。
     * 每个玩家初始资产为 0。玩家 0 首先开始。
     * [Purpose] Creates 4 players (0-3) and generates a 4x3 grid of random mines.
     * All players start with 0 assets. Player 0 starts.
     */
    public Game() {
        //创建地图
        //create map
        this.grid = new Mine[3][4];

        //创建地图坐标存
        // 储容器
        //creat box for map
        this.positions = new ArrayList<>();
        //创建玩家编号
        //create number of player
        this.currentPlayerIndex = 0;

        // 正确初始化玩家：独立循环 (Initialise players: independent loop)
        this.players = new ArrayList<>();
        for (int i = 0; i < 4; i++) {
            this.players.add(new Player(i));
        }
        // 这里是添加地图矿山的坐标点
        // This is where you add the coordinates for the mine on the map
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 4; x++) {
                this.grid[y][x] = Mine.generate(random);
                this.positions.add(new Position(x, y));
            }
        }
    }

    /**
     * 【方法功能】获取当前可供购买的矿井列表副本。
     * [Function] Get the mines currently available to purchase.
     * @return 矿井列表的副本。 (a copy of the mines available to purchase.)
     */
    public List<Mine> getMines() {
        List<Mine> copy = new ArrayList<>();
        for (Mine[] row : grid) {
            for (Mine m : row) {
                if (m == null) {
                   m = Mine.generate(random);
                }
                copy.add(m);
            }
        }
        return copy;
    }

    /**
     * 【方法功能】获取指定位置的矿井。
     * [Function] Get the mine at a particular position.
     * * @param position 坐标位置。 (the position of the mine to get.)
     * @return 该位置的矿井或 null。 (the mine at that position, or null if out of bounds.)
     */
    public Mine getMine(Position position) {
        if (!this.positions.contains(position)) {
            return null;
        }
        int x = position.x();
        int y = position.y();
        return this.grid[y][x];
    }

    /**
     * 【方法功能】获取玩家列表的副本。
     * [Function] Get the players playing this game.
     * @return 玩家列表副本。 (a copy of the players.)
     */
    public List<Player> getPlayers() {
        return new ArrayList<>(this.players);
    }

    /**
     * 【方法功能】获取当前轮到的玩家。如果游戏已经结束，则返回游戏的获胜者。
     * [Function] Get the current player (whose turn it is). If the game is over,
     * the current player returned is the winner.
     * * 【主要用途】用于 UI 显示当前操作者，或在游戏结束时获取并展示最终赢家。
     * [Purpose] Used for UI to display the current operator, or to retrieve and
     * showcase the final winner when the game ends.
     *
     * @return 当前玩家对象或获胜者对象。
     * (The player object whose turn it is, or the winner object.)
     */
    public Player getCurrentPlayer() {
        return  this.players.get(this.currentPlayerIndex);
    }

    /**
     * 【方法功能】判断并返回游戏是否已经结束。
     * [Function] Returns whether the game is over based on score and turn conditions.
     * 【主要用途】在每个回合结束时调用，用于验证是否满足终局的三大标准：
     * 1. 胜利点数门槛：至少一名玩家得分 >= 15。
     * 2. 领跑者唯一性：最高分获得者不能有平局。
     * 3. 轮次公平性：当前索引必须为 0，确保每人操作次数相同。
     * [Purpose] Called at the end of each action to verify three end-game criteria:
     * 1. Score Threshold: At least one player has scores >= 15.
     * 2. Leader Uniqueness: No ties for the highest score.
     * 3. Turn Fairness: Current index must be 0, ensuring everyone has had equal turns.
     *
     * @return 如果游戏满足所有结束条件则返回 true，否则返回 false。
     * (Returns true if all conditions are met; otherwise false.)
     */
    public boolean isOver() {
        if (this.currentPlayerIndex != 0) {
            return false;
        }

        int maxScore = -1;
        int winnersCount = 0;

        // 找最高分和统计人数
        for (Player p : this.players) {
            int score = p.getVictoryPoints();
            if (score > maxScore) {
                maxScore = score;
                winnersCount = 1;
            } else if (score == maxScore && maxScore != -1) {
                winnersCount++;
            }
        }

        // 只有 15 分达标 且 没有平局 且 刚好回到玩家 0，才返回 true
        return (maxScore >= 15) && (winnersCount == 1);
    }

    /**
     * 【方法功能】尝试让当前玩家购买位于特定位置的矿井。如果购买成功，
     * 系统会在该位置随机生成并添加一个替代矿井，随后将回合移交给下一位玩家；
     * 若游戏因此结束，则进入游戏结束状态。
     * [Function] Attempts to have the current player purchase the mine available at
     * a particular position. If the purchase was successful, a randomly generated
     * replacement mine is added to the board, then the turn passes to the next player;
     * if the game is over, it moves to the game over state.
     * * 【主要用途】处理玩家购买资产的逻辑、地图资源的动态重置以及游戏状态的流转。
     * [Purpose] Handles the logic for player asset acquisition, dynamic resetting
     * of board resources, and game state transitions.
     *
     * @param position 游戏板上矿井所在的位置。
     * (The position on the game board of the mine.)
     * @return 如果购买成功则返回 true，否则返回 false。
     * (Whether the purchase was successful.)
     */
    public boolean attemptPurchase(Position position) {
        //如何定位矿山，让位置直接返回Mine的信息
        if (!this.positions.contains(position)) {
            return false;
        }
        //前面创建了一个list去存储所有矿山的位置，这里是检测是否这个坐标是否包含在矿山列表里
        //Earlier, we created a list to store the locations of all mines. Here,
        // we check whether these coordinates are included in the list of mines.
        Mine mine = getMine(position);
        Player currentPlayer = getCurrentPlayer();

        //购买逻辑已经检测了是否可以购买，还有买了以后减去折扣
        //The purchase logic has already checked whether the item can be purchased
        // and applied the discount after the purchase.
        if (currentPlayer.purchase(mine)) {
            int x =  position.x();
            int y = position.y();
            this.grid[y][x] = Mine.generate(random);
            //购买结束后，切换到下一个玩家
            //Once the purchase is complete, switch to the next player.
            this.currentPlayerIndex = (this.currentPlayerIndex + 1) % players.size();
            return true;
        }
        return false;
    }

    /**
     * 【方法功能】尝试让当前玩家收集宝石。如果收集成功，回合将移交给下一位玩家；
     * 若游戏因此结束，则进入游戏结束状态。
     * [Function] Attempts to have the current player collect some gems. If successful,
     * the turn passes to the next player; if the game is over, it moves to the game over state.
     * * 【主要用途】处理核心的游戏循环逻辑，包括资源获取、回合交替和终止条件检查。
     * [Purpose] Handles core game loop logic, including resource acquisition,
     * turn alternation, and termination condition checking.
     *
     * @param gems 要收集的宝石池对象。
     * (The gems to collect.)
     * @return 如果宝石成功收集则返回 true，否则返回 false。
     * (Returns whether the gems were successfully collected.)
     */
    public boolean attemptCollectCoins(GemPool gems) {
        //This checks whether the game has ended and whether the gems have run out.
        if (gems == null || isOver()) {
            return false;
        }

        Player currentPlayer = getCurrentPlayer();
        boolean success = currentPlayer.addGems(gems);

        // 2. 只要动作成功，【无脑】切换到下一个人
        // 这样做是为了保证回合的完整性，让 index 回到 0。
        if (success) {
            this.currentPlayerIndex = (this.currentPlayerIndex + 1) % players.size();
        }

        return success;
    }
}

