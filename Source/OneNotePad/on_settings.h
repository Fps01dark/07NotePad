#pragma once

#include <QSettings>

class OnSettings : public QSettings
{
	Q_OBJECT
public:
	explicit OnSettings(QObject* parent = nullptr);
	~OnSettings();
	void        SetBoolList(const QString& key, const QList<bool>& boolList);
	QList<bool> BoolList(const QString& key);

private:
};
