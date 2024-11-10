#pragma once

#include <QSet>
#include <QTabWidget>

class MessageBus;

class CustomTabWidget :public QTabWidget
{
	Q_OBJECT
public:
	explicit CustomTabWidget(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~CustomTabWidget();
	void SetTabBar(QTabBar* tab_bar);

private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
};
