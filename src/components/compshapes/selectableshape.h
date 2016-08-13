#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/compshape.h>

namespace ngs {

class COMPSHAPES_EXPORT SelectableShape: public CompShape {
 public:
  COMPONENT_ID(CompShape, InvalidComponent);
  SelectableShape(Entity* entity, size_t did);
  virtual ~SelectableShape();

  // Selection.
  virtual void select(bool select);
  virtual bool is_selected() const;

  // Our sub interfaces.
  virtual bool is_selectable() {return true;}

 protected:
  bool _selected;
};

}
