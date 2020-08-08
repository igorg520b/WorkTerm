#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QSpinBox>
#include <QTableView>

#include <QtCharts>
#include <QVTKOpenGLNativeWidget.h>

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkDataSetMapper.h>
#include <vtkCamera.h>
#include <vtkProp3DCollection.h>
#include <vtkNamedColors.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QGridLayout *gridLayout;
    QVBoxLayout *vbox1;

    // toolbar
    QSlider *slider;                // in toolbar
    QSpinBox *spin;                 // in toolbar


    // plots
    QChartView *chartViewGraphs, *chartViewCircles;

    QLineSeries *series_tagnential, *series_normal, *series_selected;
    QLineSeries *series_mohr, *series_mohr_selected;

    QChart *chart_line, *chart_line_mohr;

    // vtk
    QVTKOpenGLNativeWidget *qt_vtk_widget;
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkNamedColors> colors;



    // info
    QTableView *table;
//    QLabel *lblNormalTraction, *lblTangentialTraction;
//    QLabel *lblFwdAngle, *lblBwdAngle, *lblSampleNumber;
//    QLabel *lblComputeTime, *lblEvaluations;



private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
