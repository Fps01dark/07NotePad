#pragma once

#include <QMenuBar>

class QMenu;
class QAction;
class MessageBus;
class OnTextEdit;

class OnMenuBar : public QMenuBar
{
	Q_OBJECT
public:
	explicit OnMenuBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~OnMenuBar();
	void SetRecentFiles(const QStringList& recent_list);
	QStringList GetRecentFiles() const;

private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
	// 菜单栏-文件
	QAction* m_newAction = nullptr;
	QAction* m_openAction = nullptr;
	QMenu* m_openFolderMenu = nullptr;
	QAction* m_explorerAction = nullptr;
	QAction* m_cmdAction = nullptr;
	QAction* m_folderAsWorkspace = nullptr;
	QAction* m_defaultViewerAction = nullptr;
	QAction* m_openFolderAsWorkspaceAction = nullptr;
	QAction* m_reloadAction = nullptr;
	QAction* m_saveAction = nullptr;
	QAction* m_saveAsAction = nullptr;
	QAction* m_copySaveAsAction = nullptr;
	QAction* m_saveAllAction = nullptr;
	QAction* m_renameAction = nullptr;
	QAction* m_closeAction = nullptr;
	QAction* m_closeAllAction = nullptr;
	QMenu* m_closeMenu = nullptr;
	QAction* m_closeAllButCurrentAction = nullptr;
	QAction* m_closeLeftAction = nullptr;
	QAction* m_closeRightAction = nullptr;
	QAction* m_closeAllUnchangeAction = nullptr;
	QAction* m_deleteFileAction = nullptr;
	QAction* m_loadSessionAction = nullptr;
	QAction* m_saveSessionAction = nullptr;
	QAction* m_printAction = nullptr;
	QAction* m_printNowAction = nullptr;
	QMenu* m_recentFileMenu = nullptr;
	QStringList m_recentFiles;
	QAction* m_openAllRecentAction = nullptr;
	QAction* m_clearRecentAction = nullptr;
	QAction* m_exitSotfwareAction = nullptr;
	// 菜单栏-编辑
	QAction* m_undoAction = nullptr;
	QAction* m_redoAction = nullptr;
	QAction* m_cutAction = nullptr;
	QAction* m_copyAction = nullptr;
	QAction* m_pasteAction = nullptr;
	QAction* m_deleteTextAction = nullptr;
	QAction* m_selectAllAction = nullptr;
	QAction* m_beginEndSelectAction = nullptr;
	QAction* m_beginEndColumnSelectAction = nullptr;
	QMenu* m_insertMenu = nullptr;
	QAction* m_shortTimeAction = nullptr;
	QAction* m_longTimeAction = nullptr;
	QAction* m_customTimeAction = nullptr;
	QMenu* m_copyToClipboardMenu = nullptr;
	QAction* m_copyFilePathAction = nullptr;
	QAction* m_copyFileNameAction = nullptr;
	QAction* m_copyFileDirAction = nullptr;
	QAction* m_copyAllFileNameAction = nullptr;
	QAction* m_copyAllFilePathAction = nullptr;
	QMenu* m_indentMenu = nullptr;
	QAction* m_insertLineIndentAction = nullptr;
	QAction* m_deleteLineIndentAction = nullptr;
	QMenu* m_convertCaseMenu = nullptr;
	QAction* m_UPPERCASEAction = nullptr;
	QAction* m_lowercaseAction = nullptr;
	QAction* m_ProperCaseAction = nullptr;
	QAction* m_ProperCaseBlendAction = nullptr;
	QAction* m_SentencecaseAction = nullptr;
	QAction* m_SentencecaseBlendAction = nullptr;
	QAction* m_iNVERTcASEAction = nullptr;
	QAction* m_ranDOmCasEAction = nullptr;
	QMenu* m_lineOperationsMenu = nullptr;
	QAction* m_duplicateCurrentLineAction = nullptr;
	QAction* m_removeDuplicateLinesAction = nullptr;
	QAction* m_removeConsecutiveDuplicateLinesAction = nullptr;
	QAction* m_splitLinesAction = nullptr;
	QAction* m_joinLinesAction = nullptr;
	QAction* m_moveUpCurrentLineAction = nullptr;
	QAction* m_moveDownCurrentLineAction = nullptr;
	QAction* m_removeEmptyLinesAction = nullptr;
	QAction* m_removeEmptyLinesBlankAction = nullptr;
	QAction* m_insertBlankLineAboveCurrentAction = nullptr;
	QAction* m_insertBlankLineBelowCurrentAction = nullptr;
	QAction* m_reverseLineOrderAction = nullptr;
	QAction* m_randomizeLineOrderAction = nullptr;
	QAction* m_sortLinesLexicographicallyAscendingAction = nullptr;
	QAction* m_sortLinesLexAscendingIgnoringCaseAction = nullptr;
	QAction* m_sortLinesAsIntegersAscendingAction = nullptr;
	QAction* m_sortLinesAsDecimalsCommaAscendingAction = nullptr;
	QAction* m_sortLinesAsDecimalsDotAscendingAction = nullptr;
	QAction* m_sortLinesLexicographicallyDescendingAction = nullptr;
	QAction* m_sortLinesLexDescendingIgnoringCaseAction = nullptr;
	QAction* m_sortLinesAsIntegersDescendingAction = nullptr;
	QAction* m_sortLinesAsDecimalsCommaDescendingAction = nullptr;
	QAction* m_sortLinesAsDecimalsDotDescendingAction = nullptr;
	QMenu* m_commentMenu = nullptr;
	QAction* m_toggleSingleLineCommentAction = nullptr;
	QAction* m_singleLineCommentAction = nullptr;
	QAction* m_singleLineUncommentAction = nullptr;
	QAction* m_blockCommentAction = nullptr;
	QAction* m_blockUncommentAction = nullptr;
	QMenu* m_autoCompletionMenu = nullptr;
	QAction* m_functionCompletionAction = nullptr;
	QAction* m_wordCompletionAction = nullptr;
	QAction* m_functionParametersHintAction = nullptr;
	QAction* m_functionParametersPreviousHintAction = nullptr;
	QAction* m_functionParametersNextHintAction = nullptr;
	QAction* m_pathCompletion = nullptr;
	QMenu* m_eolConvertMenu = nullptr;
	QAction* m_eolWindowsAction = nullptr;
	QAction* m_eolUnixAction = nullptr;
	QAction* m_eolMacintoshAction = nullptr;
	QMenu* m_blankOperationsMenu = nullptr;
	QAction* m_trimTrailingSpaceAction = nullptr;
	QAction* m_trimLeadingSpaceAction = nullptr;
	QAction* m_trimLeadingAndTrailingSpaceAction = nullptr;
	QAction* m_eolToSpaceAction = nullptr;
	QAction* m_trimBothAndEolToSpaceAction = nullptr;
	QAction* m_tabToSpaceAction = nullptr;
	QAction* m_spaceToTabAllAction = nullptr;
	QAction* m_spaceToTabLeadingAction = nullptr;
	QMenu* m_pasteSpecialMenu = nullptr;
	QMenu* m_onSelectionMenu = nullptr;
	QMenu* m_multiSelectAllMenu = nullptr;
	QMenu* m_multiSelectNextMenu = nullptr;
	QAction* m_columnModeAction = nullptr;
	QAction* m_columnEditorAction = nullptr;
	QAction* m_characterPanelAction = nullptr;
	QAction* m_clipboardHistoryAction = nullptr;
	QAction* m_setReadOnlyAction = nullptr;
	QAction* m_clearReadOnlyFlagAction = nullptr;
	// 菜单栏-查找
	QAction* m_findAction = nullptr;
	QAction* m_findInFilesAction = nullptr;
	QAction* m_findNextAction = nullptr;
	QAction* m_findPreviousAction = nullptr;
	QAction* m_selectAndFindNextAction = nullptr;
	QAction* m_selectAndFindPreviousAction = nullptr;
	QAction* m_findVolatileNextAction = nullptr;
	QAction* m_findVolatilePreviousAction = nullptr;
	QAction* m_replaceAction = nullptr;
	QAction* m_incrementalSearchAction = nullptr;
	QAction* m_searchResultsWindowAction = nullptr;
	QAction* m_nextSearchResultAction = nullptr;
	QAction* m_previousSearchResultAction = nullptr;
	QAction* m_gotoAction = nullptr;
	QAction* m_gotoMatchingBraceAction = nullptr;
	QAction* m_selectAllBetweenMatchingBracesAction = nullptr;
	QAction* m_markAction = nullptr;
	QMenu* m_styleAllOccurrencesOfTokenMenu = nullptr;
	QMenu* m_styleOneTokenMenu = nullptr;
	QMenu* m_clearStyleMenu = nullptr;
	QMenu* m_jumpUpMenu = nullptr;
	QMenu* m_jumpDownMenu = nullptr;
	QMenu* m_copyStyledTextMenu = nullptr;
	QMenu* m_bookMarkMenu = nullptr;
	QAction* m_findCharactersInRangeAction = nullptr;
	// 菜单栏-视图
	QAction* m_alwaysOnTopAction = nullptr;
	QAction* m_toggleFullScreenModeAction = nullptr;
	QAction* m_postItAction = nullptr;
	QAction* m_distractionFreeModeAction = nullptr;
	QMenu* m_viewCurrentFileInMenu = nullptr;
	QMenu* m_showSymbolMenu = nullptr;
	QMenu* m_zoomMenu = nullptr;
	QMenu* m_moveCloneCurrentDocumentMenu = nullptr;
	QMenu* m_tabMenu = nullptr;
	QAction* m_wordWrapAction = nullptr;
	QAction* m_focusOnAnotherViewAction = nullptr;
	QAction* m_hideLinesAction = nullptr;
	QAction* m_foldAllAction = nullptr;
	QAction* m_unfoldAllAction = nullptr;
	QAction* m_foldCurrentLevelAction = nullptr;
	QAction* m_unfoldCurrentLevelAction = nullptr;
	QMenu* m_foldLevelMenu = nullptr;
	QMenu* m_unfoldLevelMenu = nullptr;
	QAction* m_summaryAction = nullptr;
	QMenu* m_projectMenu = nullptr;
	QAction* m_folderAsWorkspaceAction = nullptr;
	QAction* m_documentMapAction = nullptr;
	QAction* m_documentListAction = nullptr;
	QAction* m_synchroniseVerticalScrollingAction = nullptr;
	QAction* m_synchroniseHorizontalScrollingAction = nullptr;
	QAction* m_textDirectionRTLAction = nullptr;
	QAction* m_textDirectionLTRAction = nullptr;
	QAction* m_monitoringAction = nullptr;
	// 菜单栏-编码
	QAction* m_anslAction = nullptr;
	// 菜单栏-语言
	QAction* m_noneAction = nullptr;
	// 菜单栏-设置
	QAction* m_preferencesAction = nullptr;
	// 菜单栏-工具
	QMenu* m_md5Menu = nullptr;
	// 菜单栏-宏
	QAction* m_startRecordingAction = nullptr;
	// 菜单栏-运行
	QAction* m_runAction = nullptr;
	// 菜单栏-插件
	QAction* m_openPluginsFolderAction = nullptr;
	// 菜单栏-窗口
	QMenu* m_sortByMenu = nullptr;
	// 菜单栏-帮助
	QAction* m_commandLineArgumentsAction = nullptr;
	QAction* m_aboutQt = nullptr;
	QAction* m_aboutOneNotePad = nullptr;
	QAction* m_debugInfoAction = nullptr;
};
