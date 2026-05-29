package mines.model;

import java.util.EnumMap;
import java.util.HashMap;
import java.util.Map;

/**
 * GemPool 类用于存储和管理游戏中五种宝石的数量。
 * The GemPool class stores and manages the quantities of five gem types in the game.
 * 主要用途 (Main Purpose): 作为玩家仓库或矿坑成本的载体，支持不可变的加减运算。
 */
public class GemPool {
    // 内部存储容器：使用高效的 EnumMap 存储宝石类型及其数量
    // Internal storage: Uses an efficient EnumMap to store GemTypes and their amounts.
    //?
    private Map<GemType, Integer> gems = new EnumMap<>(GemType.class);

    /**
     * 默认构造函数：创建一个所有宝石数量均为 0 的空池。
     * Default constructor: Creates an empty pool with all gem counts set to 0.
     */
    public GemPool() {

        this(0, 0, 0, 0, 0);
    }

    /**
     * 单类型构造函数：创建一个仅包含一种宝石数量的池。
     * Single-type constructor: Creates a pool with a specific amount of one gem type.
     * @param gemType 宝石类型 (The type of gem)
     * @param amount  宝石数量 (The amount of that gem)
     */
    public GemPool(GemType gemType, int amount) {
        // 1. 先调用默认构造函数，确保所有宝石初始为 0
        this();

        // 2. 允许存入任何数值（包括负数），以便通过数学减法测试
        if (gemType != null) {
            this.gems.put(gemType, amount);
        }
    }

    /**
     * 全参构造函数：初始化五种宝石的具体数量。
     * Canonical constructor: Initializes the specific amounts for all five gem types.
     * @param quartz   石英数量 (Amount of Quartz)
     * @param ruby     红宝石数量 (Amount of Ruby)
     * @param sapphire 蓝宝石数量 (Amount of Sapphire)
     * @param emerald  绿宝石数量 (Amount of Emerald)
     * @param onyx     玛瑙数量 (Amount of Onyx)
     */
    public GemPool(int quartz, int ruby, int sapphire, int emerald, int onyx) {
        this.gems = new EnumMap<>(GemType.class);
        this.gems.put(GemType.QUARTZ, quartz);
        this.gems.put(GemType.RUBY, ruby);
        this.gems.put(GemType.SAPPHIRE, sapphire);
        this.gems.put(GemType.EMERALD, emerald);
        this.gems.put(GemType.ONYX, onyx);
    }

    /**
     * 使用 getOrDefault 实现的优化版本
     * Optimized version using getOrDefault
     */
    public int get(GemType gemType) {
        //check the gemType if is null
        if (this.gems.get(gemType) == null) {
            return 0;
        }
        return this.gems.get(gemType);
    }

