---
layout: post
title:  Data Models and Query Languages
date:   2021-03-23
author: hding
catalog: true
tags:
   - DDIA
---
## 前言
Data Model非常重要,因为它:

> Not only on how the software is written, but also on how we think about the problem that we are solving

编程的本质是建模,如网络模型(OSI七层模型),数据库模型,Web应用模型等等.如同搭建高楼一样,模型是一层层搭建而起,一个从顶至底的general的模型是:

1. An application developer look at the real world (in which there are people, organizations, goods, actions, money flows, sensors, etc.) and model it in terms of objects or data structures, and APIs that manipulate those data structures.  
2. When need to store those data structures, SWE express them in terms of a general-purpose data model, such as JSON or XML documents, tables in a relational database, or a graph model.  
3. SWE who built database software decided on a way of representing that JSON/XML/relational/graph data in terms of bytes in memory, on disk, or on a network.    
4. HWE have figured out how to represent bytes in terms of electrical currents, pulses of light, magnetic fields, and more.

甚至在其中某一层,可以更加细分,比如某个API的实现基于另外一个底层的API.但是每层模型的idea是一致的:`通过提供简洁的Data Model/API,隐藏了该层的复杂性,实现了对该层的抽象`.  
因此,Data Model十分重要,我们需要根据合适的业务场景,来决定采用哪一种Data Model.在本篇章中我们将介绍`Relational Model`,`Document Model`和`Graph-Like Model`.

## Relational Model VS Document Model

#### Motivation For NoSQL(NotOnly SQL)
- A need for `greater scalability` than relational databases can easily achieve, including very large datasets or very high write throughput
- A widespread preference for `free and open source` software over commercial database products
- `Specialized query operations` that are not well supported by the relational model
- Frustration with the `restrictiveness of relational schemas`, and a desire for a more dynamic and expressive data model

