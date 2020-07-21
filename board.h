#ifndef COARD_H
#define COARD_H
#include <vector>
#include <QDialog>
#include <QtCore>
#include <QtWidgets>
#include <QTimer>
#include <string>
#include <unordered_map>
#include <queue>
#include <stack>

#include <fstream>
#include <QPainter>
#include <ctime>
class Board : public QGraphicsScene
{
    Q_OBJECT
public:
    Board();
    void displayBoard();
    void generateGame(std::string fileName);
    void testOptionCounts();

    void keyPressEvent(QKeyEvent * event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent * event) override;
    void generateDifficulty(int minDifficult, int maxDifficulty);

private:
    static const int SUDOKU_SQUARE_CELLS_WIDTH=3;//width of a square in Sudkou ( base game has nine number in a square
    static const int BOARD_WIDTH=SUDOKU_SQUARE_CELLS_WIDTH*SUDOKU_SQUARE_CELLS_WIDTH;
    static const int BOARD_SQUARES=BOARD_WIDTH*BOARD_WIDTH;

    static const int BLOCK_SIZE=50;
    struct Entry
    {
        int m_index;
        int m_number;
        int m_guessCorner; // 0 is top left 1 top right, 2 bottom left, 3 bottom right
        int m_heapIndex;
        std::vector<int> m_guesses;
        int m_editMode; // 0 not editing current Entry, 1 editing full choice, 2 editing guess
        bool m_isConfirmed; // this will show if the entry was given in the puzzle
        int m_numOptions;
        Entry();
    };

    struct LightUpSquare : public QGraphicsObject//used to display when an entry is not allowed
    {
        int m_index;
        QBrush m_lightUpBrush;
        LightUpSquare(int index, QColor color);
        QRectF boundingRect() const;
        void paint(QPainter *painter,const QStyleOptionGraphicsItem *style, QWidget *widget);
    };

    struct Heap
    {
        int size;
        int capacity;
        std::vector<std::pair<int, int>> squaresToChoose;

    };
    Heap solveOrder;
    std::vector<std::vector<int>> m_squareCounts;
    std::vector<std::vector<int>> m_rowCounts;//first number is the row number, then the number entry
    std::vector<std::vector<int>> m_colCounts;
    std::vector<int> m_solution;
    std::vector<Entry> m_board;
    int m_confirmedSquares;
    int m_currentIndex;

    //heap methods
    int right(const int index);
    int left(const int index);
    int parent(const int index);
    void push(std::pair<int, int> add);
    std::pair<int, int> heapTop();
    std::pair<int, int> heapPop();
    void deleteHeapIndex(int index);
    void bubbleDown(int index);
    void bubbleUp(int index);
    void heapSwap(int parent, int child);
    void heapify();
    void initializeHeap();

    void displayAllEntries();
    void resetLevel();
    void generateRandomFinishedBoard();
    int measureDifficulty();
    void heapSolve();
    void linearSolve();
    bool displayNumber(int number, int index);
    void displayGuess(int number, int index);
    void displayEntry(int index);
    void deSelectEntry();
    void displayBox(QColor color, int xCoord, int yCoord, int size);
    void displayThickBorders();
    void delay(int time);
    void lightSquare(int index, QColor color);
    void iterateHeapSolution(std::stack<std::pair<int, int>> &choices, std::pair<int, int> &currentPair);
    static int indexToXCoord(int index);
    static int indexToYCoord(int index);
    static int indexToRow(int index);
    static int indexToCol(int index);
    static int indexToSquare(int index);
    static int coordToIndex(int xCoord, int yCoord);
    static std::vector<int> randVector(int count);
    void updateHeapAddedEntry(int index, int number);
    void updateHeapDeletedEntry(int index, int number);
    void addNumberAnimation(int index, int number, QSequentialAnimationGroup *animateOrder);
    void deleteNumberAnimation(int index, int number, QSequentialAnimationGroup *animateOrder);
    void changeNumberAnimation(int index, int number, QSequentialAnimationGroup *animateOrder);

    void deleteEntry(int index);

};

#endif // COARD_H
