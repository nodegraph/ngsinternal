#include <QtWidgets/QApplication>
#include <QtQml/QQmlApplicationEngine>

#if ARCH == ARCH_IOS
  #include <QQmlExtensionPlugin>
  Q_IMPORT_PLUGIN(QtQuick2Plugin)
  Q_IMPORT_PLUGIN(QtQuickControlsPlugin)
  Q_IMPORT_PLUGIN(QtQuick2DialogsPlugin)
  Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)

  //Q_IMPORT_PLUGIN(DialogPlugin)
  //Q_IMPORT_PLUGIN(WindowPlugin)
  Q_IMPORT_PLUGIN(QmlFolderListModelPlugin)
  Q_IMPORT_PLUGIN(QmlSettingsPlugin)
  //Q_IMPORT_PLUGIN(DialogsPrivatePlugin)
  Q_IMPORT_PLUGIN(QtQuickLayoutsPlugin)
  Q_IMPORT_PLUGIN(QtQuickExtrasPlugin)
  //Q_IMPORT_PLUGIN(QtGraphicalEffectsPrivate)
  //Q_IMPORT_PLUGIN(ModelsPlugin)


  Q_IMPORT_PLUGIN(QtQuick2DialogsPrivatePlugin)
  Q_IMPORT_PLUGIN(QtQmlModelsPlugin)
  Q_IMPORT_PLUGIN(QtQuick2PrivateWidgetsPlugin)

//Q_IMPORT_PLUGIN( qtlabscalendarplugin)
//Q_IMPORT_PLUGIN( qtlabscontrolsplugin)
//Q_IMPORT_PLUGIN( qtlabsmaterialstyleplugin)
//Q_IMPORT_PLUGIN( qtlabsuniversalstyleplugin)
//Q_IMPORT_PLUGIN( qmlfolderlistmodelplugin)
//Q_IMPORT_PLUGIN( qmlsettingsplugin)
//Q_IMPORT_PLUGIN( qtlabstemplatesplugin)
//Q_IMPORT_PLUGIN( quick3dcoreplugin)
//Q_IMPORT_PLUGIN( quick3dinputplugin)
//Q_IMPORT_PLUGIN( quick3dlogicplugin)
//Q_IMPORT_PLUGIN( quick3drenderplugin)
//Q_IMPORT_PLUGIN( declarative_audioengine)
//Q_IMPORT_PLUGIN( declarative_bluetooth)
//Q_IMPORT_PLUGIN( qtcanvas3d)
//Q_IMPORT_PLUGIN( qtgraphicaleffectsprivate)
//Q_IMPORT_PLUGIN( declarative_location)
//Q_IMPORT_PLUGIN( declarative_multimedia)
//Q_IMPORT_PLUGIN( declarative_nfc)
//Q_IMPORT_PLUGIN( declarative_positioning)
//Q_IMPORT_PLUGIN( modelsplugin)
//Q_IMPORT_PLUGIN( qtqmlstatemachine)
//Q_IMPORT_PLUGIN( qtquickcontrolsplugin)
//Q_IMPORT_PLUGIN( qtquickextrasflatplugin)
//Q_IMPORT_PLUGIN( dialogplugin)
//Q_IMPORT_PLUGIN( dialogsprivateplugin)
//Q_IMPORT_PLUGIN( qtquickextrasplugin)
//Q_IMPORT_PLUGIN( qquicklayoutsplugin)
//Q_IMPORT_PLUGIN( qmllocalstorageplugin)
//Q_IMPORT_PLUGIN( particlesplugin)
//Q_IMPORT_PLUGIN( widgetsplugin)
//Q_IMPORT_PLUGIN( qtquickscene3dplugin)
//Q_IMPORT_PLUGIN( windowplugin)
//Q_IMPORT_PLUGIN( qmlxmllistmodelplugin)
//Q_IMPORT_PLUGIN( qtquick2plugin)
//Q_IMPORT_PLUGIN( declarative_sensors)
//Q_IMPORT_PLUGIN( qmltestplugin)
//Q_IMPORT_PLUGIN( declarative_webchannel)
//Q_IMPORT_PLUGIN( declarative_qmlwebsockets)
//Q_IMPORT_PLUGIN( declarative_webview)
#endif

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  engine.addImportPath("qrc:/qml");
  engine.addImportPath("qrc:/");

#if ARCH == ARCH_IOS
  // Register QML types.
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2Plugin().instance())->registerTypes("QtQuick");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2PrivateWidgetsPlugin().instance())->registerTypes("QtQuick.PrivateWidgets");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->registerTypes("QtQuick.Controls");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->registerTypes("QtQuick.Controls.Private");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->registerTypes("QtQuick.Controls.Styles");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2DialogsPlugin().instance())->registerTypes("QtQuick.Dialogs");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2DialogsPrivatePlugin().instance())->registerTypes("QtQuick.Dialogs.Private");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickLayoutsPlugin().instance())->registerTypes("QtQuick.Layouts");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2WindowPlugin().instance())->registerTypes("QtQuick.Window");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickExtrasPlugin().instance())->registerTypes("QtQuick.Extras");

  // Initialize engine with these QML types.

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2Plugin().instance())->initializeEngine(&engine, "QtQuick");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2PrivateWidgetsPlugin().instance())->initializeEngine(&engine, "QtQuick.PrivateWidgets");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->initializeEngine(&engine, "QtQuick.Controls");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->initializeEngine(&engine,
                                                                                                            "QtQuick.Controls.Private");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->initializeEngine(&engine,
                                                                                                            "QtQuick.Controls.Styles");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2DialogsPlugin().instance())->initializeEngine(&engine, "QtQuick.Dialogs");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2DialogsPrivatePlugin().instance())->initializeEngine(&engine, "QtQuick.Dialogs.Private");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickLayoutsPlugin().instance())->initializeEngine(&engine, "QtQuick.Layouts");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2WindowPlugin().instance())->initializeEngine(&engine, "QtQuick.Window");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickExtrasPlugin().instance())->initializeEngine(&engine, "QtQuick.Extras");

  //qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtGraphicalEffectsPlugin().instance())->registerTypes("QtGraphicalEffects.private");
  //qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtGraphicalEffectsPlugin().instance())->initializeEngine(&engine, "QtQraphicalEffects");
#endif



    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    return app.exec();
}

