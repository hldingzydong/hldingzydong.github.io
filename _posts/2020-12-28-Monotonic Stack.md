---
layout: post
title:  Monotonic Stack
date:   2020-12-28
author: hding
catalog: true
tags:
   - algorithm
---
# Monotonic Stack
1. 单调栈用于求解下一个更大元素的问题,见leetcode#496.   
2. 当找到下一个更大元素,即可求解距离下一个更大元素的距离,见leetcode#739.  
3. 由于可以求某个方向的更大元素,因此可以拓展到求解左右两边下一个更大元素,即左右边界,见leetcode#84的easy做法.
4. 优化#3,有时为了一遍扫描求解左右边界,可以先将元素压入递减(递增)栈中,当该元素被pop时,说明此时栈中下一个元素和当前位置的元素就是其左右边界,见leetcode#42和leetcode#84的one scan做法.

#### Basic
- [496. Next Greater Element I](https://leetcode.com/problems/next-greater-element-i/)
```python
class Solution:
    def nextGreaterElement(self, nums1: List[int], nums2: List[int]) -> List[int]:
        record = {}
        stack = []
        for i in range(len(nums2) - 1, -1, -1):
            while len(stack) > 0 and stack[-1] < nums2[i]:
                stack.pop()
            record[nums2[i]] = stack[-1] if stack else -1
            stack.append(nums2[i])

        ans = []
        for i in range(len(nums1)):
            ans.append(record[nums1[i]])
        return ans
```

- [739. Daily Temperatures](https://leetcode.com/problems/daily-temperatures/)
```python
class Solution:
    def dailyTemperatures(self, T: List[int]) -> List[int]:
        stack = []
        ans = []
        for i in range(len(T) - 1, -1, -1):
            while stack and T[stack[-1]] <= T[i]:
                stack.pop()
                
            if stack:
                ans.append(stack[-1] - i)
            else:
                ans.append(0)

            stack.append(i)
        
        res = []
        while ans:
            res.append(ans.pop())
        return res

```


#### Advance
- [42. Trapping Rain Water](https://leetcode.com/problems/trapping-rain-water/)
```java
class Solution {
    public int trap(int[] height) {
        if(height.length < 3) {
            return 0;
        }

        Stack<Integer> stack = new Stack<Integer>();
        int ans = 0;
        for(int i = 0; i < height.length; i++) {
            while(!stack.isEmpty() && height[stack.peek()] < height[i]) {
                int lowIndex = stack.pop();
                if(!stack.isEmpty()) {
                    int minHeight = Math.min(height[i], height[stack.peek()]);
                    ans = ans + (minHeight - height[lowIndex]) * (i - stack.peek() - 1);
                }
            }
            stack.push(i);
        }
        return ans;   
    }
}
```

- [84. Largest Rectangle in Histogram](https://leetcode.com/problems/largest-rectangle-in-histogram/)
  - easy read
```python
class Solution:
    def largestRectangleArea(self, heights: List[int]) -> int:
        if len(heights) == 0:
            return 0

        rightDir = []
        rightDirStack = []
        for i in range(len(heights) - 1, -1, -1):
            while rightDirStack and heights[rightDirStack[-1]] >= heights[i]:
                rightDirStack.pop()

            if rightDirStack:
                # rightDir.insert(0, rightDirStack[-1] - i - 1)
                rightDir.append(rightDirStack[-1] - i - 1)
            else:
                # rightDir.insert(0, len(heights)-1 - i)
                rightDir.append(len(heights)-1 -i)

            rightDirStack.append(i)

        leftDir = []
        leftDirStack = []
        for i in range(len(heights)):
            while leftDirStack and heights[leftDirStack[-1]] >= heights[i]:
                leftDirStack.pop()

            if leftDirStack:
                leftDir.append(i - leftDirStack[-1] - 1)
            else:
                leftDir.append(i)

            leftDirStack.append(i)
        
        maxRectange = 0
        for i in range(len(heights)):
            maxRectange = max(maxRectange, heights[i] * (rightDir[len(heights)-1 - i] + leftDir[i] + 1))
        return maxRectange
```

  - one scan
```python
class Solution:
    def largestRectangleArea(self, heights: List[int]) -> int:
        if len(heights) == 1:
            return heights[0]
        
        stack = []
        heights.append(0)  # for [1,2,3,4] increasing sequence
        index = 0
        maxRectangle = 0
        while index < len(heights):  # because append(0), so must could break loop
            if not stack or heights[index] >= heights[stack[-1]]:
                stack.append(index)
                index += 1
            else:
                h = heights[stack.pop()]
                if not stack:
                    maxRectangle = max(maxRectangle, index * h)
                else:
                    leftBound = stack[-1]
                    rightBound = index
                    maxRectangle = max(maxRectangle, (rightBound - leftBound - 1) * h)
        return maxRectangle
```

- [316. Remove Duplicate Letters](https://leetcode.com/problems/remove-duplicate-letters/)
- [402. Remove K Digits](https://leetcode.com/problems/remove-k-digits/)
- [456. 132 Pattern](https://leetcode.com/problems/132-pattern/)
```python
class Solution:
    def find132pattern(self, nums: List[int]) -> bool:
        if (len(nums) < 3):
            return False

        prefixMin = [0 for i in range(len(nums))]
        prefixMin[0] = nums[0]
        for i in range(1, len(nums)):
            prefixMin[i] = min(prefixMin[i - 1], nums[i])

        stack = []
        for i in range(len(nums) - 1, -1, -1):
            while stack and stack[-1] <= prefixMin[i]:
                stack.pop()

            if stack and stack[-1] < nums[i]:
                return True

            stack.append(nums[i])
        return False
```

- [503. Next Greater Element II](https://leetcode.com/problems/next-greater-element-ii/)
- [907. Sum of Subarray Minimums](https://leetcode.com/problems/sum-of-subarray-minimums/)
```python
class Solution:
    def sumSubarrayMins(self, arr: List[int]) -> int:
        stack = []
        arr.append(0)
        index = 0
        module = 1e9 + 7
        ans = 0
        while index < len(arr):
            if not stack or arr[index] >= arr[stack[-1]]:
                stack.append(index)
                index += 1
            else:
                minIndex = stack.pop()
                minValue = arr[minIndex]
                rightBound = index
                if stack:
                    leftBound = stack[-1]
                else:
                    leftBound = -1
                    
                ans = ans + (rightBound - minIndex) * (minIndex - leftBound) * minValue
                ans = int(ans % module)
        return ans
```