#include "snake.h"

Snake::Snake(QObject *parent) :
    QObject(parent)
{
    this->addNode(mkNode(QPoint(0, 0), SnakeBody, Right));
    this->addNode(mkNode(QPoint(1, 0), SnakeBody, Right));
    this->addNode(mkNode(QPoint(2, 0), SnakeBody, Right));

    this->god = new QTimer();
    this->m_size = QSize(50, 50);

    connect(this->god, SIGNAL(timeout()), this, SLOT(tick()));
}

bool Snake::addNode(Node *n)
{
    if (this->gridLookup.contains(n->pos)) {
        return false;
    }
    this->gridLookup[n->pos] = n;
    this->m_nodes[n->type].append(n);

    return true;
}

bool Snake::delNode(QPoint pos)
{
    if (!this->gridLookup.contains(pos)) {
        return false;
    }
    Node *n = this->gridLookup[pos];

    this->m_nodes[n->type].removeOne(n);
    this->gridLookup.remove(pos);

    delete n;
    return true;
}

void Snake::delNode(NodeType type, int at)
{
    Node *n = this->m_nodes[type].at(at);
    this->gridLookup.remove(n->pos);
    this->m_nodes[type].removeAt(at);
    delete n;
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

Snake::Node* Snake::mkNode(QPoint pos, NodeType type, Orientation orientation)
{
    Node* n = new Node;
    n->pos = pos;
    n->orientation = orientation;
    n->type = type;
    return n;
}

void Snake::start()
{
    this->god->start(200);
    this->newApple();
}

void Snake::newApple(QPoint except)
{
    QPoint p;
    do {
        p = this->rndPoint();
    } while ((!p.isNull() && p == except) || this->gridLookup.contains(p));
    this->addNode(this->mkNode(p, Apple, Nowhere));
    emit refreshNodes();
}

QPoint Snake::rndPoint()
{
    return QPoint(qrand() % this->size().width(),
                  qrand() % this->size().height());
}

void Snake::tick()
{
    QPoint newCoords = this->m_nodes[SnakeBody].last()->pos
            + orientationPoint(this->m_nodes[SnakeBody].last()->orientation);

    // teleportation:
    if (newCoords.x() >= this->size().width())
        newCoords.setX(0);
    if (newCoords.y() >= this->size().height())
        newCoords.setY(0);
    if (newCoords.x() < 0)
        newCoords.setX(this->size().width() - 1);
    if (newCoords.y() < 0)
        newCoords.setY(this->size().height() - 1);


    if (this->gridLookup.contains(newCoords) &&
            this->gridLookup.value(newCoords)->type == Apple) {
        // delete the apple and make a new one
        this->delNode(newCoords);
        this->newApple(newCoords);
    } else {
        // remove a node at the "tail" only if not growing
        this->delNode(SnakeBody, 0);
    }

    // add a node at the "head"
    this->addNode(mkNode(newCoords, SnakeBody
        , this->m_nodes[SnakeBody].last()->orientation));

    emit refreshNodes();
}

void Snake::orient(Orientation o)
{
    this->m_nodes[SnakeBody].last()->orientation = o;
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

inline uint qHash(const QPoint &p)
{
    return qHash(((p.x() + p.y()) * (p.x() + p.y() + 1) + p.y()) / 2);
}
