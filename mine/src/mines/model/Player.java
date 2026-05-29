package mines.model;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * 玩家类，存储玩家的资源和积分信息。
 * Player class, storing player resources and score information.
 */
public class Player {
    private final int playNumber;
    private int point;
    private GemPool gems;
    private final List<Mine> mines;
    private int victoryPoints;

    /**
     * 【方法功能】构造函数：初始化一个新玩家。
     * [Function] Constructor: Initialise a new player.
     * * 【主要用途】在游戏初始化阶段（Game Setup），根据游戏人数创建对应的玩家实例。
     * 初始状态：0 宝石、0 矿井、0 胜利点数。
     * [Purpose] During the Game Setup phase, creates player instances based on
     * the number of players. Starting state: 0 gems, 0 mines, 0 victory points.
     *
     * @param playNumber 玩家在游戏中的顺序编号 (0-3)。
     * (The player's number in play order (0-3).)
     */
    public Player(int playNumber) {
        this.playNumber = playNumber;
        this.gems = new GemPool(0, 0, 0, 0, 0);
        this.mines = new ArrayList<>();
        this.victoryPoints = 0;
    }

    /**
     * 【方法功能】尝试让该玩家购买一座矿井。
     * [Function] Attempt to have this player purchase a mine.
     * * 【主要用途】实现带折扣的购买逻辑：
     * 1. 折扣计算：玩家每拥有一座某种类型的矿井，购买新矿井时该类型的宝石需求就减少 1 个。
     * 2. 费用扣除：应用折扣后的剩余费用从玩家当前的宝石库存中扣除。
     * 3. 资产更新：若购买成功，将矿井加入收藏并增加玩家的胜利点数。
     * [Purpose] Implements purchase logic with discounts:
     * 1. Discount Calculation: Each mine already owned reduces the cost by 1 of its associated gem type.
     * 2. Cost Removal: After discount, remaining cost is removed from the player's gems.
     * 3. Asset Update: On success, adds the mine to collection and adds its victory points to the player.
     *
     * @param mine 要购买的矿井对象 (The mine the player will purchase).
     * @return 如果无法支付费用则返回 false，购买成功则返回 true。
     * (Returns false if the mine could not be purchased, true otherwise.)
     */
    public boolean purchase(Mine mine) {
            if (mine == null) {
                return false;
            }

            // 1. 获取折扣：现有的矿坑产出即为折扣 (Get discount from existing mines)
            GemPool discount = this.getMines();

            // 2. 计算实际需要支付的宝石数量 (Calculate remaining cost)
            // 利用 remove 方法，它会自动把减法结果保底为 0
            GemPool remainingCost = mine.cost().remove(discount);
            // 3. 检查玩家是否付得起剩余成本 (Check if player can afford it)
            // 用手中的宝石减去剩余成本，如果有任何一种宝石变成负数，则说明付不起
            //if player's gems substract the mine price is negative number. meaning cannot afford.
            if (this.gems.subtract(remainingCost).hasNegative()) {
                return false; // 买不起，不执行任何扣除
            }

            // 4. 真正执行购买操作 (Execute purchase)
            // 4. Execute the purchase
            // A. 从当前宝石中扣除费用
            // A. Deduct the cost from the current gems
            this.gems = this.gems.subtract(remainingCost);

            // B. 将矿坑加入收藏列表
            // B. Add the mine to the favorites list
            this.mines.add(mine);

            // C. 累加玩家的总分数 (假设 Player 类有一个 victoryPoints 字段)
            // C. Calculate the player's total score (assuming the Player class has a victoryPoints field)
            this.victoryPoints += mine.victoryPoints();
            return true;
    }

    /**
     * 【方法功能】尝试向玩家当前的宝石收藏中添加一组宝石。
     * [Function] Attempt to add a pool of gems to the player's current gem collection.
     * * 【主要用途】强制执行收集规则：玩家只能添加 3 个不同种类的宝石，或者 2 个同种类的宝石。
     * 若不符合这两种形式，添加将失败。
     * [Purpose] Enforces collection rules: a player may add 3 gems each of separate types,
     * or 2 gems of the same type. If the collection is of any other form, this method will fail.
     *
     * @param toCollect 玩家将要收集的新宝石池对象。
     * (The new gems this player will collect.)
     * @return 如果符合规则且成功收集则返回 true，否则返回 false。
     * (Whether the gems were successfully collected.)
     */
    public boolean addGems(GemPool toCollect) {

        if ( toCollect == null) {
            return false;
        }
        //Check if GemPool is null
        if ( toCollect.isEmpty()) {
            return false;
        }
        int totalGems = 0;
        int typeCount = 0;
        for (GemType type : GemType.values()) {
            int count = toCollect.get(type);
            if (count > 0) {
                totalGems += count;
                typeCount++;
            }
        }

        // 验证规则 (Validate rules)
        boolean valid = false;
        if (typeCount == 3 && totalGems == 3) {
            // 情况一：3 种不同宝石，每种各 1 颗
            valid = true;
        } else if (typeCount == 1 && totalGems == 2) {
            // 情况二：1 种宝石，共 2 颗
            valid = true;
        }

        if (valid) {
            // 重新赋值以维护不可变性 (Reassign to maintain immutability)
            this.gems = this.gems.add(toCollect);
            return true;
        }
        return false;
    }

