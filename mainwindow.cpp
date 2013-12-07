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

    connect(ui->eraseBtn, SIGNAL(clicked()), this, SLOT(erase()));
    connect(ui->drawBtn, SIGNAL(clicked()), this, SLOT(draw()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::draw()
{
    this->snake->start();
}

void MainWindow::erase()
{

}
