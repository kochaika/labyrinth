#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    tmr = new QTimer(this);
    connect(tmr, SIGNAL(timeout()), this, SLOT(timerTick()));
    tmr->start(100);
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));
    ui->setupUi(this);
    connect(ui->pushButton_generate, SIGNAL(clicked()), this, SLOT(blockInterface()));
    connect(ui->pushButton_generate, SIGNAL(clicked()), this, SLOT(generate()));
    connect(this, SIGNAL(emitGenerationComplete()), this, SLOT(unBlockInterface()));
    this->setFixedSize(500,530);
    ui->graphicsView->setFixedSize(450,450);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    pm = new QPixmap(400,400);
    maze_pm = new QPixmap(400,400);
    maze_pm->fill(QColor(0,0,0,0));
    painter = new QPainter(pm);
    left = true; // налево - свободно
    right = true; // направо - свободно
    up = true; // вверх - свободно
    bottom = true; // вниз - свободно
    r = 7;
    initMaze();
    clearCells();
    scene->addPixmap(*pm);
    //generate();
}

void MainWindow::timerTick()
{
    QPen pen;
    pen.setColor(Qt::black);
    painter->setBrush(QBrush(Qt::black));
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawEllipse(x,y,r,r);
    switch (grav_direction) {
        case 0: // вправо
                if (right){
                    x=x+1;
                    if (x >= pm->width())
                        x=pm->width()-1;
                }
                break;
        case 1: // вверх
                if (up){
                    y=y-1;
                    if (y < 0)
                        y=0;
                }
                break;
        case 2: // влево
                if (left){
                    x=x-1;
                    if (x < 0)
                        x=0;
                }
                break;
        case 3: // вниз
                if (bottom){
                    y=y+1;
                    if (y >= pm->height()-r)
                        y=pm->height()-1;
                }
                break;
    }

    repaint();

}

void MainWindow::paintEvent(QPaintEvent * /*event*/)
{

    QPen pen(Qt::gray, 2);
    painter->setPen(pen);
   // QPoint a(0,0), b(200,200);
    // painter->drawLine(a,b);
  /*  painter->drawRect(0,0,pm->width(),pm->height());
    painter->setPen(QPen(Qt::gray, 5));
    for (int i = 0; i < cells_v; i++)
        for (int j = 0; j < cells_h; j++) {
            if ( cells[i][j]->get_left_edge() ) //
                painter->drawLine(j * width,  i * height, j * width,  (i + 1) * height);
            if ( cells[i][j]->get_top_edge() )
                painter->drawLine(j * width,  i * height, (j + 1) * width, i * height);
            if ( cells[i][j]->get_right_edge() )
                painter->drawLine((j + 1) * width,  i * height, (j + 1) * width, (i + 1) * height);
            if ( cells[i][j]->get_bottom_edge() )
                painter->drawLine(j * width,  (i + 1) * height, (j + 1) * width, (i + 1) * height);
        }
*/
    pen.setColor(Qt::green);
    painter->setPen(pen);
    painter->setBrush(QBrush(Qt::green));
    painter->drawEllipse(pm->width()-10,pm->height()-10,12,12);
   // painter->drawEllipse(0,10,12,12);
    pen.setColor(Qt::red);
    painter->setBrush(QBrush(Qt::red));
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawEllipse(x,y,r,r);

    im = pm->toImage();

    QColor color;
    // проверяем путь вниз
    int check = 0;
    for (int i = 0; i<=r; i++){
        color = im.pixel(x+i,y+r+1);
        if (color.name() ==  "#a0a0a4")
            check++;
    }
    if (check != 0)
        bottom = false; // вниз нельзя
    // проверяем путь вверх
    check = 0;
    for (int i = 0; i<=r; i++){
        color = im.pixel(x+i,y-1);
        if (color.name() ==  "#a0a0a4")
            check++;
    }
    if (check != 0)
        up = false; // вверх нельзя

    // проверяем путь вправо
    check = 0;
    for (int i = 0; i<=r; i++){
        color = im.pixel(x+r+1,y+i);
        if (color.name() ==  "#a0a0a4")
            check++;
    }
    if (check != 0)
        right = false; // вправо нельзя
    // проверяем путь влево
    check = 0;
    for (int i = 0; i<=r; i++){
        color = im.pixel(x-1,y+i);
        if (color.name() ==  "#a0a0a4")
            check++;
    }
    if (check != 0)
        left = false; // влево нельзя
        //
    color = im.pixel(x+r+1,y+r+1);

    if (color.name() == "#00ff00"){
               // добрались до финиша
        painter->setBrush(QBrush(Qt::black));
        painter->drawRect(QRectF(0,0,pm->width(),pm->height()));
        painter->drawText(QRectF(pm->width()/2-10,pm->height()/2-10,100,100),"YOU WIN");
        ui->pushButton_generate->setFocus();
        ui->pushButton_generate->setEnabled(true);
        clearCells();
    }

    painter->setBrush(QBrush(Qt::black));
    scene->clear();
    scene->addPixmap(*pm);
    //qDebug()<<x<<","<<y<<" l "<<left<<" r "<<right<<" u "<<up<<" b "<<bottom;
}

