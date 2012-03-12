#include "logger.h"
#include "mainwindow.h"

// Init static variable(s)
Logger* Logger::m_instance;

void Logger::init(QString appPath) {
    m_instance = new Logger;

    m_instance->m_appPath = appPath;

    if (!appPath.endsWith('/') && !appPath.endsWith('\\')) {
	appPath.append(QDir::separator());
	m_instance->m_appPath = appPath;
    }

    QDir logDir(appPath);

    if (!logDir.exists("log")) {
	if (!logDir.mkdir("log")) {
	    qDebug() << "Unable to create logging directory. Bailing.";
	    return;
	}
    }

    // Generate log file path /home/user/blind264/blind264.log
    appPath.append(QString("log").append(QDir::separator()).append("blind264.log"));

    // Initialize the log file object
    m_instance->m_logFile = new QFile(appPath);

    // Init flag(s)
    m_instance->m_failedAccess = false;

    log("blind264 version " + B264_VERSION + "\n\n");
}

QString Logger::logDir() {
    return m_instance->m_appPath;
}

void Logger::log(QString s) {
    if (!m_instance->m_logFile->open(QIODevice::WriteOnly) && !m_instance->m_failedAccess) {
	// Error opening log file
	qDebug() << "Error opening file: " << m_instance->m_logFile->errorString();
	QMessageBox::critical(0, "blind264", "Failed to open log file:\n" +
			      m_instance->m_logFile->errorString()
			      + "\nData will not be saved to disk");
	m_instance->m_failedAccess = true;
    }
}
