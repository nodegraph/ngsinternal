#include <native/javabridge.h>

#include <QtCore/QDebug>

namespace ngs {

JavaBridge::JavaBridge(QObject *parent)
    : QObject(parent) {
}

void JavaBridge::acquire_activity() {
  // Make sure we have a wrapper around the our custom java activity.
  if (!_activity.isValid()) {
    _activity = QAndroidJniObject::callStaticObjectMethod("com/youmacro/app/JavaBridgeImp",
                                              "get_java_bridge",
                                              "()Lcom/youmacro/app/JavaBridgeImp;");
  }
}

void JavaBridge::notify(const QString& title, const QString& content) {
  acquire_activity();

  // Wrap the arguments.
  QAndroidJniObject jni_title = QAndroidJniObject::fromString(title);
  QAndroidJniObject jni_content = QAndroidJniObject::fromString(content);

  // If we have the activity, we can call some member methods on the activity.
  if (_activity.isValid()) {
    _activity.callMethod<void>("notify",
                            "(Ljava/lang/String;Ljava/lang/String;)V",
                            jni_title.object<jstring>(),
                            jni_content.object<jstring>());
  }
}

void JavaBridge::vibrate(long milli_secs) {
  acquire_activity();

  if (_activity.isValid()) {
    // Note the jlong cast is important, otherwise a huge (usually) negative is
    // sent over to the java side.
    _activity.callMethod<void>("vibrate", "(J)V", (jlong)milli_secs);
  }
}

void JavaBridge::dismiss_keyboard_from_webview() {
  acquire_activity();
  if (_activity.isValid()) {
    // Note the jlong cast is important, otherwise a huge (usually) negative is
    // sent over to the java side.
    _activity.callMethod<void>("dismiss_keyboard_from_webview");
  }
}

}
