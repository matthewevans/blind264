#ifndef LOGGER_H
#define LOGGER_H

#include <QtGui>

class Logger : public QObject
{
    Q_OBJECT
public:
    static void init(QString);
    static void log(QString);
    static QString logDir();

signals:
    
public slots:

private:
    static Logger *m_instance;

    QString m_appPath;
    QFile *m_logFile;
    bool m_failedAccess;
};

#endif // LOGGER_H
