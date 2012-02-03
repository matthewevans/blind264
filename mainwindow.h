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
    QSpinBox *m_bframes;
    QSpinBox *m_psyrdStart;
    QSpinBox *m_psyrdEnd;

    QSpinBox *m_crfStart;
    QSpinBox *m_crfEnd;
    QSpinBox *m_qcompStart;
    QSpinBox *m_qcompEnd;

    QSpinBox *m_aqstrengthStart;
    QSpinBox *m_aqstrengthEnd;

    QLineEdit *m_customField;

    QPushButton *m_startBtn;
    QPushButton *m_quitBtn;

    void setupComponents();
};

#endif // MAINWINDOW_H
