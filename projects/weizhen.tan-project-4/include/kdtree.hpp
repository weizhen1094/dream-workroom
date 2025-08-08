/**
@file    kdtree.hpp
@author  weizhen.tan@digipen.edu
@date    23/07/2025

This file contains the declaration of the KdTree class.

*//*__________________________________________________________________________*/

#ifndef KDTREE_HPP
#define KDTREE_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <vector>
#include <memory>
#include <isingleton.hpp>
#include <ecs.hpp>
#include <components/boundingvolume.hpp>
#include <bvhierarchy.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class KdTree
 * @brief This class is responsible for construction, storing and management of 
 * KdTree.
 */
class KdTree : public ISingleton<KdTree>
{

public:

	/**
	  * @brief builds kd tree top down recursively
	  * @param _depth - depth of the kd tree
	  * @param _node - node to split recursively until termination conditions are met
	  * @param _ents - entities in scene
	  */
	void BuildKDTree(int _depth, std::unique_ptr<TreeNode> const& _node, std::vector<entity>& _ents);

	BVH_STRATEGY_TYPE& GetSplitStrategy() { return m_SplitStrategy; };
	int& GetNumObjPerNode() { return m_NumObjPerNode; };
	entity GetKdTreeEnt() { return m_KdTreeEnt; };
	std::unique_ptr<TreeNode> const& GetRoot() { return m_Root; };
	std::vector<std::vector<bool*>>& GetVisibilityFlags() { return m_VisibilityFlags; };
	void ClearTree() { m_Root = std::make_unique<TreeNode>(); };

private:

	friend class ISingleton<KdTree>;

	KdTree() : m_SplitStrategy(TOP_DOWN_BV_CENTER_MEDIAN), m_NumObjPerNode(10), m_Root(std::make_unique<TreeNode>()),
		m_KdTreeEnt(ECS.CreateDefaultEntity("KdTree")) { ECS.registry().emplace<BVList>(m_KdTreeEnt); };
	~KdTree() {  };

	/**
	 * Helper functions
	 */

	/**
	  * @brief splits the list of entities passed in from BuildTopDown(), very similar to 
	  * BVHierarchy::PartitionObjects() except in kdtree there is implicit axis ordering (X -> Y -> Z)
	  * instead of choosing the largest spread axis
	  * @param _ents - list of entities to split
	  * @param _axis - passed in from BuildTopDown(), axis changes every split based on tree depth % 3
	  * @param _left - entities to be stored in left subtree
	  * @param _split - split entity/coplanar entities to be stored in internal node
	  * @param _right - entities to be stored in right subtree
	  */
	float PartitionObjects(std::vector<entity>& _ents, int _axis, std::vector<entity>& _left, std::vector<entity>& _split, std::vector<entity>& _right);


	BVH_STRATEGY_TYPE m_SplitStrategy;
	int m_NumObjPerNode;
	std::unique_ptr<TreeNode> m_Root;
	entity m_KdTreeEnt; // to hold all AABBs generated
	std::vector<std::vector<bool*>> m_VisibilityFlags; // isActive flag for bv grouped by lvl
};

#define KDTREE KdTree::GetInstance() // macro for easy access
#endif /* KDTREE_HPP */