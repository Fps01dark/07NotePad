#include "main_window.h"

#include "framework.h"

#include "custom_menu_bar.h"
#include "custom_tool_bar.h"
#include "custom_text_edit.h"
#include "custom_tab_widget.h"
#include "main_core.h"

namespace
{
	const QString MAIN_WINDOW_INI_PATH = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/NotePad/Setting/main_window.ini";
}

MainWindow::MainWindow(QWidget* parent)
	:QMainWindow(parent)
{
	ui.setupUi(this);

	m_mainCore = new MainCore(this);

	InitUi();
	InitValue();
	InitConnect();
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	QSize new_size = event->size();
	QSettings settings(MAIN_WINDOW_INI_PATH, QSettings::IniFormat);
	settings.setValue("window/width", new_size.width());
	settings.setValue("window/height", new_size.height());
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	m_mainCore->ExitSoftware();
}

void MainWindow::InitUi()
{
	QSettings settings(MAIN_WINDOW_INI_PATH, QSettings::IniFormat);
	int width = settings.value("window/width", 1920).toInt();
	int height = settings.value("window/height", 1080).toInt();
	resize(width, height);
}
void MainWindow::InitValue()
{
}

void MainWindow::InitConnect()
{
}