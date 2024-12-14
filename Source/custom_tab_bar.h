#pragma once

#include <QTabBar>

class MessageBus;

class CustomTabBar : public QTabBar {
    Q_OBJECT
   public:
    explicit CustomTabBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
    ~CustomTabBar();

   protected:
    // 重写 contextMenuEvent 来显示右键菜单
    void contextMenuEvent(QContextMenuEvent* event) override;

   private:
    void InitUi();
    void InitValue();
    void InitConnect();

   private:
    std::shared_ptr<MessageBus> m_messageBus = nullptr;
    QMenu*                      m_itemMenu = nullptr;
    QAction*                    m_closeAction = nullptr;
    QMenu*                      m_closeMenu = nullptr;
    QAction*                    m_closeAllButCurrentAction = nullptr;
    QAction*                    m_closeLeftAction = nullptr;
    QAction*                    m_closeRightAction = nullptr;
    QAction*                    m_closeAllUnchangeAction = nullptr;
    QAction*                    m_saveAction = nullptr;
    QAction*                    m_saveAsAction = nullptr;
    QMenu*                      m_openFolderMenu = nullptr;
    QAction*                    m_explorerAction = nullptr;
    QAction*                    m_cmdAction = nullptr;
    QAction*                    m_folderAsWorkspace = nullptr;
    QAction*                    m_defaultViewerAction = nullptr;
    QAction*                    m_renameAction = nullptr;
    QAction*                    m_deleteAction = nullptr;
    QAction*                    m_reloadAction = nullptr;
    QAction*                    m_printAction = nullptr;
    QAction*                    m_readOnlyAction = nullptr;
    QAction*                    m_clearReadOnlyAction = nullptr;
    QMenu*                      m_copyToClipboardMenu = nullptr;
    QAction*                    m_copyPathAction = nullptr;
    QAction*                    m_copyNameAction = nullptr;
    QAction*                    m_copyDirAction = nullptr;
    QMenu*                      m_moveFileMenu = nullptr;
    QAction*                    m_moveToStartAction = nullptr;
    QAction*                    m_moveToEndAction = nullptr;
    QAction*                    m_moveToOtherViewAction = nullptr;
    QAction*                    m_cloneToOtherViewAction = nullptr;
    QAction*                    m_moveToNewInstanceAction = nullptr;
    QAction*                    m_openInNewInstanceAction = nullptr;
    QMenu*                      m_colorTabMenu = nullptr;
    QAction*                    m_color1Action = nullptr;
    QAction*                    m_color2Action = nullptr;
    QAction*                    m_color3Action = nullptr;
    QAction*                    m_color4Action = nullptr;
    QAction*                    m_color5Action = nullptr;
    QAction*                    m_removeColorAction = nullptr;
};
