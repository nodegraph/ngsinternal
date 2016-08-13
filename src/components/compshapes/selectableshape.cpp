#include <components/compshapes/selectableshape.h>

namespace ngs {

SelectableShape::SelectableShape(Entity* entity, size_t did):
  CompShape(entity, did),
  _selected(false) {
}

SelectableShape::~SelectableShape() {
}

// Selection.
void SelectableShape::select(bool select) {
  _selected = select;
}

bool SelectableShape::is_selected() const {
  return _selected;
}

}
