#include "checkers.h"
#include "windows.h"
using namespace std;

const int xSquare = 8;
const int ySquare = 8;

Piece* pieces[ySquare][xSquare];

bool turn = true;

int restrictedX = 10; // for the second "take", red must start with these pieces to remember where the first take ended
int restrictedY = 10; // 10 because it'll definitely be a move out of the board, just to initialize

/*  INITIALIZE THE CHECKERS WINDOW, DEFINE PIECE PLACEMENT */

Checkers::Checkers(QWidget *parent)
    : QMainWindow(parent)
{

    srand(time(0));
    resize(80*8, 80*8);

    /* Real board pieces*/

    for (int i = 0; i < ySquare; i++) {

        for (int j = 0; j < xSquare; j++) {

            pieces[j][i] = NULL;

            if ( (!(j % 2) && !(i % 2)) || ((j % 2) && (i % 2)) ) {

                if (j >= 0 && j <= 2) {
                    pieces[j][i] = new Piece(false,false);
                }

                else if (j >= 5 && j <= 7) {
                    pieces[j][i] = new Piece(true,false);

                }
            }
        }
    }

    /* Testing pieces */

//    pieces[1][1] = new Piece(false, false);
//    pieces[3][1] = new Piece(true, false); // me
}

Piece::Piece(bool makeRed, bool makeKing) {

    isKing = makeKing;
    isRed = makeRed;

}

void Piece::draw(QPainter* p, int x, int y) {

    if (pieces[y][x] == NULL) {
        return;
    }

    if(isRed) {
        p->setBrush(Qt::red);
    }

    else {
        p->setBrush(Qt::black);
    }

    p->drawEllipse(QPoint(x*80 + 80/2, y*80 + 80/2), 80 / 2 - 4, 80 / 2 - 4);

    if(isKing) {
        p->setBrush(Qt::white);
        p->drawEllipse(QPoint(x*80 + 80/2, y*80 + 80/2), 5, 5);
    }
}

void Piece::doMove(int startx, int starty, int endx, int endy) { // actions and modifications to pieces

    if (pieces[starty][startx]->isRed && (endy == 0)) { // red got to the end, king

        pieces[starty][startx]->isKing = true;
    }

    if (!pieces[starty][startx]->isRed && (endy == 7)) { // black got to the end, king

        pieces[starty][startx]->isKing = true;
    }

    /* ^^^ Replaced by vvv (from canMove) */

    int posx, posy, counter = 0;

    for (int step = 1; step < (abs(endx - startx)); step++) {

        if (endx > startx) {
            posx = startx + step; // top left
        }
        else {
            posx = startx - step; // bottom right
        }

        if (endy > starty) {
            posy = starty + step; // top right
        }
        else {
            posy = starty - step; // bottom left
        }


        if (pieces[posy][posx] != NULL) {
            delete pieces[posy][posx];  // for memory leak
            pieces[posy][posx] = NULL;
        }
    }

    pieces[endy][endx] = pieces[starty][startx];
    pieces[starty][startx] = NULL;                  // actually move and then delete jumped piece

}

bool Piece::canMove(int startx, int starty, int endx, int endy) {

    if (restrictedX != 10 || restrictedY != 10) { // ONLY FOR RED MOVES

        if (startx != restrictedX || starty != restrictedY) { // if you're starting with a piece that isn't the one that just ate
            return false;
        }

        if (abs(startx - endx) != 2) {  // second move must be one that takes a piece
            return false;
        }
    }

    if ((pieces[starty][startx] == NULL)) { // nobody can start with no piece
        return false;
    }

    if (turn && !(pieces[starty][startx]->isRed)) { // if my turn but not red piece
        return false;
    }

    if (!turn && (pieces[starty][startx]->isRed)) { // if not my turn but trying to move red piece
        return false;
    }
    //qDebug() << "Here 0" << endl;

    if (startx > 7|| starty > 7 || endx > 7 || endy > 7 ||
            startx < 0 || starty < 0 || endx < 0 || endy < 0) { // within the board
        return false;
    }
    //qDebug() << "Here 1" << endl;

    if (pieces[endy][endx] != NULL) { // place is occupied

        return false;

    }
    //qDebug() << "Here 2" << endl;

    if (!pieces[starty][startx]->isKing) {

            if (((pieces[starty][startx]->isRed) && (endy > starty)) ||
              !pieces[starty][startx]->isRed && (endy < starty)) { // pieces cant go back

                return false;

            }

    }
    //qDebug() << "Here 3" << endl;

    if ((!pieces[starty][startx]->isKing) &&
            !((abs(startx - endx) == 1 && abs(starty - endy) == 1) ||
            (abs(startx - endx) == 2 && abs(starty - endy) == 2))) {

        return false;
    }
    //qDebug() << "Here 4" << endl;

    if (!pieces[starty][startx]->isKing &&
            (abs(startx - endx) == 2 && abs(starty - endy) == 2)) { // if jumped

        int midX = (startx + (endx - startx) / 2);
        int midY = (starty + (endy - starty) / 2);

        if ((pieces[midY][midX] == NULL) ||
                (turn && (pieces[midY][midX]->isRed) ||
                 (!turn && (!pieces[midY][midX]->isRed)))) { // dont jump your own pieces or blanks
            return false;
        }
    }
    //qDebug() << "Here 5" << endl;

    if (pieces[starty][startx]->isKing) {

        if (abs(startx - endx) != abs(starty - endy)) { // goes diagonally

            return false;
        }

        int posx, posy, counter = 0;

        for (int step = 1; step < (abs(endx - startx)); step++) {

            if (endx > startx)
                posx = startx + step; // top left
            else
                posx = startx - step; // bottom right

            if (endy > starty)
                posy = starty + step; // top right
            else
                posy = starty - step; // bottom left


            if (pieces[posy][posx] != NULL) {

                if ((pieces[posy][posx]->isRed && turn) ||
                    (!pieces[posy][posx]->isRed && !turn)) { // cannot eat own pieces

                return false;

                }

                if ((!pieces[posy][posx]->isRed && turn) ||
                        (pieces[posy][posx]->isRed && !turn)) { // counting opponent pieces so you don't eat more than one

                    counter++;

                }
            }

        }

        if (counter > 1) {

            return false;
        }
    }

    return true;
}

