---
layout: post
title:  Basic Introdcution Of Database Manage System
date:   2020-03-02
author: hding
catalog: true
tags:
   - database
---
# Overview
This article briefly introduces the Database Manage System (DBMS), including its Storage, Buffer Pool Manager, Table Index, Query, Concurrent Control, Log and Recovery. A Database Manage System is built by combining these components.  

Before introducing Database Manage System, there are some basic concepts need to explain:  
> A **data model** is collection of concepts for describing the data in a database.  
> A **schema** is a description of a particular collection of data, using a given data model.  
> A **relation** is unordered set that contain the relationship of attributes that represent entities.  
> A relation’s **primary key** uniquely identifies a single tuple.  
> A **foreign key** specifies that an attribute from one relation has to map to a tuple in another relation.  

Now, let's begin our adventure.

# [1.   Storage](https://15445.courses.cs.cmu.edu/fall2019/notes/03-storage1.pdf)
The Database Manage System we are talking is a **disk-oriented** System.
![DiskOrientedDBMS](/img/DataBase/DiskOrientedDBMS.jpeg)

### 1.1 File Storage
The DBMS stores a database as one or more files on disk and OS doesn't know anything about the contents of these files. The Storage Manager organize the files as a collection of pages.

### 1.2 Page Storage
> A **page** is a fixed-size block of data. It can contain tuples, meta-data, indexes or log records.  

##### 1.2.1 Files Layout
There are three ways to organize pages inside a file.  
- **Heap File Organization**: an unordered collection of pages where tuples are stored in random order. So pages need meta-data to track what pages exist and which one have free space. Here you can see two ways to represent:
![LinkedList](/img/DataBase/LinkedList.jpeg){:height="70%" width="70%"}
![PageDirectory](/img/DataBase/PageDirectory.jpeg){:height="70%" width="70%"}

- Sequential/Sorted File Organization
- Hashing File Organization

##### 1.2.2 Pages Meta-data
Include **Page Size, Checksum, DBMS version, Transaction Visibility, Compression Information**.  
Some systems (Oracle) require pages to be **self-contained**.

### 1.3 Tuple Storage
Assuming inside a page, we only store tuples, let’s discuss how to organize the data stored.
> A **tuple** is a set of attribute values (also known as its domain) in the relation.

##### 1.3.1 Pages Layout 
There are two ways to have pages layout: **slotted pages** and **log records**.
![SlottedPages](/img/DataBase/SlottedPages.jpeg){:height="70%" width="70%"}
![LogStructured](/img/DataBase/LogStructured1.jpeg){:height="70%" width="70%"}

##### 1.3.2 Tuples Layout
Each tuple is prefixed with a header that contains meta-data about: **Visibility info (Concurrency Control)**, Bit Map for NULL values.  
We don't need to store meta-data about the schema because attribute are typically stored in the order that we specify them when creating the table.
![TupleData](/img/DataBase/TupleData.jpeg)

##### 1.3.3 Record Ids
DBMS uses a unique record identifier (**page_id + offset / slot**) to track individual tuples.

### [1.4 Data Representation Inside Tuples](https://15445.courses.cs.cmu.edu/fall2019/notes/04-storage2.pdf)
- INTEGER / BIGINT / SMALLINT / TINYINT (C/C++ Representation)
- FLOAT / REAL vs NUMERIC / DECIMAL  (IEEE-754 Standard/Fixed-point Decimals)
	- Variable-precision numberic type that uses the "native" C/C++ types, eg: **FLOAT**,**REAL/DOUBLE**, stored directly as specified by IEEE-754, typically **faster** than arbitary precision numbers but can have **rounding errors**. (使用原生的C/C++类型,不需要具体的精度,非常快,但是会有微小的误差)  
	```c++
	int main(int argc, char* argv[]) {
		float x = 0.1;
		float y = 0.2;
		printf("x+y = %f\n", x+y);  	// x+y = 0.300000
		printf("0.3 = %f\n", 0.3);  	// 0.3 = 0.300000
		printf("x+y = %.20f\n", x+y);   // x+y = 0.30000001192092895508
		printf("0.3 = %.20f\n", 0.3);   // 0,3 = 0.29999999999999998890
	}
	``` 

	- Numeric data types with arbitrary precision and scale. Used when round errors are unacceptable, eg: **NUMERIC**, **DECIMAL**, Typically stored in a exact, variable-length binary representation with additional meta-data. (Like a **VARCHAR** but not stored as a string). (这种类型需指明具体的精度是多少,存储时需带有额外的元数据)
	```c++
	typedef unsigned char NumericDigit;
	typedef struct {
		int ndigits; 			// # of Digits
		int weight;  			// Weight of 1st Digit
		int scale;   			// Scale Factor
		int sign;    			// Positive/Negative/NaN
		NumericDigit *digits;   // Digit Storage
	} numeric;
	```

