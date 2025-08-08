/**
@file    ecs.hpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains the entt header and ECS class that encapsulates entt logic.

*//*__________________________________________________________________________*/

#ifndef ECS_HPP
#define ECS_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <isingleton.hpp>
#include <math.hpp>
#include <entt/entt.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class ECS
 * @brief This class is responsible for storing the entt registry and creation
 * of entities(more later..).
 */
class ECSManager : public ISingleton<ECSManager>
{

public:

    entt::registry& registry();
    unsigned count();
    entt::entity selectedEnt();
    void selectedEnt(entt::entity);

    void DestroyEntity(entt::entity);

    entt::entity CreateDefaultEntity(std::string _name = "Entity", vec3 _pos = vec3(0), vec3 _rot = vec3(0), vec3 _scale = vec3(1));
    entt::entity CreateCameraEntity(); // for viewportGUI
    entt::entity CreateDirectionLightEntity();
    entt::entity CreatePointLightEntity();
    entt::entity CreateSpotLightEntity();
    entt::entity CreatePoint3DEntity();
    entt::entity CreatePlaneEntity();
    entt::entity CreateTriangleEntity();
    entt::entity CreateSphereEntity();
    entt::entity CreateAABBEntity();
    entt::entity CreateRayEntity();
    entt::entity CreateMeshEntity(std::string _name);

private:
    
    friend class ISingleton<ECSManager>;

    /*
     * Ctors and Dtor.
     */

    ECSManager() : m_RenderOrderTracker(0) {}
    ~ECSManager() {}

    entt::registry m_Registry;
    unsigned m_RenderOrderTracker;
    entt::entity m_SelectedEnt; ///< for gui
};

#define ECS ECSManager::GetInstance() // macro for easy access
using namespace entt::literals;
using namespace entt;
#endif /* ECS_HPP */