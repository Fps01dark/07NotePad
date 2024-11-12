#pragma once

#include <QObject>
#include <QSet>

class QString;

class MessageBus;
class MainWindow;
class CustomMenuBar;
class CustomToolBar;
class CustomTabBar;
class CustomTextEdit;
class CustomTabWidget;
class DirWorkspaceDock;

class MainCore :public QObject
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
	void NewFile();
	void OpenFile(const QStringList& file_path);
	bool SaveFile(int index, const QString& file_path);
	void CloseFile(int index);
	bool DeleteFile(int index);
	void ReloadFile();
	void OpenExplorer(const QString& file_dir);
	void OpenCmd(const QString& file_dir);
	void OpenInDefaultViewer(const QString& file_path);

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
	MainWindow* m_mainWindow = nullptr;
	CustomMenuBar* m_menuBar = nullptr;
	CustomToolBar* m_toolBar = nullptr;
	CustomTabBar* m_tabBar = nullptr;
	CustomTabWidget* m_centralWidget = nullptr;
	DirWorkspaceDock* m_dirWorkSpace = nullptr;
	// 以下数量保持一致，下标为 m_centralWidget Tab下标
	QList<QString> m_openedFileName;
	QList<QString> m_openedFilePath;
	QList<bool> m_savedFile;
	QList<CustomTextEdit*> m_textWidget;
};
