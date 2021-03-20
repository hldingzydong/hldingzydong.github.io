---
layout: post
title:  Sliding Window
date:   2020-12-06
author: hding
catalog: true
tags:
   - algorithm
---
# Sliding Window
`Key Word`: contious, subString, subArray

## Fixed Window Size
#### Practice
- [480. Sliding Window Median](https://leetcode.com/problems/sliding-window-median/)
- [567. Permutation in String](https://leetcode.com/problems/permutation-in-string/)



## Flexible Window Size
#### Template
```java
int left = 0, right = 0;

while (right < s.size()) {
    window.add(s[right]);
    right++;
    
    while (valid) {
        window.remove(s[left]);
        left++;
    }
}
```
#### Practice
- [76. Minimum Window Substring](https://leetcode.com/problems/minimum-window-substring/)
- [395. Longest Substring with At Least K Repeating Characters](https://leetcode.com/problems/longest-substring-with-at-least-k-repeating-characters/)
- [438. Find All Anagrams in a String](https://leetcode.com/problems/find-all-anagrams-in-a-string/)
```java
class Solution {
    public List<Integer> findAnagrams(String s, String p) {
        List<Integer> ans = new ArrayList<>();
        if(s.length() < p.length()) {
            return ans;
        }

        int[] pCnt = new int[26];
        for(int i = 0; i < p.length(); i++) {
            pCnt[p.charAt(i) - 'a']++;
        }

        int goal = p.length();
        int left = 0, right = 0;
        while (right < s.length()) {
            int rightCharIndex = s.charAt(right) - 'a';
            pCnt[rightCharIndex]--;
            if(pCnt[rightCharIndex] >= 0) {
                goal--;
            }
            right++;
            while (goal == 0) {
                if(right - left == p.length()) {
                    ans.add(left);
                }
                int leftCharIndex = s.charAt(left) - 'a';
                if(pCnt[leftCharIndex] >= 0) {
                    goal++;
                }
                pCnt[leftCharIndex]++;
                left++;
            }
        }
        return ans;
    }
}
```

- [424. Longest Repeating Character Replacement](https://leetcode.com/problems/longest-repeating-character-replacement/)
- [632. Smallest Range Covering Elements from K Lists](https://leetcode.com/problems/smallest-range-covering-elements-from-k-lists/)
- [713. Subarray Product Less Than K](https://leetcode.com/problems/subarray-product-less-than-k/)
- [930. Binary Subarrays With Sum](https://leetcode.com/problems/binary-subarrays-with-sum/)
- [992. Subarrays with K Different Integers](https://leetcode.com/problems/subarrays-with-k-different-integers/)


## Extension
[Prefix Sum](https://leetcode-cn.com/problems/subarrays-with-k-different-integers/solution/xi-fa-dai-ni-xue-suan-fa-yi-ci-gao-ding-qian-zhu-4/)