#include <base/memoryallocator/taggednew.h>

#include <native/variantmaptreemodel.h>
#include <native/treeitem.h>
#include <native/treemodel.h>

#include "treeitem.h"

#include <cassert>
#include <iostream>

#include <QtCore/QStringList>
#include <QtCore/QVariantMap>

namespace ngs {

VariantMapTreeModel::VariantMapTreeModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent),
      _root_item(NULL),
      _path_size(0)
{
    QList<QVariant> rootData;
    rootData << "Title" << "Summary";
    _root_item = new_ff TreeItem(rootData);
    build_tree_items(data.split(QString("\n")), _root_item);
}

VariantMapTreeModel::VariantMapTreeModel(const QVariant& value, QObject *parent)
: QAbstractItemModel(parent),
  _root_item(NULL),
  _path_size(0)
{
  init(value);
}

VariantMapTreeModel::~VariantMapTreeModel()
{
    delete_ff(_root_item);
}

void VariantMapTreeModel::init(const QVariant& value) {
  _root_variant = value;
  beginResetModel();
  {
    delete_ff(_root_item);
    QList<QVariant> rootData;
    rootData << "Title" << "Summary";
    _root_item = new_ff TreeItem(rootData);
    build_tree_items_2("", value, _root_item);
  }
  endResetModel();
}

void VariantMapTreeModel::reset_root() {
  beginResetModel();
  {
    // Destroy the current root item.
    delete_ff(_root_item);

    // Create the novel root item.
    QList<QVariant> rootData;
    rootData << "Name" << "Value";
    _root_item = new_ff TreeItem(rootData);

    // Find the next child which is to become the next root.
    QVariant fake_root = _root_variant;
    for (size_t i=0; i<_path_size; ++i) {
      if (fake_root.canConvert(QMetaType::QVariantMap)) {
        QVariantMap map = fake_root.toMap();
        fake_root = map.find(_path[i]).value();
      } else if (fake_root.canConvert(QMetaType::QVariantList)) {
        QVariantList list = fake_root.toList();
        fake_root = list.at(_path[i].toInt());
      } else {
        assert(false);
      }
    }

    // Build the tree items.
    build_tree_items_2("", fake_root, _root_item);
  }
  endResetModel();
}

void VariantMapTreeModel::push_root(int child_root) {
  QModelIndex child_index = index(child_root,0);
  QVariant child_data = child_index.data(Qt::DisplayRole);
  QString child_name = child_data.toList()[0].toString();

  // When pushing the next child, the popped elements
  // at the tail end of the path vector get lost.
  if (_path_size != _path.size()) {
    _path.resize(_path_size);
  }

  // Push the next child element onto the path.
  _path.push_back(child_name);
  _path_size = _path.size();

  // Reset the root to the next child.
  reset_root();
}

void VariantMapTreeModel::pop_root() {
  if (_path_size>0) {
    --_path_size;
  }
  reset_root();
}

void VariantMapTreeModel::unpop_root() {
  if (_path.size() > _path_size) {
    ++_path_size;
  }
  reset_root();
}

int VariantMapTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->get_column_count();
    else
        return _root_item->get_column_count();
}

QVariant VariantMapTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if (index.column()==0) {
      QVariantList list;
      list.reserve(2);
      list.append(item->get_data(0));
      list.append(item->get_data(1));
      return list;
    }
    return item->get_data(index.column());
}

Qt::ItemFlags VariantMapTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant VariantMapTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _root_item->get_data(section);

    return QVariant();
}

QModelIndex VariantMapTreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = _root_item;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->get_child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex VariantMapTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->get_parent_item();

    if (parentItem == _root_item)
        return QModelIndex();

    return createIndex(parentItem->get_row(), 0, parentItem);
}

int VariantMapTreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = _root_item;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->get_child_count();
}

void VariantMapTreeModel::build_tree_items(const QStringList &lines, TreeItem *parent)
{
    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].mid(position, 1) != " ")
                break;
            position++;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                // The last child of the current parent is now the new_ff parent
                // unless the current parent has no children.

                if (parents.last()->get_child_count() > 0) {
                    parents << parents.last()->get_child(parents.last()->get_child_count()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append the next item to the current parent's list of children.
            parents.last()->append_child(new_ff TreeItem(columnData, parents.last()));
        }

        ++number;
    }
}

void VariantMapTreeModel::build_tree_items_2(const QString& name, const QVariant& value, TreeItem *parent) {

  if (value.canConvert(QMetaType::QVariantMap) || value.canConvert(QMetaType::QVariantList)) {
    // Create the next tree child item representing this map or list, if it has a name.
    TreeItem* child = parent;
    if (!name.isEmpty()) {
      QList<QVariant> column_data;
      column_data << name;
      column_data << QVariant();
      child = new_ff TreeItem(column_data,parent);
      parent->append_child(child);
    }

    // Recurse over the children in maps.
    if (value.canConvert(QMetaType::QVariantMap)) {
      QVariantMap map = value.toMap();
      QVariantMap::iterator iter;
      for (iter = map.begin(); iter != map.end(); ++iter) {
        build_tree_items_2(iter.key(),iter.value(),child);
      }
    }

    // Otherwise recurse over the children in lists.
    else if (value.canConvert(QMetaType::QVariantList)) {
      QVariantList list = value.toList();
      for (int i=0; i<list.size(); ++i) {
        QString name = QString::number(i);
        build_tree_items_2(name,list[i],child);
      }
    }
  } else {
    QList<QVariant> column_data;
    column_data << name;
    column_data << value;
    TreeItem* child = new_ff TreeItem(column_data, parent);
    parent->append_child(child);
  }
}

}
