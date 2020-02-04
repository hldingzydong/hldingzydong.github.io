---
layout: post
title:  Leetcode-95-UniqueBinarySearchTreeII
date:   2020-02-03
author: hding
catalog: true
tags:
   - recursion, DP
---
### Description
Given an integer n, generate all structurally unique **BST**'s (binary search trees) that store values *1 ... n*.

### Example
```
Input: 3
Output:
[
  [1,null,3,2],
  [3,2,null,1],
  [3,1,null,null,2],
  [2,1,3],
  [1,null,2,null,3]
]
Explanation:
The above output corresponds to the 5 unique BST's shown below:

   1         3     3      2      1
    \       /     /      / \      \
     3     2     1      1   3      2
    /     /       \                 \
   2     1         2                 3
```

### Solution 1 : Recursion
```java
class Solution {
public List<TreeNode> generateTrees(int n) {
        List<TreeNode> list = new ArrayList<TreeNode>();
        if(n < 1) {
            return list;
        }else{
            return generateContiTrees(1,n);
        }
    }
    
    private List<TreeNode> generateContiTrees(int l, int r) {
            List<TreeNode> list = new ArrayList<TreeNode>();
            if(l > r) {
                list.add(null);
                return list;
            }else {
                for(int i=l;i<=r;i++) {
                  List<TreeNode> listLeft = generateContiTrees(l,i-1);
                  List<TreeNode> listRight = generateContiTrees(i+1,r);
                  for(int j=0;j<listLeft.size();j++) {
                    for(int k=0;k<listRight.size();k++) {
                        TreeNode node = new TreeNode(i);
                        node.left = listLeft.get(j);
                        node.right = listRight.get(k);
                        list.add(node);
                    }
                 } 
            }
            return list;
           }
    }
}
```


### Solution 2 : DP
```java
class Solution {
    public List<TreeNode> generateTrees(int n) {
        if(n < 1) return new ArrayList<>();
        else {
            List<TreeNode>[][] dp = new ArrayList[n][n];
            for(int i = 0; i < n ; i++) {
                for(int row = 0, col = i; row < n - i && col < n; row++, col++) {
                    dp[row][col] = new ArrayList<>();
                    if(row == col) {
                        dp[row][col].add(new TreeNode(row + 1));
                    }else{
                        for(int k = row; k <= col; k++) {
                            List<TreeNode> leftTrees = k-1>= row ? dp[row][k-1] : new ArrayList<>();
                            List<TreeNode> rightTrees = k+1 <= col ? dp[k+1][col] : new ArrayList<>();
                            
                            if(k-1 < row) leftTrees.add(null);
                            if(k+1 > col) rightTrees.add(null);

                            for(TreeNode leftNode : leftTrees) {
                                for(TreeNode rightNode : rightTrees) {
                                    TreeNode root = new TreeNode(k+1);
                                    root.left = leftNode;
                                    root.right = rightNode;
                                    dp[row][col].add(root);
                                }
                            }
                        }
                    }
                }
            }
            return dp[0][n-1];
        }
        
    }
}
```


























