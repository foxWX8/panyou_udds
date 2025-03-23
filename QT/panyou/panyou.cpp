#include "panyou.h"
#pragma execution_character_set("utf-8")
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextCodec>
panyou::panyou(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::panyouClass())
{
    ui->setupUi(this);
    setWindowTitle(u8"�������-����-2025-3-20"); // ���ߺ�����
	resize(1280, 720); // ���ô��ڴ�С
    setupUI();
    
    // ��ʼ�����
    setupTable();
    setupTree();
    // ��ʼ�����ݿ�
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");

    // TCP
    setupTcpUI();

    // ������
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    myWidget = new MyWidget(this);


    // �������
    QHBoxLayout* tableLayout = new QHBoxLayout();
    QPushButton* reverseBtn = new QPushButton(u8"������", this);
    tableLayout->addWidget(tableView);
    tableLayout->addWidget(reverseBtn);

    // ����ͼ����
    QHBoxLayout* treeLayout = new QHBoxLayout();
    QVBoxLayout* treeBtnLayout = new QVBoxLayout();
    QPushButton* btnClear = new QPushButton(u8"���С��ֵ", this);
    QPushButton* btnFilter = new QPushButton(u8"����������", this);
    treeBtnLayout->addWidget(btnClear);
    treeBtnLayout->addWidget(btnFilter);
    treeBtnLayout->addStretch();
    treeLayout->addWidget(treeView);
    treeLayout->addLayout(treeBtnLayout);

    //
    QGroupBox* networkGroup = new QGroupBox(u8"����ͨ��");
    QVBoxLayout* networkLayout = new QVBoxLayout();
	tcpTextEdit = new QTextEdit(this);
    networkLayout->addWidget(tcpTextEdit);
	tcpButton = new QPushButton(u8"��ʼ����", this);
    networkLayout->addWidget(tcpButton);
    networkGroup->setLayout(networkLayout);

    // �ļ�����
    QGroupBox* fileGroup = new QGroupBox(u8"�ļ�����");
    QVBoxLayout* fileLayout = new QVBoxLayout();
	fileButton = new QPushButton(u8"ѡ���ļ�·��", this);
    fileLayout->addWidget(fileButton);
    fileGroup->setLayout(fileLayout);

    // ���ݿ�����
    QGroupBox* dbGroup = new QGroupBox(u8"���ݿ����");
    QHBoxLayout* dbLayout = new QHBoxLayout();
	dbCreateButton = new QPushButton(u8"�������ݿ�", this);
    dbLayout->addWidget(dbCreateButton);
	dbSortButton = new QPushButton(u8"�������ݿ�", this);
    dbLayout->addWidget(dbSortButton);
	dbFilterButton = new QPushButton(u8"ɸѡ���ݿ�", this);
    dbLayout->addWidget(dbFilterButton);
    dbGroup->setLayout(dbLayout);

  
    // ��ϲ���
    mainLayout->addLayout(tableLayout);
    mainLayout->addLayout(treeLayout);
    mainLayout->addWidget(networkGroup);
    mainLayout->addWidget(fileGroup);
    mainLayout->addWidget(dbGroup);
    mainLayout->addWidget(myWidget);
	myWidget->setMinimumHeight(100);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // �����źŲ�
    connect(reverseBtn, &QPushButton::clicked, this, &panyou::reverseTable);
    connect(btnClear, &QPushButton::clicked, this, &panyou::clearSmallNumbers);
    connect(btnFilter, &QPushButton::clicked, this, &panyou::filterOddRows);
    connect(tcpButton, &QPushButton::clicked, this, &panyou::onTcpSendToggle);
    connect(fileButton, &QPushButton::clicked, this, &panyou::onSelectFilepath);
    connect(dbCreateButton, &QPushButton::clicked, this, &panyou::onCreateDatabase);
    connect(dbSortButton, &QPushButton::clicked, this, &panyou::onSortDatabase);
    connect(dbFilterButton, &QPushButton::clicked, this, &panyou::onFilterDatabase);
    

   

   
}

panyou::~panyou()
{
    if (m_fileThread) {
        if (m_fileThread->isRunning()) {
            m_fileThread->requestInterruption();
            m_fileThread->wait(3000);
        }
        delete m_fileThread;
    }
    delete ui;
}

