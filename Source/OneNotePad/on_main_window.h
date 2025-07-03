#pragma once

#include <QMainWindow>
#include <QSet>

class QString;
class QCloseEvent;
class OnCore;

class OnMainWindow final : public QMainWindow
{
	Q_OBJECT
public:
	explicit OnMainWindow(QWidget* parent = nullptr);
	~OnMainWindow();

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	// M
	OnCore* m_mainCore = nullptr;
};
