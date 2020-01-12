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
![get a page](/img/DiskOrientedDBMS.png)

##### File Storage  
**The DBMS stores a database as one or more files on disk**. The OS doesn't know anything about the contents of these files.  

The **storage manager** is responsible for maintaining a database's files.It organizes the files as a collection of **pages**.(tracks data read/writtenm to pages and tracks the available space).





##### Page Layout

##### Tuple Layout 











  











	













































