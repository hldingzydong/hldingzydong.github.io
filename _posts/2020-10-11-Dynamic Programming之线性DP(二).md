---
layout: post
title:  Dynamic Programming之线性DP(二)
date:   2020-10-11
author: hding
catalog: true
tags:
   - algorithm
---
# 线性DP
双串问题, 状态为`dp[i][j]`, i为第一串的位置, j为第二串的位置.
```java
// Template
for(int i = 0; i <= s1.length(); i++) {
    for(int j = 0; j <= s2.length(); j++) {
        // i为s1的index
        // j为s2的index
        dp[i][j].calculate()
    }
}
```


## Part I - LCS
`Key Words`: Subsequence Two-String Common

#### [1143. Longest Common Subsequence](https://leetcode.com/problems/longest-common-subsequence/)
###### Analysis
`Input`: Two strings (s1 s2)  
`Output`: Length of longest common subsequence

###### Solution
```java
int[][] dp = new int[s1.length() + 1][s2.length() + 1];

for(int i = 0; i <= s1.length(); i++) {
    for(int j = 0; j <= s2.length(); j++) {
        if(i == 0 || j == 0) {
            dp[i][j] = 0;
        } else if(s1.charAt(i-1) == s2.charAt(j-1)) {
            dp[i][j] = 1 + dp[i-1][j-1];
        } else {
            dp[i][j] = Math.max(dp[i-1][j], dp[i][j-1]);
        }
    }
}
```
###### Time Complexity
O(n^2)



#### [712. Minimum ASCII Delete Sum for Two Strings](https://leetcode.com/problems/minimum-ascii-delete-sum-for-two-strings/)
`Key`: Find the longest common subsequence, but the `longest` means ASCII sum maximun
```java
for(int i = 0; i <= s1.length(); i++) {
    for(int j = 0; j <= s2.length(); j++) {
        if(i == 0 || j == 0) {
            dp[i][j] = 0;
        } else if(s1.charAt(i-1) == s2.charAt(j-1)) {
            dp[i][j] = s1.charAt(i-1) + dp[i-1][j-1];  // Comparing with #1143
        } else {
            dp[i][j] = Math.max(dp[i-1][j], dp[i][j-1]);
        }
    }
}
```



#### [1092. Shortest Common Supersequence](https://leetcode.com/problems/shortest-common-supersequence/)
`Key`: Find the longest common subsequence (not only length), keep another array to `record the path`
```java
int[][] dp = new int[s1.length() + 1][s2.length() + 1];
String[][] path = new String[s1.length() + 1][s2.length() + 1];

for(int i = 0; i <= s1.length(); i++) {
    for(int j = 0; j <= s2.length(); j++) {
        if(i == 0 || j == 0) {
            dp[i][j] = 0;
            path[i][j] = "";
        } else if(s1.charAt(i-1) == s2.charAt(j-1)) {
            dp[i][j] = 1 + dp[i-1][j-1];
            path[i][j] = s1.charAt(i-1) + path[i-1][j-1];
        } else {
            dp[i][j] = Math.max(dp[i-1][j], dp[i][j-1]);
            path[i][j] = (dp[i][j] == dp[i-1][j] ? path[i-1][j] : path[i][j-1]);
        }
    }
}
```



#### [718. Maximum Length of Repeated Subarray](https://leetcode.com/problems/maximum-length-of-repeated-subarray/)
`Key`: Subsequence -> Contious
```java
// 为了与above保持统一风格,将题目中的int[]转换为string
for(int i = 0; i <= s1.length(); i++) {
    for(int j = 0; j <= s2.length(); j++) {
        if(i == 0 || j == 0 || s1.charAt(i-1) != s2.charAt(j-1)) {
            dp[i][j] = 0;
        } else {
            dp[i][j] = 1 + dp[i-1][j-1];
            maxLength = Math.max(maxLength, dp[i][j]);
        }
    }
}
```



## Part II - String Match
#### [72. Edit Distance](https://leetcode.com/problems/edit-distance/)
`Key`: traverse 3 cases
```java
for(int i = 0; i <= s1.length(); i++) {
    for(int j = 0; j <= s2.length(); j++) {
        if(i == 0) {
            dp[i][j] = j;
        } else if(j == 0) {
            dp[i][j] = i;
        } else if(s1.charAt(i-1) == s2.charAt(j-1)) {
            dp[i][j] = dp[i-1][j-1];
        } else {
            dp[i][j] = 1 + Math.min(dp[i-1][j-1], Math.min(dp[i-1][j], dp[i][j-1]));
        }
    }
}
```

