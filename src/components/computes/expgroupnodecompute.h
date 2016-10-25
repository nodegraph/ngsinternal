#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/groupnodecompute.h>
#include <entities/componentids.h>

// QT
#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace ngs {

class BaseGroupTraits;

class COMPUTES_EXPORT ExpGroupNodeCompute: public QObject, public GroupNodeCompute {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, ExpGroupNodeCompute);
  ExpGroupNodeCompute(Entity* entity);
  virtual ~ExpGroupNodeCompute();

  // Inputs and Outputs.
  virtual void create_inputs_outputs();

  // Hints.
  static QVariantMap init_hints();
  static const QVariantMap _hints;
  virtual const QVariantMap& get_hints() const {return _hints;}

  // Scripting API.
  Q_INVOKABLE void set_output_value(const QString& name, const QVariant& value);

 protected:

  // Our state.
  virtual bool update_state();

  virtual void evaluate_script();

 private:
  Dep<BaseGroupTraits> _group_traits;
};

}
