#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QTime>
#include <QDir>
#include <QFileInfo>
#include "snake.h"
#include "canvas.h"

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
     * \brief the snake class (all the game logic)
     */
    Snake *snake;

    /*!
     * \brief reads a json file
     * \param filename
     * \return the root object
     */
    QJsonObject readJson(QString filename);

    /*!
     * \brief reads map settings from a json file
     * \param filename
     * \return success value
     */
    bool readLevel(QString filename);

    /*!
     * \brief reads a level list from a json file
     * \param filename
     * \return success value
     */
    bool readLevelList(QString filename);

    /*!
     * \brief all pixmaps for rendering nodes
     */
    Canvas::PixmapMap pixmaps;

private slots:
    /*!
     * \brief game state changed
     */
    void stateChanged();

    /*!
     * \brief snake length changed
     */
    void lengthChanged();

    /*!
     * \brief nuke ALL teh pixmaps!
     */
    void clearPixmaps();

    /*!
     * \brief select a level from the list and load it
     */
    void selectLevel();

    /*!
     * \brief this is executed as soon as we enter the event loop
     */
    void loadInitialConfig();

protected:
    /*!
     * \brief widget keypress event
     * \param event the key event object
     * called when a key event is received by the widget
     * (in most cases this requires the widget to be focused)
     */
    void keyPressEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
