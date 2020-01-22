---
layout: post
title:  DatabaseSystem之JoinsAlgorithm
date:   2020-01-20
author: hding
catalog: true
tags:
   - database
---
## Overview

#### 我们为什么需要Join?
在关系DB中,我们normalize tables来避免不重要信息的重复。  
我们使用Join算法来重建原始的tuples并且不会带来信息的丢失。  

#### Join Algorithm
将两张表一次性结合起来,通常在 query plan 中我们将较小的table放在左边("outer table")。

#### Join Operators
- Output: 在query plan中,join operator 向parent给出什么样的data?  
For a tuple **r ∈ R** and a tuple **s ∈ S** that match on join attributes, concatenate rand s together into a new tuple.  
内容会随着**处理模型、存储模型和query**而变化.
	- Join Operator Output: Data
	copy在outer和inner tuples的attributes,生成一个新的output tuple. 后续的operators不需要回到base tables
	来获得more data。 
	![Join Operator Output Data](/img/DataBase/JoinOperatorOutputData.jpeg)

	- Join Operator output: Record Ids
	只copy join keys和匹配的tuples的record ids. Ideal for column stores因为DBMS不需要copy对此次query无用的数据。 
	![Join Operator Output Record Ids](/img/DataBase/JoinOperatorOutputRecordIds.jpeg)


- Cost Analysis Criteria: 我们如何去分辨哪个join算法比另外一个更好?  
![Join IO Cost](/img/DataBase/JoinIOCost.jpeg)


## Join Algorithms

#### Nested Loop Join

###### Simple
```java
foreach tuple r ∈ R:
	foreach tuple s ∈ S:
	emit, if r and s match
```
I/O cost: M + (m·N)

###### Block
```java
foreach block BR ∈ R: 
	foreach block BS ∈ S:
		foreach tuple r ∈ BR: 
			foreach tuple s ∈ Bs:
				emit, if r and s match
```
I/O cost: M + (M · N)  
该算法对disk的访问更少,(对于 R 中的每个block, 扫描 S 一次)。

如果我们有B个Buffer Pool Page可以利用呢？我们使用 B-2 个buffers扫描outer table,使用一个buffer来扫描 inner table, 一个buffer来存储output.
```java
foreach B-2 blocks bR  ∈R: 
	foreach block bS ∈ S:
		foreach tuple r ∈ bR: 
			foreach tuple s ∈ bs:
				emit, if r and s match
```
I/O cost: M + (⌈M / (B-2)⌉ · N)  
if B-2 > M 呢？ Cost: M + N


###### Index 
通过使用index,我们可以避免使用sequential scan来找到inner table matches.
```java
foreach tuple r ∈ R:
	foreach tuple s ∈ Index(ri = sj):
		emit, if r and s match
```
I/O cost: 假设对每个index的探查的cost为 C, 故为 M + (m · C)


###### Summary
对于 Nested Loop Join ,选择较小的table作为outer table,尽可能得在memory中放置outer table。然后循环遍历inner table,或者使用index.


#### Sort-Merge Join
###### Process
- Phase#1:Sort  
依据join key(s),使用外部排序对两张表分别排序。
- Phase#2:Merge  
用cursors遍历两张排序表,输出matching的tuples.根据join type,可能会需要backtrack.

