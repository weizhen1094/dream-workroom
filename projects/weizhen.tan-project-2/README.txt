# UI usage instructions (especially if not described in the project brief)

Shortcut toggle for bounding volume type for all the mesh objects in scene hierarchy window.

Enable/Disable bounding volume types using is Active flag in inspector window.
(Multiple/Duplicate bv types can be added to a single entity
 initial scene setup has all types added to mesh entities in scene)

Object color changes with view frustum culling against biggest bv.
- Inside: Orange 
- Intersecting: Yellow-Green
- Outside: Blue

Toggle visibility of actual mesh object in inspector window under Mesh component.

Move selected object through Transform component in inspector window 
or using keypresses described below:
- position in x-axis using A/D 
- position in y-axis using Q/E
- position in z-axis using W/S
- rotation in x-axis using Up and Down Arrow keys 
- rotation in y-axis using Left and Right Arrow keys
- rotation in z-axis using Comma "," and Period "." key

## Camera Controls 
- Hold down right mouse button in viewport window to adjust your view.
- Hold down right mouse button in viewport window and WASD to move around.
- Toggle between FPS and Orbit camera in inspector window when Camera Entity is selected, or keypress C
- Click on the viewport window once and use mouse scroll to zoom in and out.

## GUI Windows Description 
- Scene Hierarchy(left window):
	Left click on entities to select.
	Right click on entities to bring up sub menu to destroy.
	(note: camera entities cannot be destroyed this way, see below)
- Main Viewport (top center window):
	Attached to "Camera - Main Viewport" entity, the entity is destroyed if you close the window.
- Inspector(top right window):
	Displays and let users modify components the current selected entity has. 
	Some components can only be added if required components are in entity (e.g. BV requires Mesh loaded from models)
- Sub Viewport (bottom left window):
	Same as Main Viewport.
- Drag and drop windows to dock. 
- Windows can be created by "Add Windows" button on menu bar at top of application window (duplicates can be created).

# Completed parts
- [x] Scene Setup 10%
└ Object Load & Diffuse Light Rendering 5%
└ Simple Diffuse Shading Code Implementation 5%
- [x] Bounding Volume Calculations 60%
└ AABB 5%
└ Sphere – Ritter’s 7.5%
└ Sphere – Larsson’s 12.5%
└ Sphere – PCA 15%
└ OBB – PCA 20%
- [x] Display & Interactivity 25%
└ Toggle bounding volume types 5%
└ Show volumes in wireframe 5%
└ Debug + First Person Cameras 15%

# Incomplete/buggy parts with explanation 
# Assumptions and crash conditions

| Does it crash | Severity | Location                            | Issue Description                                                                                        |
| ------------- | -------- | ----------------------------------- | -------------------------------------------------------------------------------------------------------- |
| no            | minor    | Camera                              | Camera mouse look sometimes jerky                                                                        | 
| no            | minor    | SceneHierarchyGUI - DisplayEntities | Changing order of entities by dragging can be wonky sometimes(the order should not be the same)          | 

# File paths, function names, and line numbers of key logic

include/systems/render.hpp - render system
src/systems/render.cpp line 27 Init() 
- loads shaders
- creates primitive meshes
- loads models using assimp 
- creates bounding volumes for models loaded
src/systems/render.cpp line 102 Update() - rendering logic

shaders/ColorFragmentShader.frag line 35 BlinnPhong()

src/boundingvolume.hpp line 177 CreateAabbBruteForce()

src/boundingvolume.hpp line 202 CreateSphereRitters()
src/boundingvolume.hpp line 250 CreateSphereLarssons()
src/boundingvolume.hpp line 324 CreateSpherePCA()

src/boundingvolume.hpp line 361 CreateObbPCA()

# Test platform details
Windows 11
GL Renderer  : NVIDIA GeForce RTX 3060/PCIe/SSE2
GL Version   : 4.6.0 NVIDIA 566.36
GL Version   : 4.6
GLSL Version : 4.60 NVIDIA

# Weekly time breakdown

| Time       | Tasks                                                                            |
| ---------- | -------------------------------------------------------------------------------- |
| Week 1     | Window Class, Input                                                              |
| Week 2     | Buffer Class, Vertex Attribute Management, Functions to generate geometries      |
| Week 3/4   | ImGui setup, ECS, FPS + Orbit camera, Light + Material                           |
| Week 5     | Load .obj using Assimp, Bounding volumes                                         |
| Week 8     | Bounding volumes                                                                 |
| Week 9/10  | BVH                                                                              |
| Week 11    | Intersection Test scenes                                                         |