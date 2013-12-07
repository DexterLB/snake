#include "snake.h"

Snake::Snake(QObject *parent) :
    QObject(parent)
{
}

Snake::Node Snake::mkNode(QPoint pos, NodeType type)
{
    Node n;
    n.pos = pos;
    n.type = type;
    return n;
}
