#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/computes/scriptnodecompute.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QObject>

class QJsonValue;
class QQmlContext;

namespace ngs {


class GUICOMPUTES_EXPORT IsolateByTypeAndValueNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, IsolateByTypeAndValueNodeCompute);
  IsolateByTypeAndValueNodeCompute(Entity* entity);
  virtual ~IsolateByTypeAndValueNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT IsolateByPositionNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, IsolateByPositionNodeCompute);
  IsolateByPositionNodeCompute(Entity* entity);
  virtual ~IsolateByPositionNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT IsolateByDimensionsNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, IsolateByDimensionsNodeCompute);
  IsolateByDimensionsNodeCompute(Entity* entity);
  virtual ~IsolateByDimensionsNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT IsolateByViewportNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, IsolateByViewportNodeCompute);
  IsolateByViewportNodeCompute(Entity* entity);
  virtual ~IsolateByViewportNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT SortByDistanceToAnchorsNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, SortByDistanceToAnchorsNodeCompute);
  SortByDistanceToAnchorsNodeCompute(Entity* entity);
  virtual ~SortByDistanceToAnchorsNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT IsolateByBoundaryNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, IsolateByBoundaryNodeCompute);
  IsolateByBoundaryNodeCompute(Entity* entity);
  virtual ~IsolateByBoundaryNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT IsolateToOneSideNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, IsolateToOneSideNodeCompute);
  IsolateToOneSideNodeCompute(Entity* entity);
  virtual ~IsolateToOneSideNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT IsolateByIndexNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, IsolateByIndexNodeCompute);
  IsolateByIndexNodeCompute(Entity* entity);
  virtual ~IsolateByIndexNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT JoinElementsNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, JoinElementsNodeCompute);
  JoinElementsNodeCompute(Entity* entity);
  virtual ~JoinElementsNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT PruneElementsNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, PruneElementsNodeCompute);
  PruneElementsNodeCompute(Entity* entity);
  virtual ~PruneElementsNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT PruneDuplicatesNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, PruneDuplicatesNodeCompute);
  PruneDuplicatesNodeCompute(Entity* entity);
  virtual ~PruneDuplicatesNodeCompute();
};

}
