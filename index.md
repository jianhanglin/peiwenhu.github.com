---
layout: page
title: Peiwen Hu's Blog

---
{% include JB/setup %}

Hello, this is my blog, where I will once in a thousand years post some stuff about what I've been doing(or perhaps thinking about).
If you happen to enter here without knowing who I am, [here is my profile.](/pages/about.html)
  <hr class="featurette-divider">

<div class="container marketing">

  <div class="row">

	<div class="span4">
	  <p><a class="btn" href="{{ BASE_PATH }}{{ site.posts[0].url }}">Read more &raquo;</a></p>
	  <h2>{{site.posts[0].title}}</h2>
	  <img class="img-rectangle" src="{{site.posts[0].img}}"> 

	  <p>{{site.posts[0].description}}</p>
	</div><!-- /.span4 -->

	<div class="span4">
	  <p><a class="btn" href="{{ BASE_PATH }}{{ site.posts[1].url }}">Read more &raquo;</a></p>
	  <h2>{{site.posts[1].title}}</h2>
	  <img class="img-rectangle" src="{{site.posts[1].img}}"> 

	  <p>{{site.posts[1].description}}</p>
	</div><!-- /.span4 -->

	<div class="span4">
	  <p><a class="btn" href="{{ BASE_PATH }}{{ site.posts[2].url }}">Read more &raquo;</a></p>
	  <h2>{{site.posts[2].title}}</h2>
	  <img class="img-rectangle" src="{{site.posts[2].img}}"> 

	  <p>{{site.posts[2].description}}</p>
	</div><!-- /.span4 -->
  </div><!-- /.row -->
<!-- end heading -->

<div class="featurette">
<h2 class="featurette-heading">Posts</h2>
<p class="lead">"talk is cheap"</p>
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

<!--  <hr class="featurette-divider"> -->
</div>

  <hr class="featurette-divider">

<div class="featurette">
<h2 class="featurette-heading">People I admire</h2>
</div>


[Lynn Yang(in Mandarin)](http://www.lynnyang.co/)  
[Bret Victor](http://worrydream.com/)  
A lot
  <hr class="featurette-divider">

##[My Amazon wish list: )]( http://amzn.com/w/274A3YF743468)
