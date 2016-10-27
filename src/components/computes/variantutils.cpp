

//// Use QVariant::toBool() to convert to a bool.
//bool Compute::variant_is_bool(const QVariant& value) {
//  is_static();
//  if (value.userType() == QMetaType::Bool) {
//    return true;
//  }
//  return false;
//}
//
//// Use QVariant::toList() to convert to a list.
//bool Compute::variant_is_list(const QVariant& value) {
//  is_static();
//  // For some reason the value.userType() call returns 1119 which doesn't match
//  // QMetaType::QVariantList or QMetaType::QStringList. So we use canConvert instead.
//  if (value.canConvert(QMetaType::QVariantList)) {
//    return true;
//  }
//  return false;
//}
//
//// Use QVariant::toMap() to convert to a map.
//bool Compute::variant_is_map(const QVariant& value) {
//  is_static();
//  if (value.userType() == QMetaType::QVariantMap) {
//    return true;
//  }
//  return false;
//}
//
//void Compute::merge_maps(QVariantMap& target, const QVariantMap& giver) {
//  is_static();
//  for (QVariantMap::const_iterator iter = giver.constBegin(); iter != giver.constEnd(); ++iter) {
//    target.insert(iter.key(), iter.value());
//  }
}

// Merges a source into the target
// The top level type of the target is maintained.
// However internally the target's content's types may change.
// It is up to the js script or c++ code to check for this.
// target <- source
// map <- map: merge source properties into target
// map <- !map: insert/override a "value" property into the target with the !map source value.
// list <- list: replaces target with source
// list <- map: wipes the target list and add the
// list <- !list: wipes the target list and adds the !list element from the source
// null <- any: target is assigned the source
// invalid <- any: target is assigned the source
// other <- any: target gets the source value if it can be convert to the target's type
//void Compute::merge(QVariant& target, const QVariant& source) {
//  if (variant_is_map(target)) {
//    QVariantMap target_map = target.toMap();
//    if (variant_is_map(source)) {
//      // Merge properties.
//      QVariantMap source_map = source.toMap();
//      merge_maps(target_map, source_map);
//    } else {
//      // Insert a value property.
//      target_map["value"] = source;
//    }
//    target = target_map;
//  } else if (variant_is_list(target)) {
//    QVariantList target_list = target.toList();
//    if (variant_is_list(source)) {
//      // Overwrite target list, because
//      // we consider the array length to be a property also.
//      QVariantList source_list = source.toList();
//      target_list = source_list;
//    } else if (variant_is_map(source)) {
//      QVariantMap source_map = source.toMap();
//      if (source_map.count("value")) {
//        target_list.clear();
//        target_list.push_back(source_map["value"]);
//      } else {
//        target_list.clear();
//        target_list.push_back(source);
//      }
//    } else {
//      // Wipe target list, and insert one source element
//      // because we consider the source to be an array of length one.
//      target_list.clear();
//      target_list.push_back(source);
//    }
//    target = target_list;
//  } else if (target.isNull()) {
//    target = source;
//  } else if (target.isValid()) {
//    target = source;
//  } else {
//    //
//    if (source.canConvert(target.type())) {
//      // Note even though canConvert may return true, when actually
//      // performing the convert it may fail. For example a string
//      // can theoretically convert to a number. But the string may
//      // contain non-numeric chars, in which case the conversion fails.
//      QVariant copy = source;
//      bool success = copy.convert(target.type());
//      if (success) {
//        target = copy;
//      }
//    }
//  }
//}

// Thie method returns a copy of the target but with source values/properties merged into it.
// A target property is only overridden if the property name and type matches exactly for objects and arrays.
// For primitive types it will try to convert the types.
// If the target does not contain a named property from the source, then it is inserted (regardless of type).
//QVariant Compute::deep_merge(const QVariant& target, const QVariant& source) {
//  if (variant_is_map(target)) {
//    if (variant_is_map(source)) {
//      QVariantMap tmap = target.toMap();
//      QVariantMap smap = source.toMap();
//      for (QVariantMap::const_iterator iter = smap.constBegin(); iter != smap.constEnd(); ++iter) {
//        QString key = iter.key();
//        if (tmap.count(key)) {
//          tmap[key] = deep_merge(tmap[key], smap[key]);
//        } else {
//          tmap[key] = smap[key];
//        }
//      }
//      return tmap;
//    } else {
//      // Otherwise source data doesn't make it into the target.
//      return target;
//    }
//  } else if (variant_is_list(target)) {
//    if (variant_is_list(source)) {
//      QVariantList tlist = target.toList();
//      QVariantList slist = source.toList();
//      QVariantList result;
//      // The first element of the target list acts as a prototype if present.
//      QVariant proto;
//      if (!tlist.isEmpty()) {
//        proto = tlist[0];
//      }
//      for (int i=0; i<slist.size(); ++i) {
//        if (proto.isValid()) {
//          result.push_back(deep_merge(proto, slist[i]));
//        } else {
//          result.push_back(slist[i]);
//        }
//      }
//      return result;
//    } else {
//      // Otherwise source data doesn't make it into the target.
//      return target;
//    }
//  } else if (target.isNull()) {
//    return source;
//  } else if (target.isValid()) {
//    return source;
//  } else {
//    if (source.canConvert(target.type())) {
//      // Note even though canConvert may return true, when actually
//      // performing the convert it may fail. For example a string
//      // can theoretically convert to a number. But the string may
//      // contain non-numeric chars, in which case the conversion fails.
//      QVariant copy = source;
//      bool success = copy.convert(target.type());
//      if (success) {
//        return copy;
//      }
//    }
//    return target;
//  }
//}


bool Compute::evaluate_expression_qml(const QString& text, QVariant& result, QString& error) const {
  internal();
  QQmlEngine engine;

  // Create a new context to run our javascript expression.
  QQmlContext eval_context(engine.rootContext());

  // Add the input values into the context.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    QVariant value = input->get_output("out");
    const std::string& input_name = input->get_name();
    eval_context.setContextProperty(input_name.c_str(), value);
  }

  // Create the expression.
  QQmlExpression expr(&eval_context, NULL, text);

  // Run the expression.
  // The way we set up the expression context, there can't be any side effects.
  // We only care about the return value.
  QVariant js_result = expr.evaluate();
  if (expr.hasError()) {
    error = expr.error().toString();
    qDebug() << "Error: expression has an error: " << expr.error().toString() << "\n";
    return false;
  }

  std::cerr << "js_result type: " << js_result.type() << " : " << js_result.typeName() << "\n";

  QVariant non_js_value;
  if (js_result.userType() == qMetaTypeId<QJSValue>()) {
    non_js_value = qvariant_cast<QJSValue>(js_result).toVariant();
  } else {
    non_js_value = js_result;
  }

  std::cerr << "non_js_result type: " << non_js_value.type() << " : " << js_result.typeName() << "\n";

  result = non_js_value;
  std::cerr << "expression result is: " << result.toString().toStdString() << "\n";
  return true;
}

