#pragma once

#include <QMenuBar>

class QMenu;
class QAction;
class MessageBus;

class CustomMenuBar : public QMenuBar {
    Q_OBJECT
   public:
    explicit CustomMenuBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
    ~CustomMenuBar();
    void        SetRecentFiles(const QStringList& recent_list);
    QStringList GetRecentFiles() const;

   private:
    void InitUi();
    void InitValue();
    void InitConnect();

   private:
    std::shared_ptr<MessageBus> m_messageBus = nullptr;
    // 菜单栏-文件
    QAction*    m_newAction = nullptr;
    QAction*    m_openAction = nullptr;
    QMenu*      m_openFolderMenu = nullptr;
    QAction*    m_explorerAction = nullptr;
    QAction*    m_cmdAction = nullptr;
    QAction*    m_folderAsWorkspace = nullptr;
    QAction*    m_defaultViewerAction = nullptr;
    QAction*    m_openFolderAsWorkspaceAction = nullptr;
    QAction*    m_reloadAction = nullptr;
    QAction*    m_saveAction = nullptr;
    QAction*    m_saveAsAction = nullptr;
    QAction*    m_copySaveAsAction = nullptr;
    QAction*    m_saveAllAction = nullptr;
    QAction*    m_renameAction = nullptr;
    QAction*    m_closeAction = nullptr;
    QAction*    m_closeAllAction = nullptr;
    QMenu*      m_closeMenu = nullptr;
    QAction*    m_closeAllButCurrentAction = nullptr;
    QAction*    m_closeLeftAction = nullptr;
    QAction*    m_closeRightAction = nullptr;
    QAction*    m_closeAllUnchangeAction = nullptr;
    QAction*    m_deleteFileAction = nullptr;
    QAction*    m_loadSessionAction = nullptr;
    QAction*    m_saveSessionAction = nullptr;
    QAction*    m_printAction = nullptr;
    QAction*    m_printNowAction = nullptr;
    QMenu*      m_recentFileMenu = nullptr;
    QStringList m_recentFiles;
    QAction*    m_openAllRecentAction = nullptr;
    QAction*    m_clearRecentAction = nullptr;
    QAction*    m_exitSotfwareAction = nullptr;
    // 菜单栏-编辑
    QAction* m_undoAction = nullptr;
    QAction* m_redoAction = nullptr;
    QAction* m_cutAction = nullptr;
    QAction* m_copyAction = nullptr;
    QAction* m_pasteAction = nullptr;
    QAction* m_deleteTextAction = nullptr;
    QAction* m_selectAllAction = nullptr;
    QMenu*   m_insertMenu = nullptr;
    QAction* m_shortTimeAction = nullptr;
    QAction* m_longTimeAction = nullptr;
    QAction* m_customTimeAction = nullptr;
    QMenu*   m_copyToClipboardMenu = nullptr;
    QAction* m_copyFilePathAction = nullptr;
    QAction* m_copyFileNameAction = nullptr;
    QAction* m_copyFileDirAction = nullptr;
    QAction* m_copyAllFileNameAction = nullptr;
    QAction* m_copyAllFilePathAction = nullptr;
    QMenu*   m_indentMenu = nullptr;
    QAction* m_insertLineIndentAction = nullptr;
    QAction* m_deleteLineIndentAction = nullptr;
    // ...
};
