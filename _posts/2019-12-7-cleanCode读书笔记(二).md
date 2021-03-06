---
layout: post
title:  CleanCode读书笔记(二)
date:   2019-12-7
author: hding
catalog: true
tags:
   - code
---
### 错误处理
1. 遇到错误时，最好抛出异常  

2. 可控异常的代价是违反开放/封闭原则，若在方法中抛出可控异常，而catch语句在三个层级之上，则需要在catch语句和抛出异常处之间的每个方法签名中声明该异常。这意味着封装被打破了，抛出路径的每一个函数都需要了解下一层级的异常细节。  

3. 给出异常发生的环境声明，使用日志或消息块(失败的操作和失败的类型)

4. 打包调用第三方API

5. 不要返回null值，应当抛出异常或者返回特例对象。如果是调用第三方API返回null值，可考虑用新方法打包这个API，在新方法中抛出异常或返回特例对象。

6. 别传递null值  


### 单元测试
1. 测试和生产代码一样重要，需要被思考、被设计和被照料，应该像生产代码一样整洁  

2. 整洁的测试要保持**可读性**:  
   明确、简洁、足够的表达力  

3. 呈现**构造-操作-检验**模式，第一个环节构造测试数据,第二个环节操作测试数据,第三个环节检验操作是否得到了预期的结果  

4. 单个测试中的断言数量应该最小化  

5. 每个测试函数只测试一个概念  


### 类
1. 类的组织  
   ```java
   public static final int
   private static int
   private int
   private ObjectClass
   ```

2. 类应当**短小**  

3. 单一权责原则(SRP): **类或模块应有且只有一条加以修改的理由**  
    系统应该由许多短小的类而不是少量巨大的类组成。每个小类封装一个权责，只有一个修改的原因，并与少数其他类一起协同达成期望的系统行为。  

4. 内聚: 方法操作的变量越多，越黏聚到类上，若一个类中的每个变量都被每个方法使用，则该类具有最大的内聚性。  
    一般来讲，创建极大化内聚类是不可能也不可取；另一方面，希望内聚性保持在较高位置，内聚性高，意味着类中的方法和变量互相依赖，互相结合成一个逻辑整体。  

5. 将大函数拆为许多小函数，往往也是将类拆分为多个小类的时机。  

6. 开放-闭合原则(OCP): 类应当对扩展开放，对修改封闭  

7. 依赖倒置原则(dependency inversion principle, DIP): 类应当依赖于抽象而不是具体的实现细节。


### 迭代
1. 简单设计的四条规则:
  - 运行所有测试:  
    全面测试并持续通过所有测试的系统，是可测试的系统。
  - 不可重复: 模板方法
  - 表达程序员的意图: 好名称、函数和类尺寸短小、标准命名法、良好的单元测试
  - 尽可能减少类和方法的数量






  











	













































