---
layout: post
title:  Dynamic Programming之数位DP
date:   2020-11-07
author: hding
catalog: true
tags:
   - algorithm
---
# 数位DP

## Description
数位DP往往所求解的问题是,在某一区间\[L,R\]上,满足某些条件的数字的个数,这些条件往往与数字的大小无关,而与数字的组成有关;还有一种情况是求该数字的贡献量,如该数字中1的个数。

## Core
参见该 [博文](https://chengzhaoxi.xyz/13990.html) 中状态的分类描述。注意数位DP与暴力枚举的区别.

## 状态设计
```java
dp[pos][lim][zero][state] // pos:数位的位置 lim:该pos是否受到了限制,决定了该pos的取值范围
			  // zero:是否前导0 state:状态
```
其中前导0与状态可选,而pos和lim往往是必需的.

#### [902. Numbers At Most N Given Digit Set](https://leetcode.com/problems/numbers-at-most-n-given-digit-set/)
`Feature #1`: 状态中不含有前导0
```java
class Solution {
    private int dfs(int pos, int limit, Integer[] num, int[] digitsInt, int[][] dp) {
        if(pos == -1) {
            return 1;
        }
        
        // memorization
        if(dp[pos][limit] > 0) {
            return dp[pos][limit];
        }
        // 根据limit选取上界
        int upper = limit > 0 ? num[pos] : 9;
        int res = 0;
        for(int i : digitsInt) {
            if(i == upper) {
            	// 当前pos处于上界时，下一位有可能被限制
                res += dfs(pos-1, limit, num, digitsInt, dp);
            } else if (i < upper) {
                res += dfs(pos-1, 0, num, digitsInt, dp);
            }
        }
        dp[pos][limit] = res;
        return res;
    }
    
    public int atMostNGivenDigitSet(String[] digits, int n) {
        List<Integer> list = new ArrayList<>();
        while(n > 0) {
            list.add(n%10);
            n = n/10;
        }

        Integer[] num = list.toArray(new Integer[0]);
        int[][] dp = new int[list.size()][2];
        int[] digitsInt = new int[digits.length];
        for(int i = 0; i < digits.length; i++) {
            digitsInt[i] = Integer.parseInt(digits[i]);
        }
        
        int res = dfs(num.length-1, 1, num, digitsInt, dp);
        // 处理前导0
        for(int i = 1; i < num.length; i++) {
            res += dfs(i-1, 0, num, digitsInt, dp);
        }
        return res;
    }
}
```

`Feature #2`: 状态中含有前导0
```java
class Solution {
    private int dfs(int[][][] dp, int pos, int limit, Integer[] num, int[] digitsInt, int prevZero) {
        if(pos == -1) {
            return 1;
        }
        
        if(dp[pos][limit][prevZero] > 0) {
            return dp[pos][limit][prevZero];
        }

        int upper = limit > 0 ? num[pos] : 9;
        int res = 0;
        // 处理前导0,此时下一位不需要受到限制,故limit为0
        if(pos > 0 && prevZero > 0) {
            res += dfs(dp, pos-1, 0, num, digitsInt, 1);
        }
        // 此时该pos不可能为0,故后序不需要考虑前导0的情况,其他和above一样
        for (int j : digitsInt) {
            if(j == upper) {
                res += dfs(dp, pos-1, limit, num, digitsInt, 0);
            } else if (j < upper) {
                res += dfs(dp, pos-1, 0, num, digitsInt, 0);
            }
        }
        
        dp[pos][limit][prevZero] = res;
        return res;
    }
    
    public int atMostNGivenDigitSet(String[] digits, int n) {
        List<Integer> list = new ArrayList<>();
        while(n > 0) {
            list.add(n%10);
            n = n/10;
        }

        Integer[] num = list.toArray(new Integer[0]);
        int[][][] dp = new int[list.size()][2][2];
        int[] digitsInt = new int[digits.length];
        for(int i = 0; i < digits.length; i++) {
            digitsInt[i] = Integer.parseInt(digits[i]);
        }

        return dfs(dp, list.size() - 1, 1, num, digitsInt, 1);
    }
}
```

#### [357. Count Numbers with Unique Digits](https://leetcode.com/problems/count-numbers-with-unique-digits/)
`Feature`: 除了记录pos和limit之外,还需记录一个状态:已经使用掉的数字和未使用数字的集合,这里使用二进制表示集合,类似于状态压缩DP.  
对于前导0的判断是通过state是否为0判断.因为当state为0时,说明此时0-9中没有一个数字被使用,如果当前pos所选择的数字为0,则代表当前处于含有前导0的状态.
```java
class Solution {
    private int dfs(int pos, int state, int[][] dp) {
        if(pos == -1) {
            return 1;
        }
        
        if(dp[pos][state] > 0) {
            return dp[pos][state];
        }
        
        int res = 0;
        
        for(int i = 0; i < 10; i++) {
            if(i == 0 && state == 0) {
            	// 前导0
                res += dfs(pos-1, state, dp);
            } else {
                if((state>>i & 1) == 0) {
                    // 更改集合状态
                    res += dfs(pos-1, state | 1<<i, dp);
                }
            }
        }
        
        dp[pos][state] = res;
        return res;
    }
    
    public int countNumbersWithUniqueDigits(int n) {
        int fullSet = 1<<10;
        int[][]dp = new int[n][fullSet];
        
        return dfs(n-1, 0, dp);
    }
}
```

#### [233. Number of Digit One](https://leetcode.com/problems/number-of-digit-one/)
`Feature`: 除了记录pos和limit之外,还需记录一个状态:到该pos为止,前缀中含有的1的数量,即dfs function中的cnt.
```java
class Solution {
    private int dfs(int[][][] dp, int pos, int limit, int cnt, Integer[] num) {
        if(pos == -1) {
           return cnt;
        }

        if(dp[pos][cnt][limit] > 0) {
            return dp[pos][cnt][limit];
        }

        int upper = limit > 0 ? num[pos] : 9;
        int res = 0;
        for(int i = 0; i <= upper; i++) {
            // 当 i==1 时,需更新cnt
            if(i == upper) {
                res += dfs(dp, pos-1, limit, i == 1 ? cnt + 1:cnt, num);
            } else {
                res += dfs(dp, pos-1, 0, i == 1 ? cnt+1 : cnt, num);
            }
        }
        
        dp[pos][cnt][limit] = res;
        return res;
    }

    public int countDigitOne(int n) {
        if(n < 1) {
            return 0;
        }
        
        List<Integer> nList = new ArrayList<>();
        int tmp = n;
        while (n > 0) {
            nList.add(n%10);
            n = n / 10;
        }
        Integer[] num = nList.toArray(new Integer[0]);
        int bitsNum = num.length;
        int[][][] dp = new int[bitsNum][bitsNum][2];
        
        return dfs(dp, bitsNum-1, 1, 0, num);
    }
}
```