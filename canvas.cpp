#include "canvas.h"

Canvas::Canvas(QWidget *parent) :
    QWidget(parent)
{
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

int Canvas::keepAspectWidth()
{
    return qMin(this->width(), (int)(this->height() * this->aspect()));
}

int Canvas::keepAspectHeight()
{
    return this->keepAspectWidth() / this->aspect();
}

QTransform Canvas::keepAspect()
{
    QTransform transform;

    int newWidth = this->keepAspectWidth();
    int newHeight = this->keepAspectHeight();

    qreal scale = qMax((qreal)newWidth / this->width()
                       , (qreal)newHeight / this->height());

    // scale to new size
    transform.scale(scale, scale);

    // center
    transform.translate((this->width() - newHeight) / 2,
                        (this->height() - newWidth) / 2);



    return transform;
}

void Canvas::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    // those don't work for pixmaps? maybe a render backend specific bug? fixme
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

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

    // check for game over
    if (this->snake->state() == Snake::Over && !this->m_gameOverPixmap.isNull()) {
        painter.setTransform(transform);
        this->drawPixmapOnField(&painter, this->m_gameOverPixmap);
    }
}

void Canvas::drawBackground(QPainter *painter)
{
    if (!this->m_bgPixmap.isNull()) {
        this->drawPixmapOnField(painter, this->m_bgPixmap);
    } else if (this->m_bgColor.isValid()) {
        painter->fillRect(QRect(QPoint(0, 0), this->snake->size()), QBrush(this->m_bgColor));
    }
}

void Canvas::drawNode(QPainter *painter, QTransform transform, Snake::Node *node)
{
    // painter->drawRect(QRect(this->pixelCoords(node->pos), this->nodeSize()));
    QPixmap *p = NULL;
    {
        // get a random pixmap
        QList<QPixmap *> pixmaps = this->m_pixmaps->values(this->pixmapIdFromNode(*node));
        if (pixmaps.isEmpty()) {
            return; // no pixmap
        }
        // each time we draw the same node the pixmap will be the same, since rand is stored
        // inside the node struct
        p = pixmaps.at(node->rand % pixmaps.size());
    }


    // set the position to the centre of the node we're drawing
    transform.translate((qreal)(node->pos.x()) + 0.5, (qreal)(node->pos.y()) + 0.5);
    // do the rotation (around the centre)
    transform.rotate(Snake::orientationAngle(node->orientation));
    // now move back to the topleft corner
    transform.translate(-0.5, -0.5);


    painter->setTransform(transform);

    // we've targeted a 1x1 square for the node with transform, so that's our target
    QRectF target(0, 0, 1, 1);

    // because of a bug in Qt antialiased rendering on
    // a transformed coordinate system doesn't work.
    // fixme
    /*
    static const QRectF source(QPointF(0, 0), QSizeF(p->size()));
    painter->drawPixmap(target, *p, source);
    */

    // a workaround is to first do approximate scaling with QPixmap::scaled()
    // and then use the translated coordinates. this needs some thinking through, but works so far
    QRectF source(QPointF(0, 0), QSizeF((int)this->nodeSize().width(), (int)this->nodeSize().height()));    // the entire pixmap
    painter->drawPixmap(target, p->scaled((int)this->nodeSize().width(), (int)this->nodeSize().height()
                                          , Qt::IgnoreAspectRatio, Qt::SmoothTransformation), source);
}

void Canvas::drawPixmapOnField(QPainter *painter, QPixmap &pixmap)
{
    // again doing the antialising workaround
    QSizeF nodesize = this->nodeSize();
    QPixmap p = pixmap.scaled(nodesize.width() * this->snake->size().width()
                                       , this->nodeSize().height() * this->snake->size().height()
                                       , Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QRect target(QPoint(0, 0), this->snake->size());
    QRect source(QPoint(0, 0), p.size());    // the entire pixmap
    painter->drawPixmap(target, p, source);
}

QSizeF Canvas::nodeSize()
{
    return QSizeF((qreal)(this->keepAspectWidth()) / this->snake->size().width()
                , (qreal)(this->keepAspectWidth()) / this->snake->size().height());
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

void Canvas::setBgPixmap(QPixmap pixmap)
{
    this->m_bgPixmap = pixmap;
}

void Canvas::setGameOverPixmap(QPixmap pixmap)
{
    this->m_gameOverPixmap = pixmap;
}

void Canvas::setBgColor(QColor color)
{
    this->m_bgColor = color;
}
