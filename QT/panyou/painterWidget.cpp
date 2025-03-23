#include "painterWidget.h"

MyWidget::MyWidget(QWidget* parent) : QWidget(parent) {}

void MyWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    // ����ͼƬ��ʹ����Դ·����
    QPixmap pixmap(":/panyou/img/rain.png");

    // ����ͼƬ����Ӧ�ؼ���С
    pixmap = pixmap.scaled(this->size(), Qt::KeepAspectRatio);

    // ���л���
    int x = (width() - pixmap.width()) / 2;
    int y = (height() - pixmap.height()) / 2;
    painter.drawPixmap(x, y, pixmap);

}