---
layout: post
title:  Bit Manipulation
date:   2021-01-20
author: hding
catalog: true
tags:
   - algorithm
---
## 汉明距离
```java
n = n & (n-1)
```
该算式将消除n的最右边的1.

#### Practice
- [461. Hamming Distance](https://leetcode.com/problems/hamming-distance/)
- [201. Bitwise AND of Numbers Range](https://leetcode.com/problems/bitwise-and-of-numbers-range/)
- [477. Total Hamming Distance](https://leetcode.com/problems/total-hamming-distance/)



## 重复数字
```java
n ^ n == 0
```
相同的数,按位XOR将得到0

#### Practice
- [136. Single Number](https://leetcode.com/problems/single-number/)
- [260. Single Number III](https://leetcode.com/problems/single-number-iii/)