- VARCHAR / VARBINARY / TEXT / BLOB (Header with length, followed by data bytes)
	- Large Values
	  To store values that are larger than a page, the DBMS uses separate **overflow storage pages** / **external file**. But the DBMS cannot manipulate the contents of an external file.
	  ![LARGE VALUES](/img/DataBase/LargeValues.jpeg){:height="50%" width="50%"}![External Value Storage](/img/DataBase/ExternalValueStorage.jpeg){:height="50%" width="50%"}

- TIME / DATE / TIMESTAMP (32/64-bit integer of (micro)seconds since Unix epoch)

### 1.5 Storage Model
##### 1.5.1 OLTP vs OLAP
> **On-line Transaction Processing (OLTP)**, simple queries that read/update a small amount of data that is related to a single entity in the database. (平时做web后端的对DB的CRUD)

```sql
UPDATE useracct SET lastLogin = NOW(), hostname = ? WHERE userID = ?
```
> **On-line Analytical Processing (OLAP)**, complex queries that read large portions of the database spanning multiple entities. (类似于大数据)

```sql
SELECT COUNT(U.lastLogin), EXTRACT(month FROM U.lastLogin) AS month
FROM useracct AS U
WHERE U.hostname LIKE '%.gov'
GROUP BY
EXTRACT(month FROM U.lastLogin)
```

##### 1.5.2 NSM vs DSM
> **N-ary Storage Model (NSM aka “row storage”)**. The DBMS stores all attributes for a single tuple contiguously in a page.  

![DisNSM](/img/DataBase/DisNSM.jpeg){:height="50%" width="50%"}

> **Decomposition (分解) Storage Model (DSM aka “column store”)**. The DBMS stores the values of a single attribute for all tuples contiguously in a page.

![DSM](/img/DataBase/DSM.jpeg){:height="50%" width="50%"}


# [2.   Buffer Pool Manager (BPM)](https://15445.courses.cs.cmu.edu/fall2019/notes/05-bufferpool.pdf)
![DiskOrientedDBMS](/img/DataBase/DiskOrientedDBMS.jpeg)
### 2.1 Why Need BPM
According to **spatial locality** and **temporal locality**, BPM could minimize the time cost of getting a page from disk. We build our own BPM and not use OS‘s cache because we could use suitable evict algorithm to evict a page, and we could set the size of Buffer Pool.

### 2.2 Component
It is a **memory region** organized as an array of fixed-size pages. An array entry is called a **frame**. There is a **page table**, mapped from page_id to frame_id. For every page in Buffer Pool, each of them need to contain Dirty Flag and Pin/Reference Counter.  
**Dirty Flag** is used to identify whether a page is modified after read into memory. When a dirty page is evicted, DBMS needs to write it back to disk.  
**Pin/Reference Counter** is used to decide whether this page is evicted. When Pin/Reference Counter is 0, apply Replacement Policy to this page.
![BufferPoolManager](/img/DataBase/BufferPoolManager.jpeg){:height="60%" width="60%"}

### 2.3 Optimization
Multiple Buffer Pools, Pre-Fetching, Scan Sharing and Bypass().

### 2.4 Replacement Policy
##### 2.4.1 Least Recently Used (LRU)
Maintain a timestamp of when each page was last accessed. When DBMS needs to evict a page, pick the oldest.

##### 2.4.2 Clock
![Clock](/img/DataBase/Clock.jpeg){:height="70%" width="70%"}

###### 2.4.3 LRU-K
###### 2.4.4 Localization
###### 2.4.5 Priority Hints


# [3.   Table Index](https://15445.courses.cs.cmu.edu/fall2019/slides/07-trees1.pdf)
> A **table index** is a replica of a subset of a table's attributes that are organized and/or sorted for efficient access using a subset of those attributes.

### 3.1 Why We Need Index
Increase the efficiency of query, but it costs Storage and Maintenance.

### 3.2 B+Tree Implementation
> A B+Tree is an M-way search tree with the following properties:  
> (1) Perfectly balanced (every leaf node is at the same depth)  
> (2) Every node other than the root, is at least half-full: M/2-1 ≤ #keys ≤ M-1  
> (3) Every node is comprised of an array of sorted key/value pairs

![BTree](/img/DataBase/BTree.jpeg){:height="80%" width="80%"}
##### 3.2.1 Tree Node
###### 3.2.1.1 Inner Node
Every inner node with k keys has k+1 non-null children. Children are pointers to the child nodes.

