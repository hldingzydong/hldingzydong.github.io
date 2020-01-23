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


## 如何执行query plan with multiple workers？


#### Parallel vs Distributed

|   | Parallel DBMSs | Distributed DBMSs |
| :-----: | :-----: | :-----: |
| Resource | physically close to each other | can be far from each other |
| Resource Communicate | high-speed interconnect | slow(er) interconnect |
| Communication | cheap and reliable | cost and problems cannot be ignored |


## Process Models
DBMS的process model定义了系统是如何架构的来支持来自多个user的application的concurrent requests。  
DBMS中的worker是指代表client执行任务然后返回结果。

#### Process per DBMS Worker
每个worker是一个独立的OS process,依赖于OS的scheduler, 对于global的数据结构采用shared-memory,一个process crash掉了不会使整个系统down掉.
![Process Per Worker](/img/DataBase/ProcessPerWorker.jpeg)


#### Process Pool
worker使用在pool中的任何一个process，仍然依赖于OS scheduler和shared memory. Bad for CPU cache locality.
![Process Pool](/img/DataBase/ProcessPool.jpeg)


#### Thread per DBMS Worker
一个process有多个worker threads，这样DBMS管理自己的scheduling,有可能使用dispatcher thread。一旦thread crash掉了可能会杀掉整个系统.
![Thread Per Worker](/img/DataBase/ThreadPerWorker.jpeg)

使用多线程的架构,会有更少的上下文切换，不需要管理shared memory.但不意味着支持内查询.  
对于每个query plan, DBMS决定where, when, and how to execute it.  
- 应该使用多少个tasks？
- 应该使用多少个CPU cores？
- 在CPU core上应该执行什么task?
- 应该在哪里store某个task的output？


## Execution Parallelism

#### Inter- VS Intra- Query Parallelism
- Inter-Query: Different queries are executed concurrently. 提高了吞吐量,减少了延迟等待时间，提高了整体的performance.如果多个query是read-only，那么协调它们很容易,但是如果是多个query同时更新DB呢？我们将在后面讲解.


- Intra-Query: Execute the operations of a single
query in parallel.减少了对long-running queries的等待时间.通过executing its operators in parallel提高了单个query的performance.可以讲operators的organization视为生产者/消费者模型,对于每个relational operator,都有parallel algorithms，要么是有多线程access centralized 数据结构,要么是使用partitioning to divide work up.比如:Parallel Grace Hash Join:
![Parallel Grace Hash Join](/img/DataBase/ParallelGraceHashJoin.jpeg)

#### Intra-Query Parallelism
###### Intra-Operator(Horizontal)
将operators分解为独立的**fragments**,对data的不同subset,施加相同的function.DBMS插入**exchange** operator到query plan中,将来自children operator的结果合并.
具体见[slides](https://15445.courses.cs.cmu.edu/fall2019/slides/13-queryexecution2.pdf)

exchange operator type:  
- Gather: 将来自多个workers的结果结合到一个output,query plan的root必须总是一个gather exchange.
- Repartition: 重新组织多个input streams到多个output streams.
- Distribute: 将一个简单的input stream分为多个output stream。
![Intra-Operator](/img/DataBase/IntraOperator.jpeg)


###### Inter-Operator(Vertical)(pipelined parallelism)
operators是overlapped是为了pipeline data从一个stage到另外一个stage但不需要materialization(具体化)。
![Inter-Operator](/img/DataBase/InterOperator.jpeg)


###### Bushy(灌木丛生的、毛密的)
对inter-operator的扩展，workers同一时间执行来自一次query plan的不同segments的多个operator,仍然需要exchange operators来将不同segments的中间结果结合在一起。
```sql
SELECT * FROM A JOIN B JOIN C JOIN Distribute
```
如该sql语句中,一个线程执行“A JOIN B”，另一个线程执行“C JOIN D”,最终将其结果combine.
![Bushy](/img/DataBase/Bushy.jpeg)


## I/O Parallelism
如果disk成为瓶颈的话,使用additional的processes/threads来执行queries不会有太大帮助，如果每个worker读disk的不同的segment反而会使得事情更糟糕.

将DBMS设置在多个存储设备上:
- 每个DB有多个disks
- 每个disk对应一个DB
- 每个disk对应一个relation
- 将relation分布在多个disk上

#### Multi-Disk Parallelism
设置OS/硬件来将DBMS的files存储在多个存储设备上:Storage Applications、RAID Configuration。这对DBMS来说是看不见的.
关于RAID是什么，引自[Wikipedia](https://zh.wikipedia.org/zh/RAID)
> 独立硬盘冗余阵列（RAID, Redundant Array of Independent Disks），旧称廉价磁盘冗余阵列（Redundant Array of Inexpensive Disks），简称磁盘阵列。利用虚拟化存储技术把多个硬盘组合起来，成为一个或多个硬盘阵列组，目的为提升性能或数据冗余，或是两者同时提升。  
> 在运作中，取决于 RAID 层级不同，数据会以多种模式分散于各个硬盘，RAID 层级的命名会以 RAID 开头并带数字，例如：RAID 0、RAID 1、RAID 5、RAID 6、RAID 7、RAID 01、RAID 10、RAID 50、RAID 60。  
> 简单来说，RAID把多个硬盘组合成为一个逻辑硬盘，因此，操作系统只会把它当作一个实体硬盘。RAID常被用在服务器电脑上，并且常使用完全相同的硬盘作为组合。

![Multi-Disk Parallelism](/img/DataBase/MultiDiskParallelism.jpeg)


#### DataBase Partitioning
一些DBMSs允许对每个独立的DB特殊化disk的location(BPM将一页page映射到一个disk location).  
如果DBMS在一个separate的directory上存储每一个DB,在filesystem的层面上很容易实现。 
将一个简单的logical table分为被存储/管理的separate的物理segements.  
理论上讲,partitioning对application是看不见的.application获取logical table时并不关心它是如何被存储的.但是这在分布式DBMSs中不一定是这样的.

###### Vertical Partitioning
将一张table的attributes存储在a seperate location(如文件，disk volume),为了重建origin record,必须存储tuple的信息.
```sql
CREATE TABLE foo ( 
	attr1 INT,
	attr2 INT,
	attr3 INT,
	attr4 TEXT );
```
![Vertical Partitioning](/img/DataBase/VerticalPartitioning.jpeg)


###### Horizontal Partitioning 
基于一些partitioning的key,我们可以讲一张table的多个tuples分为多个segements:
- Hash Partitioning
- Range Partitioning
- Predicate Partition
![Horizontal Partitioning](/img/DataBase/HorizontalPartitioning.jpeg)





























