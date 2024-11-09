#include "main_core.h"

#include "framework.h"

#include "message_bus.h"
#include "main_window.h"
#include "custom_menu_bar.h"
#include "custom_tool_bar.h"
#include "custom_tab_bar.h"
#include "custom_tab_widget.h"
#include "text_widget.h"
#include "folder_workspace_dock_widget.h"

MainCore::MainCore(MainWindow* main_window)
	:m_mainWindow(main_window),
	QObject(main_window)
{
	m_messageBus = std::make_shared<MessageBus>();
	m_menuBar = new CustomMenuBar(m_messageBus, m_mainWindow);
	m_toolBar = new CustomToolBar(m_messageBus, m_mainWindow);
	m_centralWidget = new CustomTabWidget(m_messageBus, m_mainWindow);
	m_tabBar = new CustomTabBar(m_messageBus, m_mainWindow);
	m_dirWorkSpace = new FolderWorkspaceDockWidget(m_messageBus, m_mainWindow);

	InitUi();
	InitValue();
	InitConnect();
}

MainCore::~MainCore()
{
}

void MainCore::InitUi()
{
	// 工具栏
	m_toolBar->setMovable(false);
	m_toolBar->setWindowTitle("Tool Bar");
	// 标签栏
	m_tabBar->setTabsClosable(true);
	m_tabBar->setMovable(true);
	m_tabBar->setStyleSheet(
		"QTabBar::close-button { image: url(../Resources/standard/tabbar/closeTabButton.ico); }"
		"QTabBar::close-button:hover { image: url(../Resources/standard/tabbar/closeTabButton_hover.ico); }"
		"QTabBar::close-button:pressed { image: url(../Resources/standard/tabbar/closeTabButton_push.ico); }"
	);
	// 中心区域
	m_centralWidget->SetTabBar(m_tabBar);
	// 目录工作区
	m_dirWorkSpace->setWindowTitle("Directory Workspace");
	m_dirWorkSpace->hide();

	// 主界面
	m_mainWindow->setWindowTitle("NotePad");
	m_mainWindow->setMenuBar(m_menuBar);
	m_mainWindow->addToolBar(Qt::TopToolBarArea, m_toolBar);
	m_mainWindow->setCentralWidget(m_centralWidget);
	m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_dirWorkSpace);
}

void MainCore::InitValue()
{
	//std::function<void(const std::string&)> handler
	m_messageBus->Subscribe("Update Window Title", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index < 0)
			{
				m_mainWindow->setWindowTitle("NotePad");
			}
			else
			{
				QString win_title;
				if (index < m_openedFilePath.size())
				{
					win_title = m_openedFilePath[index];
				}
				if (win_title.isEmpty() && index < m_openedFileName.size())
				{
					win_title = m_openedFileName[index];
				}
				m_mainWindow->setWindowTitle(win_title + " - NotePad");
			}
		});
	m_messageBus->Subscribe("New File", [=]()
		{
			NewFile();
		});
	m_messageBus->Subscribe("Open File", [=]()
		{
			OpenFile();
		});
	m_messageBus->Subscribe("Open Explorer", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index < 0)
			{
			}
			else
			{
				QString file_path;
				if (index < m_openedFilePath.size())
				{
					file_path = m_openedFilePath[index];
					QFileInfo file_info(file_path);
					QDesktopServices::openUrl(QUrl::fromLocalFile(file_info.absolutePath()));
				}
			}
		});
	m_messageBus->Subscribe("Open Cmd", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index < 0)
			{
			}
			else
			{
				QString file_path;
				if (index < m_openedFilePath.size())
				{
					file_path = m_openedFilePath[index];
					QFileInfo file_info(file_path);
					QString file_dir = file_info.absolutePath();
					// 启动命令行窗口并进入文件所在目录
#ifdef Q_OS_WIN
	// Windows: 使用 cmd 打开并切换到指定目录
					QProcess::startDetached("cmd.exe", QStringList() << "/K" << "cd" << file_dir);
#elif defined(Q_OS_MAC)
	// macOS: 使用 Terminal 打开并切换到指定目录
					QProcess::startDetached("open", QStringList() << "-a" << "Terminal" << file_dir);
#elif defined(Q_OS_LINUX)
	// Linux: 使用终端打开并切换到指定目录
					QProcess::startDetached("gnome-terminal", QStringList() << "--working-directory=" + file_dir);
#endif
				}
			}
		});
	m_messageBus->Subscribe("Open Directory Workspace", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index < 0)
			{
			}
			else
			{
				QString file_path;
				if (index < m_openedFilePath.size())
				{
					file_path = m_openedFilePath[index];
					QFileInfo file_info(file_path);
					m_dirWorkSpace->SetRootDir(file_info.absolutePath());
					m_dirWorkSpace->show();
				}
			}
		});
	m_messageBus->Subscribe("Open Directory As Directory Workspace", [=]()
		{
			QString file_dir = QFileDialog::getExistingDirectory(m_mainWindow, tr("Open Directory As Directory Workspace"), qApp->applicationDirPath());
			m_dirWorkSpace->SetRootDir(file_dir);
			m_dirWorkSpace->show();
		});
	m_messageBus->Subscribe("Open In Default Viewer", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index < 0)
			{
			}
			else
			{
				QString file_path;
				if (index < m_openedFilePath.size())
				{
					file_path = m_openedFilePath[index];
					QFileInfo file_info(file_path);
					QDesktopServices::openUrl(QUrl::fromLocalFile(file_info.absoluteFilePath()));
				}
			}
		});
	m_messageBus->Subscribe("Open File", [=](const QStringList& data)
		{
			OpenFile(data);
		});
	m_messageBus->Subscribe("Save File", [=]()
		{
			SaveFile();
		});
	m_messageBus->Subscribe("Save As File", [=]()
		{
			SaveAsFile();
		});
	m_messageBus->Subscribe("Save All File", [=]()
		{
			SaveAllFile();
		});
	m_messageBus->Subscribe("Save As Clipboard", [=]()
		{
			SaveAsClipboard();
		});
	m_messageBus->Subscribe("Close File", [=]()
		{
			CloseFile();
		});
	m_messageBus->Subscribe("Close File", [=](int index)
		{
			CloseFile(index);
		});
	m_messageBus->Subscribe("Close All File", [=]()
		{
			CloseAllFile();
		});
	m_messageBus->Subscribe("Reload File", [=]()
		{
			ReloadFile();
		});
	m_messageBus->Subscribe("Clear History Record", [=]()
		{
			m_menuBar->ClearHistoryRecord();
		});
	m_messageBus->Subscribe("Exit Software", [=]()
		{
			m_mainWindow->close();
		});

	m_messageBus->Subscribe("Expand All", [=]()
		{
			m_dirWorkSpace->ExpandAll();
		});
	m_messageBus->Subscribe("Collapse All", [=]()
		{
			m_dirWorkSpace->CollapseAll();
		});
	m_messageBus->Subscribe("Locate The Current File", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index < 0)
			{
			}
			else
			{
				QString file_path;
				if (index < m_openedFilePath.size())
				{
					file_path = m_openedFilePath[index];
					QFileInfo file_info(file_path);
					m_dirWorkSpace->LocationFile(file_info.absoluteFilePath());
				}
			}
		});
}

