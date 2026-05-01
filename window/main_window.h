//
// Created by valleron on 4/23/26.
//

#ifndef MOWNIT_APPROX_MAIN_WINDOW_H
#define MOWNIT_APPROX_MAIN_WINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE

namespace Ui {
    class main_window;
}

QT_END_NAMESPACE

class main_window : public QMainWindow {
    Q_OBJECT

public:
    explicit main_window(QWidget *parent = nullptr);

    ~main_window() override;

private:
    Ui::main_window *ui;
    double start;
    double end;
    double step;
    double maxX;
    double minX;
    double maxY;
    double minY;
    QVector<std::tuple<QVector<double>, QVector<double>>> graphs;

private slots:
    void on_drawButton_clicked();
    void on_addDrawingButton_clicked();
    //void on_hideDrawingButton_clicked();
};





#endif //MOWNIT_APPROX_MAIN_WINDOW_H