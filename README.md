# ChaosEngine

Chaos Engine is a vulkan based 2D game engine written in C++ in my spare time, primarily being developed for making roguelikes.

## Getting access
This repository currently uses git submodules (considering converting the repository to not use them for convienece) so there are a couple extra steps to make it work.  
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
* Batch Rendering
* Textures and texture mapping
* Sub Textures (allows for the use of sprite sheets)
* Debug GUI (using ImGui, currently causes some issues on certain hardware, see note below)
* Basic Entity Component frame work (users can write their own components and add them to entities)

## Next steps
* Scene system (will also help optimise renderer)
* Editor UI (potentially using a seperate project so it is detachable from the main engine)
* Cameras
* Expand number of base components (e.g. renderer component, camera component, collider)
* Basic collision system (quads on quads)
* Sprite Animation
* Maths functions (Vector maths / randomisation helpers)

## Known Issues
* Rendering ImGui causes a blank screen to be rendered over the top of game primatives being rendered to screen, UI is still displayed. Only appears to affect some systems after a recent graphics driver update. Unfortunately I lack the variety of hardware to test this.
