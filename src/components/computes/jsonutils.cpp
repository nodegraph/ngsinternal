#include <components/computes/jsonutils.h>
#include <base/utils/path.h>

#include <sstream>
#include <iostream>
#include <cassert>

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlExpression>

#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>


#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include <QtCore/QString>
#include <QtCore/QStringList>


namespace ngs {

void JSONUtils::test_convert_to_bool() {
  // Convert elemental types.
  assert(convert_to_bool(QJsonValue::Null) == false);
  assert(convert_to_bool(QJsonValue::Undefined) == false);
  assert(convert_to_bool(false) == false);
  assert(convert_to_bool(true) == true);
  assert(convert_to_bool(0.0) == false);
  assert(convert_to_bool(1.0) == true);
  assert(convert_to_bool("") == false);
  assert(convert_to_bool("test") == true);

  // Convert array types.
  QJsonArray arr;
  // Check empty array.
  assert(convert_to_bool(arr) == false);
  // Check recursion on single element array for one type (numbers).
  arr.push_back(0);
  assert(convert_to_bool(arr) == false);
  arr.pop_back();
  arr.push_back(1);
  assert(convert_to_bool(arr) == true);

  // Convert object types.
  QJsonObject obj;
  // Check empty object.
  assert(convert_to_bool(obj) == false);
  // Check recursion on single element obj for one type (numbers).
  obj.insert("a", 0);
  assert(convert_to_bool(obj) == false);
  // Check recursion on multi-element obj for one type (numbers).
  obj.insert(Compute::kValuePropertyName, 1);
  assert(convert_to_bool(obj) == true);

  std::cerr << "convert_to_bool test passed\n";
}

void JSONUtils::test_convert_to_double() {
  // Convert elemental types.
  assert(convert_to_double(QJsonValue::Null) == 0.0);
  assert(convert_to_double(QJsonValue::Undefined) == 0.0);
  assert(convert_to_double(false) == 0.0);
  assert(convert_to_double(true) == 1.0);
  assert(convert_to_double(0.0) == 0.0);
  assert(convert_to_double(1.0) == 1.0);
  assert(convert_to_double("") == 0.0);
  assert(convert_to_double("test") == 0.0);
  assert(convert_to_double("123") == 123.0);

  // Convert array types.
  QJsonArray arr;
  // Check empty array.
  assert(convert_to_double(arr) == 0.0);
  // Check recursion on single element array for one type (numbers).
  arr.push_back(0);
  assert(convert_to_double(arr) == 0.0);
  arr.pop_back();
  arr.push_back(1);
  assert(convert_to_double(arr) == 1.0);

  // Convert object types.
  QJsonObject obj;
  // Check empty object.
  assert(convert_to_double(obj) == 0.0);
  // Check recursion on single element obj for one type (numbers).
  obj.insert("a", 0);
  assert(convert_to_double(obj) == 0.0);
  obj.insert("a", 1);
  assert(convert_to_double(obj) == 1.0);
  // Check recursion on multi-element obj for one type (numbers).
  obj.insert(Compute::kValuePropertyName, 0);
  assert(convert_to_double(obj) == 0.0);
  obj.insert(Compute::kValuePropertyName, 1);
  assert(convert_to_double(obj) == 1.0);

  std::cerr << "convert_to_double test passed\n";
}

void JSONUtils::test_convert_to_string() {
  // Convert elemental types.
  assert(convert_to_string(QJsonValue::Null) == "");
  assert(convert_to_string(QJsonValue::Undefined) == "");
  assert(convert_to_string(false) == "false");
  assert(convert_to_string(true) == "true");
  assert(convert_to_string(0.0) == "0");
  assert(convert_to_string(123.0) == "123");
  assert(convert_to_string("") == "");
  assert(convert_to_string("test") == "test");

  // Convert array types.
  QJsonArray arr;
  // Check empty array.
  assert(convert_to_string(arr) == "");
  // Check recursion on single element array for one type (numbers).
  arr.push_back(0);
  assert(convert_to_string(arr) == "0");
  arr.pop_back();
  arr.push_back(1);
  assert(convert_to_string(arr) == "1");

  // Convert object types.
  QJsonObject obj;
  // Check empty object.
  assert(convert_to_string(obj) == "");
  // Check recursion on single element obj for one type (numbers).
  obj.insert("a", 0);
  assert(convert_to_string(obj) == "0");
  obj.insert("a", 1);
  assert(convert_to_string(obj) == "1");
  // Check recursion on multi-element obj for one type (numbers).
  obj.insert(Compute::kValuePropertyName, 0);
  assert(convert_to_string(obj) == "0");
  obj.insert(Compute::kValuePropertyName, 1);
  assert(convert_to_string(obj) == "1");

  std::cerr << "convert_to_string test passed\n";
}

void JSONUtils::test_convert_to_object() {
  // Convert elemental types.
  assert(convert_to_object(QJsonValue::Null) == QJsonObject());
  assert(convert_to_object(QJsonValue::Undefined) == QJsonObject());
  {
    QJsonObject expected;
    expected.insert(Compute::kValuePropertyName, false);
    assert(convert_to_object(false) == expected);
    expected.insert(Compute::kValuePropertyName, true);
    assert(convert_to_object(true) == expected);
    expected.insert(Compute::kValuePropertyName, 0);
    assert(convert_to_object(0.0) == expected);
    expected.insert(Compute::kValuePropertyName, 123);
    assert(convert_to_object(123.0) == expected);
    expected.insert(Compute::kValuePropertyName, "");
    assert(convert_to_object("") == expected);
    expected.insert(Compute::kValuePropertyName, "test");
    assert(convert_to_object("test") == expected);
  }

  // Convert array types.
  {
    QJsonArray arr;
    QJsonObject expected;
    // Check empty array.
    assert(convert_to_object(arr) == QJsonObject());
    // Check recursion on single element array for one type (numbers).
    arr.push_back(0);
    expected.insert("0", 0);
    assert(convert_to_object(arr) == expected);
    arr.push_back(1);
    expected.insert("1", 1);
    assert(convert_to_object(arr) == expected);

    // Convert object types.
    QJsonObject obj;
    expected = QJsonObject();
    // Check empty object.
    assert(convert_to_object(obj) == QJsonObject());
    // Check recursion on single element obj for one type (numbers).
    obj.insert("a", 0);
    expected.insert("a", 0);
    assert(convert_to_object(obj) == expected);
    // Check recursion on multi-element obj for one type (numbers).
    obj.insert(Compute::kValuePropertyName, 1);
    expected.insert(Compute::kValuePropertyName, 1);
    assert(convert_to_object(obj) == expected);
  }

  std::cerr << "convert_to_object test passed\n";
}

void JSONUtils::test_convert_to_array() {
  // Convert elemental types.
  assert(convert_to_array(QJsonValue::Null) == QJsonArray());
  assert(convert_to_array(QJsonValue::Undefined) == QJsonArray());
  {
    QJsonArray expected;
    expected.push_back(false);
    assert(convert_to_array(false) == expected);
    expected.pop_back();
    expected.push_back(true);
    assert(convert_to_array(true) == expected);
    expected.pop_back();
    expected.push_back(0);
    assert(convert_to_array(0.0) == expected);
    expected.pop_back();
    expected.push_back(123);
    assert(convert_to_array(123.0) == expected);
    expected.pop_back();
    expected.push_back("");
    assert(convert_to_array("") == expected);
    expected.pop_back();
    expected.push_back("test");
    assert(convert_to_array("test") == expected);
  }

  // Convert array types.
  {
    QJsonArray arr;
    QJsonArray expected;
    // Check empty array.
    assert(convert_to_array(arr) == QJsonArray());
    // Check recursion on single element array for one type (numbers).
    arr.push_back(0);
    expected.push_back(0);
    assert(convert_to_array(arr) == expected);
    arr.push_back(1);
    expected.push_back(1);
    assert(convert_to_array(arr) == expected);

    // Convert object types.
    QJsonObject obj;
    expected = QJsonArray();
    // Check empty object.
    assert(convert_to_array(obj) == QJsonArray());
    // Check recursion on single element obj for one type (numbers).
    obj.insert("bbb", 222);
    expected.push_back(222);
    assert(convert_to_array(obj) == expected);
    // Check recursion on multi-element obj for one type (numbers).
    obj.insert("aaa", 111);
    // Note the resulting array will the source object's property value in alphabetized key order.
    expected.pop_back();
    expected.push_back(111);
    expected.push_back(222);
    assert(convert_to_array(obj) == expected);
    obj.insert("ccc", 333);
    expected.push_back(333);
    assert(convert_to_array(obj) == expected);
  }

  std::cerr << "convert_to_array test passed\n";
}

void JSONUtils::test_deep_merge_object_to_object() {
  QJsonObject target;
  {
    target.insert("aaa",1);
    target.insert("bbb", 2);
    QJsonObject target2;
    target2.insert("ddd",3);
    target2.insert("eee",4);
    target.insert("ccc", target2);
  }

  QJsonObject source;
  {
    source.insert("aaa",11);
    source.insert("bbb1",22);
    QJsonObject source2;
    source2.insert("ddd1",33);
    source2.insert("eee",44);
    source.insert("ccc", source2);
  }

  QJsonObject expected;
  {
    expected.insert("aaa",11);
    expected.insert("bbb",2);
    expected.insert("bbb1",22);
    QJsonObject expected2;
    expected2.insert("ddd",3);
    expected2.insert("ddd1",33);
    expected2.insert("eee",44);
    expected.insert("ccc", expected2);
  }

//  qDebug() << "target: " << JSONUtils::serialize_json_value(target);
//  qDebug() << "source: " << JSONUtils::serialize_json_value(source);
//  qDebug() << "expected: " << JSONUtils::serialize_json_value(expected);
//  qDebug() << "result: " << JSONUtils::serialize_json_value(deep_merge(target, source));

  assert(QJsonValue(expected) == deep_merge(target, source));

  std::cerr << "deep_merge_object_to_object test passed\n";
}

void JSONUtils::test_deep_merge_array_to_array() {
  QJsonArray target;
  {
    target.push_back(1);
    target.push_back(2);
    target.push_back(3);
    QJsonArray target2;
    target2.push_back(4);
    target2.push_back(5);
    target2.push_back(6);
    target.push_back(target2);
  }

  QJsonArray source;
  {
    source.push_back(11);
    source.push_back(22);
    source.push_back(33);
    QJsonArray source2;
    source2.push_back(44);
    source2.push_back(55);
    source.push_back(source2);
    source.push_back(66);
  }

  QJsonArray expected;
  {
    expected.push_back(11);
    expected.push_back(22);
    expected.push_back(33);
    QJsonArray expected2;
    expected2.push_back(44);
    expected2.push_back(55);
    expected.push_back(expected2);
    expected.push_back(66);
  }

//  qDebug() << "target: " << JSONUtils::serialize_json_value(target);
//  qDebug() << "source: " << JSONUtils::serialize_json_value(source);
//  qDebug() << "expected: " << JSONUtils::serialize_json_value(expected);
//  qDebug() << "result: " << JSONUtils::serialize_json_value(deep_merge(target, source));

  assert(QJsonValue(expected) == deep_merge(target, source));

  std::cerr << "deep_merge_array_to_array test passed\n";
}

void JSONUtils::test_deep_merge_array_to_object() {
  QJsonObject target;
  {
    target.insert("0", 0);
    target.insert("1", 1);
    target.insert("2", 2);
    target.insert(Compute::kValuePropertyName, "aaa");
    QJsonObject target2;
    target2.insert("0", 0);
    target2.insert("1", 1);
    target2.insert("2", 2);
    target2.insert(Compute::kValuePropertyName, "bbb");
    target.insert("3", target2);
  }

  QJsonArray source;
  {
    source.push_back(0);
    source.push_back(11);
    source.push_back(22);
    QJsonArray source2;
    source2.push_back(0);
    source2.push_back(44);
    source2.push_back(55);
    source.push_back(source2);
  }

  QJsonObject expected;
  {
    expected.insert("0", 0);
    expected.insert("1", 11);
    expected.insert("2", 22);
    expected.insert(Compute::kValuePropertyName, "aaa");
    QJsonObject expected2;
    expected2.insert("0", 0);
    expected2.insert("1", 44);
    expected2.insert("2", 55);
    expected2.insert(Compute::kValuePropertyName, "bbb");
    expected.insert("3", expected2);
  }

//  qDebug() << "target: " << JSONUtils::serialize_json_value(target);
//  qDebug() << "source: " << JSONUtils::serialize_json_value(source);
//  qDebug() << "expected: " << JSONUtils::serialize_json_value(expected);
//  qDebug() << "result: " << JSONUtils::serialize_json_value(deep_merge(target, source));

  assert(QJsonValue(expected) == deep_merge(target, source));

  std::cerr << "deep_merge_array_to_object test passed\n";
}

void JSONUtils::test_deep_merge_object_to_array() {
  QJsonArray target;
  {
    target.push_back(0);
    target.push_back(1);
    target.push_back(2);
    QJsonArray target2;
    target2.push_back(0);
    target2.push_back(1);
    target2.push_back(2);
    target.push_back(target2);
    target.push_back(target2);
  }

  QJsonObject source;
  {
    source.insert("a", 0);
    source.insert("aaa", 11);
    source.insert("bbb", 22);
    QJsonObject source2;
    source2.insert("a", 0);
    source2.insert("aaa", 11);
    source2.insert("bbb", 22);
    source2.insert("ccc", 33);
    source.insert("ccc", source2);
    source.insert("ddd", 44);
  }

  QJsonArray expected;
  {
    expected.push_back(0);
    expected.push_back(11);
    expected.push_back(22);
    QJsonArray expected2;
    expected2.push_back(0);
    expected2.push_back(11);
    expected2.push_back(22);
    expected2.push_back(33);
    expected.push_back(expected2);
    QJsonArray expected3;
    expected3.push_back(44);
    expected.push_back(expected3);
  }

//  qDebug() << "target: " << JSONUtils::serialize_json_value(target);
//  qDebug() << "source: " << JSONUtils::serialize_json_value(source);
//  qDebug() << "expected: " << JSONUtils::serialize_json_value(expected);
//  qDebug() << "result: " << JSONUtils::serialize_json_value(deep_merge(target, source));

  assert(QJsonValue(expected) == deep_merge(target, source));

  std::cerr << "deep_merge_object_to_array test passed\n";
}

QByteArray JSONUtils::serialize_json_value(const QJsonValue& value) {
  // We create a dummy array to shove the value into, as QJsonDocument won't take a non-array or non-object.
  // However the newer json specs allow storing singular values, this feature may be coming soon.
  QJsonArray arr;
  arr.push_back(value);
  QJsonDocument doc;
  doc.setArray(arr);
  QByteArray data = doc.toJson();
  return data;
}

QJsonValue JSONUtils::deserialize_json_value(const QByteArray& data) {
  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(data, &error);
  assert(error.error == QJsonParseError::NoError);
  QJsonArray arr = doc.array();
  assert(arr.size() == 1);
  return arr.at(0);
}

bool JSONUtils::convert_to_bool(const QJsonValue& source) {
  switch (source.type()) {
    case QJsonValue::Null: {
      return false;
      break;
    }
    case QJsonValue::Undefined: {
      return false;
      break;
    }
    case QJsonValue::Bool: {
      return source.toBool();
      break;
    }
    case QJsonValue::Double: {
      if (source.toDouble() == 0) {
        return false;
      } else {
        return true;
      }
      break;
    }
    case QJsonValue::String: {
      QString str = source.toString();
      if (str.isEmpty()) {
        return false;
      } else {
        return true;
      }
      break;
    }
    case QJsonValue::Array: {
      QJsonArray arr = source.toArray();
      if (arr.isEmpty()) {
        return false;
      } else {
        QJsonValue val = arr.first();
        return convert_to_bool(val);
      }
      break;
    }
    case QJsonValue::Object: {
      QJsonObject obj = source.toObject();
      if (obj.isEmpty()) {
        // If the object has no properties, we convert to false.
        return false;
      } else if (obj.size() == 1) {
        // If the object has one property, we convert that value.
        QJsonValue val = obj.begin().value();
        return convert_to_bool(val);
      } else {
        // If the object has more than one property, we convert a property with the name kValuePropertyName.
        QJsonObject::iterator iter = obj.find(Compute::kValuePropertyName);
        if (iter != obj.end()) {
          return convert_to_bool(iter.value());
        } else {
          return false;
        }
      }
      break;
    }
  }
  return false;
}

double JSONUtils::convert_to_double(const QJsonValue& source) {
  switch (source.type()) {
    case QJsonValue::Null: {
      return 0.0;
      break;
    }
    case QJsonValue::Undefined: {
      return 0.0;
      break;
    }
    case QJsonValue::Bool: {
      if (source.toBool()) {
        return 1.0;
      } else {
        return 0.0;
      }
      break;
    }
    case QJsonValue::Double: {
      return source.toDouble();
      break;
    }
    case QJsonValue::String: {
      QString str = source.toString();
      if (str.isEmpty()) {
        return 0.0;
      } else {
        bool ok = true;
        double d = str.toDouble(&ok);
        if (ok) {
          return d;
        } else {
          return 0.0;
        }
      }
      break;
    }
    case QJsonValue::Array: {
      QJsonArray arr = source.toArray();
      if (arr.isEmpty()) {
        return 0.0;
      } else {
        QJsonValue val = arr.first();
        return convert_to_double(val);
      }
      break;
    }
    case QJsonValue::Object: {
      QJsonObject obj = source.toObject();
      if (obj.isEmpty()) {
        // If the object has no properties, we convert to zero.
        return 0.0;
      } else if (obj.size() == 1) {
        // If the object has one property, we convert that value.
        QJsonValue val = obj.begin().value();
        return convert_to_double(val);
      } else {
        // If the object has more than one property, we convert a property with the name kValuePropertyName.
        QJsonObject::iterator iter = obj.find(Compute::kValuePropertyName);
        if (iter != obj.end()) {
          return convert_to_double(iter.value());
        } else {
          return 0.0;
        }
      }
      break;
    }
  }
  return 0.0;
}

QString JSONUtils::convert_to_string(const QJsonValue& source) {
  switch (source.type()) {
    case QJsonValue::Null: {
      return "";
      break;
    }
    case QJsonValue::Undefined: {
      return "";
      break;
    }
    case QJsonValue::Bool: {
      if (source.toBool()) {
        return "true";
      } else {
        return "false";
      }
      break;
    }
    case QJsonValue::Double: {
      return QString::number(source.toDouble());
      break;
    }
    case QJsonValue::String: {
      return source.toString();
      break;
    }
    case QJsonValue::Array: {
      QJsonArray arr = source.toArray();
      if (arr.isEmpty()) {
        return "";
      } else {
        QJsonValue val = arr.first();
        return convert_to_string(val);
      }
      break;
    }
    case QJsonValue::Object: {
      QJsonObject obj = source.toObject();
      if (obj.isEmpty()) {
        // If the object has no properties, we convert to zero.
        return "";
      } else if (obj.size() == 1) {
        // If the object has one property, we convert that value.
        QJsonValue val = obj.begin().value();
        return convert_to_string(val);
      } else {
        // If the object has more than one property, we convert a property with the name kValuePropertyName.
        QJsonObject::iterator iter = obj.find(Compute::kValuePropertyName);
        if (iter != obj.end()) {
          return convert_to_string(iter.value());
        } else {
          return "";
        }
      }
      break;
    }
  }
  return "";
}

QJsonObject JSONUtils::convert_to_object(const QJsonValue& source) {
  switch (source.type()) {
    case QJsonValue::Null: {
      return QJsonObject();
      break;
    }
    case QJsonValue::Undefined: {
      return QJsonObject();
      break;
    }
    case QJsonValue::Bool: {
      QJsonObject obj;
      obj.insert(Compute::kValuePropertyName, source);
      return obj;
      break;
    }
    case QJsonValue::Double: {
      QJsonObject obj;
      obj.insert(Compute::kValuePropertyName, source);
      return obj;
      break;
    }
    case QJsonValue::String: {
      QJsonObject obj;
      obj.insert(Compute::kValuePropertyName, source);
      return obj;
      break;
    }
    case QJsonValue::Array: {
      QJsonArray arr = source.toArray();
      if (arr.isEmpty()) {
        return QJsonObject();
      } else {
        QJsonObject obj;
        for (int i=0; i<arr.size(); ++i) {
          obj.insert(QString::number(i), arr[i]);
        }
        return obj;
      }
      break;
    }
    case QJsonValue::Object: {
      return source.toObject();
      break;
    }
  }
  return QJsonObject();
}

QJsonArray JSONUtils::convert_to_array(const QJsonValue& source) {
  switch (source.type()) {
    case QJsonValue::Null: {
      return QJsonArray();
      break;
    }
    case QJsonValue::Undefined: {
      return QJsonArray();
      break;
    }
    case QJsonValue::Bool: {
      QJsonArray arr;
      arr.push_back(source);
      return arr;
      break;
    }
    case QJsonValue::Double: {
      QJsonArray arr;
      arr.push_back(source);
      return arr;
      break;
    }
    case QJsonValue::String: {
      QJsonArray arr;
      arr.push_back(source);
      return arr;
      break;
    }
    case QJsonValue::Array: {
      return source.toArray();
      break;
    }
    case QJsonValue::Object: {
      QJsonObject obj = source.toObject();
      if (obj.isEmpty()) {
        // If the object has no properties, we convert to zero.
        return QJsonArray();
      } else {
        // Sort the keys alphabetically.
        QStringList keys = obj.keys();
        keys.sort();
        // Add the object property values into the array in alphabetical order.
        QJsonArray arr;
        for (int i=0; i<keys.size(); ++i) {
          arr.push_back(obj[keys[i]]);
        }
        return arr;
      }
      break;
    }
  }
  return QJsonArray();
}


QJsonValue JSONUtils::deep_merge(const QJsonValue& target, const QJsonValue& source) {
  switch (target.type()) {
    case QJsonValue::Null: {
      return source;
      break;
    }
    case QJsonValue::Undefined: {
      return source;
      break;
    }
    case QJsonValue::Bool: {
      return convert_to_bool(source);
      break;
    }
    case QJsonValue::Double: {
      return convert_to_double(source);
      break;
    }
    case QJsonValue::String: {
      return convert_to_string(source);
      break;
    }
    case QJsonValue::Array: {
      // For arrays we consider the size of the array to be a property as well.
      // Hence the return value will have the same number of elements as the source.
      QJsonArray target_arr = target.toArray();
      QJsonArray source_arr = convert_to_array(source);
      QJsonArray result;
      for(int i=0; i<source_arr.size(); ++i) {
        if (i<target_arr.size()) {
          QJsonValue merge = deep_merge(target_arr[i], source_arr[i]);
          result.push_back(merge);
        } else {
          result.push_back(source_arr[i]);
        }
      }
      return result;
      break;
    }
    case QJsonValue::Object: {
      // For objects we merge in the properties from the source into the target.
      QJsonObject target_obj = target.toObject();
      QJsonObject source_obj = convert_to_object(source);
      QJsonObject result = target_obj;
      QJsonObject::iterator source_iter;
      for (source_iter = source_obj.begin(); source_iter != source_obj.end(); ++source_iter) {
        QString key = source_iter.key();
        if (target_obj.find(key) != target_obj.end()) {
          QJsonValue merge = deep_merge(target_obj[key], source_iter.value());
          result.insert(key, merge);
        } else {
          result.insert(source_iter.key(), source_iter.value());
        }
      }
      return result;
      break;
    }
  }
  return target;
}

void JSONUtils::shallow_object_merge(QJsonObject& target, const QJsonObject& source) {
  for (QJsonObject::const_iterator iter = source.constBegin(); iter != source.constEnd(); ++iter) {
    std::cerr << "merging in property: " << iter.key().toStdString() << "\n";
    target.insert(iter.key(), iter.value());
  }
}

QString JSONUtils::value_to_json(QJsonValue value) {
  QJsonDocument doc;
  if (value.isArray()) {
    doc.setArray(value.toArray());
  } else if (value.isObject()) {
    doc.setObject(value.toObject());
  }
  return doc.toJson();
}

bool JSONUtils::eval_json(const QString& json, QJsonValue& result, QString& error) {
  error.clear();

  QJsonParseError parse_error;
  QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &parse_error);

