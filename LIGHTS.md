# LIGHTS PLAN

I am planning to add PBR lights to my engine.
I currently have all materials loaded.

The goal is to support directional, point and spot lights.
When an actor with a light component is selected, their light bounds will be drawn using the debug renderer (arrow, sphere, cone).

The goal is to make the light struct extensible so it's easy to add shadow atlas support in the future. For directional lights, the renderer will have it's own shadow mask texture so that I can select between CSM and raytraced shadows. The shadow techniques I will implement myself.

On update, the light component will submit itself to the renderer. The light list will then be passed to the render passes via RenderPassExecuteInfo.

The light lists will be passed via bindless. The goal in the future is to have a global scene bindless resource that holds all light buffers indices, all instances and materials etc to allow for extremely flexible rendering.

Components to add:
- Directional Light
- Point Light
- Spot Light

Files to look at:
- GBufferPass (I don't think it samples normal/ORM/emissive textures right now)
- DeferredPass
- Renderer.cpp
- World.cpp
- WorldSystem.cpp
