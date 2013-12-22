#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->snake = new Snake();

    ui->canvas->setSnake(this->snake);
    connect(this->snake, SIGNAL(refreshNodes()), this->ui->canvas, SLOT(update()));
    connect(this->snake, SIGNAL(stateChanged()), this, SLOT(stateChanged()));
    connect(this->snake, SIGNAL(snakeLengthChanged()), this, SLOT(lengthChanged()));

    connect(ui->initBtn, SIGNAL(clicked()), this, SLOT(initClicked()));
    connect(ui->startBtn, SIGNAL(clicked()), this->snake, SLOT(start()));
    connect(ui->pauseBtn, SIGNAL(clicked()), this->snake, SLOT(pause()));

}

void MainWindow::initClicked()
{
    this->readSettings("/tmp/test.json");
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

bool MainWindow::readSettings(QString filename)
{
    /*
     * in this function there be returns everywhere!
     */

    this->snake->init();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "failed to open file " << filename;
        return false;   // ******************
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
            return false;   // ******************
        }
        root = doc.object();
    }
    QJsonValue val;

    QSize size;

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

    val = root.value(QString("nodes"));
    if (!val.isArray()) {
        qDebug() << "nodes is not an array";
        return false;
    }
    QJsonObject obj;
    QJsonArray arr;
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

    return true;
}
