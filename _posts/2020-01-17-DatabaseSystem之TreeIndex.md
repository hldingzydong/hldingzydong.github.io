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
Table Index是一张表的属性的被排序的子集，被创建为了更高效得访问这些attribute的子集。DBMS确保table的内容和index在逻辑上是同步的。DBMS还负责在执行每个query时选择最好的index。当为每个DB创建一定数量的index时，需要考虑为之存储付出的代价和为了维持这些index(每次insert时需要更新index)所付出的代价。

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
如果L只有M/2 - 1个entries,尝试重新分配,从相邻的结点借一些KV。  
如果重新分配失败,就merge L和它的兄弟姐妹.如果merge发生了,就必须删除L的parent中指向L或其兄弟姐妹的pointer.


##### Clustered Index(聚簇索引)
**The table is stored in the sort order specified by the primary key.**可以以heap-或者index-organized的形式存储。即table中的tuples就是按照Clustered Index存放的。  
如果一张table没有一个primary key，**有些**DBMS会自动帮其生成一个隐藏起来的row id primary key。


##### Selection Conditions
如果query提供了search key中的any attribute,**有些**DBMS可以使用B+Tree Index. 而对于hash index, 我们必须使用search key中的所有attributes。  
如:Index on <a,b,c>, 支持(a=5 AND b=3) & 支持(b=3)
![Selection Conditions](/img/DataBase/SelectionConditions.jpeg)


##### Node Size
越慢的storage device， 可以存储更大的node。最佳的Node Size依赖于workload:  
**Leaf Node Scan** vs **Root-to-Leaf Traversals** 


##### Merge Threshold(开始)
当node是half-full时,一些DBMS并不总是merge nodes,它会delay merge操作来减少reorganization的次数. 让这样的下溢存在，定时地rebuild tree可能会更好。


##### Variable Length Keys
- Pointers: key为指向tuple的attribute的pointer
- Variable Length Nodes: index中的每个node的size可以变化,但这需要更谨慎的内存管理
- Padding(填补): 总是将key pad至最大长度的key类型的长度
- Key Map/Indirection: 嵌入一个指针数组,该数组会映射到这个node中的KV list  
	![Key Map](/img/DataBase/KeyMap.jpeg) 


##### Non-Unique Indexes 对于Duplciate的key如何处理呢
- Duplicate Keys: 使用同样的叶结点的布局，但是存储duplicate keys多次
	![Duplicate Keys](/img/DataBase/DuplicateKeys.jpeg)
	- Append Record Id  
	将tuple的unique record_id作为key的一部分,确保所有的key是unique的,这样DBMS可以使用partial key来查找tuple.
	![Append Record Id](/img/DataBase/AppendRecordId.jpeg)

	- Overflow Leaf Nodes  
	将叶结点spill为包含duplicate keys的overflow nodes,但这样更难maintain和modify.
	![Overflow Leaf Nodes](/img/DataBase/OverflowLeafNodes.jpeg)

- Value Lists: 只存储key一次,但是维持一个unique values的linked list
	![Value Lists](/img/DataBase/ValueLists.jpeg)


##### 如何在一个leaf node中找到所需的KV呢
- Linear: 从开始到最后scan
- Binary: 二分查找
- Interpolation: 根据已知的key的分布,推断出所需的key的位置
![Intra Node](/img/DataBase/IntraNode.jpeg)


### Optimizations

##### Prefix Compression
存储在同一个leaf node中的keys,可能会有同样的前缀。因此我们不store完整的key,可以抽出公共的前缀,存储每个key的唯一的suffix.
![Prefix Compression](/img/DataBase/PrefixCompression.jpeg)


##### Suffix Truncation(切断)
非叶结点只用来“direct traffic”,我们并不需要完整的key,因此我们可以存储最小化的前缀，这样的前缀只需要起到正确的路由作用就好.
![Suffix Truncation](/img/DataBase/SuffixTruncation.jpeg)

![Suffix Truncation2](/img/DataBase/SuffixTruncation2.jpeg)


##### Bulk Insert
最快去build一个B+Tree的方法是,首先排序keys,再从bottom至up来建立B+Tree.
![Bulk Insert](/img/DataBase/BulkInsert.jpeg)