void MainCore::InitConnect()
{
	connect(m_centralWidget, &CustomTabWidget::currentChanged, [=]()
		{
			m_messageBus->Publish("Update Window Title");
		});
	connect(m_centralWidget, &CustomTabWidget::tabCloseRequested, [=](int index)
		{
			m_messageBus->Publish("Close File", index);
		});
}

void MainCore::NewFile()
{
	int index = 0;
	QString new_file_name = tr("new ") + QString::number(index);
	while (m_openedFileName.contains(new_file_name))
	{
		index++;
		new_file_name = tr("new ") + QString::number(index);
	}
	TextWidget* text_widget = new TextWidget(m_mainWindow);
	m_openedFileName.append(new_file_name);
	m_openedFilePath.append("");
	m_textWidget.append(text_widget);
	m_centralWidget->addTab(text_widget, QIcon(":/NotePad/standard/tabbar/saved.ico"), new_file_name);
}

void MainCore::OpenFile()
{
	QStringList&& file_paths = QFileDialog::getOpenFileNames(m_mainWindow, tr("Open"), qApp->applicationDirPath(), "All types(*.*)");
	OpenFile(file_paths);
}

void MainCore::OpenFile(const QStringList& file_paths)
{
	for (const QString& file_path : file_paths)
	{
		if (file_path.isEmpty() || !QFileInfo::exists(file_path))
		{
			continue;
		}

		QFileInfo file_info(file_path);
		QString abs_file_path = file_info.absoluteFilePath();
		int index = m_openedFilePath.indexOf(abs_file_path);
		if (index != -1)
		{
			m_centralWidget->setCurrentIndex(index);
		}
		else
		{
			QFile file(file_path);
			if (file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				QTextStream in(&file);
				in.setEncoding(QStringConverter::Utf8);
				TextWidget* text_widget = new TextWidget(m_mainWindow);
				text_widget->SetText(in.readAll());
				m_openedFileName.append(file_info.fileName());
				m_openedFilePath.append(abs_file_path);
				m_textWidget.append(text_widget);
				m_centralWidget->addTab(text_widget, QIcon(":/NotePad/standard/tabbar/saved.ico"), file_info.fileName());
				m_centralWidget->setCurrentIndex(m_centralWidget->count() - 1);
				file.close();
			}
		}
	}
}

