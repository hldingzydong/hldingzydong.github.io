---
layout: post
title:  Leetcode-779-K-thSymbolInGrammar
date:   2020-02-03
author: hding
catalog: true
tags:
   - recursion
---
### Description
On the first row, we write a 0. Now in every subsequent row, we look at the previous row and replace each occurrence of 0 with 01, and each occurrence of 1 with 10.

Given row N and index K, return the K-th indexed symbol in row N. (The values of K are 1-indexed.) (1 indexed).

### Example
##### Example 1
```
Input: N = 1, K = 1
Output: 0
```

##### Example 2
```
Input: N = 2, K = 1
Output: 0
```

##### Example 3
```
Input: N = 2, K = 2
Output: 1
```

##### Example 4
```
Input: N = 4, K = 5
Output: 1
```


##### Explanation
```
row 1: 0
row 2: 01
row 3: 0110
row 4: 01101001
```


##### Note
```
1. N will be an integer in the range [1, 30].
2. K will be an integer in the range [1, 2^(N-1)].
```

### Solution 1 : Recursion
```java
class Solution {
    public int kthGrammar(int N, int K) {
        if(N == 1 && K == 1) return 0;
        else{
            int parent = kthGrammar(N-1, (K+1)/2);
            if(parent == 0) {
                if(K%2 == 0) return 1;
                else return 0;
            }else{
                if(K%2 == 0) return 0;
                else return 1;
            }
        }
    }
}
```


### Solution 2 : DP(But with MLE)
```java
class Solution {
    public int kthGrammar(int N, int K) {
        int row = N;
        int col = (int)Math.pow(2,N-1);
        int[][] dp = new int[row][col];
        dp[0][0] = 0;
        for(int i = 1; i < row; i++) {
            int maxCol = (int)Math.pow(2,i);
            for(int j = 0; j < maxCol; j++) {
                if(dp[i-1][j/2] == 0) {
                    if(j % 2 == 0) {
                        dp[i][j] = 0;
                    }else{
                        dp[i][j] = 1;
                    }
                }else{
                    if(j % 2 == 0) {
                        dp[i][j] = 1;
                    }else{
                        dp[i][j] = 0;
                    }
                }
            }
        }
        return dp[N-1][K-1];
    }
}
```



























