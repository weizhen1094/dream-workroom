# UI usage instructions (especially if not described in the project brief)

The bounding volumes and bvhierarchy will update whenever you update the bvhierarchy settings or
objects transforms using the inspector window(after letting go).

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
- Project 4 Settings(bottom center window):
	Window for Project 4 to toggle Octree/k-d tree settings.
- Sub Viewport (bottom left window):
	Same as Main Viewport.
- Drag and drop windows to dock. 
- Windows can be created by "Add Windows" button on menu bar at top of application window (duplicates can be created).

## Camera Controls 
- Hold down right mouse button in viewport window to adjust your view.
- Hold down right mouse button in viewport window and WASD to move around.
- Toggle between FPS and Orbit camera in inspector window when Camera Entity is selected.

## Bounding Volume Description
- Toggle visibility using is Active flag in inspector.
- BV is color coded using type. Toggle type in inspector using dropdown menu.
- BV center and extends computed based on entity's transform.

# Discuss the effect of choosing different heuristics in your README.

Nearest neighbour should be chosen for scenes with objects that are grouped close together to avoid overlapping bvs or bvs with empty spaces inside
Minimum child volume and surface area produce tighter bounding volumes than nearest neighbour heuristic

but for the initial scene setup
all three heuristics combined produces the tightest bv with lesser levels, produces same result as min child volume
nearest neighbour only and min child surface area seem to produce the same result.. maybe my implementation is incorrect


# Completed parts
- [x] Scene Setup
Load a scene using any 8 models from the provided dataset.
	Loaded 4Sphere.obj, bunny.obj, cup.obj, head.obj, lucy_princeton.obj, ogre.obj, rhino.obj, starwars1.obj
For each object (OBJ file), compute:
	Axis-Aligned Bounding Box (AABB)
	Bounding Sphere (PCA-based method)
	Oriented Bounding Box (OBB using Covariance Matrix and Eigen Vectors)
Implement:
First-Person Camera (WASD movement)
Top-Down Debug Camera or PIP Camera Overview
Allow toggling between bounding volume types.
- [x] Bounding Volume Hierarchy (70%)
Build BVHs using AABB and Bounding Spheres. 
- [x] Top-Down Method 
Experiment with different split-point strategies: 
    Median of BV centers 
    Median of BV extents 
    K-even splits on one axis 
Choose split planes along X, Y, or Z to minimize total volume of child nodes. 
Implement multiple termination conditions: 
	Stop when a leaf has a single object. 
	Stop when a leaf has a maximum of two objects. 
	Stop when tree height = 2. 
Allow toggling between the different split methods and termination conditions. 
- [x] Bottom-Up Method 
Create a bounding volume for every object. 
Use heuristics to merge child nodes into parent nodes: 
	Nearest neighbor 
	Minimum combined child volume 
	Minimum combined child surface area 
- [x] Discuss the effect of choosing different heuristics in your README.
- [x] Visualization Color each level of the BVH with a unique color

# Incomplete/buggy parts with explanation 
# Assumptions and crash conditions

| Does it crash | Severity | Location                            | Issue Description                                                                                        |
| ------------- | -------- | ----------------------------------- | -------------------------------------------------------------------------------------------------------- |
| no            | minor    | Camera                              | Camera mouse look sometimes jerky                                                                        | 
| no            | minor    | SceneHierarchyGUI - DisplayEntities | Changing order of entities by dragging can be wonky sometimes(the order should not be the same)          | 

# File paths, function names, and line numbers of key logic

include/bvhierarchy.hpp + src/bvhierarchy.cpp - contains BVHierarchy class, which stores the bvh tree and functions to generate bvh
src/bvhierarchy.cpp line 16 BuildTopDown()
src/bvhierarchy.cpp line 52 PartitionObjects()
src/bvhierarchy.cpp line 163 BuildBottomUp()
src/bvhierarchy.cpp line 235 FindNodesToMerge()

include/components/boundingvolume.hpp - contains BoundingVolume, Aabb, Sphere and Obb struct, helper functions to generate bvs and merge bvs
include/components/boundingvolume.hpp line 426 MergeAabbs()
include/components/boundingvolume.hpp line 450 MergeTwoSpheres()
include/components/boundingvolume.hpp line 478 MergeSpheres()
include/components/boundingvolume.hpp line 499 GetAxisWithLargestSpread()

include/gui/bvhgui.hpp + src/gui/bvhgui.cpp - contains BVHGUI class, displays gui for bvh settings and responsible for creating/updating bvh

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