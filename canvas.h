#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPainter>
#include <QtDebug>
#include <QList>
#include <QKeyEvent>
#include <QMultiHash>
#include <QPixmap>
#include <QPointF>
#include <QSizeF>
#include <QTransform>

#include "snake.h"
/*!
 * \class Canvas
 * \brief visual interface widget
 */
class Canvas : public QWidget
{
    Q_OBJECT
public:
    /*!
     * \brief constructor
     */
    explicit Canvas(QWidget *parent = 0);

    /*!
     * \brief pixmap identification
     * the properties of a node that are sufficient to assign
     * a pixmap to it
     */
    typedef struct {
        Snake::NodeAttribute attr;
        Snake::Bend bend;
    } PixmapId;

    /*!
     * \brief stores a list of all pixmaps for all possible nodes
     */
    typedef QMultiHash<PixmapId, QPixmap*> PixmapMap;

    /*!
     * \brief the size of a single node in pixels
     * \return size of a single node
     */
    QSizeF nodeSize();

    /*!
     * \brief convert node coordinates to pixel coordinates
     * \param coords the node coordinates (number of nodes from x,y=0)
     * \return calculated coordinates in pixels
     */
    QPoint pixelCoords(QPoint coords);

    /*!
     * \brief set the aspect ratio for a single node
     * (don't really see when this won't be 1, but hey!)
     * \param a width/height
     */
    void setNodeAspect(qreal a);

    /*!
     * \brief set the Snake class to be used. Must be done before update()
     * \param s the Snake class this widget will be an interface to
     */
    void setSnake(Snake *s);

    /*!
     * \brief set the pixmap map. Must be done before update()
     * \param p pointer to an external PixmapMap
     */
    void setPixmaps(PixmapMap *p);

    /*!
     * \brief pixmapIdFromNode
     * \param n a node
     * \return PixmapId
     */
    PixmapId pixmapIdFromNode(Snake::Node &n);

signals:

public slots:

private:
    /*!
     * \brief holds the Snake class pointer
     */
    Snake *snake;

    /*!
     * \brief all pixmaps to be used for rendering nodes
     */
    PixmapMap *m_pixmaps;

    /*!
     * \brief aspect ratio for a single node
     */
    qreal m_nodeAspect;

    /*!
     * \brief draw a single node on the canvas
     * \param painter a QPainter that paints on the widget
     * \param transform the base transform matrix
     * \param node the node
     * must be called from inside an expose (paint) event
     * nodes must be redrawn on widget updates
     */
    void drawNode(QPainter *painter, QTransform transform, Snake::Node *node);

    /*!
     * \brief transform the drawing area in order to keep the aspect ratio
     * \return transform matrix
     */
    QTransform keepAspect();

    /*!
     * \brief aspect ratio for the entire field
     * \return width/height
     */
    qreal aspect();



    /*!
     * \brief sets the background (only of the field area)
     */
    void drawBackground(QPainter *painter);

protected:
    /*!
     * \brief widget paint event
     * \param event the paint event object
     * called whenever the contents of the widget need to be redrawn
     * one such case is explicitly calling update()
     * a paint event may also occur when an area of the widget is uncovered
     * or during a WM redraw event
     */
    void paintEvent(QPaintEvent *event);

    /*!
     * \brief widget keypress event
     * \param event the key event object
     * called when a key event is received by the widget
     * (in most cases this requires the widget to be focused)
     */
    void keyPressEvent(QKeyEvent *event);

private slots:
    /*!
     * \brief upon field size change
     */
    void sizeChanged();
};

/*!
 * \brief hashing function for PixmapID
 * \param id duh!
 * \return a value that is (probably) different for different id's
 */
inline uint qHash(const Canvas::PixmapId &id)
{
    // even more dragons
    return qHash(((((int)id.attr + (int)id.bend) * ((int)id.attr + (int)id.bend + 1) + (int)id.bend)) / 2);
}

/*!
 * \brief comparison of two PixmapIDs
 * \param a
 * \param b
 * \return a == b
 */
inline bool operator==(const Canvas::PixmapId a, const Canvas::PixmapId b) {
    return (a.attr == b.attr) && (a.bend == b.bend);
}

#endif // CANVAS_H
