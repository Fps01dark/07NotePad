#include "on_menu_bar.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QActionGroup>
#include <QApplication>

#include "message_bus.h"
#include "on_text_edit.h"

namespace
{
	const int MAX_HISTORY_FILE_SIZE = 10;
}

OnMenuBar::OnMenuBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	: m_messageBus(message_bus), QMenuBar(parent)
{
	InitUi();
	InitValue();
	InitConnect();
}

OnMenuBar::~OnMenuBar()
{
}

void OnMenuBar::SetRecentFiles(const QStringList& recent_list)
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

QStringList OnMenuBar::GetRecentFiles() const
{
	QStringList recent_files;
	const QList<QAction*>& actions = m_recentFileMenu->actions();
	for (int i = 0; i < actions.size(); ++i)
	{
		recent_files.append(actions[i]->text());
	}
	return recent_files;
}

void OnMenuBar::InitUi()
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
	QMenu* edit_menu = this->addMenu(tr("Edit"));
	m_undoAction = edit_menu->addAction(tr("Undo"));
	m_redoAction = edit_menu->addAction(tr("Redo"));
	edit_menu->addSeparator();
	m_cutAction = edit_menu->addAction(tr("Cut"));
	m_copyAction = edit_menu->addAction(tr("Copy"));
	m_pasteAction = edit_menu->addAction(tr("Paste"));
	m_deleteTextAction = edit_menu->addAction(tr("Delete"));
	m_selectAllAction = edit_menu->addAction(tr("Select All"));
	m_beginEndSelectAction = edit_menu->addAction(tr("Begin/End Select"));
	m_beginEndSelectAction->setCheckable(true);
	m_beginEndColumnSelectAction = edit_menu->addAction(tr("Begin/End Select in Column Mode"));
	m_beginEndColumnSelectAction->setCheckable(true);
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
	m_ProperCaseAction = m_convertCaseMenu->addAction(tr("Proper Case"));
	m_ProperCaseBlendAction = m_convertCaseMenu->addAction(tr("Proper Case(blend)"));
	m_SentencecaseAction = m_convertCaseMenu->addAction(tr("Sentence case"));
	m_SentencecaseBlendAction = m_convertCaseMenu->addAction(tr("Sentence case(blend)"));
	m_iNVERTcASEAction = m_convertCaseMenu->addAction(tr("iNVERT cASE"));
	m_ranDOmCasEAction = m_convertCaseMenu->addAction(tr("ranDOm CasE"));
	m_lineOperationsMenu = edit_menu->addMenu(tr("Line Operations"));
	m_duplicateCurrentLineAction = m_lineOperationsMenu->addAction(tr("Duplicate Current Line"));
	m_removeDuplicateLinesAction = m_lineOperationsMenu->addAction(tr("Remove Duplicate Lines"));
	m_removeConsecutiveDuplicateLinesAction = m_lineOperationsMenu->addAction(tr("Remove Consecutive Duplicate Lines"));
	m_splitLinesAction = m_lineOperationsMenu->addAction(tr("Split Lines"));
	m_joinLinesAction = m_lineOperationsMenu->addAction(tr("Join Lines"));
	m_moveUpCurrentLineAction = m_lineOperationsMenu->addAction(tr("Move Up Current Line"));
	m_moveDownCurrentLineAction = m_lineOperationsMenu->addAction(tr("Move Down Current Line"));
	m_removeEmptyLinesAction = m_lineOperationsMenu->addAction(tr("Remove Empty Lines"));
	m_removeEmptyLinesBlankAction = m_lineOperationsMenu->addAction(tr("Remove Empty Lines(Containing Blank Characters)"));
	m_insertBlankLineAboveCurrentAction = m_lineOperationsMenu->addAction(tr("Insert Blank Line Above Current"));
	m_insertBlankLineBelowCurrentAction = m_lineOperationsMenu->addAction(tr("Insert Blank Line Below Current"));
	m_reverseLineOrderAction = m_lineOperationsMenu->addAction(tr("Reverse Line Order"));
	m_randomizeLineOrderAction = m_lineOperationsMenu->addAction(tr("Randomize Line Order"));
	m_lineOperationsMenu->addSeparator();
	m_sortLinesLexicographicallyAscendingAction = m_lineOperationsMenu->addAction(tr("Sort Lines Lexicographically Ascending"));
	m_sortLinesLexAscendingIgnoringCaseAction = m_lineOperationsMenu->addAction(tr("Sort Lines Lex. Ascending Ignoring Case"));
	m_sortLinesAsIntegersAscendingAction = m_lineOperationsMenu->addAction(tr("Sort Lines As Integers Ascending"));
	m_sortLinesAsDecimalsCommaAscendingAction = m_lineOperationsMenu->addAction(tr("Sort Lines As Decimals(Comma) Ascending"));
	m_sortLinesAsDecimalsDotAscendingAction = m_lineOperationsMenu->addAction(tr("Sort Lines As Decimals(Dot) Ascending"));
	m_lineOperationsMenu->addSeparator();
	m_sortLinesLexicographicallyDescendingAction = m_lineOperationsMenu->addAction(tr("Sort Lines Lexicographically Descending"));
	m_sortLinesLexDescendingIgnoringCaseAction = m_lineOperationsMenu->addAction(tr("Sort Lines Lex. Descending Ignoring Case"));
	m_sortLinesAsIntegersDescendingAction = m_lineOperationsMenu->addAction(tr("Sort Lines As Integers Descending"));
	m_sortLinesAsDecimalsCommaDescendingAction = m_lineOperationsMenu->addAction(tr("Sort Lines As Decimals(Comma) Descending"));
	m_sortLinesAsDecimalsDotDescendingAction = m_lineOperationsMenu->addAction(tr("Sort Lines As Decimals(Dot) Descending"));
	m_commentMenu = edit_menu->addMenu(tr("Comment/Uncomment"));
	m_toggleSingleLineCommentAction = m_commentMenu->addAction(tr("Toggle Single Line Comment"));
	m_singleLineCommentAction = m_commentMenu->addAction(tr("Single Line Comment"));
	m_singleLineUncommentAction = m_commentMenu->addAction(tr("Single Line Uncomment"));
	m_blockCommentAction = m_commentMenu->addAction(tr("Block Comment"));
	m_blockUncommentAction = m_commentMenu->addAction(tr("Block Uncomment"));
	m_autoCompletionMenu = edit_menu->addMenu(tr("Auto Completion"));
	m_functionCompletionAction = m_autoCompletionMenu->addAction(tr("Function Completion"));
	m_wordCompletionAction = m_autoCompletionMenu->addAction(tr("Word Completion"));
	m_functionParametersHintAction = m_autoCompletionMenu->addAction(tr("Function Parameters Hint"));
	m_functionParametersPreviousHintAction = m_autoCompletionMenu->addAction(tr("Function Parameters Previous Hint"));
	m_functionParametersNextHintAction = m_autoCompletionMenu->addAction(tr("Function Parameters Next Hint"));
	m_pathCompletion = m_autoCompletionMenu->addAction(tr("Path Completion"));
	m_eolConvertMenu = edit_menu->addMenu(tr("EOL Conversion"));
	m_eolWindowsAction = m_eolConvertMenu->addAction(tr("Windows(CR LF)"));
	m_eolWindowsAction->setCheckable(true);
	m_eolUnixAction = m_eolConvertMenu->addAction(tr("Unix(LF)"));
	m_eolUnixAction->setCheckable(true);
	m_eolMacintoshAction = m_eolConvertMenu->addAction(tr("Macintosh(CR)"));
	m_eolMacintoshAction->setCheckable(true);
	QActionGroup* eol_convert_action_group = new QActionGroup(this);
	eol_convert_action_group->addAction(m_eolWindowsAction);
	eol_convert_action_group->addAction(m_eolUnixAction);
	eol_convert_action_group->addAction(m_eolMacintoshAction);
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
	help_menu->addSeparator();
	m_aboutQt = help_menu->addAction(tr("About Qt"));
	m_aboutQt->setIcon(QIcon(":/Qt-project.org/Qt-project.org/QMessagebox/Images/qtlogo-64.png"));
	m_aboutOneNotePad = help_menu->addAction(tr("About OneNotePad"));
	m_aboutOneNotePad->setIcon(QIcon(":/Prefix/NotepadNext.ico"));
	m_debugInfoAction = help_menu->addAction(tr("Debug Info"));
}

