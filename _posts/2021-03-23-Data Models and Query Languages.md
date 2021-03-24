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

编程的本质,是建模,如网络模型(OSI七层模型),数据库模型,Web应用模型等等.而如同搭建高楼一样,模型是一层层搭建而起.一个从顶至底的general的模型是:

1. An application developer look at the real world (in which there are people, organizations, goods, actions, money flows, sensors, etc.) and model it in terms of objects or data structures, and APIs that manipulate those data structures.  
2. When need to store those data structures, SWE express them in terms of a general-purpose data model, such as JSON or XML documents, tables in a relational database, or a graph model.  
3. SWE who built database software decided on a way of representing that JSON/XML/relational/graph data in terms of bytes in memory, on disk, or on a network.    
4. HWE have figured out how to represent bytes in terms of electrical currents, pulses of light, magnetic fields, and more.

甚至其中在某一层,可以更加细分,比如某个API基于另外一个底层的API.但是每层模型的思想是一致的:`通过提供简洁的Data Model/API,隐藏了该层的复杂性,实现了对该层的抽象`.  
因此,Data Model十分重要,我们需要根据合适的业务场景,来决定采用哪一种Data Model.在本篇章中我们将介绍`Relational Model`,`Document Model`和`Graph-Like Model`.

## Relational Model VS Document Model

#### Motivation For NoSQL(NotOnly SQL)
- A need for `greater scalability` than relational databases can easily achieve, including very large datasets or very high write throughput.
- A widespread preference for `free and open source` software over commercial database products
- `Specialized query operations` that are not well supported by the relational model
- Frustration with the `restrictiveness of relational schemas`, and a desire for a more dynamic and expressive data model

#### Go through one example
让我们通过一个例子来体会Relational Model和Document Model.如图所示,我们想要去表示比尔盖茨的简历:
![Representing a LinkedIn profile using a relational schema. Photo of Bill Gates courtesy of Wikimedia Commons, Ricardo Stuckert, Agência Brasil.](https://learning.oreilly.com/library/view/designing-data-intensive-applications/9781491903063/assets/ddia_0201.png)

##### `Phase1`
可以看到,一份简历中包含last_name,first_name,教育经历,工作经历,个人总结和联系方式.因为一个人只会在简历上有一个last_name和一个first_name,那么使用一张表(Relational Model)来记录并表示是没有问题的,那么对于其他项呢,比如教育经历,一个人可能含有很多教育经历(`one-to-many`),那么用Relational Model如何表示呢?这里给出了三种solution:

1. Put positions, education, and contact information in `separate tables`, with a foreign key reference to the users table, as figure shows.  
2. Later versions of the SQL standard added `support for structured datatypes and XML data`; this allowed multi-valued data to be stored within a single row, with support for querying and indexing inside those documents.  
3. `Encode` jobs, education, and contact info `as a JSON or XML document`, `store it on a text column` in the database, and let the application interpret its structure and content. In this setup, you typically cannot use the database to query for values inside that encoded column.

对于self-contained的简历,采用Document Model可能更为合适.

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

同时隐含了`重复`的问题.  
- 假设不使用ID,那么存储的数据(industry_id="Philanthropy")只能被人类认识.因此,当使用string/text来存储时,所有使用字段的地方,都是对人类有意义的信息的duplication,假设有一天我们要对某个信息进行更改,比如更换某个region的名字,那么在所有使用它的地方都要进行更改,这将带来write的开销,而且容易造成不一致性(有的table更新成新的名字,有的table没有).   
- 当我们使用ID来存储时,ID是机器认识的,因为人类不认识它,它从不需要改变.当我们要更换某个region的名字时,只需要修改一处即可.  

但是,当进行query时,需要join两张表.  
在Phase1中,我们认为Document Model最适合用来表示简历.但是Document Model不适合甚至不支持进行join,因此若使用Document Model,则需要在application的code中进行join.  
除此之外,随着我们的模型逐渐完善,有可能对模型提出了其他的需求,比如需要将教育经历中的学校作为一个Entity,或者将工作经历中的公司作为一个Entity.如图LinkedIn上的简历所示:
![The company name is not just a string, but a link to a company entity. Screenshot of linkedin.com.](https://learning.oreilly.com/library/view/designing-data-intensive-applications/9781491903063/assets/ddia_0203.png)

这就引入了`many-to-many`的关系.可以进一步将简历抽象为如图所示:
![Extending résumés with many-to-many relationships.](https://learning.oreilly.com/library/view/designing-data-intensive-applications/9781491903063/assets/ddia_0204.png)



##### `Phase3`
为了解决many-to-many的关系,又提出了Network Model.




#### Relational Model VS Document Model





## Query Language For Data

## Graph-Like Data Models