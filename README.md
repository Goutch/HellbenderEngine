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
- [ ] Fix vram leak when resizing window or changing the vsync
- [ ] 2D transform component
- [ ] Model animations support (very low prio, not needed right now)
- [ ] ECS
  - [ ] Local transformations, transformations should be able to be calculated in a separate thread. this is already possible but not really straight forward we need.
  - [ ] Archetype caching
  - [ ] When doing parallel foreach the group should give a thread index so user can accumulate a result.
- [ ] UI library
  - [ ] UIThemes
  - [ ] Layout system
  - [ ] Buttons
  - [ ] Labels
  - [ ] Panels
- [ ] Engine Internal rework
  - [ ] Swapchain semaphores are not handled correctly, new validation layers will throw an error.
  - [ ] Descriptor allocation need to change, use a descriptor allocator from VkGuide or something similar
- [ ] Engine API rework
  - [ ] Resources
    - [x] Use Handles
      - [x] Pipeline
      - [x] PipelineInstance
      - [x] Images
      - [x] Shaders
      - [x] RenderTargets
      - [x] Fences
      - [x] Buffers
      - [x] TexelBuffers
      - [x] Meshes
      - [ ] Font 
    - [ ] Add Shader hot reloading
    - [ ] Geometry class has too many parameters, should use an info struct as an input.
- [ ] Documentation
### Wiki
- [Home](https://github.com/Goutch/HellbenderEngine/wiki)

