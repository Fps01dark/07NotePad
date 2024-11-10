#include "main_core.h"

#include "framework.h"

#include "message_bus.h"
#include "main_window.h"
#include "custom_menu_bar.h"
#include "custom_tool_bar.h"
#include "custom_tab_bar.h"
#include "custom_tab_widget.h"
#include "custom_text_edit.h"
#include "directory_workspace_dock.h"

MainCore::MainCore(MainWindow* main_window)
	:m_mainWindow(main_window),
	QObject(main_window)
{
	m_messageBus = std::make_shared<MessageBus>();
	m_menuBar = new CustomMenuBar(m_messageBus, m_mainWindow);
	m_toolBar = new CustomToolBar(m_messageBus, m_mainWindow);
	m_centralWidget = new CustomTabWidget(m_messageBus, m_mainWindow);
	m_tabBar = new CustomTabBar(m_messageBus, m_mainWindow);
	m_dirWorkSpace = new DirectoryWorkspaceDock(m_messageBus, m_mainWindow);

	InitUi();
	InitValue();
	InitConnect();
}

MainCore::~MainCore()
{
}

void MainCore::ExitSoftware()
{
	m_messageBus->Publish("Exit Software");
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
	// Base
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
			QStringList&& file_paths = QFileDialog::getOpenFileNames(m_mainWindow, tr("Open"), qApp->applicationDirPath(), "All types(*.*)");
			OpenFile(file_paths);
		});
	m_messageBus->Subscribe("Open File", [=](const QStringList& data)
		{
			OpenFile(data);
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
					OpenExplorer(file_info.absolutePath());
				}
			}
		});
	m_messageBus->Subscribe("Open Explorer", [=](const QString& data)
		{
			OpenExplorer(data);
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
					OpenCmd(file_info.absolutePath());
				}
			}
		});
	m_messageBus->Subscribe("Open Cmd", [=](const QString& data)
		{
			OpenCmd(data);
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
					if (!file_path.isEmpty())
					{
						m_dirWorkSpace->SetRootDir(file_info.absolutePath());
					}
					m_dirWorkSpace->show();
				}
			}
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
					OpenInDefaultViewer(file_info.absoluteFilePath());
				}
			}
		});
	m_messageBus->Subscribe("Open In Default Viewer", [=](const QString& data)
		{
			OpenInDefaultViewer(data);
		});
	m_messageBus->Subscribe("Open Directory As Directory Workspace", [=]()
		{
			QString file_dir = QFileDialog::getExistingDirectory(m_mainWindow, tr("Open Directory As Directory Workspace"), qApp->applicationDirPath());
			if (!file_dir.isEmpty())
			{
				m_dirWorkSpace->SetRootDir(file_dir);
				m_dirWorkSpace->show();
			}
		});
	m_messageBus->Subscribe("Reload File", [=]()
		{
			ReloadFile();
		});
	m_messageBus->Subscribe("Save File", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index < 0)
			{
				return;
			}
			SaveFile(index, m_openedFilePath[index]);
		});
	m_messageBus->Subscribe("Save As File", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index < 0)
			{
				return;
			}
			SaveFile(index, "");
		});
	m_messageBus->Subscribe("Save As Clipboard", [=]()
		{
			QClipboard* clipboard = QApplication::clipboard();
			QString file_path = QFileDialog::getSaveFileName(m_mainWindow, tr("Save A Copy As..."), qApp->applicationDirPath(), tr("Text files(*.txt);;All types(*.*)"));
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
		});
	m_messageBus->Subscribe("Save All File", [=]()
		{
			for (int index = 0; index < m_centralWidget->count(); ++index)
			{
				SaveFile(index, m_openedFilePath[index]);
			}
		});
	m_messageBus->Subscribe("Close File", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index < 0)
			{
				return;
			}
			CloseFile(index);
		});
	m_messageBus->Subscribe("Close File", [=](int index)
		{
			CloseFile(index);
		});
	m_messageBus->Subscribe("Close All File", [=]()
		{
			for (int index = m_centralWidget->count() - 1; index >= 0; --index)
			{
				CloseFile(index);
			}
		});
	m_messageBus->Subscribe("Close All But Current File", [=]()
		{
			for (int index = m_centralWidget->count() - 1; index >= 0; --index)
			{
				if (index != m_centralWidget->currentIndex())
				{
					CloseFile(index);
				}
			}
		});
	m_messageBus->Subscribe("Close Left File", [=]()
		{
			int current_index = m_centralWidget->currentIndex();
			for (int index = current_index - 1; index >= 0; --index)
			{
				CloseFile(index);
			}
		});
	m_messageBus->Subscribe("Close Right File", [=]()
		{
			int current_index = m_centralWidget->currentIndex();
			for (int index = m_centralWidget->count() - 1; index >= current_index + 1; --index)
			{
				CloseFile(index);
			}
		});
	m_messageBus->Subscribe("Close All Unchanged File", [=]()
		{
			for (int index = m_centralWidget->count() - 1; index >= 0; --index)
			{
				if (m_savedFile[index] == true)
				{
					CloseFile(index);
				}
			}
		});
	m_messageBus->Subscribe("Delete File", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index < 0)
			{
				return;
			}
			QString file_path = m_openedFilePath[index];
			if (!file_path.isEmpty() && QFileInfo::exists(file_path))
			{
				if (QMessageBox::Ok == QMessageBox::question(m_mainWindow, tr("Delete File"),
					tr("The file") + "\"" + file_path + "\"" + tr("will be removed and this file will be closed.Continue?"), QMessageBox::Ok, QMessageBox::No))
				{
					DeleteFile(index);
					CloseFile(index);
				}
			}
		});
	m_messageBus->Subscribe("Clear Recent Record", [=]()
		{
			m_menuBar->ClearHistoryRecord();
		});
	m_messageBus->Subscribe("Exit Software", [=]()
		{
			m_mainWindow->close();
		});

	// Directory
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
	m_messageBus->Subscribe("Copy Path", [=](const QString& data)
		{
			QClipboard* clipboard = QApplication::clipboard();
			clipboard->setText(data);
		});
	m_messageBus->Subscribe("Copy Name", [=](const QString& data)
		{
			QClipboard* clipboard = QApplication::clipboard();
			clipboard->setText(data);
		});

	// QTabWidget
	m_messageBus->Subscribe("Tab Moved", [=](int data1, int data2)
		{
			m_openedFileName.swapItemsAt(data1, data2);
			m_openedFilePath.swapItemsAt(data1, data2);
			m_savedFile.swapItemsAt(data1, data2);
			m_textWidget.swapItemsAt(data1, data2);
		});
	m_messageBus->Subscribe("Text Changed", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index < 0)
			{
				return;
			}
			m_savedFile[index] = false;
			m_centralWidget->setTabIcon(index, QIcon(":/NotePad/standard/tabbar/unsaved.ico"));
		});

	// MainWindow
	m_messageBus->Subscribe("Exit Software", [=]()
		{
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
	connect(m_tabBar, &CustomTabBar::tabMoved, [=](int from, int to)
		{
			m_messageBus->Publish("Tab Moved", from, to);
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
	CustomTextEdit* text_widget = new CustomTextEdit(m_mainWindow);
	connect(text_widget, &CustomTextEdit::textChanged, [=]()
		{
			m_messageBus->Publish("Text Changed");
		});
	m_openedFileName.append(new_file_name);
	m_openedFilePath.append("");
	m_savedFile.append(true);
	m_textWidget.append(text_widget);
	m_centralWidget->addTab(text_widget, QIcon(":/NotePad/standard/tabbar/saved.ico"), new_file_name);
	m_centralWidget->setCurrentIndex(m_centralWidget->count() - 1);
}

void MainCore::OpenFile(const QStringList& file_paths)
{
	for (const QString& file_path : file_paths)
	{
		QFileInfo file_info(file_path);
		if (file_path.isEmpty() || !file_info.exists() || file_info.isDir())
		{
			continue;
		}
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
				CustomTextEdit* text_widget = new CustomTextEdit(m_mainWindow);
				text_widget->SetText(in.readAll());
				connect(text_widget, &CustomTextEdit::textChanged, [=]()
					{
						m_messageBus->Publish("Text Changed");
					});
				m_openedFileName.append(file_info.fileName());
				m_openedFilePath.append(abs_file_path);
				m_savedFile.append(true);
				m_textWidget.append(text_widget);
				m_centralWidget->addTab(text_widget, QIcon(":/NotePad/standard/tabbar/saved.ico"), file_info.fileName());
				m_centralWidget->setCurrentIndex(m_centralWidget->count() - 1);
				file.close();
			}
		}
	}
}

void MainCore::SaveFile(int index, const QString& file_path)
{
	if (index < 0)
	{
		return;
	}
	if (file_path.isEmpty() || !QFileInfo::exists(file_path))
	{
		QString select_path = QFileDialog::getSaveFileName(m_mainWindow, tr("Save File") + "\"" + m_openedFileName[index] + "\"", qApp->applicationDirPath() + "/" + m_openedFileName[index], "Text files(*.txt);;All types(*.*)");
		if (!select_path.isEmpty())

		{
			// 保存
			QFile file(select_path);
			if (file.open(QIODevice::WriteOnly | QIODevice::Text))
			{
				QTextStream in(&file);
				in.setEncoding(QStringConverter::Utf8);
				in << m_textWidget[index]->GetText();
				file.close();
			}
			// 保存后处理
			QFileInfo file_info(select_path);
			m_openedFileName[index] = file_info.fileName();
			m_openedFilePath[index] = file_info.absoluteFilePath();
			m_savedFile[index] = true;
			m_centralWidget->setTabIcon(index, QIcon(":/NotePad/standard/tabbar/saved.ico"));
			m_centralWidget->setTabText(index, file_info.fileName());
			m_messageBus->Publish("Update Window Title");
		}
	}
	else
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
		m_savedFile[index] = true;
		m_centralWidget->setTabIcon(index, QIcon(":/NotePad/standard/tabbar/saved.ico"));
		m_centralWidget->setTabText(index, file_info.fileName());
		m_messageBus->Publish("Update Window Title");
	}
}

