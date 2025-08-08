/**
@file    bvhierarchy.cpp
@author  weizhen.tan@digipen.edu
@date    09/07/2025

This file contains the definition of the BVHierarchy class.

*//*__________________________________________________________________________*/

#include <bvhierarchy.hpp>
#include <algorithm>
#include <queue>
/*                                                                   includes
----------------------------------------------------------------------------- */

void BVHierarchy::BuildTopDown(int _depth, std::unique_ptr<TreeNode> const&  _node, std::vector<entity>& _ents)
{
	// compute parent bv
	ComputeParentBVTreeNode(_node, GetBVList(_ents));
	ECS.registry().get<BVList>(m_BVHTreeEnt).push_back(_node->bv);

	// set flags for bvh level visibility
	if (m_VisibilityFlags.size() <= _depth) { m_VisibilityFlags.push_back(std::vector<bool*>()); }
	m_VisibilityFlags[_depth].push_back(&_node->bv->isActive); _node->bv->isActive = true;
	_node->bv->depth = _depth;

	// termination
	// float combined = 0.5f * _ents.size() + 0.5f * _depth;
	if ((m_Config.exitFlags[MAX_NUM_OBJ].first && _ents.size() == m_Config.exitFlags[MAX_NUM_OBJ].second) ||
		(m_Config.exitFlags[MAX_TREE_HEIGHT].first && _depth == m_Config.exitFlags[MAX_TREE_HEIGHT].second))
	{
		_node->type = TreeNode::NODE_TYPE::LEAF;
		_node->entities = _ents;
	}
	else // split
	{
		_node->type = TreeNode::NODE_TYPE::INTERNAL;

		unsigned k = PartitionObjects(_ents, _depth%3); 

		_node->pLeft = std::make_unique<TreeNode>(); 
		_node->pRight = std::make_unique<TreeNode>();

		std::vector<entity> left(_ents.begin(), _ents.begin() + k);
		std::vector<entity> right(_ents.begin() + k, _ents.end());

		BuildTopDown(_depth + 1, _node->pLeft, left);
		BuildTopDown(_depth + 1, _node->pRight, right);
	}
}

unsigned BVHierarchy::PartitionObjects(std::vector<entity>& _ents, int _axis)
{
	// choose split plane - xyz with largest spread
	std::vector<vec3> ctrs;
	for (auto& ent : _ents)
	{
		BVList& bvList = ECS.registry().get<BVList>(ent);
		for (auto bv : bvList)
		{
			if (m_Config.type != bv->type) { continue; }
			switch (bv->type)
			{
			case BV_TYPE::AABB: ctrs.push_back(std::dynamic_pointer_cast<Aabb>(bv)->center); break;
			case BV_TYPE::BSPHERE_PCA: ctrs.push_back(std::dynamic_pointer_cast<Sphere>(bv)->center); break;
			}
		}
	}
	CARDINAL_AXES axis = GetAxisWithLargestSpread(ctrs);
	// sort entities by split strategy
	std::sort(_ents.begin(), _ents.end(), [&_axis, &axis, &m_Config = m_Config](entity _ent, entity _ent1)
		{
			BVList& bvList = ECS.registry().get<BVList>(_ent);
			auto bv = std::find_if(bvList.begin(), bvList.end(),
				[&m_Config](std::shared_ptr<BoundingVolume> _bv) { return  _bv->type == m_Config.type; });
			BVList& bvList1 = ECS.registry().get<BVList>(_ent1);
			auto bv1 = std::find_if(bvList1.begin(), bvList1.end(),
				[&m_Config](std::shared_ptr<BoundingVolume> _bv) { return  _bv->type == m_Config.type; });
			if (bv == bvList.end() || bv1 == bvList1.end()) { return false; }

			switch ((*bv)->type)
			{
			case BV_TYPE::AABB:
			{
				switch (m_Config.splitStrategy)
				{
				case TOP_DOWN_BV_CENTER_MEDIAN:
					return std::dynamic_pointer_cast<Aabb>(*bv)->center[axis] < std::dynamic_pointer_cast<Aabb>(*bv1)->center[axis];
				case TOP_DOWN_BV_EXTENT_MEDIAN:
					return std::dynamic_pointer_cast<Aabb>(*bv)->halfExtents[axis] < std::dynamic_pointer_cast<Aabb>(*bv1)->halfExtents[axis];
				case TOP_DOWN_K_EVEN_SPLIT_SINGLE_AXIS: // axis changes every level
					return std::dynamic_pointer_cast<Aabb>(*bv)->center[_axis] < std::dynamic_pointer_cast<Aabb>(*bv1)->center[_axis];
				}
			} break;
			case BV_TYPE::BSPHERE_PCA:
			{
				switch (m_Config.splitStrategy)
				{
				case TOP_DOWN_BV_CENTER_MEDIAN:
					return std::dynamic_pointer_cast<Sphere>(*bv)->center[axis] < std::dynamic_pointer_cast<Sphere>(*bv1)->center[axis];
				case TOP_DOWN_BV_EXTENT_MEDIAN:
					return std::dynamic_pointer_cast<Sphere>(*bv)->radius < std::dynamic_pointer_cast<Sphere>(*bv1)->radius;
				case TOP_DOWN_K_EVEN_SPLIT_SINGLE_AXIS: // axis changes every level
					return std::dynamic_pointer_cast<Sphere>(*bv)->center[_axis] < std::dynamic_pointer_cast<Sphere>(*bv1)->center[_axis];
				}
			}break;
			}
		});
	switch (m_Config.splitStrategy)
	{
	case TOP_DOWN_BV_EXTENT_MEDIAN: 
	case TOP_DOWN_BV_CENTER_MEDIAN: return _ents.size() / 2;
	case TOP_DOWN_K_EVEN_SPLIT_SINGLE_AXIS: 
	{
		unsigned k = _ents.size() / TOP_DOWN_K_EVEN;
		// divide by k and check cost to choose where to split
		float minCost = FLT_MAX; unsigned chosenK = k;
		for (unsigned i = 1; i < _ents.size(); i + k)
		{
			BVList left = GetBVList(std::vector<entity>(_ents.begin(), _ents.begin() + i));
			BVList right = GetBVList(std::vector<entity>(_ents.begin() + i, _ents.end()));
			float cost = GetSACost(left, right);
			if (minCost > cost) { minCost = cost; chosenK = i; }
		}
		return _ents.size() / chosenK;
	}	
	}
	return _ents.size();
}

