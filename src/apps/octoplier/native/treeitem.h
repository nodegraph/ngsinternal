#pragma once

#include <QtCore/QList>
#include <QtCore/QVariant>

class TreeItem
{
public:
    explicit TreeItem(const QList<QVariant> &data, TreeItem *parentItem = 0);
    ~TreeItem();

    void append_child(TreeItem *child);

    TreeItem* get_child(int row);
    int get_child_count() const;
    int get_column_count() const;
    QVariant get_data(int column) const;
    int get_row() const;
    TreeItem *get_parent_item();

private:
    QList<TreeItem*> _child_items;
    QList<QVariant> _item_data;
    TreeItem *_parent_item;
};