void Checkers::mousePressEvent(QMouseEvent* e) {

    startx = e->x() / 80;
    starty = e->y() / 80;

}

void Checkers::mouseReleaseEvent(QMouseEvent* e) {

    endx = e->x() / 80;
    endy = e->y() / 80;

    if (pieces[starty][startx]->canMove(startx, starty, endx, endy)) {

        pieces[starty][startx]->doMove(startx, starty, endx, endy); // only if value 2 is true, then the piece will be eaten

        repaint();

        // check first move

        if (!pieces[endy][endx]->isKing &&
                abs(startx - endx) == 2 &&
                pieces[endy][endx]->canMove(endx, endy, endx + 2, endy - 2)) { // not king, a move that takes, and next is legal move

            restrictedX = endx; // you have to make the next move with this same piece
            restrictedY = endy;

            return; // give player another chance to take

        }

        // check second move

        if (!pieces[endy][endx]->isKing &&
                abs(startx - endx) == 2 &&
                pieces[endy][endx]->canMove(endx, endy, endx - 2, endy - 2)) {

            restrictedX = endx; // make the next move, but in the other direction
            restrictedY = endy;

            return; // successful again

        }


        // there are no more pieces to take

        restrictedX = 10; // reset restricted pieces
        restrictedY = 10;

        turn = !turn;

        if (turn) {
            setWindowTitle("YOUR MOVE");
        }

        else {
            setWindowTitle("OPPONENT IS THINKING");
        }

        repaint();

        if (gameOver()) { // if game ends before computer's turn
            return;
        }

        Sleep(1000);
        randomAI();

        if (gameOver()) { // if game ends before our turn
            return;
        }

    }

    else {

        qDebug() << "Bad move. Try again." << endl;
    }

}


void Checkers::paintEvent(QPaintEvent* e) {

    QPainter paint(this);

    paint.setBrush(QColor(240,128,128));

    for (int i = 0; i < ySquare; i++) {

        for (int j = 0; j < xSquare; j++) {

            if ( (!(j % 2) && !(i % 2)) || ((j % 2) && (i % 2)) ) {

                paint.setBrush(QColor(240,128,128));
                paint.drawRect(i * 80, j * 80, 80, 80);
            }

            pieces[j][i]->draw(&paint,i,j);
        }
    }
}

void Checkers::randomAI() {

    int startx, starty, endx, endy;

    while (true) {

        startx = rand()%8;
        starty = rand()%8;

        if ((pieces[starty][startx] == NULL) ||
                (pieces[starty][startx]->isRed)) { // if random spot is not black, redo
            continue;
        }

        endx = rand()%8;
        endy = rand()%8;

        if ((pieces[starty][startx]->canMove(startx, starty, endx, endy))) { // if AI can move,

            pieces[starty][startx]->doMove(startx, starty, endx, endy); // AI moves

            repaint();

            while (!pieces[endy][endx]->isKing && abs(startx - endx) == 2) { // if not king and the last move was a capture, go into

                Sleep(1000);

                startx = endx; // the old starting position is now the new starting position
                starty = endy;

                // check first move

                endx = startx + 2;
                endy = starty + 2;

                if (pieces[starty][startx]->canMove(startx, starty, endx, endy)) {

                    pieces[starty][startx]->doMove(startx, starty, endx, endy);
                    repaint();
                    continue; // try to take one more piece if successful

                }

                // check second move

                endx = startx - 2;
                endy = starty + 2;

                if (pieces[starty][startx]->canMove(startx, starty, endx, endy)) {

                    pieces[starty][startx]->doMove(startx, starty, endx, endy);
                    repaint();
                    continue; // try again

                }

                break; // no other pieces to be taken

            }

            turn = !turn;

            if (turn) {
                setWindowTitle("YOUR MOVE");
            }

            else {
                setWindowTitle("OPPONENT IS THINKING");
            }

            repaint();
            return;
        }

    }
}

bool Checkers::gameOver() {

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            if (pieces[j][i] == NULL) {

                continue;
            }

            if ((pieces[j][i]->isRed && turn) ||     // check for red on red turn
                (!pieces[j][i]->isRed && !turn)) { // check for black on black turn

                startx = i; // finds every red piece
                starty = j;

                for (int k = 0; k < 8; k++) {
                    for (int l = 0; l < 8; l++) {

                        endx = k; // check every square for possiblity of end position
                        endy = l;

                        //qDebug() << i << j << k << l << endl;

                        if (pieces[starty][startx]->canMove(startx, starty, endx, endy)) { // can you still move to them? then game isn't over yet

                            return false;

                        }
                    }
                }
            }
        }
    }

    if (turn) {
        setWindowTitle("GAME OVER -- BLACK WINS. YOU BRING SHAME UPON YOUR FAMILY.");
    }

    else {
        setWindowTitle("GAME OVER -- YOU WIN. CLICK HERE TO CLAIM 1 FREE INTERNET.");
    }

    repaint();
    return true;

}

Checkers::~Checkers() {

}
