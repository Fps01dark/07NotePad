#pragma once

#include <QWidget>

#include "ui_text_widget.h"

class QTextEdit;

class TextWidget :public QWidget
{
public:
	TextWidget(QWidget* parent = nullptr);
	~TextWidget();
	void SetText(const QString& text);
	QString GetText() const;
private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	Ui::TextWidget ui;
	QTextEdit* m_textEdit = nullptr;
};
