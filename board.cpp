#include "board.h"

Board::Board()
{
    m_confirmedSquares = 0;
    m_board.resize(BOARD_SQUARES);
    m_solution.resize(BOARD_SQUARES);

    m_squareCounts.resize(BOARD_WIDTH);
    m_rowCounts.resize(BOARD_WIDTH);
    m_colCounts.resize(BOARD_WIDTH);

    for(int i=0; i<BOARD_WIDTH; i++)
    {
        m_squareCounts[i].resize(BOARD_WIDTH);
        m_rowCounts[i].resize(BOARD_WIDTH);
        m_colCounts[i].resize(BOARD_WIDTH);
    }
    for(int i=0; i<BOARD_SQUARES; i++)
    {
        Entry firstEntry;
        firstEntry.m_number = -1;
        m_board[i]=firstEntry;
    }
    for(int i=0; i<BOARD_WIDTH; i++)
    {
        for(int j=0; j<BOARD_WIDTH; j++)
        {
            m_squareCounts[i][j]=-1;
            m_rowCounts[i][j]=-1;
            m_colCounts[i][j]=-1;
        }
    }

    m_currentIndex = -1;
}

void Board::displayBoard()
{
    const int boardWidth = SUDOKU_SQUARE_CELLS_WIDTH*SUDOKU_SQUARE_CELLS_WIDTH;
    const int numCells = boardWidth*boardWidth;

    QGraphicsRectItem * rectangle;
    QBrush whiteBrush(Qt::white);
    QPen blackPen(Qt::black);
    int xCoord = 0;
    int yCoord = 0;

    displayThickBorders();

    xCoord = 0;
    yCoord = 0;
    blackPen.setWidth(1);

    for(int i=0; i<boardWidth; i++)
    {
        for(int j=0; j<boardWidth; j++)
        {
            rectangle = addRect(xCoord, yCoord, BLOCK_SIZE, BLOCK_SIZE, blackPen);
            xCoord+=BLOCK_SIZE;
        }
        xCoord=0;
        yCoord+=BLOCK_SIZE;
    }

    xCoord = 0;
    yCoord = 0;


}

void Board::generateGame(std::string fileName)
{
    std::ifstream currentFile;
    currentFile.open(fileName);
    char readDigit;
    int nextCell=0;
    solveOrder.squaresToChoose.resize(BOARD_SQUARES);
    for(int i=0; i<BOARD_SQUARES; i++)
    {
        currentFile>>readDigit;
        nextCell = readDigit - '0';
        nextCell--;
        if(nextCell!=-1)
        {
            displayNumber(nextCell, i);
            m_board[i].m_isConfirmed = true;
            m_confirmedSquares++;
            displayEntry(i);

        }
    }

}

void Board::testOptionCounts()
{
    for(int i=0; i<BOARD_SQUARES; i++)
    {
        if(m_board[i].m_number==-1)
        {
            int xCoord = indexToXCoord(i);
            int yCoord = indexToYCoord(i);
            displayBox(Qt::white, xCoord, yCoord, BLOCK_SIZE);
            QGraphicsTextItem * io = new QGraphicsTextItem;
            QGraphicsRectItem * rectangle;
            QBrush whiteBrush(Qt::white);
            QPen blackPen(Qt::black);
            blackPen.setWidth(1);

            QFont addFont("Helvetica [Cronyx]", 12, 3, false);
            io->setPos(xCoord+BLOCK_SIZE/3, yCoord+BLOCK_SIZE/8);
            io->setPlainText(QString::number(m_board[i].m_numOptions));
            io->setFont(addFont);
            addItem(io);
            displayThickBorders();


        }

    }
}

void Board::heapSolve()
{
    initializeHeap();

    std::stack<std::pair<int, int>> choices;
    std::pair<int, int> currentPair(heapTop().second,0);
    heapPop();
    while(choices.size()+m_confirmedSquares<BOARD_SQUARES)
    {
        iterateHeapSolution(choices, currentPair);
    }
}

