---
layout: post
title:  leetcode brilliant solution
date:   2021-01-02
author: hding
catalog: true
tags:
   - algorithm
---
#### [54. Spiral Matrix](https://leetcode.com/problems/spiral-matrix/)
```python
class Solution:
    def spiralOrder(self, matrix: List[List[int]]) -> List[int]:
        res = []
        upper = 0
        lower = len(matrix) - 1
        left = 0
        right = len(matrix[0]) - 1
        while True:
            for i in range(left, right + 1):
                res.append(matrix[upper][i])
            upper = upper + 1
            if upper > lower:
                break

            for i in range(upper, lower + 1):
                res.append(matrix[i][right])
            right = right - 1
            if left > right:
                break

            for i in range(right, left - 1, -1):
                res.append(matrix[lower][i])
            lower = lower - 1
            if upper > lower:
                break

            for i in range(lower, upper - 1, -1):
                res.append(matrix[i][left])
            left = left + 1
            if left > right:
                break
        return res

```



#### [56. Merge Intervals](https://leetcode.com/problems/merge-intervals/)
```python
class Solution:
    def merge(self, intervals: List[List[int]]) -> List[List[int]]:
        res = []
        intervals = sorted(intervals, key=lambda x:x[0])
        for i in range(len(intervals)):
            if res and res[-1][1] >= intervals[i][0]:
                res[-1][1] = max(res[-1][1], intervals[i][1])
            else:
                res.append(intervals[i])
        return res
```



#### [86. Partition List](https://leetcode.com/problems/partition-list/)
```python
# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, val=0, next=None):
#         self.val = val
#         self.next = next
class Solution:
    def partition(self, head: ListNode, x: int) -> ListNode:
        if head is None or head.next is None:
            return head

        lessDummy = ListNode()
        lessPtr = lessDummy
        largerDummy = ListNode()
        largerPtr = largerDummy

        while head:
            if head.val < x:
                lessPtr.next = head
                lessPtr = lessPtr.next
            else:
                largerPtr.next = head
                largerPtr = largerPtr.next
            head = head.next

        largerPtr.next = None
        lessPtr.next = largerDummy.next
        return lessDummy.next
```



#### [95. Unique Binary Search Trees II](https://leetcode.com/problems/unique-binary-search-trees-ii/)
```java
/**
 * Definition for a binary tree node.
 * public class TreeNode {
 *     int val;
 *     TreeNode left;
 *     TreeNode right;
 *     TreeNode() {}
 *     TreeNode(int val) { this.val = val; }
 *     TreeNode(int val, TreeNode left, TreeNode right) {
 *         this.val = val;
 *         this.left = left;
 *         this.right = right;
 *     }
 * }
 */
class Solution {
    public List<TreeNode> generateTrees(int n) {
        if(n == 0) {
            return new ArrayList<>();
        }
        List<TreeNode>[] dp = new List[n+1];
        dp[0] = new ArrayList<>();
        dp[0].add(null);

        for(int i = 1; i <= n; i++) {
        	dp[i] = new ArrayList<>();
        	for(int mid = 1; mid <= i; mid++) {
        		for(TreeNode left : dp[mid-1]) {
        			for(TreeNode right : dp[i - mid]) {
        				dp[i].add(new TreeNode(mid, left, clone(right, mid)));
        			}
        		}
        	}
        }
        return dp[n];
    }

    private TreeNode clone(TreeNode origin, int delta) {
    	if(origin == null) {
    		return origin;
    	}
    	return new TreeNode(origin.val + delta, clone(origin.left, delta), clone(origin.right, delta));
    }
}
```



