#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <tuple>
#include <QMainWindow>
#include <QString>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <QStringList>
#include <QStringListModel>
#include <vtkOrientationMarkerWidget.h>
#include <vtkNamedColors.h>
#include <QSignalMapper>

#include "ui_mainwindow.h"
#include "binary_data_class.hpp"


class MainWindow : public QMainWindow , public Ui::MainWindow
{
    Q_OBJECT
public:
    mutable Binary_data_class *binary = NULL;
    MainWindow(char *c);
    ~MainWindow();
public slots:
    void slotExit();
    void slotToText();
    void slotOpenFile();
    void slotResetCamera(); 
    void slotResult(const QString& typeResult);
private : 
    QString objDirectory ; 
    vtkSmartPointer<vtkOrientationMarkerWidget> widget;
    QSignalMapper* signalMapper ;
    QStringListModel *model;
    void setAxes(vtkSmartPointer<vtkNamedColors> &colors);
    void setVTK(const int& choice ,const std::string& typeResult, const float& step);
};

#endif // MAINWINDOW_H
