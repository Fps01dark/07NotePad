#include "one_notepad_application.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

OneNotepadApplication::OneNotepadApplication(int& argc, char** argv)
	:SingleApplication(argc, argv)
{
#ifdef Q_OS_WIN
	// 创建一个系统范围的互斥锁，以便安装程序可以检测它是否正在运行
	CreateMutex(NULL, false, L"OneNotepadMutex");
#endif
}

OneNotepadApplication::~OneNotepadApplication()
{
}