#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "snake.h"

namespace Ui {
class MainWindow;
}

/*!
 * \class MainWindow
 * \brief the main window class
 * this is constructed inside main() and the destructor will be called upon
 * application exit or qApp->exit()
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief constructor
     */
    explicit MainWindow(QWidget *parent = 0);

    /*!
     * \brief destructor
     */
    ~MainWindow();

private:
    /*!
     * \brief ui object
     * this object is created by MOC at compile time by parsing the UI XML files
     */
    Ui::MainWindow *ui;

    /*!
     * \brief actual stuff
     */
    Snake *snake;

private slots:
};

#endif // MAINWINDOW_H
