---
layout: post
title:  Union Find
date:   2020-09-21
author: hding
catalog: true
tags:
   - algorithm
---
# Union Find
`Weighted Quick Union with Path Compression`
![Union Find](/img/Algorithm/UnionFind.jpg)


## Pseudocode
```
def makeSet(x):
	p[x] = x
	rank[x] = 0			
```
```
def find(x):
	if p[x] == x:
		return x
	p[x] = find(p[x])		// Path Compression
	return p[x]
```
```
def union(x, y):
	x = find(x)
	y = find(y)

	if r[x] > r[y]: 		// Union by rank(height without compression)
		swap(x, y)

	p[x] = y
	if r[x] == r[y]:		
		r[y] += 1
```


## Complexity Analysis
Any sequence of <= m finds, <= n makeSets, unions, takes time O((m + n) * log\*n).  
Proof:  
- [phase1 54:15](https://www.bilibili.com/video/BV1nv411i7AR?p=10)  
- [phase2 00:00](https://www.bilibili.com/video/BV1D54y1U7aV?p=11)


## Practice
- [990. Satisfiability of Equality Equations](https://leetcode.com/problems/satisfiability-of-equality-equations/)
- [1202. Smallest String With Swaps](https://leetcode.com/problems/smallest-string-with-swaps/)
- [1319. Number of Operations to Make Network Connected](https://leetcode.com/problems/number-of-operations-to-make-network-connected/)