void MainCore::SaveFile()
{
	int index = m_centralWidget->currentIndex();
	if (index < 0)
	{
		return;
	}

	QString file_path = m_openedFilePath[index];
	if (file_path.isEmpty() || !QFileInfo::exists(file_path))
	{
		file_path = QFileDialog::getSaveFileName(m_mainWindow, "保存文件\"" + m_openedFileName[index] + "\"", qApp->applicationDirPath() + "/" + m_openedFileName[index], "Text files(*.txt);;All types(*.*)");
	}
	if (!file_path.isEmpty())
	{
		// 保存
		QFile file(file_path);
		if (file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QTextStream in(&file);
			in.setEncoding(QStringConverter::Utf8);
			in << m_textWidget[index]->GetText();
			file.close();
		}
		// 保存后处理
		QFileInfo file_info(file_path);
		m_openedFileName[index] = file_info.fileName();
		m_openedFilePath[index] = file_info.absoluteFilePath();
		m_centralWidget->setTabText(index, file_info.fileName());
		m_messageBus->Publish("Update Window Title");
	}
}

void MainCore::SaveAsFile()
{
	int index = m_centralWidget->currentIndex();
	if (index < 0)
	{
		return;
	}

	QString file_path = QFileDialog::getSaveFileName(m_mainWindow, "另存为文件...", qApp->applicationDirPath() + "/" + m_openedFileName[index], "Text files(*.txt);;All types(*.*)");
	if (!file_path.isEmpty())
	{
		// 保存
		QFile file(file_path);
		if (file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QTextStream in(&file);
			in.setEncoding(QStringConverter::Utf8);
			in << m_textWidget[index]->GetText();
			file.close();
		}
		// 保存后处理
		QFileInfo file_info(file_path);
		m_openedFileName[index] = file_info.fileName();
		m_openedFilePath[index] = file_info.absoluteFilePath();
		m_centralWidget->setTabText(index, file_info.fileName());
		m_messageBus->Publish("Update Window Title");
	}
}

void MainCore::SaveAllFile()
{
	for (int i = 0; i < m_centralWidget->count(); ++i)
	{
		QString file_path = m_openedFilePath[i];
		if (file_path.isEmpty() || !QFileInfo::exists(file_path))
		{
			file_path = QFileDialog::getSaveFileName(m_mainWindow, "保存文件\"" + m_openedFileName[i] + "\"", qApp->applicationDirPath() + "/" + m_openedFileName[i], "Text files(*.txt);;All types(*.*)");
		}
		if (!file_path.isEmpty())
		{
			// 保存
			QFile file(file_path);
			if (file.open(QIODevice::WriteOnly | QIODevice::Text))
			{
				QTextStream in(&file);
				in.setEncoding(QStringConverter::Utf8);
				in << m_textWidget[i]->GetText();
				file.close();
			}
			// 保存后处理
			QFileInfo file_info(file_path);
			m_openedFileName[i] = file_info.fileName();
			m_openedFilePath[i] = file_info.absoluteFilePath();
			m_centralWidget->setTabText(i, file_info.fileName());
			m_messageBus->Publish("Update Window Title");
		}
	}
}

void MainCore::SaveAsClipboard()
{
	QClipboard* clipboard = QApplication::clipboard();
	QString file_path = QFileDialog::getSaveFileName(m_mainWindow, "拷贝另存为文件...", qApp->applicationDirPath(), "Text files(*.txt);;All types(*.*)");
	if (!file_path.isEmpty())
	{
		// 保存
		QFile file(file_path);
		if (file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QTextStream in(&file);
			in.setEncoding(QStringConverter::Utf8);
			in << clipboard->text();
			file.close();
		}
	}
}

void MainCore::CloseFile()
{
	int index = m_centralWidget->currentIndex();
	if (index < 0)
	{
		return;
	}
	// 最近文件
	if (!m_openedFilePath[index].isEmpty())
	{
		m_menuBar->AddHistoryRecord(QStringList() << m_openedFilePath[index]);
	}
	// 关闭
	m_openedFileName.removeAt(index);
	m_openedFilePath.removeAt(index);
	m_textWidget.removeAt(index);
	m_centralWidget->removeTab(index);
}

void MainCore::CloseFile(int index)
{
	if (index < 0)
	{
		return;
	}
	// 最近文件
	if (!m_openedFilePath[index].isEmpty())
	{
		m_menuBar->AddHistoryRecord(QStringList() << m_openedFilePath[index]);
	}
	// 关闭
	m_openedFileName.removeAt(index);
	m_openedFilePath.removeAt(index);
	m_textWidget.removeAt(index);
	m_centralWidget->removeTab(index);
}

void MainCore::CloseAllFile()
{
	// 最近文件
	for (int i = 0; i < m_openedFilePath.size(); ++i)
	{
		if (!m_openedFilePath[i].isEmpty())
		{
			m_menuBar->AddHistoryRecord(QStringList() << m_openedFilePath[i]);
		}
	}
	// 关闭
	m_openedFileName.clear();
	m_openedFilePath.clear();
	m_textWidget.clear();
	m_centralWidget->clear();
}

void MainCore::ReloadFile()
{
	int index = m_centralWidget->currentIndex();
	if (index < 0)
	{
		return;
	}
	QString file_path = m_openedFilePath[index];
	if (file_path.isEmpty() || !QFileInfo::exists(file_path))
	{
	}
	else
	{
		QFile file(file_path);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream in(&file);
			in.setEncoding(QStringConverter::Utf8);
			m_textWidget[index]->SetText(in.readAll());
			file.close();
		}
	}
}