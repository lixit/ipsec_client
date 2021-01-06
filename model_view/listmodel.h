#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>

//demonstrate model/view
//    QSplitter *splitter = new QSplitter(this);

//    QFileSystemModel *model = new QFileSystemModel;
//    model->setRootPath(QDir::currentPath());


//    QTreeView *tree = new QTreeView(splitter);
//    tree->setModel(model);
//    tree->setRootIndex(model->index(QDir::currentPath()));

//    QListView *list = new QListView(splitter);
//    list->setModel(model);
//    list->setRootIndex(model->index(QDir::currentPath()));

//    splitter->setWindowTitle("Two views onto the same file system model");

//    setCentralWidget(splitter);

class ListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ListModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
};

#endif // LISTMODEL_H
