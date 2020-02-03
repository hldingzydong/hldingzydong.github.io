---
layout: post
title:  Leetcode-50-Pow(x, n)
date:   2020-02-03
author: hding
catalog: true
tags:
   - recursion
---
### Description
Implement pow(x, n), which calculates x raised to the power n (x^n).

### Example
##### Example 1
```
Input: 2.00000, 10
Output: 1024.00000
```

##### Example 2
```
Input: 2.10000, 3
Output: 9.26100
```

##### Example 3
```
Input: 2.00000, -2
Output: 0.25000
Explanation: 2-2 = 1/22 = 1/4 = 0.25
```

##### Note
```
-100.0 < x < 100.0
n is a 32-bit signed integer, within the range [−231, 231 − 1]
```

### Solution 1 : Recursion
```java
class Solution {
    public double myPow(double x, int n) {
        if(n == 0) return 1;
        if(n == 1) return x;
        int t = n / 2;
        if(n < 0) {
            t = -t;
            x = 1 / x;
        }
        double res = myPow(x, t);
        if(n % 2 == 0) return res * res;
        return res * res * x;
    }
}
```



























