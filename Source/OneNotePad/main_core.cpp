#include "main_core.h"

#include <random>

#include <QProcess>
#include <QPrinter>
#include <QPainter>
#include <QClipboard>
#include <QWheelEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrintDialog>
#include <QStandardPaths>
#include <QDesktopServices>

#include "custom_menu_bar.h"
#include "custom_settings.h"
#include "custom_tab_bar.h"
#include "custom_tab_widget.h"
#include "custom_text_edit.h"
#include "custom_tool_bar.h"
#include "dir_workspace_dock.h"
#include "main_window.h"
#include "message_bus.h"
#include "custom_status_bar.h"

namespace
{
	const QString FILE_BACK_UP_DIR = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/OneNotePad/BackUp";
}

MainCore::MainCore(MainWindow* main_window)
	: m_mainWindow(main_window), QObject(main_window)
{
	m_messageBus = std::make_shared<MessageBus>();
	m_settings = new CustomSettings(m_mainWindow);
	m_menuBar = new CustomMenuBar(m_messageBus, m_mainWindow);
	m_toolBar = new CustomToolBar(m_messageBus, m_mainWindow);
	m_centralWidget = new CustomTabWidget(m_messageBus, m_mainWindow);
	m_tabBar = new CustomTabBar(m_messageBus, m_mainWindow);
	m_statusBar = new CustomStatusBar(m_messageBus, m_mainWindow);
	m_dirWorkSpace = new DirWorkspaceDock(m_messageBus, m_mainWindow);

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
	m_mainWindow->setWindowIcon(QIcon(":/OneNotePad/npp.ico"));
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
	m_mainWindow->setStatusBar(m_statusBar);
	m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_dirWorkSpace);

	// 加载最近文件记录
	QStringList&& recent_paths = m_settings->value("MainCore/RecentFilePaths").toStringList();
	m_menuBar->SetRecentFiles(recent_paths);
	// 加载上次打开的文件
	LoadSettings();
}

