#pragma once
#include <gui/gui_export.h>
#include <QtWidgets/QDockWidget>


class GUI_EXPORT DockWidget: public QDockWidget {
  Q_OBJECT
 public:
  DockWidget(const QString & title, QWidget* parent);
  virtual ~DockWidget();

 protected:

  // Qt Overrides.
  void keyPressEvent(QKeyEvent *event);

 private:


};
