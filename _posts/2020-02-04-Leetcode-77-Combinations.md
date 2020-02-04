---
layout: post
title:  Leetcode-77-Combinations
date:   2020-02-04
author: hding
catalog: true
tags:
   - recursion, DivideAndConquer
---
### Description
Given two integers n and k, return all possible combinations of k numbers out of 1 ... n.


### Example
```
Input: n = 4, k = 2
Output:
[
  [2,4],
  [3,4],
  [2,3],
  [1,2],
  [1,3],
  [1,4],
]
```


### Solution 1 : Recursion
```java
class Solution {
    public List<List<Integer>> combine(int n, int k) {
        List<List<Integer>> result = new ArrayList<List<Integer>>();
        if (k > n || k < 0) {
            return result;
        }
        if (k == 0) {
            result.add(new ArrayList<Integer>());
            return result;
        }
        result = combine(n - 1, k - 1);
        for (List<Integer> list : result) {
            list.add(n);
        }
        result.addAll(combine(n - 1, k));
        return result;
    }
}
```


### Solution 2 : Backtracking
```java
class Solution {
    private List<List<Integer>> res = new ArrayList<>();
    private List<Integer> tmpRes = new ArrayList<>();
    
    private boolean isValid(int n, int k, int count, int num) {
        if((n - num) >= (k - (count+1))) return true;
        else return false;
    }
    
    private void combineHelper(int n, int k, int count, int min) {
        if(count < k) {
            for(int i=min;i<=n;i++) {
                if(isValid(n,k,count,i)) {
                    tmpRes.add(i);
                    combineHelper(n,k,count+1,i+1);
                    tmpRes.remove(new Integer(i));
                }
            }
        }else if(count == k) {
            res.add(tmpRes);
            List<Integer> tmpRes2 = new ArrayList<>();
            for(Integer digit : tmpRes) {
                tmpRes2.add(digit);
            }
            tmpRes = tmpRes2;
        }
    }
    
    public List<List<Integer>> combine(int n, int k) {
        combineHelper(n,k,0,1);
        return res;
    }
}
```




