void MainCore::InitValue()
{
	// File
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
	// Edit
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
	m_messageBus->Subscribe("Begin/End Select", [=](int start)
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* editor = m_textWidget[index];
				if (start != 0)
				{
					// 开始
					editor->SetBeginSelectPosition(editor->currentPos());
					editor->changeSelectionMode(SC_SEL_STREAM);
				}
				else
				{
					// 结束
					editor->setSel(editor->GetBeginSelectPosition(), editor->currentPos());
					editor->SetBeginSelectPosition(-1);
				}
			}
		});
	m_messageBus->Subscribe("Begin/End Select in Column Mode", [=](int start)
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				// TODO:不知道为什么不起作用
				CustomTextEdit* editor = m_textWidget[index];
				if (start != 0)
				{
					// 开始
					editor->SetBeginColumnSelectPosition(editor->currentPos());
					editor->changeSelectionMode(SC_SEL_RECTANGLE);
				}
				else
				{
					// 结束
					editor->setAnchor(editor->GetBeginColumnSelectPosition());
					editor->SetBeginColumnSelectPosition(-1);
				}
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
	m_messageBus->Subscribe("Proper Case", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* editor = m_textWidget[index];
				// 获取位置
				sptr_t start_pos = editor->selectionStart();
				sptr_t end_pos = editor->selectionEnd();
				// 获取文本
				QString text = QString::fromUtf8(editor->textRange(start_pos, end_pos));
				for (int i = 0; i < text.size(); ++i)
				{
					if (text[i].isLetter())
					{
						if (i >= 2 && text[i - 1] == '\'' && text[i - 2].isLetterOrNumber())
						{
							text[i] = text[i].toLower();
						}
						else if (i < 1 || (i >= 1 && !text[i - 1].isLetterOrNumber()))
						{
							text[i] = text[i].toUpper();
						}
						else
						{
							text[i] = text[i].toLower();
						}
					}
				}
				// 替换
				editor->setTargetRange(start_pos, end_pos);
				editor->replaceTarget(-1, text.toUtf8().constData());
				editor->setSel(start_pos, end_pos);
			}
		});
	m_messageBus->Subscribe("Proper Case(blend)", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* editor = m_textWidget[index];
				// 获取位置
				sptr_t start_pos = editor->selectionStart();
				sptr_t end_pos = editor->selectionEnd();
				// 获取文本
				QString text = QString::fromUtf8(editor->textRange(start_pos, end_pos));
				for (int i = 0; i < text.size(); ++i)
				{
					if (text[i].isLetter())
					{
						if (i < 1 || (i >= 1 && !text[i - 1].isLetterOrNumber()))
						{
							text[i] = text[i].toUpper();
						}
					}
				}
				// 替换
				editor->setTargetRange(start_pos, end_pos);
				editor->replaceTarget(-1, text.toUtf8().constData());
				editor->setSel(start_pos, end_pos);
			}
		});
	m_messageBus->Subscribe("Sentence case", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* editor = m_textWidget[index];
				// 获取位置
				sptr_t start_pos = editor->selectionStart();
				sptr_t end_pos = editor->selectionEnd();
				// 获取文本
				QString text = QString::fromUtf8(editor->textRange(start_pos, end_pos));
				bool is_new_sentence = true;
				bool was_eol_r = false;
				bool was_eol_n = false;
				for (int i = 0; i < text.size(); ++i)
				{
					if (text[i].isLetter())
					{
						if (is_new_sentence)
						{
							text[i] = text[i].toUpper();
							is_new_sentence = false;
						}
						else
						{
							text[i] = text[i].toLower();
						}
						was_eol_r = false;
						was_eol_n = false;
						if (text[i] == "i" &&
							((i < 1) ? false : (text[i - 1].isSpace() || text[i - 1] == '(' || text[i - 1] == '"')) &&
							((i + 1 == text.size()) ? false : (text[i + 1].isSpace() || text[i + 1] == '\'')))
						{
							text[i] = 'I';
						}
					}
					else if (text[i] == '.' || text[i] == '!' || text[i] == '?')
					{
						if ((i + 1 == text.size()) ? true : text[i].isLetterOrNumber())
						{
							is_new_sentence = false;
						}
						else
						{
							is_new_sentence = true;
						}
					}
					else if (text[i] == '\r')
					{
						if (was_eol_r)
						{
							is_new_sentence = true;
						}
						else
						{
							was_eol_r = true;
						}
					}
					else if (text[i] == '\n')
					{
						if (was_eol_n)
						{
							is_new_sentence = true;
						}
						else
						{
							was_eol_n = true;
						}
					}
				}
				// 替换
				editor->setTargetRange(start_pos, end_pos);
				editor->replaceTarget(-1, text.toUtf8().constData());
				editor->setSel(start_pos, end_pos);
			}
		});
	m_messageBus->Subscribe("Sentence case(blend)", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* editor = m_textWidget[index];
				// 获取位置
				sptr_t start_pos = editor->selectionStart();
				sptr_t end_pos = editor->selectionEnd();
				// 获取文本
				QString text = QString::fromUtf8(editor->textRange(start_pos, end_pos));
				bool is_new_sentence = true;
				bool was_eol_r = false;
				bool was_eol_n = false;
				for (int i = 0; i < text.size(); ++i)
				{
					if (text[i].isLetter())
					{
						if (is_new_sentence)
						{
							text[i] = text[i].toUpper();
							is_new_sentence = false;
						}
						was_eol_r = false;
						was_eol_n = false;
						if (text[i] == "i" &&
							((i < 1) ? false : (text[i - 1].isSpace() || text[i - 1] == '(' || text[i - 1] == '"')) &&
							((i + 1 == text.size()) ? false : (text[i + 1].isSpace() || text[i + 1] == '\'')))
						{
							text[i] = 'I';
						}
					}
					else if (text[i] == '.' || text[i] == '!' || text[i] == '?')
					{
						if ((i + 1 == text.size()) ? true : text[i].isLetterOrNumber())
						{
							is_new_sentence = false;
						}
						else
						{
							is_new_sentence = true;
						}
					}
					else if (text[i] == '\r')
					{
						if (was_eol_r)
						{
							is_new_sentence = true;
						}
						else
						{
							was_eol_r = true;
						}
					}
					else if (text[i] == '\n')
					{
						if (was_eol_n)
						{
							is_new_sentence = true;
						}
						else
						{
							was_eol_n = true;
						}
					}
				}
				// 替换
				editor->setTargetRange(start_pos, end_pos);
				editor->replaceTarget(-1, text.toUtf8().constData());
				editor->setSel(start_pos, end_pos);
			}
		});
	m_messageBus->Subscribe("iNVERT cASE", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* editor = m_textWidget[index];
				// 获取位置
				sptr_t start_pos = editor->selectionStart();
				sptr_t end_pos = editor->selectionEnd();
				// 获取文本
				QString text = QString::fromUtf8(editor->textRange(start_pos, end_pos));
				for (int i = 0; i < text.size(); ++i)
				{
					if (text[i].isLower())
					{
						text[i] = text[i].toUpper();
					}
					else
					{
						text[i] = text[i].toLower();
					}
				}
				// 替换
				editor->setTargetRange(start_pos, end_pos);
				editor->replaceTarget(-1, text.toUtf8().constData());
				editor->setSel(start_pos, end_pos);
			}
		});
	m_messageBus->Subscribe("ranDOm CasE", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* editor = m_textWidget[index];
				// 获取位置
				sptr_t start_pos = editor->selectionStart();
				sptr_t end_pos = editor->selectionEnd();
				// 获取文本
				QString text = QString::fromUtf8(editor->textRange(start_pos, end_pos));
				for (int i = 0; i < text.size(); ++i)
				{
					if (text[i].isLetter())
					{
						if (std::rand() & true)
						{
							text[i] = text[i].toUpper();
						}
						else
						{
							text[i] = text[i].toLower();
						}
					}
				}
				// 替换
				editor->setTargetRange(start_pos, end_pos);
				editor->replaceTarget(-1, text.toUtf8().constData());
				editor->setSel(start_pos, end_pos);
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
				CustomTextEdit* editor = m_textWidget[index];
				sptr_t from_line = 0;
				sptr_t to_line = 0;
				if (editor->selections() > 1)
				{
					// Alt
					if (editor->selectionIsRectangle() || editor->selectionMode() == SC_SEL_THIN)
					{
						// 获取行号
						sptr_t rect_sel_anchor = editor->rectangularSelectionAnchor();
						sptr_t rect_sel_caret = editor->rectangularSelectionCaret();
						from_line = editor->lineFromPosition(rect_sel_anchor);
						to_line = editor->lineFromPosition(rect_sel_caret);
					}
					else
					{
						return;
					}
				}
				else
				{
					if (editor->selectionStart() == editor->selectionEnd())
					{
						// 获取行号
						from_line = 0;
						to_line = editor->lineCount() - 1;
					}
					else
					{
						// 获取行号
						sptr_t start_pos = editor->selectionStart();
						sptr_t end_pos = editor->selectionEnd();
						from_line = editor->lineFromPosition(start_pos);
						to_line = editor->lineFromPosition(end_pos);
						if ((from_line != to_line) && (static_cast<size_t>(editor->positionFromLine(to_line)) == end_pos))
						{
							--to_line;
						}
					}
				}
				// 获取位置
				sptr_t start_pos = editor->positionFromLine(from_line);
				sptr_t end_pos = editor->positionFromLine(to_line) + editor->lineLength(to_line);
				// 反转文本
				QString text = QString::fromUtf8(editor->textRange(start_pos, end_pos));
				QStringList text_list = text.split(editor->GetEOLString());
				bool sort_entire_document = to_line == editor->lineCount() - 1;
				if (sort_entire_document == false)
				{
					if (text_list.rbegin()->isEmpty())
					{
						text_list.pop_back();
					}
				}
				std::reverse(text_list.begin(), text_list.end());
				QString joined_text = text_list.join(editor->GetEOLString());
				if (sort_entire_document == true)
				{
					assert(joined_text.length() == text.length());
				}
				else
				{
					assert(joined_text.length() + editor->GetEOLString().length() == text.length());
					joined_text += editor->GetEOLString();
				}
				// 替换
				editor->setTargetRange(start_pos, end_pos);
				editor->replaceTarget(-1, joined_text.toUtf8().constData());
			}
		});
	m_messageBus->Subscribe("Randomize Line Order", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* editor = m_textWidget[index];
				sptr_t from_line = 0;
				sptr_t to_line = 0;
				if (editor->selections() > 1)
				{
					// Alt
					if (editor->selectionIsRectangle() || editor->selectionMode() == SC_SEL_THIN)
					{
						// 获取行号
						sptr_t rect_sel_anchor = editor->rectangularSelectionAnchor();
						sptr_t rect_sel_caret = editor->rectangularSelectionCaret();
						from_line = editor->lineFromPosition(rect_sel_anchor);
						to_line = editor->lineFromPosition(rect_sel_caret);
					}
					else
					{
						return;
					}
				}
				else
				{
					if (editor->selectionStart() == editor->selectionEnd())
					{
						// 获取行号
						from_line = 0;
						to_line = editor->lineCount() - 1;
					}
					else
					{
						// 获取行号
						sptr_t start_pos = editor->selectionStart();
						sptr_t end_pos = editor->selectionEnd();
						from_line = editor->lineFromPosition(start_pos);
						to_line = editor->lineFromPosition(end_pos);
						if ((from_line != to_line) && (static_cast<size_t>(editor->positionFromLine(to_line)) == end_pos))
						{
							--to_line;
						}
					}
				}
				// 获取位置
				sptr_t start_pos = editor->positionFromLine(from_line);
				sptr_t end_pos = editor->positionFromLine(to_line) + editor->lineLength(to_line);
				// 反转文本
				QString text = QString::fromUtf8(editor->textRange(start_pos, end_pos));
				QStringList text_list = text.split(editor->GetEOLString());
				bool sort_entire_document = to_line == editor->lineCount() - 1;
				if (sort_entire_document == false)
				{
					if (text_list.rbegin()->isEmpty())
					{
						text_list.pop_back();
					}
				}
				unsigned seed = static_cast<unsigned>(time(NULL));
				std::shuffle(text_list.begin(), text_list.end(), std::default_random_engine(seed));
				QString joined_text = text_list.join(editor->GetEOLString());
				if (sort_entire_document == true)
				{
					assert(joined_text.length() == text.length());
				}
				else
				{
					assert(joined_text.length() + editor->GetEOLString().length() == text.length());
					joined_text += editor->GetEOLString();
				}
				// 替换
				editor->setTargetRange(start_pos, end_pos);
				editor->replaceTarget(-1, joined_text.toUtf8().constData());
			}
		});
	m_messageBus->Subscribe("EOL Conversion", [=](int eolMode)
		{
			qDebug() << "This file is " << __FILE__ << " on line " << __LINE__;
			qDebug(Q_FUNC_INFO);

			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* editor = m_textWidget[index];
				editor->convertEOLs(SC_EOL_CRLF);
				editor->setEOLMode(SC_EOL_CRLF);
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

	// Debug
	m_messageBus->Subscribe("Debug", [=]()
		{
			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				// 选择的区域个数
				sptr_t sel1 = m_textWidget[index]->selections();
				// 获取矩形选择的锚点位置
				sptr_t sel2 = m_textWidget[index]->rectangularSelectionAnchor();
				qDebug() << "Debug";
			}
		});
}

