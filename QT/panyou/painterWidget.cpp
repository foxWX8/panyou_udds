#include "painterWidget.h"

MyWidget::MyWidget(QWidget* parent) : QWidget(parent) {}

void MyWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    // 加载图片（使用资源路径）
    QPixmap pixmap(":/panyou/img/rain.png");

    // 缩放图片以适应控件大小
    pixmap = pixmap.scaled(this->size(), Qt::KeepAspectRatio);

    // 居中绘制
    int x = (width() - pixmap.width()) / 2;
    int y = (height() - pixmap.height()) / 2;
    painter.drawPixmap(x, y, pixmap);

}