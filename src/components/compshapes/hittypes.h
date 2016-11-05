#pragma once
#include <components/compshapes/compshapes_export.h>

namespace ngs {

enum class HitRegion: int {
  kMissedRegion,
  kLinkHeadRegion,
  kLinkTailRegion,

  kInputShapeRegion,
  kOutputShapeRegion,

  kEditMarkerRegion,
  kViewMarkerRegion,
  kProcessingMarkerRegion,
  kCleanMarkerRegion,
  kErrorMarkerRegion,

  kNodeShapeRegion, // include all node shapes ... dot nodes, input nodes, output nodes, etc
  kShape,
};

struct HitRegionHash
{
    int operator()(HitRegion t) const
    {
        return static_cast<int>(t);
    }
};

}
