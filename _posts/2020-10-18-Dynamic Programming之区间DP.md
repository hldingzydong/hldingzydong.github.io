---
layout: post
title:  Dynamic Programming之区间DP
date:   2020-10-18
author: hding
catalog: true
tags:
   - algorithm
---
# 区间DP
`无后效性`:确定了[i:j]区间内的结果后,当[0:i],[j:-1]区间发生变化时,对已确定的子问题([i:j])不构成影响  
`最优子结构`: [i:j]区间内的最优解,是基于其内部区间的最优解得出的

```java
dp[i][j] // i,j代表某一区间的左右两端

// Template
// Time Complexity: O(n^2)
for(int len = 2; len <= length; len++) {
    for(int left = 0; left <= length - len; left++) {
        int right = left + len - 1;
        // left 为区间的左端点
        // right 为区间的右端点
        dp[left][right].calculate();
    }
}
```



## Part I - 回文系列
`Key`: 为了判断某一string是否为回文,至少需要记录该string的左右两端位置

#### [5. Longest Palindromic Substring](https://leetcode.com/problems/longest-palindromic-substring/)
`Feature`: Find the longest palindromic substring
```java
String s // input

for(int len = 2; len <= s.length(); len++) {
    for(int left = 0; left <= s.length() - len; left++) {
        int right = left + len - 1;
                
        if(s.charAt(left) == s.charAt(right)) {
            if(len == 2) {
                dp[left][right] = 2;
            } else if(dp[left+1][right-1] == len - 2){
                dp[left][right] = len;
            } 
                    
            if(dp[left][right] > maxLength) {
                maxLength = dp[left][right];
                maxLeft = left;
                maxRight = right;
            }
        }
    }
}

return s.substring(maxLeft, maxRight+1);
```



#### [647. Palindromic Substrings](https://leetcode.com/problems/palindromic-substrings/)
`Feature`: Count how many palindromic substrings in this string  
`Note`: The substrings with different start indexes or end indexes are counted as different substrings even they consist of same characters
```java
for(int len = 2; len <= s.length(); len++) {
    for(int left = 0; left <= s.length() - len; left++) {
        int right = left + len - 1;
                
        if(len == 2) {
           // dp[i][j] - s[i:j]区间内是回文的substring的数量
           dp[left][right] = s.charAt(left) == s.charAt(right) ? 3 : 2;
           // isPalindromic[i][j] - s[i:j]是否是回文字符串
           isPalindromic[left][right] = s.charAt(left) == s.charAt(right) ? true : false;
        } else {
            dp[left][right] = dp[left][right-1] + dp[left+1][right] - dp[left+1][right-1];
            if(s.charAt(left) == s.charAt(right) && isPalindromic[left+1][right-1]) {
                dp[left][right]++;
                isPalindromic[left][right] = true;
            }
        }
    }
}
```



#### [516. Longest Palindromic Subsequence](https://leetcode.com/problems/longest-palindromic-subsequence/)
`Feature`: Find the longest palindromic subsequence's length
```java
for(int len = 2; len <= s.length(); len++) {
    for(int left = 0; left <= s.length() - len; left++) {
        int right = left + len - 1;
        // dp[i][j] - 在s[i,j]区间内最长的回文子序列的长度
        dp[left][right] = Math.max(dp[left+1][right], dp[left][right-1]);
                
        if(s.charAt(left) == s.charAt(right)) {
            if(len == 2) {
                dp[left][right] = 2;
            } else {
                dp[left][right] = Math.max(dp[left][right], dp[left+1][right-1] + 2);
            }
        }
    }
}
```



#### [1312. Minimum Insertion Steps to Make a String Palindrome](https://leetcode.com/problems/minimum-insertion-steps-to-make-a-string-palindrome/)
```java
for(int len = 2; len <= s.length(); len++) {
    for(int left = 0; left <= s.length() - len; left++) {
        int right = left + len - 1;
        
        // dp[i][j] - 在s[i:j]区间内为了使其成为一个回文串所需要的插入次数
        if(len == 2) {
            dp[left][right] = s.charAt(left) == s.charAt(right) ? 0 : 1;
        } else {
            dp[left][right] = s.charAt(left) == s.charAt(right) ? dp[left+1][right-1] : Math.min(dp[left][right-1], dp[left+1][right]) + 1;
        }
    }
}
```



#### [730. Count Different Palindromic Subsequences](https://leetcode.com/problems/count-different-palindromic-subsequences/)
`Feature`: Find the number of different non-empty palindromic subsequences
```java
for(int len = 2; len <= S.length(); len++) {
    for(int left = 0; left <= S.length() - len; left++) {
        int right = left + len - 1;
                
        if(S.charAt(left) == S.charAt(right)) {
            int low = left + 1;
            int high = right - 1;
            while(low <= high && S.charAt(low) != S.charAt(left)) {
                low++;
            }
            while(low <= high && S.charAt(high) != S.charAt(right)) {
                high--;
            }
            // dp[i][j] - 在S[i:j]区间内, 含有的不同的非空回文子序列的数量        
            if(low > high) {
                dp[left][right] = dp[left+1][right-1] * 2 + 2;
            } else if(low == high) {
                dp[left][right] = dp[left+1][right-1] * 2 + 1;
            } else {
                dp[left][right] = dp[left+1][right-1] * 2 - dp[low+1][high-1];
            }
        } else {
            dp[left][right] = dp[left+1][right] + dp[left][right-1] - dp[left+1][right-1];
        }
                
        dp[left][right] = dp[left][right] < 0 ? dp[left][right] + 1000000007 : dp[left][right] % 1000000007;
    }
}
```



