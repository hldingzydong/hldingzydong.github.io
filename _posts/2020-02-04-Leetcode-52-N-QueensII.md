---
layout: post
title:  Leetcode-52-N-QueensII
date:   2020-02-04
author: hding
catalog: true
tags:
   - recursion, backtracking
---
### Description
The n-queens puzzle is the problem of placing n queens on an n×n chessboard such that no two queens attack each other.

![N-Queens](https://assets.leetcode.com/uploads/2018/10/12/8-queens.png)

Given an integer n, return the number of distinct solutions to the n-queens puzzle.


### Example
```
Input: 4
Output: 2
Explanation: There are two distinct solutions to the 4-queens puzzle as shown below.
[
 [".Q..",  // Solution 1
  "...Q",
  "Q...",
  "..Q."],

 ["..Q.",  // Solution 2
  "Q...",
  "...Q",
  ".Q.."]
]
```


### Solution 1 : Recursion
```java
class Solution {
    private int res = 0;
    private boolean[] col;
    private boolean[] diagonal;
    private boolean[] antiDiagonal;
    
    private void totalNQueensHelper(int n, int row_index, int col_index) {
        if(row_index == n-1) {
            if(col[col_index] == false && diagonal[row_index + col_index] == false && antiDiagonal[(n-1)-row_index+col_index] == false) {
                res++;
                return;
            }
        }else{ //[2][1] [1][0] 6  [0][n-1] 2*n-2   [n-1][0] 0 (n-1)-row_index+col_idnex
            if(col[col_index] == false && diagonal[row_index + col_index] == false && antiDiagonal[(n-1)-row_index+col_index] == false) {
                col[col_index] = true;
                diagonal[row_index + col_index] = true;
                antiDiagonal[(n-1)-row_index+col_index] = true;
                for(int i = 0; i < n; i++) {
                    totalNQueensHelper(n, row_index+1, i);
                }
                col[col_index] = false;
                diagonal[row_index + col_index] = false;
                antiDiagonal[(n-1)-row_index+col_index] = false;
            }
        }
    }
    
    public int totalNQueens(int n) {
        if(n < 1) return 0;
        
        col = new boolean[n];
        diagonal = new boolean[2*n-1];
        antiDiagonal = new boolean[2*n-1];
        
        for(int i=0; i<n; i++) {
            totalNQueensHelper(n,0,i);
        }
        
        return res;
    }
}
```



























