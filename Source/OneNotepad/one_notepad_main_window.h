#pragma once

#include <QMainWindow>
#include <QSet>

#include "ui_one_notepad_main_window.h"

class QString;
class QCloseEvent;
class OneNotepadCore;

class OneNotepadMainWindow final : public QMainWindow
{
	Q_OBJECT
public:
	explicit OneNotepadMainWindow(QWidget* parent = nullptr);
	~OneNotepadMainWindow();

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	// V
	Ui::OneNotepadMainWindow ui;

	// M
	OneNotepadCore* m_mainCore = nullptr;
};
