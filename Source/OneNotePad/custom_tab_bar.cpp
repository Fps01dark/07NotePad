#include "custom_tab_bar.h"

#include <QMenu>
#include <QContextMenuEvent>

#include "message_bus.h"

CustomTabBar::CustomTabBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	: m_messageBus(message_bus), QTabBar(parent)
{
	InitUi();
	InitValue();
	InitConnect();
}

CustomTabBar::~CustomTabBar() {}

void CustomTabBar::contextMenuEvent(QContextMenuEvent* event)
{
	int index = tabAt(event->pos());
	if (index >= 0 && index < count())
	{
		setCurrentIndex(index);
		// 在右键点击的当前位置弹出菜单
		m_itemMenu->exec(event->globalPos());
	}
}

void CustomTabBar::InitUi()
{
	m_itemMenu = new QMenu(this);
	m_closeAction = m_itemMenu->addAction(tr("Close"));
	m_closeMenu = m_itemMenu->addMenu(tr("Close Multiple Tabs"));
	m_closeAllButCurrentAction = m_closeMenu->addAction(tr("Close All But Current File"));
	m_closeLeftAction = m_closeMenu->addAction(tr("Close All To The Left"));
	m_closeRightAction = m_closeMenu->addAction(tr("Close All To The Right"));
	m_closeAllUnchangeAction = m_closeMenu->addAction(tr("Close All Unchanged"));
	m_saveAction = m_itemMenu->addAction(tr("Save"));
	m_saveAsAction = m_itemMenu->addAction(tr("Save As..."));
	m_openFolderMenu = m_itemMenu->addMenu(tr("Open Into"));
	m_explorerAction = m_openFolderMenu->addAction(tr("Open Containing Folder In Explorer"));
	m_cmdAction = m_openFolderMenu->addAction(tr("Open Containing Folder In Cmd"));
	m_folderAsWorkspace = m_openFolderMenu->addAction(tr("Open Containing Folder As Workspace"));
	m_openFolderMenu->addSeparator();
	m_defaultViewerAction = m_openFolderMenu->addAction(tr("Open In Default Viewer"));
	m_renameAction = m_itemMenu->addAction(tr("Rename"));
	m_deleteAction = m_itemMenu->addAction(tr("Delete File"));
	m_reloadAction = m_itemMenu->addAction(tr("Reload"));
	m_printAction = m_itemMenu->addAction(tr("Print"));
	m_itemMenu->addSeparator();
	m_readOnlyAction = m_itemMenu->addAction(tr("Read-Only"));
	m_clearReadOnlyAction = m_itemMenu->addAction(tr("Clear Read-Only Flag"));
	m_itemMenu->addSeparator();
	m_copyToClipboardMenu = m_itemMenu->addMenu(tr("Copy To Clipboard"));
	m_copyPathAction = m_copyToClipboardMenu->addAction(tr("Copy Full File Path"));
	m_copyNameAction = m_copyToClipboardMenu->addAction(tr("Copy File Name"));
	m_copyDirAction = m_copyToClipboardMenu->addAction(tr("Copy Directory Path"));
	m_moveFileMenu = m_itemMenu->addMenu(tr("Move File"));
	m_moveToStartAction = m_moveFileMenu->addAction(tr("Move To Satrt"));
	m_moveToEndAction = m_moveFileMenu->addAction(tr("Move To End"));
	m_moveFileMenu->addSeparator();
	m_moveToOtherViewAction = m_moveFileMenu->addAction(tr("Move To Other View"));
	m_cloneToOtherViewAction = m_moveFileMenu->addAction(tr("Clone To Other View"));
	m_moveToNewInstanceAction = m_moveFileMenu->addAction(tr("Move To New Instance"));
	m_openInNewInstanceAction = m_moveFileMenu->addAction(tr("Open In New Instance"));
	m_colorTabMenu = m_itemMenu->addMenu(tr("Apply Color To Tab"));
	m_color1Action = m_colorTabMenu->addAction(tr("Apply Color 1"));
	m_color2Action = m_colorTabMenu->addAction(tr("Apply Color 2"));
	m_color3Action = m_colorTabMenu->addAction(tr("Apply Color 3"));
	m_color4Action = m_colorTabMenu->addAction(tr("Apply Color 4"));
	m_color5Action = m_colorTabMenu->addAction(tr("Apply Color 5"));
	m_removeColorAction = m_colorTabMenu->addAction(tr("Remove Color"));
}

void CustomTabBar::InitValue()
{
}

void CustomTabBar::InitConnect()
{
	connect(m_closeAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Close File");
		});
	connect(m_closeAllButCurrentAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Close All But Current File");
		});
	connect(m_closeLeftAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Close Left File");
		});
	connect(m_closeRightAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Close Right File");
		});
	connect(m_closeAllUnchangeAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Close All Unchanged File");
		});
	connect(m_saveAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Save File");
		});
	connect(m_saveAsAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Save As File");
		});
	connect(m_explorerAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open Explorer");
		});
	connect(m_cmdAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open Cmd");
		});
	connect(m_folderAsWorkspace, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open Directory Workspace");
		});
	connect(m_defaultViewerAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open In Default Viewer");
		});
	connect(m_renameAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Save As File");
		});
	connect(m_deleteAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Delete File");
		});
	connect(m_reloadAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Reload File");
		});
	connect(m_copyPathAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Copy Path");
		});
	connect(m_copyNameAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Copy Name");
		});
	connect(m_copyDirAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Copy Directory");
		});
}