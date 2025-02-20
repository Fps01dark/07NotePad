#pragma once

#include <QLabel>
#include <QMouseEvent>

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget *parent=Q_NULLPTR, Qt::WindowFlags f=Qt::WindowFlags()) 
        : QLabel(parent, f) 
    {
    }
    explicit ClickableLabel(const QString &text, QWidget *parent=Q_NULLPTR, Qt::WindowFlags f=Qt::WindowFlags()) 
        : QLabel(text, parent, f) 
    {
    }
};


class StatusLabel : public ClickableLabel
{
    Q_OBJECT

public:
    explicit StatusLabel(int size = 200, QWidget *parent = 0)
        : ClickableLabel(parent), m_preferredSize(size) 
    {
        setTextFormat(Qt::PlainText);
    }

protected:
    QSize sizeHint() const override 
    {
        QSize size = ClickableLabel::sizeHint();
        size.setWidth(m_preferredSize);
        return size;
    }

    QSize minimumSizeHint() const override 
    { 
        return QSize(); 
    }

private:
    int m_preferredSize;
};

