#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // init PythonQt and Python itself
    PythonQt::init();

    m_pqt = PythonQt::self();

    // set up signals/slots
    connect(m_pqt, SIGNAL(pythonStdOut(QString)), this, SLOT(pythonStdOut(QString)));
    connect(m_pqt, SIGNAL(pythonStdErr(QString)), this, SLOT(pythonStdOut(QString)));

    // get a smart pointer to the __main__ module of the Python interpreter
    m_context = m_pqt->getMainModule();
    m_context.evalFile("helloworld.py");

    // Initialize UI components
    setupComponents();

    this->resize(this->sizeHint());

    // Center the window
    this->setGeometry(
	QStyle::alignedRect(
	    Qt::LeftToRight,
	    Qt::AlignCenter,
	    this->size(),
	    qApp->desktop()->availableGeometry()
	));
}

void MainWindow::pythonStdOut(QString out) {
    qDebug() << out;

    if (out == "Enter something: ") {
	qDebug() << "need user input";
	int num = QInputDialog::getInt(this, "Input", "Enter #:");
	m_context.addVariable("opt", QVariant(num));
    }
}

void MainWindow::setupComponents() {
    // Create group box and init form layout
    QGroupBox *inputGroupBox = new QGroupBox("Input / Output");
    QFormLayout *inputLayout = new QFormLayout;
    inputGroupBox->setLayout(inputLayout);

    // Just a label...
    inputLayout->addRow("Source", new QWidget());

    // Source file text box/button
    m_srcFile = new QLineEdit;
    m_srcFile->setMinimumWidth(500);
    m_srcChooseBtn = new QPushButton("...");
    m_srcChooseBtn->setMaximumWidth(50);
    inputLayout->addRow(m_srcFile, m_srcChooseBtn);

    inputLayout->addRow("Output", new QWidget());

    // Output file text box/button
    m_outFile = new QLineEdit;
    m_outFile->setMinimumWidth(500);
    m_outChooseBtn = new QPushButton("...");
    m_outChooseBtn->setMaximumWidth(50);
    inputLayout->addRow(m_outFile, m_outChooseBtn);

    QHBoxLayout *colLayout = new QHBoxLayout;
    QFormLayout *colOneLayout = new QFormLayout;
    QFormLayout *colTwoLayout = new QFormLayout;
    QVBoxLayout *customLayout = new QVBoxLayout;
    QHBoxLayout *btnLayout = new QHBoxLayout;
    colLayout->addLayout(colOneLayout);
    colLayout->addSpacing(50);
    colLayout->addLayout(colTwoLayout);

    // First column
    m_grainySource = new QComboBox;
    m_grainySource->addItems(QStringList() << "Yes" << "No");
    colOneLayout->addRow("Grainy Source:", m_grainySource);

    m_bframes = new QDoubleSpinBox;
    m_bframes->setSingleStep(0.1);
    colTwoLayout->addRow("bframes:", m_bframes);

    m_psyrdStart = new QDoubleSpinBox;
    m_psyrdEnd = new QDoubleSpinBox;

    QHBoxLayout *psyrdLayout = new QHBoxLayout;
    psyrdLayout->setAlignment(Qt::AlignLeft);
    psyrdLayout->addWidget(m_psyrdStart);
    psyrdLayout->addWidget(new QLabel("to"));
    psyrdLayout->addWidget(m_psyrdEnd);

    colOneLayout->addRow("psy-rd:", psyrdLayout);

    // Second column
    m_crfStart = new QDoubleSpinBox;
    m_crfEnd = new QDoubleSpinBox;

    QHBoxLayout *crfLayout = new QHBoxLayout;
    crfLayout->setAlignment(Qt::AlignLeft);
    crfLayout->addWidget(m_crfStart);
    crfLayout->addWidget(new QLabel("to"));
    crfLayout->addWidget(m_crfEnd);

    QLabel *crfLabel = new QLabel("crf:");
    crfLabel->setAlignment(Qt::AlignRight);
    colTwoLayout->addRow(crfLabel, crfLayout);

    m_qcompStart = new QDoubleSpinBox;
    m_qcompEnd = new QDoubleSpinBox;

    QHBoxLayout *qcompLayout = new QHBoxLayout;
    qcompLayout->setAlignment(Qt::AlignLeft);
    qcompLayout->addWidget(m_qcompStart);
    qcompLayout->addWidget(new QLabel("to"));
    qcompLayout->addWidget(m_qcompEnd);

    colOneLayout->addRow("qcomp:", qcompLayout);

    // Third Column
    m_aqstrengthStart = new QDoubleSpinBox;
    m_aqstrengthEnd = new QDoubleSpinBox;

    QHBoxLayout *aqstrengthLayout = new QHBoxLayout;
    aqstrengthLayout->setAlignment(Qt::AlignLeft);
    aqstrengthLayout->addWidget(m_aqstrengthStart);
    aqstrengthLayout->addWidget(new QLabel("to"));
    aqstrengthLayout->addWidget(m_aqstrengthEnd);

    colTwoLayout->addRow("aq-strength:", aqstrengthLayout);

    QGroupBox *customBox = new QGroupBox("Custom x264 Parameters");
    customBox->setLayout(customLayout);

    m_customField = new QLineEdit;
    customLayout->addWidget(m_customField);

    m_startBtn = new QPushButton("&Start");
    m_quitBtn = new QPushButton("&Quit");

    connect(m_startBtn, SIGNAL(clicked()), this, SLOT(doStart()));
    connect(m_quitBtn, SIGNAL(clicked()), this, SLOT(doQuit()));

    QHBoxLayout *btns = new QHBoxLayout;
    btns->addWidget(m_quitBtn);
    btns->addWidget(m_startBtn);

    btnLayout->setAlignment(Qt::AlignRight);
    btnLayout->addWidget(m_quitBtn, 0, Qt::AlignRight);
    btnLayout->addWidget(m_startBtn, 0, Qt::AlignRight);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(inputGroupBox);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(colLayout);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(customBox);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(btnLayout);

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(mainLayout);

    this->setCentralWidget(mainWidget);
}

void MainWindow::doStart() {
    // Run Python Script here~
}

void MainWindow::doQuit() {
    qApp->exit();
}

MainWindow::~MainWindow()
{
    
}
