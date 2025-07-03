#include "on_main_window.h"

#include "on_menu_bar.h"
#include "on_tab_widget.h"
#include "on_text_edit.h"
#include "on_tool_bar.h"
#include "on_core.h"

OnMainWindow::OnMainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	m_mainCore = new OnCore(this);

	InitUi();
	InitValue();
	InitConnect();
}

OnMainWindow::~OnMainWindow() {}

void OnMainWindow::closeEvent(QCloseEvent* event) { m_mainCore->ExitSoftware(); }

void OnMainWindow::InitUi() {}
void OnMainWindow::InitValue() {}

void OnMainWindow::InitConnect() {}