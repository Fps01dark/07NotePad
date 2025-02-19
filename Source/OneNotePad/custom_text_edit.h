#pragma once

#include <ScintillaEdit.h>

class QWidget;
class QWheelEvent;
class MessageBus;

class CustomTextEdit : public ScintillaEdit
{
	Q_OBJECT
public:
	explicit CustomTextEdit(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
	~CustomTextEdit();
	QString GetFileName() const;
	void SetFileName(const QString& file_name);
	QString GetFilePath() const;
	void SetFilePath(const QString& file_path);
	bool GetSaveStatus() const;
	void SetSaveStatus(bool save_status);
	void Cut();
	QString GetEOLString() const;

protected:
	void wheelEvent(QWheelEvent* event) override;

private:
	void InitUi();
	void InitValue();
	void InitConnect();
	void SetupEditor();

private:
	std::shared_ptr<MessageBus> m_messageBus = nullptr;

	QString m_fileName;
	QString m_filePath;
	bool m_savedStatus;
};