void Board::linearSolve()
{
    std::vector<std::pair<int, int>> squaresToChoose;
    squaresToChoose.resize(BOARD_SQUARES - m_confirmedSquares);
    int currentIndex = 0;
    std::vector<std::vector<int>> guessOrder;
    guessOrder.resize(BOARD_WIDTH);
    for(int i=0; i<BOARD_WIDTH; i++)
    {
        guessOrder[i].resize(BOARD_WIDTH);
        guessOrder[i] = randVector(BOARD_WIDTH);
    }
    for(int i=0; i<BOARD_SQUARES; i++)
    {
        if(!m_board[i].m_isConfirmed)
        {
            std::pair<int, int> addToSquares(m_board[i].m_numOptions, i);
            squaresToChoose[currentIndex] = addToSquares;
            currentIndex++;
        }
    }

    currentIndex = 0;
    std::stack<std::pair<int, int>> choices;
    std::pair<int, int> addPair(squaresToChoose[currentIndex].second, 0);
    choices.push(addPair);
    while(currentIndex<squaresToChoose.size())
    {
        std::pair<int, int> currentPair = choices.top();
        int currentNumber = currentPair.second;
        int currentBoardIndex = currentPair.first;
        if(currentNumber>=BOARD_WIDTH)//exhausted all possiblilites for this square for this path, go back one guess
        {
            deleteEntry(currentPair.first);
            //lightSquare(currentPair.first, Qt::red);
            //delay(100);
            choices.pop();
            currentIndex--;
        }
        else if(displayNumber(guessOrder[indexToRow(currentBoardIndex)][currentNumber], currentBoardIndex ))//the placement was succesful
        {
            //delay(100);
            currentIndex++;
            displayEntry(choices.top().first);
            if(currentIndex>=squaresToChoose.size())
            {
                break;
            }
            std::pair<int, int> nextPair(squaresToChoose[currentIndex].second, 0);
            choices.push(nextPair);
        }
        else//placement failed try next option
        {
            std::pair<int,int> offPair = choices.top();
            choices.pop();
            int nextNumber = offPair.second + 1;
            std::pair<int, int> addPair(squaresToChoose[currentIndex].second, nextNumber);
            choices.push(addPair);
        }
    }

}
void Board::keyPressEvent(QKeyEvent * event)
{
    QGraphicsScene::keyPressEvent(event);
    int number=-1;
    if(event->key() == Qt::Key_Z)
    {
        heapSolve();
        return;
    }
    if(event->key() == Qt::Key_X)
    {
        return;
    }
    if(event->key() == Qt::Key_N)
    {
        return;
    }

    if(m_currentIndex==-1)
    {
        return;
    }
    if(event->key() ==Qt::Key_1)
    {
        number=0;
    }
    else if(event->key() ==Qt::Key_2)
    {
        number=1;
    }
    else if(event->key() ==Qt::Key_3)
    {
        number=2;
    }
    else if(event->key() ==Qt::Key_4)
    {
        number=3;
    }
    else if(event->key() ==Qt::Key_5)
    {
        number=4;
    }
    else if(event->key() ==Qt::Key_6)
    {
        number=5;
    }
    else if(event->key() ==Qt::Key_7)
    {
        number=6;
    }
    else if(event->key() ==Qt::Key_8)
    {
        number=7;
    }
    else if(event->key() ==Qt::Key_9)
    {
        number=8;
    }
    else if(event->key() == Qt::Key_Delete)
    {
        deleteEntry((m_currentIndex));
        return;
    }
    else
    {
        return;
    }
    if(m_board[m_currentIndex].m_editMode==1)
    {

        if(!displayNumber(number, m_currentIndex));
        {
            QColor flashColor(255, 153, 51);
            int squareBlockingIndex = m_squareCounts[indexToSquare(m_currentIndex)][number];
            int rowBlockingIndex = m_rowCounts[indexToRow(m_currentIndex)][number];
            int colBlockingIndex = m_colCounts[indexToCol(m_currentIndex)][number];

            if(squareBlockingIndex!=-1)
            {
                lightSquare(squareBlockingIndex, flashColor);
            }
            if(rowBlockingIndex!=-1)
            {
                lightSquare(rowBlockingIndex, flashColor);
            }
            if(colBlockingIndex!=-1)
            {
                lightSquare(colBlockingIndex, flashColor);
            }
        }
        deSelectEntry();
    }
    else if(m_board[m_currentIndex].m_editMode==2)
    {
        displayGuess(number, m_currentIndex);
        deSelectEntry();
    }
}

void Board::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    QBrush whiteBrush(Qt::white);
    QBrush blueBrush(QColor(102, 204, 255, 70));
    QColor red(255, 0, 0, 70);
    QPen blackPen(Qt::black);
    int currentIndex = m_currentIndex;
    int xCoord = event->scenePos().x();
    int yCoord = event->scenePos().y();

    QGraphicsScene::mousePressEvent(event);

    int nextIndex = coordToIndex(xCoord, yCoord);
    if(nextIndex==-1)//selection is out of bounds, deselect current entry
    {
        deSelectEntry();
        return;
    }
    if(m_board[nextIndex].m_isConfirmed)//the space is part of the puzzle and cant be changed
    {

        if(m_currentIndex!=-1)
        {
            deSelectEntry();
        }
        lightSquare(nextIndex, Qt::red);
        lightSquare(nextIndex, Qt::red);
        return;
    }


    if(m_board[nextIndex].m_editMode==0)//square was not selected, it will now be selected for
    {
        if(m_currentIndex!=-1)//don't deselect if it is the first selection
        {
            deSelectEntry();
        }
        m_currentIndex = nextIndex;
        addRect(indexToXCoord(m_currentIndex), indexToYCoord(m_currentIndex), BLOCK_SIZE, BLOCK_SIZE, blackPen, blueBrush);
        m_board[m_currentIndex].m_editMode = 1;
    }
    else if(m_board[nextIndex].m_editMode==1 || m_board[nextIndex].m_editMode==2)
    {
        if(m_board[nextIndex].m_number!=-1)//don't deselect if it is the first selection
        {
            deSelectEntry();
        }
        else
        {
            int xHalf = xCoord/(BLOCK_SIZE/2)%2;
            int yHalf = yCoord/(BLOCK_SIZE/2)%2;
            m_currentIndex = nextIndex;
            displayBox(Qt::white, indexToXCoord(m_currentIndex), indexToYCoord(m_currentIndex), BLOCK_SIZE);
            xCoord = indexToXCoord(m_currentIndex) + (xHalf*(BLOCK_SIZE/2));
            yCoord = indexToYCoord(m_currentIndex) + (yHalf*(BLOCK_SIZE/2));
            m_board[m_currentIndex].m_guessCorner = xHalf + 2*yHalf;
            displayEntry(m_currentIndex);
            displayBox(red, xCoord, yCoord, BLOCK_SIZE/2);
            m_board[m_currentIndex].m_editMode = 2;
        }
    }
}

