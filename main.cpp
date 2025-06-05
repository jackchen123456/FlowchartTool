#include "MainWindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDir>
#include <QFile>

QString getQmlPath()
{
	// 调试目录
	QString appDir = QCoreApplication::applicationDirPath();
	QDir currentDir(appDir);

	QString qmlFilePath = QDir::cleanPath(currentDir.absolutePath() + QDir::separator() +
		QString("mui") + QDir::separator() +
		QString("zh_CN") + QDir::separator() +
		QString("lessoncode.qm")
	);

	if (QFile::exists(qmlFilePath))
	{
		return qmlFilePath;
	}

	return QString();

}

int main(int argc, char* argv[]) {
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);      // 自适应dpi
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	QApplication app(argc, argv);
	QApplication::setFont(QFont("Microsoft YaHei", 10));

	QTranslator translator;
	if (translator.load(getQmlPath()))
	{
		app.installTranslator(&translator);
	}

	MainWindow window;
	window.show();

	return app.exec();
}
