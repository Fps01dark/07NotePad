#include "custom_tool_bar.h"

#include "framework.h"

#include "message_bus.h"

CustomToolBar::CustomToolBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	:m_messageBus(message_bus),
	QToolBar(parent)
{
	InitUi();
	InitValue();
	InitConnect();
}

CustomToolBar::~CustomToolBar()
{
}

void CustomToolBar::InitUi()
{
	m_newAction = addAction(QIcon(":/NotePad/standard/toolbar/newFile.bmp"), tr("New"));
	m_openAction = addAction(QIcon(":/NotePad/standard/toolbar/openFile.bmp"), tr("Open"));
	m_saveAction = addAction(QIcon(":/NotePad/standard/toolbar/saveFile.bmp"), tr("Save"));
	m_saveAllAction = addAction(QIcon(":/NotePad/standard/toolbar/saveAll.bmp"), tr("Save All"));
	m_closeAction = addAction(QIcon(":/NotePad/standard/toolbar/closeFile.bmp"), tr("Close"));
	m_closeAllAction = addAction(QIcon(":/NotePad/standard/toolbar/closeAll.bmp"), tr("Close All"));
	m_printAction = addAction(QIcon(":/NotePad/standard/toolbar/print.bmp"), tr("Print"));
	addSeparator();
}

void CustomToolBar::InitValue()
{
}

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
	connect(m_closeAllAction, &QAction::triggered, [=]()
		{
			m_messageBus->Publish("Close All File");
		});
}