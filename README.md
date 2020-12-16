# Pierogoids (Boid Simulation)

## Background 
The goal is to simulate 3D flocking behavior of groups of bird-like objects. We were inspired by the simplicity and realistic nature of existing boid simulations and wanted to implement our own twist.

Boid simulations utilize relatively simple rules, but complex behaviors arise from the interplay between them. Without implementing any sort of AI or individual control for any boid, they collectively affect one another to create the flocking behavior seen.

Additionally, we decided to inject our own creative twist to the traditional bird model. Instead of birds or triangles, our subjects were 3D pierogies<sup>0</sup>, and we added extensions such as user-input sliders, pierogi point-of-view, predator, and attractor.

Our simulation is based on Craig Reynolds' boids paper<sup>1</sup> in 1987, which aimed to model flocking behavior of birds and produced a simulation that lay somewhere between predictability and randomness --- the so-called edge of chaos. The overarching concept is that the individual behavior of each bird matters less so than the holistic group behavior, which makes it easy to model with relatively simple mechanics.

Boid simulations are still widely used, with a variety of extensions and changes. Beyond just video games, they have been used in swarm robotics for controlling UAVs, CGI bat swarms, and even for generating motions for armies on foot or horseback in movies and TV shows.

Our boids model uses three main rules for simulation:
* Collision Avoidance: avoid collisions with nearby flockmates
* Velocity Matching: attempt to match velocity with nearby flockmates
* Flock Centering: attempt to stay close to nearby flockmates

Here is a demonstration of our results: https://youtu.be/30657Bs5SEA

## References
Note: most of the starter code, especially in gloo/, are credited to Lingxiao Li, Yuanming Hu, and Dmitriy Smirnov at MIT. 

<sup>0</sup> https://www.facebook.com/SKA.PW/posts/3403330286371177

<sup>1</sup> http://www.cs.toronto.edu/~dt/siggraph97-course/cwr87/

<sup>2</sup> https://www.red3d.com/cwr/nobump/nobump.html
