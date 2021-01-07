# HellbenderEngine
# Why?
- **I want to Learn graphics and engines.**
- **I like c++.**
- **I want to do a lot of procedural generation and raytracing.**

# Goals 
- **Easy to use and modular API.**
- **Minimal dependencies and lightweight.**
- **Built for programmers.**

# features
#### HB Graphic Engine library: (Priority)
- Entity and components
	- Camera
	- CameraController
	- MeshRenderer
	- ModelRenderer
	- MeshInstancesRenderer
- 3D mesh rendering with OpenGL
- Instancing
- keyboard and mouse input handling
- Resource loading
	 - Model
	 - Texture
	 - Shaders
- Material system
- Job System
- Event system

#### HB Editor: (Not a priority)
 - Basic Scene view
 	- add/destroy entities
	- multi selection
 - Basic Entity inspector 
## In progress:
 - Compute shaders support.
 - Serialization :
	 - Scene , entities and components
	 - Resources, models,textures,
	 - Project file
 - Projects samples.

## Long term goals

 - Vulkan support
 - Scripting
 - Deffered rendering
 - Bullet physics
 - Particules system

## Getting started
### Compiling
These are 3 tested compilers : 
- GCC
- VC
- MINGW

### Wiki
- [Home](https://github.com/Goutch/HellbenderEngine/wiki) 
- [Hello Triangle](https://github.com/Goutch/HellbenderEngine/wiki/Hello-triangle)

## Qestions and Answers
Q: Why should I use this engine instead of unity/unreal?  
A: Tou shouldn't unless you want to use an open source engine.  

Q: Why there is no lighting system?  
A: I am waiting to implement a deffered rendering system before I add any lights.  
