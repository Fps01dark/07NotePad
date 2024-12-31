#include "main_window.h"

#include "custom_menu_bar.h"
#include "custom_tab_widget.h"
#include "custom_text_edit.h"
#include "custom_tool_bar.h"
#include "framework.h"
#include "main_core.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);

    m_mainCore = new MainCore(this);

    InitUi();
    InitValue();
    InitConnect();
}

MainWindow::~MainWindow() {}

void MainWindow::closeEvent(QCloseEvent* event) { m_mainCore->ExitSoftware(); }

void MainWindow::InitUi() {}
void MainWindow::InitValue() {}

void MainWindow::InitConnect() {}