##### Pointer Swizzling(混合)
Nodes使用page_id来引用其他index中的node。DBMS在traversal中必须从page table中得到内存位置.  
如果一个page被pin到了Buffer Pool中,那么我们可以存储raw pointers而不是page_id.这样可以避免从page_table中查找地址.
![Pointer Swizzling](/img/DataBase/PointerSwizzling.jpeg)

![Pointer Swizzling2](/img/DataBase/PointerSwizzling2.jpeg)



### Type Of Index


##### Implicit(暗示的) Index
许多DBMS自动得创建index来加强integrity(完整的) constraints,如Primary Keys, Unique Constraints, 但并不包括referential constraints(外键):
```sql
CREATE TABLE foo(
	id SERIAL PRIMARY KEY,
	val1 INT NOT NULL,
	val2 VARCHAR(32) UNIQUE
);
```
```sql
CREATE UNIQUE INDEX foo_pkey ON foo(id);
```
```sql
CREATE UNIQUE INDEX foo_val2_key ON foo(val2);
```


##### Partial Index
对于整张table的子集(部分tuple)创建index.这样可以减少index的大小和维持它所需要的代价.
```sql
CREATE INDEX idx_foo ON foo(a,b) WHERE c = 'WuTang';
```
```sql
SELECT b FROM foo WHERE a = 123 AND c = 'WuTang';
```
	

##### Covering Index
如果所有需要被获取的fields都被包含在index中,那么DBMS就不需要retrieve对应的tuple,这样减少了对Buffer Pool的需求.
```sql
CREATE INDEX idx_foo ON foo(a,b);
```
```sql
SELECT b FROM foo WHERE a = 123;
```


##### Index Include Columns
嵌入additional的columns来支持index-only query.被include进的columns只会被存储在B+Tree的叶结点中,而不是search key中.
```sql
CREATE INDEX idx_foo ON foo(a,b)
		INCLUDE (c);
```
```sql
SELECT b FROM foo WHERE a = 123 AND c = 'WuTang';
```


##### Functional/Expression Indexes
An index does not need to store keys in the same way that they appear in their base table. We  can use expressions when declaring an index.
```sql
SELECT * FROM users WHERE EXTRACT(dow FROM login) = 2;
```
```sql
CREATE INDEX idx_user_login ON users (EXTRACT(dow FROM login));
```
```sql
CREATE INDEX idx_user_login ON users(login) WHERE EXTRACT(dow FROM login) = 2;
```


##### Trie Index
在B+Tree中的非叶结点无法告知我们某个key是否存在于index中，我们总是必须遍历至叶结点才知道。这意味着one buffer pool page miss per level in the tree 才能知道某个key是否存在于index中.  
Trie Index使用a digital representation of keys来一个个检测prefixes而不是比较整个key.
![Trie Index](/img/DataBase/TrieIndex.jpeg)

###### Trie Index Properties
大小取决于key的space和length,不取决于已存在的keys和插入顺序,也不需要rebalancing的操作。  
所有的operation是O(k)的复杂度,k是key的长度.通向一个叶结点的路径代表着这个leaf对应的key，keys在暗中隐藏着，可以根据path被restructured.

###### Trie Key Span(跨度、跨越)
每个trie level的span取决于the number of bits that each partial key / digit represents，如果该digit存在于keys中，那么store一个pointer指向trie的下一个level。否则存储null。  
这决定了每个node的fan-out(输出)和这个tree的物理高度。(n-way Trie == Fan-Out of n)
![Trie Key Span](/img/DataBase/TrieKeySpan.jpeg)


##### Radix Tree
省略掉所有只有一个child的node,缺点是DBMS总需要检查原始的tuple来test是否某个key match。
![Radix Tree](/img/DataBase/RadixTree.jpeg)


至今为止,我们所讨论的index，对于"point"和"range"query是有用的，比如找出所有code为15217的customer,找到在2018年至2019年期间的所有订单。但是它们不适用于keyword查找,比如找出wikipedia中所有包含“Haoling”的article。这就引出了:  
> Inverted Index

Inverted Index存储了words到records的映射，这些records在目标attribute中包含了对应的word。  
query的类型有:
- Phrase Search: 找到含有 a list of words in the given order的records
- Proximity(附近) Search: 找到 two words occur within n words of each other 的records
- Wildcard(通配符) Search: 找到 contain words that match some pattern(如正则表达式)的records


在design index时要考虑:
- what to store: index至少需要存储包含在每个record中的words,也可以存储frequency,position和其他meta-data
- when to update: 维持辅助的数据结构来进行update
































