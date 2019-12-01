---
layout: post
title:  cleanCode读书笔记(一)
date:   2019-12-1
---
####命名
1. 名称为什么会存在，做什么事，应该怎么用
   eg: int d;  //消逝的时间，以日计 ->  int fileAgeInDays;
  	    
2. 避免误导，切莫使用与本意想悖
   - 不应当使用专有名称
  	 eg: hp、aix、sco
   - 不应当用accountList指明一组账号，除非真的是List类型
  	 eg: accountList  ->  accountGroup || bunchOfAccounts
   - 不要使用不同处较小的名称
  	 eg:  XYZControllerForEfficientHandlingOfStrings & XYZControllerForEfficientStorageOfStrings
3. 有意义的区分
   ```
   public static void badParamCopyChars(char a1[], char a2[]);
    
   public static void goodParamCopyChars(char source[], char destination[]);
   ```
4. 不要有废话，Variable不当出现在变量名中，Table不应出现在表名中
   eg：Name**String**, Customer**Object**, money**Account**, customer**Info**,
   **the**Message

5. 使用读得出来的名称
   eg: genymdhms(生成日期、年、月、日、时、分、秒) -> generationTimestamp

6. 使用可搜索的名称，名称长短应与其作用域大小相对应，单字母名称仅适用于短方法中的局部变量
   eg: “e”是一个在搜索上的坏变量名

7. 类名和对象名应当是**名词**或者**名词短语**, 方法名应当是**动词**或**动词短语**

8. 一词一义
   eg: add可表示将两个值相加，也可表示将一个object加入到一个collection中

9. 使用解决方案领域的名称(CS术语、算法名、模式名)，毕竟只有程序员会读代码

10. 使用源于所涉问题领域的名称(前提是没办法使用解决方案领域的名称)

11. 添加有意义的语境,如**增加前缀**，或者**创建一个专属的类**
	eg: firstName、lastName、state   ->  addrFirstName、addrLastName、addrState || 创建Address的类

12. 只要短名称足够清楚，就比长名称好，不要为其添加不必要的语境
	eg: 构造对象
	```
	class Address;
	Address accountAddress = new Address();
	Address customerAddress = new Address();
	```
	eg: 用在类名
	```
	interface Address;
	class PostalAddress implements Address;
	class MAC implements Address;
	class URI implements Address;
	```









	













