void MainCore::InitConnect()
{
	qDebug() << "This file is " << __FILE__ << " on line " << __LINE__;
	qDebug(Q_FUNC_INFO);
	// 初次
	int index = m_centralWidget->currentIndex();
	if (index >= 0)
	{
		CustomTextEdit* editor = m_textWidget[index];
		m_messageBus->Publish("Update Window Title");
		m_messageBus->Publish("Update Status Bar", editor);
	}
	connect(m_centralWidget, &CustomTabWidget::currentChanged, [=]()
		{
			qDebug() << "This file is " << __FILE__ << " on line " << __LINE__;
			qDebug(Q_FUNC_INFO);

			int index = m_centralWidget->currentIndex();
			if (index >= 0)
			{
				CustomTextEdit* editor = m_textWidget[index];
				m_messageBus->Publish("Update Window Title");
				m_messageBus->Publish("Update Menu Bar", editor);
				m_messageBus->Publish("Update Status Bar", editor);
			}
		});
	connect(m_centralWidget, &CustomTabWidget::tabCloseRequested, [=](int index)
		{
			m_messageBus->Publish("Close File", index);
		});
	connect(m_tabBar, &CustomTabBar::tabMoved, [=](int from, int to)
		{
			m_textWidget.swapItemsAt(from, to);
		});
}

