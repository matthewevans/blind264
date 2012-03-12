#ifndef TESTRIG_H
#define TESTRIG_H

#include <QObject>
#include <QtCore>

class TestRig : public QObject
{
    Q_OBJECT
public:
    explicit TestRig(QString source, bool keep = false);

    void start();
    void setRange(int, int, int);
    QString toString();
    void override(QString, QStringList, QString msg = QString());
    QMap<QString, double> testEncode(QString outFile = "test.mkv", bool usePrevious = true, bool verbose = true);

    void bframes();
    
signals:
    void requestPrint(QString);
    
public slots:

private:
    QString m_source;
    bool m_keep;
    QMap<QString, QMap<QString, double> > m_results;
    QMap<QString, QString> m_params;
    QStringList m_avsText;
    QMap<QString, double> m_baseline;

    int m_frames;
    QSize m_dims;
    
};

#endif // TESTRIG_H
