#ifndef SNAKE_H
#define SNAKE_H

#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QSize>
#include <QMap>
#include <QHash>
#include <QList>
#include <QtDebug>

class Snake : public QObject
{
    Q_OBJECT
public:
    enum Orientation {
        Left,
        Right,
        Up,
        Down,
        Nowhere
    };

    enum NodeType {
        SnakeBody,
        Apple
    };

    enum GameState {
        Playing,
        Paused,
        Stopped,
        Over,
        Undefined
    };

    typedef struct {
        QPoint pos;
        Orientation orientation;
        NodeType type;
    } Node;

    typedef QMap< NodeType, QList<Node*> > NodeMap;

    explicit Snake(QObject *parent = 0);

    static Node* mkNode(QPoint pos, NodeType type, Orientation orientation);
    static QPoint orientationPoint(Orientation o);

    NodeMap nodes();

    QSize size();

    GameState state();

signals:
    void refreshNodes();
    void stateChanged(GameState state);

public slots:
    bool start();
    bool pause();
    void init();
    void orient(Orientation o);

private slots:
    void tick();
    void setState(GameState state);

private:
    NodeMap m_nodes;
    QHash<QPoint, Node*> gridLookup;

    bool addNode(Node *n);
    void delNode(NodeType type, int at);
    bool delNode(QPoint pos);   // slow
    void clearNodes();


    QList<Node> snakeBody;
    QTimer *god;
    QSize m_size;

    GameState m_state;

    void newApple(QPoint except = QPoint());
    QPoint rndPoint();
};

#endif // SNAKE_H