bool Board::displayNumber(int number, int index)
{

    if(m_squareCounts[indexToSquare(index)][number]==-1 && m_rowCounts[indexToRow(index)][number]==-1 && m_colCounts[indexToCol(index)][number]==-1)//indicates  that is a valid entry
    {
        if(m_board[index].m_number!=-1)
        {
            deleteEntry(index);
        }

        //update the options number on the squares
        int indexSquareStart = (indexToSquare(index)/SUDOKU_SQUARE_CELLS_WIDTH)*(BOARD_WIDTH*SUDOKU_SQUARE_CELLS_WIDTH)+(indexToSquare(index)%SUDOKU_SQUARE_CELLS_WIDTH)*SUDOKU_SQUARE_CELLS_WIDTH;
        int indexSquareEnd = indexSquareStart + BOARD_WIDTH*(SUDOKU_SQUARE_CELLS_WIDTH);

        for(int i = indexSquareStart; i<indexSquareEnd; i+=BOARD_WIDTH)
        {
            for(int j=i; j<i+SUDOKU_SQUARE_CELLS_WIDTH; j++)
            {
                if(m_colCounts[indexToCol(j)][number]==-1 && m_rowCounts[indexToRow(j)][number]==-1 && j!=index)
                {
                    m_board[j].m_numOptions--;
                    int heapIndex = m_board[j].m_heapIndex;
                    if(heapIndex!=0 || heapIndex==heapTop().second)
                    {
                        solveOrder.squaresToChoose[heapIndex].first--;
                    }
                }
            }
        }

        //update the options number on the columns
        for(int i = indexToCol(index); i<BOARD_SQUARES; i+=BOARD_WIDTH)
        {
            if(m_squareCounts[indexToSquare(i)][number]==-1 && m_rowCounts[indexToRow(i)][number]==-1 && indexToSquare(i)!=indexToSquare(index) && i!=index)
            {
                m_board[i].m_numOptions--;
                int heapIndex = m_board[i].m_heapIndex;
                if(heapIndex!=0 || heapIndex==heapTop().second)
                {
                    solveOrder.squaresToChoose[heapIndex].first--;
                }
            }
        }
        //update the options number on the rows
        for(int i = indexToRow(index)*BOARD_WIDTH; i<(indexToRow(index)+1)*BOARD_WIDTH; i++)
        {
            if(m_squareCounts[indexToSquare(i)][number]==-1 && m_colCounts[indexToCol(i)][number]==-1 && indexToSquare(i)!=indexToSquare(index) && i!=index)
            {
                m_board[i].m_numOptions--;
                int heapIndex = m_board[i].m_heapIndex;
                if(heapIndex!=0 || heapIndex==heapTop().second)
                {
                    solveOrder.squaresToChoose[heapIndex].first--;
                }
            }
        }

        m_squareCounts[indexToSquare(index)][number]=index;
        m_rowCounts[indexToRow(index)][number]=index;
        m_colCounts[indexToCol(index)][number]=index;
        m_board[index].m_number=number;


        return true;

    }
    else
    {
        return false;
    }


}

void Board::displayGuess(int number, int index)
{
    QGraphicsTextItem * io = new QGraphicsTextItem;
    int xCoord = indexToXCoord(index);
    int yCoord = indexToYCoord(index);

    if(m_squareCounts[indexToSquare(index)][number]==-1 && m_rowCounts[indexToRow(index)][number]==-1 && m_colCounts[indexToCol(index)][number]==-1)//indicates  that is a valid entry
    {
        if(m_board[m_currentIndex].m_number!=-1)
        {
            return;
        }
        m_board[index].m_guesses[m_board[index].m_guessCorner]=number;
        displayEntry(index);

    }
    else //placement is blocked by another entry, light up that entry to show which
    {
        QColor flashColor(255, 153, 51);
        int squareBlockingIndex = m_squareCounts[indexToSquare(index)][number];
        int rowBlockingIndex = m_rowCounts[indexToRow(index)][number];
        int colBlockingIndex = m_colCounts[indexToCol(index)][number];

        if(squareBlockingIndex!=-1)
        {
            lightSquare(squareBlockingIndex, flashColor);
        }
        if(rowBlockingIndex!=-1)
        {
            lightSquare(rowBlockingIndex, flashColor);
        }
        if(colBlockingIndex!=-1)
        {
            lightSquare(colBlockingIndex, flashColor);
        }

    }


}

void Board::displayEntry(int index)
{
    int xCoord = indexToXCoord(index);
    int yCoord = indexToYCoord(index);
    Entry currentEntry = m_board[index];
    QGraphicsTextItem * io = new QGraphicsTextItem;
    QGraphicsRectItem * rectangle;
    QBrush whiteBrush(Qt::white);
    QPen blackPen(Qt::black);
    blackPen.setWidth(1);
    addRect(xCoord, yCoord, BLOCK_SIZE, BLOCK_SIZE, blackPen, whiteBrush);

    if(currentEntry.m_number!=-1)
    {
        QFont addFont("Helvetica [Cronyx]", 18, 3, false);
        addFont.setWeight(75);
        addFont.setUnderline(true);
        io->setPos(xCoord+BLOCK_SIZE/3, yCoord+BLOCK_SIZE/8);
        io->setPlainText(QString::number(currentEntry.m_number+1));
        if(currentEntry.m_isConfirmed)
        {
            addFont.setBold(true);
            addFont.setUnderline(true);
        }
        else
        {
            addFont.setBold(false);
            addFont.setUnderline(false);
        }
        io->setFont(addFont);
        addItem(io);
        displayThickBorders();
        return;
    }
    if(currentEntry.m_guesses[0]!=-1)
    {
        QGraphicsTextItem * io = new QGraphicsTextItem;
        io->setPlainText(QString::number(currentEntry.m_guesses[0]+1));
        io->setPos(xCoord, yCoord);
        addItem(io);

    }
    if(currentEntry.m_guesses[1]!=-1)
    {
        QGraphicsTextItem * io = new QGraphicsTextItem;
        io->setPlainText(QString::number(currentEntry.m_guesses[1]+1));
        io->setPos(xCoord+BLOCK_SIZE/2, yCoord);
        addItem(io);

    }
    if(currentEntry.m_guesses[2]!=-1)
    {
        QGraphicsTextItem * io = new QGraphicsTextItem;
        io->setPlainText(QString::number(currentEntry.m_guesses[2]+1));
        io->setPos(xCoord, yCoord+BLOCK_SIZE/2);
        addItem(io);
    }
    if(currentEntry.m_guesses[3]!=-1)
    {
        QGraphicsTextItem * io = new QGraphicsTextItem;
        io->setPlainText(QString::number(currentEntry.m_guesses[3]+1));
        io->setPos(xCoord+BLOCK_SIZE/2, yCoord+BLOCK_SIZE/2);
        addItem(io);
    }


}

