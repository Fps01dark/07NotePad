#pragma once

#include <QStatusBar>

class QLabel;
class MessageBus;
class CustomTextEdit;

class CustomStatusBar : public QStatusBar
{
public:
	explicit CustomStatusBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~CustomStatusBar();
	void Refresh(CustomTextEdit* editor);

private:
	void InitUi();
	void InitValue();
	void InitConnect();

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;
	QLabel* m_docType = nullptr;
	QLabel* m_docSize = nullptr;
	QLabel* m_docPos = nullptr;
	QLabel* m_eolFormat = nullptr;
	QLabel* m_unicodeType = nullptr;
	QLabel* m_overType = nullptr;
};
