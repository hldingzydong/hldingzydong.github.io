---
layout: post
title:  Graph PartIII之有向图
date:   2020-09-13
author: hding
catalog: true
tags:
   - algorithm
---
# Directed Graph

## Q1 - Is there a path between *s* and *t* ?
Same as undirected graph
###### Practice
- [399. Evaluate Division](https://leetcode.com/problems/evaluate-division/)



## Q2 - What is the shortest path between *s* and *t* ?
#### Unit Weight
Same as undirected graph

#### Non-Unit Weight
##### No-Negative
`Dijktra Algorithm`  
Ps: Why need to be non-negative?
![Dijktra Algorithm Non-negative](/img/Algorithm/DijktraAlgorithmNon-negative.jpeg)

###### Practice
- [743. Network Delay Time](https://leetcode.com/problems/network-delay-time/)

##### Including Negative But No Cycle (DAG)
1. Consider vertices in `topological order`  
2. Relax all edges pointing from that vertex
Key: Refer to `Bellman-Ford Algorithm`, by using `topological order` could make sure the only sequence.

Ps: If want to know the longest path in a DAG with negative weight:
1. Negate all weights  
2. Find shortest path
3. Negate the shortest path

##### Including Negative With Non-negative Cycles
Precondition: If there is a cycle whose path is negative, then there is no shortest path.  
`Bellman-Ford Algorithm`
```
dist[s] = 0
dist[V - s] = 0
for i = 1,...,|V|-1:
	for e in E:
		relax(e)
```
Key:
![Bellman-Ford Algorithm](/img/Algorithm/Bellman-Ford.jpg)
![Bellman-Ford Proof](/img/Algorithm/Bellman-FordProof.jpg)

## Q3 - Is there a cycle in the graph? (DAG, Directed Acyclic Grapg, 有向无环图)
```
// GC - global clock
def DFS_GC(G):
	global_clock = 0			// time counter
	for v in V_Array:
		visited[v] = false
		prev[v] = null			// save the time before enter stack
		post[v] = null			// save the time after exit stack

	for v in V_Array:
		if !visited[v]:
			Explore_GC(G, v)
```
```
def Explore_GC(G, v):
	visited[v] = true
	prev[v] = global_clock
	global_clock += 1

	for u in v.neighbors():
		if !visited[u]:
			Explore_GC(G, u)

	post[v] = global_clock
	global_clock += 1
```
1. Run `DFS_GC(G)` then get prev[v], post[v] array
2. If there is `back edge` by using prev[v], post[v] and Edges, then this graph has cycle
3. What is a back edge? If there is a edge from u to v, and prev[v] < prev[u] < post[u] < post[v]

#### Is there a negative cycle ?
Run `Bellman-Ford Algorithm` and if any vertex *v* updated in phase *v*, there exists a negative cycle.



#### [Topological Sort(拓扑排序, Linearization Of Graph)](https://www.bilibili.com/video/BV1rx411W7gV?p=11) 
##### Background
Given a set of tasks to be completed with `precedence constrains`, in which order should we completed these tasks ? (These tasks must be constructed as a `DAG`)

##### Procedure
Run `DFS_GC(G)` then output vertices in `DESC order of post[v]`

##### Correctness Proof
Refer to [video 07:15](https://www.bilibili.com/video/BV1rx411W7gV?p=11).

##### Practice
- [207.Course Schedule](https://leetcode.com/problems/course-schedule/)
	Hint: Judge DAG
- [210.Course Schedule II](https://leetcode.com/problems/course-schedule-ii/)
	Hint: Topological Sort



## Q4 - How many components in a graph?
#### Strong Connectivity Components(SCC)
Exist a path from *v* to *u*, and exist a path from *u* to *v*
#### Idea
1. deduce G' from G by reverting direction of edges
2. run DFS(G') to have post[v]
3. 
```
V_Array = reverse post[v]
scc = 0
def SCC(G):
	for v in V_Array:
		if !visited[v]:
			Explore(G, v)
			scc++
```