void Board::deSelectEntry()
{
    if(m_currentIndex==-1)//already nothing selected
    {
        return;
    }
    QBrush whiteBrush(Qt::white);
    QBrush blueBrush(QColor(102, 204, 255, 70));
    QBrush redBrush(QColor(255, 0, 0, 70));
    QPen blackPen(Qt::black);
    int currentIndex = m_currentIndex;
    addRect(indexToXCoord(m_currentIndex), indexToYCoord(m_currentIndex), BLOCK_SIZE, BLOCK_SIZE, blackPen, whiteBrush);
    displayEntry(currentIndex);
    displayThickBorders();
    m_board[currentIndex].m_editMode = 0;//set previous square to not selected
    m_currentIndex = -1;


}

void Board::displayBox(QColor color, int xCoord, int yCoord, int size)
{
    QGraphicsRectItem * rectangle;
    QBrush brush(color);
    QPen blackPen(Qt::black);
    addRect(xCoord, yCoord, size, size, blackPen, brush);

}

void Board::displayThickBorders()
{
    QGraphicsRectItem * rectangle;
    QBrush whiteBrush(Qt::white);
    QPen blackPen(Qt::black);
    int xCoord = 0;
    int yCoord = 0;

    blackPen.setWidth(3);
    for(int i=0; i<SUDOKU_SQUARE_CELLS_WIDTH; i++)
    {
        for(int j=0; j<SUDOKU_SQUARE_CELLS_WIDTH; j++)
        {
            rectangle = addRect(xCoord, yCoord, SUDOKU_SQUARE_CELLS_WIDTH*BLOCK_SIZE, SUDOKU_SQUARE_CELLS_WIDTH*BLOCK_SIZE, blackPen);
            xCoord+=SUDOKU_SQUARE_CELLS_WIDTH*BLOCK_SIZE;
        }
        xCoord=0;
        yCoord+=SUDOKU_SQUARE_CELLS_WIDTH*BLOCK_SIZE;
    }

}

void Board::delay(int time)
{
    QTime dieTime= QTime::currentTime().addMSecs(time);
    while (QTime::currentTime() <dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}
Board::LightUpSquare::LightUpSquare(int index, QColor c)
{
    m_index = index;
    m_lightUpBrush = QBrush(c);
    this->setOpacity(0);
}

QRectF Board::LightUpSquare::boundingRect() const
{
    QRectF out(indexToXCoord(m_index), indexToYCoord(m_index), BLOCK_SIZE, BLOCK_SIZE);
    return out;
}

void Board::LightUpSquare::paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget)
{
    QRectF rec = this->boundingRect();
    painter->fillRect(rec, m_lightUpBrush);


}


void Board::lightSquare(int index, QColor color)
{
    int fadeTime=500;
    LightUpSquare *square = new LightUpSquare(index, color);
    addItem(square);
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    square->setGraphicsEffect(eff);
    QPropertyAnimation *fadeIn = new QPropertyAnimation(square,"opacity");
    fadeIn->setDuration(fadeTime);
    fadeIn->setStartValue(0);
    fadeIn->setEndValue(0.5);
    fadeIn->setEasingCurve(QEasingCurve::InBack);
    fadeIn->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *fadeOut = new QPropertyAnimation(square,"opacity");
    fadeOut->setDuration(fadeTime);
    fadeOut->setStartValue(0.5);
    fadeOut->setEndValue(0);
    fadeOut->setEasingCurve(QEasingCurve::InBack);
    fadeOut->start(QPropertyAnimation::DeleteWhenStopped);

}

void Board::iterateHeapSolution(std::stack<std::pair<int, int> > &choices, std::pair<int, int> &currentPair)
{
    int currentBoardIndex = currentPair.first;
    int number = currentPair.second;
    if(number>=BOARD_WIDTH)//exhausted all possiblilites for this square for this path, go back one guess
    {
        std::pair<int, int> heapPair(m_board[currentBoardIndex].m_numOptions, currentBoardIndex);
        push(heapPair);
        currentPair = choices.top();
        choices.pop();

        currentBoardIndex = currentPair.first;
        number = currentPair.second;

        if(number!=-1)
        {
            deleteEntry(currentPair.first);
            lightSquare(currentPair.first, Qt::red);
            delay(100);
            updateHeapDeletedEntry(currentBoardIndex, number);
        }
        currentPair.second++;

    }
    else if(displayNumber(number, currentBoardIndex))//the placement was succesful
    {
        choices.push(currentPair);
        displayEntry(currentBoardIndex);
        lightSquare(currentPair.first, Qt::green);
        delay(100);
        if(solveOrder.size==0)
        {
            return;
        }
        int nextIndex = heapTop().second;
        heapPop();
        currentPair.first = nextIndex;
        currentPair.second = 0;

    }
    else//placement failed try next option
    {
        currentPair.second++;
    }

}

int Board::indexToXCoord(int index)
{
    return (index%(BOARD_WIDTH))*BLOCK_SIZE;
}

