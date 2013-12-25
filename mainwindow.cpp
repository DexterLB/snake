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
    connect(this->ui->playBtn, SIGNAL(clicked()), this, SLOT(selectLevel()));
    this->stateChanged();
    this->readLevelList("levels.json");
}

void MainWindow::stateChanged()
{
    if (this->snake->state() == Snake::Undefined) {
        this->ui->menuBox->show();
        this->ui->canvas->hide();
        this->ui->infoLabel->setText(tr("Select a level"));
    } else {
        this->ui->menuBox->hide();
        this->ui->canvas->show();
        this->ui->canvas->setFocus();

        switch(this->snake->state()) {
        case Snake::Stopped:
            this->ui->infoLabel->setText(tr("Press Space (that big long button) to start!"));
            break;
        case Snake::Playing:
            this->ui->infoLabel->setText(tr("Press Space to pause, N for rage reset or R for rage quit. Also, you suck."));
            break;
        case Snake::Paused:
            this->ui->infoLabel->setText(tr("Press Space to resume"));
            break;
        case Snake::Over:
            this->ui->infoLabel->setText(tr("Aww, you piece of crap. Press N for new game or R for another level"));
            break;
        default:
            break;  // do nothing
        }
    }
}

void MainWindow::selectLevel()
{
    this->readLevel(this->ui->levelList->selectedItems().at(0)->data(Qt::UserRole).toString());
}

void MainWindow::lengthChanged()
{
    this->ui->lengthLabel->setText(QString::number(this->snake->snakeLength()));
}

MainWindow::~MainWindow()
{
    this->clearPixmaps();
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

QJsonObject MainWindow::readJson(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "failed to open file " << filename;
        return QJsonObject();
    }
    QJsonParseError err;
    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "json parse error: " << err.errorString() << " at " << err.offset;
        qDebug() << data.insert(err.offset - 1, "<!>");
        return QJsonObject();
    }
    return doc.object();
}

bool MainWindow::readLevelList(QString filename)
{
    QJsonObject root = this->readJson(filename);
    QJsonArray levels = root.value(QString("levels")).toArray();
    QString name, file;
    QListWidgetItem *item;
    for (QJsonArray::ConstIterator i = levels.constBegin();
         i != levels.constEnd(); ++i) {
        name = (*i).toObject().value("name").toString();
        file = (*i).toObject().value("file").toString();
        if (name.isEmpty() || file.isEmpty()) {
            qDebug() << "empty value";
            return false;
        }
        item = new QListWidgetItem(name, this->ui->levelList);
        item->setData(Qt::UserRole, QVariant(file));
    }
    return true;
}

bool MainWindow::readLevel(QString filename)
{
    /*
     * in this function there be returns everywhere!
     * todo: make it prettier
     */

    this->snake->init();

    QJsonObject root = this->readJson(filename);
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
    case Qt::Key_N:
        this->selectLevel();
        break;
    case Qt::Key_R:
        this->snake->reset();
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
        default:
            break;  // do nothing
        }
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}
