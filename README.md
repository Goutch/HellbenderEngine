# HellbenderEngine /!\ WIP /!\
## My personnal toy Rendering engine/framework.
## A vulkan abstraction library

# Goals
- **Common library for my Vulkan projects**
- **Cross platforms**
- **Lightweight**
- **Fast**

# Why?
- **I like Vulkan and graphics**
- **I want to do ray tracing.**

# Features
#### HB Graphics library:
- Rasterization Pipeline
  - Ordered rendering for transparency and UI
  - Unordered rendering for 3D opaque objects
- Text/Font rendering
- Resource loading:
	- Textures
	- Shaders
	- GLTF models
    - Fonts
- Compute shaders
- Shader reflection with SPIRV-Cross
- Ray tracing pipeline shaders
	- miss
	- closest hit
	- any hit
	- intersection
#### Other
- Optionnal Scene/ECS architecture
	- Camera (perspective)
	- Camera2D (orthographic)
	- CameraController
	- MeshRenderer
    - ModelRenderer
- Sounds
- Event system
- keyboard and mouse input handling

#### TODO :
- [ ] Documentation
- [x] Model loading 
- [ ] Model animations support
- [x] Support for Multiple descriptor sets
- [ ] ECS multithreading support
## Getting started
### Examples
- [HellbenderExamples](https://github.com/Goutch/HellbenderExamples) Repo
### Wiki (Might be Out of date...)
- [Home](https://github.com/Goutch/HellbenderEngine/wiki)
## Q&A
Q: Why should I use this engine instead of unity/unreal?  
A: You shouldn't this is a toy engine made for me.

### Notes:
HBE used to support Opengl but it is deprecated as of now I am not sure if I will support it in the future. Checkout the opengl branch if you want a "working" Opengl version.(Warning: the engine was nothing like it is today)

![Path Tracing Screenshot](https://github.com/Goutch/HellbenderEngine/blob/master/screenshots/Sponza.PNG)

