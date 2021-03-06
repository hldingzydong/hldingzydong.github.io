---
layout: post
title:  Graph PartI之概论
date:   2020-09-10
author: hding
catalog: true
tags:
   - algorithm
---
# Graph

## Representation
- Adjacency Matrix
- Edge Sets
- Adjacency Lists 



## DFS
We assume that the graph has **n vertexes** and **m edges**.
```
def DFS(G):
	for v in V_Array:
		visited[v] = false			// mark vertex

	for v in V_Array:
		if !visited[v]:
			Explore(G, v)
```
```
def Explore(G, v):
	visited[v] = true
	for u in v.neighbors():
		if !visited[u]:
			Explore(G, u)
```



## BFS
```
def BFS(G, s):
	dist[s] = 0						// save distance
	Queue q = [s]

	for v in V_Array - s:
		dist[v] = inf

	while !q.isEmpty():
		u = q.poll()
		for v in u.neightbors():
			if dist[v] = inf		// in case of infinite loop
				q.push(v)
				dist[v] = dist[u] + 1
```



## Discuss

#### Path
- Is there a path between *s* and *t* ?
- What is the shortest path between *s* and *t* ?

#### Connectivity
- How many components in a graph?

#### Cycle
- Is there a cycle in the graph?



## Challenge

#### Is a graph bipartite ?
- [785. Is Graph Bipartite?](https://leetcode.com/problems/is-graph-bipartite/)
- [886. Possible Bipartition](https://leetcode.com/problems/possible-bipartition/)

#### Find a (general) cycle that uses every edge exactly once ?
Eulerian tour

#### Find a cycle that visits every vertex exactly once ?
NP-Complete