float BVHierarchy::GetSACost(BVList const& _left, BVList const& _right)
{
	BVList parent = _left; parent.insert(parent.end(), _right.begin(), _right.end());
	float sumSA = GetSA(parent);
	return _left.size() * (GetSA(_left) / sumSA) + _right.size() * (GetSA(_right) / sumSA);
}

float BVHierarchy::GetSA(BVList const& _bv)
{
	switch (_bv[0]->type)
	{
	case BV_TYPE::AABB: 
	{
		std::vector<std::shared_ptr<Aabb>> tmp;
		for(auto bv : _bv) { tmp.push_back(std::dynamic_pointer_cast<Aabb>(bv)); }
		Aabb combined = Aabb(MergeAabbs(tmp));
		float w = combined.halfExtents.x * 2;
		float h = combined.halfExtents.y * 2;
		float d = combined.halfExtents.z * 2;
		return 2 * (w*h + w*d + h*d);
	} 
	case BV_TYPE::BSPHERE_PCA:
	{
		std::vector<std::shared_ptr<Sphere>> tmp;
		for (auto bv : _bv) { tmp.push_back(std::dynamic_pointer_cast<Sphere>(bv)); }
		Sphere combined = Sphere(MergeSpheres(tmp));
		combined.type = BSPHERE_PCA;
		return 4 * std::numbers::pi * combined.radius;
	} 
	}
}

