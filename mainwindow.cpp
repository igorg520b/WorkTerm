#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    gridLayout = new QGridLayout;
    vbox1 = new QVBoxLayout;


    // charts
    chartViewGraphs = new QChartView;
    chartViewCircles = new QChartView;

    series_tagnential = new QLineSeries;
    series_normal = new QLineSeries;
    series_selected = new QLineSeries;
    series_mohr = new QLineSeries;
    series_mohr_selected = new QLineSeries;

    chart_line = new QChart;
    chart_line_mohr = new QChart;

    chartViewGraphs->setChart(chart_line);
    chartViewCircles->setChart(chart_line_mohr);

    // vtk
    qt_vtk_widget = new QVTKOpenGLNativeWidget();
    qt_vtk_widget->setRenderWindow(renderWindow);
    renderer->SetBackground(colors->GetColor3d("White").GetData());
    renderWindow->AddRenderer(renderer);

    // summary
    table = new QTableView;

    // toopbar
    slider = new QSlider(Qt::Horizontal);
    ui->toolBar->addWidget(slider);

    spin = new QSpinBox();
    spin->setRange(0,10000);
    slider->setRange(0,10000);
    ui->toolBar->addWidget(spin);

    QWidget *w = new QWidget();
    w->setLayout(gridLayout);
    setCentralWidget(w);

//    gridLayout->addItem(chartViewGraphs)
    gridLayout->addWidget(chartViewGraphs, 0, 0);
    gridLayout->addWidget(chartViewCircles, 0, 1);
    gridLayout->addWidget(qt_vtk_widget, 1, 0);
    gridLayout->addWidget(table, 1, 1);


}

MainWindow::~MainWindow()
{
    delete ui;
}

