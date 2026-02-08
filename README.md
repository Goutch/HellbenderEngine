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
#### HB Graphics:
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
- Ray tracing pipeline
	- miss
	- closest hit
	- any hit
	- intersection

#### HB ECS
- Entity Grouping per components
- Systems
- Scene (Container for Systems and Entities)
- OnAttach/OnDetach Component Events
- Update/Draw/Render Events
- Included Components
  - Camera (perspective)
  - Camera2D (orthographic)
  - CameraController
  - MeshRenderer
  - ModelRenderer
  - TextRenderer
  - UIPanel
#### Other
- Sounds
- Keyboard and Mouse input handling

#### TODO :
- [ ] 2D transform component
- [ ] Model animations support (low prio)
- [ ] ECS
  - [ ] Archetype caching
- [ ] UI library
  - [ ] UIThemes
  - [ ] Buttons
  - [ ] Labels
  - [ ] Panels
- [ ] Resources rework, use handles instead of pointers
- [ ] Documentation
### Wiki
- [Home](https://github.com/Goutch/HellbenderEngine/wiki)

