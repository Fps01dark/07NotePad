#include "main_core.h"

#include "framework.h"
#include "custom_menu_bar.h"
#include "custom_settings.h"
#include "custom_tab_bar.h"
#include "custom_tab_widget.h"
#include "custom_text_edit.h"
#include "custom_tool_bar.h"
#include "dir_workspace_dock.h"
#include "main_window.h"
#include "message_bus.h"

namespace
{
	const QString FILE_BACK_UP_DIR = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/OneNotePad/BackUp";
}

MainCore::MainCore(MainWindow* main_window) : m_mainWindow(main_window), QObject(main_window)
{
	m_messageBus = std::make_shared<MessageBus>();
	m_settings = new CustomSettings(m_mainWindow);
	m_menuBar = new CustomMenuBar(m_messageBus, m_mainWindow);
	m_toolBar = new CustomToolBar(m_messageBus, m_mainWindow);
	m_centralWidget = new CustomTabWidget(m_messageBus, m_mainWindow);
	m_tabBar = new CustomTabBar(m_messageBus, m_mainWindow);
	m_dirWorkSpace = new DirWorkspaceDock(m_messageBus, m_mainWindow);

	InitUi();
	InitValue();
	InitConnect();
}

MainCore::~MainCore() {}

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
		"QTabBar::close-button { image: "
		"url(:/OneNotePad/standard/tabbar/closeTabButton.ico); }"
		"QTabBar::close-button:hover { image: "
		"url(:/OneNotePad/standard/tabbar/closeTabButton_hover.ico); }"
		"QTabBar::close-button:pressed { image: "
		"url(:/OneNotePad/standard/tabbar/closeTabButton_push.ico); }");
	// 中心区域
	m_centralWidget->SetTabBar(m_tabBar);
	// 目录工作区
	m_dirWorkSpace->setWindowTitle("Directory Workspace");
	m_dirWorkSpace->hide();

	// 主界面
	int width = m_settings->value("MainWindow/Width", 1920).toInt();
	int height = m_settings->value("MainWindow/Height", 1080).toInt();
	m_mainWindow->resize(width, height);
	m_mainWindow->setWindowTitle("OneNotePad");
	m_mainWindow->setMenuBar(m_menuBar);
	m_mainWindow->addToolBar(Qt::TopToolBarArea, m_toolBar);
	m_mainWindow->setCentralWidget(m_centralWidget);
	m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_dirWorkSpace);

	// 加载最近文件记录
	QStringList&& recent_paths = m_settings->value("MainCore/RecentFilePaths").toStringList();
	m_menuBar->SetRecentFiles(recent_paths);
	// 加载上次打开的文件

	LoadSettings();
}

