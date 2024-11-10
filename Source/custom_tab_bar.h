#pragma once

#include <QTabBar>

class MessageBus;

class CustomTabBar :public QTabBar
{
	Q_OBJECT
public:
	explicit CustomTabBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~CustomTabBar();

private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
};
