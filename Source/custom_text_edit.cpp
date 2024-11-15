#include "custom_text_edit.h"

#include "framework.h"

#include "message_bus.h"

CustomTextEdit::CustomTextEdit(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	:m_messageBus(message_bus),
	QTextEdit(parent)
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
			m_messageBus->Publish("Change Zoom", current_size + 1);
		}
		else
		{
			m_messageBus->Publish("Change Zoom", current_size - 1);
		}
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