#### [44. Wildcard Matching](https://leetcode.com/problems/wildcard-matching/)
[Logical Explanation](https://leetcode.com/problems/wildcard-matching/discuss/370736/Detailed-Intuition-From-Brute-force-to-Bottom-up-DP)
```python
class Solution:
    def isMatch(self, s: str, p: str) -> bool:
        if len(s) == 0:
            if len(p) == 0:
                return True
            else:
                for i in range(len(p)):
                    if p[i] != '*':
                        return False
                return True
            
        elif len(p) == 0:
            return False

        dp = [[False for i in range(len(p) + 1)] for j in range(len(s) + 1)]

        # base cases
        dp[0][0] = True
        for i in range(len(p)):
            if p[i] == '*':
                dp[0][i+1] = dp[0][i]
            
        # iteration
        for sIndex in range(len(s)):
            for pIndex in range(len(p)):
                if p[pIndex] == '?' or p[pIndex] == s[sIndex]:
                    dp[sIndex+1][pIndex+1] = dp[sIndex][pIndex]
                elif p[pIndex] == '*': # match empty || match single char || match ("aaaaa", "a*")
                    dp[sIndex+1][pIndex+1] = dp[sIndex+1][pIndex] or dp[sIndex][pIndex] or dp[sIndex][pIndex+1]
        return dp[len(s)][len(p)]
```


#### [10. Regular Expression Matching](https://leetcode.com/problems/regular-expression-matching/)
[Logical Explanation](https://leetcode.com/problems/regular-expression-matching/discuss/5651/Easy-DP-Java-Solution-with-detailed-Explanation)
```python
class Solution:
    def isMatch(self, s: str, p: str) -> bool:
        dp = [[False for i in range(len(p) + 1)] for j in range(len(s) + 1)]
        dp[0][0] = True
        for j in range(1, len(p) + 1):
            if p[j - 1] == '*':
                dp[0][j] = dp[0][j - 2]

        for i in range(1, len(s) + 1):
            for j in range(1, len(p) + 1):
                currCharMatch = s[i - 1] == p[j - 1] or p[j - 1] == '.'
                if currCharMatch:
                    dp[i][j] = dp[i - 1][j - 1]
                elif p[j - 1] == '*':
                    prevCharMatch = s[i - 1] == p[j - 2] or p[j - 2] == '.'
                    matchEmpty = dp[i][j - 2]
                    matchOnce = dp[i][j - 1] if prevCharMatch else False
                    matchMulti = dp[i - 1][j] if prevCharMatch else False
                    dp[i][j] = matchEmpty or matchOnce or matchMulti
        return dp[len(s)][len(p)]
```


## Part III - Others
#### [97. Interleaving String](https://leetcode.com/problems/interleaving-string/)
```java
// dp[i][j]: i代表第一串的[0,i], j代表第二串的[0,j]
// dp[i][j]代表由第一串的[0,i]和第二串的[0,j]能否组成target的[0, i+j-1]
for(int i = 1; i <= s1.length(); i++) {
    char s1c = s1.charAt(i-1);
    for(int j = 1; j <= s2.length(); j++) {
        char s2c = s2.charAt(j-1);
        char s3c = s3.charAt(i+j-1);

        if(s3c == s1c && s3c == s2c) {
            dp[i][j] = dp[i-1][j] || dp[i][j-1];
        } else if(s3.charAt(i+j-1) == s1.charAt(i-1)) {
            dp[i][j] = dp[i-1][j];
        } else if(s3.charAt(i+j-1) == s2.charAt(j-1)) {
            dp[i][j] = dp[i][j-1];
        } else {
            dp[i][j] = false;
        }
    }
}
```
#### [115. Distinct Subsequences](https://leetcode.com/problems/distinct-subsequences/)
```java
// dp[i][j]: i为第一串的位置, j为第二串的位置
// 在第一串的[0,i]中, 能够匹配第二串前j个字符的subsequence的最大数量
for(int i = 0; i <= s.length(); i++) {
    for(int j = 0; j <= t.length(); j++) {
        if (j == 0) {
            dp[i][j] = 1;
        } else if(i == 0) {
            dp[i][j] = 0;
        } else if(s.charAt(i-1) == t.charAt(j-1)) {
            dp[i][j] = dp[i-1][j-1] + dp[i-1][j];
        } else {
            dp[i][j] = dp[i-1][j];
        }
    }
}
```