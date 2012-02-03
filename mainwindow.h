#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "PythonQt.h"
#include <QtGui>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void pythonStdOut(QString);
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    PythonQt *m_pqt;
    PythonQtObjectPtr m_context;

    QLineEdit *m_srcFile;
    QLineEdit *m_outFile;
    QPushButton *m_srcChooseBtn;
    QPushButton *m_outChooseBtn;

    QComboBox *m_grainySource;
    QDoubleSpinBox *m_bframes;
    QDoubleSpinBox *m_psyrdStart;
    QDoubleSpinBox *m_psyrdEnd;

    QDoubleSpinBox *m_crfStart;
    QDoubleSpinBox *m_crfEnd;
    QDoubleSpinBox *m_qcompStart;
    QDoubleSpinBox *m_qcompEnd;

    QDoubleSpinBox *m_aqstrengthStart;
    QDoubleSpinBox *m_aqstrengthEnd;

    QLineEdit *m_customField;

    QPushButton *m_startBtn;
    QPushButton *m_quitBtn;

    void setupComponents();

private slots:
    void doStart();
    void doQuit();

    void chooseSrcFile();
    void chooseOutFile();
};

#endif // MAINWINDOW_H
