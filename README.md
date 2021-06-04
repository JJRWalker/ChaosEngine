# ChaosEngine

Chaos Engine is a vulkan based 2D game engine written in C++ in my spare time, primarily being developed for making roguelikes. The main goal for the engine is to make it easily expandable and portable. So if in the future we want to expand to 3D development, we can just add extra features such as mesh importing, 3D colliders, ect... ontop of our current systems. Another thing I wanted to emphasise in the engine was the ability to do everything through code if you wanted to. Engines like Unity are extremely user friendly, but certain features aren't accessible to programmers through code. While there should be UI options for these things to speed up development, there should also be a user friendly way of doing them in code. Finally I wanted a C++ engine that focused on the C++ API, a lot of engines are written in C++ but focus a lot on the user experience of those who aren't making games in C++. E.g. Unreals blueprints, Godots GD Script, Unity C#. While these are great for making games rapidly, I wanted to try and write something more performant and learn a lot along the way.

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
* Button input system (allows multiple keys to be assigned to an input)
* Batch Rendering
* Textures and texture mapping
* Sub Textures (allows for the use of sprite sheets)
* Debug GUI (using ImGui, currently causes some issues on certain hardware, see note below)
* Editor UI (Has some basic functionality such as changing transform and sprite rendered however cannot save / load yet)
* Node based entity system (similar to the one used in godot)
* Collision system (simple for now, quads and circles)

## Next steps
* Improving the renderer, overtime it has become less stable might be better to redo it entirely
* Controller support for input system
* Level serialization
* Audio
