Currently a simple 3D graphics engine with features such as .obj loading, primitives intersection tests, bounding volume construction(AABB,spheres,OBB), bounding volume hierarchy, view frustum culling, octree, kd-tree but aiming to develop this into a game engine, was originally built for CSD3151/CS350.

# How to build the project
Click on `run.bat` or using command prompt create a build folder in root and `cmake ..` in build folder directory

#### things to note regarding building the project

using run.bat takes longer as it also runs the `cmake --build` command 
  
if cmake is unable to find git 
> add below lines of code to top of `CMakeLists.txt` and replace "file_path"s with your directory of git 
```
set(GIT_EXECUTABLE "C:/Program Files/Git/cmd/git.exe" CACHE FILEPATH "C:/Program Files/Git/cmd/git.exe" FORCE)
find_package(Git)
message(STATUS "GIT_EXECUTABLE='${GIT_EXECUTABLE}'")
```

# TODO LIST (not in any particular order)
- [ ] refactor CSD3151 projects into a single project
- [ ] add support for multiple light sources
- [ ] add shadows(gpass)
- [ ] add support for textures
- [ ] proper asset management for loading of textures and models
- [ ] serialization of scenes
- [ ] look into entt reflection
- [ ] fix orthographic camera 
- [ ] integrate imguizmo
- [ ] render layer system 
- [ ] entity tree for nested entities
- [ ] undo redo system
..more to add wahoo

# Buggy parts with explanation 
| Does it crash | Severity | Location                            | Issue Description                                                                                        |
| ------------- | -------- | ----------------------------------- | -------------------------------------------------------------------------------------------------------- |
| no            | minor    | Camera                              | Camera mouse look sometimes jerky                                                                        | 
| no            | minor    | SceneHierarchyGUI - DisplayEntities | Changing order of entities by dragging can be wonky sometimes(the order should not be the same)          | 

# dependencies
glfw
glew
EnTT
imgui
assimp
eigen
