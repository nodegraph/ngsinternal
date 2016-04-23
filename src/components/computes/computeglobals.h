#pragma once
#include <components/computes/computes_export.h>

class QObject;

namespace ngs {

// This is used by our computer classes to access the NodePoolAPI
// qml object on the qml side.
COMPUTES_EXPORT extern QObject* g_node_pool_api;

}
