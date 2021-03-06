---
layout: post
title:  Dynamic Programming之背包DP
date:   2020-10-22
author: hding
catalog: true
tags:
   - algorithm
---
# 背包DP

## [Template](https://github.com/hldingzydong/CS61B/blob/master/Knapsack/Knapsack.java)

## [01背包](https://www.kancloud.cn/kancloud/pack/70125)
#### [1049. Last Stone Weight II](https://leetcode.com/problems/last-stone-weight-ii/)
`Feature`: 转换为01背包问题, 选择子集使其之和无限接近总和(sum)的一半,此时剩余的石头的重量(sum - 2\*maxHalfSum)最小
```java
boolean[] dp = new boolean[sum/2+1]; // true代表存在子集使得子集的总重量恰好为i, false则代表不存在
dp[0] = true;   // 当 j == stones[i] 时,即只放该物品可填满背包,故为true

int maxHalfSum = 0;
for(int i = 0; i < stones.length; i++) {
    // 01背包 - 倒序
    for(int j = target; j >= stones[i]; j--) {
        dp[j] = dp[j] || dp[j-stones[i]];
        if(dp[j]) {
            maxHalfSum = Math.max(maxHalfSum, j);
        }
    }
}
```

#### [416. Partition Equal Subset Sum](https://leetcode.com/problems/partition-equal-subset-sum/)
`Feature`: 01背包,要求恰好装满背包
```java
boolean[] dp = new boolean[target+1]; // true代表存在子集使得子集的总重量恰好为i, false则代表不存在
dp[0] = true;
        
for(int i = 0; i < nums.length; i++) {
    for(int j = target; j >= nums[i]; j--) {
        dp[j] = dp[j] || dp[j - nums[i]];
    }
}

return dp[target];
```

#### [879. Profitable Schemes](https://leetcode.com/problems/profitable-schemes/)
`Feature`: 01背包,但是多了一个条件是利润不得低于P,因此多引入了一个状态变量
```java
// question method
int profitableSchemes(int G, int P, int[] group, int[] profit);

int[][][] dp = new int[group.length][G+1][P+1];
// base case, 只有第一项crime可供选择
for(int j = 0; j <= G; j++) {
    for(int k = 0; k <= P; k++) {
        if(k == 0) {
            // 因为至少产生0利润,那么只要人数够,可以有两种选择,选或者不选([0], [])，否则只能不选
            dp[0][j][k] = group[0] <= j ? 2 : 1;
        } else {
            // 因为至少要产生k利润,那么只有选([0]),或者没有决策(0)
            dp[0][j][k] = group[0] <= j && profit[0] >= k ? 1 : 0;
        }
    }
}

// iterate
for(int i = 1; i < group.length; i++) {
    for(int j = 0; j <= G; j++) {
        for(int k = 0; k <= P; k++) {
            dp[i][j][k] = dp[i-1][j][k]; // 不选
            if(j >= group[i]) { // 选
                dp[i][j][k] += dp[i-1][j-group[i]][k-profit[i]];
            } else {
                dp[i][j][k] += dp[i-1][j-group[i]][0];
            }
        }
    }
}
```

## [完全背包](https://www.kancloud.cn/kancloud/pack/70126)
#### [279. Perfect Squares](https://leetcode.com/problems/perfect-squares/)
`Feature`: 完全背包, 每个candidate可以选无限次(包括0次)
```java
class Solution {
    public int numSquares(int n) {
        int floor = (int)Math.sqrt(n);
        if(floor * floor == n) {
            return 1;
        }
        int[] squares = new int[floor+1];
        for(int i = 0; i < squares.length; i++) {
            squares[i] = i * i;
        }

        int[] dp = new int[n+1];
        Arrays.fill(dp, Integer.MAX_VALUE);
        dp[0] = 0;
        dp[1] = 1;
        for(int i = 2; i <= n; i++) {
            for(int j = 1; j <= floor; j++) {
                if(squares[j] > i) {
                    break;
                } else {
                    dp[i] = Math.min(dp[i], 1 + dp[i - squares[j]]);
                }
            }
        }
        return dp[n];
    }
}
```

