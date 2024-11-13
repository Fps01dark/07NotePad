#include "custom_menu_bar.h"

#include "framework.h"

#include "message_bus.h"

CustomMenuBar::CustomMenuBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	:m_messageBus(message_bus),
	QMenuBar(parent)
{
	InitUi();
	InitValue();
	InitConnect();
}

CustomMenuBar::~CustomMenuBar()
{
}

void CustomMenuBar::SetRecentFiles(const QStringList& recent_list)
{
	m_recentFileMenu->clear();
	for (int i = 0; i < recent_list.size(); ++i)
	{
		// 新增
		QAction* action = new QAction(recent_list[i], m_recentFileMenu);
		connect(action, &QAction::triggered, [=]()
			{
				QFileInfo file_info(recent_list[i]);
				if (recent_list[i].isEmpty() || !file_info.exists() || file_info.isDir())
				{
					m_recentFileMenu->removeAction(action);
				}
				m_messageBus->Publish("Open File", QStringList() << recent_list[i]);
			});
		// 添加
		m_recentFileMenu->addAction(action);
	}
}

void CustomMenuBar::InitUi()
{
	// 菜单栏-文件
	QMenu* file_menu = this->addMenu(tr("File"));
	m_newAction = file_menu->addAction(tr("New"));
	m_openAction = file_menu->addAction(tr("Open..."));
	m_openFolderMenu = file_menu->addMenu(tr("Open Contanining Folder"));
	m_explorerAction = m_openFolderMenu->addAction(tr("Explorer"));
	m_cmdAction = m_openFolderMenu->addAction(tr("Cmd"));
	m_openFolderMenu->addSeparator();
	m_folderAsWorkspace = m_openFolderMenu->addAction(tr("Folder As Workspace"));
	m_defaultViewerAction = file_menu->addAction(tr("Open In Default Viewer"));
	m_openFolderAsWorkspaceAction = file_menu->addAction(tr("Open Folder As Workspace..."));
	m_reloadAction = file_menu->addAction(tr("Reload From Disk"));
	m_saveAction = file_menu->addAction(tr("Save"));
	m_saveAsAction = file_menu->addAction(tr("Save As..."));
	m_copySaveAsAction = file_menu->addAction(tr("Save A Copy As..."));
	m_saveAllAction = file_menu->addAction(tr("Save All"));
	m_renameAction = file_menu->addAction(tr("Rename..."));
	m_closeAction = file_menu->addAction(tr("Close"));
	m_closeAllAction = file_menu->addAction(tr("Close All"));
	m_closeMenu = file_menu->addMenu(tr("Close Multiple File"));
	m_closeAllButCurrentAction = m_closeMenu->addAction(tr("Close All But Current File"));
	m_closeLeftAction = m_closeMenu->addAction(tr("Close All To The Left"));
	m_closeRightAction = m_closeMenu->addAction(tr("Close All To The Right"));
	m_closeAllUnchangeAction = m_closeMenu->addAction(tr("Close All Unchanged"));
	m_deleteAction = file_menu->addAction(tr("Delete File"));
	file_menu->addSeparator();
	m_loadSessionAction = file_menu->addAction(tr("Load Session..."));
	m_saveSessionAction = file_menu->addAction(tr("Save Session..."));
	file_menu->addSeparator();
	m_printAction = file_menu->addAction(tr("Print..."));
	m_printNowAction = file_menu->addAction(tr("Print Now"));
	file_menu->addSeparator();
	m_recentFileMenu = file_menu->addMenu(tr("Recent File"));
	m_openAllRecentAction = file_menu->addAction(tr("Open All Recent Files"));
	m_clearRecentAction = file_menu->addAction(tr("Clear Recent Files Record"));
	file_menu->addSeparator();
	m_exitSotfwareAction = file_menu->addAction(tr("Exit"));
	// 菜单栏-编辑

	// 菜单栏-查找

	// 菜单栏-视图

	// 菜单栏-编码

	// 菜单栏-语言

	// 菜单栏-设置

	// 菜单栏-关于
}

void CustomMenuBar::InitValue()
{
}

void CustomMenuBar::InitConnect()
{
	connect(m_newAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("New File");
		});
	connect(m_openAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open File");
		});
	connect(m_explorerAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open Explorer");
		});
	connect(m_cmdAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open Cmd");
		});
	connect(m_defaultViewerAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open In Default Viewer");
		});
	connect(m_folderAsWorkspace, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open Directory Workspace");
		});
	connect(m_openFolderAsWorkspaceAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open Directory As Directory Workspace");
		});
	connect(m_reloadAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Reload File");
		});
	connect(m_saveAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Save File");
		});
	connect(m_saveAsAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Save As File");
		});
	connect(m_copySaveAsAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Save As Clipboard");
		});
	connect(m_saveAllAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Save All File");
		});
	connect(m_renameAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Save As File");
		});
	connect(m_closeAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Close File");
		});
	connect(m_closeAllAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Close All File");
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
	connect(m_deleteAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Delete File");
		});
	connect(m_openAllRecentAction, &QAction::triggered, [=]()
		{
			for (auto action : m_recentFileMenu->actions())
			{
				action->triggered();
			}
		});
	connect(m_clearRecentAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Clear Recent File");
		});
	connect(m_exitSotfwareAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Exit Software");
		});
}