# boids

## Proposal 
We propose implementing a boids model for a flock/school/herd of birds/fish/animals. This would extend assignment 3’s physically-based animation. The final product would be an executable that runs a boid simulation in a simple 3-dimensional world.

In particular, the project begins with implementing the 3 primary rules in Craig Reynolds’ original paper<sup>1</sup>:
* Collision Avoidance: avoid collisions with nearby flockmates
* Velocity Matching: attempt to match velocity with nearby flockmates
* Flock Centering: attempt to stay close to nearby flockmates

Another important aspect in order to get interesting animations is adding non-boid objects to the scene, and implementing some form of object avoidance. There are various ways to approach this<sup>2</sup>, with varying complexity. A “force field” approach seems like it would work well for simple spheres/cylinders.

In order for the animation to last longer than a few seconds in our field of view, adding boundaries will be necessary. These could be simple dimensions that the boids bounce off of or wrap to the other side at first, but could become closer to objects later.

To start, each boid can be one of the small spheres already renderable via the starter code, but could be modified to a paper airplane-like shape or a simple bird model later.

A stretch goal would be allowing users to click locations in the rendering to dynamically alter the direction the boids head toward. This would amount to implementing a simple attractor object, and moving it to a location in 3D space on click.

Another extension for user interaction, time permitting, would be to add sliders allowing users to adjust the parameters of the various boid rules.


## References

<sup>1</sup> http://www.cs.toronto.edu/~dt/siggraph97-course/cwr87/

<sup>2</sup> https://www.red3d.com/cwr/nobump/nobump.html
