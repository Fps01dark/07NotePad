#pragma once

#include <QObject>
#include <QSet>

class QString;

class MessageBus;
class CustomSettings;
class MainWindow;
class CustomMenuBar;
class CustomToolBar;
class CustomTabBar;
class CustomTextEdit;
class CustomTabWidget;
class DirWorkspaceDock;

class MainCore : public QObject
{
	Q_OBJECT
public:
	explicit MainCore(MainWindow* main_window);
	~MainCore();
	void ExitSoftware();

private:
	void InitUi();
	void InitValue();
	void InitConnect();
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
	CustomSettings* m_settings = nullptr;
	MainWindow* m_mainWindow = nullptr;
	CustomMenuBar* m_menuBar = nullptr;
	CustomToolBar* m_toolBar = nullptr;
	CustomTabBar* m_tabBar = nullptr;
	CustomTabWidget* m_centralWidget = nullptr;
	DirWorkspaceDock* m_dirWorkSpace = nullptr;
	int m_fontSize = 9;
	// 下标为 m_centralWidget Tab下标
	QList<CustomTextEdit*> m_textWidget;
};