int Board::indexToYCoord(int index)
{
    return (index/(BOARD_WIDTH))*BLOCK_SIZE;
}

int Board::indexToRow(int index)
{
    return (index/(BOARD_WIDTH));
}

int Board::indexToCol(int index)
{
    return (index%(BOARD_WIDTH));
}

int Board::indexToSquare(int index)
{
    return (index/(SUDOKU_SQUARE_CELLS_WIDTH)%SUDOKU_SQUARE_CELLS_WIDTH)+index/(BOARD_WIDTH*SUDOKU_SQUARE_CELLS_WIDTH)*SUDOKU_SQUARE_CELLS_WIDTH;
}

int Board::coordToIndex(int xCoord, int yCoord)
{
    int out = (yCoord/BLOCK_SIZE)*(BOARD_WIDTH) + (xCoord/BLOCK_SIZE);
    int maxXCoord = BOARD_WIDTH*BLOCK_SIZE;
    int maxYCoord = BOARD_WIDTH*BLOCK_SIZE;

    if(xCoord<=0 || yCoord<=0 || xCoord >=maxXCoord || yCoord>=maxYCoord)
    {
        return -1;
    }
    return out;
}

std::vector<int> Board::randVector(int count)
{
    std::vector<int> out;
    out.resize(count);
    for(int i=0; i<count; i++)
    {
        out[i]=i;
    }
    for(int i=0; i<count; i++)
    {
        int swapIndex = std::rand()%count;
        std::swap(out[i], out[swapIndex]);
    }
    return out;
}

void Board::updateHeapAddedEntry(int index, int number)
{
    //update the options number on the squares
    if(index==-1)
    {
        return;
    }
    int indexSquareStart = (indexToSquare(index)/SUDOKU_SQUARE_CELLS_WIDTH)*(BOARD_WIDTH*SUDOKU_SQUARE_CELLS_WIDTH)+(indexToSquare(index)%SUDOKU_SQUARE_CELLS_WIDTH)*SUDOKU_SQUARE_CELLS_WIDTH;
    int indexSquareEnd = indexSquareStart + BOARD_WIDTH*(SUDOKU_SQUARE_CELLS_WIDTH);

    for(int i = indexSquareStart; i<indexSquareEnd; i+=BOARD_WIDTH)
    {
        for(int j=i; j<i+SUDOKU_SQUARE_CELLS_WIDTH; j++)
        {
            if(m_board[j].m_number==-1)
            {
                int heapIndex = m_board[j].m_heapIndex;
                bubbleUp(heapIndex);
            }

        }
    }

    //update the options number on the columns
    for(int i = indexToCol(index); i<BOARD_SQUARES; i+=BOARD_WIDTH)
    {
        if(m_board[i].m_number==-1)
        {
            int heapIndex = m_board[i].m_heapIndex;
            bubbleUp(heapIndex);
        }
    }
    //update the options number on the rows
    for(int i = indexToRow(index)*BOARD_WIDTH; i<(indexToRow(index)+1)*BOARD_WIDTH; i++)
    {
        if(m_board[i].m_number==-1)
        {
            int heapIndex = m_board[i].m_heapIndex;
            bubbleUp(heapIndex);
        }

    }


}

void Board::updateHeapDeletedEntry(int index, int number)
{
    if(index==-1)
    {
        return;
    }
    //update the options number on the squares
    int indexSquareStart = (indexToSquare(index)/SUDOKU_SQUARE_CELLS_WIDTH)*(BOARD_WIDTH*SUDOKU_SQUARE_CELLS_WIDTH)+(indexToSquare(index)%SUDOKU_SQUARE_CELLS_WIDTH)*SUDOKU_SQUARE_CELLS_WIDTH;
    int indexSquareEnd = indexSquareStart + BOARD_WIDTH*(SUDOKU_SQUARE_CELLS_WIDTH);

    for(int i = indexSquareStart; i<indexSquareEnd; i+=BOARD_WIDTH)
    {
        for(int j=i; j<i+SUDOKU_SQUARE_CELLS_WIDTH; j++)
        {
            if(m_colCounts[indexToCol(j)][number]==-1 && m_rowCounts[indexToRow(j)][number]==-1 && j!=index)
            {
                if(m_board[j].m_number==-1)
                {
                    int heapIndex = m_board[j].m_heapIndex;
                    bubbleUp(heapIndex);
                }
            }
        }
    }

    //update the options number on the columns
    for(int i = indexToCol(index); i<BOARD_SQUARES; i+=BOARD_WIDTH)
    {
        if(m_squareCounts[indexToSquare(i)][number]==-1 && m_rowCounts[indexToRow(i)][number]==-1 && indexToSquare(i)!=indexToSquare(index) && i!=index)
        {
            if(m_board[i].m_number==-1)
            {
                int heapIndex = m_board[i].m_heapIndex;
                bubbleUp(heapIndex);
            }
        }
    }
    //update the options number on the rows
    for(int i = indexToRow(index)*BOARD_WIDTH; i<(indexToRow(index)+1)*BOARD_WIDTH; i++)
    {
        if(m_squareCounts[indexToSquare(i)][number]==-1 && m_colCounts[indexToCol(i)][number]==-1 && indexToSquare(i)!=indexToSquare(index) && i!=index)
        {
            if(m_board[i].m_number==-1)
            {
                int heapIndex = m_board[i].m_heapIndex;
                bubbleUp(heapIndex);
            }
        }
    }

}

