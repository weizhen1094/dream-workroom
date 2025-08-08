# UI usage instructions (especially if not described in the project brief)

Objects change color on collision, change intersection tests scenes using top leftmost "Scenes" button on menu bar.

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
- Inspector(right window):
	Displays and let users modify components the current selected entity has. 
- Sub Viewport (bottom left window):
	Same as Main Viewport.
- Drag and drop windows to dock. 
- Windows can be created by "Add Windows" button on menu bar at top of application window (duplicates can be created).

# Completed parts
- [x] Task 1: Window class 
- [x] Task 2 & 3: Buffer class VBO and Attribute management
- [x] Task 4: ECS
- [x] Task 5: Light rendering
- [x] Task 6: Interactivity (Camera)
- [x] Basic Intersection Tests
	Sphere vs Sphere
	AABB vs Sphere
	Sphere vs AABB
	AABB vs AABB
- [x] Point-Based Tests
	Point vs Sphere
	Point vs AABB
	Point vs Triangle
	Point vs Plane
- [x] Ray-Based Tests
	Ray vs Plane
	Ray vs AABB
	Ray vs Sphere
	Ray vs Triangle
- [x] Plane-Based Tests
	Plane vs AABB
	Plane vs Sphere

# Incomplete/buggy parts with explanation 
# Assumptions and crash conditions

| Does it crash | Severity | Location                            | Issue Description                                                                                        |
| ------------- | -------- | ----------------------------------- | -------------------------------------------------------------------------------------------------------- |
| no            | minor    | Camera                              | Camera mouse look sometimes jerky                                                                        | 
| no            | minor    | SceneHierarchyGUI - DisplayEntities | Changing order of entities by dragging can be wonky sometimes(the order should not be the same)          | 

# File paths, function names, and line numbers of key logic

include/intersectiontests.hpp - contains functions to test for pairwise intersection/classification of inside/intersecting/outside

src/intersectiontests.cpp line 15 OverlapSphereSphere()
src/intersectiontests.cpp line 21 OverlapSphereAabb()
src/intersectiontests.cpp line 30 OverlapAabbAabb()

src/intersectiontests.cpp line 39 OverlapPointSphere()
src/intersectiontests.cpp line 44 OverlapPointAabb()
src/intersectiontests.cpp line 51 OverlapPointTriangle()
src/intersectiontests.cpp line 60 ClassifyPointPlane()

src/intersectiontests.cpp line 68 IntersectionTimeRayPlane()
src/intersectiontests.cpp line 76 IntersectionTimeRayTriangle()
src/intersectiontests.cpp line 102 IntersectionTimeRaySphere()
src/intersectiontests.cpp line 123 IntersectionTimeRayAabb()

src/intersectiontests.cpp line 160 ClassifyPlaneAabb()
src/intersectiontests.cpp line 170 ClassifyPlaneSphere()

include/systems/collision.hpp - system that checks for intersections between primitives and updates the hasCollided flag in Transform component
src/systems/collision.cpp line 23 Update() - checking every pair of primitives for intersections

include/systems/movement.hpp - system that updates Transform components from user input
src/systems/movement.cpp line 24 Update() - polling for user input every frame and updating Transform components

include/systems/render.hpp - render system
src/systems/render.cpp line 27 Init() - loads shaders and creates primitive meshes
src/systems/render.cpp line 102 Update() - rendering logic

include/components/camera.hpp - contains the camera component struct

include/components/light.hpp  - contains the light component struct

include/ecs.hpp + src/ecs.cpp - contains ECSManager class to encapsulate entt with helper functions to create entities with components e.g. CreateSphereEntity();

include/buffer.hpp - contains Vertex struct, Buffer and Framebuffer classes that encapsulates and handle OpenGL logic to create/bind/draw/delete VBO,VAO,EBO

include/components/renderable.hpp 
- model/mesh component class that contains information needed to render a model loaded from .obj or primitives
- helper functions to generate primitives such as Point3D, Plane, Triangle, Sphere, AABB, Ray etc.. 
- helper function to load models using assimp library 

include/window.hpp + src/window.cpp - contains Window class that encapsulates GLFW logic.

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