#### [322. Coin Change](https://leetcode.com/problems/coin-change/)
`Feature`: 完全背包,每个硬币可以选无限次(包括0次)
```java
int[] dp = new int[amount + 1];
Arrays.fill(dp, Integer.MAX_VALUE);
dp[0] = 0;	// base case, 由于dp[i]代表和恰好为i的最少方案数,也可能没有方案,因此最初除了0之外其他都是没有方案,初始化为Integer.MAX_VALUE,代表没有方案
        
for(int i = 1; i <= amount; i++) {
    for(int j = 0; j < coins.length; j++) {
        if(i >= coins[j] && dp[i - coins[j]] != Integer.MAX_VALUE) {
            dp[i] = Math.min(dp[i], dp[i - coins[j]] + 1);
        }
    }
}

return dp[amount] == Integer.MAX_VALUE ? -1 : dp[amount];
```

#### [377. Combination Sum IV](https://leetcode.com/problems/combination-sum-iv/)
`Feature`: 同上,可选无数次
```java
class Solution {
    public int combinationSum4(int[] nums, int target) {
        Arrays.sort(nums);
        
        int[] dp = new int[target+1];
        dp[0] = 1;
        for(int i = 1; i < dp.length; i++) {
            for(int j = 0; j < nums.length; j++) {
                if(nums[j] > i) {
                    break;
                }
                dp[i] = dp[i] + dp[i - nums[j]];
            }
        }
        return dp[target];
    }
}
```

###### Follow-up
Q: What if negative numbers are allowed in the given array?  
A: The problem with negative numbers is that now the combinations could be potentially of infinite length. Think about `nums = [-1, 1]` and `target = 1`. We can have all sequences of arbitrary length that follow the patterns `-1, 1, -1, 1, ..., -1, 1, 1` and `1, -1, 1, -1, ..., 1, -1, 1` (there are also others, of course, just to give an example). So we should limit the length of the combination sequence, so as to give a bound to the problem.



#### [518. Coin Change 2](https://leetcode.com/problems/coin-change-2/)
`Feature`: 同上,但是求方案数,参考[背包九讲中求方案总数](https://www.kancloud.cn/kancloud/pack/70133)
```java
// dp[i][j] - 代表从前i种类型的硬币中pick,使得总和为j的方案数
// dp[i-1][j] - 代表不选择当前这种类型
// dp[i][j-coins[i]] - 代表选择当前类型后，再接着从当前类型选,只是target的sum减少了
dp[i][j] = dp[i-1][j] + dp[i][j-coins[i]];

// optimal 完全背包 - 正序
int[] dp = new int[amount+1];
dp[0] = 1;   // 什么都不选择使得总和为0,仅有该一个方案,也是为了处理
             // dp[j] = dp[j] + 1, when j == coins[i]
for(int i = 0; i < coins.length; i++) {
   for(int j = 1; j <= amount; j++) {
       dp[j] = dp[j] + (j >= coins[i] ? dp[j-coins[i]] : 0);
   }
}
return dp[amount];
```

## [二维背包](https://www.kancloud.cn/kancloud/pack/70129)
#### [474. Ones and Zeroes](https://leetcode.com/problems/ones-and-zeroes/)
`Feature`: 多一层循环
```java
for(int i = 1; i < strs.length; i++) {
    for(int j = 0; j <= len; j++) {
        for(int k = 0; k <= len; k++) {
            if(zero_count <= j && one_count <= k) {
                dp[i][j][k] = Math.max(dp[i-1][j][k], dp[i-1][j-zero_count][k-one_count] + 1);
            } else {
                dp[i][j][k] = dp[i-1][j][k];
            }
        }
    }
}
```

## [背包问题问法变化](https://www.kancloud.cn/kancloud/pack/70133)
#### 完全背包-求方案总数
```python
f[0][0] = 1
f[i][v] = sum{f[i-1][v], f[i][v-c[i]]}
```

#### 01背包-求最优方案总数
```python
for i = 1..N
   for v = 0..V
        f[i][v] = max{f[i-1][v],f[i-1][v-c[i]]+w[i]}
        g[i][v] = 0

        if(f[i][v] == f[i-1][v])
            inc(g[i][v], g[i-1][v])

        if(f[i][v] == f[i-1][v-c[i]]+w[i])
            inc(g[i][v], g[i-1][v-c[i]])
```