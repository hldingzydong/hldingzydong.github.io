---
layout: post
title:  DatabaseSystem之Stroge
date:   2020-01-12
author: hding
catalog: true
tags:
   - database
---
### [面向disk的architecture](https://15445.courses.cs.cmu.edu/fall2019/slides/03-storage1.pdf)
首先查看storage hierarchy:
![storage hierarchy](https://blobscdn.gitbook.com/v0/b/gitbook-28427.appspot.com/o/assets%2F-LMjQD5UezC9P8miypMG%2F-LY_HB8UaEfE1efciC8V%2F-LY_K0SNgM4yb-lsVBlJ%2FScreen%20Shot%202019-02-13%20at%201.28.29%20PM.jpg?alt=media&token=8cd28260-ebb5-4729-8a41-732675a64afc)

![access times](https://blobscdn.gitbook.com/v0/b/gitbook-28427.appspot.com/o/assets%2F-LMjQD5UezC9P8miypMG%2F-LY_HB8UaEfE1efciC8V%2F-LY_Kgs6xp4XVNA9n-FF%2FScreen%20Shot%202019-02-13%20at%201.31.21%20PM.jpg?alt=media&token=f4dade9f-4870-4c87-83bb-bd419e087ce1)


DBMS(DataBase Manage System)数据的首要存储位置在non-volatile的disk(SSD、HDD、Network Storage)。这样DBMS可以存储管理更多的数据，但是对disk的读写IO很昂贵。


### DBMS是如何在non-volatile的disk上存储数据呢？
![get a page](/img/DiskOrientedDBMS.jpeg)

##### File Storage  
**The DBMS stores a database as one or more files on disk**. The OS doesn't know anything about the contents of these files.

The **storage manager** is responsible for maintaining a database's files.It organizes the files as a collection of **pages**.(tracks data read/writtenm to pages and tracks the available space).

A **page** is a fixed-size block of data. Each page is given a unique identifier.
1. It can contain tuples, meta-data, indexes, log records...
2. Most systems do not mix page types.
3. Some systems require a page to be self-contained(every page contain meta-data).
4. There different notions of "pages" in a DBMS:
	- Hardware Page(usually 4KB, at this level the device can guarantee a "failsafe write")
	- OS Page(usually 4KB)
	- Database Page(512B-16KB)
5. Different DBMSs manage pages in files on disk in different ways:
	- **Heap File Organization**
	  A heap file is an unordered collection of pages where tuples that are stored in random order. Need meta-data to keep track of what pages exist and which ones have free space. Two ways to represent a heap file:   
	  **Linked List** || **Page Directory**
	  ![Linked List](/img/LinkedList.jpeg)

	  ![Page Directory](/img/PageDirectory.jpeg)

	- Sequential / Sorted File Organization
	- Hashing File Organization


##### Page Layout
Every page contains a **header** of metadata about the page's contents:
- Page Size
- Checksum
- DBMS Version
- Transaction Visibility
- Compression Information
Some systems require pages to be **self-contained** (e.g., Oracle).

Assuming we only store tuples, let's discuss how to organize the data stored inside of page.
- **Tuple oriented**
	![Tuple Storage](/img/TupleStorage.jpeg)
	![Slotted Pages](/img/SlottedPages.jpeg)
- **Log-structured**
	![Log-Structured File Organization I](/img/LogStructured1.jpeg)
	![Log-Structured File Organization II](/img/LogStructured2.jpeg)



##### Tuple Layout 
A tuple is essentially a sequence of bytes. It's the job of the DBMS to interpret(解释、说明) those bytes into attribute types and values.

Each tuple is prefixed with a header that contains meta-data about it:
- Visibility info (concurrency control)
- Bit Map for **NULL** values
We do not need to store meta-data about the schema. Attribute are typically stored in the order that you specify them when you create the table.
![Tuple Data](/img/TupleData.jpeg)  

###### Denormalized Tuple Data
Can physically denormalize (e.g., "prejoin") related tuples and store them together in the same page. Potentially reduces the amount of I/O for common workload patterns. Can make updates more expensive.
![Denormalized Tuple Data 1](/img/DenormalizedTupleData1.jpeg)

![Denormalized Tuple Data 2](/img/DenormalizedTupleData2.jpeg)

###### Record Ids
DBMS needs a way to keep track of individual tuples. Each tuple is assigned a unique record identifier. Most common: **page_id + offset/slot**. Can also contain file location info.


##### Data Representation 
- INTEGER/BIGINT/SMALLINT/TINYINT (C/C++ Representation)
- FLOAT/REAL vs NUMERIC/DECIMAL   (IEEE-754 Standard/Fixed-point Decimals)
	- Variable-precision numberic type that uses the "native" C/C++ types, eg: **FLOAT**,**REAL/DOUBLE**, stored directly as specified by IEEE-754, typically **faster** than arbitary precision numbers but can have **rounding errors**. (使用原生的C/C++类型，不需要具体的精度，非常快，但是会有微小的误差)
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

	- Numeric data types with arbitrary precision and scale. Used when round errors are unacceptable, eg: **NUMERIC**, **DECIMAL**, Typically stored in a exact, variable-length binary representation with additional meta-data. (Like a **VARCHAR** but not stored as a string). (这种类型需指明具体的精度是多少，存储时需带有额外的元数据)
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

- VARCHAR/VARBINARY/TEXT/BLOB     (Header with length, followed by data bytes)
	- Large Values
	  Most DBMSs don't allow a tuple to exceed the size of a single page. To store values that are larger than a page, the DBMS uses separate **overflow** storage pages.
	  ![LARGE VALUES](/img/LargeValues.jpeg)

	  Some systems allow you to store a really large value in an external file. Treated as a **BLOB** type. The DBMS **cannot** manipulate the contents of an external file.
	  ![External Value Storage](/img/ExternalValueStorage.jpeg)

- TIME/DATE/TIMESTAMP			  (32/64-bit integer of (micro)seconds since Unix epoch)



##### System Catalogs
A DBMS stores meta-data about databases in its internal catalogs:
- Tables, columns, indexes, views
- Users, permissions
- Internal statistics
You can query the DBMS’s internal **INFORMATION_SCHEMA** catalog to get info about the database.
```sql
SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE table_catalog = '<db name>';
SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE table_name = 'student';
```



##### Storage Models

###### OLTP(On-line Transaction Processing)
Simple queries that read/update **a small amount** of data that is related to a single entity in the database.(平时做web后端的对DB的CRUD) 
```sql
SELECT P.*, R.*
FROM pages AS P
INNER JOIN revisions AS R
ON P.latest = R.revID
WHERE P.pageID = ?
```
```sql
UPDATE useracct
SET lastLogin = NOW(),
hostname = ?
WHERE userID = ?
```

###### OLAP(On-line Analytical Processing)
Complex queries that read **large** portions of the database spanning multiple entities.(类似于大数据)
```sql
SELECT COUNT(U.lastLogin), EXTRACT(month FROM U.lastLogin) AS month
FROM useracct AS U
WHERE U.hostname LIKE '%.gov'
GROUP BY
EXTRACT(month FROM U.lastLogin)
```

The DBMS can store tuples in different ways that are better for either OLTP or OLAP workloads.
###### N-ary Storage Model(aka "row storage") (NSM)
The DBMS stores all attributes for a single tuple contiguously in a page.
![NSM](/img/NSM.jpeg)

Advantage: 
1. Fast inserts, updates, and deletes;
2. Good for queries that need the **entire** tuple.  
Disadvantage:
1. Not good for scanning large portions of the table and/or a subset of the attributes.
![DisNSM](/img/DisNSM.jpeg)


###### Decomposition(分解) Storage Model(aka "column store") (DSM)
The DBMS stores the values of a single attribute for all tuples contiguously in a page.
![DSM](/img/DSM.jpeg)

![Tuple Identification](/img/TupleIdentification.jpeg)

Advantage: 
1. Reduces the amount wasted I/O because the DBMS only reads the data that it needs.
2. Better query processing and data compression.  
Disadvantage:
1. Slow for point queries, inserts, updates, and deletes because of tuple splitting/stitching.














  











	













































