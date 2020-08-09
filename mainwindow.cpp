#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model.isBoundary = false;
    model.AddSector(-0.0804524,-0.126599,7.1294e-06,0.0694121,-0.132974,6.35204e-06,4.62408,1.44387,-0.0311261);
    model.AddSector(0.0694121,-0.132974,6.35204e-06,0.149865,-0.0063741,1.42622e-05,3.5138,1.1411,-0.21346);
    model.AddSector(0.149865,-0.0063741,1.42622e-05,0.0804524,0.126599,7.43639e-06,3.55001,1.10216,0.218794);
    model.AddSector(0.0804524,0.126599,7.43639e-06,-0.0694121,0.132974,6.11511e-06,4.62,1.46422,-0.0410333);
    model.AddSector(-0.0694121,0.132974,6.11511e-06,-0.149865,0.00637403,1.39815e-05,3.58378,1.18685,-0.207684);
    model.AddSector(-0.149865,0.00637403,1.39815e-05,-0.0804524,-0.126599,7.1294e-06,3.60604,1.10554,0.212817);
    model.Evaluate();
    selectedModel = &model;

    gridLayout = new QGridLayout;
    //vbox1 = new QVBoxLayout;

    // charts
    chartViewGraphs = new QChartView;
    chartViewCircles = new QChartView;

    series_tangential = new QLineSeries;
    series_tangential->setName("tangential");

    series_normal = new QLineSeries;
    series_normal->setName("normal");

    series_selected = new QScatterSeries;

    series_mohr = new QLineSeries;
    series_mohr->setName("normal vs tangential traction");

    series_mohr_selected = new QScatterSeries;

    chart_line = new QChart;
    chart_line->addSeries(series_tangential);
    chart_line->addSeries(series_normal);
    chart_line->addSeries(series_selected);

    chart_line_mohr = new QChart;
    chart_line_mohr->addSeries(series_mohr);
    chart_line_mohr->addSeries(series_mohr_selected);

    chartViewGraphs->setChart(chart_line);
    chartViewCircles->setChart(chart_line_mohr);

    chart_line_mohr->createDefaultAxes();
    chart_line->createDefaultAxes();


    // vtk
    qt_vtk_widget = new QVTKOpenGLNativeWidget();
    qt_vtk_widget->setRenderWindow(renderWindow);
    renderer->SetBackground(colors->GetColor3d("White").GetData());
    renderer->AddActor(actor_mesh);
    renderer->AddActor(actor_arrows);
    renderer->AddActor(actor_labels);
    renderWindow->AddRenderer(renderer);

    points_origin->InsertPoint(0, 0,0,0);

    // summary
    table = new QTableWidget;
    table->setRowCount(4);
    table->setColumnCount(2);

    QTableWidgetItem *row0 = new QTableWidgetItem("index");
    QTableWidgetItem *row1 = new QTableWidgetItem("angle");
    QTableWidgetItem *row2 = new QTableWidgetItem("normal traction");
    QTableWidgetItem *row3 = new QTableWidgetItem("tangential traction");

    twi_selectedIdx = new QTableWidgetItem;

    twi_angle = new QTableWidgetItem;
    twi_normal_traction = new QTableWidgetItem;
    twi_tangential_traction = new QTableWidgetItem;

    table->setItem(0,1, twi_selectedIdx);
    table->setItem(1,1, twi_angle);
    table->setItem(2,1, twi_normal_traction);
    table->setItem(3,1, twi_tangential_traction);
    table->setItem(0,0, row0);
    table->setItem(1,0, row1);
    table->setItem(2,0, row2);
    table->setItem(3,0, row3);


    // toopbar
    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0,Model::number_of_ponts-1);
    ui->toolBar->addWidget(slider);

//    spin = new QSpinBox();
//    spin->setRange(0,10000);
//    ui->toolBar->addWidget(spin);

    QWidget *w = new QWidget();
    w->setLayout(gridLayout);
    setCentralWidget(w);

//    gridLayout->addItem(chartViewGraphs)
    gridLayout->addWidget(chartViewGraphs, 0, 0);
    gridLayout->addWidget(chartViewCircles, 0, 1);
    gridLayout->addWidget(qt_vtk_widget, 1, 0);
    gridLayout->addWidget(table, 1, 1);

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent( QShowEvent*)
{
    // for testing
    UpdateGUI();
}

void MainWindow::sliderValueChanged(int)
{
    UpdateGUI();
}



