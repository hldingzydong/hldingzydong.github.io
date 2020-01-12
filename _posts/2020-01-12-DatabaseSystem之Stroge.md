---
layout: post
title:  DatabaseSystem之Stroge
date:   2020-01-12
author: hding
catalog: true
tags:
   - database
---
### 面向disk的architecture  
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
	  A heap file is an unordered collection of pages where tuples that are stored in random order. Need meta-data to keep track of what pages exist and which ones have free space. Two ways to represent a heap file: Linked List || Page Directory.
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
- Tuple oriented
	![Tuple Storage](/img/TupleStorage.jpeg)
	![Slotted Pages](/img/SlottedPages.jpeg)Log
- Log-structured
	![Log-Structured File Organization I](/img/LogStructured1.jpeg)
	![Log-Structured File Organization II](/img/LogStructured2.jpeg)



##### Tuple Layout 
A tuple is essentially a sequence of bytes. It's the job of the DBMS to interpret(解释、说明) those bytes into attribute types and values.










  











	













































