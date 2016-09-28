#pragma once
#include <components/compshapes/compshapes_export.h>

namespace ngs {

enum COMPSHAPES_EXPORT HitRegion {
  kMissed,
  kLinkHead,
  kLinkTail,
  kInputShapeRegion,
  kOutputShapeRegion,
  kNodeShapeRegion, // include all node shapes ... dot nodes, input nodes, output nodes, etc
  kShape,
};

}
