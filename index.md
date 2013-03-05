---
layout: page
title: Peiwen Hu's Blog

---
{% include JB/setup %}

Hello, this is my blog, where I will periodically post some stuff about what I've been doing(or perhaps thinking about).
If you happen to enter here without knowing who I am, [here is my profile.](/pages/about.html)
<hr class="featurette-divider">

<div class="featurette">
<h2 class="featurette-heading">Posts</h2>
<p class="lead">Though I prefer doing rather than talking</p>
</div>

<ul class="posts">
  {% for post in site.posts %}
    <li><span>{{ post.date | date_to_string }}</span> &raquo; <a href="{{ BASE_PATH }}{{ post.url }}">{{ post.title }}</a></li>
  {% endfor %}
</ul>

<div class="featurette">
<h2 class="featurette-heading">Ongoing stuff</h2>
<a href="http://www.github.com/peiwenhu">my git repositories</a>
</div>

### People I admire

[Lynn Yang(in Mandarin)](http://www.lynnyang.co/)  
[Bret Victor](http://worrydream.com/)  
A lot

##[My Amazon wish list: )]( http://amzn.com/w/274A3YF743468)