void MainCore::InitValue()
{
	// Base
	m_messageBus->Subscribe("Update Window Title", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index < 0)
			{
				m_mainWindow->setWindowTitle("OneNotePad");
			}
			else
			{
				QString win_title;
				if (index < m_textWidget.size())
				{
					win_title = m_textWidget[index]->GetFilePath();
				}
				if (win_title.isEmpty() && index < m_textWidget.size())
				{
					win_title = m_textWidget[index]->GetFileName();
				}
				m_mainWindow->setWindowTitle(win_title + " - OneNotePad");
			}
		});
	m_messageBus->Subscribe("New File", [=]()
		{
			static int count = 0;
			while (true)
			{
				QString new_file_name = tr("new %1").arg(count++);
				bool can_use_name = true;
				for (int i = 0; i < m_textWidget.size(); ++i)
				{
					if (m_textWidget[i]->GetFileName() == new_file_name)
					{
						can_use_name = false;
						break;
					}
				}
				if (can_use_name)
				{
					NewFile(new_file_name);
					break;
				}
			}
		});
	m_messageBus->Subscribe("Open File", [=]()
		{
			QStringList&& file_paths = QFileDialog::getOpenFileNames(m_mainWindow, tr("Open"), "", "All types(*.*)");
			for (const QString& file_path : file_paths)
			{
				OpenFile(file_path);
			}
		});
	m_messageBus->Subscribe("Open File", [=](const QString& data)
		{
			OpenFile(data);
		});
	m_messageBus->Subscribe("Open Explorer", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QString&& file_path = m_textWidget[index]->GetFilePath();
				QFileInfo file_info(file_path);
				QDesktopServices::openUrl(QUrl::fromLocalFile(file_info.absolutePath()));
			}
		});
	m_messageBus->Subscribe("Open Explorer", [=](const QString& data)
		{
			QDesktopServices::openUrl(QUrl::fromLocalFile(data));
		});
	m_messageBus->Subscribe("Open Cmd", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QString&& file_path = m_textWidget[index]->GetFilePath();
				QFileInfo file_info(file_path);
				QString   file_dir = file_info.absolutePath();
				// 启动命令行窗口并进入文件所在目录
				QProcess::startDetached("cmd.exe", QStringList() << "/K" << "cd" << file_dir);
			}
		});
	m_messageBus->Subscribe("Open Cmd", [=](const QString& data)
		{
			// 启动命令行窗口并进入文件所在目录
			QProcess::startDetached("cmd.exe", QStringList() << "/K" << "cd" << data);
		});
	m_messageBus->Subscribe("Open Directory Workspace", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QString&& file_path = m_textWidget[index]->GetFilePath();
				QFileInfo file_info(file_path);
				if (!file_path.isEmpty())
				{
					m_dirWorkSpace->SetRootDir(file_info.absolutePath());
					m_dirWorkSpace->show();
				}
			}
		});
	m_messageBus->Subscribe("Open Directory As Workspace", [=]()
		{
			QString file_dir = QFileDialog::getExistingDirectory(m_mainWindow, tr("Open Directory As Workspace"), qApp->applicationDirPath());
			if (!file_dir.isEmpty()) {
				m_dirWorkSpace->SetRootDir(file_dir);
				m_dirWorkSpace->show();
			}
		});
	m_messageBus->Subscribe("Open In Default Viewer", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QString&& file_path = m_textWidget[index]->GetFilePath();
				QDesktopServices::openUrl(QUrl::fromLocalFile(file_path));
			}
		});
	m_messageBus->Subscribe("Open In Default Viewer", [=](const QString& data)
		{
			QDesktopServices::openUrl(QUrl::fromLocalFile(data));
		});
	m_messageBus->Subscribe("Reload File", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QString&& file_path = m_textWidget[index]->GetFilePath();
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
						// TODO:不一定是UTF-8编码格式的数据
						m_textWidget[index]->setText(in.readAll().toUtf8().constData());
						file.close();
					}
				}
			}
		});
	m_messageBus->Subscribe("Save File", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QString&& file_path = m_textWidget[index]->GetFilePath();
				if (file_path.isEmpty() || !QFileInfo::exists(file_path))
				{
					file_path = QFileDialog::getSaveFileName(
						m_mainWindow, tr("Save File") + "\"" + m_textWidget[index]->GetFileName() + "\"",
						qApp->applicationDirPath() + "/" + m_textWidget[index]->GetFileName(),
						"Text files(*.txt);;All types(*.*)");
					if (!file_path.isEmpty())
					{
						SaveFile(index, file_path);
					}
				}
				else
				{
					SaveFile(index, file_path);
				}
			}
		});
	m_messageBus->Subscribe("Save As File", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QString file_path = QFileDialog::getSaveFileName(
					m_mainWindow, tr("Save As File") + "\"" + m_textWidget[index]->GetFileName() + "\"",
					qApp->applicationDirPath() + "/" + m_textWidget[index]->GetFileName(),
					"Text files(*.txt);;All types(*.*)");
				if (!file_path.isEmpty()) {
					SaveFile(index, file_path);
				}
			}
		});
	m_messageBus->Subscribe("Save All File", [=]()
		{
			for (int index = 0; index < m_centralWidget->count(); ++index)
			{
				QString&& file_path = m_textWidget[index]->GetFilePath();
				if (file_path.isEmpty() || !QFileInfo::exists(file_path))
				{
					file_path = QFileDialog::getSaveFileName(
						m_mainWindow, tr("Save File") + "\"" + m_textWidget[index]->GetFileName() + "\"",
						qApp->applicationDirPath() + "/" + m_textWidget[index]->GetFileName(),
						"Text files(*.txt);;All types(*.*)");
					if (!file_path.isEmpty())
					{
						SaveFile(index, file_path);
					}
				}
				else
				{
					SaveFile(index, file_path);
				}
			}
		});
	m_messageBus->Subscribe("Save As Clipboard", [=]()
		{
			QClipboard* clipboard = QApplication::clipboard();
			QString     file_path = QFileDialog::getSaveFileName(m_mainWindow, tr("Save A Copy As..."),
				qApp->applicationDirPath(),
				tr("Text files(*.txt);;All types(*.*)"));
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
	m_messageBus->Subscribe("Close File", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				if (m_textWidget[index]->GetSaveStatus() == true)
				{
					CloseFile(index);
				}
				else
				{
					QMessageBox question_box(QMessageBox::Question, tr("Save File"),
						tr("Save file") + "\"" + m_textWidget[index]->GetFileName() + "\"?",
						QMessageBox::Ok, m_mainWindow);
					question_box.addButton(QMessageBox::No);
					question_box.addButton(QMessageBox::Cancel);
					int ret = question_box.exec();
					if (ret == QMessageBox::Ok)
					{
						QString&& file_path = m_textWidget[index]->GetFilePath();
						if (file_path.isEmpty() || !QFileInfo::exists(file_path))
						{
							file_path = QFileDialog::getSaveFileName(
								m_mainWindow, tr("Save File") + "\"" + m_textWidget[index]->GetFileName() + "\"",
								qApp->applicationDirPath() + "/" + m_textWidget[index]->GetFileName(),
								"Text files(*.txt);;All types(*.*)");
							if (!file_path.isEmpty())
							{
								SaveFile(index, file_path);
								CloseFile(index);
							}
						}
						else {
							SaveFile(index, file_path);
							CloseFile(index);
						}
					}
					else if (ret == QMessageBox::No) {
						CloseFile(index);
					}
					else if (ret == QMessageBox::Cancel) {
					}
				}
			}
		});
	m_messageBus->Subscribe("Close File", [=](int index)
		{
			if (index >= 0 && index < m_centralWidget->count())
			{
				if (m_textWidget[index]->GetSaveStatus() == true)
				{
					CloseFile(index);
				}
				else
				{
					QMessageBox question_box(QMessageBox::Question, tr("Save File"),
						tr("Save file") + "\"" + m_textWidget[index]->GetFileName() + "\"?",
						QMessageBox::Ok, m_mainWindow);
					question_box.addButton(QMessageBox::No);
					question_box.addButton(QMessageBox::Cancel);
					int ret = question_box.exec();
					if (ret == QMessageBox::Ok)
					{
						QString&& file_path = m_textWidget[index]->GetFilePath();
						if (file_path.isEmpty() || !QFileInfo::exists(file_path))
						{
							file_path = QFileDialog::getSaveFileName(
								m_mainWindow, tr("Save File") + "\"" + m_textWidget[index]->GetFileName() + "\"",
								qApp->applicationDirPath() + "/" + m_textWidget[index]->GetFileName(),
								"Text files(*.txt);;All types(*.*)");
							if (!file_path.isEmpty()) {
								SaveFile(index, file_path);
								CloseFile(index);
							}
						}
						else {
							SaveFile(index, file_path);
							CloseFile(index);
						}
					}
					else if (ret == QMessageBox::No) {
						CloseFile(index);
					}
					else if (ret == QMessageBox::Cancel) {
					}
				}
			}
		});
	m_messageBus->Subscribe("Close All File", [=]()
		{
			for (int index = m_centralWidget->count() - 1; index >= 0; --index)
			{
				if (m_textWidget[index]->GetSaveStatus() == true)
				{
					CloseFile(index);
				}
				else
				{
					QMessageBox question_box(QMessageBox::Question, tr("Save File"),
						tr("Save file") + "\"" + m_textWidget[index]->GetFileName() + "\"?",
						QMessageBox::Ok, m_mainWindow);
					question_box.addButton(QMessageBox::No);
					question_box.addButton(QMessageBox::Cancel);
					int ret = question_box.exec();
					if (ret == QMessageBox::Ok) {
						QString&& file_path = m_textWidget[index]->GetFilePath();
						if (file_path.isEmpty() || !QFileInfo::exists(file_path)) {
							file_path = QFileDialog::getSaveFileName(
								m_mainWindow, tr("Save File") + "\"" + m_textWidget[index]->GetFileName() + "\"",
								qApp->applicationDirPath() + "/" + m_textWidget[index]->GetFileName(),
								"Text files(*.txt);;All types(*.*)");
							if (!file_path.isEmpty()) {
								SaveFile(index, file_path);
								CloseFile(index);
							}
						}
						else {
							SaveFile(index, file_path);
							CloseFile(index);
						}
					}
					else if (ret == QMessageBox::No) {
						CloseFile(index);
					}
					else if (ret == QMessageBox::Cancel) {
					}
				}
			}
		});
	m_messageBus->Subscribe("Close All But Current File", [=]()
		{
			for (int index = m_centralWidget->count() - 1; index >= 0; --index)
			{
				if (index != m_centralWidget->currentIndex())
				{
					if (m_textWidget[index]->GetSaveStatus() == true)
					{
						CloseFile(index);
					}
					else {
						QMessageBox question_box(
							QMessageBox::Question, tr("Save File"),
							tr("Save file") + "\"" + m_textWidget[index]->GetFileName() + "\"?", QMessageBox::Ok,
							m_mainWindow);
						question_box.addButton(QMessageBox::No);
						question_box.addButton(QMessageBox::Cancel);
						int ret = question_box.exec();
						if (ret == QMessageBox::Ok)
						{
							QString&& file_path = m_textWidget[index]->GetFilePath();
							if (file_path.isEmpty() || !QFileInfo::exists(file_path)) {
								file_path = QFileDialog::getSaveFileName(
									m_mainWindow,
									tr("Save File") + "\"" + m_textWidget[index]->GetFileName() + "\"",
									qApp->applicationDirPath() + "/" + m_textWidget[index]->GetFileName(),
									"Text files(*.txt);;All types(*.*)");
								if (!file_path.isEmpty()) {
									SaveFile(index, file_path);
									CloseFile(index);
								}
							}
							else {
								SaveFile(index, file_path);
								CloseFile(index);
							}
						}
						else if (ret == QMessageBox::No) {
							CloseFile(index);
						}
						else if (ret == QMessageBox::Cancel) {
						}
					}
				}
			}
		});
	m_messageBus->Subscribe("Close Left File", [=]()
		{
			int current_index = m_centralWidget->currentIndex();
			for (int index = current_index - 1; index >= 0; --index)
			{
				if (m_textWidget[index]->GetSaveStatus() == true) {
					CloseFile(index);
				}
				else {
					QMessageBox question_box(QMessageBox::Question, tr("Save File"),
						tr("Save file") + "\"" + m_textWidget[index]->GetFileName() + "\"?",
						QMessageBox::Ok, m_mainWindow);
					question_box.addButton(QMessageBox::No);
					question_box.addButton(QMessageBox::Cancel);
					int ret = question_box.exec();
					if (ret == QMessageBox::Ok)
					{
						QString&& file_path = m_textWidget[index]->GetFilePath();
						if (file_path.isEmpty() || !QFileInfo::exists(file_path)) {
							file_path = QFileDialog::getSaveFileName(
								m_mainWindow, tr("Save File") + "\"" + m_textWidget[index]->GetFileName() + "\"",
								qApp->applicationDirPath() + "/" + m_textWidget[index]->GetFileName(),
								"Text files(*.txt);;All types(*.*)");
							if (!file_path.isEmpty()) {
								SaveFile(index, file_path);
								CloseFile(index);
							}
						}
						else {
							SaveFile(index, file_path);
							CloseFile(index);
						}
					}
					else if (ret == QMessageBox::No) {
						CloseFile(index);
					}
					else if (ret == QMessageBox::Cancel) {
					}
				}
			}
		});
	m_messageBus->Subscribe("Close Right File", [=]()
		{
			int current_index = m_centralWidget->currentIndex();
			for (int index = m_centralWidget->count() - 1; index >= current_index + 1; --index)
			{
				if (m_textWidget[index]->GetSaveStatus() == true)
				{
					CloseFile(index);
				}
				else
				{
					QMessageBox question_box(QMessageBox::Question, tr("Save File"),
						tr("Save file") + "\"" + m_textWidget[index]->GetFileName() + "\"?",
						QMessageBox::Ok, m_mainWindow);
					question_box.addButton(QMessageBox::No);
					question_box.addButton(QMessageBox::Cancel);
					int ret = question_box.exec();
					if (ret == QMessageBox::Ok)
					{
						QString&& file_path = m_textWidget[index]->GetFilePath();
						if (file_path.isEmpty() || !QFileInfo::exists(file_path))
						{
							file_path = QFileDialog::getSaveFileName(
								m_mainWindow, tr("Save File") + "\"" + m_textWidget[index]->GetFileName() + "\"",
								qApp->applicationDirPath() + "/" + m_textWidget[index]->GetFileName(),
								"Text files(*.txt);;All types(*.*)");
							if (!file_path.isEmpty())
							{
								SaveFile(index, file_path);
								CloseFile(index);
							}
						}
						else
						{
							SaveFile(index, file_path);
							CloseFile(index);
						}
					}
					else if (ret == QMessageBox::No) {
						CloseFile(index);
					}
					else if (ret == QMessageBox::Cancel) {
					}
				}
			}
		});
	m_messageBus->Subscribe("Close All Unchanged File", [=]()
		{
			for (int index = m_centralWidget->count() - 1; index >= 0; --index)
			{
				if (m_textWidget[index]->GetSaveStatus() == true)
				{
					CloseFile(index);
				}
			}
		});
	m_messageBus->Subscribe("Delete File", [=]() {
		int index = m_centralWidget->currentIndex();
		if (index >= 0)
		{
			QString&& file_path = m_textWidget[index]->GetFilePath();
			if (!file_path.isEmpty() && QFileInfo::exists(file_path)) {
				if (QMessageBox::Ok ==
					QMessageBox::question(m_mainWindow, tr("Delete File"),
						tr("The file") + "\"" + file_path + "\"" +
						tr("will be removed and this file will "
							"be closed.Continue?"),
						QMessageBox::Ok, QMessageBox::No)) {
					QFile file(file_path);
					file.remove();
					CloseFile(index);
				}
			}
		}
		});
	m_messageBus->Subscribe("Print", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				// 创建QPrinter对象
				QPrinter     printer;
				QPrintDialog printDialog(&printer, m_mainWindow);

				// 打开打印对话框
				if (printDialog.exec() == QDialog::Accepted)
				{
					CustomTextEdit* text_edit = m_textWidget[index];
					// 创建QPainter对象用于绘制内容
					QPainter painter;
					if (painter.begin(&printer))
					{
						// 设置打印内容，绘制文本或其他图形
						painter.setFont(text_edit->font());
						painter.drawText(100, 100, text_edit->getText(text_edit->length()));
						painter.end();  // 完成绘制
					}
					else
					{
						qDebug() << "打印失败";
					}
				}
			}
		});
	m_messageBus->Subscribe("Clear Recent Record", [=]()
		{
			m_menuBar->SetRecentFiles(QStringList());
		});
	m_messageBus->Subscribe("Exit Software", [=]()
		{
			m_mainWindow->close();
		});

	m_messageBus->Subscribe("Undo", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->undo();
			}
		});
	m_messageBus->Subscribe("Redo", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->redo();
			}
		});
	m_messageBus->Subscribe("Cut", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->cut();
			}
		});
	m_messageBus->Subscribe("Copy", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->copyAllowLine();
			}
		});
	m_messageBus->Subscribe("Paste", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->paste();
			}
		});
	m_messageBus->Subscribe("Delete", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->clear();
			}
		});
	m_messageBus->Subscribe("Select All", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->selectAll();
			}
		});
	m_messageBus->Subscribe("Insert Short Time", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QDateTime current_date_time = QDateTime::currentDateTime();
				QString date_string = current_date_time.toString("HH:mm yyyy/MM/dd");
				// TODO:不一定是UTF-8编码格式的数据
				m_textWidget[index]->addText(date_string.toUtf8().length(), date_string.toUtf8().constData());
			}
		});
	m_messageBus->Subscribe("Insert Long Time", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QDateTime current_date_time = QDateTime::currentDateTime();
				QString date_string = current_date_time.toString("HH:mm yyyy年MM月dd日");
				// TODO:不一定是UTF-8编码格式的数据
				m_textWidget[index]->addText(date_string.toUtf8().length(), date_string.toUtf8().constData());
			}
		});
	m_messageBus->Subscribe("Insert Custom Time", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QDateTime current_date_time = QDateTime::currentDateTime();
				QString date_string = current_date_time.toString("yyyy-MM-dd HH:mm:ss");
				// TODO:不一定是UTF-8编码格式的数据
				m_textWidget[index]->addText(date_string.toUtf8().length(), date_string.toUtf8().constData());
			}
		});
	m_messageBus->Subscribe("Copy All Names", [=]()
		{
			QClipboard* clipboard = QApplication::clipboard();
			QString all_names;
			for (int i = 0; i < m_textWidget.size(); ++i)
			{
				all_names.append(m_textWidget[i]->GetFileName() + "\n");
			}
			clipboard->setText(all_names);
		});
	m_messageBus->Subscribe("Copy All Paths", [=]()
		{
			QClipboard* clipboard = QApplication::clipboard();
			QString all_paths;
			for (int i = 0; i < m_textWidget.size(); ++i)
			{
				all_paths.append(m_textWidget[i]->GetFilePath() + "\n");
			}
			clipboard->setText(all_paths);
		});
	m_messageBus->Subscribe("Increase Line Indent", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->tab();
			}
		});
	m_messageBus->Subscribe("Decrease Line Indent", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->backTab();
			}
		});
	m_messageBus->Subscribe("UPPERCASE", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->upperCase();
			}
		});
	m_messageBus->Subscribe("lowercase", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->lowerCase();
			}
		});
	m_messageBus->Subscribe("Duplicate Current Line", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->lineDuplicate();
			}
		});
	m_messageBus->Subscribe("Remove Duplicate Line", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				// TODO:通过搜索功能删除重复行
			}
		});
	m_messageBus->Subscribe("Remove Consecutive Duplicate Lines", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				// TODO:通过搜索功能删除重复行
			}
		});
	m_messageBus->Subscribe("Split Lines", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->targetFromSelection();
				m_textWidget[index]->linesSplit(0);
			}
		});
	m_messageBus->Subscribe("Join Lines", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->targetFromSelection();
				m_textWidget[index]->linesJoin();
			}
		});
	m_messageBus->Subscribe("Move Up Current Line", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->moveSelectedLinesUp();
			}
		});
	m_messageBus->Subscribe("Move Down Current Line", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->moveSelectedLinesDown();
			}
		});
	m_messageBus->Subscribe("Remove Empty Lines", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				// TODO:通过搜索功能删除空行
			}
		});
	m_messageBus->Subscribe("Remove Empty Lines Blank", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				// TODO:通过搜索功能删除空行
			}
		});
	m_messageBus->Subscribe("Insert Blank Line Above Current", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* text_edit = m_textWidget[index];
				// 获取当前行号
				intptr_t current_line_index = text_edit->lineFromPosition(text_edit->currentPos());
				// 获取当前行开始的位置
				intptr_t pos = text_edit->positionFromLine(current_line_index);
				// 插入空行
				text_edit->insertText(pos, "\r\n");
			}
		});
	m_messageBus->Subscribe("Insert Blank Line Below Current", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* text_edit = m_textWidget[index];
				// 获取当前行号
				intptr_t current_line_index = text_edit->lineFromPosition(text_edit->currentPos());
				// 获取下一行开始的位置
				intptr_t pos = text_edit->positionFromLine(current_line_index + 1);
				// TODO:不一定是UTF-8编码格式的数据
				text_edit->insertText(pos, text_edit->GetEOLString().toUtf8().constData());
			}
		});
	m_messageBus->Subscribe("Reverse Line Order", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* text_edit = m_textWidget[index];
				QByteArray sel_text = text_edit->getSelText();
			}
		});
	m_messageBus->Subscribe("Randomize Line Order", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
			}
		});

	m_messageBus->Subscribe("EOL Conversion", [=](int eolMode)
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->convertEOLs(eolMode);
				m_textWidget[index]->setEOLMode(eolMode);
				m_menuBar->UpdateEOLAction(m_textWidget[index]);
			}
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
			if (index >= 0)
			{
				QString&& file_path = m_textWidget[index]->GetFilePath();
				m_dirWorkSpace->LocationFile(file_path);
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
			m_textWidget.swapItemsAt(data1, data2);
		});
	m_messageBus->Subscribe("Text Changed", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				m_textWidget[index]->SetSaveStatus(false);
				m_centralWidget->setTabIcon(index, QIcon(":/OneNotePad/standard/tabbar/unsaved.ico"));
			}
		});
	m_messageBus->Subscribe("Copy Path", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QClipboard* clipboard = QApplication::clipboard();
				clipboard->setText(m_textWidget[index]->GetFilePath());
			}
		});
	m_messageBus->Subscribe("Copy Name", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QClipboard* clipboard = QApplication::clipboard();
				clipboard->setText(m_textWidget[index]->GetFileName());
			}
		});
	m_messageBus->Subscribe("Copy Directory", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				QClipboard* clipboard = QApplication::clipboard();
				QFileInfo   file_info(m_textWidget[index]->GetFilePath());
				clipboard->setText(file_info.absolutePath());
			}
		});
	m_messageBus->Subscribe("Change Zoom", [=](int data)
		{
			m_fontSize = data;
			for (int i = 0; i < m_textWidget.size(); ++i)
			{
				QFont font = m_textWidget[i]->font();
				font.setPointSize(m_fontSize);
				m_textWidget[i]->setFont(font);
			}
		});

	// MainWindow
	m_messageBus->Subscribe("Exit Software", [=]()
		{
			// 保存主窗口大小
			QSize main_window_size = m_mainWindow->size();
			m_settings->setValue("MainWindow/Width", main_window_size.width());
			m_settings->setValue("MainWindow/Height", main_window_size.height());
			// 保存上次打开文件
			m_settings->setValue("MainCore/RecentFilePaths", m_menuBar->GetRecentFiles());
			SaveSettings();
		});

	m_messageBus->Subscribe("Debug", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				// 选择的区域个数
				sptr_t sel = m_textWidget[index]->selections();
				qDebug() << "Debug";
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
	connect(m_tabBar, &CustomTabBar::tabMoved, [=](int from, int to)
		{
			m_messageBus->Publish("Tab Moved", from, to);
		});
}

