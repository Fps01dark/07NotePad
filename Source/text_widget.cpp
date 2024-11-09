#include "text_widget.h"

#include "framework.h"

TextWidget::TextWidget(QWidget* parent)
	:QWidget(parent)
{
	ui.setupUi(this);

	InitUi();
	InitValue();
	InitConnect();
}

TextWidget::~TextWidget()
{
}

void TextWidget::SetText(const QString& text)
{
	m_textEdit->setText(text);
}

QString TextWidget::GetText() const
{
	return m_textEdit->toPlainText();
}

void TextWidget::InitUi()
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	m_textEdit = new QTextEdit(this);
	layout->addWidget(m_textEdit);
	setLayout(layout);
}

void TextWidget::InitValue()
{
}

void TextWidget::InitConnect()
{
}