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

    connect(ui->initBtn, SIGNAL(clicked()), this->snake, SLOT(init()));
    connect(ui->startBtn, SIGNAL(clicked()), this->snake, SLOT(start()));
    connect(ui->pauseBtn, SIGNAL(clicked()), this->snake, SLOT(pause()));
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