## Part II - 区间内的中间点
`Key`: 在计算区间的最优解时,多一个中间的状态点来决定最优解,因此多一层循环(from 左端点 to 右端点)  
`Note`: 在code中很多 base case,如 len == 2 可省略,但为了代码可读性则保留

#### [1039. Minimum Score Triangulation of Polygon](https://leetcode.com/problems/minimum-score-triangulation-of-polygon/)
`Feature`:在计算区间时需要考虑三角形除了两端的第三个顶点
```java
for(int len = 3; len <= N; len++) {
    for(int left = 0; left <= N - len; left++) {
        int right = left + len - 1;
                
        // dp[i][j] - 在A[i:j]区间内最小的面积和
        dp[left][right] = Integer.MAX_VALUE;
        if(len == 3) {
            dp[left][right] = A[left] * A[left + 1] * A[right];
        } else {
            for(int k = left + 1; k < right; k++) {
                if(k == left + 1) {
                    dp[left][right] = Math.min(dp[left][right], A[left] * A[k] * A[right] + dp[k][right]);
                } else if(k == right - 1) {
                    dp[left][right] = Math.min(dp[left][right], A[left] * A[k] * A[right] + dp[left][k]);
                } else {
                    dp[left][right] = Math.min(dp[left][right], A[left] * A[k] * A[right] + dp[left][k] + dp[k][right]);
                }
            }
        }
    }
}
```



#### [241. Different Ways to Add Parentheses](https://leetcode.com/problems/different-ways-to-add-parentheses/)
`Feature`:在计算区间时需要考虑最后一次计算时,运算符是哪一个
```java
for(int len = 3; len <= dp.length; len += 2) {
    for(int left = 0; left <= dp.length - len; left += 2) {
        int right = left + len - 1;
                
        List<Integer> res = new ArrayList<>();
        for(int operatorIndex = left+1; operatorIndex < right; operatorIndex+=2) {
            BiFunction<Integer, Integer, Integer> calculator;
                    
            if(input_split_array[operatorIndex].equals("+")) {
                calculator = Integer::sum;
            } else if(input_split_array[operatorIndex].equals("-")) {
                calculator = (a, b) -> (a - b);
            } else {
                calculator = (a, b) -> (a * b);
            }
                    
            // dp[i][j] - 在s[i,j]区间内的所有运算结果
            List<Integer> leftRes = dp[left][operatorIndex-1];
            List<Integer> rightRes = dp[operatorIndex+1][right];
            for(Integer leftValue : leftRes) {
                for(Integer rightValue : rightRes) {
                    res.add(calculator.apply(leftValue, rightValue));
                }
            }
        }
                
        dp[left][right] = res;
    }
}
```



#### [1130. Minimum Cost Tree From Leaf Values](https://leetcode.com/problems/minimum-cost-tree-from-leaf-values/)
`Feature`:考虑区间内,区分左右两边来选取最大值时,中间分界线的划分位置
```java
for(int len = 2; len <= N; len++) {
    for(int left = 0; left <= N - len; left++) {
        int right = left + len - 1;
                
        // dp[i][j] - 在s[i:j]区间内, 最小的 Non-Leaf 的 Nodes 的和
        dp[left][right] = Integer.MAX_VALUE;
        if(len == 2) {
            dp[left][right] = arr[left] * arr[right];
        } else {
            for(int k = left; k < right; k++) {
                dp[left][right] = Math.min(dp[left][right], 
                                    maxValue[left][k] * maxValue[k+1][right] + dp[left][k] + dp[k+1][right]);
            }
        }
    }
}
```


#### [312. Burst Balloons](https://leetcode.com/problems/burst-balloons/)
`Feature`: 中间状态为最后一个引爆的气球,在区间内的位置
```java
// step1: extend nums
int[] extendNums = new int[nums.length + 2];
extendNums[0] = 1;
extendNums[extendNums.length - 1] = 1;
for(int i = 0; i < nums.length; i++) {
   extendNums[i+1] = nums[i];
}
nums = extendNums;
```

```java
// step2: caluculate
for(int len = 3; len <= nums.length; len++) {
    for(int left = 0; left <= nums.length - len; left++) {
        int right = left + len - 1;
               
        if(len == 3) {
            dp[left][right] = nums[left] * nums[left+1] * nums[right];
        } else {
            dp[left][right] = Integer.MIN_VALUE;
            for(int k = left + 1; k < right; k++) {
                if(k == left+1) {
                    dp[left][right] = Math.max(dp[left][right], 
                                nums[left] * nums[k] * nums[right] + dp[k][right]);
                } else if(k == right - 1) {
                    dp[left][right] = Math.max(dp[left][right],
                                nums[left] * nums[k] * nums[right] + dp[left][k]);
                } else {
                    dp[left][right] = Math.max(dp[left][right], 
                                nums[left] * nums[k] * nums[right] + dp[left][k] + dp[k][right]);
                }
            }
        }
    }
}
```