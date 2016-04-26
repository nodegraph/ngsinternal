#include <native/utils.h>

QUrl Utils::url_from_input(const QString& input)
{
    if (input.isEmpty())
        return QUrl::fromUserInput("about:blank");
    const QUrl result = QUrl::fromUserInput(input);
    return result.isValid() ? result : QUrl::fromUserInput("about:blank");
}
