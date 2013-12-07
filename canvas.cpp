#include "canvas.h"

Canvas::Canvas(QWidget *parent) :
    QWidget(parent)
{
    /*
    this->setSizeIncrement(this->nodeSize());
    this->setBaseSize(this->matrixSize());
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    */
}

void Canvas::paintEvent(QPaintEvent * /* event */)
{
    qDebug() << "entered paint event";
    QPainter painter(this);
    QList<Snake::Node> nodeList = this->snake->nodes();
    for (int i = 0; i < nodeList.size(); ++i) {
        this->drawNode(&painter, nodeList.value(i));
    }
}

void Canvas::drawNode(QPainter *painter, Snake::Node node)
{
    painter->drawRect(QRect(this->pixelCoords(node.pos), this->nodeSize()));
}

QSize Canvas::matrixSize()
{
    return QSize(20, 20);   // nodes
}

QSize Canvas::nodeSize()
{
    return QSize(10, 10);   // pixels
}

QPoint Canvas::pixelCoords(QPoint coords)
{
    coords.setX(coords.x() * this->nodeSize().width());
    coords.setY(coords.y() * this->nodeSize().height());
    return coords;
}

void Canvas::setSnake(Snake *s)
{
    this->snake = s;
}
