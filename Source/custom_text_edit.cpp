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

void CustomTextEdit::wheelEvent(QWheelEvent* event)
{
	if (event->modifiers() & Qt::ControlModifier)
	{
		int delta = event->angleDelta().y();
		QFont font = this->font();
		int current_size = font.pointSize();
		if (delta > 0)
		{
			font.setPointSize(current_size + 1);  // 放大
		}
		else
		{
			font.setPointSize(current_size - 1);  // 缩小
		}
		this->setFont(font);
	}
	else
	{
		QTextEdit::wheelEvent(event);  // 非Ctrl键盘缩放时正常处理
	}
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