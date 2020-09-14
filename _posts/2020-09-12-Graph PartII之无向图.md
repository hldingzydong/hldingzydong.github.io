---
layout: post
title:  Graph PartII之无向图
date:   2020-09-12
author: hding
catalog: true
tags:
   - algorithm
---
# Undirected Graph

## Q1 - Is there a path between *s* and *t* ?
#### Idea - DFS / BFS + edge recorder
```
def DFS(G):
	for v in V_Array:
		visited[v] = false			
		last_vertex[v] = null		// save path

	for v in V_Array:
		if !visited[v]:
			last_vertex[v] = v      // representating path ternimal
			Explore(G, v)
```
```
def Explore(G, v):
	visited[v] = true
	for u in v.neighbors():
		if !visited[u]:
			last_vertex[u] = v   	// update path
			Explore(G, u)
```
```
def hasPath(s, t):
	Explore(G, s)
	return last_vertex[t] == null
```



## Q2 - What is the shortest path between *s* and *t* ?
#### Unit Weight
 - Single Source: BFS
 - Multiple Source: BFS init with multiple sources(push sources into queue)

#### Non-Unit Weight
 - Convert to Unit Weight
 - Dijkstra Algorithm(also suitable for digraph)
 	![Dijkstra Demo](/img/Algorithm/DijkstraDemo.jpeg)  

```
def Dijkstra(G, s):
 	dist[s] = 0
 	dist[V_Array - s] = inf

 	know_shortest_vertex = {}
 	unknow_shortest_vertex = {V_Array}

 	while know_shortest_vertex != V_Array:
 		v = get_smallest_dist(unknow_shortest_vertex)
 		know_shortest_vertex.add(v)
 		for u in v.neightbors():
 			if dist[u] > dist[v] + length(v, u):
 				dist[u] = dist[v] + length(v, u)
```

Key: 对于已经确定了已经有最短path的vertex,将他们作为一个集合(或一个bigger vertex),下一个所要被纳入集合的点总是 one step away的最近的vertex.
![Dijkstra Key](/img/Algorithm/DijkstraKey.jpg)


## Q3 - How many components in a graph?
#### DFS + counter
```
// CC - connected components
def DFS_CC(G): 
	cc_count = 0				// count for the # of CC
	for v in V_Array:
		visited[v] = false
		cc_num[v] = null		// save the belonged CC

	for v in V_Array:
		if !visited[v]:
			cc_count += 1
			Explore_CC(G, v)
```
```
def Explore_CC(G, v):
	visited[v] = true
	cc_num[v] = cc_count
	for u in v.neighbors():
		if !visited[u]:
			Explore_CC(G, u)
```
###### Practice
- [200. Number of Islands](https://leetcode.com/problems/number-of-islands/)



## Q4 - Is there a cycle in the graph?
#### Idea - DFS
During DFS, if there is a vertex whose neighbor is marked, then there is a cycle.
###### Practice
- [684. Redundant Connection](https://leetcode.com/problems/redundant-connection/)