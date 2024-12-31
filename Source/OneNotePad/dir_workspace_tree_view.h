#pragma once

#include <QTreeView>

class QMouseEvent;
class QFileSystemModel;
class MessageBus;

class DirWorkspaceTreeView : public QTreeView {
    Q_OBJECT
   public:
    explicit DirWorkspaceTreeView(std::shared_ptr<MessageBus> message_bus, QFileSystemModel* model,
                                  QWidget* parent = nullptr);
    ~DirWorkspaceTreeView();
    void   SetMenu(const QMenu* menu);
    QMenu* GetMenu() const;

   protected:
    // 重写 contextMenuEvent 来显示右键菜单
    void contextMenuEvent(QContextMenuEvent* event) override;

   private:
    void InitUi();
    void InitValue();
    void InitConnect();

   private:
    std::shared_ptr<MessageBus> m_messageBus = nullptr;
    QFileSystemModel*           m_fileSystemModel = nullptr;
    QMenu*                      m_itemMenu = nullptr;
    QAction*                    m_openAction = nullptr;
    QAction*                    m_copyPathAction = nullptr;
    QAction*                    m_copyNameAction = nullptr;
    QAction*                    m_runBySystemAction = nullptr;
    QAction*                    m_explorerHereAction = nullptr;
    QAction*                    m_cmdHereAction = nullptr;
};