bool MainCore::NewFile(const QString& new_file_name)
{
	CustomTextEdit* text_widget = new CustomTextEdit(m_messageBus, m_centralWidget);
	text_widget->setEOLMode(SC_EOL_CRLF);
	//QFont font = editor->font();
	//font.setPointSize(m_fontSize);
	//text_widget->setFont(font);
	connect(text_widget, &CustomTextEdit::savePointChanged, [=]()
		{
			m_messageBus->Publish("Text Changed");
		});
	text_widget->SetFileName(new_file_name);
	text_widget->SetFilePath("");
	text_widget->SetSaveStatus(true);
	m_textWidget.append(text_widget);
	m_centralWidget->addTab(text_widget, QIcon(":/OneNotePad/standard/tabbar/saved.ico"), new_file_name);
	m_centralWidget->setCurrentIndex(m_centralWidget->count() - 1);
	m_menuBar->UpdateEOLAction(text_widget);
	return true;
}

bool MainCore::OpenFile(const QString& file_path)
{
	QFileInfo file_info(file_path);
	if (file_path.isEmpty() || !file_info.exists())
	{
		return false;
	}
	QString abs_file_path = file_info.absoluteFilePath();
	// 判断文件是否已打开
	int opened_index = -1;
	for (int i = 0; i < m_textWidget.size(); ++i)
	{
		if (m_textWidget[i]->GetFilePath() == abs_file_path)
		{
			opened_index = i;
		}
	}
	if (opened_index != -1)
	{
		// 打开过
		m_centralWidget->setCurrentIndex(opened_index);
		m_menuBar->UpdateEOLAction(m_textWidget[opened_index]);
	}
	else
	{
		// 未打开过
		QFile file(file_path);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream in(&file);
			in.setEncoding(QStringConverter::Utf8);
			CustomTextEdit* text_widget = new CustomTextEdit(m_messageBus, m_centralWidget);
			text_widget->setEOLMode(SC_EOL_CRLF);
			QFont           font = text_widget->font();
			font.setPointSize(m_fontSize);
			text_widget->setFont(font);
			// TODO:不一定是UTF-8编码格式的数据
			text_widget->setText(in.readAll().toUtf8().constData());
			file.close();

			// 打开后处理
			connect(text_widget, &CustomTextEdit::savePointChanged, [=]()
				{
					m_messageBus->Publish("Text Changed");
				});
			text_widget->SetFileName(file_info.fileName());
			text_widget->SetFilePath(abs_file_path);
			text_widget->SetSaveStatus(true);
			m_textWidget.append(text_widget);
			m_centralWidget->addTab(text_widget, QIcon(":/OneNotePad/standard/tabbar/saved.ico"), file_info.fileName());
			m_centralWidget->setCurrentIndex(m_centralWidget->count() - 1);
			m_menuBar->UpdateEOLAction(text_widget);
		}
	}
	return true;
}

