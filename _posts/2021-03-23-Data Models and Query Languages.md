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

甚至在其中某一层,可以更加细分,比如某个API的实现基于另外一个底层的API.但是每层模型的idea是一致的:`通过提供简洁的Data Model/API,来隐藏该层的复杂性,并实现对该层的抽象`.  
因此,Data Model十分重要,我们需要根据当前的业务场景,来决定采用哪一种(合适的)Data Model.在本篇章中我们将介绍`Relational Model`,`Document Model`和`Graph-Like Model`.

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
可以看到,一份简历中包含last_name,first_name,教育经历,工作经历,个人总结和联系方式这些项.因为一个人的简历上只会有一个last_name和一个first_name,那么使用一张表(即Relational Model)来表示是没有问题的.那对于其他项呢,比如教育经历,一个人可能有很多教育经历(`one-to-many`),那么用Relational Model如何表示呢?这里给出了三种solution:

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



##### `Phase2`
在上图中,可以看到在用Relational Model表示所在地(region_id="Greater Seattle Area")和所在行业(industry_id="Philanthropy")时,并没有将其作为string/text放在user表中,而是分别单独建立一张表(regions/industries table),在user表中存放的是对应region/industry的`ID`.这样做有何好处?  
Note: 这里隐含了`many-to-one`的关系(很多人可能住在同一个地方).

- `Consistent style` and `spelling across profiles`
- `Avoiding ambiguity` (e.g., if there are several cities with the same name)
- `Ease of updating`—the name is stored in only one place, so it is easy to update across the board if it ever needs to be changed (e.g., change of a city name due to political events)
- `Localization support`—when the site is translated into other languages, the standardized lists can be localized, so the region and industry can be displayed in the viewer’s language
- `Better search`  

同时隐含了`duplication`的问题.  
- 当不使用ID来存储时,那么存储的数据(比如industry_id="Philanthropy")只能被人类认识.因此,当使用string/text来存储时,所有使用对应的field的地方,都是对人类有意义的信息的duplication.假设有一天我们要对某个field进行update,比如更换某个region的名字,那在所有使用它的地方都要update,这将带来write的开销,而且容易造成不一致性(有的table的field被update成新的名字,而有的table没有).   

- 当使用ID来存储时,ID是机器认识的,但因为人类不认识它,所以它不需要update.比如当需要update某个region的名字时,只需update一处即可(即update这张额外的ID-string/text表).  

