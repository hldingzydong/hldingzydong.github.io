---
layout: post
title:  CleanCode读书笔记(一)
date:   2019-12-1
author: hding
catalog: true
tags:
   - code
---
### 命名
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
   ```java
   public static void badParamCopyChars(char a1[], char a2[]);
    
   public static void goodParamCopyChars(char source[], char destination[]);
   ```  

4. 不要有废话，Variable不当出现在变量名中，Table不应出现在表名中  
   eg：Name**String**, Customer**Object**, money**Account**, customer**Info**, **the**Message

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



### 函数
1. 短小，更短小

2. 函数应该只做一件事

3. 每个函数一个抽象层次

4. 尽可能将switch语句转为抽象工厂模式

5. 使用描述性的名称，命名方式应当保持一致
   ```java
   public void includeSetupAndTeardownPages();
   public void includeSetupPages();
   public void includeSuiteSetupPage();
   public void includeSetupPage();
   ```

6. 函数参数
   - 一元函数: 转换 || 事件
   - 标识参数(传入参数为boolean): 意味着函数做了不止一件事
   - 二元函数: 当传入参数是单个值的有序组成部分较为合理，其余应当尽可能将其转换为一元函数

7. 函数取名，对于一元函数,函数和参数应当形成一种良好的**动词/名词对**形式  
   eg: write(name) || writeField(name)

8. 无副作用，比如函数承诺只做一件事情，但是它暗中做了其他事情  
   eg:
   ```java
   public class UserValidator{
    private Crytographer crytographer;

    public boolean checkPassword(String username, String password) {
      User user = UserGateway.findByName(username);
      if(user != User.NULL) {
        String codedPhrase = user.getPhraseEncodedPassword();
        String phrase = codedPhrase.decrypt(codedPhrase, password);
        if("Valid Password".equals(phrase)) {
          //在这里，函数暗中做了其他的事情，带来了副作用
          Session.initialize();
          return true;
        }
      }
      return false;
    }
   }
   ```

9. 应避免使用输出参数，若函数必需修改某种状态，则修改所属对象的状态  
   eg:
   ```java
   //bad
   public void appendFooter(StringBuffer report);

   //good
   report.appendFooter();
   ```

10. 函数应该修改某对象的状态，或返回该对象的有关信息，两样都做常会导致混乱  
    eg:
    ```java
    public boolean set(String attribute, String value);
    ```
    Ps: 该函数设置某个指定属性，若成功则返回true，若不存在该属性则返回false  
    应当修改为:
    ```java
    if(attributeExists(String attribute)) {
      setAttribute(String attribute, String value);
      ...
    }
    ```

11. 使用异常代理返回错误码
    ```java
    if(deletePage(page) == E_OK) {
      if(register.deleteReference(page.name) == E_OK) {
        logger.log("");
      }else{
        logger.log("wrong");
      }
    }else{
      loggger.log("wrong");
    }
    ```
    应当修改为:  
    ```java
    try{
      deletePage(page);
      register.deleteReference(page.name);
    }catch(Exception e) {
      logger.log(e.getMessage());
    }
    ```
    进一步，可以做到抽离try-catch代码块  
    ```java
    public void delete(Page page) {
      try{
        deletePageAndAllReference(page);
      }catch(Exception e) {
        logError(e);
      }
    }

    private void deletePageAndAllReference(Page page) throws Exception{
       deletePage(page);
       register.deleteReference(page.name);
    }

    private void logError(Exception e) {
       logger.log(e.getMessage());
    }
    ```
    Ps: 错误处理就是一件事，处理错误的函数不应该做其他事情

12. 消灭重复
13. 如何写出好的函数？ **不断重构与思考**



### 注释
1. 哪些注释应该写呢？  
  - 法律信息
  - 提供基本信息，如解释某个抽象方法的返回值,但还是尽量用函数名称传达信息
    ```java
    // Return an instance of the Responder being tested
    protected abstract Responder responderInstance(); 
    //but above could writen in
    protected abstract Responder responderBeingTested();

    //format matched kk:mm:ss EEE, MMM dd, yyyy
    Pattern timeMatcher = Pattern.compile("\\d*:\\d*:\\d*: \\w*, \\w* \\d*, \\d*");
    ```
  - 将某些难懂的参数或返回值的意义翻译成某种可读形式
  - 警示
  - TODO注释  
  表示程序员认为应该做，但由于某些原因目前还没法完成，应当定期查看这些注释，删除不再需要的
  - 放大某种开来不合理之物的重要性  

2. 哪些注释不好？
  - 非本地信息(与当前函数无关的信息)
  - 没有明显联系
  - 注释掉的代码
  - 位置标记   


### 格式
1. 向报纸学习，源文件最顶部给出高层次的概念和算法，细节向下依次展开，被调用的函数应放在调用它的下方

2. **紧密相关的代码**和**关系密切的概念**应相互靠近  

3. 变量声明应尽可能靠近其使用位置，局部变量应在函数的顶部出现，实体变量应在类的顶部声明  

4. 相关函数: 若某函数调用了另外一个，应该将它们放在一起，调用者应放在被调用者的上方
  
5. 水平隔离:
  - 赋值操作
  ```java
  totalChars += lineSize;
  ```  
  - 函数
  ```java
  public static double root(double a, double b, double c);
  ```


### 对象和数据结构
1. 对象与数据结构的反对称性
  - 对象将数据隐藏于抽象之后，曝露操作数据的函数，
  - 数据结构曝露其数据，没有提供有意义的函数  

2. 过程式代码(使用数据结构的代码)便于在不改动既有数据结构的前提下添加新函数，面向对象代码便于在不改动既有函数的前提下添加新类  
3. 过程式代码难以添加新的数据结构，因为必须修改所有函数，面向对象代码难以添加新函数，因为必须修改所有类  

4. **得墨忒耳律**  
    模块不应该了解它所操作的对象的内部情形，即对象不应通过get/set方法曝露其内部结构
  ```java
  //违反了得墨忒耳律
  final String outputDir = ctxt.getOptions().getScratchDir().getAbsolutePath();
  //火车失事
  Options opts = ctxt.getOptions();
  File scratchDir = opts.getScratchDir();
  final String outputDir = scratchDir.getAbsolutePath();
  //单纯使用数据结构
  final String outputDir = ctxt.options.scratchDir.absolutePath;
  ```











	













































