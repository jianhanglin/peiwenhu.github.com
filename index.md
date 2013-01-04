---
layout: page
title: Peiwen Hu's Blog
tagline: Supporting tagline
---
{% include JB/setup %}

H  
ello, this is my blog, where I will periodically post some stuff about what I've been doing(or perhaps thinking about).
If you happen to enter here without knowing who I am, [here is my profile.](/pages/about.html)

## Posts

<ul class="posts">
  {% for post in site.posts %}
    <li><span>{{ post.date | date_to_string }}</span> &raquo; <a href="{{ BASE_PATH }}{{ post.url }}">{{ post.title }}</a></li>
  {% endfor %}
</ul>

## Ongoing projects
[My Github repositories](https://github.com/peiwenhu)
## Coursework

##### Fall 2012
* Computer Architecture(3.7/4.0)
* Networking the physical world(3.7/4.0)
* Advanced digital design with Verilog and FPGAs(3.7/4.0)

[Transcript](/assets/main/Grades)

##### Spring 2013(To be attended)

* Advanced data structure
* Enterprise Client-Server Software Systems Design
* High Performance Programming with Multicore and GPUs
* Introduction to Embedded Systems

## People I admire

[Lynn Yang(in Mandarin)](http://www.lynnyang.co/)  
[Bret Victor](http://worrydream.com/)  
A lot

##[My Amazon wish list: )]( http://amzn.com/w/274A3YF743468)