#pragma execution_character_set("utf-8")

#include <QtWidgets/QApplication>

#include "main_window.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}