#include "directory_workspace_tree_view.h"

#include "framework.h"

DirectoryWorkspaceTreeView::DirectoryWorkspaceTreeView(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	:m_messageBus(message_bus),
	QTreeView(parent)
{
	InitUi();
	InitValue();
	InitConnect();
}

DirectoryWorkspaceTreeView::~DirectoryWorkspaceTreeView()
{
}

void DirectoryWorkspaceTreeView::mouseMoveEvent(QMouseEvent* event)
{
	// 判断鼠标是否右键按下
	if (event->buttons() & Qt::RightButton)
	{
		// 获取鼠标当前的位置
		QPoint position = event->pos();
		qDebug() << "Mouse moved with right button at position:" << position;
	}

	// 如果需要调用父类的事件处理，可以取消注释下一行
	// QWidget::mouseMoveEvent(event);
}

void DirectoryWorkspaceTreeView::InitUi()
{
}

void DirectoryWorkspaceTreeView::InitValue()
{
}

void DirectoryWorkspaceTreeView::InitConnect()
{
}