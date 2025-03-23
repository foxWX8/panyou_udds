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
    setWindowTitle(u8"界面程序-王旭-2025-3-20"); // 作者和日期
	resize(1280, 720); // 设置窗口大小
    setupUI();
    
    // 初始化组件
    setupTable();
    setupTree();
    // 初始化数据库
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");

    // TCP
    setupTcpUI();

    // 主布局
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    myWidget = new MyWidget(this);


    // 表格区域
    QHBoxLayout* tableLayout = new QHBoxLayout();
    QPushButton* reverseBtn = new QPushButton(u8"倒序表格", this);
    tableLayout->addWidget(tableView);
    tableLayout->addWidget(reverseBtn);

    // 树形图区域
    QHBoxLayout* treeLayout = new QHBoxLayout();
    QVBoxLayout* treeBtnLayout = new QVBoxLayout();
    QPushButton* btnClear = new QPushButton(u8"清除小数值", this);
    QPushButton* btnFilter = new QPushButton(u8"保留单数行", this);
    treeBtnLayout->addWidget(btnClear);
    treeBtnLayout->addWidget(btnFilter);
    treeBtnLayout->addStretch();
    treeLayout->addWidget(treeView);
    treeLayout->addLayout(treeBtnLayout);

    //
    QGroupBox* networkGroup = new QGroupBox(u8"网络通信");
    QVBoxLayout* networkLayout = new QVBoxLayout();
	tcpTextEdit = new QTextEdit(this);
    networkLayout->addWidget(tcpTextEdit);
	tcpButton = new QPushButton(u8"开始发送", this);
    networkLayout->addWidget(tcpButton);
    networkGroup->setLayout(networkLayout);

    // 文件区域
    QGroupBox* fileGroup = new QGroupBox(u8"文件操作");
    QVBoxLayout* fileLayout = new QVBoxLayout();
	fileButton = new QPushButton(u8"选择文件路径", this);
    fileLayout->addWidget(fileButton);
    fileGroup->setLayout(fileLayout);

    // 数据库区域
    QGroupBox* dbGroup = new QGroupBox(u8"数据库操作");
    QHBoxLayout* dbLayout = new QHBoxLayout();
	dbCreateButton = new QPushButton(u8"创建数据库", this);
    dbLayout->addWidget(dbCreateButton);
	dbSortButton = new QPushButton(u8"排序数据库", this);
    dbLayout->addWidget(dbSortButton);
	dbFilterButton = new QPushButton(u8"筛选数据库", this);
    dbLayout->addWidget(dbFilterButton);
    dbGroup->setLayout(dbLayout);

  
    // 组合布局
    mainLayout->addLayout(tableLayout);
    mainLayout->addLayout(treeLayout);
    mainLayout->addWidget(networkGroup);
    mainLayout->addWidget(fileGroup);
    mainLayout->addWidget(dbGroup);
    mainLayout->addWidget(myWidget);
	myWidget->setMinimumHeight(100);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // 连接信号槽
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
    // 菜单栏
    QMenuBar* menuBar = this->menuBar();
    QMenu* fileMenu = menuBar->addMenu(u8"文件");
    QMenu* viewMenu = menuBar->addMenu(u8"视图");
    QMenu* settingsMenu = menuBar->addMenu(u8"设置");
    QMenu* helpMenu = menuBar->addMenu(u8"帮助");

    // 视图菜单项：打开新页面
    QAction* newPageAction = new QAction(u8"打开新页面", this);
    viewMenu->addAction(newPageAction);
    connect(newPageAction, &QAction::triggered, this, &panyou::openNewWindow);

    // 工具栏
    QToolBar* toolBar = addToolBar(u8"工具");
    QAction* act1 = new QAction(QIcon(":/panyou/img/clear_b.png"), u8"操作1", this);
    QAction* act2 = new QAction(QIcon(":/panyou/img/connect.png"), u8"操作2", this);
    QAction* act3 = new QAction(QIcon(":/panyou/img/data.png"), u8"操作3", this);
    QAction* act4 = new QAction(QIcon(":/panyou/img/clear_yu.png"), u8"操作4", this);
    QAction* act5 = new QAction(QIcon(":/panyou/img/net_on.png"), u8"操作5", this);


    // 添加5个动作...
    toolBar->addAction(act1);
	toolBar->addAction(act2);
	toolBar->addAction(act3);
	toolBar->addAction(act4);
	toolBar->addAction(act5);
    // 连接动作的触发信号到槽函数
    connect(act1, &QAction::triggered, []() { qDebug() << "操作1被触发"; });
	connect(act2, &QAction::triggered, []() { qDebug() << "操作2被触发"; });
	connect(act3, &QAction::triggered, []() { qDebug() << "操作3被触发"; });
	connect(act4, &QAction::triggered, []() { qDebug() << "操作4被触发"; });
	connect(act5, &QAction::triggered, []() { qDebug() << "操作5被触发"; });

    // 状态栏
    QStatusBar* statusBar = this->statusBar();
    QLabel* leftLabel = new QLabel(u8"左对齐区域", this);
    QLabel* rightLabel1 = new QLabel(u8"右1", this);
    QLabel* rightLabel2 = new QLabel(u8"右2", this);
    statusBar->addWidget(leftLabel); // 默认左对齐
    statusBar->addPermanentWidget(rightLabel1); // 右对齐
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
    qDebug() << u8"工具栏操作被触发:" << qobject_cast<QAction*>(sender())->text();
}

