#pragma once

#include <QObject>

class List;

class RecentFilesManager : public QObject
{
	Q_OBJECT
public:
	explicit RecentFilesManager(QObject* parent = nullptr);
	QString MostRecentFile() const;
	QStringList GetRecentFiles() const;
	void SetRecentFiles(const QStringList& list);
	int Count() const { return m_recentFiles.size(); }
	void AddFile(const QString& filePath);
	void RemoveFile(const QString& filePath);
	void Clear();

private:
	QStringList m_recentFiles;
};
