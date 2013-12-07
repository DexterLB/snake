#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->canvas->setNodeList(&(this->nodes));
    connect(ui->eraseBtn, SIGNAL(clicked()), this, SLOT(erase()));
    connect(ui->drawBtn, SIGNAL(clicked()), this, SLOT(draw()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::draw()
{
    Snake::Node n;
    n.pos = QPoint(ui->box_x->value(), ui->box_y->value());
    n.type = Snake::SnakeBody;
    this->nodes.append(n);
    ui->canvas->update();
}

void MainWindow::erase()
{

}
