#include "dialog.h"
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    srand (time(NULL));
    ui->setupUi(this);
    int heapTime = 0;
    int linearTime = 0;
    m_currentBoard = new Board();
    ui->graphicsView->setScene(m_currentBoard);
    m_currentBoard->displayBoard();

}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_Easy_Game_clicked()
{
    delete m_currentBoard;
    m_currentBoard = new Board();
    ui->graphicsView->setScene(m_currentBoard);
    m_currentBoard->generateDifficulty(300, 600);
}

void Dialog::on_Medium_Game_clicked()
{
    delete m_currentBoard;
    m_currentBoard = new Board();
    ui->graphicsView->setScene(m_currentBoard);
    m_currentBoard->generateDifficulty(600, 900);
}

void Dialog::on_Hard_Game_clicked()
{
    delete m_currentBoard;
    m_currentBoard = new Board();
    ui->graphicsView->setScene(m_currentBoard);
    m_currentBoard->generateDifficulty(900, 1200);
}
