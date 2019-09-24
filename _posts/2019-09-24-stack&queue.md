---
layout: post
title:  stack&queue
date:   2019-09-24
---
#### Queue

##### [usage in Java](https://leetcode.com/explore/learn/card/queue-stack/228/first-in-first-out-data-structure/1367/)
```java
	//1. Initialize a queue
	Queue<Integer> q = new LinkedList<>();
	// 2. Get the first element - return null if queue is empty.
    System.out.println("The first element is: " + q.peek());
    // 3. Push new element.
    q.offer(5);
    q.offer(13);
    // 4. Pop an element.
    q.poll();
    // 5. Get the first element.
    System.out.println("The first element is: " + q.peek());
    // 7. Get the size of the queue.
    System.out.println("The size is: " + q.size());
```

##### [Queue and BFS](https://leetcode.com/explore/learn/card/queue-stack/231/practical-application-queue/1372/)
1. used for **doing traversal** or **find the shortest path**

2. Template I
```java
/**
 * Return the length of the shortest path between root and target node.
 */
int BFS(Node root, Node target) {
    Queue<Node> queue;  
    int step = 0;       
    // initialize
    add root to queue;
    // BFS
    while (queue is not empty) {
        step = step + 1;
        // iterate the nodes which are already in the queue
        int size = queue.size();
        for (int i = 0; i < size; ++i) {
            Node cur = the first node in queue;
            return step if cur is target;
            for (Node next : the neighbors of cur) {
                add next to queue;
            }
            remove the first node from queue;
        }
    }
    return -1;          // there is no path from root to target
}
```
2. Template II
```java
/**
 * Return the length of the shortest path between root and target node.
 */
int BFS(Node root, Node target) {
    Queue<Node> queue;  
    Set<Node> visited;  // store all the nodes that we've visited
    int step = 0;       
    // initialize
    add root to queue;
    add root to visited;
    // BFS
    while (queue is not empty) {
        step = step + 1;
        // iterate the nodes which are already in the queue
        int size = queue.size();
        for (int i = 0; i < size; ++i) {
            Node cur = the first node in queue;
            return step if cur is target;
            for (Node next : the neighbors of cur) {
                if (next is not in used) {
                    add next to queue;
                    add next to visited;
                }
                remove the first node from queue;   
            }
        }
    }
    return -1;          // there is no path from root to target
}
```


#### Stack



	













































