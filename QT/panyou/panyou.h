#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_panyou.h"
#include "painterWidget.h"
#include "Filework.h"
#include <QStandardItemModel>
#include <QStatusBar>
#include <QLabel>
#include <qdebug.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QTreeView>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QTableView>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextEdit>
#include <QTimer>
#include <QThread>
QT_BEGIN_NAMESPACE
namespace Ui { class panyouClass; };
QT_END_NAMESPACE

class panyou : public QMainWindow
{
    Q_OBJECT

public:
    panyou(QWidget *parent = nullptr);
    ~panyou();
	void setupUI();
	void openNewWindow();
    void reverseTable();
    void clearSmallNumbers();
    void filterOddRows();
    void handleToolbarAction();
    void setupTable();
    void setupTree();

    void setupTcpUI();
   
    void onTcpSendToggle();
	void onSelectFilepath();
	void onCreateDatabase();
	void onSortDatabase();
	void onFilterDatabase();
    

    void handleWriteProgress(int value);
    void handleWriteFinished();
    void handleWriteError(const QString& msg);
private:
    Ui::panyouClass *ui;
    QStandardItemModel* tableModel;
    QStandardItemModel* treeModel;
    QTableView* tableView;
    QTreeView* treeView;
	MyWidget* myWidget;
    QTextEdit* tcpTextEdit;
	QPushButton* tcpButton;
    QPushButton* fileButton;
	QPushButton* dbCreateButton;
	QPushButton* dbSortButton;
    QPushButton* dbFilterButton;
    QTcpSocket* tcpSocket;
    QTimer* tcpTimer;  
    int sendCounter = 0;
    // 数据库
    QSqlDatabase db;

    // 文件线程
    FileWriterThread* m_fileThread = nullptr;
};
