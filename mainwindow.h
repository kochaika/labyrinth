#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QPen>
#include <QImage>
#include <QRgb>
#include <QColor>
#include <QPoint>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "square.h"
#include "QTime"
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QGraphicsScene *scene;
    QPixmap *pm;
    QPixmap *maze_pm;
    QImage im;
    QPainter *painter;
    QPainter *maze_painter;
    QBrush *brush;
    QPen *pen;
    int x;
    int y;
    int r;
    bool left;
    bool right;
    bool up;
    QTimer *tmr;
    bool bottom;
    //
        int grav_direction;
        //0 - Право
        //1 - Верх
        //2 - Лево
        //3 - Низ
   //
    int cells_h; // ячеек по горизонтали
    int cells_v; // ячеек по вертикали

    int width;  // ширина ячейки
    int height; // высота ячейки

    Square ***cells; // все ячейки лабиринта
    int **alg_Li;       // массив атрибутов для алгоритма Ли

    void generate(int nH, int nV, int sH, int sV); // рекурсивно вызываемая функция при генерации лабиринта
                                                                   // nH и nV - количество ячеек по горизонтали и вертикали в области
                                                                   // в которой происходит генерация новых комнат
                                                                   // sH и sV - смещение области (в количестве ячеек по горизонтали и вертикали)
                                                                   // в которой происходит генерация новых комнат
                                                                   // относительно нулевой ячейки (верхний левый угол)
    void clearCells();  // очистка поля лабиринта
    void initMaze();
    void cleareMaze();

public:
    virtual void paintEvent(QPaintEvent * /*event*/);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

signals:
    void emitGenerationComplete();

public slots:
    void generate();
    void blockInterface();
    void unBlockInterface();
    void timerTick();
};

#endif // MAINWINDOW_H
