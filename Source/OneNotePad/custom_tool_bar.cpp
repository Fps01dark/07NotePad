#include "custom_tool_bar.h"

#include "framework.h"
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
	m_newAction = addAction(QIcon(":/OneNotePad/standard/toolbar/newFile.bmp"), tr("New"));
	m_openAction = addAction(QIcon(":/OneNotePad/standard/toolbar/openFile.bmp"), tr("Open"));
	m_saveAction = addAction(QIcon(":/OneNotePad/standard/toolbar/saveFile.bmp"), tr("Save"));
	m_saveAllAction = addAction(QIcon(":/OneNotePad/standard/toolbar/saveAll.bmp"), tr("Save All"));
	m_closeAction = addAction(QIcon(":/OneNotePad/standard/toolbar/closeFile.bmp"), tr("Close"));
	m_closeAllAction = addAction(QIcon(":/OneNotePad/standard/toolbar/closeAll.bmp"), tr("Close All"));
	m_printAction = addAction(QIcon(":/OneNotePad/standard/toolbar/print.bmp"), tr("Print"));
	addSeparator();
	m_cutAction = addAction(QIcon(":/OneNotePad/standard/toolbar/cut.bmp"), tr("Cut"));
	m_copyAction = addAction(QIcon(":/OneNotePad/standard/toolbar/copy.bmp"), tr("Copy"));
	m_pasteAction = addAction(QIcon(":/OneNotePad/standard/toolbar/paste.bmp"), tr("Paste"));
	addSeparator();
	m_undoAction = addAction(QIcon(":/OneNotePad/standard/toolbar/undo.bmp"), tr("Undo"));
	m_redoAction = addAction(QIcon(":/OneNotePad/standard/toolbar/redo.bmp"), tr("Redo"));
	addSeparator();
	m_findAction = addAction(QIcon(":/OneNotePad/standard/toolbar/find.bmp"), tr("Find"));
	m_replaceAction = addAction(QIcon(":/OneNotePad/standard/toolbar/findReplace.bmp"), tr("Replace"));
	addSeparator();
	m_zoomInAction = addAction(QIcon(":/OneNotePad/standard/toolbar/zoomIn.bmp"), tr("Zoom In"));
	m_zoomOutAction = addAction(QIcon(":/OneNotePad/standard/toolbar/zoomOut.bmp"), tr("Zoom Out"));
	addSeparator();
	m_syncVerticalScrollingAction = addAction(QIcon(":/OneNotePad/standard/toolbar/syncV.bmp"), tr("Sync Vertical Scrolling"));
	m_syncHorizontalScrollingAction = addAction(QIcon(":/OneNotePad/standard/toolbar/syncH.bmp"), tr("Sync Horizontal Scrolling"));
	addSeparator();
	m_wordWrapAction = addAction(QIcon(":/OneNotePad/standard/toolbar/wrap.bmp"), tr("Word Wrap"));
	m_showAllCharactersAction = addAction(QIcon(":/OneNotePad/standard/toolbar/allChars.bmp"), tr("Show All Characters"));
	m_showIndentGuideAction = addAction(QIcon(":/OneNotePad/standard/toolbar/indentGuide.bmp"), tr("Show Indent Guide"));
	addSeparator();
	m_defineYourLanguageAction = addAction(QIcon(":/OneNotePad/standard/toolbar/udl.bmp"), tr("Define Your Language"));
	m_documentMapAction = addAction(QIcon(":/OneNotePad/standard/toolbar/docMap.bmp"), tr("Document Map"));
	m_documentListAction = addAction(QIcon(":/OneNotePad/standard/toolbar/docList.bmp"), tr("Document List"));
	m_functionListAction = addAction(QIcon(":/OneNotePad/standard/toolbar/funcList.bmp"), tr("Function List"));
	m_folderAsWorkspaceAction = addAction(QIcon(":/OneNotePad/standard/toolbar/fileBrowser.bmp"), tr("Folder As Workspace"));
	addSeparator();
	m_monitoringAction = addAction(QIcon(":/OneNotePad/standard/toolbar/monitoring.bmp"), tr("Monitoring"));
	addSeparator();
	m_startRecordingAction = addAction(QIcon(":/OneNotePad/standard/toolbar/startRecord.bmp"), tr("Start Recording"));
	m_stopRecordingAction = addAction(QIcon(":/OneNotePad/standard/toolbar/stopRecord.bmp"), tr("Stop Recording"));
	m_playbackAction = addAction(QIcon(":/OneNotePad/standard/toolbar/playRecord.bmp"), tr("Playback"));
	m_runMacroMultipleTimesAction = addAction(QIcon(":/OneNotePad/standard/toolbar/playRecord_m.bmp"), tr("Run A Macro Multiple Times"));
	m_saveCurrentRecordedMacroAction = addAction(QIcon(":/OneNotePad/standard/toolbar/saveRecord.bmp"), tr("Save Current Recorded Macro"));
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