---
layout: post
title:  Reliable, Scalable, and Maintainable Applications
date:   2021-03-20
author: hding
catalog: true
tags:
   - DDIA
---
## 前言
我们应当搭建什么样的系统?答案是`Reliability`, `Scalability`和`Maintainability`的系统.
这里给出了这三点的准确定义:

> `Reliability`  
> The system should continue to work correctly (performing the correct function at the desired level of performance) even in the face of adversity (hardware or software faults, and even human error).

> `Scalability`  
> As the system grows (in data volume, traffic volume, or complexity), there should be reasonable ways of dealing with that growth.

> `Maintainability`  
> Over time, many different people will work on the system (engineering and operations, both maintaining current behavior and adapting the system to new use cases), and they should all be able to work on it productively.

接下来,我们将深入讨论这三点,以及如何实现它们.



## Reliability
Reliability的系统应当具有如下几个特点/表现:
- The application performs the function that the user expected.
- It can tolerate the user making mistakes or using the software in unexpected ways.
- Its performance is good enough for the required use case, under the expected load and data volume.
- The system prevents any unauthorized access and abuse.  

简单讲,Reliability是指`当有错误的事情发生时,系统也可以正常工作`,这里所指的错误包括`Hardware Faults`, `Software Errors`和`Human Errors`.

注意需要区分`fault`和`failure`:

> `fault`  
> One component of the system deviating from its spec

> `failure`  
> When the system as a whole stops providing the required service to the user

在一个系统中,无法保证永远不会发生fault,比如硬件总会有crash的一天,因此系统应当是`fault-tolerant`.为了确保系统是fault-tolerant的,有时故意trigger fault来测试系统是否是fault-tolerant.

#### Hardware Faults
常见的Hardware Faults包括但不仅限于:
- Hard disks crash
- RAM becomes faulty
- The power grid has a blackout (停电)
- Someone unplugs the wrong network cable  

根据统计表明,Hard disks平均failure的时间是10-50年.因此,若一个存储集群含有10,000的disks,大概平均每天会crash一个.  

第一解决思路是`Add Redundancy`.  
- Disks can be in [RAID](https://www.youtube.com/watch?v=U-OCdTeZLac) (Redundant Array of Independent Disks)
- Servers may have dual power supplies and hot-swappable CPUs (`Hot-swappable` describes a component device that can be removed or installed without powering down the host computer)
- Datacenters may have batteries and diesel generators for backup power  

这样当某个组件fault时,就有另外一个备份来接管.虽然这样不能完全防止造成failure,会有一定的downtime,但是对于当下多数的系统来讲是足够的.


#### Software Errors
常见的Software Errors包括但不仅限于:
- A software bug that causes every instance of an application server to crash when given a particular bad input.  
- A runaway process that uses up some shared resource—CPU time, memory, disk space, or network bandwidth.  
- A service that the system depends on that slows down, becomes unresponsive, or starts returning corrupted responses.  
- Cascading failures, where a small fault in one component triggers a fault in another component, which in turn triggers further faults.  

造成Software Errors的原因往往是Developer对系统运行的环境进行了假设.一旦假设不成立,就可能发生Software Errors.

一些tips可以帮助应对Software Errors:
- Carefully thinking about assumptions and interactions in the system
- Thorough testing
- Process isolation
- Allowing processes to crash and restart
- Measuring, monitoring, and analyzing system behavior in production


#### Human Errors
由于系统operator的错误操作,造成系统crash.  
这里罗列了一些应对方法:
- Design systems in a way that minimizes opportunities for error. For example, well-designed abstractions, APIs and admin interfaces.  
- Decouple the places where people make the most mistakes from the places where they can cause failures.
- Test thoroughly at all levels, from unit tests to whole-system integration tests and manual tests.
- Allow quick and easy recovery from human errors, to minimize the impact in the case of a failure.
- Set up detailed and clear monitoring, such as performance metrics and error rates.



## Scalability
使用`Scalability`来描述`系统应对load增加的能力`.

#### Describing Load
当讨论系统的Scalability时,第一要明确,`对于当前的系统,其load是什么`.  
比如对于一个web server,它的load可能是每秒的请求; 对于DB,可能是读写比例; 对于cache,可能是cache命中率.  
只有在明确了系统的load之后,才可去观测其在不同程度的load下的performance.

#### Describing Performance
在明确load之后,可以用两种方式来观测其性能:
- When you increase a load parameter and keep the system resources (CPU, memory, network bandwidth, etc.) unchanged, how is the performance of your system affected?  
- When you increase a load parameter, how much do you need to increase the resources if you want to keep performance unchanged?  

对于不同的系统,观测性能的指标也不一样,比如对于batch processing系统,如Hadoop,应观测其`throughput`; 对于online系统,应观测其`response time`.

> `Throughput `  
> The number of records we can process per second, or the total time it takes to run a job on a dataset of a certain size.  

> `Response Time`  
> The time between a client sending a request and receiving a response.
> The response time is what the client sees: besides the actual time to process the request (the service time), it includes network delays and queueing delays.

> `Latency`  
> The duration that a request is waiting to be handled.

假设我们现在选取response time作为观测当前系统性能的指标,但是response time不是一个单一的数值,而是分布的一系列数值.如图:
![Illustrating mean and percentiles: response times for a sample of 100 requests to a service](https://learning.oreilly.com/library/view/designing-data-intensive-applications/9781491903063/assets/ddia_0104.png)

对于这样一系列分布的性能指标,相比于取平均,往往更适合使用`percentiles`来描述.因为percentiles可以体现有多少的用户经历了什么样的response time.常用的有p50,p95,p99,p999(99.9%).

> `p50`  
> Response time thresholds at which 50% of requests are faster than that particular threshold.

对于高百分比的percentiles,又叫做`tail latencies`.  
一方面它很重要,因为它所对应的response time大往往意味着有更多的数据要被处理,即更重要的客户(对于Amazon这类电商来讲,可能是购买了很多商品的客户);另一方面来讲,优化这些高的percentiles很困难,因为他们非常容易被不受系统控制的事情影响,而为优化它们所带来的收益很小.  
产生tail latencies的一个重要原因是`queue delay`,也叫`head-of-line blocking`.

> `head-of-line blocking`  
> As a server can only process a small number of things in parallel (limited, for example, by its number of CPU cores), it only takes a small number of slow requests to hold up the processing of subsequent requests.  

因此当我们测试系统的load的时候,作为测试的client应当保持一直发送请求,而不是完成一个请求的发送与接收后再发送下一个请求.


#### Approaches for Coping with Load
- Scaling Up (`Vertical Scaling`)  
Moving to a more powerful machine

- Scaling Out (`Horizontal Scaling`)  
Distributing the load across multiple smaller machines

## Maintainability
Maintainability主要是以下三个角度来定义:
> `Operability`  
> Make it easy for operations teams to keep the system running smoothly.
  
> `Simplicity`  
> Make it easy for new engineers to understand the system, by removing as much complexity as possible from the system. (Note this is not the same as simplicity of the user interface.)

> `Evolvability`  
> Make it easy for engineers to make changes to the system in the future, adapting it for unanticipated use cases as requirements change.