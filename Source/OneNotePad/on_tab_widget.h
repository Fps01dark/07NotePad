#pragma once

#include <QSet>
#include <QTabWidget>

class MessageBus;

class OnTabWidget : public QTabWidget
{
	Q_OBJECT
public:
	explicit OnTabWidget(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~OnTabWidget();
	void SetTabBar(QTabBar* tab_bar);

private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
};
