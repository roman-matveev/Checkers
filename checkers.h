#ifndef CHECKERS_H
#define CHECKERS_H

#include <QMainWindow>
#include <iostream>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <cstdlib>      //allows us to call rand()
#include <ctime>        //allows us to call time()

class Piece {
public:

    Piece(bool makeRed, bool makeKing);
    void draw(QPainter* p, int x, int y);

    bool isRed;
    bool isKing;

    bool canMove(int startx, int starty, int endx, int endy);
    void doMove(int startx, int starty, int endx, int endy);

};





class Checkers : public QMainWindow
{
    Q_OBJECT

public:
    Checkers(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    void randomAI();
    int startx, starty, endx, endy;

    bool gameOver();

    ~Checkers();

};

#endif // CHECKERS_H