bool MainCore::SaveFile(int index, const QString& file_path)
{
	if (index < 0 || index >= m_centralWidget->count())
	{
		return false;
	}
	QFileInfo file_info(file_path);
	if (file_path.isEmpty())
	{
		return false;
	}
	// 打开文件
	QFile file(file_path);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		// 保存
		QTextStream in(&file);
		in.setEncoding(QStringConverter::Utf8);
		CustomTextEdit* text_edit = m_textWidget[index];
		in << text_edit->getText(m_textWidget[index]->length());
		file.close();

		// 保存后处理
		text_edit->SetFileName(file_info.fileName());
		text_edit->SetFilePath(file_info.absoluteFilePath());
		text_edit->SetSaveStatus(true);
		m_centralWidget->setTabIcon(index, QIcon(":/OneNotePad/standard/tabbar/saved.ico"));
		m_centralWidget->setTabText(index, file_info.fileName());
		m_messageBus->Publish("Update Window Title");
	}
	return true;
}

bool MainCore::CloseFile(int index)
{
	if (index < 0 || index >= m_centralWidget->count())
	{
		return false;
	}

	// 最近文件
	if (!m_textWidget[index]->GetFilePath().isEmpty())
	{
		m_messageBus->Publish("Add Recent File", m_textWidget[index]->GetFilePath());
	}
	// 关闭
	m_textWidget.removeAt(index);
	m_centralWidget->removeTab(index);
	// 更新
	int new_index = m_centralWidget->currentIndex();
	if (new_index >= 0)
	{
		m_menuBar->UpdateEOLAction(m_textWidget[new_index]);
	}

	return true;
}