void Board::addNumberAnimation(int index, int number, QSequentialAnimationGroup *animateOrder)
{
    int fadeTime=100;
    LightUpSquare *square = new LightUpSquare(index, Qt::green);
    addItem(square);
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    square->setGraphicsEffect(eff);
    QPropertyAnimation *fadeIn = new QPropertyAnimation(square,"opacity");
    fadeIn->setDuration(fadeTime);
    fadeIn->setStartValue(0);
    fadeIn->setEndValue(0.5);
    fadeIn->setEasingCurve(QEasingCurve::InBack);
    animateOrder->addAnimation(fadeIn);

    int xCoord = indexToXCoord(index);
    int yCoord = indexToYCoord(index);
    Entry currentEntry = m_board[index];
    QGraphicsTextItem * io = new QGraphicsTextItem;
    QGraphicsRectItem * rectangle;
    QBrush whiteBrush(Qt::white);
    QPen blackPen(Qt::black);
    blackPen.setWidth(1);

    QFont addFont("Helvetica [Cronyx]", 18, 3, false);
    io->setPos(xCoord+BLOCK_SIZE/3, yCoord+BLOCK_SIZE/8);
    io->setOpacity(0);
    io->setPlainText(QString::number(currentEntry.m_number+1));
    io->setFont(addFont);
    addItem(io);
    io->setGraphicsEffect(eff);
    QPropertyAnimation *addText = new QPropertyAnimation(io,"opacity");
    addText->setDuration(10);
    addText->setStartValue(1);
    addText->setEndValue(1);
    //addText->setEasingCurve(QEasingCurve::InBack);
    animateOrder->addAnimation(addText);

    QPropertyAnimation *fadeOut = new QPropertyAnimation(square,"opacity");
    fadeOut->setDuration(fadeTime);
    fadeOut->setStartValue(0.5);
    fadeOut->setEndValue(0);
    fadeOut->setEasingCurve(QEasingCurve::InBack);
    animateOrder->addAnimation(fadeOut);

}

void Board::deleteNumberAnimation(int index, int number, QSequentialAnimationGroup *animateOrder)
{
    int fadeTime=100;
    LightUpSquare *square = new LightUpSquare(index, Qt::green);
    QBrush whiteBrush(Qt::white);
    QPen blackPen(Qt::black);
    blackPen.setWidth(1);
    int xCoord = indexToXCoord(index);
    int yCoord = indexToYCoord(index);

    addItem(square);
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    square->setGraphicsEffect(eff);
    QPropertyAnimation *fadeIn = new QPropertyAnimation(square,"opacity");
    fadeIn->setDuration(fadeTime);
    fadeIn->setStartValue(0);
    fadeIn->setEndValue(0.5);
    fadeIn->setEasingCurve(QEasingCurve::InBack);
    animateOrder->addAnimation(fadeIn);



    QPropertyAnimation *fadeOut = new QPropertyAnimation(square,"opacity");
    fadeOut->setDuration(fadeTime);
    fadeOut->setStartValue(0.5);
    fadeOut->setEndValue(0);
    fadeOut->setEasingCurve(QEasingCurve::InBack);
    animateOrder->addAnimation(fadeOut);

}

void Board::changeNumberAnimation(int index, int number, QSequentialAnimationGroup *animateOrder)
{
    int fadeTime=100;
    LightUpSquare *square = new LightUpSquare(index, Qt::green);
    addItem(square);
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    square->setGraphicsEffect(eff);
    QPropertyAnimation *fadeIn = new QPropertyAnimation(square,"opacity");
    fadeIn->setDuration(fadeTime);
    fadeIn->setStartValue(0);
    fadeIn->setEndValue(0.5);
    fadeIn->setEasingCurve(QEasingCurve::InBack);
    animateOrder->addAnimation(fadeIn);

    int xCoord = indexToXCoord(index);
    int yCoord = indexToYCoord(index);
    Entry currentEntry = m_board[index];
    QGraphicsTextItem * io = new QGraphicsTextItem;
    QGraphicsRectItem * rectangle;
    QBrush whiteBrush(Qt::white);
    QPen blackPen(Qt::black);
    blackPen.setWidth(1);

    QFont addFont("Helvetica [Cronyx]", 18, 3, false);
    io->setPos(xCoord+BLOCK_SIZE/3, yCoord+BLOCK_SIZE/8);
    io->setOpacity(0);
    io->setPlainText(QString::number(currentEntry.m_number+1));
    io->setFont(addFont);
    addItem(io);
    io->setGraphicsEffect(eff);
    QPropertyAnimation *addText = new QPropertyAnimation(io,"opacity");
    addText->setDuration(10);
    addText->setStartValue(1);
    addText->setEndValue(1);
    //addText->setEasingCurve(QEasingCurve::InBack);
    animateOrder->addAnimation(addText);

    QPropertyAnimation *fadeOut = new QPropertyAnimation(square,"opacity");
    fadeOut->setDuration(fadeTime);
    fadeOut->setStartValue(0.5);
    fadeOut->setEndValue(0);
    fadeOut->setEasingCurve(QEasingCurve::InBack);
    animateOrder->addAnimation(fadeOut);

}


Board::Entry::Entry()
{
    m_index = -1;
    m_number =-1;
    m_guesses.resize(4);
    m_guesses[0] = -1;
    m_guesses[1] = -1;
    m_guesses[2] = -1;
    m_guesses[3] = -1;
    m_guessCorner = -1;
    m_editMode = 0;
    m_numOptions=9;
    m_isConfirmed = false;
    m_heapIndex = 0;

}

