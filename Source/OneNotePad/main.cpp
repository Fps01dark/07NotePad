#include <QApplication>
#include <QDataStream>
#include <QDebug>
#include <QSettings>
#include <QSysInfo>

#include "main_window.h"
#include "one_notepad_application.h"

int main(int argc, char* argv[])
{
	qSetMessagePattern(
		"[%{time process}] "
		"%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-"
		"fatal}F%{endif}: %{message}");

	// Set these since other parts of the app references these
	QApplication::setOrganizationName("OneNotePad");
	QApplication::setApplicationName("OneNotePad");
	QGuiApplication::setApplicationDisplayName("OneNotePad");
#ifdef Q_OS_UNIX
	// Unix doesn't provide an application version by default
	QGuiApplication::setApplicationVersion(APP_VERSION);
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

	// Default settings format
	QSettings::setDefaultFormat(QSettings::IniFormat);

	OneNotepadApplication app(argc, argv);

	// Log some debug info
	qInfo("=============================");
	qInfo("%s v%s", qUtf8Printable(QApplication::applicationDisplayName()),
		qUtf8Printable(QApplication::applicationVersion()));
	// qInfo("%s v%s%s", qUtf8Printable(QApplication::applicationDisplayName()),
	// qUtf8Printable(QApplication::applicationVersion()), APP_DISTRIBUTION);
	qInfo("Build Date/Time: %s %s", __DATE__, __TIME__);
	qInfo("Qt: %s", qVersion());
	qInfo("OS: %s", qUtf8Printable(QSysInfo::prettyProductName()));
	qInfo("Locale: %s", qUtf8Printable(QLocale::system().name()));
	qInfo("CPU: %s", qUtf8Printable(QSysInfo::currentCpuArchitecture()));
	qInfo("File Path: %s", qUtf8Printable(QApplication::applicationFilePath()));
	qInfo("Arguments: %s", qUtf8Printable(app.arguments().join(' ')));
	qInfo("=============================");

	if (app.isPrimary())
	{
		MainWindow w;
		w.show();
		return app.exec();
	}
	else
	{
		qInfo() << "Primary instance already running. PID:" << app.primaryPid();
		qInfo() << "Secondary instance closing...";
		app.exit(0);
		return 0;
	}
}