###### 3.2.1.2 Leaf Node
Contains Key and Value, value could be **Record Ids** (A pointer to the location of the tuple that the index entry corresponds to) or **Tuple Data** (the actual contents of the tuple).   
Once a index stores Tuple Data, the secondary index must store record ids.
![BTreeNode1](/img/DataBase/BTreeNode1.jpeg){:height="70%" width="70%"}
![BTreeNode2](/img/DataBase/BTreeNode2.jpeg){:height="70%" width="70%"}

##### [3.2.2 Operation](https://hldingzydong.github.io/2020/02/15/CMU15-445-Spring2018-Lab2/)
###### 3.2.2.1 Insert 
Find correct leaf node L and put entry into L in sorted order. If L doesn't have enough space, **split** L keys into L and a new node L2.

###### 3.2.2.2 Delete
Find correct leaf node L and remove the entry. If L is half-full, **borrow** from sibling (adjacent node with same parent as L). If borrow fails, **merge** L and sibling, and delete entry from parent of L.

### [3.3 Concurrency Control](https://15445.courses.cs.cmu.edu/fall2019/slides/09-indexconcurrency.pdf)
##### 3.3.1 Hash Index
###### 3.3.1.1 Page Latches
Each page has its own reader-write latch.
![PageLatches](/img/DataBase/PageLatches.jpeg){:height="70%" width="70%"}
###### 3.3.1.2 Slot Latches
Each slot has its own latch.
![SlotLatches](/img/DataBase/SlotLatches.jpeg){:height="70%" width="70%"}

##### [3.3.2 B+Tree Index](https://hldingzydong.github.io/2020/02/15/CMU15-445-Spring2018-Lab2/)
We need to protect from two types of problems:  
> 1. Threads trying to modify the contents of a node at the same time.  
> 2. One thread traversing the tree while another thread splits/merges nodes.

So there is **Latch Crabbing/Coupling** algorithm.
> Protocol to allow multiple threads to access/modify B+Tree at the same time.

###### 3.3.2.1 Basic Idea
> 1. Get latch for parent;  
> 2. Get latch for child;  
> 3. Release latch for parent if child is "**safe**".

A "safe" node is one that will not split or merge when updated, which means the one is **not full on insertion**, or **more than half-full on deletion**.

###### 3.3.2.2 Find
Start at root and go down;  
Repeatedly acquire **R** latch on child, then unlatch parent.

###### 3.3.2.3 Insert / Delete
Start at root and go down, obtaining **W** latches as needed;  
Once child is latched, if it is "safe", release all latches on ancestors.

###### 3.3.2.4 Better Algorithm
**Find**: Same as before;  
**Insert / Delete**: Set latches as if for find, get to leaf and set **W** latch on leaf; If leaf is not "safe", release all latches and restart thread using previous algorithm.  
**Analysis**: This algorithm optimistically assumes that only leaf node will be modified; if not, R latches set on the first pass to leaf are wasteful.

###### 3.3.2.5 Leaf Node Level
Above are "top-down" manner, if we want to move from one leaf to another leaf?
![DeadLock](/img/DataBase/DeadLock.jpeg){:height="70%" width="70%"}
Latches do not support deadlock detection or avoidance. The leaf node sibling latch acquisition protocol must support a "no-wait" mode. The DBMS's data structures must cope with failed latch acquisitions.

### [3.4 Key](https://15445.courses.cs.cmu.edu/fall2019/slides/08-trees2.pdf)
##### 3.4.1 Duplicate Keys
###### 3.4.1.1 Append Record Id
Add the tuple's unique record id as part of the key to ensure that all keys are unique. 
![AppendRecordId](/img/DataBase/AppendRecordId.jpeg){:height="70%" width="70%"}

###### 3.4.1.2 Overflow Leaf Nodes
Allow leaf nodes to spill into overflow nodes that contain the duplicate keys.
![OverflowLeafNodes](/img/DataBase/OverflowLeafNodes.jpeg){:height="70%" width="70%"}


##### 3.4.2 Variable Length Key  

| Approach | Description |
| :-----: | :-----: |
| Pointers | Store keys as pointers to tuple’s attribute |
| Variable Length Nodes | Size of each node can vary |
| Padding (填补) | Pad the key to be max length of the key type |
| Key Map/Indirection (figure shows) | Embed an array of pointers that map to the key + value list within the node |

![KeyMap](/img/DataBase/KeyMap.jpeg){:height="70%" width="70%"}

##### 3.4.3 Find Key In Leaf Node  

