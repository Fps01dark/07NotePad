#include "on_tab_widget.h"

OnTabWidget::OnTabWidget(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	: m_messageBus(message_bus), QTabWidget(parent) {
	InitUi();
	InitValue();
	InitConnect();
}

OnTabWidget::~OnTabWidget()
{
}

void OnTabWidget::SetTabBar(QTabBar* tab_bar)
{
	setTabBar(tab_bar);
}

void OnTabWidget::InitUi()
{
}

void OnTabWidget::InitValue()
{
}

void OnTabWidget::InitConnect()
{
}