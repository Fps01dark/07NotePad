#include "on_application.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include <QObject>
#include <QIcon>

OnApplication::OnApplication(int& argc, char** argv)
	:SingleApplication(argc, argv)
{
#ifdef Q_OS_WIN
	// 创建一个系统范围的互斥锁，以便安装程序可以检测它是否正在运行
	CreateMutex(NULL, false, L"OneNotepadMutex");
#endif
	InitUi();
	InitValue();
	InitConnect();
}

OnApplication::~OnApplication()
{
}

void OnApplication::InitUi()
{
	setWindowIcon(QIcon(":/Icons/Icons/one_notepad.ico"));
}

void OnApplication::InitValue()
{
}

void OnApplication::InitConnect()
{
}