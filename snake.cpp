#include "snake.h"

Snake::Snake(QObject *parent) :
    QObject(parent)
{
    snakeBody << mkNode(QPoint(0, 0), SnakeBody, Right);
    snakeBody << mkNode(QPoint(1, 0), SnakeBody, Right);
    snakeBody << mkNode(QPoint(2, 0), SnakeBody, Right);
    this->god = new QTimer();
    this->m_size = QSize(50, 50);

    connect(this->god, SIGNAL(timeout()), this, SLOT(tick()));
}

QPoint Snake::orientationPoint(Orientation o)
{
    switch(o) {
    case Up:
        return QPoint(0, -1);
    case Down:
        return QPoint(0, 1);
    case Left:
        return QPoint(-1, 0);
    case Right:
        return QPoint(1, 0);
    default:
        return QPoint(0, 0);
    }
}

Snake::Node Snake::mkNode(QPoint pos, NodeType type, Orientation orientation)
{
    Node n;
    n.pos = pos;
    n.type = type;
    n.orientation = orientation;
    return n;
}

void Snake::start()
{
    this->god->start(500);
}

void Snake::tick()
{
    this->snakeBody.removeFirst();
    this->snakeBody.append(
                mkNode(this->snakeBody.last().pos
                       + orientationPoint(this->snakeBody.last().orientation)
                       , SnakeBody, this->snakeBody.last().orientation)
                );
    emit refreshNodes();
}

QList<Snake::Node> Snake::nodes()
{
    return this->snakeBody;
}

QSize Snake::size()
{
    return this->m_size;
}
