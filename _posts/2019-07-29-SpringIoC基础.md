---
layout: post
title:  SpringIoC基础
date:   2019-07-29
---
## 阅读官方文档
1. Inversion of Control (**IoC**)  ==   dependency injection (**DI**)  
It is a process whereby objects define their dependencies (that is, the other objects they work with) only through **constructor arguments**, **arguments to a factory method**, or **properties that are set on the object instance after it is constructed or returned from a factory method**. 
2. A bean is an object that is **instantiated(实例化), assembled（装配）, and otherwise managed by a Spring IoC container**. 
3. What is container？
   - **BeanFactory**  
   - **ApplicationContext**(a sub-interface of BeanFactory and more enterprise-specific      functionality):  
        - ApplicationContext represents the Spring IoC container and is responsible for instantiating, configuring, and assembling the beans
        - The container gets its instructions on what objects to instantiate, configure, and assemble by reading configuration metadata. **The configuration metadata is represented in XML, Java annotations, or Java code**. It lets you express the objects that compose your application and the rich interdependencies between those objects
        - it is common to create an instance of **ClassPathXmlApplicationContext** or **FileSystemXmlApplicationContext**
4. There are four ways to configure metadata
- XML-based metadata:
- Annotation-based configuration
- Java-based configuration:
- Spring’s Groovy Bean Definition DSL
5. Learn how to using container (new a ApplicationContext and through it get a bean object)
6. ApplicationContext permits the registration of existing objects that are created outside the container (by users).
7. Next we gonna to read [1.3.1](https://docs.spring.io/spring/docs/5.1.8.RELEASE/spring-framework-reference/core.html#beans-factory-class)
   