bool MainCore::NewFile(const QString& new_file_name)
{
	CustomTextEdit* text_widget = new CustomTextEdit(m_messageBus, m_centralWidget);
	text_widget->SetFileName(new_file_name);
	text_widget->SetFilePath("");
	text_widget->SetSaveStatus(true);
	text_widget->convertEOLs(SC_EOL_CRLF);
	text_widget->setEOLMode(SC_EOL_CRLF);
	connect(text_widget, &CustomTextEdit::savePointChanged, [=]()
		{
			m_messageBus->Publish("Text Changed");
		});
	connect(text_widget, &CustomTextEdit::updateUi, [=](Scintilla::Update updated)
		{
			qDebug() << "This file is " << __FILE__ << " on line " << __LINE__;
			qDebug(Q_FUNC_INFO);

			CustomTextEdit* editor = qobject_cast<CustomTextEdit*>(sender());
			if (Scintilla::FlagSet(updated, Scintilla::Update::Content))
			{
				m_messageBus->Publish("Update Document Size", text_widget);
			}
			if (Scintilla::FlagSet(updated, Scintilla::Update::Content) || Scintilla::FlagSet(updated, Scintilla::Update::Selection))
			{
				m_messageBus->Publish("Update Selection Info", text_widget);
			}
		});
	m_textWidget.append(text_widget);
	m_centralWidget->addTab(text_widget, QIcon(":/OneNotePad/standard/tabbar/saved.ico"), new_file_name);
	m_centralWidget->setCurrentIndex(m_centralWidget->count() - 1);
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
			text_widget->setText(in.readAll().toUtf8().constData());
			file.close();
			text_widget->SetFileName(file_info.fileName());
			text_widget->SetFilePath(abs_file_path);
			text_widget->SetSaveStatus(true);
			text_widget->convertEOLs(SC_EOL_CRLF);
			text_widget->setEOLMode(SC_EOL_CRLF);
			connect(text_widget, &CustomTextEdit::savePointChanged, [=]()
				{
					m_messageBus->Publish("Text Changed");
				});
			connect(text_widget, &CustomTextEdit::updateUi, [=](Scintilla::Update updated)
				{
					qDebug() << "This file is " << __FILE__ << " on line " << __LINE__;
					qDebug(Q_FUNC_INFO);

					CustomTextEdit* editor = qobject_cast<CustomTextEdit*>(sender());
					if (Scintilla::FlagSet(updated, Scintilla::Update::Content))
					{
						m_messageBus->Publish("Update Document Size", text_widget);
					}
					if (Scintilla::FlagSet(updated, Scintilla::Update::Content) || Scintilla::FlagSet(updated, Scintilla::Update::Selection))
					{
						m_messageBus->Publish("Update Selection Info", text_widget);
					}
				});
			m_textWidget.append(text_widget);
			m_centralWidget->addTab(text_widget, QIcon(":/OneNotePad/standard/tabbar/saved.ico"), file_info.fileName());
			m_centralWidget->setCurrentIndex(m_centralWidget->count() - 1);
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
	return true;
}

