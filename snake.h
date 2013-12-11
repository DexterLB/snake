#ifndef SNAKE_H
#define SNAKE_H

#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QSize>
#include <QMap>
#include <QList>

class Snake : public QObject
{
    Q_OBJECT
public:
    enum Orientation {
        Left,
        Right,
        Up,
        Down
    };

    enum NodeType {
        SnakeBody
    };

    typedef struct {
        QPoint pos;
        Orientation orientation;
    } Node;

    typedef QMap< NodeType, QList<Node> > NodeMap;

    explicit Snake(QObject *parent = 0);

    static Node mkNode(QPoint pos, Orientation orientation);
    static QPoint orientationPoint(Orientation o);

    NodeMap nodes();

    QSize size();

signals:
    void refreshNodes();

public slots:
    void start();
    void orient(Orientation o);

private slots:
    void tick();

private:
    NodeMap m_nodes;
    QList<Node> snakeBody;
    QTimer *god;
    QSize m_size;
};

#endif // SNAKE_H
