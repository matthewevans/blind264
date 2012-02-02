#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // init PythonQt and Python itself
    PythonQt::init();

    m_pqt = PythonQt::self();

    connect(m_pqt, SIGNAL(pythonStdOut(QString)), this, SLOT(pythonStdOut(QString)));
    connect(m_pqt, SIGNAL(pythonStdErr(QString)), this, SLOT(pythonStdOut(QString)));

    // get a smart pointer to the __main__ module of the Python interpreter
    m_context = m_pqt->getMainModule();
    m_context.addVariable("test", QVariant("mooo"));
    m_context.evalFile("helloworld.py");

    QVariant foo = m_context.getVariable("foo");
    QList<QVariant> fooList = foo.toList();
    QList<double> valList;

    foreach (QVariant f, fooList) {
	valList << f.toDouble();
    }

    qDebug() << "foo = " << valList;

    qDebug() << "program finished";
}

void MainWindow::pythonStdOut(QString out) {
    qDebug() << out;

    if (out == "Enter something: ") {
	qDebug() << "need user input";
	int num = QInputDialog::getInt(this, "Input", "Enter #:");
	m_context.addVariable("opt", QVariant(num));
    }
}

MainWindow::~MainWindow()
{
    
}