void OnMenuBar::InitValue()
{
	m_messageBus->Subscribe("Add Recent File", [this](const QString& file_path)
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
	m_messageBus->Subscribe("Update Menu Bar", [this](OnTextEdit* editor)
		{
			qDebug() << "This file is " << __FILE__ << " on line " << __LINE__;
			qDebug(Q_FUNC_INFO);

			// Edit
			if (editor->GetBeginSelectPosition() == -1)
			{
				// 未选中
				m_beginEndSelectAction->setChecked(false);
			}
			else
			{
				// 选中
				m_beginEndSelectAction->setChecked(true);
			}
			if (editor->GetBeginColumnSelectPosition() == -1)
			{
				// 未选中
				m_beginEndColumnSelectAction->setChecked(false);
			}
			else
			{
				// 选中
				m_beginEndColumnSelectAction->setChecked(true);
			}
		});
}

void OnMenuBar::InitConnect()
{
	// 文件
	connect(m_newAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("New File");
		});
	connect(m_openAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Open File");
		});
	connect(m_explorerAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Open Explorer");
		});
	connect(m_cmdAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Open Cmd");
		});
	connect(m_defaultViewerAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Open In Default Viewer");
		});
	connect(m_folderAsWorkspace, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Open Directory Workspace");
		});
	connect(m_openFolderAsWorkspaceAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Open Directory As Workspace");
		});
	connect(m_reloadAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Reload File");
		});
	connect(m_saveAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Save File");
		});
	connect(m_saveAsAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Save As File");
		});
	connect(m_copySaveAsAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Save As Clipboard");
		});
	connect(m_saveAllAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Save All File");
		});
	connect(m_renameAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Save As File");
		});
	connect(m_closeAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Close File");
		});
	connect(m_closeAllAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Close All File");
		});
	connect(m_closeAllButCurrentAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Close All But Current File");
		});
	connect(m_closeLeftAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Close Left File");
		});
	connect(m_closeRightAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Close Right File");
		});
	connect(m_closeAllUnchangeAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Close All Unchanged File");
		});
	connect(m_deleteFileAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Delete File");
		});
	connect(m_loadSessionAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Load Session");
		});
	connect(m_saveSessionAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Save Session");
		});
	connect(m_printAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Print");
		});
	connect(m_openAllRecentAction, &QAction::triggered, [this]()
		{
			for (auto action : m_recentFileMenu->actions())
			{
				action->triggered();
			}
		});
	connect(m_clearRecentAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Clear Recent Record");
		});
	connect(m_exitSotfwareAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Exit Software");
		});
	// 编辑
	connect(m_undoAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Undo");
		});
	connect(m_redoAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Redo");
		});
	connect(m_cutAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Cut");
		});
	connect(m_copyAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Copy");
		});
	connect(m_pasteAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Paste");
		});
	connect(m_deleteTextAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Delete");
		});
	connect(m_selectAllAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Select All");
		});
	connect(m_beginEndSelectAction, &QAction::triggered, [this]()
		{
			if (m_beginEndSelectAction->isChecked() == true)
			{
				// 开始
				m_messageBus->Publish("Begin/End Select", 1);
			}
			else
			{
				// 结束
				m_messageBus->Publish("Begin/End Select", 0);
			}
		});
	connect(m_beginEndColumnSelectAction, &QAction::triggered, [this]()
		{
			if (m_beginEndColumnSelectAction->isChecked() == true)
			{
				// 开始
				m_messageBus->Publish("Begin/End Select in Column Mode", 1);
			}
			else
			{
				// 结束
				m_messageBus->Publish("Begin/End Select in Column Mode", 0);
			}
		});
	connect(m_shortTimeAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Insert Short Time");
		});
	connect(m_longTimeAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Insert Long Time");
		});
	connect(m_customTimeAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Insert Custom Time");
		});
	connect(m_copyFilePathAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Copy Path");
		});
	connect(m_copyFileNameAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Copy Name");
		});
	connect(m_copyFileDirAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Copy Directory");
		});
	connect(m_copyAllFileNameAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Copy All Names");
		});
	connect(m_copyAllFilePathAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Copy All Paths");
		});
	connect(m_insertLineIndentAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Increase Line Indent");
		});
	connect(m_deleteLineIndentAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Decrease Line Indent");
		});
	connect(m_UPPERCASEAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("UPPERCASE");
		});
	connect(m_lowercaseAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("lowercase");
		});
	connect(m_ProperCaseAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Proper Case");
		});
	connect(m_ProperCaseBlendAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Proper Case(blend)");
		});
	connect(m_SentencecaseAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Sentence case");
		});
	connect(m_SentencecaseBlendAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Sentence case(blend)");
		});
	connect(m_iNVERTcASEAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("iNVERT cASE");
		});
	connect(m_ranDOmCasEAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("ranDOm CasE");
		});
	connect(m_toggleSingleLineCommentAction, &QAction::triggered, [this]()
		{
			// TODO:添加/删除单行注释
		});
	connect(m_singleLineCommentAction, &QAction::triggered, [this]()
		{
			// TODO:设置行注释
		});
	connect(m_singleLineUncommentAction, &QAction::triggered, [this]()
		{
			// TODO:取消行注释
		});
	connect(m_blockCommentAction, &QAction::triggered, [this]()
		{
			// TODO:设置区块注释
		});
	connect(m_blockUncommentAction, &QAction::triggered, [this]()
		{
			// TODO:取消区块注释
		});
	connect(m_functionCompletionAction, &QAction::triggered, [this]()
		{
			// TODO:函数补全
		});
	connect(m_wordCompletionAction, &QAction::triggered, [this]()
		{
			// TODO:单词补全
		});
	connect(m_functionParametersHintAction, &QAction::triggered, [this]()
		{
			// TODO:函数参数提示
		});
	connect(m_functionParametersPreviousHintAction, &QAction::triggered, [this]()
		{
			// TODO:函数参数上一个提示
		});
	connect(m_functionParametersNextHintAction, &QAction::triggered, [this]()
		{
			// TODO:函数参数下一个提示
		});
	connect(m_pathCompletion, &QAction::triggered, [this]()
		{
			// TODO:路径补全
		});
	connect(m_duplicateCurrentLineAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Duplicate Current Line");
		});
	connect(m_removeDuplicateLinesAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Remove Duplicate Line");
		});
	connect(m_removeConsecutiveDuplicateLinesAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Remove Consecutive Duplicate Lines");
		});
	connect(m_splitLinesAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Split Lines");
		});
	connect(m_joinLinesAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Join Lines");
		});
	connect(m_moveUpCurrentLineAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Move Up Current Line");
		});
	connect(m_moveDownCurrentLineAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Move Down Current Line");
		});
	connect(m_removeEmptyLinesAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Remove Empty Lines");
		});
	connect(m_removeEmptyLinesBlankAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Remove Empty Lines Blank");
		});
	connect(m_insertBlankLineAboveCurrentAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Insert Blank Line Above Current");
		});
	connect(m_insertBlankLineBelowCurrentAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Insert Blank Line Below Current");
		});
	connect(m_reverseLineOrderAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Reverse Line Order");
		});
	connect(m_randomizeLineOrderAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Randomize Line Order");
		});
	connect(m_sortLinesLexicographicallyAscendingAction, &QAction::triggered, [this]()
		{
			// TODO:升序排列文本行
		});
	connect(m_sortLinesLexAscendingIgnoringCaseAction, &QAction::triggered, [this]()
		{
			// TODO:升序排列行（不分大小写）
		});
	connect(m_sortLinesAsIntegersAscendingAction, &QAction::triggered, [this]()
		{
			// TODO:升序排列整数
		});
	connect(m_sortLinesAsDecimalsCommaAscendingAction, &QAction::triggered, [this]()
		{
			// TODO:升序排列小数（逗号作为小数点）
		});
	connect(m_sortLinesAsDecimalsDotAscendingAction, &QAction::triggered, [this]()
		{
			// TODO:升序排列小数（句号作为小数点）
		});
	connect(m_sortLinesLexicographicallyDescendingAction, &QAction::triggered, [this]()
		{
			// TODO:降序排列文本行
		});
	connect(m_sortLinesLexDescendingIgnoringCaseAction, &QAction::triggered, [this]()
		{
			// TODO:降序排列行（不分大小写）
		});
	connect(m_sortLinesAsIntegersDescendingAction, &QAction::triggered, [this]()
		{
			// TODO:降序排列整数
		});
	connect(m_sortLinesAsDecimalsCommaDescendingAction, &QAction::triggered, [this]()
		{
			// TODO:降序排列小数（逗号作为小数点）
		});
	connect(m_sortLinesAsDecimalsDotDescendingAction, &QAction::triggered, [this]()
		{
			// TODO:降序排列小数（句号作为小数点）
		});
	connect(m_eolWindowsAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("EOL Conversion", SC_EOL_CRLF);
		});
	connect(m_eolUnixAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("EOL Conversion", SC_EOL_LF);
		});
	connect(m_eolMacintoshAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("EOL Conversion", SC_EOL_CR);
		});

	connect(m_aboutQt, &QAction::triggered, &QApplication::aboutQt);
	connect(m_aboutOneNotePad, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("About OneNotePad");
		});
	connect(m_debugInfoAction, &QAction::triggered, [this]()
		{
			m_messageBus->Publish("Debug Info");
		});
}