bool MainCore::LoadSettings()
{
	QStringList&& opened_file_name = m_settings->value("MainCore/OpenedFileNames").toStringList();
	QStringList&& opened_file_path = m_settings->value("MainCore/OpenedFilePaths").toStringList();
	QList<bool>&& saved_file = m_settings->BoolList("MainCore/SavedFile");
	int current_index = m_settings->value("MainCore/CurrentIndex").toInt();
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
			text_widget->setText(in.readAll().toUtf8().constData());
			file.close();
			text_widget->SetFileName(opened_file_name[index]);
			text_widget->SetFilePath(opened_file_path[index]);
			text_widget->SetSaveStatus(saved_file[index]);
			text_widget->convertEOLs(SC_EOL_CRLF);
			text_widget->setEOLMode(SC_EOL_CRLF);
			connect(text_widget, &CustomTextEdit::savePointChanged, [=]()
				{
					m_messageBus->Publish("Text Changed");
				});
			connect(text_widget, &CustomTextEdit::updateUi, [=](Scintilla::Update updated)
				{
					qDebug() << "This file is " << __FILE__ << " on line " << __LINE__;
					qDebug(Q_FUNC_INFO);

					CustomTextEdit* editor = qobject_cast<CustomTextEdit*>(sender());
					if (Scintilla::FlagSet(updated, Scintilla::Update::Content))
					{
						m_messageBus->Publish("Update Document Size", text_widget);
					}

					if (Scintilla::FlagSet(updated, Scintilla::Update::Content) || Scintilla::FlagSet(updated, Scintilla::Update::Selection))
					{
						m_messageBus->Publish("Update Selection Info", text_widget);
					}
				});
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