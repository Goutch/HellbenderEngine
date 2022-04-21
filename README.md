# HellbenderEngine
## My personnal toy Rendering engine/framework.
### /!\ WIP: not even in alpha /!\ please keep that in mind while checking the code.

# Goals 
- **Simple API**
- **Cross platforms**
- **Easy to use**
- **Minimal dependencies and lightweight.**
- **FAST**

# Why?
- **I like graphics**
- **I like vulkan**
- **I like to code in c++.**
- **I want to do procedural generation and raytracing.**

# Features
#### HB Graphics library:
- 3D rendering with Vukkan
- Instancing
- Resource loading:
	 - Textures
	 - Shaders
	 - GLTF models

#### Other
- ECS
	- Camera (perspective)
	- Camera2D (orthographic)
	- CameraController 
	- MeshRenderer 
	- InstanceRenderer
- Job System
- Event system
- keyboard and mouse input handling

#### planned for Alpha 1.0:
- Compute shaders
- Ray tracing pipelines
- Model loading and animations
- Font rendering

## Getting started

### Compiling
These are 2 tested compilers : 
- GCC
- VC

### Wiki
- [Home](https://github.com/Goutch/HellbenderEngine/wiki) 
## Qestions and Answers
Q: Why should I use this engine instead of unity/unreal?  
A: You shouldn't this is a toy engine made for me.  

### Notes:
HBE used to support Opengl but it is deprecated as of now I am not sure if I will support it in the future. Checkout the opengl branch if you want a "working" Opengl version.
