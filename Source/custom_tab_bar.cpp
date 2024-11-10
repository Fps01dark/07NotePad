#include "custom_tab_bar.h"

#include "framework.h"

#include "message_bus.h"

CustomTabBar::CustomTabBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	:m_messageBus(message_bus),
	QTabBar(parent)
{
	InitUi();
	InitValue();
	InitConnect();
}

CustomTabBar::~CustomTabBar()
{
}

void CustomTabBar::InitUi()
{
}

void CustomTabBar::InitValue()
{
}

void CustomTabBar::InitConnect()
{
}