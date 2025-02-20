#include "custom_menu_bar.h"

#include "framework.h"
#include "message_bus.h"
#include "custom_text_edit.h"

namespace
{
	const int MAX_HISTORY_FILE_SIZE = 10;
}

CustomMenuBar::CustomMenuBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	: m_messageBus(message_bus), QMenuBar(parent)
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
				m_messageBus->Publish("Open File", recent_list[i]);
			});
		// 添加
		m_recentFileMenu->addAction(action);
	}
}

QStringList CustomMenuBar::GetRecentFiles() const
{
	QStringList recent_files;
	const QList<QAction*>& actions = m_recentFileMenu->actions();
	for (int i = 0; i < actions.size(); ++i)
	{
		recent_files.append(actions[i]->text());
	}
	return recent_files;
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
	m_deleteFileAction = file_menu->addAction(tr("Delete File"));
	file_menu->addSeparator();
	m_loadSessionAction = file_menu->addAction(tr("Load Session...(未实现)"));
	m_saveSessionAction = file_menu->addAction(tr("Save Session...(未实现)"));
	file_menu->addSeparator();
	m_printAction = file_menu->addAction(tr("Print..."));
	m_printNowAction = file_menu->addAction(tr("Print Now(未实现)"));
	file_menu->addSeparator();
	m_recentFileMenu = file_menu->addMenu(tr("Recent File"));
	m_openAllRecentAction = file_menu->addAction(tr("Open All Recent Files"));
	m_clearRecentAction = file_menu->addAction(tr("Clear Recent Files Record"));
	file_menu->addSeparator();
	m_exitSotfwareAction = file_menu->addAction(tr("Exit"));
	// 菜单栏-编辑
	QMenu* edit_menu = this->addMenu(tr("Edit"));
	m_undoAction = edit_menu->addAction(tr("Undo"));
	m_redoAction = edit_menu->addAction(tr("Redo"));
	edit_menu->addSeparator();
	m_cutAction = edit_menu->addAction(tr("Cut"));
	m_copyAction = edit_menu->addAction(tr("Copy"));
	m_pasteAction = edit_menu->addAction(tr("Paste"));
	m_deleteTextAction = edit_menu->addAction(tr("Delete"));
	m_selectAllAction = edit_menu->addAction(tr("Select All"));
	edit_menu->addSeparator();
	m_insertMenu = edit_menu->addMenu(tr("Insert"));
	m_shortTimeAction = m_insertMenu->addAction(tr("Short Date Time"));
	m_longTimeAction = m_insertMenu->addAction(tr("Long Data Time"));
	m_customTimeAction = m_insertMenu->addAction(tr("Custom Date Time"));
	m_copyToClipboardMenu = edit_menu->addMenu(tr("Copy To Clipboard"));
	m_copyFilePathAction = m_copyToClipboardMenu->addAction(tr("Copy Current Full File Path"));
	m_copyFileNameAction = m_copyToClipboardMenu->addAction(tr("Copy Current Filename"));
	m_copyFileDirAction = m_copyToClipboardMenu->addAction(tr("Copy Current Dir Name"));
	m_copyToClipboardMenu->addSeparator();
	m_copyAllFileNameAction = m_copyToClipboardMenu->addAction(tr("Copy All Filenames"));
	m_copyAllFilePathAction = m_copyToClipboardMenu->addAction(tr("Copy All File Paths"));
	m_indentMenu = edit_menu->addMenu(tr("Indent"));
	m_insertLineIndentAction = m_indentMenu->addAction(tr("Increase Line Indent"));
	m_deleteLineIndentAction = m_indentMenu->addAction(tr("Decrease Line Indent"));
	m_convertCaseMenu = edit_menu->addMenu(tr("Convert Case To"));
	m_UPPERCASEAction = m_convertCaseMenu->addAction(tr("UPPERCASE"));
	m_lowercaseAction = m_convertCaseMenu->addAction(tr("lowercase"));
	m_lineOperationsMenu = edit_menu->addMenu(tr("Line Operations"));
	m_duplicateCurrentLineAction = m_lineOperationsMenu->addAction(tr("Duplicate Current Line"));
	m_removeDuplicateLinesAction = m_lineOperationsMenu->addAction(tr("Remove Duplicate Lines(未实现)"));
	m_removeConsecutiveDuplicateLinesAction = m_lineOperationsMenu->addAction(tr("Remove Consecutive Duplicate Lines(未实现)"));
	m_splitLinesAction = m_lineOperationsMenu->addAction(tr("Split Lines"));
	m_joinLinesAction = m_lineOperationsMenu->addAction(tr("Join Lines"));
	m_moveUpCurrentLineAction = m_lineOperationsMenu->addAction(tr("Move Up Current Line"));
	m_moveDownCurrentLineAction = m_lineOperationsMenu->addAction(tr("Move Down Current Line"));
	m_removeEmptyLinesAction = m_lineOperationsMenu->addAction(tr("Remove Empty Lines(未实现)"));
	m_removeEmptyLinesBlankAction = m_lineOperationsMenu->addAction(tr("Remove Empty Lines(Containing Blank Characters)(未实现)"));
	m_insertBlankLineAboveCurrentAction = m_lineOperationsMenu->addAction(tr("Insert Blank Line Above Current"));
	m_insertBlankLineBelowCurrentAction = m_lineOperationsMenu->addAction(tr("Insert Blank Line Below Current"));
	m_reverseLineOrderAction = m_lineOperationsMenu->addAction(tr("Reverse Line Order"));
	m_randomizeLineOrderAction = m_lineOperationsMenu->addAction(tr("Randomize Line Order"));
	m_lineOperationsMenu->addSeparator();
	m_eolConvertMenu = edit_menu->addMenu(tr("EOL Conversion"));
	m_eolWindowsAction = m_eolConvertMenu->addAction(tr("Windows(CR LF)"));
	m_eolUnixAction = m_eolConvertMenu->addAction(tr("Unix(LF)"));
	m_eolMacintoshAction = m_eolConvertMenu->addAction(tr("Macintosh(CR)"));
	// 菜单栏-查找
	QMenu* search_menu = this->addMenu(tr("Search"));
	m_findAction = search_menu->addAction(tr("Find"));
	// 菜单栏-视图
	QMenu* view_menu = this->addMenu(tr("View"));
	m_alwaysOnTopAction = view_menu->addAction(tr("Always On Top"));
	// 菜单栏-编码
	QMenu* encoding_menu = this->addMenu(tr("Encoding"));
	m_anslAction = encoding_menu->addAction(tr("ANSI"));
	// 菜单栏-语言
	QMenu* language_menu = this->addMenu(tr("Language"));
	m_noneAction = language_menu->addAction(tr("None(Normal Text)"));
	// 菜单栏-设置
	QMenu* settings_menu = this->addMenu(tr("Settings"));
	m_preferencesAction = settings_menu->addAction(tr("Preferences"));
	// 菜单栏-工具
	QMenu* tools_menu = this->addMenu(tr("Tools"));
	m_md5Menu = tools_menu->addMenu(tr("MD5"));
	// 菜单栏-宏
	QMenu* macro_menu = this->addMenu(tr("Macro"));
	m_startRecordingAction = macro_menu->addAction(tr("Start Recording"));
	// 菜单栏-运行
	QMenu* run_menu = this->addMenu(tr("Run"));
	m_runAction = run_menu->addAction(tr("Run"));
	// 菜单栏-插件
	QMenu* plugins_menu = this->addMenu(tr("Plugins"));
	m_openPluginsFolderAction = plugins_menu->addAction(tr("Open Plugins Folder"));
	// 菜单栏-窗口
	QMenu* window_menu = this->addMenu(tr("Window"));
	m_sortByMenu = window_menu->addMenu(tr("Sort By"));
	// 菜单栏-帮助
	QMenu* help_menu = this->addMenu(tr("Help"));
	m_commandLineArgumentsAction = help_menu->addAction(tr("Command Line Arguments"));
	m_debugAction = help_menu->addAction(tr("Debug"));
}

