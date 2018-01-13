#pragma once

#include <set>

struct FontIcon
{
//	int codePoint;	// code point in .ttf
	std::string fontText;
	int fontSize;
	const char* fontFace;
};

template<class ItemType, class DataType>
class TTreeModel
{
public:
	
//	T* parent(T* child) const = 0;
	ItemType*		childAt(ItemType* parent, int row) const;
	int 			rowCount(ItemType* parent) const;
	bool 			hasChildren(ItemType* parent) const;
	DataType		data(ItemType* item) const;
	const FontIcon*	icon(ItemType* item) const { return nullptr; }
//	bool		setData(ItemType* item, const DataType& data);
	
	void SetRootItem(ItemType* root)
	{
		m_root = root;
	}
	
protected:
	ItemType* m_root;
};

/*
template<class ItemType, class DataType>
class AbstractTreeModel : public AbstractItemModel<ItemType, DataType>
{
public:
//	bool IsItemFolded(ItemType* item) const
//	{
//		auto it = m_unfoldedItems.find(item);
//		return it == m_unfoldedItems.end();
//	}
	
//	bool FoldItem(ItemType* item)
//	{
//		m_unfoldedItems.erase(item);
//	}
//
//	bool UnfoldItem(ItemType* item)
//	{
//		m_unfoldedItems.insert(item);
//	}
	
protected:
//	std::set<ItemType*> m_unfoldedItems;
};
*/

template<class ItemType, class DataType>
class TListModel
{
public:
	ItemType*	childAt(ItemType* parent, int row) const;
	int		 	rowCount(ItemType* parent) const;
	DataType	data(ItemType* item) const;
//	virtual bool hasChildren(ItemType* parent) const override
//	{
//		if (this->m_root == parent)
//			return true;
//		return false;
//	}
};

