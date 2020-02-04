---
layout: post
title:  Leetcode-37-SudokuSolver
date:   2020-02-04
author: hding
catalog: true
tags:
   - recursion
   - backtracking
---
### Description
Write a program to solve a Sudoku puzzle by filling the empty cells.

A sudoku solution must satisfy all of the following rules:

1. Each of the digits 1-9 must occur exactly once in each row.
2. Each of the digits 1-9 must occur exactly once in each column.
3. Each of the the digits 1-9 must occur exactly once in each of the 9 3x3 sub-boxes of the grid.

Empty cells are indicated by the character '.'.



### Example
![sudo1](https://upload.wikimedia.org/wikipedia/commons/thumb/f/ff/Sudoku-by-L2G-20050714.svg/250px-Sudoku-by-L2G-20050714.svg.png)
![sudo2](https://upload.wikimedia.org/wikipedia/commons/thumb/3/31/Sudoku-by-L2G-20050714_solution.svg/250px-Sudoku-by-L2G-20050714_solution.svg.png)



##### Note
1. The given board contain only digits 1-9 and the character '.'.
2. You may assume that the given Sudoku puzzle will have a single unique solution.
3. The given board size is always 9x9.



### Solution 1 : Recursion
```java
class Solution {
    private boolean judgeFilledAbility(char[][] board, int row_index, int col_index, char target) {
        for(int i=0; i<9; i++) {
            if(board[row_index][i] == target || board[i][col_index] == target) return false;
        }
        
        int row = (row_index/3)*3;
        int col = (col_index/3)*3;
        for(int i=0;i<3;i++) {
            for(int j=0;j<3;j++) {
                if(board[row+i][col+j] == target) return false;
            }
        }
        return true;
    }
    
    private boolean solveSudokuHelper(char[][] board, int row_index, int col_index) {
            if(board[row_index][col_index] == '.') {
                for(char k = '1';k<='9';k++) {
                    if(judgeFilledAbility(board, row_index, col_index, k)) {
                        board[row_index][col_index] = k;
                        
                        int next_row_index = -1;
                        int next_col_index = -1;
                        for(int i=row_index;i<9;i++) {
                            for(int j=0;j<9;j++) {
                                if(board[i][j] == '.') {
                                    next_row_index = i;
                                    next_col_index = j;
                                    break;
                                }
                            }
                            if(next_row_index != -1) break;
                        }
                        
                        if(next_row_index == -1 || solveSudokuHelper(board, next_row_index, next_col_index) == true) return true;
                        
                        board[row_index][col_index] = '.';    
                    }
                }   
            }   
            return false;
    }
    
    public void solveSudoku(char[][] board) {
        for(int i=0;i<9;i++) {
            for(int j=0;j<9;j++) {
                    if(board[i][j] == '.') {
                        solveSudokuHelper(board, i, j);
                        return;
                    }
            }
        }
    }
}
```





























