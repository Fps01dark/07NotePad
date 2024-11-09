#pragma once

#include<QDockWidget>

class MessageBus;

class DirectoryDockWidget :public QDockWidget
{
public:
	DirectoryDockWidget(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~DirectoryDockWidget();

private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
};
