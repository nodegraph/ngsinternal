#pragma once
#include <components/computes/computes_export.h>

namespace ngs {

// In lighting node graphs there will only be one type of data flowing through the node graph.
// In shader node graphs there will be much more, but we will add those when we get there.
enum COMPUTES_EXPORT PlugDataType {
	kQVariant = 0, // holds a javascript object value as a QVariant.
};

}








