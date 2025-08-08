/**
@file    octree.hpp
@author  weizhen.tan@digipen.edu
@date    22/07/2025

This file contains the declaration of the Octree class and struct OctTreeNode.

*//*__________________________________________________________________________*/

#ifndef OCTREE_HPP
#define OCTREE_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <vector>
#include <memory>
#include <isingleton.hpp>
#include <ecs.hpp>
#include <components/boundingvolume.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

enum OCTREE_STRADDLING_TYPE 
{ 
	OBJ_CENTER, 
	ALL_OVERLAPPING_CELLS,
	CURRENT_LV_CELL,
	SPLIT_OBJ,
	OCTREE_STRADDLING_TYPE_TOTAL,
};

/**
 * @struct OctTreeNode
 * @brief This struct holds the data for octree node
 */
struct OctTreeNode
{
	vec3 center;
	float halfExtent;
	std::shared_ptr<BoundingVolume> bv;
	std::vector<entity> entities;
	std::unique_ptr<OctTreeNode> pChildren[8];
	vec3 GetMin() { return center - vec3(halfExtent); };
	vec3 GetMax() { return center + vec3(halfExtent); };
};

/**
 * @class Octree
 * @brief This class is responsible for construction, storing and management of 
 * Octree.
 */
class Octree : public ISingleton<Octree>
{

public:

	/**
	  * @brief builds octree top down recursively
	  * @param _ents - entities in scene
	  */
	void BuildOctree(std::vector<entity> const& _ents);

	OCTREE_STRADDLING_TYPE& GetStraddleMethod() { return m_StraddleMethod; };
	int& GetNumObjPerNode() { return m_NumObjPerNode; };
	entity GetOctreeEnt() { return m_OctreeEnt; };
	void ClearTree() { m_Root = std::make_unique<OctTreeNode>(); };

private:

	friend class ISingleton<Octree>;

	Octree() : m_StraddleMethod(OBJ_CENTER), m_NumObjPerNode(10), m_Root(std::make_unique<OctTreeNode>()),
		m_OctreeEnt(ECS.CreateDefaultEntity("Octree")) { ECS.registry().emplace<BVList>(m_OctreeEnt); };
	~Octree() {  };

	/**
	 * Helper functions
	 */

	/**
	  * @brief builds octree top down recursively
	  * @param _depth - depth of the octree
	  * @param _node - current cell to split
	  * @param _ents - entities in parent cell
	  */
	void BuildOctree(int _depth, std::unique_ptr<OctTreeNode> const& _node, std::vector<entity> const& _ents);
	/**
	  * @brief insert entities into cell if its inside and handles straddling
	  * @param _node - current cell to insert into
	  * @param _ents - entities in parent cell
	  */
	void InsertIntoOctree(std::unique_ptr<OctTreeNode> const& _node, std::vector<entity> const& _ents);
	/**
	  * @brief checks if entity is inside or straddling cell
	  * @param _node - current cell to insert into
	  * @param _ent - entity AABB
	  */
	SIDE_RESULT ClassifyEntityCell(std::unique_ptr<OctTreeNode> const& _node, Aabb const& _ent);
	std::tuple<int, bool> GetChildIndex(std::unique_ptr<OctTreeNode> const& _node, Aabb const& _ent);

	OCTREE_STRADDLING_TYPE m_StraddleMethod;
	int m_NumObjPerNode;
	std::unique_ptr<OctTreeNode> m_Root;
	entity m_OctreeEnt; // to hold all AABBs generated
};

#define OCTREE Octree::GetInstance() // macro for easy access
#endif /* OCTREE_HPP */