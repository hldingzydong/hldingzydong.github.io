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
This article briefly introduces the Database Manage System(DBMS), including its Storage, Buffer Pool Manager, Index, Query, Concurrenct Control, Log and Recovery. A Database Manage System is built by combining these components.  

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
Some systems(Oracle) require pages to be **self-contained**.

### 1.3 Tuple Storage
Assuming inside a page, we only store tuples, let’s discuss how to organize the data stored.
> A **tuple** is a set of attribute values (also known as its domain) in the relation.

##### 1.3.1 Pages Layout 
There are two ways to have pages layout: **slotted pages** and **log records**.
![SlottedPages](/img/DataBase/SlottedPages.jpeg){:height="70%" width="70%"}
![LogStructured](/img/DataBase/LogStructured1.jpeg){:height="70%" width="70%"}

##### 1.3.2 Tuples Layout
Each tuple is prefixed with a header that contains meta-data about: **Visibility info(Concurrency Control)**, Bit Map for NULL values.  
We don't need to store meta-data about the schema because attribute are typically stored in the order that we specify them when creating the table.
![TupleData](/img/DataBase/TupleData.jpeg)

##### 1.3.3 Record Ids
DBMS uses a unique record identifier(**page_id + offset / slot**) to track individual tuples.

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
> **On-line Transaction Processing(OLTP)**, simple queries that read/update a small amount of data that is related to a single entity in the database.(平时做web后端的对DB的CRUD)

```sql
UPDATE useracct SET lastLogin = NOW(), hostname = ? WHERE userID = ?
```
> **On-line Analytical Processing(OLAP)**, complex queries that read large portions of the database spanning multiple entities.(类似于大数据)

```sql
SELECT COUNT(U.lastLogin), EXTRACT(month FROM U.lastLogin) AS month
FROM useracct AS U
WHERE U.hostname LIKE '%.gov'
GROUP BY
EXTRACT(month FROM U.lastLogin)
```

##### 1.5.2 NSM vs DSM
> **N-ary Storage Model(NSM aka “row storage”)**.The DBMS stores all attributes for a single tuple contiguously in a page.  

![DisNSM](/img/DataBase/DisNSM.jpeg){:height="50%" width="50%"}

> **Decomposition(分解) Storage Model(DSM aka “column store”)**. The DBMS stores the values of a single attribute for all tuples contiguously in a page.  

![DSM](/img/DataBase/DSM.jpeg){:height="50%" width="50%"}


 
# [2.   Buffer Pool Manager (BPM)](https://15445.courses.cs.cmu.edu/fall2019/notes/05-bufferpool.pdf)
![DiskOrientedDBMS](/img/DataBase/DiskOrientedDBMS.jpeg)
### 2.1 Why Need BPM
According to **spatial locality** and **temporal locality**, BPM could minimize the time cost of getting a page from disk. We build our own BPM and not use OS‘s cache because we could use suitable evict algorithm to evict a page, and we could set the size of Buffer Pool.

### 2.2 Component
It is a **memory region** organized as an array of fixed-size pages. An array entry is called a **frame**. There is a **page table**, mapped from page_id to frame_id. For every pages in Buffer Pool, each of them need to contains Dirty Flag and Pin/Reference Counter.  
**Dirty Flag** is used to identify whether a page is modified after read into memory. When a dirty page is evicted, DBMS needs to write it back to disk.  
**Pin/Reference Counter** is used to decide whether this page is evicted. When Pin/Reference Counter is 0, apply Replacement Policy to this page.
![BufferPoolManager](/img/DataBase/BufferPoolManager.jpeg){:height="60%" width="60%"}

### 2.3 Optimization
Multiple Buffer Pools, Pre-Fetching, Scan Sharing and Bypass().

### 2.4 Replacement Policy
##### 2.4.1 Least Recently Used(LRU)
Maintain a timestamp of when each page was last accessed. When DBMS needs to evict a page, pick the oldest.

##### 2.4.2 Clock
![Clock](/img/DataBase/Clock.jpeg){:height="70%" width="70%"}

