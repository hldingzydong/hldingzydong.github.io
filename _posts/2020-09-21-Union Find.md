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
#### Array
- [368. Largest Divisible Subset](https://leetcode.com/problems/largest-divisible-subset/)
- [765. Couples Holding Hands](https://leetcode.com/problems/couples-holding-hands/)
- [924. Minimize Malware Spread](https://leetcode.com/problems/minimize-malware-spread/)
- [928. Minimize Malware Spread II](https://leetcode.com/problems/minimize-malware-spread-ii/)
- [947. Most Stones Removed with Same Row or Column](https://leetcode.com/problems/most-stones-removed-with-same-row-or-column/)
- [952. Largest Component Size by Common Factor](https://leetcode.com/problems/largest-component-size-by-common-factor/)
- [990. Satisfiability of Equality Equations](https://leetcode.com/problems/satisfiability-of-equality-equations/)
- [1202. Smallest String With Swaps](https://leetcode.com/problems/smallest-string-with-swaps/)
- [1319. Number of Operations to Make Network Connected](https://leetcode.com/problems/number-of-operations-to-make-network-connected/)

#### Map(String-String)
- [721. Accounts Merge](https://leetcode.com/problems/accounts-merge/)
- [839. Similar String Groups](https://leetcode.com/problems/similar-string-groups/)
- [959. Regions Cut By Slashes](https://leetcode.com/problems/regions-cut-by-slashes/)