bool MainCore::LoadSettings()
{
	QStringList&& opened_file_name = m_settings->value("MainCore/OpenedFileNames").toStringList();
	QStringList&& opened_file_path = m_settings->value("MainCore/OpenedFilePaths").toStringList();
	QList<bool>&& saved_file = m_settings->BoolList("MainCore/SavedFile");
	int           current_index = m_settings->value("MainCore/CurrentIndex").toInt();
	m_fontSize = m_settings->value("CustomTextEdit/FontSize").toInt();
	for (int index = 0; index < opened_file_path.size(); ++index)
	{
		QString file_path = opened_file_path[index];
		if (saved_file[index] == false)
		{
			file_path = FILE_BACK_UP_DIR + "/" + opened_file_name[index];
		}
		// 打开的文件
		QFile file(file_path);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream in(&file);
			in.setEncoding(QStringConverter::Utf8);
			CustomTextEdit* text_widget = new CustomTextEdit(m_messageBus, m_centralWidget);
			text_widget->setEOLMode(SC_EOL_CRLF);
			QFont           font = text_widget->font();
			font.setPointSize(m_fontSize);
			text_widget->setFont(font);
			text_widget->setText(in.readAll().toUtf8().constData());
			connect(text_widget, &CustomTextEdit::savePointChanged, [=]()
				{
					m_messageBus->Publish("Text Changed");
				});
			text_widget->SetFileName(opened_file_name[index]);
			text_widget->SetFilePath(opened_file_path[index]);
			text_widget->SetSaveStatus(saved_file[index]);
			m_textWidget.append(text_widget);
			if (saved_file[index] == true)
			{
				m_centralWidget->addTab(text_widget,
					QIcon(":/OneNotePad/standard/tabbar/saved.ico"),
					opened_file_name[index]);
			}
			else
			{
				m_centralWidget->addTab(text_widget,
					QIcon(":/OneNotePad/standard/tabbar/unsaved.ico"),
					opened_file_name[index]);
			}
			file.close();
			m_menuBar->UpdateEOLAction(text_widget);
		}
	}
	// 设置当前文件
	if (current_index < m_centralWidget->count())
	{
		m_centralWidget->setCurrentIndex(current_index);
	}
	else
	{
		m_centralWidget->setCurrentIndex(m_centralWidget->count() - 1);
	}
	return true;
}