void MainWindow::generate(){
    x = r-1;
    y = r-1;
    clearCells();
    generate(cells_h, cells_v, 0, 0);
    grav_direction = qrand() % 4;
    emitGenerationComplete();
    painter->drawRect(0,0,pm->width(),pm->height());
    painter->setPen(QPen(Qt::gray, 5));
    for (int i = 0; i < cells_v; i++)
        for (int j = 0; j < cells_h; j++) {
            if ( cells[i][j]->get_left_edge() ) //
                painter->drawLine(j * width,  i * height, j * width,  (i + 1) * height);
            if ( cells[i][j]->get_top_edge() )
                painter->drawLine(j * width,  i * height, (j + 1) * width, i * height);
            if ( cells[i][j]->get_right_edge() )
                painter->drawLine((j + 1) * width,  i * height, (j + 1) * width, (i + 1) * height);
            if ( cells[i][j]->get_bottom_edge() )
                painter->drawLine(j * width,  (i + 1) * height, (j + 1) * width, (i + 1) * height);
        }
    repaint();
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    QPen pen;
    pen.setColor(Qt::black);
    painter->setBrush(QBrush(Qt::black));
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawEllipse(x,y,r,r);
    switch (e->key())
    {
    case Qt::Key_Escape:
        this->close();
        return;
    case Qt::Key_Up:
        if (up){
            y=y-1;
            if (y < 0)
                y=0;
        }
      //  qDebug() << "y ="<< y;
        break;
    case Qt::Key_Down:
        if (bottom){
            y=y+1;
            if (y >= pm->height()-r)
                y=pm->height()-1;
        }
      //  qDebug() << "y ="<< y;
        break;
    case Qt::Key_Left:
        if (left){
            x=x-1;
            if (x < 0)
                x=0;
        }
      //  qDebug() << "x ="<< x;
        break;
    case Qt::Key_Right:
        if (right){
            x=x+1;
            if (x >= pm->width())
                x=pm->width()-1;
        }
      //  qDebug() << "x ="<< x;
        break;
    default:
        e->ignore();
    }
    left = true;
    right = true;
    bottom = true;
    up = true;
    repaint();
}

void MainWindow::keyPressEvent(QKeyEvent * e)
{
    QPen pen;
    pen.setColor(Qt::black);
    painter->setBrush(QBrush(Qt::black));
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawEllipse(x,y,r,r);
    switch (e->key())
    {
    case Qt::Key_Escape:
        this->close();
        return;
    case Qt::Key_Up:
        if (up){
            y=y-1;
            if (y < 0)
                y=0;
        }
      //  qDebug() << "y ="<< y;
        break;
    case Qt::Key_Down:
        if (bottom){
            y=y+1;
            if (y >= pm->height()-r)
                y=pm->height()-1;
        }
      //  qDebug() << "y ="<< y;
        break;
    case Qt::Key_Left:
        if (left){
            x=x-1;
            if (x < 0)
                x=0;
        }
      //  qDebug() << "x ="<< x;
        break;
    case Qt::Key_Right:
        if (right){
            x=x+1;
            if (x >= pm->width())
                x=pm->width()-1;
        }
      //  qDebug() << "x ="<< x;
        break;
    default:
        e->ignore();
    }
    left = true;
    right = true;
    bottom = true;
    up = true;
    repaint();
    //scene->update();
}

MainWindow::~MainWindow()
{
    cleareMaze();
    delete ui;
}

void MainWindow::blockInterface() {
    ui->pushButton_generate->setDisabled(true);
    //ui->graphicsView->setFocus();
    //ui->pushButton_search->setDisabled(true);
}

void MainWindow::unBlockInterface() {
    ui->pushButton_generate->setEnabled(true);
   // ui->pushButton_search->setEnabled(true);
}


void MainWindow::clearCells() {
    for (int i = 0; i < cells_v; i++)
       for (int j = 0; j < cells_h; j++) {
           cells[i][j]->set_left_edge(false);
           cells[i][j]->set_top_edge(false);
           cells[i][j]->set_right_edge(false);
           cells[i][j]->set_bottom_edge(false);

           if (i == 0)
               cells[i][j]->set_top_edge(true);
           else if (i == cells_v - 1)
               cells[i][j]->set_bottom_edge(true);

           if (j == 0)
               cells[i][j]->set_left_edge(true);
           else if (j == cells_h - 1)
               cells[i][j]->set_right_edge(true);
        }
}


