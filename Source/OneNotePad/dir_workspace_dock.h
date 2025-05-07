#pragma once

#include <QDockWidget>

class QAction;
class QToolBar;
class QFileSystemModel;
class MessageBus;
class DirWorkspaceTreeView;

class DirWorkspaceDock : public QDockWidget
{
	Q_OBJECT
public:
	explicit DirWorkspaceDock(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~DirWorkspaceDock();
	void    SetRootDir(const QString& root_dir);
	QString GetRootDir() const;

private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
	QFileSystemModel* m_fileSystemModel = nullptr;
	DirWorkspaceTreeView* m_treeView = nullptr;
};
