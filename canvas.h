#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPainter>
#include <QtDebug>
#include <QList>
#include <QKeyEvent>

#include "snake.h"

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = 0);
    QSize nodeSize();
    QSize pixelSize();
    QPoint pixelCoords(QPoint coords);
    void setSnake(Snake *s);

signals:

public slots:

private:
    Snake *snake;
    void drawNode(QPainter *painter, Snake::Node node);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

#endif // CANVAS_H
