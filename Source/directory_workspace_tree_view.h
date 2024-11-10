#pragma once

#include <QTreeView>

class MessageBus;

class DirectoryWorkspaceTreeView : public QTreeView
{
	Q_OBJECT
public:
	explicit DirectoryWorkspaceTreeView(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~DirectoryWorkspaceTreeView();

protected:
	void mouseMoveEvent(QMouseEvent* event) override;

private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
};
