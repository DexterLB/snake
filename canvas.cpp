#include "canvas.h"

Canvas::Canvas(QWidget *parent) :
    QWidget(parent)
{
    /*
    this->setSizeIncrement(this->nodeSize());
    this->setBaseSize(this->matrixSize());
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    */
    this->setBackgroundRole(QPalette::Base);
    this->setAutoFillBackground(true);
}

void Canvas::paintEvent(QPaintEvent * /* event */)
{
    qDebug() << "entered paint event";
    QPainter painter(this);
    QTransform transform;   // coordinate system transform matrix
                            // woo fancy words

    // make the coordinate system use "nodes" instead of "pixels"
    transform.scale(this->nodeSize().width(), this->nodeSize().height());

    // iterate over all nodes and draw them
    Snake::NodeMap nm = this->snake->nodes();
    for (Snake::NodeMap::ConstIterator i = nm.constBegin();
         i != nm.constEnd(); ++i) {
        for (int j = 0; j != (*i).size(); ++j) {
            this->drawNode(&painter, transform, (*i).value(j));
        }
    }
}

void Canvas::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Left:
        this->snake->orient(Snake::Left);
        break;
    case Qt::Key_Right:
        this->snake->orient(Snake::Right);
        break;
    case Qt::Key_Up:
        this->snake->orient(Snake::Up);
        break;
    case Qt::Key_Down:
        this->snake->orient(Snake::Down);
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void Canvas::drawNode(QPainter *painter, QTransform transform, Snake::Node *node)
{
    // painter->drawRect(QRect(this->pixelCoords(node->pos), this->nodeSize()));
    QPixmap *p = this->m_pixmaps->value(this->pixmapIdFromNode(*node));

    // set the position to the centre of the node we're drawing
    transform.translate((qreal)(node->pos.x()) + 0.5, (qreal)(node->pos.y()) + 0.5);
    transform.rotate(Snake::orientationAngle(node->orientation));


    painter->setTransform(transform);

    static const QRectF source(QPointF(0, 0), QSizeF(p->size()));    // the entire pixmap
    static const QRectF target(QPointF(-0.5, -0.5), QSizeF(1, 1));
    painter->drawPixmap(target, *p, source);
}

QSize Canvas::nodeSize()
{
    return QSize(15, 15);   // pixels
}

QSize Canvas::pixelSize()
{
    return QSize(this->nodeSize().width() * this->snake->size().width()
            , this->nodeSize().height() * this->snake->size().height());
}

QPoint Canvas::pixelCoords(QPoint coords)
{
    coords.setX(coords.x() * this->nodeSize().width());
    coords.setY(coords.y() * this->nodeSize().height());
    return coords;
}

void Canvas::sizeChanged()
{
    this->setSizeIncrement(this->nodeSize());
    this->setFixedSize(this->pixelSize());
    this->updateGeometry();
}

void Canvas::setSnake(Snake *s)
{
    this->snake = s;
    connect(this->snake, SIGNAL(sizeChanged()), this, SLOT(sizeChanged()));
}

void Canvas::setPixmaps(PixmapMap *p)
{
    this->m_pixmaps = p;
}

Canvas::PixmapId Canvas::pixmapIdFromNode(Snake::Node &n)
{
    PixmapId p;
    p.attr = n.attr;
    p.bend = n.bend;
    return p;
}
