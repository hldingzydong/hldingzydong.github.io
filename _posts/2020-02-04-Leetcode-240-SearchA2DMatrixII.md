---
layout: post
title:  Leetcode-240-SearchA2DMatrixII
date:   2020-02-04
author: hding
catalog: true
tags:
   - recursion, DivideAndConquer
---
### Description
Write an efficient algorithm that searches for a value in an m x n matrix. This matrix has the following properties:

- Integers in each row are sorted in ascending from left to right.
- Integers in each column are sorted in ascending from top to bottom.

### Example
Consider the following matrix:
```
[
  [1,   4,  7, 11, 15],
  [2,   5,  8, 12, 19],
  [3,   6,  9, 16, 22],
  [10, 13, 14, 17, 24],
  [18, 21, 23, 26, 30]
]
```
Given target = 5, return true.  
Given target = 20, return false.


### Solution 1 : Recursion
```java
class Solution {
    private boolean searchMatrixHelper(int[][] matrix, int target, int row_index, int col_index) {
        int row = matrix.length;
        int col = matrix[0].length;
        if(row_index > row-1 || col_index < 0 ) return false;
        else{
            if(matrix[row_index][col_index] == target) return true;
            else if(matrix[row_index][col_index] > target) {
                return searchMatrixHelper(matrix, target, row_index, col_index-1);
            }else{
                return searchMatrixHelper(matrix, target, row_index+1, col_index);
            }
        }
    }
    
    public boolean searchMatrix(int[][] matrix, int target) {
        if(matrix.length < 1) return false;
        int col = matrix[0].length;
        return searchMatrixHelper(matrix, target, 0, col-1);
    }
}
```


### Solution 2 : Non-Recursion
```java
class Solution {
    public boolean searchMatrix(int[][] matrix, int target) {
        if (matrix.length == 0 || matrix[0].length == 0)
            return false;
        
        int n = matrix.length;
        int m = matrix[0].length;
        
        int row = n - 1;
        int col = 0;
        
        while (row >= 0 && col < m)
        {
            if (matrix[row][col] > target)
                row--;
            else if (matrix[row][col] < target)
                col++;
            else
                return true;
        }
        
        return false;
    }
}
```


























