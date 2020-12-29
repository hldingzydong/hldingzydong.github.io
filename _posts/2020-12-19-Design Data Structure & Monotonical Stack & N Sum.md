---
layout: post
title:  Design Data Structure & Monotonical Stack & N Sum
date:   2020-12-19
author: hding
catalog: true
tags:
   - algorithm
---
# Design Data Structure

- [146. LRU Cache](https://leetcode.com/problems/lru-cache/)
- [155. Min Stack](https://leetcode.com/problems/min-stack/)
- [208. Implement Trie (Prefix Tree)](https://leetcode.com/problems/implement-trie-prefix-tree/)
- [211. Design Add and Search Words Data Structure](https://leetcode.com/problems/design-add-and-search-words-data-structure/)
- [225. Implement Stack using Queues](https://leetcode.com/problems/implement-stack-using-queues/)
- [232. Implement Queue using Stacks](https://leetcode.com/problems/implement-queue-using-stacks/)
- [380. Insert Delete GetRandom O(1)](https://leetcode.com/problems/insert-delete-getrandom-o1/)
- [381. Insert Delete GetRandom O(1) - Duplicates allowed](https://leetcode.com/problems/insert-delete-getrandom-o1-duplicates-allowed/)
- [460. LFU Cache](https://leetcode.com/problems/lfu-cache/)
- [676. Implement Magic Dictionary](https://leetcode.com/problems/implement-magic-dictionary/)
- [895. Maximum Frequency Stack](https://leetcode.com/problems/maximum-frequency-stack/)
- [981. Time Based Key-Value Store](https://leetcode.com/problems/time-based-key-value-store/)



# Monotonical Stack

- [316. Remove Duplicate Letters](https://leetcode.com/problems/remove-duplicate-letters/)
- [402. Remove K Digits](https://leetcode.com/problems/remove-k-digits/)
- [456. 132 Pattern](https://leetcode.com/problems/132-pattern/)



# N Sum

- [1. Two Sum](https://leetcode.com/problems/two-sum/)
- [15. 3Sum](https://leetcode.com/problems/3sum/)
```python
class Solution:
    def threeSum(self, nums: List[int]) -> List[List[int]]:
        res = []
        nums.sort()
        for i in range(len(nums) - 2):
            # nums[j] > ... > nums[i] > 0, break
            if nums[i] > 0:
                break

            # remove duplicate
            if i > 0 and nums[i] == nums[i - 1]:
                continue

            left = i + 1
            right = len(nums) - 1
            while left < right:
                currSum = nums[i] + nums[left] + nums[right] 
                if currSum == 0:
                    res.append([nums[i], nums[left], nums[right]])
                    left = left + 1
                    right = right - 1
                    while left < right and nums[left] == nums[left-1]:
                        left = left + 1
                    while left < right and nums[right] == nums[right+1]:
                        right = right - 1
                elif currSum < 0:
                    left = left + 1
                else:
                    right = right - 1
        return res
```

- [16. 3Sum Closest](https://leetcode.com/problems/3sum-closest/)
```python
class Solution:
    def threeSumClosest(self, nums: List[int], target: int) -> int:
        nums.sort()
        minDistance = float('inf')
        cloestSum = 0
        for i in range(len(nums) - 2):
            ## jump over duplicate
            if i > 0 and nums[i] == nums[i - 1]:
                continue

            left = i + 1
            right = len(nums) - 1
            while left < right:
                currSum = nums[i] + nums[left] + nums[right]
                distance = abs(currSum - target)
                if distance < minDistance:
                    minDistance = distance
                    cloestSum = currSum
                if currSum == target:
                    return target
                elif currSum < target:
                    left = left + 1
                else:
                    right = right - 1
        return cloestSum
```

- [18. 4Sum](https://leetcode.com/problems/4sum/)
`Solution`: 在3Sum上套一层即可

- [167. Two Sum II - Input array is sorted
](https://leetcode.com/problems/two-sum-ii-input-array-is-sorted/)
- [454. 4Sum II](https://leetcode.com/problems/4sum-ii/)
- [653. Two Sum IV - Input is a BST](https://leetcode.com/problems/two-sum-iv-input-is-a-bst/)
- [923. 3Sum With Multiplicity](https://leetcode.com/problems/3sum-with-multiplicity/)