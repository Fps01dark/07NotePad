#pragma execution_character_set("utf-8")
#include "directory_dock_widget.h"

DirectoryDockWidget::DirectoryDockWidget(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	:m_messageBus(message_bus),
	QDockWidget(parent)
{
	InitUi();
	InitValue();
	InitConnect();
}

DirectoryDockWidget::~DirectoryDockWidget()
{
}

void DirectoryDockWidget::InitUi()
{
	setWindowTitle("目录列表");
}

void DirectoryDockWidget::InitValue()
{
}

void DirectoryDockWidget::InitConnect()
{
}