void Board::deleteEntry(int index)
{
    QGraphicsRectItem * rectangle;
    QBrush whiteBrush(Qt::white);
    QPen blackPen(Qt::black);
    int numberRemoved = m_board[index].m_number;

    int xCoord = indexToXCoord(index);
    int yCoord = indexToYCoord(index);
    blackPen.setWidth(1);

    if(numberRemoved<0 ||numberRemoved>=BOARD_WIDTH)//indicates that a delete is being used on an empty square
    {
        m_board[index].m_guesses[0] = -1;
        m_board[index].m_guesses[1] = -1;
        m_board[index].m_guesses[2] = -1;
        m_board[index].m_guesses[3] = -1;
        m_board[index].m_editMode = 0;

        addRect(xCoord, yCoord, BLOCK_SIZE, BLOCK_SIZE, blackPen, whiteBrush);
        displayThickBorders();
        return;

    }

    //update the options number on the squares
    int indexSquareStart = (indexToSquare(index)/SUDOKU_SQUARE_CELLS_WIDTH)*(BOARD_WIDTH*SUDOKU_SQUARE_CELLS_WIDTH)+(indexToSquare(index)%SUDOKU_SQUARE_CELLS_WIDTH)*SUDOKU_SQUARE_CELLS_WIDTH;
    int indexSquareEnd = indexSquareStart + BOARD_WIDTH*(SUDOKU_SQUARE_CELLS_WIDTH);

    for(int i = indexSquareStart; i<indexSquareEnd; i+=BOARD_WIDTH)
    {
        for(int j=i; j<i+SUDOKU_SQUARE_CELLS_WIDTH; j++)
        {
            if(m_colCounts[indexToCol(j)][numberRemoved]==-1 && m_rowCounts[indexToRow(j)][numberRemoved]==-1 && j!=index)
            {
                m_board[j].m_numOptions++;
                int heapIndex = m_board[j].m_heapIndex;
                solveOrder.squaresToChoose[heapIndex].first++;
            }
            else if(indexToCol(j)==indexToCol(index) && m_rowCounts[indexToRow(j)][numberRemoved]==-1 && j!=index)
            {
                m_board[j].m_numOptions++;
                int heapIndex = m_board[j].m_heapIndex;
                solveOrder.squaresToChoose[heapIndex].first++;
            }
            else if(indexToRow(j)==indexToRow(index) && m_colCounts[indexToCol(j)][numberRemoved]==-1 && j!=index)
            {
                m_board[j].m_numOptions++;
                int heapIndex = m_board[j].m_heapIndex;
                solveOrder.squaresToChoose[heapIndex].first++;
            }
        }
    }

    //update the options number on the columns
    for(int i = indexToCol(index); i<BOARD_SQUARES; i+=BOARD_WIDTH)
    {
        if(m_squareCounts[indexToSquare(i)][numberRemoved]==-1 && m_rowCounts[indexToRow(i)][numberRemoved]==-1 && indexToSquare(i)!=indexToSquare(index) && i!=index)
        {
            m_board[i].m_numOptions++;
            int heapIndex = m_board[i].m_heapIndex;
            solveOrder.squaresToChoose[heapIndex].first++;
        }
    }
    //update the options number on the rows
    for(int i = indexToRow(index)*BOARD_WIDTH; i<(indexToRow(index)+1)*BOARD_WIDTH; i++)
    {
        if(m_squareCounts[indexToSquare(i)][numberRemoved]==-1 && m_colCounts[indexToCol(i)][numberRemoved]==-1 && indexToSquare(i)!=indexToSquare(index) && i!=index)
        {
            m_board[i].m_numOptions++;
            int heapIndex = m_board[i].m_heapIndex;
            solveOrder.squaresToChoose[heapIndex].first++;
        }
    }



    m_squareCounts[indexToSquare(index)][numberRemoved] = -1;
    m_rowCounts[indexToRow(index)][numberRemoved] =-1;
    m_colCounts[indexToCol(index)][numberRemoved] =-1;
    m_board[index].m_number = -1;
    m_board[index].m_editMode = 0;

    addRect(xCoord, yCoord, BLOCK_SIZE, BLOCK_SIZE, blackPen, whiteBrush);
    displayThickBorders();

}


void Board::initializeHeap()
{
    solveOrder.capacity = BOARD_SQUARES - m_confirmedSquares;
    solveOrder.size = BOARD_SQUARES - m_confirmedSquares;
    solveOrder.squaresToChoose.resize(solveOrder.capacity);
    int currentIndex = 0;
    for(int i=0; i<BOARD_SQUARES; i++)
    {
        if(!m_board[i].m_isConfirmed)
        {
            std::pair<int, int> addToSquares(m_board[i].m_numOptions, i);
            m_board[i].m_heapIndex = currentIndex;
            solveOrder.squaresToChoose[currentIndex] = addToSquares;
            currentIndex++;
        }
    }
    heapify();
}

void Board::displayAllEntries()
{
    for(int i=0; i<BOARD_SQUARES; i++)
    {
        displayEntry(i);
    }
}

void Board::resetLevel()
{
    for(int i=0; i<BOARD_SQUARES; i++)
    {
        if(m_board[i].m_isConfirmed==false)
        {
            deleteEntry(i);
        }
    }
}

void Board::generateRandomFinishedBoard()
{
    initializeHeap();
    linearSolve();
    for(int i=0; i<BOARD_SQUARES; i++)
    {
        m_solution[i] = m_board[i].m_number;
    }
    while(solveOrder.size>0)
    {
        heapPop();
    }

}