void MainWindow::UpdateGUI()
{
    // TABLE
    int selected_idx = slider->value();
    Model::Result &res = selectedModel->results[selected_idx];

    QString strIdx = QString::number(selected_idx);
    twi_selectedIdx->setText(strIdx);

    twi_angle->setText(QString::number(res.angle_fwd*180/M_PI));
    twi_normal_traction->setText(QString::number(res.trac_normal));
    twi_tangential_traction->setText(QString::number(res.trac_tangential));

    // PLOTS
    double ymin = DBL_MAX, ymax = -DBL_MAX;
    double xmin = DBL_MAX, xmax = -DBL_MAX;
    series_mohr->clear();
    series_normal->clear();
    series_tangential->clear();


    for(unsigned i=0;i<Model::number_of_ponts;i++)
    {
        Model::Result &res = selectedModel->results[i];
        double val_x = res.trac_normal;
        double val_y = res.trac_tangential;

        xmin = std::min(xmin, val_x);
        xmax = std::max(xmax, val_x);
        ymin = std::min(ymin, val_y);
        ymax = std::max(ymax, val_y);
        series_mohr->append(val_x,val_y);
    }
    double span_y = ymax-ymin;
    double span_x = xmax-xmin;
    double span = std::max(span_y, span_x)*0.65;
    double avgx = (xmax+xmin)/2;

    QList<QAbstractAxis*> axes = chart_line_mohr->axes();
    axes[0]->setRange(avgx-span, avgx+span);
    axes[1]->setRange(-span, span);


    xmin = ymin = DBL_MAX;
    xmax = ymax = -DBL_MAX;
    for(unsigned i=0;i<Model::number_of_ponts;i++)
    {
        Model::Result &res = selectedModel->results[i];
        double val_x = res.angle_fwd;
        xmin = std::min(xmin, val_x);
        xmax = std::max(xmax, val_x);
        ymin = std::min(ymin, res.trac_tangential);
        ymax = std::max(ymax, res.trac_tangential);
        ymin = std::min(ymin, res.trac_normal);
        ymax = std::max(ymax, res.trac_normal);
        series_normal->append(val_x,res.trac_normal);
        series_tangential->append(val_x,res.trac_tangential);
    }
    span_y = ymax-ymin;
    span_x = xmax-xmin;
    span = std::max(span_y, span_x)*0.65;
    avgx = (xmax+xmin)/2;

    axes = chart_line->axes();
    axes[0]->setRange(0, selectedModel->max_angle);
    axes[1]->setRange(ymin-span_y*0.1, ymax+span_y*0.1);


    // plots - selections
    series_selected->clear();
    series_mohr_selected->clear();
    series_selected->append(res.angle_fwd, res.trac_normal);
    series_mohr_selected->append(res.trac_normal, res.trac_tangential);

    // VTK
    int nSectors = selectedModel->fan.size();

    points->SetNumberOfPoints(nSectors*3);
    vtkIdType pts2[3];
    ugrid->Reset();
    for(int i=0;i<nSectors;i++)
    {
        Model::Sector &s = selectedModel->fan[i];
        points->SetPoint(i*3+0, 0, 0, 0);
        points->SetPoint(i*3+1, s.u.x(), s.u.y(), s.u.z());
        points->SetPoint(i*3+2, s.v.x(), s.v.y(), s.v.z());

        pts2[0] = i*3+0;
        pts2[1] = i*3+1;
        pts2[2] = i*3+2;
        ugrid->InsertNextCell(VTK_TRIANGLE, 3, pts2);
    }

    points->Modified();
    ugrid->SetPoints(points);

    dataSetMapper->SetInputData(ugrid);
    actor_mesh->SetMapper(dataSetMapper);
    actor_mesh->GetProperty()->SetColor(colors->GetColor3d("Seashell").GetData());
    actor_mesh->GetProperty()->SetLineWidth(0.5);
    actor_mesh->GetProperty()->EdgeVisibilityOn();



    // sector numbers
    geometryFilter->SetInputData(ugrid);
    geometryFilter->Update();

    idfilter->SetInputConnection(geometryFilter->GetOutputPort());
    idfilter->PointIdsOff();
    idfilter->CellIdsOn();
    idfilter->FieldDataOff();

    cellCenters->SetInputConnection(idfilter->GetOutputPort());

    labledDataMapper->SetInputConnection(cellCenters->GetOutputPort());
    labledDataMapper->SetLabelModeToLabelScalars();

    actor_labels->SetMapper(labledDataMapper);
    actor_labels->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

    // arrow
    ugrid_arrow_origin->Reset();
    pts2[0] = 0;
    ugrid_arrow_origin->InsertNextCell(VTK_VERTEX, 1, pts2);
    ugrid_arrow_origin->SetPoints(points_origin);

    arrowCoords->Reset();
    arrowCoords->SetNumberOfComponents(3);
    arrowCoords->SetName("arrowCoords");
    arrowCoords->SetNumberOfTuples(1);
    arrowCoords->SetTuple(0, res.tn.data());
    arrowCoords->Modified();

    ugrid_arrow_origin->GetPointData()->AddArray(arrowCoords);
    ugrid_arrow_origin->GetPointData()->SetActiveVectors("arrowCoords");

    arrowSource->SetShaftRadius(0.02);
    arrowSource->SetTipRadius(0.03);
    arrowSource->SetTipLength(0.06);
    glyph3D->SetSourceConnection(arrowSource->GetOutputPort());
    glyph3D->SetVectorModeToUseVector();
    glyph3D->SetInputData(ugrid_arrow_origin);
    glyph3D->OrientOn();
    glyph3D->ScalingOn();
//    glyph3D->ScalingOff();
//    glyph3D->SetColorModeToColorByVector();
//    glyph3D->SetScaleModeToScaleByVector();
    glyph3D->SetScaleFactor(0.1);
    mapper_arrows->SetInputConnection(glyph3D->GetOutputPort());
    actor_arrows->SetMapper(mapper_arrows);

    renderWindow->Render();



}

