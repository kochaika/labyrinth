#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include <QPixmap>
#include <QWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QPointF>
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
#include <QDebug>
#include <QTime>
#include <QDir>
#include <QInputDialog>
#include <QFontDatabase>
#include <QScreen>
#include <QtSensors/QAccelerometer>
#include "result.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QGraphicsScene *scene;
    QGraphicsScene *back_scene;
    QPixmap *pm;
    QPixmap *maze_pm;
    QImage im;
    QPainter *painter;
    QPainter *maze_painter;
    QBrush *brush;
    QPen *pen;
    QFont *font;
    QTime *your_time;
    QTimer *timer;
    Results *res;
//    QAccelerometer *sensor;

    int scale;
    int x;
    int y;
    int r;
    bool key_up_pressed;
    bool key_down_pressed;
    bool key_left_pressed;
    bool key_right_pressed;
    float user_time;
    QString time_d;
    QString time_r;
    QString input_uname;
    QString caption;
    QString caption_time;

    qreal xAccel;
    qreal yAccel;
    qreal zAccel;

    int indent; // отступ от края
    double utime;
    bool left;
    bool right;
    bool up;
    QTimer *tmr;
    QTimer *key_tmr;
    bool bottom;
    bool just_won; // выиграл
    long timer_tick_counter;
    long accel_tick_counter;
    int maze_level; //  сложность организации лабиринта
    int gravity_level; // сила гравитации
    //
        int grav_direction;
        //0 - Право
        //1 - Верх
        //2 - Лево
        //3 - Низ
   //
    int complexity;                         // - сложность лабиринта. В идеале от 1 до 4
    int gravity_time_average;
    int gravity_time_dispersion;
    int gravity_timer_time;
    int accel_timer_time;
    int accel_time_average;
    int accel_time_dispersion;
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
   // virtual void keyPressEvent(QKeyEvent *e);
  //  virtual void keyReleaseEvent(QKeyEvent *e);

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

signals:
    void emitGenerationComplete();
    void emitMazeComplete();

public slots:
    void generate();
    void blockInterface();
    void unBlockInterface();
    void timerTick();
    void Key_timerTick();
private slots:
    void on_maze_level_sliderMoved(int position);
    void on_gravity_level_sliderMoved(int position);
};

#endif // MAINWINDOW_H
