#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    gravity_time_average = 40;
    gravity_time_dispersion = 50;
    //gravity_timer_time = 40;
    on_gravity_level_sliderMoved(1);
    on_maze_level_sliderMoved(1);
    res = new Results(".");
/*    std::vector<std::pair<std::string,double> > data = res->GetResults(); //получили результаты
    std::vector<std::pair<std::string,double> >::iterator it;
    //вывели
    for(it=data.begin();it!=data.end();it++)
    {
        std::pair<std::string,double> temp = *it;
        qDebug()<<temp.first.c_str();
        qDebug()<<temp.second;
    }
    //добавили
    res->AddResult("User 1.0",1.0);
    //получили результаты
    data = res->GetResults();
    //вывели
    for(it=data.begin();it!=data.end();it++)
    {
        std::pair<std::string,double> temp = *it;
        qDebug()<<temp.first.c_str();
        qDebug()<<temp.second;
    }*/

    tmr = new QTimer(this);
    key_tmr = new QTimer(this);
    connect(tmr, SIGNAL(timeout()), this, SLOT(timerTick()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerDone()));
    timer->start(10);
    caption = "";

    gravity_level = 50;
    maze_level = 2;
    utime = 0.0;
    connect(key_tmr, SIGNAL(timeout()), this, SLOT(Key_timerTick()));
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));
    ui->setupUi(this);

    connect(ui->pushButton_generate, SIGNAL(clicked()), this, SLOT(blockInterface()));
    connect(ui->pushButton_generate, SIGNAL(clicked()), this, SLOT(generate()));
    connect(this, SIGNAL(emitGenerationComplete()), this, SLOT(unBlockInterface()));

    input_uname = "Unknown";
    ui->maze_level->setMinimum(1);
    ui->maze_level->setMaximum(4);
    ui->gravity_level->setMinimum(1);
    ui->gravity_level->setMaximum(10);

    this->setFixedSize(500,645);
    ui->graphicsView->setFixedSize(480,480);
    ui->graphicsView->setBackgroundBrush(QBrush(Qt::darkGray));
    scene = new QGraphicsScene(this);

    user_time = 0;
    your_time = new QTime();
    time_d = "";
    time_r = "";
    pm = new QPixmap(470,470);

    painter = new QPainter(pm);
    font = new QFont("Bavaria");

    left = true; // налево - свободно
    right = true; // направо - свободно
    up = true; // вверх - свободно
    bottom = true; // вниз - свободно
    r = 7;
    indent = 35;
    x = indent + r-1;
    y = indent + r-1;

    initMaze();
    clearCells();

    just_won = false;
    ui->graphicsView->setScene(scene);

    std::vector<std::pair<std::string,double> > data = res->GetResults(); //получили результаты
    std::vector<std::pair<std::string,double> >::iterator it;
    it = data.begin();
    std::pair<std::string,double> temp = *it;
    time_d = time_d.number(temp.second);
    ui->best_time_view->setText(time_d);

    ui->your_time_view->setText("Let's play");
}

