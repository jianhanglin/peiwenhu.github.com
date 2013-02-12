---
layout: post
title: "A self-replicate code in Java"
description: ""
category: Study
tags: Java
---
{% include JB/setup %}

<applet code="com.github.peiwenhu.SelfReplicate.class" codebase="/assets/SelfReplicate/" WIDTH="600" HEIGHT="700">
</applet>

>So What I did is to put all the code in a string: weirdString.  

>Then I split this string into two parts, one before the declaration of the string, one after. And I store the two parts in a String\[\], and modify the original string in order to convert the \\\" and \n and suchlike to be literal, instead of their escaping function.  

>Then print out the first part + the modified string + the second part.  

As an optional assignment for EC504 advanced data structure