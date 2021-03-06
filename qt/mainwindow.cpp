#include "mainwindow.h"
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkNew.h>
#include <vtkRendererCollection.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkPointData.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkAxesActor.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <QVTKInteractor.h>
#include <QString>
#include <vtkDataSetMapper.h>
#include <QtWidgets>
#include <vector>
#include <algorithm>
#include <map> 

MainWindow::MainWindow(char *c )
{
    this->setupUi(this);
    this->objDirectory = QString::fromUtf8(c);
    this->label->setVisible(false);
    this->listView->setVisible(false);

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    this->qvtkWidget->setRenderWindow(renderWindow);

    auto colors = vtkSmartPointer<vtkNamedColors>::New();

    this->model = new QStringListModel(this);
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());
    this->qvtkWidget->renderWindow()->AddRenderer (renderer);
    this->qvtkWidget->renderWindow()->SetWindowName("AtilaSoftwareCalculator");
    this->setAxes(colors);
    renderer->ResetCamera();

    connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
    connect(this->actionOpenFile, SIGNAL(triggered()), this, SLOT(slotOpenFile()));
    
}
void MainWindow::setAxes(vtkSmartPointer<vtkNamedColors> &colors){
    auto axes = vtkSmartPointer<vtkAxesActor>::New();
    this->widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    double rgba[4]{0.0, 0.0, 0.0, 0.0};
    colors->GetColor("rgb",rgba);
    this->widget->SetOutlineColor(rgba[0], rgba[1], rgba[2]);
    this->widget->SetOrientationMarker( axes );
    this->widget->SetInteractor( this->qvtkWidget->interactor() );
    this->widget->SetViewport( 0.0, 0.0, 0.2, 0.2 );
    this->widget->SetEnabled( 1 );
    this->widget->InteractiveOn();
}

MainWindow::~MainWindow()
{
    delete this;
}
void MainWindow::slotExit()
{
    cout<<"Out"<<endl;
    this->close();
    qApp->exit(); 
}
void MainWindow::slotOpenFile(){
  cout<<"Opening of a file .."<<endl;
  QString fileName = QFileDialog::getOpenFileName(this,
                        "Select a .res file",
                        this->objDirectory,
                        "Res (*.res)");
  if (!fileName.isEmpty()){
    //delete(this->binary);   
    //this->qvtkWidget->renderWindow()->GetRenderers()->GetFirstRenderer()->RemoveAllViewProps();
    if (this->binary == NULL){
      this->binary = new Binary_data_class(fileName.toStdString());

      QAction *b = this->menuBar1->addAction("To Text");
      QAction *c = this->menuBar1->addAction("Zoom on Area");
      QAction *d = this->menuBar1->addAction("Interact with object");
      QAction *e = this->menuBar1->addAction("Reset Camera");
      connect(b, SIGNAL(triggered()), this, SLOT(slotToText()));
      connect(c, SIGNAL(triggered()), this, SLOT(slotZoomArea()));
      connect(d, SIGNAL(triggered()), this, SLOT(slotInteractObj()));
      connect(e, SIGNAL(triggered()), this, SLOT(slotResetCamera()));

      std::vector<float> steps;
      std::map<float,QMenu*>stepsXmenus;
      for (auto &&res : this->binary->results_)
      {
        //Permet de savoir si le menu a déjà été crée 
        if ( !std::binary_search(steps.begin(), steps.end(), res.step_)){
            steps.push_back(res.step_);
            QMenu *menu1 = this->menuResults->addMenu(QString::number(res.step_));
            stepsXmenus.insert(std::pair<float, QMenu*>(res.step_, menu1));
        }
        //Crée un menu de resultat dans le menu dans le menu correspondant au step du resultat 
        QMenu *menu = stepsXmenus[res.step_]->addMenu( QString::fromStdString(res.analysis_));
        for (int i = 0 ; i<res.result_size_ ; i++){
          QAction *a;
          if (res.component_names_.size()>0 )
          {
            a = menu->addAction(QString::fromStdString(res.component_names_.at(i)));
          }else{
            a = menu->addAction(QString::number(i));
          }
          connect(a, &QAction::triggered, [this,&res,i]()
          {
              this->slotResult(res,i);
          });
        }
      }
    this->setVTK(this->binary->results_.front(),0);
    }else
    {
      delete this->binary;
      this->binary = NULL;
      this->qvtkWidget->renderWindow()->GetRenderers()->GetFirstRenderer()->RemoveAllViewProps();
    }
  }
}
void MainWindow::slotResult(Str_Result& res , const int& choice ){
    this->qvtkWidget->renderWindow()->GetRenderers()->GetFirstRenderer()->RemoveAllViewProps();
    this->setVTK(res,choice); 
  }
void MainWindow::setVTK(Str_Result &res, const int& choice ){
    this->binary->setScalarFromQT2(res,choice); 

    this->model->setStringList(this->binary->getstrList());
    this->listView->setModel(this->model); 
    this->listView->adjustSize();
    this->label->setVisible(true);
    this->listView->setVisible(true);
    
    //Crée une table de couleur par rapport au valeur du scalar  
    double range[2]= {this->binary->getScalars()->GetRange()[0],this->binary->getScalars()->GetRange()[1]};
    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(this->binary->getScalars()->GetNumberOfTuples() + 1 );
    lut->SetTableRange(range);
    lut->Build();
    
    this->binary->getUGrid()->GetPointData()->SetScalars(this->binary->getScalars());
    auto mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputData(this->binary->getUGrid());
    mapper->SetScalarRange(range);
    mapper->SetLookupTable(lut);

    auto scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->SetLookupTable(mapper->GetLookupTable());
    scalarBar->SetTitle( string(res.analysis_ + std::__cxx11::to_string(choice)).c_str()  );
    scalarBar->UnconstrainedFontSizeOn ();
    scalarBar->SetNumberOfLabels(5);
    scalarBar->SetBarRatio(scalarBar->GetBarRatio()/2.0);

    auto polyActor = vtkSmartPointer<vtkActor>::New();
    polyActor->SetMapper(mapper);

    this->qvtkWidget->renderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(polyActor);
    this->qvtkWidget->renderWindow()->GetRenderers()->GetFirstRenderer()->AddActor2D(scalarBar);
    this->qvtkWidget->renderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();
}
void MainWindow::slotToText(){

  this->binary->toTextFile();  
  QMessageBox msgBox;
  msgBox.setText("Done, saved in the current directory ");
  msgBox.exec();
}
void MainWindow::slotResetCamera(){

  this->qvtkWidget->renderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();

}
void MainWindow::slotZoomArea(){
  auto style = vtkSmartPointer<vtkInteractorStyleRubberBandZoom>::New();
  this->qvtkWidget->renderWindow()->GetInteractor()->SetInteractorStyle(style);
}
void MainWindow::slotInteractObj(){
  auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  this->qvtkWidget->renderWindow()->GetInteractor()->SetInteractorStyle(style);
}