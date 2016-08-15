#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>

class TreeItem;

namespace ngs {

class VariantMapTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit VariantMapTreeModel(const QString &data, QObject *parent = 0);
    explicit VariantMapTreeModel(const QVariant& data, QObject *parent = 0);
    ~VariantMapTreeModel();

    void init(const QVariant& value);
    void push_root(int child_row);
    void pop_root();
    void unpop_root();

    // Qt overrides.
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

public slots:



private:
    void reset_root();
    void build_tree_items(const QStringList &lines, TreeItem *parent);
    void build_tree_items_2(const QString& name, const QVariant& value, TreeItem *parent);

    TreeItem *_root_item;
    QVariant _root_variant;

    std::vector<QString> _path; // When popping elements they are left in the list so that we can unpop.
    size_t _path_size; // This integer indexes one past the actual index of the last path element.
};

}
