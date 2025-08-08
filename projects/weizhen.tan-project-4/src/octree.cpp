/**
@file    octree.cpp
@author  weizhen.tan@digipen.edu
@date    22/07/2025

This file contains the definition of the Octree class.

*//*__________________________________________________________________________*/

#include <octree.hpp>
#include <intersectiontests.hpp>
#include <components/material.hpp>
#include <random>
/*                                                                   includes
----------------------------------------------------------------------------- */

void Octree::BuildOctree(std::vector<entity> const& _ents)
{
	// init root node
	Aabb bv = MergeAabbs(GetAABBs(_ents)); bv.depth = 0;
	m_Root->center = bv.center; 
	m_Root->halfExtent = max(max(bv.halfExtents.x, bv.halfExtents.y), bv.halfExtents.z);
	bv.halfExtents = vec3(m_Root->halfExtent);
	m_Root->entities = _ents; 
	m_Root->bv = std::make_shared<Aabb>(bv);
	ECS.registry().get<BVList>(m_OctreeEnt).push_back(m_Root->bv);

	// build octree recursively
	BuildOctree(0, m_Root, _ents);
}

void Octree::BuildOctree(int _depth, std::unique_ptr<OctTreeNode> const& _node, std::vector<entity> const& _ents)
{
	// add ents that are inside this cell and handle straddling
	if (_depth > 0)  {  InsertIntoOctree(_node, _ents);  }

	// if doesnt contain geometry dont render cell
	if (_node->entities.empty()) { _node->bv->isActive = false; }

	// create subtrees only if m_NumObjPerNode objects exist within parent node
	if (_node->entities.size() > m_NumObjPerNode)
	{
		vec3 offset; float step = _node->halfExtent * 0.5f;
		for (unsigned i = 0; i < 8; ++i)
		{
			offset.x = i & 1 ? step : -step;
			offset.y = i & 2 ? step : -step;
			offset.z = i & 4 ? step : -step;

			_node->pChildren[i] = std::make_unique<OctTreeNode>();
			_node->pChildren[i]->center = _node->center + offset;  _node->pChildren[i]->halfExtent = step;

			Aabb bv; 
			bv.center = _node->pChildren[i]->center; 
			bv.halfExtents = vec3(_node->pChildren[i]->halfExtent);
			bv.depth = _depth + 1;
			_node->pChildren[i]->bv = std::make_shared<Aabb>(bv);
			ECS.registry().get<BVList>(m_OctreeEnt).push_back(_node->pChildren[i]->bv);

			BuildOctree(_depth + 1, _node->pChildren[i], _node->entities);
		}
	}
}

void Octree::InsertIntoOctree(std::unique_ptr<OctTreeNode> const& _node, std::vector<entity> const& _ents)
{
	// get aabbs of ents in parent cell
	std::vector<std::shared_ptr<Aabb>> aabbs = GetAABBs(_ents);
	unsigned i = 0;
	for (auto aabb : aabbs)
	{
		// check if obj is inside or straddling
		switch (ClassifyEntityCell(_node, *aabb))
		{
		case OUTSIDE: break;
		case INSIDE: _node->entities.push_back(_ents[i]); break;
		case OVERLAPPING:
			switch (m_StraddleMethod)
			{
			case OBJ_CENTER:
				if(OverlapPointAabb(aabb->center, _node->GetMin(), _node->GetMax()))
				{ _node->entities.push_back(_ents[i]); }
				break;
			case ALL_OVERLAPPING_CELLS: _node->entities.push_back(_ents[i]); break;
			case CURRENT_LV_CELL: break;
			case SPLIT_OBJ:		  break;
			}
			break;
		} ++i;

	}
	std::random_device rd; std::mt19937 gen(rd());  
	//auto clrs = GetColors();
	//std::uniform_int_distribution<> dis(0, clrs.size()-1);
	std::uniform_real_distribution<> dis(0, 1.f);// smtimes produces bad results - change to HSV?
	vec3 clr = /*clrs[dis(gen)];*/vec3(dis(gen), dis(gen), dis(gen));
	for (auto ent : _node->entities)
	{
		ECS.registry().get<Material>(ent).kAmbient =
		ECS.registry().get<Material>(ent).kDiffuse = clr;
	}
}

std::tuple<int, bool> Octree::GetChildIndex(std::unique_ptr<OctTreeNode> const& _node, Aabb const& _ent)
{
	bool bStraddle = false;
	int index = 0, flag = 1;
	for(unsigned axis = 0; axis < 3; ++axis)
	{
		float d = (_ent.center[axis] - _node->center[axis]);
		// Check if d is within bounds of the BV
		if (abs(d) <= _ent.halfExtents[axis])
		{ bStraddle = true; break; }
		// which of + or - value for the bit?
		if (d > 0) { index |= (1 << axis); }
	}
	return { index, bStraddle };
}

SIDE_RESULT Octree::ClassifyEntityCell(std::unique_ptr<OctTreeNode> const& _node, Aabb const& _ent)
{
	if (!OverlapAabbAabb(_node->GetMin(), _node->GetMax(), _ent.GetMin(), _ent.GetMax()))
	{ return OUTSIDE; }
	else if (OverlapPointAabb(_ent.GetMin(), _node->GetMin(), _node->GetMax()) &&
			OverlapPointAabb(_ent.GetMax(), _node->GetMin(), _node->GetMax()))
	{ return INSIDE; }
	return OVERLAPPING;
}