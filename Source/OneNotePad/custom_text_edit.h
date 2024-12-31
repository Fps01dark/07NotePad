#pragma once

#include <QTextEdit>

class QWidget;
class QWheelEvent;
class MessageBus;

class CustomTextEdit : public QTextEdit {
    Q_OBJECT
   public:
    explicit CustomTextEdit(std::shared_ptr<MessageBus> message_bus, QWidget* parent = nullptr);
    ~CustomTextEdit();
    void    SetText(const QString& text);
    QString GetText() const;

   protected:
    void wheelEvent(QWheelEvent* event) override;

   private:
    void InitUi();
    void InitValue();
    void InitConnect();

   private:
    std::shared_ptr<MessageBus> m_messageBus = nullptr;
};
