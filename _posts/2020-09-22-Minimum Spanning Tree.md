---
layout: post
title:  Minimum Spanning Tree
date:   2020-09-22
author: hding
catalog: true
tags:
   - algorithm
---
# Minimum Spanning Tree(MST)

## Definition
Input: `un`directed graph G=(V,E), edge with weight(w > 0)  
Output: tree T in G that minimizes w(T) = sum(w(e)), for e in T.edges & touch all vertices
###### Tree
a connected acyclic graph <-> have V-1 edges



## Proof
![MST](/img/Algorithm/MST.jpg)



## Kruskal Algorithm
#### Pseudocode
```
def Kruskal():
	MST = {}
	sort(E) 				// in increasing order of weight
	for e in E:
		if MST.add(e) has cycle:	// by using Union Find
			continue
		else
			MST.add(e)
```
```
def Kruskal_UnionFind():
	MST = {}
	for v in V:
		makeSet(v)			// init union sets

	sort(E)
	for e in E:  // e = (v1,v2)
		if find(v1) != find(v2) 	// no cycle
			union(v1, v2)
			MST.add(e)

	return MST
```
#### Complexity Analysis
Refer to [`Weighted Quick Union with Path Compression`](https://hldingzydong.github.io/2020/09/21/Union-Find/) Complexity Analysis, will have V makeSets & unions, 2\*E finds, so complexity is O((V + 2·E)(log\*V)).



## Prime Algorithm
#### Lazy
```
def Prime_Lazy():
	MST = {}
	pq = PriorityQueue()				// key = edge, priority = weight

	for v in V:
		marked[v] = false

	visit(G, 0)
	while(!pq.isEmpty()) {
		Edge e = pq.deleteMin()			// get the shortest edge in PQ
		v, w = e.endpoints()
		if(marked[v] && marked[w]):		// lazy
			continue

		MST.add(e)
		if(!marked[v]) 
			visit(G, v)
		if(!marked[w])
			visit(G, w)
	}
```
```
def visit(G, v):
	marked[v] = true
	for e in v.neighbors:
		if(!marked[e.another_endpoint(v)]):
			pq.insert(e)
```

#### Eager(Optional)
Maintain a PQ of vertices connected by an edge to *MST*, where priority of vertex *v* = weight of shortest edge connecting *v* to *MST*
- Delete min vertex *v* and add its associated edge *e = v-w* to *MST*
- Update PQ by considering all edges *e = v-x* incident to *v*
	- ignore if *x* is already in *MST*
	- add *x* to PQ if not already on it
	- **decrease priority** of *x* if *v-x* becomes shortest edge connecting *x* to *MST*

#### Complexity Analysis
Decided by PQ impl:  
For PQ is binary heap, O(m\*logn)  
For PQ is Fibon heap, O(m + n\*logn)



## Practice
- [1584. Min Cost to Connect All Points](https://leetcode.com/problems/min-cost-to-connect-all-points/)