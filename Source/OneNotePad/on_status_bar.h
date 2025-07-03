#pragma once

#include <QStatusBar>

class QLabel;
class MessageBus;
class OnTextEdit;

class OnStatusBar : public QStatusBar
{
public:
	explicit OnStatusBar(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~OnStatusBar();
	void Refresh(OnTextEdit* editor);

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
