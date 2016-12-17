#pragma once
#include <components/computes/computes_export.h>

class QString;

class QJsonValue;
class QJsonObject;
class QJsonArray;
class QJSEngine;
class QByteArray;


namespace ngs {

class Path;

class COMPUTES_EXPORT JSONUtils {
 public:

  // Testing QJsonValue conversion routines.
  static void test_convert_to_bool();
  static void test_convert_to_double();
  static void test_convert_to_string();
  static void test_convert_to_object();
  static void test_convert_to_array();

  // Testing deep merge recursive logic. Note this is just a sanity check.
  // It doesn't check every possible case.
  static void test_deep_merge_object_to_object();
  static void test_deep_merge_array_to_array();
  static void test_deep_merge_array_to_object();
  static void test_deep_merge_object_to_array();

  // Convert to JSON.
  static QByteArray serialize_json_value(const QJsonValue& value); // Embeds the value as a simgle in an array.
  static QJsonValue deserialize_json_value(const QByteArray& data);

  // QJsonValue conversion routines.
  static bool convert_to_bool(const QJsonValue& source);
  static double convert_to_double(const QJsonValue& source);
  static QString convert_to_string(const QJsonValue& source);
  static QJsonObject convert_to_object(const QJsonValue& source);
  static QJsonArray convert_to_array(const QJsonValue& source);

  // Deep merging of QJsonValues.
  static QJsonValue deep_merge(const QJsonValue& target, const QJsonValue& source);

  // Shallow merging of Objects.
  static void shallow_object_merge(QJsonObject& target, const QJsonObject& source);

  // Evaluate strings into values.
  static QString value_to_json(QJsonValue);
  static bool eval_json(const QString& json, QJsonValue& result, QString& error);
  static bool eval_js(const QString& expr, QJsonValue& result, QString& error);
  static QJsonValue eval_js2(const QString& expr);
  static bool eval_js_in_context(QJSEngine& engine, const QString& expr, QJsonValue& result, QString& error);


  // Grab values from objects.
  static QJsonValue extract_value(const QJsonObject& src, const Path& src_path, const QJsonValue& target);
  static QJsonObject embed_value(const QJsonObject& obj, const Path& dest_path, const QJsonValue& dest_value);
  static QJsonObject erase_value(const QJsonObject& obj, const Path& path);
};

}
