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

};

#endif // MAINWINDOW_H
