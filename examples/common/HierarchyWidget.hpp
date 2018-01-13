#pragma once

#include <vector>
#include <set>
#include <FishGUI/Widget.hpp>

#include "TSelectionModel.hpp"
#include "ItemModel.hpp"

#include "TreeViewWidget.hpp"

struct GameObject
{
	std::string name;
	std::vector<GameObject*> children;
	
	GameObject(const std::string& name) : name(name) {}
};

typedef TTreeModel<GameObject, std::string> HierarchyModel;

template<>
inline GameObject* HierarchyModel::childAt(GameObject* parent, int row) const
{
	return parent->children[row];
}

template<>
inline int HierarchyModel::rowCount(GameObject* parent) const
{
	if (parent == nullptr)
		return 0;
	return parent->children.size();
}

template<>
inline bool HierarchyModel::hasChildren(GameObject* parent) const
{
	if (parent == nullptr)
		return false;
	return !parent->children.empty();
}

template<>
inline std::string HierarchyModel::data(GameObject* item) const
{
	return item->name;
}


//struct HierarchyModel : public AbstractTreeModel<GameObject, std::string>
//{
//public:
//
//	virtual GameObject* childAt(GameObject* parent, int row) const override
//	{
//		return parent->children[row];
//	}
//
//	virtual int rowCount(GameObject* parent) const override
//	{
//		if (parent == nullptr)
//			return 0;
//		return parent->children.size();
//	}
//
//	virtual bool hasChildren(GameObject* parent) const override
//	{
//		if (parent == nullptr)
//			return false;
//		return !parent->children.empty();
//	}
//
//	virtual std::string data(GameObject* item) const override
//	{
//		return item->name;
//	}
//};


class HierarchyWidget : public FishGUI::TreeWidget<GameObject>
{
public:
	typedef FishGUI::TreeWidget<GameObject> Super;
	
	HierarchyWidget(const char* name);
	
private:
};
