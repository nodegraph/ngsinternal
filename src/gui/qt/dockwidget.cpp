#include <gui/qt/dockwidget.h>

#include <iostream>

DockWidget::DockWidget(const QString & title, QWidget* parent)
  : QDockWidget(title,parent){

}

DockWidget::~DockWidget() {

}

void DockWidget::keyPressEvent(QKeyEvent *event) {
  std::cerr << "DockWidget got key press event \n";
}
