#include "one_notepad_main_window.h"

#include "custom_menu_bar.h"
#include "custom_tab_widget.h"
#include "custom_text_edit.h"
#include "custom_tool_bar.h"
#include "one_notepad_core.h"

OneNotepadMainWindow::OneNotepadMainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_mainCore = new OneNotepadCore(this);

	InitUi();
	InitValue();
	InitConnect();
}

OneNotepadMainWindow::~OneNotepadMainWindow() {}

void OneNotepadMainWindow::closeEvent(QCloseEvent* event) { m_mainCore->ExitSoftware(); }

void OneNotepadMainWindow::InitUi() {}
void OneNotepadMainWindow::InitValue() {}

void OneNotepadMainWindow::InitConnect() {}