  if (parse_error.error == QJsonParseError::NoError) {
    result = QJsonValue(); // undefined.
    error = parse_error.errorString();
    return false;
  }

  if (doc.isObject()) {
    result = doc.object();
    return true;
  } else if (doc.isArray()) {
    result = doc.array();
    return true;
  }

  return false;
}

bool JSONUtils::eval_js(const QString& expr, QJsonValue& result, QString& error) {
  QJSEngine engine;
  return eval_js_in_context(engine, expr, result, error);
}

QJsonValue JSONUtils::eval_js2(const QString& expr) {
  QJSEngine engine;
  QJsonValue result;
  QString error;
  eval_js_in_context(engine, expr, result, error);
  return result;
}

bool JSONUtils::eval_js_in_context(QJSEngine& engine, const QString& expr, QJsonValue& result, QString& error) {
  // Evaluate the expression.
  QJSValue jsresult;
  jsresult = engine.evaluate(expr, "expression", 0);
  if (jsresult.isError()) {
    // Update the error string.
    std::stringstream ss;
    ss << "Uncaught exception at line: " << jsresult.property("lineNumber").toInt() << "\n";
    ss << "name: " << jsresult.property("name").toString().toStdString() << "\n";
    ss << "message: " << jsresult.property("message").toString().toStdString() << "\n";
    ss << "stack: " << jsresult.property("stack").toString().toStdString() << "\n";
    error = ss.str().c_str();
    std::cerr << ss.str() << "\n";
    return false;
  }

  result = engine.fromScriptValue<QJsonValue>(jsresult);
  std::cerr << "js result is: " << jsresult.toVariant().toString().toStdString() << "\n";
  std::cerr << "expression result is: " << result.toString().toStdString() << "\n";
  return true;
}

