#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <cmath>
#include <Eigen/Geometry>
#include <iomanip>
#include <cfloat>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Model *model = new Model;
    model->isBoundary = false;
    model->AddSector(-0.0804524,-0.126599,7.1294e-06,0.0694121,-0.132974,6.35204e-06,4.62408,1.44387,-0.0311261);
    model->AddSector(0.0694121,-0.132974,6.35204e-06,0.149865,-0.0063741,1.42622e-05,3.5138,1.1411,-0.21346);
    model->AddSector(0.149865,-0.0063741,1.42622e-05,0.0804524,0.126599,7.43639e-06,3.55001,1.10216,0.218794);
    model->AddSector(0.0804524,0.126599,7.43639e-06,-0.0694121,0.132974,6.11511e-06,4.62,1.46422,-0.0410333);
    model->AddSector(-0.0694121,0.132974,6.11511e-06,-0.149865,0.00637403,1.39815e-05,3.58378,1.18685,-0.207684);
    model->AddSector(-0.149865,0.00637403,1.39815e-05,-0.0804524,-0.126599,7.1294e-06,3.60604,1.10554,0.212817);

    models.push_back(model);

    model = new Model;
    model->isBoundary = true;
    model->AddSector(-0.0867755,0.05456,0.000800575,-0.085338,-0.0808328,-0.00291115,18.8618,5.24858,0.604117);
    model->AddSector(-0.085338,-0.0808328,-0.00291115,0.0112856,-0.0864482,-0.00227359,10.1852,-0.517818,2.41207);
    model->AddSector(0.0112856,-0.0864482,-0.00227359,0.0867755,-0.05456,-0.000736006,1.68372,-1.84484,3.50632);

    models.push_back(model);

    model = new Model;
    model->isBoundary = true;
    model->AddSector(0.0871197,-0.0953093,-0.00147892,0.104189,0.00711211,-0.000556284,9.46015,1.12215,-2.48601);
    model->AddSector(0.104189,0.00711211,-0.000556284,0.0421382,0.0464292,0.000203371,3.49938,-0.69808,0.171079);

    models.push_back(model);

    model = new Model;
    model->isBoundary = true;
    model->AddSector(0.140231,0.0389385,2.62128e-06,0.0433272,0.11605,-4.62367e-06,0.55391,0.647449,-0.0864676);
    model->AddSector(0.0433272,0.11605,-4.62367e-06,-0.0928865,0.0981676,-6.09364e-06,1.26464,0.957015,-0.260301);
    model->AddSector(-0.0928865,0.0981676,-6.09364e-06,-0.140188,-0.0389793,2.08664e-06,1.50313,1.34882,-0.21918);

    models.push_back(model);

    model = new Model;
    model->isBoundary = false;
    model->AddSector(-0.121666,-0.0413276,7.31515e-06,-0.0404978,-0.137792,8.16867e-06,1.32592,1.34946,-0.305451);
    model->AddSector(-0.0404978,-0.137792,8.16867e-06,0.0832074,-0.100172,6.08423e-06,1.50142,1.34715,-0.218812);
    model->AddSector(0.0832074,-0.100172,6.08423e-06,0.114863,0.0179242,1.46605e-06,1.26295,0.95555,-0.260039);
    model->AddSector(0.114863,0.0179242,1.46605e-06,0.0806698,0.136044,-9.70057e-08,1.66774,1.73367,-0.514309);
    model->AddSector(0.0806698,0.136044,-9.70057e-08,-0.025141,0.132821,2.93461e-06,1.2932,1.63841,-0.433953);
    model->AddSector(-0.025141,0.132821,2.93461e-06,-0.103108,0.0621824,5.95843e-06,1.21137,1.41362,-0.326353);
    model->AddSector(-0.103108,0.0621824,5.95843e-06,-0.121666,-0.0413276,7.31515e-06,1.15086,1.24152,-0.317631);

    models.push_back(model);

    std::cout << std::setprecision(5);
    for(Model *m : models) {
        m->InitializeFan();
        m->EvaluateViaBrent();
        std::cout << "trac_n( " << m->fracture_angle << " ) = " << m->max_normal_trac <<
                     " ;iter = " << m->iterations << std::endl;
    }


    selectedModel = models[0];

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

    sector_limits = new QScatterSeries;
    sector_limits->setName("sector boundaries");
    sector_limits_mohr = new QScatterSeries;
    sector_limits_mohr->setName("sector boundaries");

    max_graph = new QScatterSeries;
    max_graph->setName("max traction");
    max_mohr = new QScatterSeries;
    max_mohr->setName("max traction");
    sector_limits->setMarkerSize(10);
    sector_limits_mohr->setMarkerSize(10);
    max_graph->setMarkerSize(20);
    max_mohr->setMarkerSize(20);

    series_mohr = new QLineSeries;
    series_mohr->setName("normal vs tangential traction");

    series_mohr_selected = new QScatterSeries;

    chart_line = new QChart;
    chart_line->addSeries(series_tangential);
    chart_line->addSeries(series_normal);
    chart_line->addSeries(max_graph);
    chart_line->addSeries(series_selected);
    chart_line->addSeries(sector_limits);

    chart_line_mohr = new QChart;
    chart_line_mohr->addSeries(series_mohr);
    chart_line_mohr->addSeries(max_mohr);
    chart_line_mohr->addSeries(series_mohr_selected);
    chart_line_mohr->addSeries(sector_limits_mohr);

    chartViewGraphs->setChart(chart_line);
    chartViewCircles->setChart(chart_line_mohr);

    chart_line_mohr->createDefaultAxes();
    chart_line->createDefaultAxes();


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
    slider->setRange(0,199);
    ui->toolBar->addWidget(slider);

    spin = new QSpinBox();
    spin->setRange(0,models.size()-1);
    ui->toolBar->addWidget(spin);

    QWidget *w = new QWidget();
    w->setLayout(gridLayout);
    setCentralWidget(w);
