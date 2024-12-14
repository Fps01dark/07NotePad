#pragma once

#include <QToolBar>

class MessageBus;

class CustomToolBar : public QToolBar {
   public:
    explicit CustomToolBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
    ~CustomToolBar();

   private:
    void InitUi();
    void InitValue();
    void InitConnect();

   private:
    std::shared_ptr<MessageBus> m_messageBus = nullptr;
    // 工具栏
    QAction* m_newAction = nullptr;
    QAction* m_openAction = nullptr;
    QAction* m_saveAction = nullptr;
    QAction* m_saveAllAction = nullptr;
    QAction* m_closeAction = nullptr;
    QAction* m_closeAllAction = nullptr;
    QAction* m_printAction = nullptr;
};
