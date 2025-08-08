/**
@file    bvhierarchy.hpp
@author  weizhen.tan@digipen.edu
@date    09/07/2025

This file contains the declaration of the BVHierarchy class and related enums and
struct BvhConfig and TreeNode.

*//*__________________________________________________________________________*/

#ifndef BVHIERARCHY_HPP
#define BVHIERARCHY_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <vector>
#include <memory>
#include <isingleton.hpp>
#include <ecs.hpp>
#include <components/boundingvolume.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

enum BVH_STRATEGY_TYPE
{
	TOP_DOWN_BV_CENTER_MEDIAN,
	TOP_DOWN_BV_EXTENT_MEDIAN,
	TOP_DOWN_K_EVEN_SPLIT_SINGLE_AXIS,
	BVH_STRATEGY_TYPE_TOTAL,
};
constexpr unsigned TOP_DOWN_K_EVEN = 2;

enum BVH_EXIT_CONDITION
{
	MAX_NUM_OBJ,
	MAX_TREE_HEIGHT,
	BVH_EXIT_CONDITION_TOTAL,
};

enum BVH_MERGE_HEURISTIC
{
	BTM_UP_NEAREST_NEIGHBOUR,
	BTM_UP_MIN_CHILD_VOL,
	BTM_UP_MIN_CHILD_SA,
	BVH_MERGE_HEURISTIC_TOTAL,
};

/**
 * @struct TreeNode
 * @brief This struct holds the data for bvh settings
 */
struct BvhConfig
{
	BV_TYPE type					{AABB};
	bool isTopDown{ true };

	BVH_STRATEGY_TYPE splitStrategy { TOP_DOWN_BV_CENTER_MEDIAN };
	std::pair<bool, unsigned> exitFlags[BVH_EXIT_CONDITION_TOTAL]{ {true,1},{false,2} };

	bool mergeHeuristics[BVH_MERGE_HEURISTIC_TOTAL]{ true,true,true };
};

/**
 * @struct TreeNode
 * @brief This struct holds the data for tree node
 */
struct TreeNode
{
	enum NODE_TYPE { INTERNAL, LEAF };
	NODE_TYPE type;
	std::shared_ptr<BoundingVolume> boundingVolume;
	std::vector<entity> entities;
	std::unique_ptr<TreeNode> pLeft;
	std::unique_ptr<TreeNode> pRight;
};

/**
 * @class BVHTree
 * @brief This class is responsible for construction, storing and management of 
    Bounding Volume Hierarchy Tree.
	Since currently only 1 scene can be loaded at a single time, and a scene 
	contains only 1 bvh tree, this class is made into a singleton...
 */
class BVHierarchy : public ISingleton<BVHierarchy>
{

public:

	/**
	  * @brief builds bvh top down recursively
	  * @param _depth - depth of the bvh tree
	  * @param _node - node to split recursively until termination conditions are met
	  * @param _ents - list of entities in node passed in
	  */
	void BuildTopDown(int _depth, std::unique_ptr<TreeNode> const& _node, std::vector<entity>& _ents);
	/**
	  * @brief builds bvh from bottom up
	  * @param _ents - list of entities in scene
	  */
	void BuildBottomUp(std::vector<entity>& _ents);

	void ClearBVHData();
	bool IsTreeBalanced() { return IsTreeBalanced(m_Root); };

	BvhConfig& GetConfig() { return m_Config; };
	std::vector<std::vector<bool*>>& GetVisibilityFlags() { return m_VisibilityFlags; };
	std::unique_ptr<TreeNode> const& GetRoot() { return m_Root; };
	static std::vector<vec3>& GetBVHLvColors() { return s_BVHLvColors; };

private:

	friend class ISingleton<BVHierarchy>;

	BVHierarchy() : 
		m_Root(std::make_unique<TreeNode>()), 
		m_BVHTreeEnt(ECS.CreateDefaultEntity("BVHTree")) { ECS.registry().emplace<BVList>(m_BVHTreeEnt); };
	~BVHierarchy() {  };

	/**
	 * Helper functions
	 */

	 /**
	  * @brief splits the list of entities passed in from BuildTopDown()
	  * @param _ents - list of entities to split
	  * @param _axis - passed in from BuildTopDown(), axis changes every split based on tree depth % 3
	  */
	unsigned PartitionObjects(std::vector<entity>& _ents, int _axis);

	/**
	  * @brief surface area cost 
	  * @param _left - left node
	  * @param _right - right node
	  */
	float GetSACost(BVList const& _left, BVList const& _right);
	/**
	  * @brief surface area of merged bounding volumes
	  */
	float GetSA(BVList const& _bv);

	/**
	  * @brief find closest pair of nodes in list, called from BuildBottomUp()
	  * @param _availList - list of nodes yet to be merged to root
	  * @return indexes of chosen pair of nodes in _availList
	  */
	std::pair<unsigned,unsigned> FindNodesToMerge(std::vector<std::unique_ptr<TreeNode>>& _availList);

	/**
	  * @brief volume of merged bounding volumes
	  * @param _left - left node
	  * @param _right - right node
	  */
	float GetVolumeCost(std::shared_ptr<BoundingVolume> const& _left, std::shared_ptr<BoundingVolume> const& _right);
	/**
	  * @brief distance between bounding volumes
	  * @param _left - left node
	  * @param _right - right node
	  */
	float GetDistanceCost(std::shared_ptr<BoundingVolume> const& _left, std::shared_ptr<BoundingVolume> const& _right);

	void ComputeParentBVTreeNode(std::unique_ptr<TreeNode> const& _node, BVList const& _bv);
	BVList GetBVList(std::vector<entity>const& _ents);
	unsigned GetTreeHeight(std::unique_ptr<TreeNode> const& _root);
	bool IsTreeBalanced(std::unique_ptr<TreeNode> const& _root);

	std::vector<std::vector<bool*>> m_VisibilityFlags; // isActive flag for bv grouped by lvl
	std::unique_ptr<TreeNode> m_Root;
	entity m_BVHTreeEnt; // to hold all bvs generated
	BvhConfig m_Config;

	static std::vector<vec3> s_BVHLvColors;

};

#define BVH BVHierarchy::GetInstance() // macro for easy access
#endif /* BVHIERARCHY_HPP */