void CustomMenuBar::InitValue()
{
	m_messageBus->Subscribe("Add Recent File", [=](const QString& file_path)
		{
			// Data
			m_recentFiles.removeOne(file_path);
			if (m_recentFiles.size() >= MAX_HISTORY_FILE_SIZE)
			{
				m_recentFiles.removeLast();
			}
			m_recentFiles.prepend(file_path);
			// UI
			SetRecentFiles(m_recentFiles);
		});
}

void CustomMenuBar::InitConnect()
{
	// 文件
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
			m_messageBus->Publish("Open Directory As Workspace");
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
	connect(m_deleteFileAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Delete File");
		});
	connect(m_printAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Print");
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
			m_messageBus->Publish("Clear Recent Record");
		});
	connect(m_exitSotfwareAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Exit Software");
		});
	// 编辑
	connect(m_undoAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Undo");
		});
	connect(m_redoAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Redo");
		});
	connect(m_cutAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Cut");
		});
	connect(m_copyAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Copy");
		});
	connect(m_pasteAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Paste");
		});
	connect(m_deleteTextAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Delete");
		});
	connect(m_selectAllAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Select All");
		});
	connect(m_shortTimeAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Insert Short Time");
		});
	connect(m_longTimeAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Insert Long Time");
		});
	connect(m_customTimeAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Insert Custom Time");
		});
	connect(m_copyFilePathAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Copy Path");
		});
	connect(m_copyFileNameAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Copy Name");
		});
	connect(m_copyFileDirAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Copy Directory");
		});
	connect(m_copyAllFileNameAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Copy All Names");
		});
	connect(m_copyAllFilePathAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Copy All Paths");
		});
	connect(m_insertLineIndentAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Increase Line Indent");
		});
	connect(m_deleteLineIndentAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Decrease Line Indent");
		});
	connect(m_UPPERCASEAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("UPPERCASE");
		});
	connect(m_lowercaseAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("lowercase");
		});
	connect(m_duplicateCurrentLineAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Duplicate Current Line");
		});
	connect(m_removeDuplicateLinesAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Remove Duplicate Line");
		});
	connect(m_removeConsecutiveDuplicateLinesAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Remove Consecutive Duplicate Lines");
		});
	connect(m_splitLinesAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Split Lines");
		});
	connect(m_joinLinesAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Join Lines");
		});
	connect(m_moveUpCurrentLineAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Move Up Current Line");
		});
	connect(m_moveDownCurrentLineAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Move Down Current Line");
		});
	connect(m_removeEmptyLinesAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Remove Empty Lines");
		});
	connect(m_removeEmptyLinesBlankAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Remove Empty Lines Blank");
		});
	connect(m_insertBlankLineAboveCurrentAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Insert Blank Line Above Current");
		});
	connect(m_insertBlankLineBelowCurrentAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Insert Blank Line Below Current");
		});
	connect(m_reverseLineOrderAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Reverse Line Order");
		});
	connect(m_randomizeLineOrderAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Randomize Line Order");
		});

	connect(m_eolWindowsAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("EOL Conversion", SC_EOL_CRLF);
		});
	connect(m_eolUnixAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("EOL Conversion", SC_EOL_LF);
		});
	connect(m_eolMacintoshAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("EOL Conversion", SC_EOL_CR);
		});

	connect(m_debugAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Debug");
		});
}