void MainWindow::timerTick()
{
    timer_tick_counter--;
    if(timer_tick_counter <0)
    {
        timer_tick_counter = gravity_time_average + (qrand() % gravity_time_dispersion);
        grav_direction = qrand() % 4;
    }
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
    if (!just_won)
    {
        QPen pen;
        painter->setBrush(QBrush(Qt::black));
        // красный контур - нет гравитации
        // cиний - влево
        if (grav_direction == 2)
        {
            pen.setColor(Qt::cyan);
            painter->setBrush(QBrush(Qt::cyan));
        }
        // желтый - вниз
        if (grav_direction == 3)
        {
            pen.setColor(Qt::yellow);
            painter->setBrush(QBrush(Qt::yellow));
        }
        // розовый - вправо
        if (grav_direction == 0)
        {
            pen.setColor(Qt::magenta);
            painter->setBrush(QBrush(Qt::magenta));
        }
        // белый - вверх
        if (grav_direction == 1)
        {
            pen.setColor(Qt::white);
            painter->setBrush(QBrush(Qt::white));
        }

        // pen.setWidth(1);
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

        utime = (double)your_time->elapsed()/1000;
        caption_time = QString("%1").
                arg(utime);
        ui->your_time_view->setText(caption_time);

        if (color.name() == "#00ff00"){
            // добрались до финиша
            just_won = true; // выиграл, пока не нажал Генерация, ничего рисовать не надо
            tmr->stop();
            time_d = time_d.number(your_time->elapsed()/1000);
            time_r = time_r.number(your_time->elapsed()%1000);
            utime = (double)your_time->elapsed()/1000;
            bool ok;
            painter->setPen(QPen(Qt::black));
            painter->setPen(QPen(Qt::black));
            painter->drawEllipse(x-r,y-r,r*r,r*r);

            input_uname = QInputDialog::getText(NULL, tr("Input your name"),tr("Your name:"), QLineEdit::Normal,
                                                QDir::home().dirName(), &ok);
            if (!input_uname.isEmpty())
                res->AddResult(input_uname.toStdString(),utime);

            painter->setBrush(QBrush(Qt::black));
            painter->drawRect(QRectF(0,0,pm->width(),pm->height()));
            painter->setPen(QPen(Qt::white));
            font->setPointSize(12);
            font->setWeight(QFont::Bold);
            painter->setFont(*font);
            painter->drawText(QRectF(pm->width()/2-100,pm->height()/2-100,400,400),input_uname+", your time is "+time_d+","+time_r+" s");

            // отрисовываем 5 лучших результатов

            std::vector<std::pair<std::string,double> > data = res->GetResults(); //получили результаты
            std::vector<std::pair<std::string,double> >::iterator it;
            //вывели
            int i = 1;
            font->setPointSize(10);
            painter->setPen(QPen(Qt::lightGray));
            font->setWeight(QFont::Bold);
            painter->setFont(*font);

            for(it=data.begin();it!=data.end();it++,i++)
            {
                std::pair<std::string,double> temp = *it;
                time_d = time_d.number(temp.second);
                //qDebug() << input_uname << " сравниваем с " << QString(temp.first.c_str());

                painter->drawText(QRectF(pm->width()/2-100,pm->height()/2-80+15*i,400,400),QString::fromUtf8(temp.first.c_str())+"'s time is "+time_d+" s");

            }

            it = data.begin();
            std::pair<std::string,double> temp = *it;
            time_d = time_d.number(temp.second);
            ui->best_time_view->setText(time_d);
            ui->your_time_view->setText("Let's play again!");
            ui->pushButton_generate->setFocus();
            ui->pushButton_generate->setEnabled(true);

            clearCells();
        }

        painter->setBrush(QBrush(Qt::black));
        scene->clear();

        scene->addPixmap(*pm);
    }
}

void MainWindow::generate()
{
    key_up_pressed = false;
    key_down_pressed = false;
    key_left_pressed = false;
    key_right_pressed = false;
    timer_tick_counter = gravity_time_average + (qrand() % gravity_time_dispersion);    // Гравитация <=======================> (!!!!!!!!!!!!!)
    x = indent + r-1;
    y = indent + r-1;
    clearCells();
    generate(cells_h, cells_v, 0, 0);
    grav_direction = qrand() % 4;
    emitGenerationComplete();
    painter->setPen(QPen(Qt::gray));
    painter->drawRect(2,2,pm->width()-2,pm->height()-2);
    painter->setPen(QPen(Qt::gray, 5));
    for (int i = 0; i < cells_v; i++)
        for (int j = 0; j < cells_h; j++) {
            if ( cells[i][j]->get_left_edge() ) //
                painter->drawLine(j * width + indent,  i * height + indent, j * width + indent,  (i + 1) * height + indent);
            if ( cells[i][j]->get_top_edge() )
                painter->drawLine(j * width + indent,  i * height + indent, (j + 1) * width + indent, i * height + indent);
            if ( cells[i][j]->get_right_edge() )
                painter->drawLine((j + 1) * width + indent,  i * height + indent, (j + 1) * width + indent, (i + 1) * height + indent);
            if ( cells[i][j]->get_bottom_edge() )
                painter->drawLine(j * width + indent,  (i + 1) * height + indent, (j + 1) * width + indent, (i + 1) * height + indent);
        }
    // cиний - влево
    //if (grav_direction == 2)
    // {
    //pen.setColor(Qt::cyan);
    painter->setPen(QPen(Qt::cyan,indent*2-2));
    painter->drawLine(1,1,1,1000);
    // }
    // желтый - вниз
    //if (grav_direction == 3)
    //{
    painter->setPen(QPen(Qt::yellow,indent*2-2));
    painter->drawLine(1,pm->height(),1000,pm->height());
    //}
    // розовый - вправо
    //if (grav_direction == 0)
    //{
    painter->setPen(QPen(Qt::magenta,indent*2-2));
    painter->drawLine(pm->width(),1,pm->width(),1000);
    //}
    // белый - вверх
    //if (grav_direction == 1)
    //{
    painter->setPen(QPen(Qt::white,indent*2-2));
    painter->drawLine(1,1,1000,1);
    //}
    QPen pen(Qt::gray, 2);
    painter->setPen(pen);

    pen.setColor(Qt::green);
    pen.setWidth(3);
    painter->setPen(pen);
    painter->setBrush(QBrush(Qt::darkGreen));
    //painter->drawEllipse(60,60,30,30);
    painter->drawEllipse(indent+400-8,indent+400-8,12,12);
    just_won = false; // начал игру, еще не выиграл

    your_time->start();
    tmr->start(gravity_timer_time);
    key_tmr->start(25);
    repaint();
}