// Note when target is null or defined the value comes back without any merging.
QJsonValue JSONUtils::extract_value(const QJsonObject& src, const Path& src_path, const QJsonValue& target) {
  QJsonObject obj(src);
  Path path(src_path);

  if (path.empty()) {
    return obj;
  }

  while(!path.empty()) {
    // Grab an element of the path.
    std::string key = path.front();
    path.pop_front();

    // If the path is now empty, we're at the right location to extract the value.
    if (path.empty()) {
      QJsonValue value = obj.value(key.c_str());
      QJsonValue merged = JSONUtils::deep_merge(target, value);
      return merged;
    }

    // Make sure the object has something at this path_elem.
    if (!obj.contains(key.c_str())) {
      return QJsonValue();
    }

    // Make sure the object has this key.
    if (!obj.contains(key.c_str())) {
      return QJsonValue();
    }

    // Dive into the object one step.
    QJsonValue child = obj.value(key.c_str());

    // The child should always be an object.
    if (!child.isObject()) {
      return QJsonValue();
    }

    // Convert the child to an object.
    obj = child.toObject();
  }
  return QJsonValue();
}

QJsonObject JSONUtils::embed_value(const QJsonObject& obj, const Path& dest_path, const QJsonValue& dest_value) {
  if (dest_path.empty()) {
    return obj;
  }

  QString child_key(dest_path.front().c_str());

  // Base Case.
  if (dest_path.size() == 1) {
    QJsonObject result(obj);
    result.insert(child_key, dest_value);
    return result;
  }

  // Get the recursive object and dest path.
  QJsonObject child_obj = obj.value(child_key).toObject();
  Path child_path(dest_path);
  child_path.pop_front();

  // Recursive case.
  // Insert the result of a recursive call.
  QJsonObject result(obj);
  result.insert(child_key, embed_value(child_obj, child_path, dest_value));
  return result;
}

QJsonObject JSONUtils::erase_value(const QJsonObject& obj, const Path& path2) {
  if (path2.empty()) {
    return obj;
  }

  QString child_key(path2.front().c_str());

  // Base Case.
  if (path2.size() == 1) {
    QJsonObject result(obj);
    QJsonObject::iterator iter = result.find(child_key);
    if (iter != result.end()) {
      result.erase(iter);
    }
    return result;
  }

  // Make sure the object has the child_key, otherwise we can't find the location to erase.
  if (!obj.contains(child_key)) {
    return obj;
  }

  // Get the recursive object and dest path.
  QJsonObject child_obj = obj.value(child_key).toObject();
  Path child_path(path2);
  child_path.pop_front();

  // Recursive case.
  // Insert the result of a recursive call.
  QJsonObject result(obj);
  result.insert(child_key, erase_value(child_obj, child_path));
  return result;
}

}