| Approach | Description |
| :-----: | :-----: |
| Linear | Scan node keys from beginning to end |
| Binary | Jump to middle key, pivot left/right depending on comparison |
| Interpolation (figure shows) | Approximate location of desired key based on known distribution of keys |

![IntraNode](/img/DataBase/IntraNode.jpeg){:height="50%" width="50%"}

### 3.5 Index Type
##### 3.5.1 Clustered Index
> The table is stored in the sort order specified by the **primary key**.

##### 3.5.2 Non-Unique Index  

| Approach | Description |
| :-----: | :-----: |
| Duplicate Keys | Use the same leaf node layout but store duplicate keys multiple times |
| Value Lists (figure shows)| Store each key only once and maintain a linked list of unique values |

![ValueLists](/img/DataBase/ValueLists.jpeg){:height="70%" width="70%"}

##### [3.5.3 Implicit Index](https://15445.courses.cs.cmu.edu/fall2019/slides/08-trees2.pdf)
> Enforce integrity but not referential constraints (foreign keys), like **Primary Keys** and **Unique Constraints**.

```sql
CREATE TABLE foo(
    id SERIAL PRIMARY KEY,
    val1 INT NOT NULL,
    val2 VARCHAR(32) UNIQUE
);
```
##### 3.5.4 Covering Index
> If all the fields needed to process the query are available in an index, then the DBMS does not need to retrieve the tuple.

```sql
CREATE INDEX idx_foo ON foo(a,b);
```
##### 3.5.5 Partial Index
> Create an index on a subset of the entire table. 

```sql
CREATE INDEX idx_foo ON foo(a,b) WHERE c = 'WuTang';
```
##### 3.5.6 Index Include Columns
> Embed additional columns in indexes to support index-only queries. These extra columns are only stored in the leaf nodes and are not part of the search key.

```sql
CREATE INDEX idx_foo ON foo(a,b) INCLUDE (c);
```
##### 3.5.7 Function/Expression Index
> An index does not need to store keys in the same way that they appear in their base table.

```sql
CREATE INDEX idx_user_login ON users (EXTRACT(dow FROM login));
```
### 3.6 Optimization
###### 3.6.1 Prefix Compression
###### 3.6.2 Suffix Truncation (切断)
###### 3.6.3 Bulk Insert
###### 3.6.4 Pointer Swizzling

