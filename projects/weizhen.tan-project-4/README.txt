# UI usage instructions (especially if not described in the project brief)

The oct/k-dtrees will update whenever you update the tree settings or when objects move.

Toggle wireframe for all the objects in the settings windows.

For octrees, only cells containing objects will be rendered.
Colors for the objects are randomly generated for each cell on a level and may sometimes 
be too similar, you can regenerate the colors in the settings windows.

For kdtree, objects at split will remain orange.

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
	Some components can only be added if required components are in entity (e.g. BV requires Mesh loaded from models)
- Project 4 Settings(bottom center window):
	Window for Project 4 to toggle Octree/k-d tree settings.
- Sub Viewport (bottom left window):
	Same as Main Viewport.
- Drag and drop windows to dock. 
- Windows can be created by "Add Windows" button on menu bar at top of application window (duplicates can be created).

# Completed parts
- [x] Scene Creation 15%
└ Scene created with specified objects 15%
- [x] Adaptive Octree 40%
└ Creation of adaptive octree 10%
└ Two straddling resolution methods 10%
└ Termination criteria implementation 10%
└ Colored level rendering 10%
- [x] KD-Tree 40%
└ Creation of KD-tree 10%
└ Two split-point experiments 10%
└ Termination criteria implementation 10%
└ Colored level rendering 10%

# Incomplete/buggy parts with explanation 
# Assumptions and crash conditions

| Does it crash | Severity | Location                            | Issue Description                                                                                        |
| ------------- | -------- | ----------------------------------- | -------------------------------------------------------------------------------------------------------- |
| no            | minor    | Camera                              | Camera mouse look sometimes jerky                                                                        | 
| no            | minor    | SceneHierarchyGUI - DisplayEntities | Changing order of entities by dragging can be wonky sometimes(the order should not be the same)          | 

# File paths, function names, and line numbers of key logic

src/octree.cpp line 17, line 32 BuildOctree()
src/octree.cpp line 65 InsertIntoOctree()

src/kdtree.cpp line 17 BuildKDTree()
src/kdtree.cpp line 70 PartitionObjects()
include/components/boundingvolume.hpp line 544 SplitAABB()

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
| Week 12    | Octree and k-d tree                                                              |