void panyou::setupTable()
{
    tableView = new QTableView(this);
    tableModel = new QStandardItemModel(8, 5, this);
    tableModel->setHorizontalHeaderLabels({ u8"姓名", u8"年龄", u8"性别", u8"成绩", u8"等级" });

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 5; col++) {
            QStandardItem* item = new QStandardItem(QString(u8"数据%1-%2").arg(row).arg(col));
            tableModel->setItem(row, col, item);
        }
    }
    tableView->setModel(tableModel);
}

void panyou::setupTree()
{
    treeView = new QTreeView(this);
    treeModel = new QStandardItemModel(this);
    treeModel->setHorizontalHeaderLabels({ u8"随机数数据" });

    QStandardItem* root = treeModel->invisibleRootItem();
    for (int i = 0; i < 8; i++) {
        QStandardItem* parent = new QStandardItem(QString(u8"节点%1").arg(i + 1));
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
        QString msg = QString(u8"消息%1").arg(++sendCounter);
        tcpSocket->write(msg.toUtf8());
        tcpTextEdit->append(u8"发送: " + msg.toUtf8());
        });
}

void panyou::onTcpSendToggle()
{
    if (tcpTimer->isActive()) {
        tcpTimer->stop();
        tcpSocket->disconnectFromHost();
        tcpButton->setText(u8"开始发送");
    }
    else {
        tcpSocket->connectToHost("127.0.0.1", 8888);
        tcpTimer->start(500);
        tcpButton->setText(u8"停止发送");
    }
}

void panyou::onSelectFilepath()
{
    // 禁用按钮防止重复操作
    fileButton->setEnabled(false);

    QString path = QFileDialog::getExistingDirectory(this, "选择保存路径");
    if (path.isEmpty()) {
        fileButton->setEnabled(true);
        return;
    }

    // 终止之前的线程（如果存在）
    if (m_fileThread && m_fileThread->isRunning()) {
        m_fileThread->requestInterruption();
        m_fileThread->wait();
    }
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    // 创建新线程
    m_fileThread = new FileWriterThread(path, this);

    // 连接信号槽
	
    connect(m_fileThread, &FileWriterThread::finished, this,
        &panyou::handleWriteFinished);
    connect(m_fileThread, &FileWriterThread::errorOccurred, this,
        &panyou::handleWriteError);
    connect(m_fileThread, &FileWriterThread::finished, m_fileThread,
        &QObject::deleteLater);

    // 启动线程
    m_fileThread->start();
}

void panyou::handleWriteProgress(int value)
{
    ui->statusBar->showMessage("正在写入");
}

void panyou::handleWriteFinished()
{
    fileButton->setEnabled(true);
    ui->statusBar->showMessage("文件写入完成", 5000);
    m_fileThread = nullptr;
}

void panyou::handleWriteError(const QString& msg)
{
    fileButton->setEnabled(true);
    QMessageBox::critical(this, "错误", msg);
    m_fileThread = nullptr;
}


void panyou::onCreateDatabase()
{
    if (!db.open()) {
        QMessageBox::critical(this, "错误", "数据库连接失败");
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS 数据库表1 ("
        "序号 INTEGER PRIMARY KEY AUTOINCREMENT,"
        "随机数1 INTEGER,"
        "随机数2 INTEGER,"
        "随机数3 INTEGER)");

    db.transaction();
    for (int i = 0; i < 1000; i++) {
        query.prepare("INSERT INTO 数据库表1 (随机数1, 随机数2, 随机数3) VALUES (?,?,?)");
        query.addBindValue(QRandomGenerator::global()->bounded(3000));
        query.addBindValue(QRandomGenerator::global()->bounded(3000));
        query.addBindValue(QRandomGenerator::global()->bounded(3000));
        query.exec();
    }
    db.commit();

    QMessageBox::information(this, "完成", "已创建1000条记录");
    db.close();
}

void panyou::onSortDatabase()
{
    if (!db.open()) return;
    
    QSqlQuery query("SELECT * FROM 数据库表1 ORDER BY 随机数2 DESC");
	while (query.next()) {
		qDebug() << QString("序号:%1 数1:%2 数2:%3 数3:%4")
			.arg(query.value(0).toInt())
			.arg(query.value(1).toInt())
			.arg(query.value(2).toInt())
			.arg(query.value(3).toInt());
	}
    // 排序结果可以显示在表格中
    db.close();
}

void panyou::onFilterDatabase()
{
    if (!db.open()) return;

    QSqlQuery query;
    query.exec("SELECT * FROM 数据库表1 WHERE 随机数3 % 2 = 1");
    while (query.next()) {
        qDebug() << QString("序号:%1 数1:%2 数2:%3 数3:%4")
            .arg(query.value(0).toInt())
            .arg(query.value(1).toInt())
            .arg(query.value(2).toInt())
            .arg(query.value(3).toInt());
    }

    query.exec("DELETE FROM 数据库表1 WHERE 随机数3 % 2 = 0");
    db.close();
}