# [4. Query](https://15445.courses.cs.cmu.edu/fall2019/slides/10-sorting.pdf)
Here is an article [Following a Select Statement Through Postgres Internals](http://patshaughnessy.net/2014/10/13/following-a-select-statement-through-postgres-internals) pretty good.
![QueryPlan](/img/DataBase/QueryPlan.jpeg){:height="30%" width="30%"}
### 4.1 Architecture Overview
![ArchitectureOverview](/img/DataBase/ArchitectureOverview.jpeg){:height="90%" width="90%"}
The optimizer generates a mapping of a logical algebra expression to the optimal equivalent physical algebra expression. Physical operators define a specific execution strategy using an access path.

### 4.2 Access Data Methods
> An access method is a way that the DBMS can access the data stored in a table

##### 4.2.1 Sequential Scan
**Description**: For each page in the table, retrieve it from the buffer pool and iterate over each tuple and check whether to include it.
```java
for page in table.pages: 
	for t in page.tuples:
		if evalPred(t):
			// Do Something!
```
**Optimizations**: Pre-fetching, Buffer Pool Bypass, Parallelization, Zone Maps, Late Materialization, Heap Clustering.

##### 4.2.2 Index Scan
**Description**: The DBMS picks an index to find the tuples that the query needs.  
![IndexScan](/img/DataBase/IndexScan.jpeg){:height="70%" width="70%"}

##### 4.2.3 Multi-Index Scan
**Description**: If there are multiple indexes that the DBMS can use for a query, could compute sets of record ids using each matching index, then combine these sets based on the query's predicates (union vs. intersect), retrieve the records and apply any remaining predicates.
```sql
SELECT * FROM students
WHERE age < 30 AND dept = 'CS' AND country = 'US'
```
![MultiIndexScan](/img/DataBase/MultiIndexScan.jpeg){:height="70%" width="70%"}

### 4.3 Opeartion
##### 4.3.1 Sort
###### 4.3.1.1 SQL
```sql
SELECT * FROM students ORDER BY age;
```
###### 4.3.1.2 Impl
The main problem is the number of waiting for sorted data cannot fit in memory, so we could choose [External Sort](https://zh.wikipedia.org/wiki/%E5%A4%96%E6%8E%92%E5%BA%8F).  
**Analysis**:
Because we are in DBMS, sorting spends much time in IO, so we pay more attention to IO. Here is the analysis of **K-Way Merge Sort**.
![ExternalMergeSort](/img/DataBase/ExternalMergeSort.jpeg){:height="70%" width="70%"}

Also, we could use **B+Tree** for sorting.   
If the table that must be sorted already has a B+Tree index on the sort attribute(s), then we can retrieve tuples in desired sort order by **traversing the leaf pages of the tree**.  
What's more, need to consider **Clustered B+Tree** and **Unclustered B+Tree**. Unclustered B+Tree will bring too much **random IO**, it is a bad idea.
![ClusteredSort](/img/DataBase/ClusteredSort.jpeg){:height="60%" width="60%"}
![UnclusteredSort](/img/DataBase/UnclusteredSort.jpeg){:height="60%" width="60%"}

##### 4.3.2 Aggregation
###### 4.3.2.1 SQL
```sql
SELECT AVG(gpa), COUNT(login) AS cnt FROM student WHERE login LIKE '%@cs';
SELECT DISTINCT login FROM student;
```
###### 4.3.2.2 Impl
By **Sorting**:
![AggreSort](/img/DataBase/AggreSort.jpeg){:height="80%" width="80%"}


By **Hashing** (is used for unsorted operation, like **GROUP BY** or **DISTINCT**):
> (1)  Partition  

Use a hash function **h1** to split tuples into partitions on disk, so all matches live in the same partition. Assume we have **B** buffers, will use **B-1** buffers for partitions and **1** buffer for the input data.
![Partition](/img/DataBase/Partition.jpeg){:height="70%" width="70%"}

> (2)  ReHash  

Assume that each partition fits in memory, for each partition on disk, read it into memory and build an in-memory hash table base on a second hash function **h2**, then go through each bucket of this hash table to bring together matching tuples.
![ReHash](/img/DataBase/ReHash.jpeg){:height="70%" width="70%"}

>  Hash Summarization

During the ReHash phase, store pairs of the form (**GroupKey→RunningVal**), when insert a new tuple into the hash table, if find a matching **GroupKey**, just update the **RunningVal** appropriately, else insert a new **GroupKey->RunningVal**.
![HashSummarization](/img/DataBase/HashSummarization.jpeg){:height="70%" width="70%"}

**Analysis**:
Because in Phase 1 we have **B-1** "spill partitions", each should be no more than **B** blocks big, so we can hash **B * (B-1)** big table.  
That means if one table has N pages, so Buffer Pool needs at least **sqrt(N)** frames.


##### [4.3.3 Join](https://15445.courses.cs.cmu.edu/fall2019/slides/11-joins.pdf)
###### 4.3.3.1 SQL
```sql
SELECT R.id, S.cdate FROM R JOIN S ON R.id = S.id WHERE S.value > 100;
```
###### 4.3.3.2 Why We Need Join
Avoid unnecessary repetition of information and reconstruct the original tuples without any information loss.

###### 4.3.3.3 Analysis
> What data does the join operator emit to its parent operator?

**Data**: Copy the values for the attributes in outer and inner tuples into a new output tuple, so subsequent operators in the query plan never need to go back to get more data.
![JoinOperatorOutputData](/img/DataBase/JoinOperatorOutputData.jpeg){:height="40%" width="40%"}  
**Record Ids**: Only copy the joins keys along with the record ids of the matching tuples. Ideal for column stores because the DBMS does not copy data that is not needed for the query.
![JoinOperatorOutputRecordIds](/img/DataBase/JoinOperatorOutputRecordIds.jpeg){:height="50%" width="50%"}

> How to determine whether one join algorithm is better than another?

We assume there are **M** pages in table R and **m** tuples in R; **N** pages in S and **n** tuples in S. The cost metric is **# of IOs to compute join**.

###### 4.3.3.4 Impl
> **Nested Loop Join**

Simple, Use block or Use Index. 

> **Sort-Merge Join**  
> (1) **Sort**:  use external sort algorithm sort both tables on the join keys;  
> (2) **Merge**: step through the two sorted tables with cursors and emit matching tuples, but may need to backtrack depending on the join type.

```
sort R,S on join keys;
curso rR ← Rsorted, curso rS ← Ssorted;
while cursorR and cursorS:
	if cursorR > cursorS: 
		increment cursorS 
	if cursorR < cursorS: 
		increment cursorR
	elif cursorR and cursorS match: 
		emit
		increment cursorS
```

Example refer to [slides](https://15445.courses.cs.cmu.edu/fall2019/slides/11-joins.pdf).  
**Analysis**:
![SortMergeJoinCost](/img/DataBase/SortMergeJoinCost.jpeg){:height="60%" width="60%"}
The worst case for the merging phase is when the join attribute of all of the tuples in **both relations contain the same value**, which costs **(M · N) + (sort cost)**.  
When **one or both tables are already sorted on join key**, or **output must be sorted on join key**, this algorithm is useful.

> **Hash Join**  
> (1) **Build**: scan the outer relation and populate a hash table using the hash function **h1** on the join attributes;  
> (2) **Probe**: scan the inner relation and use **h1** on each tuple to jump to a location in the hash table and find a matching tuple.

```
build hash table HTR for R; 
foreach tuple s ∈ S
	output, if h1(s) ∈ HTR
```
 
**Table's Key** is the attribute(s) that the query is joining the tables on.  
**Table's Value** depends on what the operators above the join in the query plan expect as its input. Here shows the two types of value. 

| Type | Advantage | Disadvantage |
| :-----: | :-----: | :-----: |
| Full Tuple | Avoid having to retrieve the outer relation's tuple contents on a match | Takes up more space in memory | 
| Tuple Identifier | Ideal for column stores because the DBMS doesn't fetch data from disk it doesn't need | - |

Example refer to [slides](https://15445.courses.cs.cmu.edu/fall2019/slides/11-joins.pdf). 

> **Grace Hash Join**  (when tables do not fit in memory)  
> (1) **Build**: Hash both tables on the join attribute into partitions;  
> (2) **Probe**: Compares tuples in corresponding partitions for each table.

```
foreach tuple r ∈ bucketR,0: 
	foreach tuple s ∈ bucketS,0:
		emit, if match(r, s)
```

![GraceHashJoin](/img/DataBase/GraceHashJoin.jpeg){:height="90%" width="90%"}
**Grace Hash Join Analysis**:  
**Partition** -> Read + Write both tables, 2(M + N) IOs;  
**Probing** -> Read both tables, (M + N) IOs.  
So, totally costs **3(M + N)** IOs.


### [4.4 Process Model](https://15445.courses.cs.cmu.edu/fall2019/slides/12-queryexecution1.pdf)
> Process Model defines how the system executes a query plan.

##### 4.4.1 Iterator Model
Each query plan operator implements a **Next** function. On each invocation, the operator returns either **a single tuple** or a **null** marker if there are no more tuples. The operator implements a loop that calls next on its children to retrieve their tuples and then process them.

##### 4.4.2 Materialization Model
Each operator processes its input all at once and then emits its output all at once. The operator "materializes" its output as a single result, could be either **whole tuples (NSM)** or **subsets of columns (DSM)**. It is better for OLTP workloads because queries only access a small number of tuples at a time.

##### 4.4.3 Vectorized/Batch Model
Like the Iterator Model where each operator implements a **Next** function in this model. Each operator emits **a batch of tuples** instead of a single tuple. Ideal for OLAP queries because it greatly reduces the number of invocations per operator.

##### 4.4.4 Plan Processing Direction
**Top-to-Bottom** or **Bottom-to-Top**

### 4.5 Parallel
##### 4.5.1 Parallel Process Model

| Approach | Description | Figure |
| :-----: | :-----: | :-----: |
| Process Per Worker | Each worker is a separate OS process | ![ProcessPerWorker](/img/DataBase/ProcessPerWorker.jpeg) |
| Process Pool | A worker uses any process that is free in a pool | ![ProcessPool](/img/DataBase/ProcessPool.jpeg) |
| Thread Per Worker | Single process with multiple worker threads | ![ThreadPerWorker](/img/DataBase/ThreadPerWorker.jpeg) |

##### 4.5.2 Parallel Execution
###### 4.5.2.1 Inter-Query
> Different queries are executed concurrently, which increases throughput & reduces latency.

###### 4.5.2.2 Intra-Query
> Execute the operations of a single query in parallel, which decreases latency for long-running queries, like *producer/consumer* paradigm.

There are three ways to implement Intra-Query.
> Intra-Operator (Horizontal)

Decompose operators into independent **fragments** that perform the same function on different subsets of data. The DBMS inserts an **exchange** operator into the query plan to coalesce results from children operators.
![IntraOperator](/img/DataBase/IntraOperator.jpeg){:height="70%" width="70%"}
Here are types of exchange operator:

| Type | Description |
| :-----: | :-----: |
| Gather | Combine the results from multiple workers into a single output stream |
| Repartition | Reorganize multiple input streams across multiple output streams |
| Distribute | Split a single input stream into multiple output streams | 

> Inter-Operator (Vertical) (pipelined parallelism)

Operations are overlapped in order to pipeline data from one stage to the next without materialization.
![InterOperator](/img/DataBase/InterOperator.jpeg){:height="70%" width="70%"}

> Bushy

Extension of inter-operator parallelism where workers execute multiple operators from different segments of a query plan at the same time.
```sql
SELECT * FROM A JOIN B JOIN C JOIN D;
```
![Bushy](/img/DataBase/Bushy.jpeg){:height="30%" width="30%"}

##### 4.5.3 I/O Parallelism
Using additional processes/threads to execute queries in parallel won't help if the **disk is always the main bottleneck**. So we could split the DBMS installation across multiple storage devices.

###### 4.5.3.1 Multi-Disk Parallelism
Configure OS/hardware to store the DBMS's files across multiple storage devices.
![MultiDiskParallelism](/img/DataBase/MultiDiskParallelism.jpeg){:height="30%" width="30%"}

###### 4.5.3.2 DataBase Partitioning
Specify the disk location of each individual database.

> Vertical Partitioning

Store a table’s **attributes in a separate location** (e.g., file, disk volume). Have to store tuple information to reconstruct the original record.
```sql
CREATE TABLE foo ( 
	attr1 INT, attr2 INT, attr3 INT, attr4 TEXT );
```
![VerticalPartitioning](/img/DataBase/VerticalPartitioning.jpeg){:height="75%" width="75%"}

> Horizontal Partitioning

Divide the tuples of a table up into disjoint segments based on some partitioning key, like hash partition.
![HorizontalPartitioning](/img/DataBase/HorizontalPartitioning.jpeg){:height="75%" width="75%"}

### 4.6 Optimization
##### 4.6.1 Why We Need Optimizer
SQL is declarative, means that the user tells the DBMS what answer they want, not how to get the answer. Thus, the DBMS needs to translate a SQL statement into an executable query plan. But there are different ways to execute a query (e.g., join algorithms) and there will be differences in performance for these plans. Thus, the DBMS needs a way to pick the “best” plan for a given query.  

Here are two types of optimization strategies:  
- **Heuristics/Rules:** Rewrite the query to remove inefficiencies. Does not require a cost model.  
- **Cost-based Search**: Use a cost model to evaluate multiple equivalent plans and pick the one with the smallest cost.

##### 4.6.2 Heuristics / Rules
Two relational algebra expressions are equivalent if they generate the same set of tuples, so DBMS will rewrite the query plan, this technique called **Query Rewriting**.  
Here are some examples of Query Rewriting:  

- **Predicate Push-down:** Perform predicate filtering before join to reduce size of join.
![PredicatePushDown](/img/DataBase/PredicatePushDown.jpeg){:height="65%" width="65%"}

- **Projections Push down**: Perform projections early to create smaller tuples and reduce intermediate results. You can project out all attributes except the ones requested or required (e.g. join attributes).  
![ProjectionPushDown](/img/DataBase/ProjectionPushDown.jpeg){:height="65%" width="65%"}

- **Expression Simplification**: Exploit the transitive properties of boolean logic to rewrite predicate expressions into a more simple form.


##### 4.6.3 Cost-based Search
The DBMS’s optimizer will use an **internal cost model** to estimate the execution cost for a particular query plan. This provides an estimate to determine whether one plan is better than another **without having to actually run the query** (which would be slow to do for thousands of plans).  
To accomplish this, the DBMS stores **internal statistics** about tables, attributes, and indexes in its internal catalog.





# 5 Concurrency Control





# [6.   Recovery](https://15445.courses.cs.cmu.edu/fall2019/slides/20-logging.pdf)
Recovery algorithms are techniques to ensure database consistency, transaction atomicity and durability despite failures.  
It has two parts:  
> Actions **during normal txn processing** to ensure that the DBMS can recover from a failure.  
> Actions **after a failure to recover** the database to a state that ensures atomicity, consistency, and durability.

There are two important concepts: Redo and Undo
> **Redo**:The process of re-instating the effects of a **committed** txn for durability.  
> **Undo**:The process of removing the effects of an **incomplete or aborted** txn.

### 6.1 Before Crash
> **Steal**: Whether the DBMS allows an uncommitted txn to overwrite the most recent committed value of an object in **non-volatile** storage.  
> **Force**: Whether the DBMS requires that all updates made by a txn are reflected on non-volatile storage **before** the txn is allowed to commit.

![NoStealForce](/img/DataBase/NoStealForce.jpeg){:height="70%" width="70%"}

##### 6.1.1 No-Steal + Force
No-Steal means the changes by an uncommited txn were not written to disk, so no need to undo; Force means all changes are written to disk at commit time, so no need to redo.  
>**Shadow Paging** is maintaining two separate copies of the database: **Master** and **Shadow**. 

**Master** contains only changes from commited txns.  
**Shadow** is temporary database with changes made from uncommmited txns, so txns only make updates in the shadow copy.  
When a txn commits, the **root** points automatically switch the shadow to become the new master. So when need to "undo", just remove the shadow pages, leave the master and the root pointer alone.
![ShadowPaging](/img/DataBase/ShadowPaging.jpeg){:height="70%" width="70%"}

##### 6.1.2 Steal + No-Force
> **Write-Ahead Log (WAL)** is maintaining a log file in **volatile storage** that contains the changes that txns make to database.   

So after crash, it can perform undo and redo actions to recovery.   
The most important is "**DBMS must write to disk the log file records that correspond to changes made to a database object before it can flush that object to disk**." When a txn's logs have been written to disk, it can be consider commited. (More details refer to [lab4](https://hldingzydong.github.io/2020/02/29/CMU15-445-Spring2018-Lab4/))
![WAL](/img/DataBase/WAL.jpeg){:height="80%" width="80%"}

WAL will grow forever and after crash, recovery takes a long time, so DBMS periodically takes a **checkpoint** where it **flushes all buffers** (including all log records currently residing in main memory and all modified blocks) out to disk.

##### 6.1.3 Compare
![CompareRecoveryPolicy](/img/DataBase/CompareRecoveryPolicy.jpeg){:height="70%" width="70%"}
Because the crash is rare, DBMS choose No-Force + Steal.

### [6.2 After Crash](https://15445.courses.cs.cmu.edu/fall2019/slides/21-recovery.pdf)
##### 6.2.1 Log Sequence Number (LSN)
> LSN is a globally unique id of log.

| Name | Where | Definition |
| :-----: | :-----: | :-----: |
| **flushedLSN** | Memory | **Last** LSN in log on disk |
| **pageLSN** | Page | **Newest** update to page |
| **recLSN** | Page | **Oldest** update to page since it was last flushed |
| **lastLSN** | Txn | **Latest** record of txn Ti |
| **MasterRecord** | Disk | LSN of **latest** checkpoint | 

Before page x can be written to disk, must flush log firstly, which means **pageLSN(x) <= flushedLSN**.

##### 6.2.2 Compensation Log Records (CLR)
How to deal with **ABORTED** txn during undo phase? We need to add **prevLSN** field in log records.
> prevLSN: the previous LSN for this log.

![prevLSN](/img/DataBase/prevLSN.jpeg){:height="70%" width="70%"}

What's more, we need Compensation Log Records (CLR).
> A CLR has all the fields of an update log record plus the **undoNext** pointer (the next-to-be-undone LSN), describes the actions taken to undo the actions of a previous update record.  

![CLR](/img/DataBase/CLR.jpeg){:height="70%" width="70%"}

So when we need to deal with **ABORTED** txn, firstly write a **CLR** entry to the log, then restore old value.Remember, CLRs never to be undone.

##### 6.2.3 ARIES 
Algorithms for Recovery and Isolation Exploiting Semantics (**ARIES**) describe the actions after a crash to recovery.  
There is a [video](https://www.youtube.com/watch?v=S9nctHdkggk) explain ARIES algorithm quite clearly.
![ARIES](/img/DataBase/ARIES.jpeg){:height="80%" width="80%"}
###### 6.2.3.1 Analysis Phases
> Read WAL from **last checkpoint** to identify dirty pages in the buffer pool and active txns at the time of the crash.

In this phase, we are going to build two tables: DPT (Dirty Page Table) and ATT (Active Transaction Table).  
**DPT** records which pages in the buffer pool contain changes from uncommited txns, is used **for Redo** Phase, will record pageId, recLSN. During scanning, if we meet a **UPDATE** records and correspond page **P** not in DPT, we add P to DPT and set its **recLSN = (current)LSN**.  
**ATT** records currently active and unfinished txns **for Undo** Phase, will record txnId, status and lastLSN.   

###### 6.2.3.2 Redo Phases
> Repeat **all** actions (even **aborted txns** and **CLRS**) starting from an appropriate point (the log record containing **smallest recLSN**) in the log.

Remember, it is different from **Redo** concept.

###### 6.2.3.3 Undo Phases
> Reverse the actions of txns that **did not commit** before the crash. Process them in **reverse LSN order** using **lastLSN**, write a **CLR** for every modification.

![FullExample](/img/DataBase/FullExample.jpeg)


# Conclusion
In this article, we describe the most important components of DBMS, thanks to [CMU-15-445](https://15445.courses.cs.cmu.edu/fall2019/schedule.html), this course is very great. I hope I will explore more about DBMS in the future.
