//generates board with a given difficulty
//precondition: starts with a complete board,
void Board::generateDifficulty(int minDifficulty, int maxDifficulty)
{
    generateRandomFinishedBoard();
    std::vector<int> fillorder = randVector((BOARD_SQUARES+1)/2);
    int minIndex = 0;
    int maxIndex = fillorder.size()-1;
    int middleIndex = (minIndex+maxIndex)/2;// as middleindex increases game is harder
    int difficulty = -1;
    while((difficulty<minDifficulty || difficulty>maxDifficulty) && maxIndex>minIndex)
    {
        for(int i=0; i<=middleIndex; i++)
        {
            int deleteIndex = fillorder[i];
            deleteEntry(deleteIndex);
            std::pair<int, int> add(m_board[deleteIndex].m_numOptions,deleteIndex);
            push(add);
            int mirrorIndex = BOARD_SQUARES-1-fillorder[i];//maintains evenness of placement of numbers
            if(mirrorIndex!= deleteIndex)
            {
                deleteEntry(mirrorIndex);
                std::pair<int, int> add(m_board[mirrorIndex].m_numOptions,mirrorIndex);
                push(add);
            }
        }
        heapify();
        difficulty = measureDifficulty();
        if(difficulty < minDifficulty)
        {
            minIndex = middleIndex+1;
            middleIndex = (minIndex+maxIndex)/2;
        }
        else if(difficulty > maxDifficulty)
        {
            maxIndex = middleIndex-1;
            middleIndex = (minIndex+maxIndex)/2;
        }
    }
    for(int i=0; i<=middleIndex; i++)
    {
        int deleteIndex = fillorder[i];
        deleteEntry(deleteIndex);
        std::pair<int, int> add(m_board[deleteIndex].m_numOptions,deleteIndex);
        push(add);
        int mirrorIndex = BOARD_SQUARES-1-fillorder[i];//maintains evenness of placement of numbers
        if(mirrorIndex!= deleteIndex)
        {
            deleteEntry(mirrorIndex);
            std::pair<int, int> add(m_board[mirrorIndex].m_numOptions,mirrorIndex);
            push(add);
        }
    }
    heapify();
    m_confirmedSquares = 0;
    for(int i=0; i<BOARD_SQUARES; i++)
    {
        if(m_board[i].m_number!=-1)
        {
            m_board[i].m_isConfirmed = true;
            m_confirmedSquares++;
        }
    }
    displayAllEntries();
}


//measures the difficulty of a given unsolved sudoku board
// uses metric given here https://dlbeer.co.nz/articles/sudoku.html
//Precondition: a board that with empty squares but a filled m_solution vector
//Poscondition: a finished board, the sovle order heap is empty and the difficulty is returned
int Board::measureDifficulty()
{
    int numEmptySquares = BOARD_SQUARES - solveOrder.size;
    int branchDifficulty = 0;
    while(solveOrder.size>0)
    {
        int numBranches = heapTop().first;
        int nextIndex = heapTop().second;
        heapPop();
        branchDifficulty+=((numBranches-1)*(numBranches-1));
        displayNumber(m_solution[nextIndex], nextIndex);
        updateHeapAddedEntry(nextIndex, m_solution[nextIndex]);
    }
    return (numEmptySquares + 10*branchDifficulty);
}

int Board::right(const int index)
{
    return 2*index+2;
}

int Board::left(const int index)
{
    return 2*index+1;
}

int Board::parent(const int index)
{
    return (index-1)/2;
}

void Board::push(std::pair<int, int> add)
{
    if(solveOrder.size==solveOrder.capacity)
    {
        return;
    }
    solveOrder.size++;
    int heapIndex = solveOrder.size-1;
    solveOrder.squaresToChoose[heapIndex] = add;
    int index = add.second;
    m_board[index].m_heapIndex = heapIndex;
    bubbleUp(heapIndex);
}

std::pair<int, int> Board::heapTop()
{
    if(solveOrder.size<=0)
    {
        std::pair<int, int> out(-1, -1);
        return out;
    }
    std::pair<int, int> out = solveOrder.squaresToChoose[0];
    return out;
}

std::pair<int, int> Board::heapPop()
{
    if(solveOrder.size<=0)
    {
        std::pair<int, int> out(-1, -1);
        return out;
    }
    if(solveOrder.size ==1)
    {
        solveOrder.size--;
        return solveOrder.squaresToChoose[0];
    }
    std::pair<int, int> out = solveOrder.squaresToChoose[0];
    heapSwap(0, solveOrder.size-1);
    solveOrder.size--;
    bubbleDown(0);
    return out;

}

void Board::deleteHeapIndex(int index)
{
    std::pair<int, int> out = solveOrder.squaresToChoose[0];
    heapSwap(0, solveOrder.size-1);
    solveOrder.size--;
    bubbleDown(index);
}

void Board::bubbleDown(int index)
{
    int leftIndex = left(index);
    int rightIndex = right(index);
    int child = index;
    if(leftIndex < solveOrder.size && solveOrder.squaresToChoose[leftIndex] < solveOrder.squaresToChoose[index])
    {
        child = leftIndex;
    }
    if(rightIndex < solveOrder.size && solveOrder.squaresToChoose[rightIndex] < solveOrder.squaresToChoose[child])
    {
        child = rightIndex;
    }
    if(child!=index)
    {
        heapSwap(index, child);
        bubbleDown(child);
    }
}

void Board::bubbleUp(int index)// cal on index in heap
{
    while(index!=0 && solveOrder.squaresToChoose[parent(index)]>solveOrder.squaresToChoose[index] )
    {
        heapSwap(parent(index), index);
        index=parent(index);
    }
}

void Board::heapSwap(int parent, int child)
{
    int parentIndex = solveOrder.squaresToChoose[parent].second;
    int childIndex = solveOrder.squaresToChoose[child].second;
    swap(solveOrder.squaresToChoose[parent], solveOrder.squaresToChoose[child]);
    int temp = m_board[parentIndex].m_heapIndex;
    m_board[parentIndex].m_heapIndex = m_board[childIndex].m_heapIndex;
    m_board[childIndex].m_heapIndex = temp;
}

void Board::heapify()
{

    for(int i=solveOrder.size; i>=0; i--)
    {
        bubbleDown(i);
    }
}
