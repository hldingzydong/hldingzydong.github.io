---
layout: post
title:  Leetcode-22-GenerateParentheses
date:   2020-02-05
author: hding
catalog: true
tags:
   - recursion
---
### Description
Given n pairs of parentheses, write a function to generate all combinations of well-formed parentheses.

### Example
For example, given n = 3, a solution set is:
```
[
  "((()))",
  "(()())",
  "(())()",
  "()(())",
  "()()()"
]
```

### Solution 1 : Recursion
```java
class Solution {
    private List<String> res = new ArrayList<>();
    
    private void generateParenthesisHelper(String tmpStr, int leftNum, int rightNum) {
        if(leftNum == 0 && rightNum == 1) {
            tmpStr += ")";
            res.add(tmpStr);
        }else{
            if(leftNum == rightNum) {
                generateParenthesisHelper(tmpStr+"(",leftNum-1,rightNum);
            }else{
                if(leftNum > 0) {
                    generateParenthesisHelper(tmpStr+"(", leftNum-1, rightNum);
                }
                generateParenthesisHelper(tmpStr+")",leftNum,rightNum-1);
            }
        }
    }
    
    public List<String> generateParenthesis(int n) {
        generateParenthesisHelper("", n, n);
        return res;
    }
}
```





























