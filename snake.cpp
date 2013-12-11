#include "snake.h"

Snake::Snake(QObject *parent) :
    QObject(parent)
{
    m_nodes[SnakeBody] << mkNode(QPoint(0, 0), Right);
    m_nodes[SnakeBody] << mkNode(QPoint(1, 0), Right);
    m_nodes[SnakeBody] << mkNode(QPoint(2, 0), Right);
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

Snake::Node Snake::mkNode(QPoint pos, Orientation orientation)
{
    Node n;
    n.pos = pos;
    n.orientation = orientation;
    return n;
}

void Snake::start()
{
    this->god->start(200);
}

void Snake::tick()
{
    QPoint newCoords = this->m_nodes[SnakeBody].last().pos
            + orientationPoint(this->m_nodes[SnakeBody].last().orientation);

    // teleportation:
    if (newCoords.x() >= this->size().width())
        newCoords.setX(0);
    if (newCoords.y() >= this->size().height())
        newCoords.setY(0);
    if (newCoords.x() < 0)
        newCoords.setX(this->size().width() - 1);
    if (newCoords.y() < 0)
        newCoords.setY(this->size().height() - 1);

    this->m_nodes[SnakeBody].append(
                mkNode(newCoords
                       , this->m_nodes[SnakeBody].last().orientation)
                );
    this->m_nodes[SnakeBody].removeFirst();
    emit refreshNodes();
}

void Snake::orient(Orientation o)
{
    this->m_nodes[SnakeBody].last().orientation = o;
    emit refreshNodes();
}

Snake::NodeMap Snake::nodes()
{
    return this->m_nodes;
}

QSize Snake::size()
{
    return this->m_size;
}
