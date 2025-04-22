#include "custom_tool_bar.h"

#include "message_bus.h"

CustomToolBar::CustomToolBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	: m_messageBus(message_bus), QToolBar(parent) {
	InitUi();
	InitValue();
	InitConnect();
}

CustomToolBar::~CustomToolBar() {}

void CustomToolBar::InitUi()
{
	m_newAction = addAction(QIcon(":/Icons/Icons/newfile.png"), tr("New"));
	m_openAction = addAction(QIcon(":/Icons/Icons/openFile.png"), tr("Open"));
	m_saveAction = addAction(QIcon(":/Icons/Icons/saved.png"), tr("Save"));
	m_saveAllAction = addAction(QIcon(":/Icons/Icons/saveAll.png"), tr("Save All"));
	m_closeAction = addAction(QIcon(":/Icons/Icons/closeFile.png"), tr("Close"));
	m_closeAllAction = addAction(QIcon(":/Icons/Icons/closeAll.png"), tr("Close All"));
	m_printAction = addAction(QIcon(":/Icons/Icons/printer.png"), tr("Print"));
	addSeparator();
	m_cutAction = addAction(QIcon(":/Icons/Icons/cut.png"), tr("Cut"));
	m_copyAction = addAction(QIcon(":/Icons/Icons/copy.png"), tr("Copy"));
	m_pasteAction = addAction(QIcon(":/Icons/Icons/paste.png"), tr("Paste"));
	addSeparator();
	m_undoAction = addAction(QIcon(":/Icons/Icons/undo.png"), tr("Undo"));
	m_redoAction = addAction(QIcon(":/Icons/Icons/redo.png"), tr("Redo"));
	addSeparator();
	m_findAction = addAction(QIcon(":/Icons/Icons/find.png"), tr("Find"));
	m_replaceAction = addAction(QIcon(":/Icons/Icons/findReplace.png"), tr("Replace"));
	addSeparator();
	m_zoomInAction = addAction(QIcon(":/Icons/Icons/zoomIn.png"), tr("Zoom In"));
	m_zoomOutAction = addAction(QIcon(":/Icons/Icons/zoomOut.png"), tr("Zoom Out"));
	addSeparator();
	m_wordWrapAction = addAction(QIcon(":/Icons/Icons/wrap.png"), tr("Word Wrap"));
	m_showAllCharactersAction = addAction(QIcon(":/Icons/Icons/invisibleChar.png"), tr("Show All Characters"));
	m_showIndentGuideAction = addAction(QIcon(":/Icons/Icons/indentGuide.png"), tr("Show Indent Guide"));
	m_folderAsWorkspaceAction = addAction(QIcon(":/Icons/Icons/folder_go.png"), tr("Folder As Workspace"));
	addSeparator();
	addSeparator();
	m_startRecordingAction = addAction(QIcon(":/Icons/Icons/startRecord.png"), tr("Start Recording"));
	m_stopRecordingAction = addAction(QIcon(":/Icons/Icons/stopRecord.png"), tr("Stop Recording"));
	m_playbackAction = addAction(QIcon(":/Icons/Icons/playRecord.png"), tr("Playback"));
	m_runMacroMultipleTimesAction = addAction(QIcon(":/Icons/Icons/playRecord_m.png"), tr("Run A Macro Multiple Times"));
	m_saveCurrentRecordedMacroAction = addAction(QIcon(":/Icons/Icons/saveRecord.png"), tr("Save Current Recorded Macro"));
}

void CustomToolBar::InitValue() {}

void CustomToolBar::InitConnect()
{
	connect(m_newAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("New File");
		});
	connect(m_openAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Open File");
		});
	connect(m_saveAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Save File");
		});
	connect(m_saveAllAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Save All File");
		});
	connect(m_closeAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Close File");
		});
	connect(m_closeAllAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Close All File");
		});
}