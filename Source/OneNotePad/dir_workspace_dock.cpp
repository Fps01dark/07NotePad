#include "dir_workspace_dock.h"

#include <QToolBar>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QFileSystemModel>

#include "dir_workspace_tree_view.h"
#include "message_bus.h"

DirWorkspaceDock::DirWorkspaceDock(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	: m_messageBus(message_bus), QDockWidget(parent)
{
	m_fileSystemModel = new QFileSystemModel(this);
	m_treeView = new DirWorkspaceTreeView(m_messageBus, m_fileSystemModel, this);

	InitUi();
	InitValue();
	InitConnect();
}

DirWorkspaceDock::~DirWorkspaceDock() 
{
}

void DirWorkspaceDock::SetRootDir(const QString& root_dir)
{
	m_fileSystemModel->setRootPath(root_dir);
	m_treeView->setRootIndex(m_fileSystemModel->index(root_dir));
}

QString DirWorkspaceDock::GetRootDir() const 
{ 
	return m_fileSystemModel->rootPath(); 
}

void DirWorkspaceDock::InitUi()
{
	// 文件视图
	m_treeView->setModel(m_fileSystemModel);
	m_treeView->header()->hide();
	m_treeView->header()->hideSection(1);
	m_treeView->header()->hideSection(2);
	m_treeView->header()->hideSection(3);

	setWidget(m_treeView);
}

void DirWorkspaceDock::InitValue()
{
}

void DirWorkspaceDock::InitConnect()
{
	connect(m_treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex& index)
		{
			m_messageBus->Publish("Open File", m_fileSystemModel->filePath(index));
		});
}