#### [99. Recover Binary Search Tree](https://leetcode.com/problems/recover-binary-search-tree/)
```java
/**
 * Definition for a binary tree node.
 * public class TreeNode {
 *     int val;
 *     TreeNode left;
 *     TreeNode right;
 *     TreeNode() {}
 *     TreeNode(int val) { this.val = val; }
 *     TreeNode(int val, TreeNode left, TreeNode right) {
 *         this.val = val;
 *         this.left = left;
 *         this.right = right;
 *     }
 * }
 */
class Solution {
	TreeNode prev;
	TreeNode first;
	TreeNode second;

    public void recoverTree(TreeNode root) {
        inOrder(root);
        if(first != null && second != null) {
            int tmp = first.val;
    	    first.val = second.val;
    	    second.val = tmp;
        }
    }

    private void inOrder(TreeNode root) {
    	if(root == null) {
    		return;
    	}

    	inOrder(root.left);

    	if(prev != null && prev.val > root.val) {
    		if(first == null) {
    			first = prev;
    		}
    		second = root;
    	}
        prev = root;
    	inOrder(root.right);
    }
}
```



#### [117. Populating Next Right Pointers in Each Node II](https://leetcode.com/problems/populating-next-right-pointers-in-each-node-ii/)
```java
/*
// Definition for a Node.
class Node {
    public int val;
    public Node left;
    public Node right;
    public Node next;

    public Node() {}
    
    public Node(int _val) {
        val = _val;
    }

    public Node(int _val, Node _left, Node _right, Node _next) {
        val = _val;
        left = _left;
        right = _right;
        next = _next;
    }
};
*/

class Solution {
    public Node connect(Node root) {
        Node dummyLead = new Node(0);
        Node prevChild = dummyLead;
        Node tmp = root;
        while(tmp != null) {
            if(tmp.left != null) {
                prevChild.next = tmp.left;
                prevChild = prevChild.next;
            }
            if(tmp.right != null) {
                prevChild.next = tmp.right;
                prevChild = prevChild.next;
            }
            if(tmp.next != null) {
                tmp = tmp.next;
            } else {
                tmp = dummyLead.next;
                prevChild = dummyLead;
                dummyLead.next = null;
            }
        }
        return root;
    }
}
```



#### [128. Longest Consecutive Sequence](https://leetcode.com/problems/longest-consecutive-sequence/)
```python
class Solution:
    def longestConsecutive(self, nums: List[int]) -> int:
        if len(nums) < 1:
            return 0
        
        memory = {}
        ans = 1
        for i in range(len(nums)):
            if nums[i] in memory:
                continue

            left = memory[nums[i] - 1] if nums[i] - 1 in memory else 0
            right = memory[nums[i] + 1] if nums[i] + 1 in memory else 0
            length = left + right + 1
            ans = max(ans, length)

            if left > 0:
                memory[nums[i] - left] = length
            if right > 0:
                memory[nums[i] + right] = length

            memory[nums[i]] = length
        return ans
```



#### [154. Find Minimum in Rotated Sorted Array II](https://leetcode.com/problems/find-minimum-in-rotated-sorted-array-ii/)
```python
class Solution:
    def findMin(self, nums: List[int]) -> int:
        def findMinRange(left: int, right: int) -> int:
            if left == right:
                return nums[left]
            elif left + 1 == right:
                return min(nums[left], nums[right])
            elif nums[left] < nums[right]:
                return nums[left]
            else:
                mid = left + (right - left) // 2
                return min(findMinRange(left, mid - 1), findMinRange(mid, right))

        return findMinRange(0, len(nums) - 1)
```



#### [155. Min Stack](https://leetcode.com/problems/min-stack/)
```python
class MinStack:
    stack = []
    minValue = float("inf")

    def __init__(self):
        """
        initialize your data structure here.
        """
        self.minValue = float("inf")

    def push(self, x: int) -> None:
        if x <= self.minValue:
            self.stack.append(self.minValue)
            self.minValue = x
        self.stack.append(x)

    def pop(self) -> None:
        if self.minValue == self.stack.pop():
            self.minValue = self.stack.pop()

    def top(self) -> int:
        return self.stack[-1]

    def getMin(self) -> int:
        return self.minValue
# Your MinStack object will be instantiated and called as such:
# obj = MinStack()
# obj.push(x)
# obj.pop()
# param_3 = obj.top()
# param_4 = obj.getMin()

```