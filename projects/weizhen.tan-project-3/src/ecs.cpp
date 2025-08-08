/**
@file    ecs.cpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains definition of the ECS class that encapsulates entt logic.

*//*__________________________________________________________________________*/

#include <ecs.hpp>
#include <components/transform.hpp>
#include <components/renderable.hpp>
#include <components/material.hpp>
#include <components/light.hpp>
#include <components/camera.hpp>
#include <components/boundingvolume.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

registry& ECSManager::registry()          { return m_Registry; }
unsigned ECSManager::count()              { return m_RenderOrderTracker; }
entity ECSManager::selectedEnt()          { return m_SelectedEnt; }
void ECSManager::selectedEnt(entt::entity _ent) { m_SelectedEnt = _ent; }

void ECSManager::DestroyEntity(entt::entity _ent) 
{   if (m_Registry.valid(_ent)) { m_Registry.destroy(_ent); }  }

    entity ECSManager::CreateDefaultEntity(std::string _name, vec3 _pos, vec3 _rot, vec3 _scale)
    {
        entity ent = m_Registry.create();
        m_Registry.emplace<Transform>(ent, _pos, _rot, _scale);
        m_Registry.emplace<RenderOrder>(ent).value = m_RenderOrderTracker++;
        m_Registry.emplace<EntityName>(ent, _name);
        return ent;
    }
    entity ECSManager::CreateCameraEntity()
    {
        entity ent = CreateDefaultEntity();
            m_Registry.get<EntityName>(ent).value = "Camera";
            m_Registry.emplace<Framebuffer>(ent);
            m_Registry.emplace<Camera>(ent);
        return ent;
    }
    entity ECSManager::CreateDirectionLightEntity()
    {
        entity ent = CreateDefaultEntity();
            m_Registry.get<Transform>(ent).position = vec3(0.0f, 25.0f, 0.0f);
            m_Registry.get<EntityName>(ent).value = "Directional Light";
            auto& light = m_Registry.emplace<Light>(ent);
            light.type = LIGHT_DIRECTIONAL;
        return ent;
    }
    entity ECSManager::CreatePointLightEntity()
    {
        entity ent = CreateDefaultEntity();
            m_Registry.get<Transform>(ent).position = vec3(0.0f, 5.0f, 0.0f);
            m_Registry.get<EntityName>(ent).value = "Point Light";
            auto& light = m_Registry.emplace<Light>(ent);
            light.type = LIGHT_POINT;
        return ent;
    }
    entity ECSManager::CreateSpotLightEntity()
    {
        entity ent = CreateDefaultEntity();
            m_Registry.get<Transform>(ent).position = vec3(0.0f, 5.0f, 0.0f);
            m_Registry.get<EntityName>(ent).value = "Spot Light";
            auto& light = m_Registry.emplace<Light>(ent);
            light.type = LIGHT_SPOT;
        return ent;
    }
    entity ECSManager::CreatePoint3DEntity()
    {
        entity ent = CreateDefaultEntity();
            m_Registry.get<EntityName>(ent).value = "Point3D";
            auto& mesh = m_Registry.emplace<Renderable>(ent, Renderable("Point3D"));
            auto& mat = m_Registry.emplace<Material>(ent);
        return ent;
    }
    entity ECSManager::CreatePlaneEntity()
    {
        entity ent = CreateDefaultEntity();
            m_Registry.get<EntityName>(ent).value = "Plane";
            auto& mesh = m_Registry.emplace<Renderable>(ent, Renderable("Plane"));
            auto& mat = m_Registry.emplace<Material>(ent);
        return ent;
    }
    entity ECSManager::CreateTriangleEntity()
    {
        entity ent = CreateDefaultEntity();
            m_Registry.get<EntityName>(ent).value = "Triangle";
            auto& mesh = m_Registry.emplace<Renderable>(ent, Renderable("Triangle"));
            auto& mat = m_Registry.emplace<Material>(ent);
        return ent;
    }
    entity ECSManager::CreateSphereEntity()
    {
        entity ent = CreateDefaultEntity();
            m_Registry.get<EntityName>(ent).value = "Sphere";
            auto& mesh = m_Registry.emplace<Renderable>(ent, Renderable("Sphere"));
            auto& mat = m_Registry.emplace<Material>(ent);
        return ent;
    }
    entity ECSManager::CreateAABBEntity()
    {
        entity ent = CreateDefaultEntity();
            m_Registry.get<EntityName>(ent).value = "AABB";
            auto& mesh = m_Registry.emplace<Renderable>(ent, Renderable("AABB"));
            auto& mat = m_Registry.emplace<Material>(ent);
        return ent;
    }
    entity ECSManager::CreateRayEntity()
    {
        entity ent = CreateDefaultEntity();
            m_Registry.get<EntityName>(ent).value = "Ray";
            auto& mesh = m_Registry.emplace<Renderable>(ent, Renderable("Ray"));
            auto& mat = m_Registry.emplace<Material>(ent);
        return ent;
    }
    entity ECSManager::CreateMeshEntity(std::string _name)
    {
        entity ent = CreateDefaultEntity();
            m_Registry.get<EntityName>(ent).value = _name;
            auto& mesh = m_Registry.emplace<Renderable>(ent, Renderable(_name));
            auto& mat = m_Registry.emplace<Material>(ent);
        return ent;
    }