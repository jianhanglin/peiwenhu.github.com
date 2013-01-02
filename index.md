---
layout: page
title: Peiwen Hu's Blog
tagline: Supporting tagline
---
{% include JB/setup %}

H  
ello, this is my blog, where I will periodically post some stuff about what I've been doing.
If you happen to enter here without knowing who I am, [here is my profile.](/pages/about.html)

## Posts

<ul class="posts">
  {% for post in site.posts %}
    <li><span>{{ post.date | date_to_string }}</span> &raquo; <a href="{{ BASE_PATH }}{{ post.url }}">{{ post.title }}</a></li>
  {% endfor %}
</ul>

##People I admire

[Lynn Yang(in Mandarin)](http://www.lynnyang.co/)  
[Bret Victor](http://worrydream.com/)

##[My Amazon wish list]( http://amzn.com/w/274A3YF743468)