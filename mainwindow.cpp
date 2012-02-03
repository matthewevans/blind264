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

    // Output file text box/button
    m_outFile = new QLineEdit;
    m_outFile->setMinimumWidth(500);
    m_outChooseBtn = new QPushButton("...");
    m_outChooseBtn->setMaximumWidth(50);
    inputLayout->addRow(m_outFile, m_outChooseBtn);

    QHBoxLayout *colLayout = new QHBoxLayout;
    QFormLayout *colOneLayout = new QFormLayout;
    QFormLayout *colTwoLayout = new QFormLayout;
    QFormLayout *colThreeLayout = new QFormLayout;
    QVBoxLayout *customLayout = new QVBoxLayout;
    QHBoxLayout *btnLayout = new QHBoxLayout;
    colLayout->addLayout(colOneLayout);
    colLayout->addLayout(colTwoLayout);
    colLayout->addLayout(colThreeLayout);

    // First column
    m_grainySource = new QComboBox;
    colOneLayout->addRow("Grainy Source:", m_grainySource);

    m_bframes = new QSpinBox;
    colOneLayout->addRow("bframes:", m_bframes);

    m_psyrdStart = new QSpinBox;
    colOneLayout->addRow("psy-rd start:", m_psyrdStart);

    m_psyrdEnd = new QSpinBox;
    colOneLayout->addRow("psy-rd end:", m_psyrdEnd);

    // Second column
    m_crfStart = new QSpinBox;
    colTwoLayout->addRow("crf start:", m_crfStart);

    m_crfEnd = new QSpinBox;
    colTwoLayout->addRow("crf end:", m_crfEnd);

    m_qcompStart = new QSpinBox;
    colTwoLayout->addRow("qcomp start:", m_qcompStart);

    m_qcompEnd = new QSpinBox;
    colTwoLayout->addRow("qcomp end:", m_qcompEnd);

    // Third Column
    m_aqstrengthStart = new QSpinBox;
    colThreeLayout->addRow("aq-strength start:", m_aqstrengthStart);

    m_aqstrengthEnd = new QSpinBox;
    colThreeLayout->addRow("aq-strength end:", m_aqstrengthEnd);

    QGroupBox *customBox = new QGroupBox("Custom Parameters");
    customBox->setLayout(customLayout);

    m_customField = new QLineEdit;
    customLayout->addWidget(new QLabel("Custom x264 Parameters"));
    customLayout->addWidget(m_customField);

    m_startBtn = new QPushButton("&Start");
    m_quitBtn = new QPushButton("&Quit");

    QHBoxLayout *btns = new QHBoxLayout;
    btns->addWidget(m_quitBtn);
    btns->addWidget(m_startBtn);

    btnLayout->setAlignment(Qt::AlignRight);
    btnLayout->addWidget(m_quitBtn, 0, Qt::AlignRight);
    btnLayout->addWidget(m_startBtn, 0, Qt::AlignRight);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(inputGroupBox);
    mainLayout->addLayout(colLayout);
    mainLayout->addWidget(customBox);
    mainLayout->addLayout(btnLayout);

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(mainLayout);

    this->setCentralWidget(mainWidget);
}

MainWindow::~MainWindow()
{
    
}
