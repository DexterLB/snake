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

    connect(ui->initBtn, SIGNAL(clicked()), this->snake, SLOT(init()));
    connect(ui->startBtn, SIGNAL(clicked()), this->snake, SLOT(start()));
    connect(ui->pauseBtn, SIGNAL(clicked()), this->snake, SLOT(pause()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
