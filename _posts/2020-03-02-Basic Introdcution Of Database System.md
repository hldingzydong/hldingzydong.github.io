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

# 1.   Storage
The Database Manage System we are talking is a **disk-oriented** System.
![DiskOrientedDBMS](/img/DataBase/DiskOrientedDBMS.jpeg)

### 1.1 File Storage
The DBMS stores a database as one or more files on disk and OS doesn't know anything about the contents of these files. The Storage Manager organize the files as a collection of pages.

### 1.2 Page Storage
> A page is a fixed-size block of data. It can contain tuples, meta-data, indexes or log records.  

##### 1.2.1 Files Layout
There are three ways to organize pages inside a file.  
- **Heap File Organization**: an unordered collection of pages where tuples are stored in random order. So pages need meta-data to track what pages exist and which one have free space. Here you can see two ways to represent:
<figure class="half">
	<img src="/img/DataBase/LinkedList.jpeg", height="70%", width="70%">
	<img src="/img/DataBase/PageDirectory.jpeg", height="70%", width="70%">
</figure>

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

### 1.4 Data Representation Inside Tuples
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
> On-line Transaction Processing(OLTP), simple queries that read/update a small amount of data that is related to a single entity in the database.(平时做web后端的对DB的CRUD)

```sql
UPDATE useracct SET lastLogin = NOW(), hostname = ? WHERE userID = ?
```
> On-line Analytical Processing(OLAP), complex queries that read large portions of the database spanning multiple entities.(类似于大数据)

```sql
SELECT COUNT(U.lastLogin), EXTRACT(month FROM U.lastLogin) AS month
FROM useracct AS U
WHERE U.hostname LIKE '%.gov'
GROUP BY
EXTRACT(month FROM U.lastLogin)
```

##### 1.5.2 NSM vs DSM
> N-ary Storage Model(NSM aka “row storage”).The DBMS stores all attributes for a single tuple contiguously in a page.  

![DisNSM](/img/DataBase/DisNSM.jpeg){:height="50%" width="50%"}

> Decomposition(分解) Storage Model(DSM aka “column store”). The DBMS stores the values of a single attribute for all tuples contiguously in a page.  

![DSM](/img/DataBase/DSM.jpeg){:height="50%" width="50%"}



# 2.   Buffer Pool Manager(BPM)
### 2.1 Why Need BPM
### 2.2 Component
### 2.3 Optimization





# 3 Index(3)

# 4 Query(3+3)

# 5 Concurrency Control(4)

# 6 Log And Recovery(2+1)

# Conclusion


























