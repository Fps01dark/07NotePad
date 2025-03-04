#include "custom_tab_widget.h"

CustomTabWidget::CustomTabWidget(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	: m_messageBus(message_bus), QTabWidget(parent) {
	InitUi();
	InitValue();
	InitConnect();
}

CustomTabWidget::~CustomTabWidget() {}

void CustomTabWidget::SetTabBar(QTabBar* tab_bar) { setTabBar(tab_bar); }

void CustomTabWidget::InitUi() {}

void CustomTabWidget::InitValue() {}

void CustomTabWidget::InitConnect() {}