    /**
     * 检查宝石池是否逻辑为空（即所有宝石数量均 <= 0）。
     * Checks if the pool is logically empty (all gem amounts <= 0).
     * @return 如果没有大于 0 的宝石，返回 true (True if no positive gem amounts exist)
     */
    public boolean isEmpty() {
        //Function: Returns whether there is a negative amount of
        // gems for ANY type of gem in this pool.
        // @Returns:
        //whether there is a negative amount of gems for ANY type of gem in this pool.
        for (int amount : this.gems.values()) {
            if (amount > 0) {
                return false;
            }
        }
        return true;
    }
    /**
     * 检查池中是否存在任何负数数量的宝石。
     * Checks if any gem type in the pool has a negative amount.
     * @return 只要有一种宝石数量 < 0，返回 true (True if any gem count is < 0)
     */
    public boolean hasNegative() {
        for (int amount : this.gems.values()) {
            if (amount < 0) {
                return true;
            }
        }
        //check all the value which do not have negative value.
        return false;
    }
    /**
     * 增加指定数量的某种宝石，并返回新实体。
     * Adds a specific amount of a gem type and returns a new instance.
     * @param gemType 增加的类型 (The type to add)
     * @param amount  增加的数量 (The amount to add)
     * @return 包含增加后结果的新 GemPool (A new GemPool with updated counts)
     */
    public GemPool add(GemType gemType, int amount) {

        return new GemPool(
                this.get(GemType.QUARTZ) + (gemType == GemType.QUARTZ ? amount : 0),
                this.get(GemType.RUBY) + (gemType == GemType.RUBY ? amount : 0),
                this.get(GemType.SAPPHIRE) + (gemType == GemType.SAPPHIRE ? amount : 0),
                this.get(GemType.EMERALD) + (gemType == GemType.EMERALD ? amount : 0),
                this.get(GemType.ONYX) + (gemType == GemType.ONYX ? amount : 0)
        );

    }
    /**
     * 将另一个宝石池的内容合并到当前池中，并返回新实体。
     * Merges another GemPool's contents into a new GemPool instance.
     * @param other 要合并的另一个池 (The other gem pool to merge)
     * @return 合并后的新 GemPool (A new GemPool with the sum of contents)
     */
    public GemPool add(GemPool other) {

        return new GemPool(
                this.get(GemType.QUARTZ) + other.get(GemType.QUARTZ),
                this.get(GemType.RUBY) + other.get(GemType.RUBY),
                this.get(GemType.SAPPHIRE) + other.get(GemType.SAPPHIRE),
                this.get(GemType.EMERALD) + other.get(GemType.EMERALD),
                this.get(GemType.ONYX) + other.get(GemType.ONYX)
        );



    }
    /**
     * 减法运算：允许产生负数。常用于计算购买矿坑还差多少宝石。
     * Subtraction: Allows negative values. Often used to calculate gem deficits for purchases.
     * @param gemType 减去的类型 (The gem type to subtract)
     * @param amount  减去的数量 (The amount to subtract)
     * @return 减法运算后的新 GemPool，可能包含负数 (New GemPool, may contain negatives)
     */
    public GemPool subtract(GemType gemType, int amount) {

        if (gemType != null) {

            return new GemPool(
                    this.get(GemType.QUARTZ) - (gemType == GemType.QUARTZ ? amount : 0),
                    this.get(GemType.RUBY) - (gemType == GemType.RUBY ? amount : 0),
                    this.get(GemType.SAPPHIRE) - (gemType == GemType.SAPPHIRE ? amount : 0),
                    this.get(GemType.EMERALD) - (gemType == GemType.EMERALD ? amount : 0),
                    this.get(GemType.ONYX) - (gemType == GemType.ONYX ? amount : 0)
            );
        }
        return this;
        //if GemPool is null return null
    }
    public GemPool subtract(GemPool other) {
        //试卷批改是否限制每次的消息输出，就像上学期一样？

        //1.检查是否传入的值为空，或者为0
        //1.check if the other's key is null
        if (other == null) {
            return this;
        }

        //2.单纯检查是否为空即可
        return new GemPool(
                this.get(GemType.QUARTZ) - other.get(GemType.QUARTZ),
                this.get(GemType.RUBY) - other.get(GemType.RUBY),
                this.get(GemType.SAPPHIRE) - other.get(GemType.SAPPHIRE),
                this.get(GemType.EMERALD) - other.get(GemType.EMERALD),
                this.get(GemType.ONYX) - other.get(GemType.ONYX)
        );
        //这里不需要else判断，因为返回的值必然>=0,直接返回check_amount就可以了
    }
    /**
     * 移除运算：结果保底为 0。常用于实际扣除玩家手中的宝石。
     * Removal: Clamped to 0. Used for actual deduction from player inventory.
     * @param gemType 移除的类型 (The type to remove)
     * @param amount  移除的数量 (The amount to remove)
     * @return 扣除后的新实体，最小值为 0 (New instance with counts clamped to 0)
     */
    public GemPool remove(GemType gemType, int amount) {
        if (gemType == null) {
            return this;
        }
        if (amount < 0) {
            return this;
        }
        GemPool obj = subtract(gemType, amount);

        return new GemPool(
                Math.max(0, obj.get(GemType.QUARTZ)),
                Math.max(0, obj.get(GemType.RUBY)),
                Math.max(0, obj.get(GemType.SAPPHIRE)),
                Math.max(0, obj.get(GemType.EMERALD)),
                Math.max(0, obj.get(GemType.ONYX))
        );

    }

    /**
     * 批量移除：从当前池中扣除另一个池的内容，结果保底为 0。
     * Batch removal: Deducts another pool's contents, clamped to 0.
     * @param other 包含扣除数量的另一个池 (Other pool containing amounts to deduct)
     * @return 扣除后的新实体 (A new GemPool instance representing the result)
     */
    public GemPool remove(GemPool other) {
        //试卷批改是否限制每次的消息输出，就像上学期一样？

        //检查是否传入的值为空，或者为0
        //check if the other's key is null
        if (other == null) {
            return this;
        }

        //2.计算结束后，直接判断是否为负数，为负数，直接设置为0
        //prevent here have negative number
        return new GemPool(

                Math.max(0, this.get(GemType.QUARTZ) - Math.max(0, other.get(GemType.QUARTZ))),
                Math.max(0, this.get(GemType.RUBY) - Math.max(0, other.get(GemType.RUBY))),
                Math.max(0, this.get(GemType.SAPPHIRE) - Math.max(0, other.get(GemType.SAPPHIRE))),
                Math.max(0, this.get(GemType.EMERALD) - Math.max(0, other.get(GemType.EMERALD))),
                Math.max(0, this.get(GemType.ONYX) - Math.max(0, other.get(GemType.ONYX)))
        );

    }


