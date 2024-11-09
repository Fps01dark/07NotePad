#pragma once

#include <QDockWidget>
#include "ui_folder_workspace_dock_widget.h"

class QAction;
class QToolBar;
class MessageBus;
class QFileSystemModel;

class FolderWorkspaceDockWidget :public QDockWidget
{
	Q_OBJECT
public:
	explicit FolderWorkspaceDockWidget(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~FolderWorkspaceDockWidget();
	void SetRootDir(const QString& root_dir);
	QString GetRootDir() const;
	void ExpandAll();
	void CollapseAll();
	void LocationFile(const QString& file_path);

private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
	QWidget* m_widget = nullptr;
	QToolBar* m_toolBar = nullptr;
	QAction* m_expandAllAction = nullptr;
	QAction* m_collapseAllAction = nullptr;
	QAction* m_locationAction = nullptr;
	QFileSystemModel* m_fileSystemModel = nullptr;
	QTreeView* m_treeView = nullptr;
};
