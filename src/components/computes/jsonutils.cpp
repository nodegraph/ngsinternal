#include <components/computes/jsonutils.h>

#include <sstream>
#include <iostream>

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlExpression>

#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>

#include <QtCore/QJsonValue>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include <QtCore/QString>
#include <QtCore/QStringList>


namespace ngs {

QJsonValue JSONUtils::convert_to_bool(const QJsonValue& source) {
  switch (source.type()) {
    case QJsonValue::Null: {
      return QJsonValue();
      break;
    }
    case QJsonValue::Undefined: {
      return QJsonValue();
      break;
    }
    case QJsonValue::Bool: {
      return source;
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
        // If the object has more than one property, we convert a property named "value".
        QJsonObject::iterator iter = obj.find("value");
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

QJsonValue JSONUtils::convert_to_double(const QJsonValue& source) {
  switch (source.type()) {
    case QJsonValue::Null: {
      return QJsonValue();
      break;
    }
    case QJsonValue::Undefined: {
      return QJsonValue();
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
      return source;
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
        // If the object has more than one property, we convert a property named "value".
        QJsonObject::iterator iter = obj.find("value");
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

QJsonValue JSONUtils::convert_to_string(const QJsonValue& source) {
  switch (source.type()) {
    case QJsonValue::Null: {
      return QJsonValue();
      break;
    }
    case QJsonValue::Undefined: {
      return QJsonValue();
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
      return source;
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
        // If the object has more than one property, we convert a property named "value".
        QJsonObject::iterator iter = obj.find("value");
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
      obj.insert("value", source);
      return obj;
      break;
    }
    case QJsonValue::Double: {
      QJsonObject obj;
      obj.insert("value", source);
      return obj;
      break;
    }
    case QJsonValue::String: {
      QJsonObject obj;
      obj.insert("value", source);
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
      QJsonObject result;
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
    target.insert(iter.key(), iter.value());
  }
}

//void JSONUtils::prep_source_for_merge(const QJsonValue& target, QJsonValue& source) {
//  if (source.isString()) {
//    QJsonValue value = eval_js2(source.toString());
//    if (target.isObject()) {
//      if (value.isObject()) {
//        // Conver the source from a string to an object.
//        source = value;
//        return;
//      }
//    } else if (target.isArray()) {
//      if (value.isArray()) {
//        // Convert the source from a string to an array.
//        source = value;
//        return;
//      }
//    }
//  } else if (source.isObject()) {
//    if (target.isString()) {
//      // Convert the source from an object to a string.
//      QString json = value_to_json(source);
//      source = QJsonValue(json);
//    }
//  } else if (source.isArray()) {
//    if (target.isString()) {
//      // Conver the source from an array to a string.
//      QString json = value_to_json(source);
//      source = QJsonValue(json);
//    }
//  }
//}



//QJsonValue JSONUtils::deep_merge(const QJsonValue& target, const QJsonValue& source) {
//  if (source.isNull()) {
//    return target;
//  } else if (source.isUndefined()) {
//    return target;
//  }else if (target.isObject()) {
//    if (source.isObject()) {
//      QJsonObject tobj = target.toObject();
//      QJsonObject sobj = source.toObject();
//      for (QJsonObject::const_iterator siter = sobj.constBegin(); siter != sobj.constEnd(); ++siter) {
//        QString name = siter.key();
//        if (tobj.contains(name)) {
//          tobj.insert(name, deep_merge(tobj[name], siter.value()));
//        } else {
//          tobj.insert(name, siter.value());
//        }
//      }
//      return tobj;
//    } else {
//      // Otherwise source data doesn't make it into the target.
//      return target;
//    }
//  } else if (target.isArray()) {
//    if (source.isArray()) {
//      QJsonArray tarr = target.toArray();
//      QJsonArray sarr = source.toArray();
//      // The first element of the target list acts as a prototype if present.
//      QJsonValue proto;
//      if (!tarr.empty()) {
//        proto = tarr.at(0);
//      }
//      QJsonArray result;
//      for (int i = 0; i < sarr.size(); ++i) {
//        result[i] = deep_merge(proto, sarr.at(i));
//      }
//      return result;
//    } else {
//      // Otherwise source data doesn't make it into the target.
//      return target;
//    }
//  } else if (target.isNull()) {
//    return source;
//  } else if (target.isUndefined()) {
//    return source;
//  } else if (target.isBool()) {
//    if (source.isBool()) {
//      return source;
//    } else {
//      return target;
//    }
//  } else if (target.isDouble()) {
//    if (source.isDouble()) {
//      return source;
//    } else {
//      return target;
//    }
//  } else if (target.isString()) {
//    if (source.isString()) {
//      return source;
//    } else {
//      return target;
//    }
//  }
//  return target;
//}





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


}
