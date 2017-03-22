#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/computes/scriptnodecompute.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QObject>

class QJsonValue;
class QQmlContext;

namespace ngs {


class GUICOMPUTES_EXPORT FilterByTypeAndValueNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, FilterByTypeAndValueNodeCompute);
  FilterByTypeAndValueNodeCompute(Entity* entity);
  virtual ~FilterByTypeAndValueNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT FilterByPositionNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, FilterByPositionNodeCompute);
  FilterByPositionNodeCompute(Entity* entity);
  virtual ~FilterByPositionNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT FilterByDimensionsNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, FilterByDimensionsNodeCompute);
  FilterByDimensionsNodeCompute(Entity* entity);
  virtual ~FilterByDimensionsNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT FilterByViewportNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, FilterByViewportNodeCompute);
  FilterByViewportNodeCompute(Entity* entity);
  virtual ~FilterByViewportNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT FindClosestNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, FindClosestNodeCompute);
  FindClosestNodeCompute(Entity* entity);
  virtual ~FindClosestNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT FilterToSideMostNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, FilterToSideMostNodeCompute);
  FilterToSideMostNodeCompute(Entity* entity);
  virtual ~FilterToSideMostNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
};

class GUICOMPUTES_EXPORT FilterByIndexNodeCompute: public BaseScriptNodeCompute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, FilterByIndexNodeCompute);
  FilterByIndexNodeCompute(Entity* entity);
  virtual ~FilterByIndexNodeCompute();

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

}
