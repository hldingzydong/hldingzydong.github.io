---
layout: post
title:  Leetcode-46-Permutations
date:   2020-02-05
author: hding
catalog: true
tags:
   - recursion
---
### Description
Given a collection of **distinct** integers, return all possible permutations.

### Example
```
Input: [1,2,3]
Output:
[
  [1,2,3],
  [1,3,2],
  [2,1,3],
  [2,3,1],
  [3,1,2],
  [3,2,1]
]
```

### Solution 1 : Recursion
```java
class Solution {
    private List<List<Integer>> res = new ArrayList<>();
    private List<Integer> tmpRes = new ArrayList<>();
    private boolean[] visited;
    
    private void permuteHelper(int[] nums, int count) {
        if(count == nums.length){
            List<Integer> subRes = new ArrayList<>(tmpRes);
            res.add(subRes);
        }else{
            for(int i = 0;i<nums.length;i++) {
                if(visited[i] == false) {
                    visited[i] = true;
                    tmpRes.add(nums[i]);
                    permuteHelper(nums, count+1);
                    tmpRes.remove((Object)nums[i]);
                    visited[i] = false;
                }
            }
        }
            
    }
    
    public List<List<Integer>> permute(int[] nums) {
       if(nums.length > 0){
            visited = new boolean[nums.length];
            permuteHelper(nums, 0);
        }
        return res;
    }
}
```


### Solution 2 : Non-Recursion
[explain](https://leetcode.com/problems/permutations/discuss/18237/My-AC-simple-iterative-javapython-solution)
```java
class Solution {
    public List<List<Integer>> permute(int[] nums) {
        List<List<Integer>> res = new ArrayList<>();
        if(nums.length > 0){
            List<Integer> l0 = new ArrayList<>();
            l0.add(nums[0]);
            res.add(l0);
            for(int i=1;i<nums.length;i++) {
                List<List<Integer>> tmpRes = new ArrayList<>();
                for(int j=0;j<=i;j++) {
                    for(List<Integer> l : res) {
                        List<Integer> tmpL = new ArrayList<>(l);
                        tmpL.add(j,nums[i]);
                        tmpRes.add(tmpL);
                    }
                }
                res = tmpRes;
            }
        }
        return res;
    }
}
```





























