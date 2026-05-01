//
// Created by valleron on 4/23/26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_main_window.h" resolved

#include "main_window.h"
#include "ui_main_window.h"
#include "qcustomplot.h"
#include "../calculations.cpp"



main_window::main_window(QWidget *parent) : QMainWindow(parent), ui(new Ui::main_window) {
    ui->setupUi(this);

    ui->canvas->addGraph(); // linia wielomianu
    ui->canvas->addGraph(); // punkty kontrolne
    ui->canvas->addGraph(); // zadana funkcja

    ui->canvas->graph(0)->setPen(QPen(Qt::blue));

    ui->canvas->graph(1)->setLineStyle(QCPGraph::lsNone); // Brak linii dla punktów
    ui->canvas->graph(1)->setScatterStyle(QCPScatterStyle::ssCircle);
    //connect(model, &PointsModel::dataChanged, this, &MainWindow::updatePlot);
    start = -2 * M_PI + 1;//0
    end = 3 * M_PI +1;//2
    step = (end - start) / 499;
    minX = start;
    maxX = end;
    minY = start;
    maxY = start;
    graphs = QVector<std::tuple<QVector<double>,QVector<double>>>();

    QVector<double> xGraph, yGraph;
    for (int i=0;i<500;i++) {
        xGraph << start + (i * step);
        yGraph << givenFunction(xGraph.last());
        minX = std::min(minX, xGraph.last());
        maxX = std::max(maxX, xGraph.last());
        minY = std::min(minY, yGraph.last());
        maxY = std::max(maxY, yGraph.last());
    }
    graphs.push_back(std::tuple<QVector<double>,QVector<double>>(xGraph, yGraph));
    ui->canvas->xAxis->setRange(minX - 1, maxX + 1);
    ui->canvas->yAxis->setRange(minY - 2, maxY + 2);
    ui->canvas->graph(2)->setPen(QPen(Qt::red));
    ui->canvas->graph(0)->setData(xGraph, yGraph);
    ui->canvas->legend->setVisible(true);

    ui->canvas->legend->setFont(QFont("Helvetica", 9));
    ui->canvas->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignBottom);


    ui->canvas->graph(0)->setName("Funkcja bazowa");
    ui->canvas->graph(1)->setName("Węzły");
    ui->canvas->graph(2)->setName("Interpolacja");
    ui->canvas->replot();
}

main_window::~main_window() {
    delete ui;
}

void main_window::on_drawButton_clicked() {
    int n=ui->nSpinBox->value(), m=ui->mSpinBox->value();

    if (ui->polynomialRadio->isChecked()) {
        const std::vector<double> chebyshevNodes = generateNodes(n, minX, maxX);
        std::vector<double> values = valuesInNodes(&chebyshevNodes);
        std::vector<double> coefficients = calculateCoefficients(m, values);

       QVector<double> xNodes, yNodes;
        for (double chebyshev_node: chebyshevNodes) {
            xNodes << chebyshev_node;
            yNodes << givenFunction(chebyshev_node);
        }
       ui->canvas->graph(1)->setData(xNodes, yNodes);

        QVector<double> xGraph, yGraph;
       for (int i=0; i<500; i++) {
           xGraph << start + (i * step);
           yGraph << getValue(xGraph.last(), coefficients, minX, maxX);
       }
        ui->canvas->graph(2)->setData(xGraph, yGraph);
        ui->canvas->replot();
        ui->canvas->saveJpg("wykres.png");
    } else if (ui->trigRadio->isChecked()){
        const std::vector<double> nodes = generateEvenNodes(n, start, end);
        const std::vector<double> values = valuesInNodes(&nodes);

        const std::vector<double> sineCoefficients = calculateSineCoefficients(m, values, nodes, start, end);
        const std::vector<double> cosineCoefficients = calculateCosineCoefficients(m, values, nodes, start, end);

        QVector<double> xNodes, yNodes;
        for (double chebyshev_node: nodes) {
            xNodes << chebyshev_node;
            yNodes << givenFunction(chebyshev_node);
        }
        ui->canvas->graph(1)->setData(xNodes, yNodes);

        QVector<double> xGraph, yGraph;
        double cur_maxY=maxY, cur_minY=minY;
        for (int i=0; i<500; i++) {
            xGraph << start + (i * step);
            yGraph << getTrigValue(xGraph.last(), sineCoefficients, cosineCoefficients, start, end);
            cur_maxY=std::max(cur_maxY, yGraph.last());
            cur_minY=std::min(cur_minY, minY);
        }
        ui->canvas->graph(2)->setData(xGraph, yGraph);
        ui->canvas->yAxis->setRange(cur_minY - 2, cur_maxY + 2);
        ui->canvas->replot();
        ui->canvas->saveJpg("wykres.png");
    }
}

void main_window::on_addDrawingButton_clicked() {
    ui->canvas->addGraph();
}