/*
    // vtk
    points_origin->InsertPoint(0, 0,0,0);
    qt_vtk_widget = new QVTKOpenGLNativeWidget();
    qt_vtk_widget->setRenderWindow(renderWindow);
    renderer->SetBackground(colors->GetColor3d("White").GetData());
    renderer->AddActor(actor_mesh);
    renderer->AddActor(actor_arrows);
    renderer->AddActor(actor_labels);
    renderWindow->AddRenderer(renderer);
    gridLayout->addWidget(qt_vtk_widget, 1, 0);
*/
    gridLayout->addWidget(chartViewGraphs, 0, 0);
    gridLayout->addWidget(chartViewCircles, 0, 1);
    gridLayout->addWidget(table, 1, 1);

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
    connect(spin, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged(int)));

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
    double angle = selectedModel->max_angle*selected_idx/199.0;
    double normal_trac = selectedModel->normal_traction(angle);
    double tangential_trac = selectedModel->tangential_traction(angle);


    QString strIdx = QString::number(selected_idx);
    twi_selectedIdx->setText(strIdx);

    twi_angle->setText(QString::number(angle));
    twi_normal_traction->setText(QString::number(normal_trac));
    twi_tangential_traction->setText(QString::number(tangential_trac));

    // PLOTS
    series_mohr->clear();
    series_normal->clear();
    series_tangential->clear();

    double min_y = DBL_MAX;
    double max_y = -DBL_MAX;
    double min_x = DBL_MAX;
    double max_x = -DBL_MAX;
    for(unsigned i=0;i<200;i++)
    {
        double angle2 = (selectedModel->max_angle)*i/199.0;
        double n_trac = selectedModel->normal_traction(angle2);
        double t_trac = selectedModel->tangential_traction(angle2);

        series_mohr->append(n_trac,t_trac);
        series_normal->append(angle2, n_trac);
        series_tangential->append(angle2, t_trac);
        if(min_x > n_trac) min_x = n_trac;
        if(max_x < n_trac) max_x = n_trac;
        if(min_y > t_trac) min_y = t_trac;
        if(max_y < t_trac) max_y = t_trac;
    }
    double span_y = max_y-min_y;
    double span_x = max_x-min_x;
    double span = std::max(span_y, span_x)*0.65;
    double avgx = (min_x+max_x)/2;

//    double span_y = selectedModel->max_tangential_trac-selectedModel->min_tangential_trac;
//    double span_x = selectedModel->max_normal_trac-selectedModel->min_normal_trac;
//    double span = std::max(span_y, span_x)*0.65;
//    double avgx = (selectedModel->max_normal_trac+selectedModel->min_normal_trac)/2;

    QList<QAbstractAxis*> axes = chart_line_mohr->axes();
    axes[0]->setRange(avgx-span, avgx+span);
    axes[1]->setRange(-span, span);

    axes = chart_line->axes();
    axes[0]->setRange(0, selectedModel->max_angle);
    double max_coord = std::max(max_x, max_y)*1.1;
    axes[1]->setRange(-max_coord, max_coord);


    // plots - selections
    series_selected->clear();
    series_mohr_selected->clear();
    series_selected->append(angle, normal_trac);
    series_mohr_selected->append(normal_trac, tangential_trac);

    // append max points
    max_graph->clear();
    max_mohr->clear();
    max_mohr->append(selectedModel->normal_traction(selectedModel->fracture_angle),
                                 selectedModel->tangential_traction(selectedModel->fracture_angle));
    max_graph->append(selectedModel->fracture_angle,
                                 selectedModel->normal_traction(selectedModel->fracture_angle));

    // sector limits

    sector_limits->clear();
    sector_limits_mohr->clear();
    for(Model::Sector &s : selectedModel->fan) {
        double angle = s.angle1;
        double normal_traction = selectedModel->normal_traction(angle);
        double tangential_traction = selectedModel->tangential_traction(angle);
        sector_limits->append(angle, normal_traction);
        sector_limits_mohr->append(normal_traction, tangential_traction);
    }
    sector_limits->append(0, selectedModel->normal_traction(0));
    sector_limits_mohr->append(selectedModel->normal_traction(0), selectedModel->tangential_traction(0));


    // VTK
/*
    int nSectors = selectedModel->fan.size();
    points->SetNumberOfPoints(nSectors*3);
    vtkIdType pts2[3];
    ugrid->Reset();
    for(int i=0;i<nSectors;i++)
    {
        Model::Sector &s = selectedModel->fan[i];
        points->SetPoint(i*3+0, 0, 0, 0);
        points->SetPoint(i*3+1, s.u.x(), s.u.y(), 0);
        points->SetPoint(i*3+2, s.v.x(), s.v.y(), 0);

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

    double d[3]={};
    d[0] = res.tn.x();
    d[1] = res.tn.y();
    arrowCoords->SetTuple(0, d);
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
*/


}
void MainWindow::spinValueChanged(int val)
{
    selectedModel = models[val];
    UpdateGUI();
}

