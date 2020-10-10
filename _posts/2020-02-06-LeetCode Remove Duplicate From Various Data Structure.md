---
layout: post
title:  LeetCode Remove Duplicate From Various Data Structure
date:   2020-02-06
author: hding
catalog: true
tags:
   - leetcode
---
## Remove Duplicates 

#### Project Application
- 对于sql中**UNIQUE**关键字，数据库底层若需得出结果,可以先对底层的table进行外排序,然后移除duplicate values



#### From Sorted Array
26. [26. Remove Duplicates from Sorted Array](https://leetcode.wang/leetCode-26-Remove-Duplicates-from-Sorted-Array.html)  
```
快慢指针(aka“快慢链表”,复用input array的空间降低space complexity)
```

80. [80. Remove Duplicates from Sorted ArrayII](https://leetcode.wang/leetCode-80-Remove-Duplicates-from-Sorted-ArrayII.html)  
```
快慢指针(aka“快慢链表”,复用input array的空间降低space complexity); 
快慢指针变形,但每次比较的是fast和slow-1;
本人采用维护一个大小为3的sliding window,当1st和3rd相同时，移除3rd
```



#### From Sorted List
83. [83. Remove Duplicates from Sorted List](https://leetcode.wang/leetCode-83-Remove-Duplicates-from-Sorted-List.html)
82. [82. Remove Duplicates from Sorted ListII](https://leetcode.wang/leetCode-82-Remove-Duplicates-from-Sorted-ListII.html)  
```
快慢指针(慢指针始终指向distinct的末尾node,快指针遍历整个list,遇到duplicate就delete掉duplicate。为了处理head，有时需要造一个假的头指针)
递归
```



#### Remove All Adjacent Duplicates in String
1047. [easy](https://leetcode.com/problems/remove-all-adjacent-duplicates-in-string/discuss/294893/JavaC%2B%2BPython-Two-Pointers-and-Stack-Solution)  
```
快慢指针(3 ms, 41.4 MB)
stack(13 ms, 42.2 MB)
递归(637 ms, 421.1 MB)
```

1209. [II medium](https://leetcode.com/problems/remove-all-adjacent-duplicates-in-string-ii/discuss/392933/JavaC%2B%2BPython-Two-Pointers-and-Stack-Solution)  
```
快慢指针(3 ms, 41.5 MB) 需要借助辅助数组记录出现次数
stack(17 ms, 42.9 MB)
```


#### Summary
快慢指针，stack，递归，迭代