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

void CustomToolBar::InitUi() {
    m_newAction = addAction(QIcon(":/OneNotePad/standard/toolbar/newFile.bmp"), tr("New"));
    m_openAction = addAction(QIcon(":/OneNotePad/standard/toolbar/openFile.bmp"), tr("Open"));
    m_saveAction = addAction(QIcon(":/OneNotePad/standard/toolbar/saveFile.bmp"), tr("Save"));
    m_saveAllAction = addAction(QIcon(":/OneNotePad/standard/toolbar/saveAll.bmp"), tr("Save All"));
    m_closeAction = addAction(QIcon(":/OneNotePad/standard/toolbar/closeFile.bmp"), tr("Close"));
    m_closeAllAction =
        addAction(QIcon(":/OneNotePad/standard/toolbar/closeAll.bmp"), tr("Close All"));
    m_printAction = addAction(QIcon(":/OneNotePad/standard/toolbar/print.bmp"), tr("Print"));
    addSeparator();
}

void CustomToolBar::InitValue() {}

void CustomToolBar::InitConnect() {
    connect(m_newAction, &QAction::triggered, [=]() { m_messageBus->Publish("New File"); });

    connect(m_openAction, &QAction::triggered, [=]() { m_messageBus->Publish("Open File"); });
    connect(m_saveAction, &QAction::triggered, [=]() { m_messageBus->Publish("Save File"); });
    connect(m_saveAllAction, &QAction::triggered,
            [=]() { m_messageBus->Publish("Save All File"); });
    connect(m_closeAction, &QAction::triggered, [=]() { m_messageBus->Publish("Close File"); });
    connect(m_closeAllAction, &QAction::triggered,
            [=]() { m_messageBus->Publish("Close All File"); });
}