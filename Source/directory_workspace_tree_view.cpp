#include "directory_workspace_tree_view.h"

#include "framework.h"

#include "message_bus.h"

DirectoryWorkspaceTreeView::DirectoryWorkspaceTreeView(std::shared_ptr<MessageBus> message_bus, QFileSystemModel* model, QWidget* parent)
	:m_messageBus(message_bus),
	m_fileSystemModel(model),
	QTreeView(parent)
{
	InitUi();
	InitValue();
	InitConnect();
}

DirectoryWorkspaceTreeView::~DirectoryWorkspaceTreeView()
{
}

void DirectoryWorkspaceTreeView::SetMenu(const QMenu* menu)
{
}

QMenu* DirectoryWorkspaceTreeView::GetMenu() const
{
	return nullptr;
}

void DirectoryWorkspaceTreeView::contextMenuEvent(QContextMenuEvent* event)
{
	QModelIndex index = indexAt(event->pos());
	if (index.isValid())
	{
		// 在右键点击的当前位置弹出菜单
		m_itemMenu->exec(event->globalPos());
	}
}

void DirectoryWorkspaceTreeView::InitUi()
{
	m_itemMenu = new QMenu(this);
	m_openAction = m_itemMenu->addAction(tr("Open"));
	m_itemMenu->addSeparator();
	m_copyPathAction = m_itemMenu->addAction(tr("Copy File Path"));
	m_copyNameAction = m_itemMenu->addAction(tr("Copy File Name"));
	m_runBySystemAction = m_itemMenu->addAction(tr("Run By System"));
	m_itemMenu->addSeparator();
	m_explorerHereAction = m_itemMenu->addAction(tr("Explorer Here"));
	m_cmdHereAction = m_itemMenu->addAction(tr("Cmd Here"));
}

void DirectoryWorkspaceTreeView::InitValue()
{
}

void DirectoryWorkspaceTreeView::InitConnect()
{
	connect(m_openAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open File", QStringList() << m_fileSystemModel->filePath(currentIndex()));
		});
	connect(m_copyPathAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Copy Path", m_fileSystemModel->filePath(currentIndex()));
		});
	connect(m_copyNameAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Copy Name", m_fileSystemModel->fileName(currentIndex()));
		});
	connect(m_runBySystemAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open In Default Viewer", m_fileSystemModel->filePath(currentIndex()));
		});
	connect(m_explorerHereAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open Explorer", m_fileSystemModel->fileInfo(currentIndex()).absolutePath());
		});
	connect(m_cmdHereAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open Cmd", m_fileSystemModel->fileInfo(currentIndex()).absolutePath());
		});
}