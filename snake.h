#ifndef SNAKE_H
#define SNAKE_H

#include <QObject>
#include <QPoint>

class Snake : public QObject
{
    Q_OBJECT
public:
    enum NodeType {
        SnakeBody
    };

    typedef struct {
        QPoint pos;
        NodeType type;
    } Node;

    explicit Snake(QObject *parent = 0);

    static Node mkNode(QPoint pos, NodeType type);

signals:

public slots:

};

#endif // SNAKE_H
