#include "custom_text_edit.h"

#include "framework.h"

CustomTextEdit::CustomTextEdit(QWidget* parent)
	:QTextEdit(parent)
{
	InitUi();
	InitValue();
	InitConnect();
}

CustomTextEdit::~CustomTextEdit()
{
}

void CustomTextEdit::SetText(const QString& text)
{
	setText(text);
}

QString CustomTextEdit::GetText() const
{
	return toPlainText();
}

void CustomTextEdit::InitUi()
{
}

void CustomTextEdit::InitValue()
{
}

void CustomTextEdit::InitConnect()
{
}