bool MainCore::SaveSettings()
{
	QList<QString> m_openedFilePath;
	QList<QString> m_openedFileName;
	QList<bool> m_savedFile;
	for (int i = 0; i < m_textWidget.size(); ++i)
	{
		m_openedFilePath.append(m_textWidget[i]->GetFilePath());
		m_openedFileName.append(m_textWidget[i]->GetFileName());
		m_savedFile.append(m_textWidget[i]->GetSaveStatus());
	}
	// 保存上次打开文件
	m_settings->setValue("MainCore/OpenedFilePaths", m_openedFilePath);
	m_settings->setValue("MainCore/OpenedFileNames", m_openedFileName);
	m_settings->SetBoolList("MainCore/SavedFile", m_savedFile);
	m_settings->setValue("MainCore/CurrentIndex", m_centralWidget->currentIndex());

	// 保存无路径文件
	for (int index = 0; index < m_centralWidget->count(); ++index)
	{
		if (m_textWidget[index]->GetSaveStatus() == false)
		{
			QDir file_dir(FILE_BACK_UP_DIR);
			if (!file_dir.exists())
			{
				// 不存在 创建路径
				bool mk_success = file_dir.mkpath(FILE_BACK_UP_DIR);
			}
			QFile file(FILE_BACK_UP_DIR + "/" + m_textWidget[index]->GetFileName());
			if (file.open(QIODevice::WriteOnly | QIODevice::Text))
			{
				QTextStream in(&file);
				in.setEncoding(QStringConverter::Utf8);
				in << m_textWidget[index]->getText(m_textWidget[index]->length());
				file.close();
			}
		}
	}

	// 保存字体大小
	m_settings->setValue("CustomTextEdit/FontSize", m_fontSize);
	return true;
}