###### 伪代码
```java
sort R,S on join keys
curso rR ← Rsorted, curso rS ← Ssorted 
while cursorR and cursorS:
	if cursorR > cursorS: 
		increment cursorS 
	if cursorR < cursorS: 
		increment cursorR
	elif cursorR and cursorS match: 
		emit
		increment cursorS
```
具体过程见[slides](https://15445.courses.cs.cmu.edu/fall2019/slides/11-joins.pdf)

###### 算法分析
![Sort Merge Join Cost](/img/DataBase/SortMergeJoinCost.jpeg)
对于Sort-Merge Join,最坏的情况是对于两张表包含同样的value,在这样情况下，Cost为(M · N) + (sort cost)

###### 什么时候 Sort-Merge Join 是 useful 呢？  
One or both tables 已经基于join key排序;  
Output必须基于join key排序.  
Input relations 可能已经被一个explicit的sort operator排序,或者使用一个基于join key的index.



#### Hash Join
如果tuple r ∈ R 和tuple s ∈ S 满足join condition,那么它们对于join attributes有同样的value. 如果这个value被hash到了partition i，那么R tuple一定在 ri , S
tuple 一定在 si。因此, 在 ri 中的tuples只会跟在 si 中的tuples比较.

###### Process
- Phase#1:Build  
扫描outer table,基于join attributes，使用Hash Function h1建立一张hash table。  

- Phase#2:Probe  
扫描inner table, 使用Hash Function h1 来hash每个tuple,使得其可以jump到hash table的某个位置并且找到匹配的tuple.

###### 伪代码
```java
build hash table HTR for R 
foreach tuple s ∈ S
	output, if h1(s) ∈ HTR
```
具体过程见[slides](https://15445.courses.cs.cmu.edu/fall2019/slides/11-joins.pdf)

###### Hash Table 内容  
Key: join attribute  
Value: 依据implementation变化, Depends on what the operators above the join in the query plan expect as its input. 

- Approach #1: Full Tuple  
这样就免于在一次match中需要retrieve outer relation's tuple,但是在内存中会占更多的空间.

- Approach #2: Tuple Indetifier
Ideal for column stores,DBMS无需从disk中fetch无用的数据。


###### Probe Phase Optimization
Bloom Filter,引自[Wikipedia](https://zh.wikipedia.org/wiki/%E5%B8%83%E9%9A%86%E8%BF%87%E6%BB%A4%E5%99%A8)
> 当一个元素被加入集合时，通过K个散列函数将这个元素映射成一个**bit array**中的K个点，把它们置为1。  
> 检索时，我们只要看看这些点是不是都是1就（大约）知道集合中有没有它了:如果这些点有任何一个0，则被检元素一定不在；如果都是1，则被检元素**很可能**在,但也可能不在，需要到origin table中去验证。


当key很可能不会存在于hash table中时,我们可以在build的过程中创建一个Bloom Filter。  
Threads在探测Hash Table中可以先check该filter.因为filter会fit in CPU的cache，这将会非常快.
![Probe Phase Optimization](/img/DataBase/ProbePhaseOptimization.jpeg)


##### Grace Hash Join
如果我们的内存中没有足够的空间来容纳Hash Table怎么办?我们不想让Buffer Pool Manager在一次random中swap out这个Hash Table.  
在**Build Phase**中, 基于join attributes,我们hash两张tables到partitions;  
在**Probe Phase**中, 比较两张表相对应的partitions。

###### 伪代码:
```java
foreach tuple r ∈ bucketR,0: 
	foreach tuple s ∈ bucketS,0:
		emit, if match(r, s)
```
![Grace Hash Join](/img/DataBase/GraceHashJoin.jpeg)

###### Recursive Partitioning
如果上述过程中的buckets不fit in memory,我们可以使用**recursive partitioning**,将tables分为可以fit in memory的chunks.  
1. 对于无法fit in memory的bucket使用另外一个Hash Function h2(h1 != h2)来建立另外一张Hash Table.
2. 对于在那一层的另外一张table的bucket的每个tuple，我们probe it.
![Recursive Partitioning](/img/DataBase/RecursivePartitioning.jpeg)


###### 算法分析:  
假设我们有足够的buffers, Cost = 3(M + N);
Partition Phase: Read + Write 两张表 -> 2(M + N) IOs  
Probing Phase: Read 两张表 -> (M + N) IOs  


## Summary 

| Algorithm | IO Cost | Example |
| :-----: | :-----: | :-----: |
| Simple Nested Loop Join | M + (m · N) | 1.3 hours |
| Block Nested Loop Join | M + (M · N) | 50 seconds |
| Index Nested Loop Join | M + (M · logN) | 20 seconds |
| Sort-Merge Join | M + N + (sort cost) | 0.59 seconds |
| Hash Join | 3(M + N) | 0.45 seconds |

## Conclusion
对于operator execution, hashing总是比sorting好。但是对于不一致的data或者当结果需要被sorted时,sorting更好.  
好的DBMS使用其中一个或者全都用.













