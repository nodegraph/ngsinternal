#include <native/treeitem.h>
#include <native/treeitem.h>
#include <QtCore/QStringList>

#include <base/memoryallocator/taggednew.h>

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
{
    _parent_item = parent;
    _item_data = data;
}

TreeItem::~TreeItem()
{
  for (int i=0; i<_child_items.size(); ++i) {
    delete_ff(_child_items[i]);
  }
  _child_items.clear();
}

void TreeItem::append_child(TreeItem *item)
{
    _child_items.append(item);
}

TreeItem *TreeItem::get_child(int row)
{
    return _child_items.value(row);
}

int TreeItem::get_child_count() const
{
    return _child_items.count();
}

int TreeItem::get_column_count() const
{
    return _item_data.count();
}

QVariant TreeItem::get_data(int column) const
{
    return _item_data.value(column);
}

TreeItem *TreeItem::get_parent_item()
{
    return _parent_item;
}

int TreeItem::get_row() const
{
    if (_parent_item)
        return _parent_item->_child_items.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
//! [8]
