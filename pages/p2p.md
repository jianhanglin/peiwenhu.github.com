---
layout: page
title: "P2p discussion"
description: "504 project discussion"
---
{% include JB/setup %}

#which references contain the background you need to complete the project

[Peer-To-Peer Searching](http://www.searchtools.com/info/peer-to-peer.html)  

[Efficient Peer-To-Peer Lookup Based on a Distributed Trie](/assets/p2p/MichaelRadek.pdf)  

[Chord: A Scalable Peer-to-peer Lookup Service for Internet Applications](/assets/p2p/chord.pdf)  

[Efficient Peer-to-Peer Keyword Searching](/assets/p2p/Patrick.pdf)  

[web crawler:wikipedia](/assets/p2p/crawlerWiki.pdf)  

#what interesting data structures and algorithms may be used in your project

Data structure: distributed hash table.  

Algorithms: To be determined  

#the envisioned role of each person in the group project and a tentative schedule of goals and deadlines leading up to the project’s completion

To be determined  

#potential modifications and optimizations to the system.

We decide to optimize the p2p search peers by replacing computers with embedded systems like Raspberry Pis, once the fundamental goal is achieved.  

#your testing and evaluation strategy for the project.

As the project description suggests:  

We should be able to have:  

* A Graphical User Interface for searching for one word.
* A distributed web crawler: the amount of indexing done by any one peer in the system must be capped by a user-allotted amount, no one peer should hold more than half of the database.
* Crawling the www.bu.edu domain in under three days.
* Searching the www.bu.edu domain on two peers in under 10 seconds.
* The entire system must be able to function without access to the broader Internet.