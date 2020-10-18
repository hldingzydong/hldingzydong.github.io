---
layout: post
title:  Dynamic Programming之线性DP(三)
date:   2020-10-17
author: hding
catalog: true
tags:
   - algorithm
---
# 线性DP

## Part I - 股票系列
`Key`: 由于股票可选择买/卖/什么也不做, 因此应当保存每个位置的状态(`持有/不持有`股票),同时受到`卖股票时必须持有股票,买股票时不可持有股票`的限制
```java
int[] prices // input

dp[i][j]     // i表示位置, j表示持有股票的状态(0为不持有,1为持有)

maxProfit    // output,最大利润
```
#### [122. Best Time to Buy and Sell Stock II](https://leetcode.com/problems/best-time-to-buy-and-sell-stock-ii/)
`Feature`: 交易次数无限
```java
for(int i = 1; i < prices.length; i++) {
    dp[i][0] = Math.max(dp[i-1][0], dp[i-1][1] + prices[i]);
    dp[i][1] = Math.max(dp[i-1][1], dp[i-1][0] - prices[i]);
}

```
Optimal Space:
```java
for(int i = 1; i < prices.length; i++) {
    currNotHold = Math.max(prevNotHold, prevHold + prices[i]);
    currHold = Math.max(prevHold, prevNotHold - prices[i]);
            
    prevNotHold = currNotHold;
    prevHold = currHold;
}
```

#### [714. Best Time to Buy and Sell Stock with Transaction Fee](https://leetcode.com/problems/best-time-to-buy-and-sell-stock-with-transaction-fee/)
`Feature`: 交易次数无限但每次交易需要支付fee
```java
dp[i][0] = Math.max(dp[i-1][0], dp[i-1][1] + prices[i] - fee); // 每次交易时需要支付fee
dp[i][1] = Math.max(dp[i-1][1], dp[i-1][0] - prices[i]);
```

#### [309. Best Time to Buy and Sell Stock with Cooldown](https://leetcode.com/problems/best-time-to-buy-and-sell-stock-with-cooldown/)
`Feature`: 在每次卖出股票后第二天无法购买
```java
for(int i = 2; i < prices.length; i++) {
    dp[i][0] = Math.max(dp[i-1][0], dp[i-1][1] + prices[i]);
    dp[i][1] = Math.max(dp[i-1][1], dp[i-2][0] - prices[i]); // 由于冷冻期存在,每次购买股票时一定是从 前天 不持有股票的状态下购买,因为前天有可能卖出了股票
}
```

#### [123. Best Time to Buy and Sell Stock III](https://leetcode.com/problems/best-time-to-buy-and-sell-stock-iii/)
`Feature`: 最多成交两次
```java
sellOnce[i][1] = Math.max(sellOnce[i-1][1], -prices[i]);   // 由于这是第一次交易，即在此之前未进行过交易,因此为-prices[i]
sellOnce[i][0] = Math.max(sellOnce[i-1][0], sellOnce[i-1][1] + prices[i]);
            
sellTwice[i][1] = Math.max(sellTwice[i-1][1], sellOnce[i-1][0] - prices[i]); // 与上同理
sellTwice[i][0] = Math.max(sellTwice[i-1][0], sellTwice[i-1][1] + prices[i]);
```

#### [188. Best Time to Buy and Sell Stock IV](https://leetcode.com/problems/best-time-to-buy-and-sell-stock-iv/)
`Feature`: 最多成交k次
```java
// dp[i][j][k]: i为位置,j为状态,k为第k次交易
for(int kk = 1; kk < k; kk++) {
    for(int i = 1; i < prices.length; i++) {
        if(i == kk*2) {
            dp[i][1][kk] = dp[i-1][0][kk-1] - prices[i];
            dp[i][0][kk] = Integer.MIN_VALUE;
        } else if (i > kk*2) {
            dp[i][1][kk] = Math.max(dp[i-1][1][kk], dp[i-1][0][kk-1] - prices[i]);
            dp[i][0][kk] = Math.max(dp[i-1][0][kk], dp[i-1][1][kk] + prices[i]);
        }
   }
}
```



## Part II - Matrix 
```java
int[][] matrix  // input

dp[i][j]        // i,j表示位置
```
#### [120. Triangle](https://leetcode.com/problems/triangle/)
```java
List<List<Integer>> triangle // input

for(int i = 1; i < row; i++) {
    List<Integer> currValues = triangle.get(i);
    for(int j = 0; j < currValues.size(); j++) {
        if(j == 0) {
            dp[i][j] = dp[i-1][j] + currValues.get(j);
        } else if (j == currValues.size() - 1) {
            dp[i][j] = dp[i-1][j-1] + currValues.get(j);
        } else {
            dp[i][j] = Math.min(dp[i-1][j-1], dp[i-1][j]) + currValues.get(j);
        }
    }
}
```

#### [64. Minimum Path Sum](https://leetcode.com/problems/minimum-path-sum/)
```java
int[][] grid // input
dp[i][j] = Math.min(dp[i-1][j], dp[i][j-1]) + grid[i][j]; // formula
```

#### [931. Minimum Falling Path Sum](https://leetcode.com/problems/minimum-falling-path-sum/)
```java
int[][] A // input
for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
        if(i == 0) {
            dp[i][j] = A[i][j];
        } else if (j == 0){
            dp[i][j] = Math.min(dp[i-1][j], dp[i-1][j+1]) + A[i][j];
        } else if (j == N-1) {
            dp[i][j] = Math.min(dp[i-1][j-1], dp[i-1][j]) + A[i][j];
        } else {
            dp[i][j] = Math.min(Math.min(dp[i-1][j-1], dp[i-1][j]), dp[i-1][j+1]) + A[i][j];
        }
    }
}
```
#### [1289. Minimum Falling Path Sum II](https://leetcode.com/problems/minimum-falling-path-sum-ii/)
与above不同之处在于,每次扫描需要计算:
```java
minValue
minValueIndex // 用于判断是否处于同一列

secondMinValue
secondMinValueIndex
```

#### [221. Maximal Square](https://leetcode.com/problems/maximal-square/)
```java
b[i][j] = Math.min(Math.min(b[i][j-1] , b[i-1][j-1]), b[i-1][j]) + 1;
result = Math.max(b[i][j], result); // update result
```

#### [174. Dungeon Game](https://leetcode.com/problems/dungeon-game/)
`Feature`: 倒推DP
```java
int[][] dp = new int[row+1][col+1];
for(int i = 0; i < dp.length; i++) {
    Arrays.fill(dp[i], Integer.MAX_VALUE);
}
dp[row][col-1] = 1;
dp[row-1][col] = 1;

for(int i = row-1; i > -1; i--) {
    for(int j = col-1; j > -1; j--) {
        int hp = Math.min(dp[i+1][j], dp[i][j+1]) - dungeon[i][j];
        dp[i][j] = hp > 0 ? hp : 1;
    }
}
```