void panyou::setupUI()
{
    // �˵���
    QMenuBar* menuBar = this->menuBar();
    QMenu* fileMenu = menuBar->addMenu(u8"�ļ�");
    QMenu* viewMenu = menuBar->addMenu(u8"��ͼ");
    QMenu* settingsMenu = menuBar->addMenu(u8"����");
    QMenu* helpMenu = menuBar->addMenu(u8"����");

    // ��ͼ�˵������ҳ��
    QAction* newPageAction = new QAction(u8"����ҳ��", this);
    viewMenu->addAction(newPageAction);
    connect(newPageAction, &QAction::triggered, this, &panyou::openNewWindow);

    // ������
    QToolBar* toolBar = addToolBar(u8"����");
    QAction* act1 = new QAction(QIcon(":/panyou/img/clear_b.png"), u8"����1", this);
    QAction* act2 = new QAction(QIcon(":/panyou/img/connect.png"), u8"����2", this);
    QAction* act3 = new QAction(QIcon(":/panyou/img/data.png"), u8"����3", this);
    QAction* act4 = new QAction(QIcon(":/panyou/img/clear_yu.png"), u8"����4", this);
    QAction* act5 = new QAction(QIcon(":/panyou/img/net_on.png"), u8"����5", this);


    // ���5������...
    toolBar->addAction(act1);
	toolBar->addAction(act2);
	toolBar->addAction(act3);
	toolBar->addAction(act4);
	toolBar->addAction(act5);
    // ���Ӷ����Ĵ����źŵ��ۺ���
    connect(act1, &QAction::triggered, []() { qDebug() << "����1������"; });
	connect(act2, &QAction::triggered, []() { qDebug() << "����2������"; });
	connect(act3, &QAction::triggered, []() { qDebug() << "����3������"; });
	connect(act4, &QAction::triggered, []() { qDebug() << "����4������"; });
	connect(act5, &QAction::triggered, []() { qDebug() << "����5������"; });

    // ״̬��
    QStatusBar* statusBar = this->statusBar();
    QLabel* leftLabel = new QLabel(u8"���������", this);
    QLabel* rightLabel1 = new QLabel(u8"��1", this);
    QLabel* rightLabel2 = new QLabel(u8"��2", this);
    statusBar->addWidget(leftLabel); // Ĭ�������
    statusBar->addPermanentWidget(rightLabel1); // �Ҷ���
    statusBar->addPermanentWidget(rightLabel2);

	

    
}

void panyou::openNewWindow()
{
	panyou* newWindow = new panyou();
	newWindow->show();
}


void panyou::reverseTable()
{
    QList<QList<QStandardItem*>> rows;
    for (int i = 0; i < tableModel->rowCount(); ++i) {
        rows.append(tableModel->takeRow(0));
    }
    std::reverse(rows.begin(), rows.end());
    foreach(auto row, rows) {
        tableModel->appendRow(row);
    }
}

void panyou::clearSmallNumbers()
{
    QStandardItem* root = treeModel->invisibleRootItem();
    for (int i = 0; i < root->rowCount(); ++i) {
        QStandardItem* parent = root->child(i);
        for (int j = 0; j < parent->rowCount(); ++j) {
            QStandardItem* item = parent->child(j);
            int val = item->text().toInt();
            if (val <= 25) item->setText("0");
        }
    }
}

void panyou::filterOddRows()
{
    QStandardItem* root = treeModel->invisibleRootItem();
    for (int i = root->rowCount() - 1; i >= 0; --i) {
        if (i % 2 == 0) root->removeRow(i);
    }
}

void panyou::handleToolbarAction()
{
    qDebug() << u8"����������������:" << qobject_cast<QAction*>(sender())->text();
}

void panyou::setupTable()
{
    tableView = new QTableView(this);
    tableModel = new QStandardItemModel(8, 5, this);
    tableModel->setHorizontalHeaderLabels({ u8"����", u8"����", u8"�Ա�", u8"�ɼ�", u8"�ȼ�" });

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 5; col++) {
            QStandardItem* item = new QStandardItem(QString(u8"����%1-%2").arg(row).arg(col));
            tableModel->setItem(row, col, item);
        }
    }
    tableView->setModel(tableModel);
}

void panyou::setupTree()
{
    treeView = new QTreeView(this);
    treeModel = new QStandardItemModel(this);
    treeModel->setHorizontalHeaderLabels({ u8"���������" });

    QStandardItem* root = treeModel->invisibleRootItem();
    for (int i = 0; i < 8; i++) {
        QStandardItem* parent = new QStandardItem(QString(u8"�ڵ�%1").arg(i + 1));
        for (int j = 0; j < 5; j++) {
            int num = QRandomGenerator::global()->bounded(50);
            QStandardItem* child = new QStandardItem(QString::number(num));
            parent->appendRow(child);
        }
        root->appendRow(parent);
    }
    treeView->setModel(treeModel);
    treeView->expandAll();
}

