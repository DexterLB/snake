#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPainter>
#include <QtDebug>
#include <QList>
#include <QKeyEvent>

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
     * \brief the size of a single node in pixels
     * \return size of a single node
     */
    QSize nodeSize();

    /*!
     * \brief the size of the entire widget in pixels
     * \return size of the widget (excluding external borders)
     */
    QSize pixelSize();

    /*!
     * \brief convert node coordinates to pixel coordinates
     * \param coords the node coordinates (number of nodes from x,y=0)
     * \return calculated coordinates in pixels
     */
    QPoint pixelCoords(QPoint coords);

    /*!
     * \brief set the Snake class to be used
     * \param s the Snake class this widget will be an interface to
     */
    void setSnake(Snake *s);

signals:

public slots:

private:
    /*!
     * \brief holds the Snake class pointer
     */
    Snake *snake;

    /*!
     * \brief draw a single node on the canvas
     * must be called from inside an expose (paint) event
     * nodes must be redrawn on widget updates
     */
    void drawNode(QPainter *painter, Snake::Node *node);

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

#endif // CANVAS_H
