#pragma once

#include <QToolBar>

class MessageBus;

class CustomToolBar : public QToolBar
{
public:
	explicit CustomToolBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~CustomToolBar();

private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
	// 工具栏-文件
	QAction* m_newAction = nullptr;
	QAction* m_openAction = nullptr;
	QAction* m_saveAction = nullptr;
	QAction* m_saveAllAction = nullptr;
	QAction* m_closeAction = nullptr;
	QAction* m_closeAllAction = nullptr;
	QAction* m_printAction = nullptr;
	// 工具栏-编辑
	QAction* m_cutAction = nullptr;
	QAction* m_copyAction = nullptr;
	QAction* m_pasteAction = nullptr;
	QAction* m_undoAction = nullptr;
	QAction* m_redoAction = nullptr;
	// 工具栏-搜索
	QAction* m_findAction = nullptr;
	QAction* m_replaceAction = nullptr;
	// 工具栏-视图
	QAction* m_zoomInAction = nullptr;
	QAction* m_zoomOutAction = nullptr;
	QAction* m_syncVerticalScrollingAction = nullptr;
	QAction* m_syncHorizontalScrollingAction = nullptr;
	QAction* m_wordWrapAction = nullptr;
	QAction* m_showAllCharactersAction = nullptr;
	QAction* m_showIndentGuideAction = nullptr;
	QAction* m_defineYourLanguageAction = nullptr;
	QAction* m_documentMapAction = nullptr;
	QAction* m_documentListAction = nullptr;
	QAction* m_functionListAction = nullptr;
	QAction* m_folderAsWorkspaceAction = nullptr;
	QAction* m_monitoringAction = nullptr;
	// 工具栏-宏
	QAction* m_startRecordingAction = nullptr;
	QAction* m_stopRecordingAction = nullptr;
	QAction* m_playbackAction = nullptr;
	QAction* m_runMacroMultipleTimesAction = nullptr;
	QAction* m_saveCurrentRecordedMacroAction = nullptr;
};
