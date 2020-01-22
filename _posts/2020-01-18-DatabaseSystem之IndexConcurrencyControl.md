---
layout: post
title:  DatabaseSystem之IndexConcurrencyControl
date:   2020-01-18
author: hding
catalog: true
tags:
   - database
---
## Concurrency Control

#### 正确条件
- 逻辑正确: 我看到的data是否是我应该看到的?
- 物理正确: Is the internal representation of the object sound?

#### Latches Overview

###### Locks vs Latches

|      | Locks | Latches |
| :-----: | :-----: | :-----: |
| Seperate | 用户的事务 | 线程 |
| Protect | 数据库内容 | 内存中的数据结构 |
| During | 整个事务 | 临界区 |
| Modes | 共享、执行、更新、Intention | 读、写 |
| Deadlock | detection & resolution | 回避 |
| ...by... | waits-for、timeout、aborts | coding discipline |
| kept in... | Lock Manager | 被保护的数据结构 |  


###### Latch Modes
- Read Mode
- Wirte Mode

###### Latch Implementations
- Blocking OS Mutex
```c++
std::mutex m;
//do something
m.lock();
//do something special
m.unlock();
```

- Test-and-Set Spin Latch(TAS)

> 引自[Wikipedia](https://zh.wikipedia.org/wiki/%E6%A3%80%E6%9F%A5%E5%B9%B6%E8%AE%BE%E7%BD%AE):  
> 在计算机科学中，检查并设置（test-and-set-lock，TSL）是一种不可中断的原子运算。TSL对某个存储器位置写入1（set）并返回其旧值。  
> 在多个进程可同时访问存储器同个地址时，如果一个程序正在运行TSL，其他程序在它运行完成前不能运行TSL。
```c++
function Lock(boolean *lock) {
    while (test_and_set(lock) == 1)
      ;
}
```
> 上述述为一种利用TSL指令来实现**自旋锁**,当旧值为0时，这程序可以得到锁。否则的话，它会一直尝试将1写入存储器位置，直到旧值为0。

> 锁(lock)的状态一般是0(未锁)与1(已锁)。因此下列test_and_set的实现是等价的：  
> if (lock==0) then 置锁, 进入临界区; else 忙等待, 重新测试; endif  
> 读取lock状态; lock被置为1; 测试读出的lock状态，判断进入临界区还是忙等待;


- Reader-Writer Latch
 使用时需要小心管理read/write队列来避免starvation
![Read Write Lock](/img/DataBase/ReadWriteLock.jpeg)



#### Hash Table Latching
Hash Table很容易支持concurrent因为访问Hash Table的方式有限,所有的thread必须以相同方向读table.每一次只会access一个page/slot，deadlock不可能发生.但是为了resize table,需要对整张table用一个global latch(如在header page上)
- Page Latch: 每页page都有自己的read-write latch
![Page Latches](/img/DataBase/PageLatches.jpeg)

- Slot Latch: 每个slot都有自己的latch，可以使用一个简易模式的latch来减少meta-data and
computational overhead.
![Slot Latches](/img/DataBase/SlotLatches.jpeg)



#### B+Tree Concurrency Control
对于B+Tree的并发控制，我们希望可以允许多个线程同一时间read/update B+Tree,那么我们要确保一下两个问题得到解决:
1. 在同一时间多个threads想要modify一个node的content.
2. 某个thread在遍历tree的同时，有其他thread在split/merge nodes.


###### Latch Crabbing/Coupling
允许多线程在同一时间access/modify B+Tree。大致思路为:
> 给 parent 拿把 latch
> 给 child 拿把 latch  
> 如果"safe"的话,就 release 掉 parent 的 latch
>
> safe node是指当被 update 时,不会发生 split 或 merge 的 node (insert时 Not full, delete 时超过 half-full)

Find: 从root开始往下找，重复如此.  
	  在child上拿一把 Read latch，再release掉parent的latch。  
![Find Node](/img/DataBase/FindNode.jpeg)

Insert/Delete: 从root开始，重复如此,如果需要的话拿一把 Write Latch。一旦child被latch,就check它是否安全. 如果child安全,就release它的ancestors的latch.
![Delete Node1](/img/DataBase/DeleteNode1.jpeg)
![Delete Node2](/img/DataBase/DeleteNode2.jpeg)
![Insert Node](/img/DataBase/InsertNode.jpeg)



###### Better Latching Algorithm
![Origin Latch](/img/DataBase/OriginLatch.jpeg)

该算法假设leaf node是safe的,使用read lathes，一步步向下crab直到到达该leaf node, 然后verify它是不是safe的. 如果该leaf node不是safe的,就使用Write Latch跑之前的算法.  
![Better Algorithm](/img/DataBase/BetterAlgorithm.jpeg)

Insert: 和previous一样
Insert/Delete: 就像search一样set latch，到达leaf时，对其set Write latch。如果leaf不安全,就释放掉所有的latches,重新开始，使用Write Latch跑之前的insert/delete算法。  

分析:假设只有叶结点会被modified,则该算法是有效的；否则的话,第一轮遍历所set的Read Latches就浪费了.


###### Leaf Node Scan
在之前的例子中所有的thread去获取latches都是"top-down",即一个thread只能从它当前所在的node的child上请求latch，如果desired latch不可获得,该thread必须wait.  
那么如果我们想要从一个叶结点scan至另一个叶结点呢?  
![Two Read](/img/DataBase/TwoRead.jpeg)
![Dead Lock](/img/DataBase/DeadLock.jpeg)

Latches不支持deadlock 探测或避免。想要解决上述问题，我们只能靠我们的代码discipline.  
叶结点的兄弟姐妹获取latches的政策，必须支持"no-wait" mode。DBMS的数据结构必须可以应对latches获取失败的case.  

因此我们有了"Delayed Parent Updates"策略。  
每当有一个叶结点overflow时，我们必须更新至少三个nodes:
- 被分裂的叶结点
- 被新建的node
- parent node

Blink-Tree Optimization: 当一个叶结点overflow时,延迟更新其parent node
见[slide](https://15445.courses.cs.cmu.edu/fall2019/slides/09-indexconcurrency.pdf)