但是,`当进行query时,需要join两张表`.  
在Phase1中,我们认为Document Model最适合用来表示简历.但是Document Model不适合甚至有时不支持进行join,因此若使用Document Model,则有时需要在application的code中进行join.  
除此之外,随着我们的模型逐渐完善,(FA)有可能对模型提出了其他的需求,比如需要将教育经历中的学校作为一个Entity,或者将工作经历中的公司作为一个Entity.如图LinkedIn上的简历所示:
![The company name is not just a string, but a link to a company entity. Screenshot of linkedin.com.](https://learning.oreilly.com/library/view/designing-data-intensive-applications/9781491903063/assets/ddia_0203.png)

这就引入了`many-to-many`的关系.可以进一步将简历抽象为如图所示:
![Extending résumés with many-to-many relationships.](https://learning.oreilly.com/library/view/designing-data-intensive-applications/9781491903063/assets/ddia_0204.png)



##### `Phase3`
- 为解决`many-to-many`的关系,研究者又提出了`Network Model`.在Document Model的树形结构中,每个record仅有一个parent.而在Network Model中,允许每个record拥有多个parent.这样就实现了`many-to-one`和`many-to-many`的关系.因此,在Network Model中的query,其本质是对图的遍历.但是,这样的模型使得query和update变得复杂,而且不灵活.一旦我们update了access path,那必须查看之前写的query(遍历)并且重写使得其能够apply新的路径.  

- 那么`many-to-many`在Relational Model中是怎么解决的呢?Relational Model只需要正常创建表并且存放数据即可.当进行query时,只需要declare即可,具体如何query由数据库内核中的query optimizer完成.  

- 现如今的Document Model中,通过`document reference`也解决了`many-to-one`和`many-to-many`的问题(如同Relational Model中的`foreign key`一样).



#### Relational Model VS Document Model
简单概括一下,`Document Model`的scheme更加灵活,同时因为locality具有更好的performance,并且有时更贴近于某些application所使用的data structure.`Relational Model`对join,many-to-one和many-to-many提供了更好的支持.  

`第一个问题`,哪一种Data Model更适用于application呢?  
答案是取决于application本身的特点,比如application所使用的data structure的data之间的关系.  

`第二个问题`,哪一种Data Model的Scheme更灵活?  
Document Model的scheme更具有灵活性,不如称Document Model的scheme为scheme-on-read:
> `scheme-on-read`  
> The structure of the data is implicit, and only interpreted when the data is read.

与其相对应的是scheme-on-write:
> `scheme-on-write`  
> The traditional approach of relational databases, where the schema is explicit and the database ensures all written data conforms to it.

当需要对data的format进行change的时候,scheme的灵活性显得尤为重要.举个例子,假设现在要从简历中的name分出来first_name,那对于Document Model,只需在应用层中,读取old的data时进行如下操作即可:
```javascript
if (user && user.name && !user.first_name) {
    user.first_name = user.name.split(" ")[0];
}
```
而对于Relational Model呢?需要对表结构进行更改:
```sql
ALTER TABLE users ADD COLUMN first_name text;
UPDATE users SET first_name = split_part(name, ' ', 1);      -- PostgreSQL
UPDATE users SET first_name = substring_index(name, ' ', 1);      -- MySQL
```
当跑`UPDATE`的sql时,对于数据量庞大的表,将消耗很多的时间,一个可替代的方案是,将first_name列初始时都置为NULL,如同Document Model一样,只有当读data时才更新.  

除此之外,当collection中的items并非都具有一样的结构时,灵活的schema更具有优势.为什么会有不一样的结构呢?
- There are many different types of objects, and it is not practicable to put each type of object in its own table.  
- The structure of the data is determined by external systems over which you have no control and which may change at any time.  

`第三个问题`,是Data的`Locality`.  
对于Document Model,每一项Document往往被存储为一个continuous的string,被encode为JSON,XML或BINARY的变种.如果application经常需要访问整个Document,比如用它去渲染一个web page,那么Document的Locality将为其带来更好的performance.而在Relational Model中,将其分为了多个表,那对于多个表的查询将是对disk的更多access,这会消耗更多的时间.  
不过这一点`仅仅在需要对Document的大部分Data访问时才更有优势`.当application只access一小部分数据时,那retrieve整个Document就显得浪费.而更新Document时,往往需要对整个Document进行重写,只有当update并不改变其size时,才有可能实现in place的替换.基于此,`推荐将Document尽可能往小的size存储,并且尽量避免会增加Document的size的write操作`.


## Query Language For Data
#### Declarative vs Imperative
- Declarative`更精确`,且`更易使用`   
- Declarative`隐藏了DB引擎的实现细节`,可能获取更好的performance.比如,Imperative中的loop对order有着要求,而Declarative却并不需要order,交由DB内核的optimizer实现  
- Declarative可实现`parallel execution`(Imperative难以实现是因为它强制指令必须以特定的order执行)

#### MapReduce Querying
MapReduce介于Declarative与Imperative之间,Developer只需要在map()和reduce()中写imperative的code,而无需关心其他的细节.目前被一些NoSQL的DB支持,如MongoDB和CouchDB.  
关于MapReduce更detail的介绍请refer谷歌的"*MapReduce: Simplified Data Processing on Large Clusters*".

## Graph-Like Data Models
当application中有很多`many-to-many`的relation时,采用Graph-Like Data Model更合适.比如`Social graphs`,`The web graph`和`Road or rail networks`.  
在graph中的vertex可以是不同的type,比如在`Social graphs`中,vertex可以是人,也可以是地点,评论等等.  
这一部分将讨论两种Graph-Like Data Model,分别是`Property Graphs model`(如Neo4j,Titan,and InfiniteGraph)和`Triple-Stores model`(如Datomic,AllegroGraph),也将讨论三种对于Graph-Like Data Model的query language,分别是`Cypher`,`SPARQL`和`Datalog`.  
一个简单的Graph-Like Data Model如图所示,后续对query language的举例皆是基于该graph:
![example for Graph-Like Data Model](https://learning.oreilly.com/library/view/designing-data-intensive-applications/9781491903063/assets/ddia_0205.png)

#### Property Graphs and Cypher
在Property Graphs中,每个vertex由以下组成:
- A unique identifier  
- A set of outgoing edges  
- A set of incoming edges  
- A collection of properties (key-value pairs)  

每条edge由以下组成:
- A unique identifier  
- The vertex at which the edge starts (the *tail vertex*)  
- The vertex at which the edge ends (the *head vertex*)  
- A label to describe the kind of relationship between the two vertices  
- A collection of properties (key-value pairs)  

使用如下SQL语句(Relation Model)可构建Property Graphs,由一张vertex表和一张edge表来表示:
```sql
CREATE TABLE vertices (
    vertex_id   integer PRIMARY KEY,
    properties  json
);
```  
```sql
CREATE TABLE edges (
    edge_id     integer PRIMARY KEY,
    tail_vertex integer REFERENCES vertices (vertex_id),
    head_vertex integer REFERENCES vertices (vertex_id),
    label       text,
    properties  json
);
```  
```sql
CREATE INDEX edges_tails ON edges (tail_vertex);
CREATE INDEX edges_heads ON edges (head_vertex);
```
Note: 为每条edge存储其tail_vertex和head_vertex,当需要查询vertex的incoming or outgoing edges时,可通过查询edge表的tail_vertex和head_vertex获得.  
关于此model,有以下三个方面需说明:
1. Any vertex can have an edge connecting it with any other vertex. There is no schema that restricts which kinds of things can or cannot be associated.  
2. Given any vertex, you can efficiently find both its incoming and its outgoing edges, and thus *traverse* the graph.  
3. By using different labels for different kinds of relationships, you can store several different kinds of information in a single graph, while still maintaining a clean data model.  

由此可看出Graph-Like Data Model具有很好的`flexibility`,并且可以表达一些在Relation Model中难以表达的relation,如:
- different kinds of regional structures in different countries (France has *départements* and *régions*, whereas the US has *counties* and *states*)  
- quirks of history such as a country within a country (ignoring for now the intricacies of sovereign states and nations)  
- varying granularity of data (Lucy’s current residence is specified as a city, whereas her place of birth is specified only at the level of a state)

此外,还具有很好的`evolvability`,当需要在application中增加feature时,很容易扩展Graph-Like Data Model(只需要增加vertex和edge).

接下来将讨论对Property Graphs的查询语言:`Cypher`.

> Cypher is a declarative query language for property graphs, created for the Neo4j graph database.  

以下是实现上图中简单的Graph-Like Data Model的例子的Cypher语句:
```
CREATE
  (NAmerica:Location {name:'North America', type:'continent'}),
  (USA:Location      {name:'United States', type:'country'  }),
  (Idaho:Location    {name:'Idaho',         type:'state'    }),
  (Lucy:Person       {name:'Lucy' }),
  (Idaho) -[:WITHIN]->  (USA)  -[:WITHIN]-> (NAmerica),
  (Lucy)  -[:BORN_IN]-> (Idaho)

```
当需要*find people who emigrated from the US to Europe*,可以使用Cypher进行match:
```
MATCH
  (person) -[:BORN_IN]->  () -[:WITHIN*0..]-> (us:Location {name:'United States'}),
  (person) -[:LIVES_IN]-> () -[:WITHIN*0..]-> (eu:Location {name:'Europe'})
RETURN person.name
```
Note:`(person) -[:BORN_IN]-> ()`匹配用label为`BORN_IN`的edge所连接的两个vertex,并且将edge的tail_vertex绑定到变量person,不绑定head_vertex.  
对于该match可以这样解释:
1. *person* has an outgoing *BORN_IN* edge to some vertex. From that vertex, you can follow a chain of outgoing *WITHIN* edges until eventually you reach a vertex of type *Location*, whose *name* property is equal to "*United States*".  
2. That same *person* vertex also has an outgoing *LIVES_IN* edge. Following that edge, and then a chain of outgoing *WITHIN* edges, you eventually reach a vertex of type *Location*, whose *name* property is equal to "*Europe*".  
3. For each such *person* vertex(match 1 & 2), return the *name* property.


#### Triple-Stores and SPARQL
在Triple-Stores中,所有的信息都以`(subject, predicate, object)`这样的形式存储.subject是graph中的vertex,而object可以是两种类型:
1. `Primitive datatype`, such as a string or a number.此时,该形式表示的是subject(vertex)的property,如(lucy,age,33).  
2. `Another vertex` in the graph.此时,该形式表示的是一条edge,subject是tail_vertex,object是head_vertex,如(lucy,marriedTo,alain).

接下来将讨论对Triple-Stores的查询语言:`SPARQL`.

> SPARQL is a query language for triple-stores using the [RDF data model](https://www.zhihu.com/question/28706033/answer/41818511).

以下是实现上图中简单的Graph-Like Data Model的例子中*find people who emigrated from the US to Europe*的SPARQL语句:
```
PREFIX : <urn:example:>

SELECT ?personName WHERE {
  ?person :name ?personName.
  ?person :bornIn  / :within* / :name "United States".
  ?person :livesIn / :within* / :name "Europe".
}
```  
将Cypher和SPARQL进行对比,可以看到它们的结构非常相似(因为Cypher借鉴于SPARQL):
```
(person) -[:BORN_IN]-> () -[:WITHIN*0..]-> (location)   # Cypher

?person :bornIn / :within* ?location.                   # SPARQL
```
但因为RDF Data Model并不区分property和edge,都是采用predicate,因此可以用同样的syntax来匹配property:
```
(usa {name:'United States'})   # Cypher

?usa :name "United States".    # SPARQL
```

#### Foundation: Datalog
Datalog比Cypher和SPARQL更加古老,现在已经不常用,但是它非常重要,因为它为其他语言提供了基石.它现在被Datomic和Cascalog(a Datalog implementation for querying large datasets in Hadoop)使用.  
Datalog的data model和SPARQL非常相似,但是它不采用`(subject, predicate, object)`,而使用`predicate(subject, object)`,如下所示:
```
name(namerica, 'North America').
type(namerica, continent).
```

以下是实现上图中简单的Graph-Like Data Model的例子中*find people who emigrated from the US to Europe*的Datalog语句:
```
within_recursive(Location, Name) :- name(Location, Name).     /* Rule 1 */

within_recursive(Location, Name) :- within(Location, Via),    /* Rule 2 */
                                    within_recursive(Via, Name).

migrated(Name, BornIn, LivingIn) :- name(Person, Name),       /* Rule 3 */
                                    born_in(Person, BornLoc),
                                    within_recursive(BornLoc, BornIn),
                                    lives_in(Person, LivingLoc),
                                    within_recursive(LivingLoc, LivingIn).

?- migrated(Who, 'United States', 'Europe').
```
Datalog通过定义新的rule来引进新的predicate,如*within_recursive*和*migrated*.这些rule不被存储,而是由其他rules derived,并且可以call它们自己(因此可递归)或者其他rules.  
当系统对`:-`的右边语句找到match时,可以apply该rule到系统中,即将该rule(`:-`的左边)加入到系统中.



## Practice
### Mongo DB - Document Model
##### [Step1 - Setup using Docker](https://hub.docker.com/_/mongo)
- 从Docker hub拉取latest的mongoDB image
```
docker pull mongo   
```
- run拉取的image
```
docker run --name {name for container} -p 27017:27017 -d mongo:{tag}
```
将本机27017端口映射到container内部的27017端口,使得后续可通过本机27017端口对container内的mongoDB进行访问.
- 进入container内部run command
```
docker exec -it {name for container} bash
``` 

##### [Step2 - Quick start](https://docs.mongodb.com/manual/tutorial/getting-started/)
##### [Step3 - Java access MongoDB](https://www.mongodb.com/java)



### Neo4j DB - Graph Model
###### [Step1 - Setup using Docker](https://hub.docker.com/_/neo4j)
- 从Docker hub拉取latest的neo4j image  
```
docker pull neo4j
```  

- run拉取的image
```
docker run --publish=7474:7474 --publish=7687:7687 neo4j
```
Detail请参考Docker hub中的 [How to use this image](https://hub.docker.com/_/neo4j).

###### [Step2 - Quick start](https://towardsdatascience.com/getting-started-with-neo4j-in-10-minutes-94788d99cc2b)