#include "recent_files_manager.h"

#include "framework.h"

namespace
{
	const int MAX_HISTORY_FILE_SIZE = 10;
}

RecentFilesManager::RecentFilesManager(QObject* parent)
	:QObject(parent)
{
}

void RecentFilesManager::AddFile(const QString& filePath)
{
	// Attempt to remove it first to make sure it is not added twice
	RemoveFile(filePath);

	// Set a limit on how many can be in the list
	if (m_recentFiles.size() >= MAX_HISTORY_FILE_SIZE)
	{
		m_recentFiles.removeLast();
	}

	m_recentFiles.prepend(filePath);
}

void RecentFilesManager::RemoveFile(const QString& filePath)
{
	m_recentFiles.removeOne(filePath);
}

void RecentFilesManager::Clear()
{
	// Clear the file list
	m_recentFiles.clear();
}

QString RecentFilesManager::MostRecentFile() const
{
	Q_ASSERT(!m_recentFiles.empty());

	return m_recentFiles.first();
}

QStringList RecentFilesManager::GetRecentFiles() const
{
	return m_recentFiles;
}

void RecentFilesManager::SetRecentFiles(const QStringList& list)
{
	Clear();
	m_recentFiles.append(list);
}