#pragma once

#include <QObject>
#include <QSet>

class QString;

class MessageBus;
class OnSettings;
class OnMainWindow;
class OnMenuBar;
class OnToolBar;
class OnTabBar;
class OnTextEdit;
class OnTabWidget;
class OnStatusBar;
class DirWorkspaceDock;

class OnCore : public QObject
{
	Q_OBJECT
public:
	explicit OnCore(OnMainWindow* main_window);
	~OnCore();
	void ExitSoftware();

private:
	void InitUi();
	void InitValue();
	void InitConnect();
	void UpdateWindowTitle();
	// 文件
	bool NewFile(const QString& new_file_name);
	bool OpenFile(const QString& file_path);
	bool SaveFile(int index, const QString& file_path);
	bool CloseFile(int index);
	// 加载上次退出文件及状态
	bool LoadSettings();
	bool SaveSettings();

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
	OnSettings* m_settings = nullptr;
	OnMainWindow* m_mainWindow = nullptr;
	OnMenuBar* m_menuBar = nullptr;
	OnToolBar* m_toolBar = nullptr;
	OnTabBar* m_tabBar = nullptr;
	OnTabWidget* m_centralWidget = nullptr;
	OnStatusBar* m_statusBar = nullptr;
	DirWorkspaceDock* m_dirWorkSpace = nullptr;
	int m_fontSize = 9;
	// 下标为 m_centralWidget Tab下标
	QList<OnTextEdit*> m_textWidget;
};
