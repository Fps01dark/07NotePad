#pragma execution_character_set("utf-8")
#include "main_core.h"

#include "framework.h"

#include "message_bus.h"
#include "main_window.h"
#include "custom_menu_bar.h"
#include "custom_tool_bar.h"
#include "custom_tab_bar.h"
#include "custom_tab_widget.h"
#include "text_widget.h"

MainCore::MainCore(MainWindow* main_window, QObject* parent)
	:m_mainWindow(main_window),
	QObject(parent)
{
	m_messageBus = std::make_shared<MessageBus>();
	m_menuBar = new CustomMenuBar(m_messageBus, m_mainWindow);
	m_toolBar = new CustomToolBar(m_messageBus, m_mainWindow);
	m_centralWidget = new CustomTabWidget(m_messageBus, m_mainWindow);
	m_tabBar = new CustomTabBar(m_messageBus, m_mainWindow);
	m_tabBar->setTabsClosable(true);
	m_tabBar->setMovable(true);
	m_tabBar->setStyleSheet(
		"QTabBar::close-button { image: url(../Resources/standard/tabbar/closeTabButton.ico); }"
		"QTabBar::close-button:hover { image: url(../Resources/standard/tabbar/closeTabButton_hover.ico); }"
		"QTabBar::close-button:pressed { image: url(../Resources/standard/tabbar/closeTabButton_push.ico); }"
	);
	m_centralWidget->SetTabBar(m_tabBar);

	InitUi();
	InitValue();
	InitConnect();
}

MainCore::~MainCore()
{
}

CustomMenuBar* MainCore::GetCustomMenuBar() const
{
	return m_menuBar;
}

CustomToolBar* MainCore::GetCustomToolBar() const
{
	return m_toolBar;
}

CustomTabWidget* MainCore::GetCustomTabWidget() const
{
	return m_centralWidget;
}

void MainCore::InitUi()
{
}

void MainCore::InitValue()
{
	//std::function<void(const std::string&)> handler
	m_messageBus->Subscribe("New File", [=]()
		{
			NewFile();
		});
	m_messageBus->Subscribe("Open File", [=]()
		{
			OpenFile();
		});
	m_messageBus->Subscribe("Open File", [=](const QString& data)
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
}

void MainCore::InitConnect()
{
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
	m_centralWidget->addTab(text_widget, QIcon(":/NotePad/standard/tabbar/saved.ico"), new_file_name);
	m_openedFileName.append(new_file_name);
	m_openedFilePath.append("");
	m_textWidget.append(text_widget);
}

void MainCore::OpenFile()
{
	QString&& file_path = QFileDialog::getOpenFileName(m_mainWindow, tr("Open"), qApp->applicationDirPath(), "All types(*.*)");
	if (!file_path.isEmpty())
	{
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
				in.setCodec("UTF-8");
				TextWidget* text_widget = new TextWidget(m_mainWindow);
				text_widget->SetText(in.readAll());
				m_centralWidget->addTab(text_widget, QIcon(":/NotePad/standard/tabbar/saved.ico"), file_info.fileName());
				m_centralWidget->setCurrentIndex(m_centralWidget->count() - 1);
				m_openedFileName.append(file_info.fileName());
				m_openedFilePath.append(abs_file_path);
				m_textWidget.append(text_widget);
				file.close();
			}
		}
	}
}

void MainCore::OpenFile(const QString& file_path)
{
	if (file_path.isEmpty() || !QFileInfo::exists(file_path))
	{
		return;
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
			in.setCodec("UTF-8");
			TextWidget* text_widget = new TextWidget(m_mainWindow);
			text_widget->SetText(in.readAll());
			m_centralWidget->addTab(text_widget, QIcon(":/NotePad/standard/tabbar/saved.ico"), file_info.fileName());
			m_centralWidget->setCurrentIndex(m_centralWidget->count() - 1);
			m_openedFileName.append(file_info.fileName());
			m_openedFilePath.append(abs_file_path);
			m_textWidget.append(text_widget);
			file.close();
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
			in.setCodec("UTF-8");
			in << m_textWidget[index]->GetText();
			file.close();
		}
		// 保存后处理
		QFileInfo file_info(file_path);
		m_openedFileName[index] = file_info.fileName();
		m_openedFilePath[index] = file_info.absoluteFilePath();
		m_centralWidget->setTabText(index, file_info.fileName());
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
			in.setCodec("UTF-8");
			in << m_textWidget[index]->GetText();
			file.close();
		}
		// 保存后处理
		QFileInfo file_info(file_path);
		m_openedFileName[index] = file_info.fileName();
		m_openedFilePath[index] = file_info.absoluteFilePath();
		m_centralWidget->setTabText(index, file_info.fileName());
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
				in.setCodec("UTF-8");
				in << m_textWidget[i]->GetText();
				file.close();
			}
			// 保存后处理
			QFileInfo file_info(file_path);
			m_openedFileName[i] = file_info.fileName();
			m_openedFilePath[i] = file_info.absoluteFilePath();
			m_centralWidget->setTabText(i, file_info.fileName());
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
			in.setCodec("UTF-8");
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
	m_centralWidget->removeTab(index);
	m_openedFileName.removeAt(index);
	m_openedFilePath.removeAt(index);
	m_textWidget.removeAt(index);
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
	m_centralWidget->removeTab(index);
	m_openedFileName.removeAt(index);
	m_openedFilePath.removeAt(index);
	m_textWidget.removeAt(index);
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
	m_centralWidget->clear();
	m_openedFileName.clear();
	m_openedFilePath.clear();
	m_textWidget.clear();
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
			in.setCodec("UTF-8");
			m_textWidget[index]->SetText(in.readAll());
			file.close();
		}
	}
}