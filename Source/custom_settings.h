#pragma once

#include <QSettings>

class CustomSettings : public QSettings {
    Q_OBJECT
   public:
    explicit CustomSettings(QObject* parent = nullptr);
    ~CustomSettings();
    void        SetBoolList(const QString& key, const QList<bool>& boolList);
    QList<bool> BoolList(const QString& key);

   private:
};
