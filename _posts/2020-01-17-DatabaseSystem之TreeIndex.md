---
layout: post
title:  DatabaseSystem之TreeIndex
date:   2020-01-17
author: hding
catalog: true
tags:
   - database
---
## Table Indexes
Table Index是一张表的属性的被排序子集，为了更高效得访问。DBMS确保table的内容和index在逻辑上是同步的。DBMS还负责在执行每个query时选择最好的index。当为每个DB创建一定数量的index时，需要考虑为之存储付出的代价和为了维持这些index(每次insert时需要更新index)所付出的代价。

### B+ Tree Overview
B+树是一种自平衡的树结构，保持数据有序性，保证search、sequential access、insertion和deletion以O(logN)的时间复杂度进行。  
B+树是M-way search tree,它有以下这些属性:
1. 完美平衡(每个叶结点都在同一个depth)
2. 除了root结点外，每个node都是至少半满的: M/2 - 1 <= #keys <= M-1
3. 每个非叶node，若有k个key,则有k+1个非空children
![B+ Tree](/img/DataBase/BTree.jpeg)

##### Nodes
B+树的每个node是由KV数组(数组总保持key有序)组成。key是该index基于的属性，value由node的种类决定。
![BTreeNode1](/img/DataBase/BTreeNode1.jpeg)

![BTreeNode2](/img/DataBase/BTreeNode2.jpeg)
  
叶结点的value可以是**Record Ids(指向对应的tuple的位置的pointer)**或**Tuple Data(对应的tuple的content，倘若已经有index这样存储，则后面的index必须以Record Ids的形式存储)**。  
Ps: B-Tree是所有的node存储的KV的V永远是实际的value(Record Ids or Tuple Data)。而B+Tree只有叶结点存储这样的KV。

##### B+Tree的insert
找到对应的叶结点L，如果L有足够的空间就OK。  
如果L没有就把L分裂为L和一个新的结点L2.重新平均得分配entries,copy中间的key到L的parent,再将指向L2的pointer插入到L的parent。


##### B+Tree的delete
找到对应的叶结点L，如果L至少是half-full的就OK。  
如果L只有M/2 - 1个entries,尝试重新分配，从相邻的结点借一些KV。  
如果重新分配失败，就merge L和它的兄弟姐妹.如果merge发生了,就必须删除L的parent中指向L或其兄弟姐妹的pointer。


##### Clustered Index(聚簇索引)
**The table is stored in the sort order specified by the primary key.**可以以heap-或者index-organized的形式存储。即table中的tuples就是按照Clustered Index存放的。  
如果一张table没有一个primary key，有些DBMS会自动帮其生成一个隐藏起来的row id primary key。


##### Selection Conditions
如果query提供了search key中的any attribute,**有些**DBMS可以使用B+Tree Index. 而对于hash index, 我们必须使用search key中的所有attributes。  
如:Index on <a,b,c>, 支持(a=5 AND b=3) & 支持(b=3)
![Selection Conditions](/img/DataBase/SelectionConditions.jpeg)

##### Node Size
越慢的storage device， 可以存储更大的node。最佳的Node Size依赖于workload:  
**Leaf Node Scan** vs **Root-to-Leaf Traversals** 

##### Merge Threshold
当node时半空时,一些DBMS并不总是merge nodes,它会delay merge操作来减少reorganization的次数. 让这样的下溢存在，定时得rebuild tree可能会更好。

##### Variable Length Keys
- Pointers: key为指向tuple的attribute的pointer
- Variable Length Nodes: index中的每个node的size可以变化,但这需要更谨慎的内存管理
- Padding(填补): 总是将key pad至最大长度的key类型的长度
- Key Map/Indirection: 嵌入一个指针数组,该数组会映射到这个node中的KV list  
	![Key Map](/img/DataBase/KeyMap.jpeg) 

##### Non-Unique Indexes
- Duplicate Keys: 使用同样的叶结点的布局，但是存储duplicate keys多次
	![Duplicate Keys](/img/DataBase/DuplicateKeys.jpeg)

- Value Lists: 只存储key一次,但是维持一个unique values的linked list
	![Value Lists](/img/DataBase/ValueLists.jpeg)

##### 如何在一个leaf node中找到所需的KV呢
- Linear: 从开始到最后scan
- Binary: 二分查找
- Interpolation: 根据已知的key的分布,推断出所需的key的位置
![Intra Node](/img/DataBase/IntraNode.jpeg)


### Optimizations

##### Prefix Compression
存储在用一个leaf node中的keys,可能会有同样的前缀。因此我们不store完整的key,可以抽出公共的前缀,存储每个key的唯一的suffix.
![Prefix Compression](/img/DataBase/PrefixCompression.jpeg)


##### Suffix Truncation(切断)
非叶结点只用来“direct traffic”,我们并不需要完整的key,因此我们可以存储最小化的前缀，这样的前缀只需要起到正确的路由作用就好.
![Suffix Truncation](/img/DataBase/SuffixTruncation.jpeg)

![Suffix Truncation2](/img/DataBase/SuffixTruncation2.jpeg)


##### Bulk Insert
最快去build一个B+Tree的方法是,首先排序keys,在从bottom至up来建立B+Tree.
![Bulk Insert](/img/DataBase/BulkInsert.jpeg)


##### Pointer Swizzling(混合)
Nodes使用page_id来引用其他index中的node。DBMS在traversal中必须从page table中得到内存位置.  
如果一个page被pin到了Buffer Pool中,那么我们可以存储raw pointers而不是page_id.这样可以避免从page_table中查找地址.
![Pointer Swizzling](/img/DataBase/PointerSwizzling.jpeg)

![Pointer Swizzling2](/img/DataBase/PointerSwizzling2.jpeg)


	













