    /**
     * 方法功能 (Method Function):
     * 生成当前宝石池的哈希码 (数字指纹)。
     * * 主要用途 (Main Purpose):
     * 支持 Java 的哈希容器 (如 HashMap/HashSet)。确保“逻辑相等”的对象拥有相同的哈希值。
     * * 返回值 (Returns):
     * @return 基于内部宝石数量计算出的唯一整数值 (Unique integer based on gem counts).
     */
    @Override
    public int hashCode() {
        // java.util.Objects.hash 会自动读取 Map 里的内容，并计算出一个混合指纹。
        // This utility method computes a single hash value based on the Map's key-value pairs.
        return java.util.Objects.hash(this.gems);
    }

    /**
     * 判断两个宝石池是否完全一致。
     * Compares this GemPool with the specified object for equality.
     * @param obj 要对比的另一个对象 (The object to be compared for equality).
     * @return 如果两个池子包含相同数量的每种宝石，返回 true (true if identical).
     */
    @Override
    public boolean equals(Object obj) {

        // 1. 基础地址检查：如果是同一个内存地址，肯定相等
        // Check for reference equality: if they share the same address, they are identical.
        if (this == obj) {
            return true;
        }

        // 2. 基础合法性检查：如果对比的对象是 null，或者类不一致，肯定不相等
        // Check for null or class mismatch: ensure the object is a GemPool instance.
        if (obj == null) {
            return false;
        }

        //check whether is come from same class
        if (this.getClass() != obj.getClass()) {
            return false;
        }
        //if(obj.instanceof GemPool obj)
        // 3. 强制类型转换，准备对比内部数据
        // Cast the object to GemPool to access its gem data.
        GemPool other = (GemPool) obj;

        // 4. 深度遍历对比：利用枚举类遍历，确保每一种宝石的数量都对齐
        // Iterate through all GemType values to verify that each count matches.
        for (GemType gemType : GemType.values()) {
            // 这里会调用你之前写的 get() 方法，它会处理好默认值 0
            // Uses the get() method to compare counts, handling missing keys as 0.
            if (this.get(gemType) != other.get(gemType)) {
                return false;
            }
        }
        // 5. 只有所有检查都通过，才返回 true
        // If all checks pass, the two pools are logically identical.
        return true;
    }

    /**
     * 将宝石池转换为人类可读的字符串格式。
     * Returns a string representation of this GemPool in the form: "VQ WR XS YE ZO" where V, W, X, Y, Z are replaced
     * with the number of QUARTZ, RUBY, SAPPHIRE, EMERALD, and ONYX respectively. Additionally, any value which is less
     * than or equal to 0 will not appear in the final string. for example: a pool containing 5 QUARTZ, -3 RUBY, and 2
     * SAPPHIRE will result in: "5Q 2S"
     * * 格式要求 (Format): "VQ WR XS YE ZO"
     * 规则 (Rules): 忽略数量 <= 0 的宝石。严格按 Q, R, S, E, O 顺序。
     * * @return 格式化后的字符串，例如 "5Q 2S" (A formatted string).
     * a string representation of this GemPool.
     */
    @Override
    public String toString() {
        // 使用 StringBuilder 高效构建字符串
        // Use StringBuilder to construct the string efficiently.
        StringBuilder result = new StringBuilder();

        // 1. 检查石英 (check Quartz)
        int q = this.get(GemType.QUARTZ);
        if (q > 0) {
            result.append(q).append("Q ");
        }

        // 2. 检查红宝石 (check Ruby)
        int r = this.get(GemType.RUBY);
        if (r > 0) {
            result.append(r).append("R ");
        }

        // 3. 检查蓝宝石 (check Sapphire)
        int s = this.get(GemType.SAPPHIRE);
        if (s > 0) {
            result.append(s).append("S ");
        }

        // 4. 检查绿宝石 (check Emerald)
        int e = this.get(GemType.EMERALD);
        if (e > 0) {
            result.append(e).append("E ");
        }

        // 5. 检查玛瑙 (check Onyx)
        int o = this.get(GemType.ONYX);
        if (o > 0) {
            result.append(o).append("O ");
        }

        // trim() 方法的作用是切掉字符串首尾多余的空格
        // The trim() method removes any leading or trailing whitespace (like the extra space at the end).
        return result.toString().trim();
    }
}











