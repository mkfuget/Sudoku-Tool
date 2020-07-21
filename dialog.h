#ifndef DIALOG_H
#define DIALOG_H
#include "ui_dialog.h"
#include "board.h"
#include <chrono>
#include <iostream>

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();
private:
    Board* m_currentBoard;
private slots:
    void on_Easy_Game_clicked();

    void on_Medium_Game_clicked();

    void on_Hard_Game_clicked();

private:
    Ui::Dialog *ui;
};
#endif // DIALOG_H
