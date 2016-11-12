#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/utils/enumutil.h>

#include <QtCore/QString>


namespace ngs {

class Path;

COMMS_EXPORT QStringList path_to_string_list(const Path& path);
COMMS_EXPORT Path string_list_to_path(const QStringList& list);

}