    /**
     * 【方法功能】获取该玩家当前收集的胜利点数总和。
     * [Function] Get the total number of victory points this player has collected.
     * * 【主要用途】用于计算排名、检查是否达到终局条件（如 15 分）以及在游戏结束时判定获胜者。
     * [Purpose] Used to calculate rankings, check if endgame conditions are met
     * (e.g., 15 points), and determine the winner when the game concludes.
     *
     * @return 玩家收集的胜利点数。
     * (The number of victory points this player has collected.)
     */
    public int getVictoryPoints() {
        int total = 0;
        for (Mine m : mines) {
             total += m.victoryPoints();
        }
        return total;
    }

    /**
     * 【方法功能】获取该玩家的编号（按游戏行动顺序）。
     * [Function] Get this player's number (in play order).
     * * @return 玩家的编号。 (this player's number.)
     */
    public int getPlayerNumber() {
        return this.playNumber;
    }

    /**
     * 【方法功能】获取该玩家当前收集的所有宝石。
     * [Function] Get this player's currently collected gems.
     * * @return 代表玩家宝石库存的 GemPool 对象。 (this player's gems.)
     */
    public GemPool getGems() {
        return this.gems;
    }

    /**
     * 【方法功能】获取该玩家当前拥有的矿井，并以 GemPool 形式返回。
     * [Function] Get this player's currently owned mines as a pool of GemTypes.
     * * 【主要用途】将玩家拥有的 Mine 列表转换成一个 GemPool，其中每个 GemType 的数量
     * 代表玩家拥有的该类型矿井的总数。
     * [Purpose] Converts the player's list of mines into a GemPool, where the
     * quantity of each GemType represents the total number of mines of that type owned.
     *
     * @return 代表矿井数量分布的 GemPool。 (this player's mines as a pool of GemTypes.)
     */
    public GemPool getMines() {
        // 1. 创建一个初始为全 0 的宝石池 (Start with an empty pool of all 0s)
        GemPool production = new GemPool();

        // 2. 遍历玩家手中的每一个矿坑 (Iterate through every mine owned)
        for (Mine mine : this.mines) {
            // 3. 获取矿坑的产出类型 (Get the gem type produced by this mine)
            GemType type = mine.gemType();

            // 4. 利用 GemPool 的 add 方法增加 1 颗对应宝石
            // 使用不可变对象模式，必须将返回的新对象重新赋值
            // Add 1 to the count for this gem type. Must reassign since GemPool is immutable.
            production = production.add(type, 1);
        }
        return production;
    }
    //form:QUARTZ [M] (G), RUBY [M] (G), SAPPHIRE [M] (G), EMERALD [M] (G), ONYX [M] (G)

    /**
     * 【方法功能】返回代表该玩家状态的字符串。
     * [Function] Return a string to represent this player.
     * * 【主要用途】以特定格式展示玩家资产: "宝石类型 [矿井数] (宝石数)"。
     * [Purpose] Displays player assets in a specific format: "GemType [M] (G)".
     *
     * @return 格式化后的玩家信息字符串。 (a string to represent this player.)
     */
    @Override
    public String toString() {
        GemPool minPool = getMines();
        GemPool gemPool = getGems();
        GemType[] types = {
            GemType.QUARTZ, GemType.RUBY, GemType.SAPPHIRE, GemType.EMERALD, GemType.ONYX
        };
        StringBuilder sb = new StringBuilder();
        for(int i =0; i < types.length; i++) {
            GemType type = types[i];
            int m = minPool.get(type);
            int g = gemPool.get(type);

            sb.append(type.name())
                    .append(" [").append(m).append("] ")
                    .append("(").append(g).append(")");

            // 如果不是最后一个元素，添加逗号和空格
            // Add a comma and space if it's not the last element
            if (i < types.length - 1) {
                sb.append(", ");
            }
        }
            return sb.toString();
    }

}
