include(FetchContent)

# Macro to import GLFW
macro(import_glfw)
    if(NOT TARGET glfw)  # Guard to prevent multiple inclusion
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG 3.3.8
        )
        if(NOT glfw_POPULATED)
            set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
            set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
            set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
            FetchContent_Populate(glfw)
        endif()

        add_subdirectory(${glfw_SOURCE_DIR})
        include_directories(${GLFW_SOURCE_DIR}/include)
    endif()
endmacro()

# Macro to import glm
macro(import_glm)
    if(NOT TARGET glm)  # Guard to prevent multiple inclusion
        FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            # GIT_TAG 0.9.9.8
            GIT_TAG master
        )
        FetchContent_MakeAvailable(glm)

        include_directories(${glm_SOURCE_DIR})
    endif()
endmacro()

# Macro to import glew
macro(import_glew)
    if(NOT TARGET glew_s)  # Guard to prevent multiple inclusion
        include(FetchContent)

        FetchContent_Declare(
            glew
            GIT_REPOSITORY https://github.com/omniavinco/glew-cmake.git
            GIT_TAG master  # Or use a specific commit or tag if preferred
        )

        # Disable shared builds if needed
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libs" FORCE)
        FetchContent_MakeAvailable(glew)

        # Optional: include directories if needed manually (though target_link_libraries handles it)
        include_directories(${glew_SOURCE_DIR}/include)
    endif()
endmacro()

# Macro to import EnTT
macro(import_entt)
    if(NOT TARGET entt)  # Guard to prevent multiple inclusion
        FetchContent_Declare(
            entt
            GIT_REPOSITORY https://github.com/skypjack/entt.git
            GIT_TAG master  # Or use a specific commit or tag if preferred
        )
        FetchContent_MakeAvailable(entt)

        # Optional: include directories if needed manually (though target_link_libraries handles it)
        include_directories(${entt_SOURCE_DIR})
    endif()
endmacro()

# Macro to import ImGui
macro(import_imgui)
    if(NOT TARGET imgui)  # Guard to prevent multiple inclusion
        include(FetchContent)

        FetchContent_Declare(
            imgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG docking  # Or use a specific commit or tag if preferred
        )
        FetchContent_MakeAvailable(imgui)
        set(IMGUI_SOURCES 
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        )
        set(IMGUI_HEADERS 
            ${imgui_SOURCE_DIR}/imgui.h
            ${imgui_SOURCE_DIR}/imconfig.h
            ${imgui_SOURCE_DIR}/imgui_internal.h
            ${imgui_SOURCE_DIR}/imstb_rectpack.h
            ${imgui_SOURCE_DIR}/imstb_textedit.h
            ${imgui_SOURCE_DIR}/imstb_truetype.h
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
        )
        add_library(
            imgui 
            STATIC 
            ${IMGUI_HEADERS}
            ${IMGUI_SOURCES}
            )
        include_directories(${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
    endif()
endmacro()

macro(import_imguizmo)
    if(NOT TARGET imguizmo)  # Guard to prevent multiple inclusion
        include(FetchContent)

        FetchContent_Declare(
            imguizmo
            GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
            GIT_TAG master  # Or use a specific commit or tag if preferred
        )
        FetchContent_MakeAvailable(imguizmo)
        add_library(
            imguizmo 
            STATIC 
            ${imguizmo_SOURCE_DIR}/ImGuizmo.h
            ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
            )
        include_directories(${imguizmo_SOURCE_DIR})
    endif()
endmacro()

macro(import_properties)
    if(NOT TARGET properties)  # Guard to prevent multiple inclusion
        include(FetchContent)

        FetchContent_Declare(
            properties
            GIT_REPOSITORY https://gitlab.com/LIONant/properties.git
            GIT_TAG master  # Or use a specific commit or tag if preferred
        )
        FetchContent_MakeAvailable(properties)
        add_library(
            properties 
            STATIC 
            ${properties_SOURCE_DIR}/src/Properties.h
            )
        include_directories( ${properties_SOURCE_DIR}/src )
        # tell cmake this is c++ bc thrs no source files 
        set_target_properties(properties PROPERTIES LINKER_LANGUAGE CXX)
    endif()
endmacro()

macro(import_assimp)
    if(NOT TARGET assimp)  # Guard to prevent multiple inclusion
        include(FetchContent)

        FetchContent_Declare(
            assimp
            GIT_REPOSITORY https://github.com/assimp/assimp.git
            GIT_TAG master  # Or use a specific commit or tag if preferred
        )
        FetchContent_MakeAvailable(assimp)

        include_directories(${assimp_SOURCE_DIR})
    endif()
endmacro()

# Macro to import eigen
macro(import_eigen)
    if(NOT TARGET eigen)  # Guard to prevent multiple inclusion
        FetchContent_Declare(
            eigen
            GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
            GIT_TAG master  # Or use a specific commit or tag if preferred
        )
        FetchContent_MakeAvailable(eigen)

        # Optional: include directories if needed manually (though target_link_libraries handles it)
        include_directories(${eigen_SOURCE_DIR})
    endif()
endmacro()

# Macro to import all dependencies
macro(importDependencies)
    message(STATUS "Starting to import dependencies...")

    message(STATUS "Importing GLFW...")
    import_glfw()
    message(STATUS "GLFW imported successfully.")

    message(STATUS "Importing GLM...")
    import_glm()
    message(STATUS "GLM imported successfully.")

    message(STATUS "Importing GLEW...")
    import_glew()
    message(STATUS "GLEW imported successfully.")

    message(STATUS "Importing EnTT...")
    import_entt()
    message(STATUS "EnTT imported successfully.")

    message(STATUS "Importing ImGui...")
    import_imgui()
    message(STATUS "ImGui imported successfully.")

    message(STATUS "Importing ImGuizmo...")
    import_imguizmo()
    message(STATUS "ImGuizmo imported successfully.")

    # message(STATUS "Importing LIONant Properties...")
    # import_properties()
    # message(STATUS "LIONant Properties imported successfully.")

    message(STATUS "Importing Open Asset Import Library (assimp)...")
    import_assimp()
    message(STATUS "assimp imported successfully.")

    message(STATUS "Importing Eigen...")
    import_eigen()
    message(STATUS "Eigen imported successfully.")

    message(STATUS "All dependencies have been imported successfully.")
endmacro()