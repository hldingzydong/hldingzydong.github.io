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

For example, if the input num[] is {1,2,3}: First, add 1 into the initial List<List<Integer>> (let's call it "answer").

Then, 2 can be added in front or after 1. So we have to copy the List in answer (it's just {1}), add 2 in position 0 of {1}, then copy the original {1} again, and add 2 in position 1. Now we have an answer of {{2,1},{1,2}}. There are 2 lists in the current answer.

Then we have to add 3. first copy {2,1} and {1,2}, add 3 in position 0; then copy {2,1} and {1,2}, and add 3 into position 1, then do the same thing for position 3. Finally we have 2·3=6 lists in answer, which is what we want.

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





