void MainWindow::generate(int nH, int nV, int sH, int sV) {

    // 1) если комната в длинну или ширину равна 1 ячейке, то внутри уже нельзя ничего генерировать => выходим
    if ( (nH == 1 || nV == 1) )
        return;

    // 2) если все стороны больше чем в две ячейки, продолжаем выполнять всё что ниже

    // находим номера ячеек по которым будут проходить границы
    int indexH, indexV;
    if (nH == 2)
        indexH = 0;
    else
        indexH = qrand() % (nH - 1);

    if (nV == 2)
        indexV = 0;
    else
        indexV = qrand() % (nV - 1);

    // разбиваем комнату на четыре сектора
    for (int i = 0; i < nV; i++)
        cells[sV + i][indexH + sH]->set_right_edge(true);

    for (int i = 0; i < nH; i++)
        cells[indexV + sV][sH + i]->set_bottom_edge(true);


    // находим номера ячеек на границе в которых делаем отверстия
    int holeH, holeV;

    if ( indexH == 0 ) {
        holeH = 0;
        cells[indexV + sV][holeH + sH]->set_bottom_edge(false);
        holeH = 1;
        cells[indexV + sV][holeH + sH]->set_bottom_edge(false);
    } else if ( nH - indexH == 2 ) {
        holeH = qrand() % (indexH + 1);
        cells[indexV + sV][holeH + sH]->set_bottom_edge(false);
        holeH = indexH + 1;
        cells[indexV + sV][holeH + sH]->set_bottom_edge(false);
    } else {
        holeH = qrand() % (indexH + 1);
        cells[indexV + sV][holeH + sH]->set_bottom_edge(false);
        holeH = qrand() % (nH - indexH - 2) + indexH + 1;
        cells[indexV + sV][holeH + sH]->set_bottom_edge(false);
    }

    if ( indexV == 0 ) {
        holeV = 0;
        cells[holeV + sV][indexH + sH]->set_right_edge(false);
        holeV = 1;
        cells[holeV + sV][indexH + sH]->set_right_edge(false);
    } else if ( nV - indexV == 2 ) {
        holeV = qrand() % (indexV + 1);
        cells[holeV + sV][indexH + sH]->set_right_edge(false);
        holeV = indexV + 1;
        cells[holeV + sV][indexH + sH]->set_right_edge(false);
    } else {
        holeV = qrand() % (indexV + 1);
        cells[holeV + sV][indexH + sH]->set_right_edge(false);
        holeV = qrand() % (nV - indexV - 2 ) + indexV + 1;
        cells[holeV + sV][indexH + sH]->set_right_edge(false);
    }

    // закрываем один из проходов
    switch (qrand() % 4) {
        case 0: // горизонтальный слева
            for (int i = sH; i < sH + indexH + 1; i++)
                cells[indexV + sV][i]->set_bottom_edge(true);
            break;
        case 1: // горизонтальный справа
            for (int i = 0; i < nH - indexH - 1; i++)
                cells[indexV + sV][indexH + 1 + i + sH]->set_bottom_edge(true);
            break;
        case 2: // вертикальный сверху
            for (int i = sV; i < sV + indexV + 1; i++)
                cells[i][indexH + sH]->set_right_edge(true);
            break;
        case 3: // вертикальный снизу
            for (int i =  0; i < nV - indexV - 1; i++)
                cells[indexV + 1 + i + sV][indexH + sH]->set_right_edge(true);
            break;
    }

    // рекурсия в нижний правый отсек
    generate(nH - indexH - 1, nV - indexV - 1, sH + indexH + 1 , sV + indexV + 1);
    // рекурсия в верхний левый отсек
    generate(indexH + 1, indexV + 1, sH, sV);
    // рекурсия в верхний правый отсек
    generate(nH - indexH - 1, indexV + 1, sH + indexH + 1, sV);
    // рекурсия в нижний левый отсек
    generate(indexH + 1, nV - indexV - 1, sH, sV + indexV + 1);
}

void MainWindow::initMaze(){
    cells_h = 20;
    cells_v = 20;

    width = 20;
    height = 20;

   // shiftForPoints = 2;
    //x1 = shiftForPoints;
   // y1 = shiftForPoints;
   // x2 = cells_h * width - (width - shiftForPoints);
   // y2 = cells_v * height - (height - shiftForPoints);
    //  path_found = false;

    // Выделяем память
     alg_Li = new int*[cells_h];
     for (int i = 0; i < cells_h; i++)
         alg_Li[i] = new int[cells_v];

     cells = new Square** [cells_h];
     for (int i = 0; i < cells_h; i++)
         cells[i] = new Square* [cells_v];

     for (int i = 0; i < cells_v; i++)
        for (int j = 0; j < cells_h; j++)
            cells[i][j] = new Square(0, 0, 0, 0);

     // приводим ячейки к виду пустого лабиринта
     clearCells();
}


void MainWindow::cleareMaze(){
    // Освобождаем выделенную ранее память
    for (int i = 0; i < cells_h; i++)
        for (int j =0; j < cells_v; j++)
            delete cells[i][j];

    for (int i = 0; i < cells_h; i++) {
       delete [] cells[i];
    }
    delete [] cells;

    for (int i = 0; i < cells_h; i++) {
        delete [] alg_Li[i];
    }
    delete [] alg_Li;

}

