package mines.model;
import java.util.Random;
import java.util.Objects;
/**
 * Function:Creates an instance of a Mine record class.
 * Record Components:
 * gemType - The type of gem this mine produces.
 * victoryPoints - The number of victory points purchasing this mine awards.
 * cost - The cost in gems of this mine.
 */
public record Mine(GemType gemType, int victoryPoints, GemPool cost) {
    //这个cost参数包含多个宝石，所以是一个价格表
    //The `cost` parameter contains multiple gems, so it is a price list

    /**
     * 【方法功能】从随机源生成一个新的矿井实例。
     * [Function] Generate a new mine from a source of randomness.
     * * 【主要用途】用于在游戏初始化或玩家购买矿井后，在地图上补充新的随机资源。
     * 特性：满足确定性，相同的随机种子将产生完全相同的矿井。
     * [Purpose] Used to replenish random resources on the board during setup or
     * after a purchase. Feature: Deterministic; same seed produces identical results.
     *
     * @param random 用于生成矿井的随机源 (The source of randomness to use).
     * @return 一个符合随机约束的新矿井实例 (A new random Mine).
     */
    public static Mine generate(Random random) {
        GemType type = GemType.values()[random.nextInt(GemType.values().length)];
        //setting the point of the mine(random)
        int victoryPoint = random.nextInt(4);
        int minCost =2 + victoryPoint;
        int maxCost =2 * victoryPoint + 2;
        int totalCost = random.nextInt(maxCost - minCost + 1) + minCost;

        // 分配成本 (Distribute Cost)
        // 技巧：随机循环 totalCost 次，每次随机选一种宝石加 1
        // Tip: Iterate through totalCost times in a random order, randomly selecting one gem each time and adding 1 to its cost
        int[] counts = new int[5];
        for (int i = 0; i < totalCost; i++) {
            counts[random.nextInt(5)]++;
        }

        GemPool costPool = new GemPool(counts[0], counts[1], counts[2], counts[3], counts[4]);
        return new Mine(type, victoryPoint, costPool);
    }

    //"V|TYPE COST"
    //eg: 3|QUART Q: 2, R: 3
    /**
     * 【方法功能】返回代表该矿井的格式化字符串。
     * [Function] Returns a formatted string representing this mine.
     * * 【主要用途】用于 UI 显示或控制台日志，格式固定为 24 字符宽： "V|TYPE          COST"。
     * [Purpose] Used for UI display or console logging, with a fixed width of 24 characters: "V|TYPE          COST".
     *
     * @return 长度为 24 的格式化字符串。 (A string of length 24 representing this mine.)
     */
    @Override
    public String toString() {
    String prefix = victoryPoints + "|" + this.gemType;
    String cost_fix = this.cost.toString();
        //Using GemPool‘s toString（）
        //output form： "1Q 1R 1S 1E 1O"

    int spacenumber = 24 - prefix.length()-cost_fix.length();
    String midspace = " ".repeat(Math.max(0,spacenumber));
    return prefix + midspace + cost_fix;
    }

    /**
     * 【方法功能】返回此矿井对象的哈希码值。
     * [Function] Returns a hash code value for this mine object.
     * * 【主要用途】该值派生自每个记录组件（gemType, victoryPoints, cost）的哈希码。
     * 它是支持哈希表（如 HashMap 或 HashSet）高效运行的基础。
     * [Purpose] The value is derived from the hash code of each record component.
     * It is the foundation for the efficient operation of hash tables (e.g., HashMap or HashSet).
     *
     * @return 此对象的哈希码值。 (A hash code value for this object.)
     */
    @Override
    public final int hashCode() {
        // 它会把所有零件“搅拌”在一起，算出一个唯一的数字
        // It mixes all components together to calculate a unique number.
        return Objects.hash(gemType, victoryPoints, cost);
    }

    /**
     * 【方法功能】指示某个其他对象是否与此对象“相等”。
     * [Function] Indicates whether some other object is "equal to" this one.
     * * 【主要用途】用于集合操作（如 List.contains）或逻辑判断。
     * 只有当另一个对象也是 Mine 类，且所有组件（gemType, victoryPoints, cost）都相等时，才返回 true。
     * [Purpose] Used for collection operations or logical checks. Returns true only if
     * the other object is of the same Mine class and all components are equal.
     *
     * @param o 要与之比较的参考对象 (The object with which to compare).
     * @return 如果此对象与参数 o 相同则返回 true，否则返回 false。
     * (true if this object is the same as the o argument; false otherwise.)
     */
    @Override
    public final boolean equals(Object o) {
        // 检查是否为同一个地址 (Identity check)
        if (this == o) {
            return true;
        }

        // 检查空值和类是否一致 (Null and class check)
        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        // Casting type
        // 这样你才能访问到对方的 victoryPoints, gemType 和 cost
        Mine other = (Mine) o;

        // 根据文档进行“全组件对比” (Component-wise comparison)
        return this.victoryPoints == other.victoryPoints &&        // int 用 == (Primitive)
                Objects.equals(this.gemType, other.gemType) &&      // Enum 用 Objects.equals (Reference)
                Objects.equals(this.cost, other.cost);              // GemPool 用 Objects.equals (Reference)
    }

    /**
     * 【方法功能】返回 victoryPoints 记录组件的值。
     * [Function] Returns the value of the victoryPoints record component.
     * * @return 购买此矿井可获得的胜利点数。 (the value of the victoryPoints record component.)
     */
    @Override
    public int victoryPoints(){

        return victoryPoints;
    }

    /**
     * 【方法功能】返回 cost 记录组件的值。
     * [Function] Returns the value of the cost record component.
     * * @return 购买此矿井所需的宝石成本池。 (the value of the cost record component.)
     */
    @Override
    public GemPool cost() {
        return cost;
    }
}