#### Go through one example
让我们通过一个例子来体会Relational Model和Document Model.如图,我们想要表示比尔盖茨的简历:
![Representing a LinkedIn profile using a relational schema. Photo of Bill Gates courtesy of Wikimedia Commons, Ricardo Stuckert, Agência Brasil.](https://learning.oreilly.com/library/view/designing-data-intensive-applications/9781491903063/assets/ddia_0201.png)

##### `Phase1`
可以看到,一份简历中包含last_name,first_name,教育经历,工作经历,个人总结和联系方式.因为一个人只会在简历上有一个last_name和一个first_name,那么使用一张表(Relational Model)来记录并表示是没有问题的,那么对于其他项呢,比如教育经历,一个人可能含有很多教育经历(`one-to-many`),那么用Relational Model如何表示呢?这里给出了三种solution:

1. Put positions, education, and contact information in `separate tables`, with a foreign key reference to the users table, as figure shows.  
2. Later versions of the SQL standard added `support for structured datatypes and XML data`; this allowed multi-valued data to be stored within a single row, with support for querying and indexing inside those documents.  
3. `Encode` jobs, education, and contact info `as a JSON or XML document`, `store it on a text column` in the database, and let the application interpret its structure and content. In this setup, you typically cannot use the database to query for values inside that encoded column.

但是,对于self-contained的简历,采用Document Model可能更为合适.

> `Self-contained`  
> Something or someone that is complete on its own and that doesn't need anything else.

比如采用JSON representation来存储:
```json
{  
  "user_id":     251,  
  "first_name":  "Bill",  
  "last_name":   "Gates",  
  "summary":     "Co-chair of the Bill & Melinda Gates... Active blogger.",  
  "region_id":   "us:91",  
  "industry_id": 131,  
  "photo_url":   "/p/7/000/253/05b/308dd6e.jpg",  
  "positions": [  
    {"job_title": "Co-chair", "organization": "Bill & Melinda Gates Foundation"},  
    {"job_title": "Co-founder, Chairman", "organization": "Microsoft"}  
  ],  
  "education": [  
    {"school_name": "Harvard University",       "start": 1973, "end": 1975},  
    {"school_name": "Lakeside School, Seattle", "start": null, "end": null}  
  ],  
  "contact_info": {  
    "blog":    "http://thegatesnotes.com",  
    "twitter": "http://twitter.com/BillGates"  
  }  
}
```
Note: JSON的表示如同树一样.



##### `Phase2`
在比尔盖茨的简历中,我们可以看到在用Relational Model表示所在地(region_id="Greater Seattle Area")和所在行业(industry_id="Philanthropy")时,我们并没有将其作为string/text放在user表中,而是分别单独建立一张表(regions/industries table),而在user表中存放的是对应region/industry的ID.这样做有何好处?  
Note: 这里隐含了`many-to-one`的关系(很多人可能住在同一个地方).

- `Consistent style` and `spelling across profiles`
- `Avoiding ambiguity` (e.g., if there are several cities with the same name)
- `Ease of updating`—the name is stored in only one place, so it is easy to update across the board if it ever needs to be changed (e.g., change of a city name due to political events)
- `Localization support`—when the site is translated into other languages, the standardized lists can be localized, so the region and industry can be displayed in the viewer’s language
- `Better search`  

同时隐含了`duplication`的问题.  
- 假设不使用ID,那么存储的数据(industry_id="Philanthropy")只能被人类认识.因此,当使用string/text来存储时,所有使用字段的地方,都是对人类有意义的信息的duplication,假设有一天我们要对某个信息进行更改,比如更换某个region的名字,那么在所有使用它的地方都要进行更改,这将带来write的开销,而且容易造成不一致性(有的table更新成新的名字,有的table没有).   

- 当我们使用ID来存储时,ID是机器认识的,因为人类不认识它,它从不需要改变.当我们要更换某个region的名字时,只需要修改一处即可.  

但是,当进行query时,需要join两张表.  
在Phase1中,我们认为Document Model最适合用来表示简历.但是Document Model不适合甚至不支持进行join,因此若使用Document Model,则需要在application的code中进行join.  
除此之外,随着我们的模型逐渐完善,有可能对模型提出了其他的需求,比如需要将教育经历中的学校作为一个Entity,或者将工作经历中的公司作为一个Entity.如图LinkedIn上的简历所示:
![The company name is not just a string, but a link to a company entity. Screenshot of linkedin.com.](https://learning.oreilly.com/library/view/designing-data-intensive-applications/9781491903063/assets/ddia_0203.png)

这就引入了`many-to-many`的关系.可以进一步将简历抽象为如图所示:
![Extending résumés with many-to-many relationships.](https://learning.oreilly.com/library/view/designing-data-intensive-applications/9781491903063/assets/ddia_0204.png)



##### `Phase3`
- 为了解决`many-to-many`的关系,又提出了Network Model.在Document Model的树形结构中,每个record仅有一个parent.而在Network Model中,允许record拥有多个parent.这样就实现了`many-to-one`和`many-to-many`的关系.因此,在Network Model中的query,其实本质上是对图的遍历.但是,这样的模型使得query和update变得复杂,并且不灵活.一旦我们更改了access path,那么我们必须查看之前写的query(遍历)并且重写使得其能够apply新的路径.  

- 那么`many-to-many`在Relational Model中是怎么解决的呢?Relational Model只需要正常创建表并且存放数据即可.当进行query时,只需要declare即可,具体如何进行query由数据库内核中的query optimizer完成.  

- 而现如今的Document Model中,通过`document reference`也解决了`many-to-one`和`many-to-many`的问题,如同Relational Model中的`foreign key`一样.



#### Relational Model VS Document Model
简单概括一下,`Document Model`的scheme更加灵活,同时因为locality而获得了更好的performance,而且有时更贴近于某些application所使用的数据结构.`Relational Model`对join,many-to-one和many-to-many提供了更好的支持.  

`第一个问题`,哪一种Data Model更适用于application呢?  
答案是取决于application本身的特点,比如data structure中的data之间的关系.  

`第二个问题`,哪一种Data Model的Scheme更灵活?  
Document Model的scheme更具有灵活性,不如称Document Model的scheme为scheme-on-read:
> `scheme-on-read`  
> The structure of the data is implicit, and only interpreted when the data is read.

与其相对应的是scheme-on-write:
> `scheme-on-write`  
> The traditional approach of relational databases, where the schema is explicit and the database ensures all written data conforms to it.

当我们需要更新对data的format进行change的时候,scheme的灵活性显得尤为重要.不如让我们举个例子,假设我们现在要从简历中的name分出来first_name,那么对于Document Model,我们只需要在应用层中,读取old的data时进行如下操作即可:
```javascript
if (user && user.name && !user.first_name) {
    user.first_name = user.name.split(" ")[0];
}
```
而对于Relational Model呢?我们需要对表结构进行更改:
```sql
ALTER TABLE users ADD COLUMN first_name text;
UPDATE users SET first_name = split_part(name, ' ', 1);      -- PostgreSQL
UPDATE users SET first_name = substring_index(name, ' ', 1);      -- MySQL
```
当跑`UPDATE`的sql时,对于数据量庞大的表,将消耗很多的时间,一个可替代的方案是,将first_name列初始时都置为NULL,如同Document Model一样,只有当读data时才更新.  

除此之外,当collection中的items并非都具有一样的结构时,灵活的schema更具有优势.为什么会有不一样的结构呢?
- There are many different types of objects, and it is not practicable to put each type of object in its own table.  
- The structure of the data is determined by external systems over which you have no control and which may change at any time.  

`第三个问题`,是Data的Locality.  
对于Document Model,每一项Document往往都是被存储为一个continuous的string,encoded为JSON,XML或BINARY的变种.如果application经常需要访问整个Document,比如用它去渲染一个web page,那么Document的Locality将会有更好的performance.而Relational Model中,将其分为了多个表,那么对于多个表的查询将需要对disk的更多查询,将消耗更多时间.  
不过这一点仅仅在需要Document的大部分Data时才更有优势.当application只access一小部分数据时,那么retrieve整个Document就显得浪费.而更新Document时,往往需要对整个Document进行重写,只有当update并不改变其size时,才有可能实现in place的替换.基于此,推荐将Document尽可能往小存储,并且尽量避免会增加Document的size的write操作.


## Query Language For Data

## Graph-Like Data Models