/*
 * This C++ source file was generated by the Gradle 'init' task.
 */

#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <assert.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>
using namespace std;

int main(int argc, char *argv[]) {
	QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
	QApplication a(argc, argv);
    MainWindow w(argv[1]);
	w.show();	
	return a.exec();
}