---
layout: post
title:  DatabaseSystem之HashTable
date:   2020-01-16
author: hding
catalog: true
tags:
   - database
---
### Hash Table
在设计Hash Table上需考虑的两个主要因素:  

#### 使用何种Hash Function？
选用Hash Function时需在**being fast**和**collision rate**之间权衡。
我们不需要对key进行加密，我们想要的是不仅快，而且冲突少的Hash Function。完美的Hash Function应当是当key1 != key2 时， hash(key1) != hash(key2).  
![Hash Function](/img/DataBase/HashFunction.jpeg)

#### 如何设计Hash Scheme ？
选用Hash Scheme需在**分配一张较大的Hash Table**和**find/insert keys是否需要额外的操作**之间权衡。  

##### Static Hashing Schemes

###### Linear Probe Hashing(线性探测法)
使用线性探测法,当delete时会出现错误，改进的方法有**Tombstone**或**Movement**。
当遇到重复的key但是不同的value时，可以使用**Separate Linked List**或**Redundant Keys**。
![Non Unique Keys](/img/DataBase/NonUniqueKeys.jpeg)

###### Robin Hood Hashing
线性探测法的变种，从rich的key中偷出slot给poor的key。每个key都有一个最佳的slot(由Hash Function计算出的slot)，每个key追踪它们离最佳的slot的距离。当insert时，如果**key1距离其最佳slot的距离**比**key2距离其最佳slot的距离**远，那么key1就优先占据slot。  
核心思想: 尽可能让每个key离它的最佳slot近，不要出现极端case。比如key1在其最佳slot上，但是key2却需要离其最佳slot很远。这样不好，应该让key1把它的slot让给key2.
![Robin Hood Hashing](/img/DataBase/RobinHoodHashing.jpeg)

###### Cuckoo Hashing
使用多张Hash Table, 每张table分别有自己的Hash Function。当insert时,检查每张table并分别进行hash，再pick有free slot的table。如果没有table有free slot,就从中选一个table，让占据在其slot上的key让出自己的slot，将这个被命运选中的key被rehash，重新被insert。这样查看和delete都是O(1),因为每次每张表只会被check一个位置。
![Cuckoo Hashing](/img/DataBase/CuckooHashing.jpeg)


![Cuckoo Hashing2](/img/DataBase/CuckooHashing2.jpeg)
  

##### Dynamic Hashing
使用Static Hashing Schemes, DBMS需要知道想要存储的KV的数量，否则当table快满时，需要grow/shrink in size。而Dynamic Hashing根据需要resize它们自己。

###### Chained Hashing
对于每个slot，维持一个**bucket**链，这样可以解决冲突，为了知道Hash Table中是否已经有某个元素，需要scan这个bucket链。
![Chained Hashing](/img/DataBase/ChainedHashing.jpeg)

###### Extendible Hashing(difficult)
如果用Chained Hashing，那么bucket链就会非常长。因此提出了Extendible Hashing。多个slot可以指向同一个bucket。当某个bucket满了时，就split。
![Extendible Hashing](/img/DataBase/ExtendibleHashing.jpeg)

![Extendible Hashing2](/img/DataBase/ExtendibleHashing2.jpeg)
有两类标志位需要注意:  
global :  slot的个数为2^(global)，当某个bucket满时，就扩容，global+1，同时reshuffling bucket
local  :  对于每个bucket，只看自己的local，不看global

###### Linear Hashing(difficult)
此方法维持一个pointer来track下一个要被split的bucket。当有任何一个bucket溢出时，就split当前pointer指向的bucket，而不是溢出的bucket。基于这个pointer来split bucket最终会get to所有溢出的bucket。  
当这个pointer指向最后一个slot时，就删除第一个Hash Function。当这个pointer指向的bucket为空时，这个pointer也可以往回走。  
使用多个Hash Function来找到某个key所在的正确的bucket。  

picture太多, 具体请refer [slide](https://15445.courses.cs.cmu.edu/fall2019/slides/06-hashtables.pdf)








	













