void panyou::setupTcpUI()
{
    tcpSocket = new QTcpSocket(this);
    tcpTimer = new QTimer(this);
    connect(tcpTimer, &QTimer::timeout, [this]() {
        QString msg = QString(u8"��Ϣ%1").arg(++sendCounter);
        tcpSocket->write(msg.toUtf8());
        tcpTextEdit->append(u8"����: " + msg.toUtf8());
        });
}

void panyou::onTcpSendToggle()
{
    if (tcpTimer->isActive()) {
        tcpTimer->stop();
        tcpSocket->disconnectFromHost();
        tcpButton->setText(u8"��ʼ����");
    }
    else {
        tcpSocket->connectToHost("127.0.0.1", 8888);
        tcpTimer->start(500);
        tcpButton->setText(u8"ֹͣ����");
    }
}

void panyou::onSelectFilepath()
{
    // ���ð�ť��ֹ�ظ�����
    fileButton->setEnabled(false);

    QString path = QFileDialog::getExistingDirectory(this, "ѡ�񱣴�·��");
    if (path.isEmpty()) {
        fileButton->setEnabled(true);
        return;
    }

    // ��ֹ֮ǰ���̣߳�������ڣ�
    if (m_fileThread && m_fileThread->isRunning()) {
        m_fileThread->requestInterruption();
        m_fileThread->wait();
    }
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    // �������߳�
    m_fileThread = new FileWriterThread(path, this);

    // �����źŲ�
	
    connect(m_fileThread, &FileWriterThread::finished, this,
        &panyou::handleWriteFinished);
    connect(m_fileThread, &FileWriterThread::errorOccurred, this,
        &panyou::handleWriteError);
    connect(m_fileThread, &FileWriterThread::finished, m_fileThread,
        &QObject::deleteLater);

    // �����߳�
    m_fileThread->start();
}

void panyou::handleWriteProgress(int value)
{
    ui->statusBar->showMessage("����д��");
}

void panyou::handleWriteFinished()
{
    fileButton->setEnabled(true);
    ui->statusBar->showMessage("�ļ�д�����", 5000);
    m_fileThread = nullptr;
}

void panyou::handleWriteError(const QString& msg)
{
    fileButton->setEnabled(true);
    QMessageBox::critical(this, "����", msg);
    m_fileThread = nullptr;
}


void panyou::onCreateDatabase()
{
    if (!db.open()) {
        QMessageBox::critical(this, "����", "���ݿ�����ʧ��");
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS ���ݿ��1 ("
        "��� INTEGER PRIMARY KEY AUTOINCREMENT,"
        "�����1 INTEGER,"
        "�����2 INTEGER,"
        "�����3 INTEGER)");

    db.transaction();
    for (int i = 0; i < 1000; i++) {
        query.prepare("INSERT INTO ���ݿ��1 (�����1, �����2, �����3) VALUES (?,?,?)");
        query.addBindValue(QRandomGenerator::global()->bounded(3000));
        query.addBindValue(QRandomGenerator::global()->bounded(3000));
        query.addBindValue(QRandomGenerator::global()->bounded(3000));
        query.exec();
    }
    db.commit();

    QMessageBox::information(this, "���", "�Ѵ���1000����¼");
    db.close();
}

void panyou::onSortDatabase()
{
    if (!db.open()) return;
    
    QSqlQuery query("SELECT * FROM ���ݿ��1 ORDER BY �����2 DESC");
	while (query.next()) {
		qDebug() << QString("���:%1 ��1:%2 ��2:%3 ��3:%4")
			.arg(query.value(0).toInt())
			.arg(query.value(1).toInt())
			.arg(query.value(2).toInt())
			.arg(query.value(3).toInt());
	}
    // ������������ʾ�ڱ����
    db.close();
}

void panyou::onFilterDatabase()
{
    if (!db.open()) return;

    QSqlQuery query;
    query.exec("SELECT * FROM ���ݿ��1 WHERE �����3 % 2 = 1");
    while (query.next()) {
        qDebug() << QString("���:%1 ��1:%2 ��2:%3 ��3:%4")
            .arg(query.value(0).toInt())
            .arg(query.value(1).toInt())
            .arg(query.value(2).toInt())
            .arg(query.value(3).toInt());
    }

    query.exec("DELETE FROM ���ݿ��1 WHERE �����3 % 2 = 0");
    db.close();
}

