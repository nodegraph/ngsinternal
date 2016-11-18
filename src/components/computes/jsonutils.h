#pragma once
#include <components/computes/computes_export.h>

class QString;

class QJsonValue;
class QJsonObject;
class QJsonArray;
class QJSEngine;


namespace ngs {

class COMPUTES_EXPORT JSONUtils {
 public:

  // Unit Testing.
  static void test_convert_to_bool();
  static void test_convert_to_double();
  static void test_convert_to_string();
  static void test_convert_to_object();
  static void test_convert_to_array();
  static void test_deep_merge();


  // Merging JSON values.
  static bool convert_to_bool(const QJsonValue& source);
  static double convert_to_double(const QJsonValue& source);
  static QString convert_to_string(const QJsonValue& source);
  static QJsonObject convert_to_object(const QJsonValue& source);
  static QJsonArray convert_to_array(const QJsonValue& source);
  static QJsonValue deep_merge(const QJsonValue& target, const QJsonValue& source);

  // Various merges and overrides.
  // Merges the properties from source into target.
  static void shallow_object_merge(QJsonObject& target, const QJsonObject& source);
  // Changes the source from a js/json string to an object or array or vice versa in order to match the target.
  //static void prep_source_for_merge(const QJsonValue& target, QJsonValue& source);
  // Merges the properties from source into target recursively, where the property types match.
  //static QJsonValue deep_merge(const QJsonValue& target, const QJsonValue& source);


  // Evaluate strings into values.
  static QString value_to_json(QJsonValue);
  static bool eval_json(const QString& json, QJsonValue& result, QString& error);
  static bool eval_js(const QString& expr, QJsonValue& result, QString& error);
  static QJsonValue eval_js2(const QString& expr);
  static bool eval_js_in_context(QJSEngine& engine, const QString& expr, QJsonValue& result, QString& error);

};

}
