#pragma once
#include <components/computes/computes_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QJsonObject>

namespace ngs {

class COMPUTES_EXPORT ScriptLoopContext : public Component {
 public:
  COMPONENT_ID(ScriptLoopContext, ScriptLoopContext)


  explicit ScriptLoopContext(Entity* parent);
  virtual ~ScriptLoopContext();

  void reset() {_context = QJsonObject();}
  QJsonObject& get_context() {return _context;}
  void set_context(const QJsonObject& context) {_context = context;}

 protected:

 private:
  QJsonObject _context;
};

}

