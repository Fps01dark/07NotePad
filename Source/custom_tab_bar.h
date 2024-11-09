#pragma once

#include <QTabBar>

class MessageBus;

class CustomTabBar :public QTabBar
{
public:
	CustomTabBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~CustomTabBar();

protected:
	void contextMenuEvent(QContextMenuEvent* event) override;

private:
	void InitUi();
	void InitValue();
	void InitConnect();
private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
};