void MainCore::CloseFile(int index)
{
	if (index < 0)
	{
		return;
	}

	if (m_savedFile[index] == true)
	{
		// 最近文件
		if (!m_openedFilePath[index].isEmpty())
		{
			m_menuBar->AddHistoryRecord(QStringList() << m_openedFilePath[index]);
		}
		// 关闭
		m_openedFileName.removeAt(index);
		m_openedFilePath.removeAt(index);
		m_savedFile.removeAt(index);
		m_textWidget.removeAt(index);
		m_centralWidget->removeTab(index);
	}
	else
	{
		QMessageBox question_box(QMessageBox::Question, tr("Save File"), tr("Save file") + "\"" + m_openedFileName[index] + "\"?", QMessageBox::Ok, m_mainWindow);
		question_box.addButton(QMessageBox::No);
		question_box.addButton(QMessageBox::Cancel);
		int ret = question_box.exec();
		if (ret == QMessageBox::Ok)
		{
			SaveFile(index, m_openedFilePath[index]);
			// 最近文件
			if (!m_openedFilePath[index].isEmpty())
			{
				m_menuBar->AddHistoryRecord(QStringList() << m_openedFilePath[index]);
			}
			// 关闭
			m_openedFileName.removeAt(index);
			m_openedFilePath.removeAt(index);
			m_savedFile.removeAt(index);
			m_textWidget.removeAt(index);
			m_centralWidget->removeTab(index);
		}
		else if (ret == QMessageBox::No)
		{
			// 最近文件
			if (!m_openedFilePath[index].isEmpty())
			{
				m_menuBar->AddHistoryRecord(QStringList() << m_openedFilePath[index]);
			}
			// 关闭
			m_openedFileName.removeAt(index);
			m_openedFilePath.removeAt(index);
			m_savedFile.removeAt(index);
			m_textWidget.removeAt(index);
			m_centralWidget->removeTab(index);
		}
		else if (ret == QMessageBox::Cancel)
		{
		}
	}
}

void MainCore::DeleteFile(int index)
{
	if (index < 0)
	{
		return;
	}
	QFile file(m_openedFilePath[index]);
	file.remove();
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

void MainCore::OpenExplorer(const QString& file_dir)
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(file_dir));
}

void MainCore::OpenCmd(const QString& file_dir)
{
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

void MainCore::OpenInDefaultViewer(const QString& file_path)
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(file_path));
}