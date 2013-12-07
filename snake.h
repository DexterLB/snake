#ifndef SNAKE_H
#define SNAKE_H

#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QSize>

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
        NodeType type;
        Orientation orientation;
    } Node;

    explicit Snake(QObject *parent = 0);

    static Node mkNode(QPoint pos, NodeType type, Orientation orientation);
    static QPoint orientationPoint(Orientation o);

    QList<Node> nodes();

    QSize size();

signals:
    void refreshNodes();

public slots:
    void start();
    void orient(Orientation o);

private slots:
    void tick();

private:
    QList<Node> snakeBody;
    QTimer *god;
    QSize m_size;
};

#endif // SNAKE_H
