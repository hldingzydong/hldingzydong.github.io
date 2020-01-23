---
layout: post
title:  DatabaseSystem之QueryExecution
date:   2020-01-21
author: hding
catalog: true
tags:
   - database
---
## Processing Models
DBMS的processing model定义了系统是如何执行一次query plan,主要有以下几个model:
- Iterator Model
- Materialization(物质化、具体化) Model
- Vectorized / Batch Model

#### Iterator Model(Pipeline Model)
Query plan的每个operator都实现了**Next**函数.在每次调用,operator都会返回一个single tuple或者一个null marker(如果没有想要的tuple)。operator实现了一个loop that calls next on its children to retrieve their tuples and then process them。

具体实例见[slides](https://15445.courses.cs.cmu.edu/fall2019/slides/12-queryexecution1.pdf)

几乎每个DBMS都使用该Model,一些operators必须block直到children emit all of their tuples， 例如 Joins, Subqueries， Order By.


#### Materialization Model
每个operator对其输入都只处理一次,emit其输出也只一次.operator将其输出具体化为一个single result,DBMS可以给出一些提出来避免scan 太多的tuples.  
输出可以是整个tuples(NSM)或者subsets of columns(DSM)。  

具体实例见[slides](https://15445.courses.cs.cmu.edu/fall2019/slides/12-queryexecution1.pdf)

比较适用于OLTP因为query一次只会access一小部分的tuples.


#### Vectorized / Batch Model
就像Iterator Model, 每个operator都实现了一个Next function。但是每个operator提交a batch of tuples而不是a single tuple。  
operator的internal loop一次处理多个tuples.batch的size基于hardware或者query properties而变化. 

具体实例见[slides](https://15445.courses.cs.cmu.edu/fall2019/slides/12-queryexecution1.pdf)


#### Plan Processing Direction
- Approach #1: Top-to-Bottom  
从root开始,从children “pull” data, tuples总是被函数调用而passed。

- Approach #2: Bottom-to-Top  
从叶结点开始,将data push至parents。允许对pipelines中caches/registers的紧密控制.


## Access Methods
Access Methods是DBMS access 存储在table中的data的方式,在relation algebra中并未定义.有以下三个approaches:  
- Sequential Scan
- Index Scan
- Multi-Index / "Bitmap" Scan

#### Sequential Scan
对于table中的每页page,将其从Buffer Pool中retrieve, 遍历每个tuple并且检查是否应该include它.  
DBMS维持一个内部的cursor,用来track它上一次examined的page/slot.

###### 伪代码
```java
for page in table.pages: 
	for t in page.tuples:
		if evalPred(t):
			// Do Something!
```

###### 优化
- Prefetching
- Buffer Pool Bypass
- Parallelization
- **Zone Maps**  
提前计算出page的attribute values的aggregates。DBMS可以先check这个zone map来决定是否需要access这个page。  
![Zone Map](/img/DataBase/ZoneMap.jpeg)

- **Late Materialization**  
DSM的DBMSs可以延迟将tuples缝合在一起,除非到了query plan的upper parts.  
![Late Materialization](/img/DataBase/LateMaterialization.jpeg)

- **Heap Clustering**  
通过clustering index, tuples在heap‘s pages中被排序.如果query获取tuples使用clustering index的attributes,那么DBMS可以直接jump到需要的pages.
![Heap Clustering](/img/DataBase/HeapClustering.jpeg)


#### Index Scan
DBMS pick一个index来找到query需要的tuples.（如何选择哪一个index将在后续课程中讲解）:
- 该index包含的attributes
- 此次query引用的attributes
- attribute的value的domains
- predicate composition
- 该index是否含有unique/non-unique的keys
![Index Scan](/img/DataBase/IndexScan.jpeg)


#### Multi-Index Scan
如果有很多个indexed可以被此次query使用,使用每个matching的index计算sets of record ids,基于此次query的predicates(union vs intersect)将这些sets结合起来，retrieve对应的records，对其apply剩下的predicates。

###### demo
```sql
SELECT * FROM students
WHERE age < 30 AND dept = 'CS' AND country = 'US'
```
假设我们有an index on **age** 和 an index on **dept**。
首先我们retrieve满足age<30的record ids，接下来retrieve 满足dept='CS'的record ids,然后取它们的intersection,然后retrieve records并且检查country = 'US'。  
Set intersection可以通过bitmaps，hash tables或Bloom Filters实现.
![Multi-Index Scan](/img/DataBase/MultiIndexScan.jpeg)


###### Index Scan Page Sorting  
基于unclustered index来retrieve tuples是效率低下的,DBMS可以首先找出来所有需要的tuples,然后基于他们的page_id排序.  
![Index Scan Page Sorting](/img/DataBase/IndexScanPageSorting.jpeg)



## Expression Evaluation
DBMS可以将一个WHERE的从句展示为一个**expression tree**.
![Expression Evaluation1](/img/DataBase/ExpressionEvaluation1.jpeg)
![Expression Evaluation2](/img/DataBase/ExpressionEvaluation2.jpeg)






























