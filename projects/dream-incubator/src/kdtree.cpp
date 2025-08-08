/**
@file    kdtree.cpp
@author  weizhen.tan@digipen.edu
@date    23/07/2025

This file contains the definition of the KdTree class.

*//*__________________________________________________________________________*/

#include <kdtree.hpp>
#include <intersectiontests.hpp>
#include <components/material.hpp>
#include <random>
/*                                                                   includes
----------------------------------------------------------------------------- */

void KdTree::BuildKDTree(int _depth, std::unique_ptr<TreeNode> const& _node, std::vector<entity>& _ents)
{
	// init root bv
	if (_depth == 0)
	{
		_node->bv = std::make_shared<Aabb>(MergeAabbs(GetAABBs(_ents)));
		ECS.registry().get<BVList>(m_KdTreeEnt).push_back(_node->bv);
	}

	if (m_VisibilityFlags.size() <= _depth) { m_VisibilityFlags.push_back(std::vector<bool*>()); }
	m_VisibilityFlags[_depth].push_back(&_node->bv->isActive); _node->bv->isActive = true;
	_node->bv->depth = _depth;

	// termination based on number of objects per node
	if (_ents.size() <= m_NumObjPerNode)
	{
		_node->type = TreeNode::NODE_TYPE::LEAF;
		_node->entities = _ents;
		// color objects
		std::random_device rd; std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0, 1.f);// smtimes produces bad results - change to HSV?
		vec3 clr = /*clrs[dis(gen)];*/vec3(dis(gen), dis(gen), dis(gen));
		for (auto ent : _node->entities)
		{
			ECS.registry().get<Material>(ent).kAmbient
				= ECS.registry().get<Material>(ent).kDiffuse = clr;
		}
	}
	else // split
	{
		_node->type = TreeNode::NODE_TYPE::INTERNAL;

		// store split point in internal point
		std::vector<entity> left;
		std::vector<entity> right;
		float splitPoint = PartitionObjects(_ents, _depth % 3, left, _node->entities, right);

		_node->pLeft = std::make_unique<TreeNode>();
		_node->pRight = std::make_unique<TreeNode>();

		// split parent bv
		auto splitAABB = SplitAABB(dynamic_cast<Aabb&>(*_node->bv), _depth % 3, splitPoint);
		_node->pLeft->bv = std::make_shared<Aabb>(splitAABB.first);
		_node->pRight->bv = std::make_shared<Aabb>(splitAABB.second);
		ECS.registry().get<BVList>(m_KdTreeEnt).push_back(_node->pLeft->bv);
		ECS.registry().get<BVList>(m_KdTreeEnt).push_back(_node->pRight->bv);

		// create subtrees
		BuildKDTree(_depth + 1, _node->pLeft, left);
		BuildKDTree(_depth + 1, _node->pRight, right);
	}
}

float KdTree::PartitionObjects(std::vector<entity>& _ents, int _axis, std::vector<entity>& _left, std::vector<entity>& _split, std::vector<entity>& _right)
{
	// sort entities by split strategy
	std::sort(_ents.begin(), _ents.end(), [&_axis, &m_SplitStrategy = m_SplitStrategy](entity _ent, entity _ent1)
		{
			BVList& bvList = ECS.registry().get<BVList>(_ent);
			auto bv = std::dynamic_pointer_cast<Aabb>(*std::find_if(bvList.begin(), bvList.end(),
				[](std::shared_ptr<BoundingVolume> _bv) { return  _bv->type == AABB; }));
			BVList& bvList1 = ECS.registry().get<BVList>(_ent1);
			auto bv1 = std::dynamic_pointer_cast<Aabb>(*std::find_if(bvList1.begin(), bvList1.end(),
				[](std::shared_ptr<BoundingVolume> _bv) { return  _bv->type == AABB; }));

			switch (m_SplitStrategy)
			{
			case TOP_DOWN_BV_CENTER_MEDIAN: return bv->center[_axis] < bv1->center[_axis];
			case TOP_DOWN_BV_EXTENT_MEDIAN: return bv->GetMin()[_axis] < bv1->GetMin()[_axis];
			}
		});
	// check for coplanar points
	_split.push_back(_ents[_ents.size() / 2]); std::remove(_ents.begin(), _ents.end(), _split.back());
	BVList& bvList = ECS.registry().get<BVList>(_split.back());
	auto bv = std::dynamic_pointer_cast<Aabb>(*std::find_if(bvList.begin(), bvList.end(),
		[](std::shared_ptr<BoundingVolume> _bv) { return  _bv->type == AABB; }));
	vec3 splitPoint = m_SplitStrategy == TOP_DOWN_BV_CENTER_MEDIAN ? bv->center : bv->GetMin();
	for (auto ent : _ents)
	{
		BVList& bvList = ECS.registry().get<BVList>(ent);
		auto bv = std::dynamic_pointer_cast<Aabb>(*std::find_if(bvList.begin(), bvList.end(),
			[](std::shared_ptr<BoundingVolume> _bv) { return  _bv->type == AABB; }));

		switch (m_SplitStrategy)
		{
		case TOP_DOWN_BV_CENTER_MEDIAN: if (bv->center[_axis] == splitPoint[_axis])   { _split.push_back(ent); } break;
		case TOP_DOWN_BV_EXTENT_MEDIAN: if (bv->GetMin()[_axis] == splitPoint[_axis]) { _split.push_back(ent); } break;
		}
	}
	// remove points at internal nodes from entlist
	for (auto ent : _split)
	{ std::remove(_ents.begin(), _ents.end(), ent); }
	// split ents
	unsigned k = _ents.size() / 2;
	_left = std::vector<entity>(_ents.begin(), _ents.begin() + k);
	_right = std::vector<entity>(_ents.begin() + k, _ents.end());

	return splitPoint[_axis];
}