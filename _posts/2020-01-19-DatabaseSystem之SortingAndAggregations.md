---
layout: post
title:  DatabaseSystem之SortingAndAggregations
date:   2020-01-19
author: hding
catalog: true
tags:
   - database
---
## Query Plan
一次query的operators就像一棵tree, data从tree的leaves流动至root,root的output就是这次query的output。
![Query Plan](/img/DataBase/QueryPlan.jpeg)

我们无法假设一张table的数据可以完全得fit内存,我们的DBMS是面向disk的,一次query的结果可能无法fit in memory。  
We are going use on the buffer pool to implement algorithms that need to spill to disk.  
我们倾向于使用最大化sequential access的算法.

## External Sort(外排序)

引自[Wikipedia](https://zh.wikipedia.org/wiki/%E5%A4%96%E6%8E%92%E5%BA%8F)  
> **外排序**是指能够处理**极大量数据**的排序算法。通常来说，外排序处理的数据不能一次装入内存，只能放在读写较慢的外存储器（通常是硬盘）上。外排序通常采用的是一种**“排序-归并”**的策略。  
> 在**排序阶段**，先读入能放在内存中的数据量，将其排序输出到一个临时文件，依此进行，将待排序数据组织为多个有序的临时文件。  
> 在**归并阶段**将这些临时文件组合为一个大的有序文件，也即排序结果。

#### External Merge Sort(外归并排序)
外排序的一个例子是**外归并排序**，它读入一些能放在内存内的数据量，在内存中排序后输出为一个顺串（即是内部数据有序的临时文件），处理完所有的数据后再进行归并。比如，要对900 MB的数据进行排序，但机器上只有100 MB的可用内存时，外归并排序按如下方法操作:
1. 读入100 MB的数据至内存中，用某种常规方式（如快速排序、堆排序、归并排序等方法）在内存中完成排序。  
2. 将排序完成的数据写入磁盘。
3. 重复步骤1和2直到所有的数据都存入了不同的100 MB的块（临时文件）中。在这个例子中，有900 MB数据，单个临时文件大小为100 MB，所以会产生9个临时文件。
4. 读入每个临时文件（顺串）的前10 MB（ = 100 MB / (9块 + 1)）的数据放入内存中的输入缓冲区，最后的10 MB作为输出缓冲区。（实践中，将输入缓冲适当调小，而适当增大输出缓冲区能获得更好的效果。）
5. 执行九路归并算法，将结果输出到输出缓冲区。一旦输出缓冲区满，将缓冲区中的数据写出至目标文件，清空缓冲区。一旦9个输入缓冲区中的一个变空，就从这个缓冲区关联的文件，读入下一个10M数据，除非这个文件已读完。这是“外归并排序”能在主存外完成排序的关键步骤 -- 因为“归并算法”(merge algorithm)对每一个大块只是顺序地做一轮访问(进行归并)，每个大块不用完全载入主存。

#### 算法分析
但是在DBMS中，大量的时间消耗在I/O上，因此我们对时间复杂度分析更着重于对I/O分析.   
- 二路归并排序分析(假设我们有N页page):  
Number of passes: 1 + ⌈log2(N)⌉(1是第一遍对每页page内部排序,需要全部扫描一遍)  
Total I/O cost: 2N(# of passes)(2是I+O)
- k路归并排序分析
![External Merge Sort](/img/DataBase/ExternalMergeSort.jpeg)

#### 优化
当DBMS在处理当前这一run时，我们可以提前fetch下一run,这样可以减少等待I/O的时间，因为我们在持续不断得使用disk.


#### 堆 vs 胜者树 vs 败者树
时间复杂度都是O(logN)，不同点在于比较次数.  

**堆**:每次排序时需要先将两个child进行比较,从中取出一个最值,再将parent与取出来的child进行比较,决定是否将parent下沉。  

**胜者树**:叶结点是candidate,非叶结点是每次比赛(比较大小)的胜者。相对于堆的优势在于,candidate每次只需要和自己的兄弟结点进行比较。
![胜者树origin](https://s.iteblog.com/pic/old/winTree1.jpg)
![胜者树final](https://s.iteblog.com/pic/old/winTree2.jpg)

**败者树**:叶结点是candidate,非叶结点是每次比赛(比较大小)的败者。相对于胜者树的优势在于,candidate每次只需要和parent进行比较。
![败者树](https://s.iteblog.com/pic/old/winTree3.jpg)  



## 使用B+Tree进行sorting
如果table已经被某一index排序好了,那么我们可以直接利用该index,遍历该index所对应的B+Tree的叶结点.
- Clustered B+Tree
	遍历叶结点即可,总是比外排序好,因为没有conputational cost,所有对disk的访问是sequential的。
	![Clustered Sort](/img/DataBase/ClusteredSort.jpeg)
- Unclustered B+Tree
	每个叶结点中包含的data是指向page的pointer,因此会引入大量的随机I/O访问,是一个bad idea.
	![Unclustered Sort](/img/DataBase/UnclusteredSort.jpeg)



## Aggregations
根据多个tuple得到一个简单的数量result,如 AVG，SUM。

Two implementation choices:
#### Sorting
![Aggregations Sort](/img/DataBase/AggreSort.jpeg)

#### Hashing
如果我们不需要data是有序的呢?例如 GROUP BY 或 DISTINCT ?
在这样的场景下,我们更倾向于选择Hash，因为Hash只移除了重复的数据,而并不对其排序,而且计算量更少.在DBMS中，我们同样需要考虑data无法 fit in memory 的情况:  
- Phase#1: Partition  
使用一个 Hash Function h1 来将tuples划分为位于disk上的partition，当partition溢出时我们将其写入disk的.  
假设我们有B个buffers，我们使用B-1个buffers用于partitions,用1个buffer来input data。
![Partition](/img/DataBase/Partition.jpeg)

- Phase#2: ReHash  
对于disk上的每部分partition,我们将其读入memory(假设每个partition fit in memory）,在memory中建立一张基于Hash Function h2的hash table。接下来扫描该 Hash Table的每个bucket,将匹配的tuples bring together.
![ReHash](/img/DataBase/ReHash.jpeg)

###### HashSummarization
在ReHash阶段,可以以(GroupKey->RunningVal)的form存储.这样当我们想要insert一个新的tuple时,我们只需要更新RunningVal即可,或者insert一个新的GroupKey->RunningVal。
![Hash Summarization](/img/DataBase/HashSummarization.jpeg)


###### 算法分析
使用该算法,我们可以hash多大的table呢？  
在Phase#1中,可以得到B-1个partitions.  
而每个Partitions不应当超过B个blocks(因为我们假设每个partition可以fit in memory).

Answer: **B·(B-1)**,即一张table，如果占有N页page,那么Buffer Pool中就至少需要sqrt(N)个frame.



























