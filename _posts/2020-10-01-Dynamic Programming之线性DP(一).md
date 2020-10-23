---
layout: post
title:  Dynamic Programming之线性DP(一)
date:   2020-10-01
author: hding
catalog: true
tags:
   - algorithm
---
# 线性DP
单串问题, 状态为`dp[i]`, i是位置(Part IV中为dp[i]\[j\], i,j都是位置), 题目列表出自[此处](https://chengzhaoxi.xyz/42296.html).  

`无后效性`: 当确定了[0:i]区间内的每个子问题的最优解后,无论[i:-1]区间怎么变化,对[0:i]区间已经求得的结果不造成影响  
`最优子结构`: dp[i]是基于[0:i]中的某个/多个子问题的最优解得出的

## Part I - LIS 
`Key Words`: Order Subsequence Length

#### [300. Longest Increasing Subsequence](https://leetcode.com/problems/longest-increasing-subsequence/)
###### Analysis
`Input`: unsorted **Integer** array A  
`Output`: **length** of **longest increasing** **subsequence**  
`Key`: 由于求subsequence,应当基于前面某个位置的最优解,求出当前位置的最优解

`Object`: Integer  
`Compare`: < = > 

###### Solution
```java
for(int i = 1; i < A.length; i++) {
    for(int j = 0; j < i-1; j++) {
        if(A[j] < A[i]) {
            dp[i] = Math.max(dp[i], dp[j] + 1)
        }
    }   
}
``` 
###### Time Complexity
O(n^2)


#### [646. Maximum Length of Pair Chain](https://leetcode.com/problems/maximum-length-of-pair-chain/)
`Object`: Pair / Intervals  
`Compare`: 
```java
// convert unsorted to sorted
Arrays.sort(pairs, (a , b)->{
        if(a[0] == b[0]){
          return a[1] - b[1];
        } else {
          return a[0] - b[0]; 
        }
      });

// LIS
for(int i = 0; i < pairs.length; i++) {
    for(int j = 0; j < i; j++) {
        if(pairs[j][1] < pairs[i][0]) {
            dp[i] = Math.max(dp[i], dp[j] + 1); 
        }
    }
}
```



#### [354. Russian Doll Envelopes](https://leetcode.com/problems/russian-doll-envelopes/)
`Object`: Envelope  
`Compare`:
```java
// convert unsorted to sorted, ascend on width and descend on height if width are same
// [3, 4] cannot contains [3, 3], so we need to put [3, 4] before [3, 3] when sorting otherwise it will be counted as an increasing number if the order is [3, 3], [3, 4]
Arrays.sort(envelopes, (a , b)->{
        if(a[0] == b[0]){
          return b[1] - a[1];  // notice here is different from #646
        } else {
          return a[0] - b[0]; 
        }
      });

// LIS
for(int i = 1; i < envelopes.length; i++) {
    for(int j = 0; j < i; j++) {
        if(envelopes[j][1] < envelopes[i][1]) {
            dp[i] = Math.max(dp[i], dp[j] + 1);
        }
    }
}
```



#### [1048. Longest String Chain](https://leetcode.com/problems/longest-string-chain/)
`Object`: String  
`Compare`:
```java
private boolean isPredecessor(String left, String right) {
        if(left.length() + 1 == right.length()) {
            String changeLeft = "";
            // find first different position
            for(int i = 0; i < left.length(); i++) {
                if(left.charAt(i) != right.charAt(i)) {
                    if(i == 0) {
                        changeLeft = right.charAt(i) + left;
                    } else {
                        changeLeft = left.substring(0, i) + right.charAt(i) + left.substring(i);
                    }
                    break;
                }
            }
            if(changeLeft.isEmpty()) {
                changeLeft = left + right.charAt(right.length()-1);
            }
            return changeLeft.equals(right);
        }
        return false;
    }
```



#### [960. Delete Columns to Make Sorted III](https://leetcode.com/problems/delete-columns-to-make-sorted-iii/)
![leetcode-960](/img/Algorithm/leetcode-960.jpg)  
`Object`: Columns  
`Compare`:
```python
for i in cols:
    for j in [0, i-1]:
        for k in [0, rows]:
            if A[k][j] > A[k][i]:
                break;  ## Not statisfy condition
				
            if k == rows:
                dp[i] = Math.max(dp[i], dp[j] + 1);
```



#### [673. Number of Longest Increasing Subsequence](https://leetcode.com/problems/number-of-longest-increasing-subsequence/)
`Key`: Comparing with LIS, keep another dp array to record the number of every increasing subsequence length ends with index


## Part II - Contious Subarray
`Key Words`: Contious Sum/Product

#### [53. Maximum Subarray](https://leetcode.com/problems/maximum-subarray/)  
###### Analysis
`Input`: unsorted integer array A  
`Output`: **contiguous subarray** (containing at least one number) which has the **largest sum**  
`Key`:由于求contious,应当是基于前一个位置的最优解,求出当前位置的最优解

###### Solution
```java
dp_max[i] = Math.max(A[i], dp_max[i-1] + A[i]); // record the maximun sum which ends with A[i]
maxSum = Math.max(maxSum, dp_max[i]);
```
###### Time Complexity
O(n)



#### [152. Maximum Product Subarray](https://leetcode.com/problems/maximum-product-subarray/)
```java
dp_max[i] = Math.max(A[i], Math.max(dp_max[i-1] * A[i], dp_min[i-1] * A[i])); // record the maximun product which ends with A[i]
dp_min[i] = Math.min(A[i], Math.min(dp_max[i-1] * A[i], dp_min[i-1] * A[i])); // record the minimun product which ends with A[i]
maxProduct = Math.max(maxProduct, dp_max[i]);
```



#### [918. Maximum Sum Circular Subarray](https://leetcode.com/problems/maximum-sum-circular-subarray/)
`Key`: Convert to `Minimum Subarray`
 
![Convert](https://assets.leetcode.com/users/motorix/image_1538888300.png)
```java
for(int i = 1; i < A.length; i++) {
    dp_max[i] = Math.max(A[i], dp_max[i-1] + A[i]); // record the maximun sum which ends with A[i]
    dp_min[i] = Math.min(A[i], dp_min[i-1] + A[i]); // record the minimun sum which ends with A[i]
                
    maxSum = Math.max(maxSum, dp_max[i]);
    minSum = Math.min(minSum, dp_min[i]);
    sum += A[i];
}
return maxSum > 0 ? Math.max(maxSum, sum - minSum) : maxSum;
```



#### [面试题17.24 最大子矩阵](https://leetcode-cn.com/problems/max-submatrix-lcci/)
`Key`: Convert to 1D array

![Convert](/img/Algorithm/leetcode-363.jpg)



#### [363. Max Sum of Rectangle No Larger Than K](https://leetcode.com/problems/max-sum-of-rectangle-no-larger-than-k/)
`Key`: Same as `面试题17.24 最大子矩阵`, but the sum of sub matrix no larger than K, so cannot use [673. Number of Longest Increasing Subsequence](https://leetcode.com/problems/number-of-longest-increasing-subsequence/) solution



#### [1186. Maximum Subarray Sum with One Deletion](https://leetcode.com/problems/maximum-subarray-sum-with-one-deletion/)
`Key`: 考虑两种状态:已经删除过和未删除过
```java
// 因为i仅依赖于i-1,可优化
for(int i = 1; i < arr.length; i++) {
    noDel[i] = Math.max(noDel[i-1] + arr[i], arr[i]);
    del[i] = Math.max(noDel[i-1], del[i-1] + arr[i]);
    maxSum = Math.max(maxSum, Math.max(noDel[i], del[i]));
}
```



## Part III - House Robber
`Key Words`: No-adjacent Sum

#### [198. House Robber](https://leetcode.com/problems/house-robber/)
###### Analysis
`Input`: unsorted positive integer array A  
`Output`: **maxSum** of items not **adjacent**  
`Key`: 由于在每个位置可选择抢或者不抢,应当需要记录在每个位置抢或者不抢的状态

###### Solution
```java
// General
// dp[2][i] - dp[0]/dp[1] 表示是否抢劫该处,i表示位置
dp[0][i] = Math.max(dp[0][i-1], dp[1][i-1]);    // exclude this item
dp[1][i] = dp[0][i-1] + nums[i];                // include this item

// Space optimization: only use dp[0][i-1], dp[1][i-1]
// Convert to a function
private int rob(int[] nums, int lo, int hi) {
    int prevExclude = 0, prevInclude = 0;
    int currExclude, currInclude;
    for (int i = lo; i <= hi; i++) {
        currExclude = Math.max(prevExclude, prevInclude);
        currInclude = prevExclude + nums[i];
        prevExclude = currExclude;
        prevInclude = currInclude;
    }
    return Math.max(prevExclude, prevInclude);
}

return rob(nums, 0, nums.length - 1);
```
###### Time Complexity
O(n)


#### [213. House Robber II](https://leetcode.com/problems/house-robber-ii/)
`Key`: Convert circular to linear, only pick [0, nums.length-2] or [1, nums.length-1]
```java
return Math.max(rob(nums, 0, nums.length-2), rob(nums, 1, nums.length - 1));
```


#### [740. Delete and Earn](https://leetcode.com/problems/delete-and-earn/)
`Key`: Generate a new array which saves the sum of each value
```java
int[] A = new int[10001];
for(int i = 0; i < nums.length; i++) {
    A[nums[i]] += nums[i];
}
        
return rob(A, 0, A.length-1);
```


#### [1388. Pizza With 3n Slices](https://leetcode.com/problems/pizza-with-3n-slices/)
`Key#1`: The number of slices picked is fixed, so add it as a state  
`Key#2`: The Pizza is a cycle, so convert it to linear, just like [213. House Robber II](https://leetcode.com/problems/house-robber-ii/)
```java
// dp[i][j] is maximum sum which we pick `j` elements from linear array `i` elements
dp[i][j] = Math.max(
            dp[i-1][j],           // don't pick element `ith`
            dp[i-2][j-1] + A[i-1] // pick element `ith` -> dp[i-2][j-1] means choose `j-1` elements from array `i-2` elements because we exclude adjacent element `(i-1)th`
           );
```



## Part IV - Relation Between Three Items
#### [873. Length of Longest Fibonacci Subsequence](https://leetcode.com/problems/length-of-longest-fibonacci-subsequence/)
`Key`: Two positions to represent a state
```java
for(int i = 2; i < A.length; i++) {  // a strictly increasing array A
    int l = 0, r = i-1;
    while(l < r) {
        int curr_sum = A[l] + A[r];
        if(curr_sum < A[i]) {
            l++;
        } else if(curr_sum > A[i]) {
            r--;
        } else {
            dp[r][i] = dp[l][r] + 1;  // dp[i][j] means the number of Fibonacci SUbsequence ends with i and j
            max = Math.max(dp[r][i], max);
            l++;
            r--;
        }
    }
}
```
#### [1027. Longest Arithmetic Subsequence](https://leetcode.com/problems/longest-arithmetic-subsequence/)
`Key`: Just like above(873. Length of Longest Fibonacci Subsequence), but use HashMap as memorization
```java
// key: d 
// value: length of subsequence ends with A[j] which B[i+1] - B[i] == d
HashMap<Integer, Integer>[] dp = new HashMap[n];

for (int j = 0; j < A.length; j++) {
    dp[j] = new HashMap<>();
    for (int i = 0; i < j; i++) {
        int d = A[j] - A[i];
        dp[j].put(d, dp[i].getOrDefault(d, 1) + 1);
        res = Math.max(res, dp[j].get(d));
    }
}
```