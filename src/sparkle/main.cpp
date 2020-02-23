/*
 * Copyright (C) 2008 Remko Troncon
 */

#include <QApplication>
#include <QLabel>

#ifdef Q_OS_MAC
#include "CocoaInitializer.h"
#include "SparkleAutoUpdater.h"
#endif

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	QLabel l("This is an auto-updating application");
	l.show();

	AutoUpdater* updater;
#ifdef Q_OS_MAC
	CocoaInitializer initializer;
	updater = new SparkleAutoUpdater("http://el-tramo.be/files/blog/mixing-cocoa-and-qt/appcast.xml");
#endif
	if (updater) {
		updater->checkForUpdates();
	}
	return app.exec();
}