void BVHierarchy::BuildBottomUp(std::vector<entity>& _ents)
{
	// init availList with bv of entities
	std::vector<std::unique_ptr<TreeNode>> availList; // nodes yet to merged into root
	for (auto& ent : _ents)
	{
		BVList& bvList = ECS.registry().get<BVList>(ent); 
		auto bv = std::find_if(bvList.begin(), bvList.end(), 
			[&m_Config = m_Config](std::shared_ptr<BoundingVolume> _bv) { return  _bv->type == m_Config.type; });
		if (bv == bvList.end()) { return; }
		availList.push_back(std::make_unique<TreeNode>()); 
		switch (m_Config.type)
		{
		case BV_TYPE::AABB: 
			availList.back()->bv 
				= std::make_shared<Aabb>(Aabb(*(std::dynamic_pointer_cast<Aabb>(*bv))));
			break;
		case BV_TYPE::BSPHERE_PCA:
			availList.back()->bv
				= std::make_shared<Sphere>(Sphere(*(std::dynamic_pointer_cast<Sphere>(*bv))));
			break;
		}
		availList.back()->entities = _ents;
		availList.back()->type = TreeNode::NODE_TYPE::LEAF;
		ECS.registry().get<BVList>(m_BVHTreeEnt).push_back(*bv);
	} // TODO: change to priority q

	while (availList.size() > 1)
	{
		std::unique_ptr<TreeNode> first{ std::make_unique<TreeNode>() }, second{ std::make_unique<TreeNode>() }; 
		// send list in and get pair of closest bv to merge
		std::pair<unsigned, unsigned> idx = FindNodesToMerge(availList); // find closest pair of bvs
		if (idx.first > idx.second) { std::swap(idx.first, idx.second); }
		// remove from unmerged nodes list
		first.swap(availList[idx.first]); second.swap(availList[idx.second]);
		auto it = availList.begin() + idx.first;
		if (it != availList.end()) { availList.erase(it); }
		it = availList.begin() + idx.second-1;
		if (it != availList.end()) { availList.erase(it); }
		// init parent bv with pairs as children
		availList.push_back(std::make_unique<TreeNode>());
		std::unique_ptr<TreeNode> const& parent = availList.back();
		parent->pLeft.swap(first); parent->pRight.swap(second);
		// compute bv
		ComputeParentBVTreeNode(parent, { parent->pLeft->bv, parent->pRight->bv });
		parent->type = TreeNode::NODE_TYPE::INTERNAL;
	}
	m_Root.swap(availList[0]);
	// traverse by level to add bvh to ecs and set visibility flags
	std::queue<TreeNode*> queue;
	queue.push(m_Root.get()); // not ideal
	int depth = 0;
	while (!queue.empty())
	{
		unsigned qSize = queue.size();
		std::vector<bool*> lvFlags;
		for (unsigned i = 0; i < qSize; ++i)
		{
			TreeNode* node = queue.front(); queue.pop();

			ECS.registry().get<BVList>(m_BVHTreeEnt).push_back(node->bv);
			lvFlags.push_back(&node->bv->isActive);
			node->bv->isActive = true;
			node->bv->depth = depth;
			if (node->pLeft != nullptr) { queue.push(node->pLeft.get()); }
			if (node->pRight != nullptr) { queue.push(node->pRight.get()); }
		}
		m_VisibilityFlags.push_back(lvFlags);
		++depth;
	} 
}

std::pair<unsigned, unsigned> BVHierarchy::FindNodesToMerge(std::vector<std::unique_ptr<TreeNode>>& _availList)
{
	std::pair<unsigned, unsigned> ret;
	// for each pair compute total cost for each heuristic and choose pair with min cost
	float distCost = m_Config.mergeHeuristics[BTM_UP_NEAREST_NEIGHBOUR] ? FLT_MAX : 0; 
	float volCost  = m_Config.mergeHeuristics[BTM_UP_MIN_CHILD_VOL] ? FLT_MAX : 0;
	float saCost   = m_Config.mergeHeuristics[BTM_UP_MIN_CHILD_SA] ? FLT_MAX : 0;

	float minCost = FLT_MAX;

	for (unsigned i = 0; i < _availList.size(); ++i)
	{
		for (unsigned j = i + 1; j < _availList.size(); ++j)
		{
			if (m_Config.mergeHeuristics[BTM_UP_NEAREST_NEIGHBOUR])
			{
				distCost = GetDistanceCost(_availList[i]->bv, _availList[j]->bv);
			}
			if (m_Config.mergeHeuristics[BTM_UP_MIN_CHILD_VOL])
			{
				volCost = GetVolumeCost(_availList[i]->bv, _availList[j]->bv);
			}
			if (m_Config.mergeHeuristics[BTM_UP_MIN_CHILD_SA])
			{
				saCost = GetSACost({ _availList[i]->bv }, { _availList[j]->bv });
			}
			float cost = distCost * 0.2f + volCost * 0.4f + saCost * 0.4f;
			if (minCost > cost)
			{
				minCost = cost; ret.first = i;  ret.second = j;
			}
		}
	} return ret;
}

float BVHierarchy::GetVolumeCost
(std::shared_ptr<BoundingVolume> const& _left, std::shared_ptr<BoundingVolume> const& _right)
{
	if (_left == nullptr || _right == nullptr) { return 0; }
	switch (_left->type)
	{
	case BV_TYPE::AABB:
	{
		std::vector<std::shared_ptr<Aabb>> tmp;
		tmp.push_back(std::dynamic_pointer_cast<Aabb>(_left));
		tmp.push_back(std::dynamic_pointer_cast<Aabb>(_right));
		Aabb combined = Aabb(MergeAabbs(tmp));
		return 2 * (combined.halfExtents.x * combined.halfExtents.y * combined.halfExtents.z);
	}
	case BV_TYPE::BSPHERE_PCA:
	{
		std::vector<std::shared_ptr<Sphere>> tmp;
		tmp.push_back(std::dynamic_pointer_cast<Sphere>(_left));
		tmp.push_back(std::dynamic_pointer_cast<Sphere>(_right));
		Sphere combined = Sphere(MergeSpheres(tmp));
		combined.type = BSPHERE_PCA;
		return (4/3) * std::numbers::pi * combined.radius * combined.radius * combined.radius;
	}
	}
}