void MainWindow::keyPressEvent(QKeyEvent * e)
{
    if (!just_won){

        switch (e->key())
        {
        case Qt::Key_Escape:
            this->close();
            return;
        case Qt::Key_Up:
            key_up_pressed = true;
            break;
        case Qt::Key_Down:
            key_down_pressed = true;
            break;
        case Qt::Key_Left:
            key_left_pressed = true;
            break;
        case Qt::Key_Right:
            key_right_pressed = true;
            break;
        default:
            e->ignore();
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    if (!just_won) {
        switch (e->key())
        {
        case Qt::Key_Escape:
            this->close();
            return;
        case Qt::Key_Up:
            key_up_pressed = false;
            break;
        case Qt::Key_Down:
            key_down_pressed = false;
            break;
        case Qt::Key_Left:
            key_left_pressed = false;
            break;
        case Qt::Key_Right:
            key_right_pressed = false;
            break;
        default:
            e->ignore();
        }
    }
}

void MainWindow::Key_timerTick()
{
    if(!(key_down_pressed||key_left_pressed||key_right_pressed||key_up_pressed))return;
    QPen pen;
    pen.setColor(Qt::black);
    painter->setBrush(QBrush(Qt::black));
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawEllipse(x,y,r,r);
    if (key_up_pressed && up){
        y=y-1;
        if (y < 0)
            y=0;
    }

    if (key_down_pressed&&bottom){
        y=y+1;
        if (y >= pm->height()-r)
            y=pm->height()-1;
    }

    if (key_left_pressed&&left){
        x=x-1;
        if (x < 0)
            x=0;
    }
    if (key_right_pressed&&right){
        x=x+1;
        if (x >= pm->width())
            x=pm->width()-1;
    }

    left = true;
    right = true;
    bottom = true;
    up = true;
    repaint();

}

void MainWindow::blockInterface() {
    ui->pushButton_generate->setDisabled(true);
    ui->gravity_level->setDisabled(true);
    ui->maze_level->setDisabled(true);
}

void MainWindow::unBlockInterface() {
    ui->pushButton_generate->setEnabled(true);
    ui->gravity_level->setEnabled(true);
    ui->maze_level->setEnabled(true);
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
    if ( (nH <= complexity || nV <= complexity) )
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

MainWindow::~MainWindow()
{
    cleareMaze();
    delete ui;
}

void MainWindow::on_maze_level_sliderMoved(int position)
{
    //qDebug()<<"old complexity = "<<complexity;
    maze_level = position;
    complexity = 5-maze_level;
    //qDebug()<<"complexity = "<<complexity;
}

void MainWindow::on_gravity_level_sliderMoved(int position)
{
    gravity_timer_time = 34+6*(11-position);
    gravity_time_average = 20+5*(11-position);
    gravity_time_dispersion = gravity_time_average;
    //qDebug()<<"on_gravity_level_sliderMoved"<<gravity_timer_time;
}
