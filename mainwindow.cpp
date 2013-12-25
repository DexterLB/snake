#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // init random number generator
    qsrand(QTime::currentTime().msec());

    ui->setupUi(this);

    this->snake = new Snake();

    ui->canvas->setSnake(this->snake);
    ui->canvas->setPixmaps(&(this->pixmaps));
    connect(this->snake, SIGNAL(refreshNodes()), this->ui->canvas, SLOT(update()));
    connect(this->snake, SIGNAL(stateChanged()), this, SLOT(stateChanged()));
    connect(this->snake, SIGNAL(snakeLengthChanged()), this, SLOT(lengthChanged()));

    connect(ui->initBtn, SIGNAL(clicked()), this, SLOT(initClicked()));
    connect(ui->startBtn, SIGNAL(clicked()), this->snake, SLOT(start()));
    connect(ui->pauseBtn, SIGNAL(clicked()), this->snake, SLOT(pause()));

}

void MainWindow::initClicked()
{
    this->readSettings("test.json");
}

void MainWindow::stateChanged()
{
    if (this->snake->state() == Snake::Over) {
        this->ui->gameOver->show();
    } else {
        this->ui->gameOver->hide();
    }
}

void MainWindow::lengthChanged()
{
    this->ui->lengthLabel->setText(QString::number(this->snake->snakeLength()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearPixmaps()
{
    Canvas::PixmapId key;
    QList<QPixmap*> deletedPixmaps;
    while (!this->pixmaps.isEmpty()) {
        key = this->pixmaps.keys().first();
        for (Canvas::PixmapMap::iterator i = this->pixmaps.find(key);
             (i != this->pixmaps.end() && i.key() == key ); ++i) {
            if (!deletedPixmaps.contains((*i))) {
                delete (*i);
                deletedPixmaps << (*i);
            }
        }
        this->pixmaps.remove(key);
    }
}

bool MainWindow::readSettings(QString filename)
{
    /*
     * in this function there be returns everywhere!
     * todo: make it prettier
     */

    this->snake->init();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "failed to open file " << filename;
        return false;
    }
    QJsonObject root;
    {
        QJsonParseError err;
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error != QJsonParseError::NoError) {
            qDebug() << "json parse error: " << err.errorString() << " at " << err.offset;
            qDebug() << data.insert(err.offset - 1, "<!>");
            return false;
        }
        root = doc.object();
    }
    QJsonValue val;

    QSize size;

    // get background colour
    this->ui->canvas->setBgColor(QColor(root.value(QString("bg-clr")).toString()));

    // get background image
    this->ui->canvas->setBgPixmap(QPixmap(root.value(QString("bg-img")).toString()));

    // get the game over image
    this->ui->canvas->setGameOverPixmap(QPixmap(root.value(QString("gameover-img")).toString()));

    // get size
    val = root.value(QString("size"));
    if (val.toArray().size() != 2) {
        qDebug() << "size is not a tuple" << val.toArray().toVariantList();
        return false;
    }
    size.setWidth(val.toArray().at(0).toInt(0));
    size.setHeight(val.toArray().at(1).toInt(0));
    if (!size.isValid()) {
        qDebug() << "size is invalid";
        return false;
    }
    this->snake->setSize(size);

    // get speed
    this->snake->setSpeed(root.value(QString("speed")).toInt(200));

    // get node aspect ratio
    this->ui->canvas->setNodeAspect(
                root.value(QString("aspect")).toDouble(1));

    // get all nodes
    val = root.value(QString("nodes"));
    if (!val.isArray()) {
        qDebug() << "nodes is not an array";
        return false;
    }
    QJsonObject obj;
    QJsonArray arr, arr2;
    Snake::NodeType type;
    Snake::NodeAttribute attr;
    Snake::Orientation orientation;
    Snake::Bend bend;
    QPoint pos;
    for (QJsonArray::ConstIterator i = val.toArray().constBegin();
         i != val.toArray().constEnd(); ++i) {
        obj = (*i).toObject();

        // get type
        type = Snake::typeFromString(obj.value(QString("type")).toString());

        // get attribute
        attr = (Snake::NodeAttribute)obj.value(QString("attr")).toInt();

        // get position
        arr = obj.value(QString("position")).toArray();
        if (arr.size() != 2) {
            qDebug() << "position is not a tuple";
            return false;
        }
        pos.setX(arr.at(0).toInt());
        pos.setY(arr.at(1).toInt());

        // get orientation
        orientation = Snake::orientationFromString(obj.value(QString("orientation")).toString("nowhere"));

        // get bend
        bend = Snake::bendFromString(obj.value("bend").toString("none"));

        this->snake->addNode(Snake::mkNode(pos, type, orientation, attr, bend));
    }

    // get pixmaps
    this->clearPixmaps();
    val = root.value(QString("pixmaps"));
    if (!val.isArray()) {
        qDebug() << "nodes is not an array";
        return false;
    }

    QPixmap *p;
    Canvas::PixmapId id;
    for (QJsonArray::ConstIterator i = val.toArray().constBegin();
         i != val.toArray().constEnd(); ++i) {
        obj = (*i).toObject();
        p = new QPixmap(obj.value(QString("file")).toString());
        if (p->isNull()) {
            qDebug() << "invalid pixmap";
            delete p;
            return false;
        }
        arr = obj.value(QString("attrs")).toArray();
        arr2 = obj.value(QString("bend")).toArray();
        for (QJsonArray::ConstIterator j = arr.constBegin();
             j != arr.constEnd(); ++j) {
            if (!((*j).isDouble())) {
                qDebug() << "attr isn't a number";
                delete p;
                return false;
            }
            id.attr = (Snake::NodeAttribute)((*j).toInt());
            if (arr2.isEmpty()) {
                // fill all bends with this pixmap
                // todo: find a proper way to iterate over an enum
                id.bend = Snake::BendNone;
                this->pixmaps.insert(id, p);
                id.bend = Snake::BendCounterclockwise;
                this->pixmaps.insert(id, p);
                id.bend = Snake::BendClockwise;
                this->pixmaps.insert(id, p);
            } else {
                for (QJsonArray::ConstIterator k = arr2.constBegin();
                     k != arr2.constEnd(); ++k) {
                    id.bend = Snake::bendFromString((*k).toString("none"));
                    this->pixmaps.insert(id, p);
                }
            }
        }
    }
    this->lengthChanged();
    return true;
}


void MainWindow::keyPressEvent(QKeyEvent *event)
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
    case Qt::Key_R:
        this->readSettings("test.json");
        break;
    case Qt::Key_Space:
        switch(this->snake->state()) {
        case Snake::Playing:
            this->snake->pause();
            break;
        case Snake::Paused:
        case Snake::Stopped:
            this->snake->start();
            break;
        }
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}