float BVHierarchy::GetDistanceCost
(std::shared_ptr<BoundingVolume> const& _left, std::shared_ptr<BoundingVolume> const& _right)
{
	if (_left == nullptr || _right == nullptr) { return 0; }
	switch (_left->type)
	{
	case BV_TYPE::AABB:
	{
		/*vec3 u = std::dynamic_pointer_cast<Aabb>(_left)->GetMin() - std::dynamic_pointer_cast<Aabb>(_right)->GetMax();
		vec3 v = std::dynamic_pointer_cast<Aabb>(_right)->GetMin() - std::dynamic_pointer_cast<Aabb>(_left)->GetMax();
		for (int i = 0; i < 3; ++i) {  u[i] = fmax(0, u[i]); v[i] = fmax(0, v[i]); }
		return dot(u, u) + dot(v, v);*/
		vec3 d = std::dynamic_pointer_cast<Aabb>(_left)->center - std::dynamic_pointer_cast<Aabb>(_right)->center;
		return dot(d, d);
	}
	case BV_TYPE::BSPHERE_PCA:
	{
		vec3 d = std::dynamic_pointer_cast<Sphere>(_left)->center - std::dynamic_pointer_cast<Sphere>(_right)->center;
		/*float radius = std::dynamic_pointer_cast<Sphere>(_left)->radius + std::dynamic_pointer_cast<Sphere>(_right)->radius;
		return dot(d, d) - radius * radius;*/
		return dot(d, d);
	}
	}
}

unsigned BVHierarchy::GetTreeHeight(std::unique_ptr<TreeNode> const& _root)
{
	if (_root == nullptr) { return 0; }
	unsigned l = GetTreeHeight(_root->pLeft);
	unsigned r = GetTreeHeight(_root->pRight);
	return max(l, r) + 1;
}

bool BVHierarchy::IsTreeBalanced(std::unique_ptr<TreeNode> const& _root)
{
	if (_root == nullptr) { return false; }

	unsigned l = GetTreeHeight(_root->pLeft);
	unsigned r = GetTreeHeight(_root->pRight);

	if (abs(l - r) > 1) { return false; }

	return IsTreeBalanced(_root->pLeft) && IsTreeBalanced(_root->pRight);
}

void BVHierarchy::ClearBVHData()
{
	ECS.registry().get<BVList>(m_BVHTreeEnt).clear();
	for (auto lv : m_VisibilityFlags) { lv.clear(); } m_VisibilityFlags.clear();
}

// attemp to clean up code using helpher functions..
void BVHierarchy::ComputeParentBVTreeNode(std::unique_ptr<TreeNode> const& _node, BVList const& _bv)
{
	switch (m_Config.type)
	{
		case AABB:
		{
			std::vector<std::shared_ptr<Aabb>> tmp; for (auto bv : _bv) { tmp.push_back(std::dynamic_pointer_cast<Aabb>(bv)); }
			_node->bv = std::make_shared<Aabb>(Aabb(MergeAabbs(tmp)));
		}
		break;
		case BSPHERE_PCA:
		{
			std::vector<std::shared_ptr<Sphere>> tmp; for (auto bv : _bv) { tmp.push_back(std::dynamic_pointer_cast<Sphere>(bv)); }
			_node->bv = std::make_shared<Sphere>(Sphere(MergeSpheres(tmp)));
			_node->bv->type = BSPHERE_PCA;
		}
		break;
		case OBB_PCA:	// TODO: bvh for OBB
			break;
	}
}

BVList BVHierarchy::GetBVList(std::vector<entity>const& _ents)
{
	BVList ret; 
	for (auto ent : _ents)
	{
		BVList& bvList = ECS.registry().get<BVList>(ent);
		auto bv = std::find_if(bvList.begin(), bvList.end(),
			[&m_Config = m_Config](std::shared_ptr<BoundingVolume> _bv) { return  _bv->type == m_Config.type; });
		if (bv == bvList.end()) { continue; }
		ret.push_back(*bv);
	}
	return ret;
}