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

/*!
 * \class Snake
 * \brief The actual game logic happens here
 * user interfaces should interact with this class
 */
class Snake : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief orientation direction
     * ordered clockwise
     */
    enum Orientation {
        Up,
        Right,
        Down,
        Left,
        Nowhere
    };

    /*!
     * \brief all possible node types
     */
    enum NodeType {
        SnakeBody,  //! a single "joint" of the snake's body
        Apple,      //! an "apple", which makes the snake grow when eaten
        Obstacle,   //! sudden death
        Void        //! passthrough
    };

    /*!
     * \brief game state
     */
    enum GameState {
        Playing,
        Paused,
        Stopped,
        Over,
        Undefined
    };

    /*!
     * \brief extra attributes (mostly for visual representation)
     */
    enum NodeAttribute {
        AttrSnakeTorso = 0,
        AttrSnakeHead = 1,
        AttrSnakeTail = 2,
        AttrApple = 3,
    };

    /*!
     * \brief node "bend", for strings of nodes that cut corners
     * namely the snake's body
     */
    enum Bend {
        BendNone,
        BendClockwise,
        BendCounterclockwise
    };

    /*!
     * \brief a single node
     * this is the main datatype for the game. a "node" is a single object
     * that might be placed on a certain position on the field. nodes can be
     * part of the snake, obstacles, powerups, etc
     */
    typedef struct {
        QPoint pos;                 //! the position on the field. NOT in pixels!
        Orientation orientation;    //! each node can have an orientation
        Bend bend;
        NodeAttribute attr;
        NodeType type;
    } Node;

    /*!
     * \brief by-id lookup table for all nodes
     * for each node type there's a list of nodes with that type.
     * this is especially useful for the snake's body, since the list can
     * be adequately used as a LIFO (QList uses an array of pointers to store
     * its data, and operations at the start and end are fast)
     */
    typedef QMap< NodeType, QList<Node*> > NodeMap;

    /*!
     * \brief by-coordinates lookup table for all nodes
     * this allows fast lookup by QPoint
     */
    typedef QHash<QPoint, Node*> NodeGrid;

    /*!
     * \brief constructor
     */
    explicit Snake(QObject *parent = 0);

    /*!
     * \brief create a node and return a pointer to it
     * a convenience function that creates a new struct
     */
    static Node* mkNode(QPoint pos, NodeType type, Orientation orientation, NodeAttribute attr, Bend bend);

    /*!
     * \brief get the vector of an Orientation
     * \param o the orientation
     * \return a vector QPoint
     * returns a (vector) point that lies at a distance 1 from (0, 0) at the
     * direction specified by o
     * in other words when added to another point it will move it with one
     * step at the given direction
     */
    static QPoint orientationPoint(Orientation o);

    /*!
     * \brief get the angle of an Orientation in degrees
     * \param o the orientation
     * \return angle in degrees (wtf, why degrees and not radians?)
     */
    static qreal orientationAngle(Orientation o);

    /*!
     * \brief get the node list
     */
    NodeMap nodes();

    /*!
     * \brief get the size of the field (in number of nodes)
     */
    QSize size();

    /*!
     * \brief the current game state
     */
    GameState state();

    /*!
     * \brief duh
     * \return the snake's length
     */
    int snakeLength();

    /*!
     * \brief convert a string to NodeType
     * \param s "(obstacle|body|apple|void)"
     * \return NodeType
     */
    static NodeType typeFromString(QString s);
    /*!
     * \brief convert a string to Orientation
     * \param s "(left|right|up|down|nowhere|.*)"
     * \return Orientation
     */
    static Orientation orientationFromString(QString s);

    /*!
     * \brief convert a string to Bend
     * \param s "(cw|ccw|none)"
     * \return Bend
     */
    static Bend bendFromString(QString s);

    /*!
     * \brief get a Bend from a change in orientation
     * \param a
     * \param b
     * \return the bend that results from changing orientation from a to b
     */
    static Bend bendFromOrientation(Orientation a, Orientation b);

    /*!
     * \brief add a node to the lookup tables
     * \param n the node pointer
     * \return success value: false if node is already in the tables
     */
    bool addNode(Node *n);

    /*!
     * \brief delete a node (lookup by direct reference
     * \param type the node type
     * \param at the position of the node in the list
     */
    void delNode(NodeType type, int at);

    /*!
     * \brief delete a node (lookup by coordinates)
     * \param pos the position of the node
     * \return success value: false if there's no node with these coordinates
     * slow.
     */
    bool delNode(QPoint pos);

    /*!
     * \brief delete ALL the nodes!
     */
    void clearNodes();

    /*!
     * \brief set the field size
     * \param size
     */
    void setSize(QSize size);

    /*!
     * \brief set the game speed
     * \param speed inverse speed (kHz^-1 = msec)
     */
    void setSpeed(int speed);

signals:
    /*!
     * \brief field refresh signal
     * this is emitted when there's any change on the field, and it needs
     * to be redrawn.
     */
    void refreshNodes();

    /*!
     * \brief game state change signal
     * \sa GameState
     */
    void stateChanged();

    /*!
     * \brief emitted when the snake grows
     */
    void snakeLengthChanged();

    /*!
     * \brief emitted when the field size changes
     */
    void sizeChanged();

public slots:
    /*!
     * \brief start the game (both initial start and unpause)
     * \return success value: false if game is already started
     * will emit stateChanged()
     */
    bool start();

    /*!
     * \brief pause the game
     * \return success value: false if game is not started
     */
    bool pause();

    /*!
     * \brief initialize the field
     * must run this before starting the game, or there will be segfaults!
     */
    void init();

    /*!
     * \brief set the orientation (direction) of the snake
     * \param o the orientation
     * sets the orientation of the snake's "head" node, which determines the
     * snake's "global" orientation
     */
    void orient(Orientation o);



private slots:
    /*!
     * \brief triggered by God upon even intervals to measure time
     */
    void tick();

    /*!
     * \brief check if this move will result in game over
     * \param coords the point which we'll be checking
     * \return true if the point is deadly to the snake
     */
    bool checkGameOver(QPoint coords);



private:
    /*!
     * \brief the Allmighty
     * Thee who measures time
     */
    QTimer *god;

    /*!
     * \brief the field size
     */
    QSize m_size;

    /*!
     * \brief the game state
     * \sa GameState
     */
    GameState m_state;

    /*!
     * \brief set the game state and emit the appropriate signals
     */
    void setState(GameState state);
    /*!
     * \sa NodeMap
     */
    NodeMap m_nodes;
    /*!
     * \sa NodeGrid
     */
    NodeGrid gridLookup;

    /*!
     * \brief the last orientation, used for determining bend
     */
    Orientation lastOrientation;

    /*!
     * \brief get a new apple
     * \param except an extra keep out point
     * creates a new node of type Apple on a random position that is not
     * on top of another node and is not on "except"
     */
    void newApple(QPoint except = QPoint());

    /*!
     * \brief fate!
     * \return a random point within the field
     */
    QPoint rndPoint();
};

/*!
 * \brief hash function for QPoint
 * \param p a QPoint
 * \return a value that's (probably) different for different points
 */
inline uint qHash(const QPoint &p)
{
    // here be dragons
    return qHash(((p.x() + p.y()) * (p.x() + p.y() + 1) + p.y()) / 2);
}


#endif // SNAKE_H
