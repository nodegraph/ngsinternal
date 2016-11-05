#pragma once
#include <components/compshapes/compshapes_export.h>

namespace ngs {

enum class HitRegion: int {
  kMissedRegion,
  kUnknownRegion,
  // Links.
  kLinkHeadRegion,
  kLinkTailRegion,
  // Plugs.
  kInputRegion,
  kOutputRegion,
  // Node Markers.
  kEditMarkerRegion,
  kViewMarkerRegion,
  kProcessingMarkerRegion,
  kCleanMarkerRegion,
  kErrorMarkerRegion,
  // Node Body.
  kNodeRegion, // include all node shapes ... dot nodes, input nodes, output nodes, etc

};

struct HitRegionHash {
  int operator()(HitRegion t) const {
    return static_cast<int>(t);
  }
};

}
