#include "folder_workspace_dock_widget.h"

#include "framework.h"

#include "message_bus.h"

FolderWorkspaceDockWidget::FolderWorkspaceDockWidget(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	:m_messageBus(message_bus),
	QDockWidget(parent)
{
	m_widget = new QWidget(this);
	m_toolBar = new QToolBar(m_widget);
	m_expandAllAction = m_toolBar->addAction(QIcon(":/NotePad/standard/panels/toolbar/fb_expand_all.ico"), tr("Expand All"));
	m_collapseAllAction = m_toolBar->addAction(QIcon(":/NotePad/standard/panels/toolbar/fb_fold_all.ico"), tr("Collapse All"));
	m_locationAction = m_toolBar->addAction(QIcon(":/NotePad/standard/panels/toolbar/fb_select_current_file.ico"), tr("Locate The Current File"));
	m_fileSystemModel = new QFileSystemModel(this);
	m_treeView = new QTreeView(this);

	InitUi();
	InitValue();
	InitConnect();
}

FolderWorkspaceDockWidget::~FolderWorkspaceDockWidget()
{
}

void FolderWorkspaceDockWidget::SetRootDir(const QString& root_dir)
{
	m_fileSystemModel->setRootPath(root_dir);
	m_treeView->setRootIndex(m_fileSystemModel->index(root_dir));
}

QString FolderWorkspaceDockWidget::GetRootDir() const
{
	return m_fileSystemModel->rootPath();
}

void FolderWorkspaceDockWidget::ExpandAll()
{
	m_treeView->expandAll();
}

void FolderWorkspaceDockWidget::CollapseAll()
{
	m_treeView->collapseAll();
}

void FolderWorkspaceDockWidget::LocationFile(const QString& file_path)
{
	// 取消选择之前的
	QModelIndexList select_indexs = m_treeView->selectionModel()->selectedIndexes();
	for (const auto& index : select_indexs)
	{
		m_treeView->selectionModel()->select(index, QItemSelectionModel::Deselect);
	}
	// 选择指定的
	QModelIndex current_index = m_fileSystemModel->index(file_path);
	m_treeView->selectionModel()->select(current_index, QItemSelectionModel::Select);
	// 展开父节点
	while (current_index.isValid()) 
	{
		m_treeView->expand(current_index);
		current_index = current_index.parent();
	}
}

void FolderWorkspaceDockWidget::InitUi()
{
	// 文件视图
	m_treeView->setModel(m_fileSystemModel);

	QVBoxLayout* layout = new QVBoxLayout(m_widget);
	layout->addWidget(m_toolBar);
	layout->addWidget(m_treeView);
	m_widget->setLayout(layout);
	setWidget(m_widget);
}

void FolderWorkspaceDockWidget::InitValue()
{
}

void FolderWorkspaceDockWidget::InitConnect()
{
	connect(m_expandAllAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Expand All");
		});
	connect(m_collapseAllAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Collapse All");
		});
	connect(m_locationAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Locate The Current File");
		});
}