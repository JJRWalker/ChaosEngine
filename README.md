# ChaosEngine

Chaos Engine is a vulkan based 2D game engine written in C++ in my spare time, primarily being developed for making roguelikes.

## Getting access
Clone repository as you normally would. Then use the following git command lines:

<pre><code>git submodule init  
git pull --recurse 
</code></pre>


## Support
Currently only windows x64 is supported, however development is ongoing with multiplatform development in mind with layers of abstraction being implemented.

As previously stated it is currently vulkan based using GLFW windows, this however can be expanded in future once all the core features have been implemented.

## Current State
Features currently implemented:
* Logging
* Input Polling
* Basic Rendering
* Textures and texture mapping

## Next steps
* Scene system (will also help optimise renderer)
* Cameras
* Entity Component System
* Basic collision system (quads on quads)
* Sprite Animation
* Maths functions (Vector maths / randomisation helpers)