##### 2.4.3 LRU-K
##### 2.4.4 Localization
##### 2.4.5 Priority Hints


# 3 Index(3)

# 4 Query(3+3)

# 5 Concurrency Control(4)

# [6.   Log And Recovery](https://15445.courses.cs.cmu.edu/fall2019/slides/20-logging.pdf)
Recovery algorithms are techniques to ensure database consistency, transaction atomicity and durability despite failures.  
It have two parts:  
- Actions **during normal txn processing** to ensure that the DBMS can recover from a failure.  
- Actions **after a failure to recover** the database to a state that ensures atomicity, consistency, and durability.


### 6.1 Before Crash
> **Steal**: Whether the DBMS allows an uncommitted txn to overwrite the most recent committed value of an object in **non-volatile** storage. 
 
> **Force**: Whether the DBMS requires that all updates made by a txn are reflected on non-volatile storage **before** the txn is allowed to commit.

![NoStealForce](/img/DataBase/NoStealForce.jpeg){:height="70%" width="70%"}

##### 6.1.1 No-Steal + Force
No-Steal means the changes by an uncommited txn were not written to disk, so no need to undo; Force means all changes are written to disk at commit time, so no need to redo.  
>**Shadow Paging** is maintaining two separate copies of the database: **Master** and **Shadow**. 

**Master** contains only changes from commited txns; **Shadow** is temporary database with changes made from uncommmited txns, so txns only make updates in the shadow copy.  
When a txn commits, the **root** points atomically switch the shadow to become the new master. So when need to "undo", just remove the shadow pages, leave the master and the root pointer alone.
![ShadowPaging](/img/DataBase/ShadowPaging.jpeg){:height="70%" width="70%"}

##### 6.1.2 Steal + No-Force
> **Write-Ahead Log(WAL)** is maintaing a log file in **volatile storage** that contains the changes that txns make to database.   

So after crash, it can perform undo and redo actions to recovery.   
The most important is "**DBMS must write to disk the log file records that correspond to changes made to a database object before it can flush that object to disk**." When a txn's logs have been written to disk, it can be consider commited.(More details refer to [lab4](https://hldingzydong.github.io/2020/02/29/CMU15-445-Spring2018-Lab4/))
![WAL](/img/DataBase/WAL.jpeg){:height="80%" width="80%"}

WAL will grow forever and after crash, recovery takes a long time, so DBMS periodically takes a **checkpoint** where it **flushes all buffers**(including all log records currently residing in main memory and all modified blocks) out to disk.

##### 6.1.3 Compare
![CompareRecoveryPolicy](/img/DataBase/CompareRecoveryPolicy.jpeg){:height="70%" width="70%"}
Because the crash is rare, DBMS choose No-Fore + Steal.


### [6.2 After Crash](https://15445.courses.cs.cmu.edu/fall2019/slides/21-recovery.pdf)
##### 6.2.1 Log Sequence Number(LSN)
> LSN is a globally unique id of log.

| Name | Where | Definition |
| :-----: | :-----: | :-----: |
| **flushedLSN** | Memory | **Last** LSN in log on disk |
| **pageLSN** | Page | **Newest** update to page |
| **recLSN** | Page | **Oldest** update to page since it was last flushed |
| **lastLSN** | Txn | **Latest** record of txn Ti |
| **MasterRecord** | Disk | LSN of **latest** checkpoint | 

##### 6.2.2 Compensation Log Records(CLR)

##### 6.2.3 ARIES 
Algorithms for Recovery and Isolation Exploiting Semantics(**ARIES**) describe the actions aftre a crash to recovery.  
There is a [video](https://www.youtube.com/watch?v=S9nctHdkggk) explain ARIES algorithm quite clearly.
###### 6.2.3.1 Analysis


###### 6.2.3.2 Redo
> **Redo**:The target is re-instating the effects of a **committed** txn for durability. Still redo all txns firstly, through "Undo" can reach the target.



###### 6.2.3.3 Undo
> **Undo**:The process of removing the effects of an **incomplete or aborted** txn.



# Conclusion


























