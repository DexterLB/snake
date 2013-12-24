#include "canvas.h"

Canvas::Canvas(QWidget *parent) :
    QWidget(parent)
{
    /*
    this->setSizeIncrement(this->nodeSize());
    this->setBaseSize(this->matrixSize());
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    */
    //this->setBackgroundRole(QPalette::Base);
    //this->setAutoFillBackground(true);
    this->m_nodeAspect = 1;
}

void Canvas::setNodeAspect(qreal a)
{
    this->m_nodeAspect = a;
}

qreal Canvas::aspect()
{
    return this->m_nodeAspect * ((qreal)this->snake->size().width()
                                 / this->snake->size().height());
}

QTransform Canvas::keepAspect()
{
    QTransform transform;

    int newWidth = qMin(this->width()
                        , (int)(this->height() * this->aspect()));
    int newHeight = qMin(this->height()
                         , (int)(this->width() / this->aspect()));


    // center
    transform.translate((this->width() - newWidth) / 2,
                        (this->height() - newHeight) / 2);

    // scale to new size
    transform.scale((qreal)newWidth / this->width()
                              , (qreal)newHeight / this->height());


    return transform;
}

void Canvas::paintEvent(QPaintEvent * /* event */)
{
    qDebug() << "entered paint event";
    QPainter painter(this);
    QTransform transform;   // coordinate system transform matrix
                            // woo fancy words
    transform *= this->keepAspect();

    // make the coordinate system use "nodes" instead of "pixels"
    transform.scale(this->nodeSize().width(), this->nodeSize().height());

    painter.setTransform(transform);

    this->drawBackground(&painter);

    // iterate over all nodes and draw them
    Snake::NodeMap nm = this->snake->nodes();
    for (Snake::NodeMap::ConstIterator i = nm.constBegin();
         i != nm.constEnd(); ++i) {
        for (int j = 0; j != (*i).size(); ++j) {
            this->drawNode(&painter, transform, (*i).value(j));
        }
    }
}

void Canvas::drawBackground(QPainter *painter)
{
    painter->fillRect(QRect(QPoint(0, 0), this->snake->size()), QBrush(QColor(0xebd9bf)));
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

    static const QRectF source(QPointF(0, 0), p->size());    // the entire pixmap
    static const QRectF target(QPointF(-0.5, -0.5), QSizeF(1, 1));
    painter->drawPixmap(target, *p, source);
}

QSizeF Canvas::nodeSize()
{
    return QSizeF((qreal)(this->size().width()) / this->snake->size().width()
                , (qreal)(this->size().height()) / this->snake->size().height());
}

QPoint Canvas::pixelCoords(QPoint coords)
{
    coords.setX(coords.x() * this->nodeSize().width());
    coords.setY(coords.y() * this->nodeSize().height());
    return coords;
}

void Canvas::sizeChanged()
{
    this->update();
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
