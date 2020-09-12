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
			last_vertex[v] = v
			Explore(G, v)
```
```
def Explore(G, v):
	visited[v] = true
	for u in v.neighbors():
		if !visited[u]:
			Explore(G, u)
```
```
Runtime: O(n + m)
```



## BFS
```
def BFS(G, s):
	dist[s] = 0						// save distance
	last_vertex[s] = s 				// save path
	Queue q = [s]

	for v in V_Array - s:
		dist[v] = inf
		last_vertex[v] = null

	while !q.isEmpty():
		u = q.poll()
		for v in u.neightbors():
			if dist[v] = inf		// in case of infinite loop
				q.push(v)
				dist[v] = dist[u] + 1
				last_vertex[v] = u
```


## Discuss

#### Path
- Is there a path between *s* and *t* ?
- What is the shortest path between *s* and *t* ?

#### Connectivity
- How many components in a graph?

#### Cycle
- Is there a cycle in the graph?