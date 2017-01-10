#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <components/computes/groupnodecompute.h>
#include <base/objectmodel/componentids.h>
#include <guicomponents/comms/message.h>

// QT
#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace ngs {

class GUICOMPUTES_EXPORT ScriptGroupNodeCompute: public QObject, public GroupNodeCompute {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, ScriptGroupNodeCompute);
  ScriptGroupNodeCompute(Entity* entity);
  virtual ~ScriptGroupNodeCompute();

  // Inputs and Outputs.
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());
  virtual const std::unordered_set<std::string>& get_fixed_inputs() const;

  // Hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  // Scripting API.
  Q_INVOKABLE QJsonValue get_input(const QString& names);
  Q_INVOKABLE void set_output(const QString& name, const QJsonValue& value);

 protected:

  // Our state.
  virtual bool update_state();

  virtual bool evaluate_script();

 private:

  std::unordered_set<std::string> _fixed_inputs;
};

}
