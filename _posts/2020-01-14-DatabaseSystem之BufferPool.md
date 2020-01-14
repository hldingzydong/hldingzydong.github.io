---
layout: post
title:  DatabaseSystem之BuffeerPool
date:   2020-01-14
author: hding
catalog: true
tags:
   - database
---
### DBMS是如何管理在memory和dish之间的数据移动？
DataBase存储需要做到空间和时间上的控制。

> 空间控制是指，where to write pages on disk.   

目的是为了使经常被使用的pages物理上也离得近些。

> 时间控制是指，When to read pages into memory and when to write them to disk

目的是为了最小化必须从disk中读取数据所消耗的时间。

### Buffer Pool Manager(BPM)
是一块内存区域，由一组大小固定的page组成。每一页page被叫做**frame**。当DBMS请求某一page时，该page的copy就被放置于某一frame中。  
同时还有一张page table， 用来记录page_id与frame_id的映射关系。同时还包含每个page的元数据，如Dirty Flag, Pin/Refere Counter.  
![Buffer Pool Manager](/img/BufferPoolManager.jpeg)

有两种分配BPM的政策:
Global Policies: Make decisions for all active txns(事务). 
Local Policies: Allocate frames to a specific txn without considering the behavior of concurrent txns.Still need to support sharing pages.


##### lock VS latch
locks:
- 使DB的逻辑上的内容远离其他事务
- 在事务持久期间被持有
- 需要支持回滚  
latches:
- 保护DBMS内部的数据结构远离其他线程(mutex)
- 在数据被操作的期间被持有
- 不需要支持回滚

##### Page Table VS Page Directory  
Page Table是在BufferPool中用来完成从page_id到frame_id的映射,即可以根据page_id找到该page在BufferPool中对应的copy(在某个frame中)。这是一个位于memory中的数据结构，不需要被存储在disk中。  
Page Directory是在DB files中用来根据page_id找到page对应的位置.他所有的变化必须被记录在disk上以允许DBMS在restart后可以找到每个page的位置。


#### BP 优化
###### Multiple BPs
对于DBMS，可以支持多个BP，如每个DB一个BP，或者每一种Page一个BP，这样可以减少锁竞争。

###### Pre-Fetching
DBMS可以根据query plan， pre-fetch一些page.
![Pre-Fetch](/img/PreFetch.jpeg)

###### Scan Sharing
当query时，可以重用storage或Operator Computations的结果，这不同于result caching。这样就允许很多query都attach到同一个cursor(光标)来扫描一个table。这里的多个query不一定是一样的，它们可以share一些中间结果。
![Scan Sharing](/img/ScanShare.jpeg)

如果某一query开始scan，但是已经有一个query正在scan，DBMS将把新开始的query的cursor，attach到ing的query的cursor上，DBMS也将track新加入的query是在哪里和之前的query attach到一起的，这样当原来的query scan之后，新加入的query也知道自己该在哪里结束scan。

###### BP Bypass()
对于sequential scan， 不会讲fetch到的page存储在BP中， 来避免BP溢出。当operator需要读一大块连续的page时work well， 也可used for临时数据，如sorting和joins。


#### Buffer Replacement Policies
当DBMS必须从BP中选一个page来evict中时，如何决策呢?  

###### least-recently used
记录每一page上次被获取的时间，每次选oldest的page，让它滚蛋，这样政策下，保持page有序性可以减少serach time on eviction。

###### clock
![Clock](/img/Clock.jpeg)

以上这两种置换政策很容易被sequential flooding影响。在sequential flooding下, 一次query只对每一page进行一次sequential scan, 这样就污染了BP，其中的page只会被使用一次，再也不会被使用了。
![Sequential Scan](/img/SequentialScan.jpeg)


###### LRU-K
DBMS track每个page的last K次引用，将其作为时间戳,计算两次访问之间的间隔。由此估计下一次该page会何时被访问。

###### Localization
DBMS 基于每次执行的事务/query来选择哪一个page被置换出去。这样最小化了BP被每个query污染的情况。比如，Postgres对于每个query维持了一个私有的很小的环状的buffer。


###### Priority Hints
DBMS知道每一page的内容(比如知道某一page存储的是index)，这样可以给BP一些提示哪些page比较重要。
![Priority Hints](PriorityHints.jpeg)


##### Dirty Page
若某一page是脏的(在读入memory后被修改过，与disk中不一样)，那DBMS必须确保它的改变被持久化。这样就会很慢。  
DBMS可以定期的scan page table，将脏页写回disk。当其被写回disk，就unset其dirty flag，或者将其置换出去。 







  











	













































