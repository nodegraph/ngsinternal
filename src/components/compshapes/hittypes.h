#pragma once
#include <components/compshapes/compshapes_export.h>

namespace ngs {

enum COMPSHAPES_EXPORT HitRegion {
  kMissed,
  kLinkHead,
  kLinkTail,
  kEditButton,
  kViewButton,
  kShape,
  kLeftEdge,
  kRightEdge,
  kBottomEdge,
  kTopEdge,
  kBottomLeftCorner,
  kBottomRightCorner,
  kTopLeftCorner,
  kTopRightCorner
};

}
