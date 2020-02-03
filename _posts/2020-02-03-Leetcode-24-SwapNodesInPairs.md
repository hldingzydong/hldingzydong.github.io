---
layout: post
title:  Leetcode-24-SwapNodesInPairs
date:   2020-02-03
author: hding
catalog: true
tags:
   - recursion
---
### Description
Given a linked list, swap every two adjacent nodes and return its head.

You may not modify the values in the list's nodes, only nodes itself may be changed.


### Example
```
Given 1->2->3->4, you should return the list as 2->1->4->3.
```

### Solution 1 : Recursion
```java
class Solution {
    public ListNode swapPairs(ListNode head) {
        if(head == null || head.next == null) 
            return head;
        
        ListNode nHead = swapPairs(head.next.next);
        head.next.next = head;
        head = head.next;
        head.next.next = nHead;
        return head;
    }
}
```


### Solution 2 : Non-Recursion
```java
class Solution {
    public ListNode swapPairs(ListNode head) {
        if(head == null || head.next == null) 
            return head;
        
        ListNode firstNode = head;
        ListNode res = head.next;
        while(firstNode != null && firstNode.next != null) {
           ListNode thirdNode = firstNode.next.next;
           firstNode.next.next = firstNode;
           if(thirdNode == null || thirdNode.next == null) {
                firstNode.next = thirdNode; 
           }else{
                firstNode.next = thirdNode.next;
           }
           firstNode = thirdNode;
        }
        return res;
    }
}
```




























