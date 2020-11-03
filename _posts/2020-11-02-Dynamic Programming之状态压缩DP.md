---
layout: post
title:  Dynamic Programming之状态压缩DP
date:   2020-11-02
author: hding
catalog: true
tags:
   - algorithm
---
# 状态压缩DP

## Tech
使用集合的二进制状态作为dp的参数,如0b0011,表示第一位和第二位已被set,第三位和第四位未被set
```java
int fullState = 1<<N;

int state;
boolean isNthSet = (state>>n & 1) == 1;
// set n th bit
state = state | (1<<n);
```

## Template
```java
int fullState = 1<<N;
int[] dp = new int[fullState];
for(int state = 0; state < fullState; state++) {
    for(int i = 0; i < N; i++) {
        if(state>>i & 1 == 1) {
            continue;
        }
        int futState = state | (1<<i);
        // state transfer formular
        dp[futState] = f(dp[state]);
    }
}
```
Details: 有时可以选择两个不同的维度作为state,从时间复杂度分析,选择fullState较小的作为集合来遍历,参考[leetcode#1125](https://leetcode.com/problems/smallest-sufficient-team/)


## Practice
#### [847. Shortest Path Visiting All Nodes](https://leetcode.com/problems/shortest-path-visiting-all-nodes/)
`Feature`: 使用Bellman-Ford Algorithm进行预处理,得到两点间最短路径长度,然后进行状态压缩DP.
```java
// question method signature
int shortestPathLength(int[][] graph);

// 预处理
int[][] shortestPath = new int[N][N]; // shortestPath[i][j]代表从i->j最短的路径长度,可通过Bellman-Ford Algorithm求得

// 开始套template
int fullState = 1<<N;
// dp[i][state]: 表示以i为最后所到达的点,state表示已经遍历过的点的集合(set 1), 所需要的最短路径长度
int[][] dp = new int[N][fullSet];
// base case
for(int i = 0; i < N; i++) {
    Arrays.fill(dp[i], Integer.MAX_VALUE/2);
    dp[i][1<<i] = 0; // 从自己出发,并且自己也被遍历过，故为0
}
// iteration
for(int state = 0; state < fullState; state++) {
    // iterate start point
    for(int st = 0; st < N; st++) {
        if((state>>st & 1) == 0) {
            continue;
        }
        // iterate end point
        for(int en = 0; en < N; en++) {
            if((state>>en & 1) == 0) {
                int futState = state | (1<<en);
                // state transfer formular
                dp[en][futState] = Math.min(dp[en][futState], dp[st][state] + shortestPath[st][en]);
            }
        }
    }
}
```

#### [943. Find the Shortest Superstring](https://leetcode.com/problems/find-the-shortest-superstring/)
`Feature`: 最终字符串 = 起点串 + 中间串 + 终点串 -> 哈密顿路问题
```java
// question method signature
String shortestSuperstring(String[] A);

int fullState = 1<<N;
// dp[i][state]: 表示以i为终点串,已经遍历了达到state的节点的 shortest superstring
String[][] dp = new String[N][fullState];
// base case
for(int i = 0; i < N; i++) {
    dp[i][1<<i] = A[i];
}
// iterate
for(int state = 1; state < fullSet; state++) {
    for(int st = 0; st < N; st++) {
        if((state>>st & 1) == 0) {
            continue;
        }
        for(int en = 0; en < N; en++) {
            if((state>>en & 1) == 0) {
                // helper function, to join 2 strs together
                String candidate = join(dp[st][state], A[en]);
                int futState = state | (1<<en);
                // state transfer formular
                if(dp[en][futState] == null || candidate.length() < dp[en][futState].length()) {
                    dp[en][futState] = candidate;
                }
            }
        }
    }
}
```

#### [980. Unique Paths III](https://leetcode.com/problems/unique-paths-iii/)
`Feature`: 从起点开始,遍历四个方向,直到所有的midterm点都被遍历过.
```java
// question method signature
int uniquePathsIII(int[][] grid)；

// convert 二维坐标 to 一维坐标
int fullIdx = _ij2idx(row-1, col, col);
// dp[i][state]: 表示以i为终点串,已经遍历了达到state的节点的 路径数
int[][] dp = new int[fullIdx + 1][((1<<fullIdx) - 1) + 1];
// base case
int startIdx = _ij2idx(start_row, start_col, col);
dp[startIdx][1<<startIdx] = 1;
// iteration
for(int state = 0; state < end_state; state++) {
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            int idx = _ij2idx(i, j, col);
            if(grid[i][j] == -1 || ((state>>idx & 1) == 0)) {
                continue;
            }

            for(int k = 0; k < 4; k++) {
                int n_i = i + direction[k][0];
                int n_j = j + direction[k][1];
                if(n_i < 0 || n_i >= row || n_j < 0 || n_j >= col || grid[n_i][n_j] == -1) {
                    continue;
                }

                int n_idx = _ij2idx(n_i, n_j, col);
                if((state>>n_idx & 1) == 0) {
                    int futState = state | (1<<n_idx);
                    // state transfer formular
                    dp[n_idx][futState] += dp[idx][state];
                }
            }
        }
    }
}
```

#### [691. Stickers to Spell Word](https://leetcode.com/problems/stickers-to-spell-word/)
`Feature`: 预处理,记录每个sticker中每个字母的出现次数,然后遍历每个状态,如果该状态下为了达到target缺少该sticker中的字母,考虑将该sticker加入其中.
```java
// question method signature
int minStickers(String[] stickers, String target);

int fullState = 1<<N;
int len = stickers.length;
// create dp array
// dp[i]表示状态为i时所需的最少的sticker的数量
int[] dp = new int[fullState];
int[][] cnt = new int[len][26];
// 预处理,记录每个sticker中每个字母出现的次数
for(int i = 0; i < len; i++) {
    for(char c : stickers[i].toCharArray()) {
        cnt[i][c - 'a']++;
    }
}
// base case
Arrays.fill(dp, Integer.MAX_VALUE);
dp[0] = 0;
// iteration
for(int state = 0; state < fullState; state++) {
    if(dp[state] != Integer.MAX_VALUE) {
        for(int i = 0; i < len; i++) {
            int futState = state;
            int[] record = Arrays.copyOf(cnt[i], cnt[i].length);
            for(int j = 0; j < target.length(); j++) {
                int idx = target.charAt(j) - 'a';
                if((futState>>j & 1) == 0 && record[idx] > 0) {
                    record[idx]--;
                    futState = (futState | 1<<j);
                }
            }
            dp[futState] = Math.min(dp[futState], dp[state] + 1);
        }
    }
}
```

#### [1125. Smallest Sufficient Team](https://leetcode.com/problems/smallest-sufficient-team/)
`Feature`: 这里我们有两个维度,一个是需求的技能的数量(16),一个是人员的数量(60),考虑到时间复杂度,采用需求的技能作为状态。预处理时,先将skill->people的映射转换为people->skill的映射,然后遍历每个状态.
```java
// question method signature
int[] smallestSufficientTeam(String[] req_skills, List<List<String>> people);
// 预处理: skill -> idx in req_skills array
Map<String, Integer> skill2idx;

int fullState = 1<<N;
int[] dp = new int[fullState];
// record path
List[] recordDp = new List[fullState];
// base case
Arrays.fill(dp, Integer.MAX_VALUE);
dp[0] = 0;
recordDp[0] = new ArrayList();
// iterate
for(int i = 0; i < people.size(); i++) {
    List<String> skills = people.get(i);

    for(int state = 0; state < fullState; state++) {
        // judge if needs this people
        boolean need = false;
        int skillState = 0;
        for(String skill : skills) {
            int idx = skill2idx.get(skill);
            need = need | ((state>>idx & 1) == 0);
            skillState = skillState | (1<<idx);
        }
        // 此刻状态下，需要这个人
        if(need  && dp[state] != Integer.MAX_VALUE) {
            int futState = state | skillState;
            // state transfer formular
            dp[futState] = Math.min(dp[futState], dp[state] + 1);
            // record path according to choice
            if(dp[futState] == dp[state] + 1) {
                recordDp[futState] = new ArrayList<>();
                recordDp[futState].addAll(recordDp[state]);
                recordDp[futState].add(i);
            }
        }
    }
}
```

#### [1434. Number of Ways to Wear Different Hats to Each Other](https://leetcode.com/problems/number-of-ways-to-wear-different-hats-to-each-other/)
`Feature`: 和above一样,预处理进行revert.
```java
// question method signature
int numberWays(List<List<Integer>> hats);

// convert people -> hats to hats -> people
List<Integer>[] hatsRev = new List[HAT_NUM + 1];

long[][] dp = new long[HAT_NUM + 1][personSet];
// base case
for(int i = 1; i <= HAT_NUM; i++) {
    List<Integer> persons = hatsRev[i];
        for(Integer person : persons) {
            dp[i][1<<person] = 1;
    }
}
// iterate
for(int i = 1; i <= HAT_NUM; i++) {
    List<Integer> persons = hatsRev[i];
        for(int state = 0; state < personSet; state++) {
            dp[i][state] += dp[i-1][state];
            dp[i][state] %= mod;
            for(Integer person : persons) {
                if((state>>person & 1) == 0) {
                    if(dp[i-1][state] != 0) {
                        dp[i][state | 1<<person] += dp[i-1][state];
                    }
                }
        }
    }
}
```