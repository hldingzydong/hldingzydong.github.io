---
layout: post
title:  Leetcode-98-ValidateBinarySearchTree
date:   2020-02-04
author: hding
catalog: true
tags:
   - recursion, DivideAndConquer
---
### Description
Given a binary tree, determine if it is a valid binary search tree (BST).

Assume a BST is defined as follows:

- The left subtree of a node contains only nodes with keys less than the node's key.
- The right subtree of a node contains only nodes with keys greater than the node's key.
- Both the left and right subtrees must also be binary search trees.


### Example
##### Example 1
```
    2
   / \
  1   3

Input: [2,1,3]
Output: true
```

##### Example 2
```
    5
   / \
  1   4
     / \
    3   6

Input: [5,1,4,null,null,3,6]
Output: false
Explanation: The root node's value is 5 but its right child's value is 4.
```


### Solution 1 : Recursion
```java
class Solution {
    private boolean validateHelper(TreeNode root, Integer min, Integer max) {
        if(root == null) return true;
        if(max != null && root.val >= max) return false;
        if(min != null && root.val <= min) return false;
        
        return validateHelper(root.left,min,root.val) && validateHelper(root.right,root.val,max);
    }
    
    public boolean isValidBST(TreeNode root) {
        return validateHelper(root,null,null);
    }
}
```


### Solution 2 : Recursion(Better Performace And DIY)
```java
class Solution {
    private boolean res = true;
    
    private Integer isValidBSTHelper(TreeNode root, boolean isLeft) {
        if(root == null){
            return null;
        }else{
            Integer leftMax = isValidBSTHelper(root.left, true);
            Integer rightMin = isValidBSTHelper(root.right, false);
            if(leftMax != null && leftMax >= root.val) {
                res = false;
                return 0;
            }
            
            if(rightMin != null && rightMin <= root.val) {
                res = false;
                return 0;
            }
            
            if(isLeft) {
                while(root.right != null) {
                    root = root.right;
                }
            }else{
                while(root.left != null) {
                    root = root.left;
                }
            }
            return root.val;
        }
    }
    
    public boolean isValidBST(TreeNode root) {
        if(root == null || (root.left == null && root.right == null)) return true;
        else{
            isValidBSTHelper(root, true);
            return res;
        }
    }
}
```


### Solution 3 : Inorder with DFS
```java
class Solution {
    double min = -Double.MAX_VALUE;
    public boolean isValidBST(TreeNode root) {
        return helper(root);
    }
    
    private boolean helper(TreeNode node) {
        if (node == null) return true;
        boolean left = helper(node.left);
        if (node.val <= min) return false;
        min = node.val;
        boolean right = helper(node.right);
        return left && right;
    }
}
```



























