#include "dir_workspace_dock.h"

#include "dir_workspace_tree_view.h"
#include "framework.h"
#include "message_bus.h"

DirWorkspaceDock::DirWorkspaceDock(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
    : m_messageBus(message_bus), QDockWidget(parent) {
    m_widget = new QWidget(this);
    m_toolBar = new QToolBar(m_widget);
    m_expandAllAction = m_toolBar->addAction(
        QIcon(":/OneNotePad/standard/panels/toolbar/fb_expand_all.ico"), tr("Expand All"));
    m_collapseAllAction = m_toolBar->addAction(
        QIcon(":/OneNotePad/standard/panels/toolbar/fb_fold_all.ico"), tr("Collapse All"));
    m_locationAction = m_toolBar->addAction(
        QIcon(":/OneNotePad/standard/panels/toolbar/fb_select_current_file.ico"),
        tr("Locate The Current File"));
    m_fileSystemModel = new QFileSystemModel(this);
    m_treeView = new DirWorkspaceTreeView(m_messageBus, m_fileSystemModel, this);

    InitUi();
    InitValue();
    InitConnect();
}

DirWorkspaceDock::~DirWorkspaceDock() {}

void DirWorkspaceDock::SetRootDir(const QString& root_dir) {
    m_fileSystemModel->setRootPath(root_dir);
    m_treeView->setRootIndex(m_fileSystemModel->index(root_dir));
}

QString DirWorkspaceDock::GetRootDir() const { return m_fileSystemModel->rootPath(); }

void DirWorkspaceDock::ExpandAll() { m_treeView->expandAll(); }

void DirWorkspaceDock::CollapseAll() { m_treeView->collapseAll(); }

void DirWorkspaceDock::LocationFile(const QString& file_path) {
    // 取消选择之前的
    QModelIndexList select_indexs = m_treeView->selectionModel()->selectedIndexes();
    for (const auto& index : select_indexs) {
        m_treeView->selectionModel()->select(index, QItemSelectionModel::Deselect);
    }
    // 选择指定的
    QModelIndex current_index = m_fileSystemModel->index(file_path);
    m_treeView->selectionModel()->select(current_index, QItemSelectionModel::Select);
    // 展开父节点
    while (current_index.isValid()) {
        m_treeView->expand(current_index);
        current_index = current_index.parent();
    }
}

void DirWorkspaceDock::InitUi() {
    // 文件视图
    m_treeView->setModel(m_fileSystemModel);

    QVBoxLayout* layout = new QVBoxLayout(m_widget);
    layout->addWidget(m_toolBar);
    layout->addWidget(m_treeView);
    m_widget->setLayout(layout);
    setWidget(m_widget);
}

void DirWorkspaceDock::InitValue() {}

void DirWorkspaceDock::InitConnect() {
    connect(m_expandAllAction, &QAction::triggered, [=]() { m_messageBus->Publish("Expand All"); });
    connect(m_collapseAllAction, &QAction::triggered,
            [=]() { m_messageBus->Publish("Collapse All"); });
    connect(m_locationAction, &QAction::triggered,
            [=]() { m_messageBus->Publish("Locate The Current File"); });
    connect(m_treeView, &QTreeView::doubleClicked, this, [=](const QModelIndex& index) {
        m_messageBus->Publish("Open File", m_fileSystemModel->filePath(index));
    });
}