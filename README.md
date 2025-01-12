# HellbenderEngine /!\ WIP /!\
## My personnal toy Rendering engine/framework.
## A vulkan abstraction library

For samples take a look at the sample repository [HellbenderSamples](https://github.com/Goutch/HellbenderSamples)
![Path Tracing Screenshot](https://github.com/Goutch/HellbenderEngine/blob/master/Screenshots/Sponza.PNG)

# Goals
- **Common library for my Vulkan projects**
- **Cross platforms**
- **Lightweight**
- **Raytracing,Compute,Rasterization**

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

### Wiki
- [Home](https://github.com/Goutch/HellbenderEngine/wiki)

