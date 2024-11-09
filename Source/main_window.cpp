#pragma execution_character_set("utf-8")
#include "main_window.h"

#include "framework.h"

#include "text_widget.h"
#include "custom_menu_bar.h"
#include "custom_tool_bar.h"
#include "custom_tab_widget.h"
#include "main_core.h"

MainWindow::MainWindow(QWidget* parent)
	:QMainWindow(parent)
{
	ui.setupUi(this);

	m_mainCore = new MainCore(this, this);

	InitUi();
	InitValue();
	InitConnect();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent* event)
{

}

void MainWindow::InitUi()
{
	// 标题
	setWindowTitle("NotePad");
	// 菜单栏
	CustomMenuBar* menu_bar = m_mainCore->GetCustomMenuBar();
	setMenuBar(menu_bar);
	// 工具栏
	CustomToolBar* tool_bar = m_mainCore->GetCustomToolBar();
	tool_bar->setMovable(false);
	addToolBar(Qt::TopToolBarArea, tool_bar);
	// 中心区域
	CustomTabWidget* central_widget = m_mainCore->GetCustomTabWidget();
	setCentralWidget(central_widget);
}
void MainWindow::InitValue()
{
}

void MainWindow::InitConnect()
{
}