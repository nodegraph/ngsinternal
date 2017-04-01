#pragma once
#include <guicomponents/quick/quick_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/dep.h>

#include <QtQuick/QQuickView>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>


class QSGTexture;

namespace ngs {

class QUICK_EXPORT NodeGraphView: public QQuickView, public Component {
  Q_OBJECT
 public:

  COMPONENT_ID(NodeGraphView, NodeGraphView);

  NodeGraphView(Entity* entity);
  virtual ~NodeGraphView();

  // Overrides.
  QSGTexture* create_texture_from_id(uint id, const QSize &size, CreateTextureOptions options = CreateTextureOption()) const;

  Q_INVOKABLE static bool app_update_is_available();
  Q_INVOKABLE static void close_splash_page();

  Q_INVOKABLE void start_app_update();
  Q_INVOKABLE bool update_is_starting();

  Q_INVOKABLE void report_app_usage();
  Q_INVOKABLE void report_ng_usage();

 private slots:
   void on_app_usage_read();
   void on_app_usage_write();

   void on_ng_usage_read();
   void on_ng_usage_write();

 private:

   void send_get_request(const QString& url, const char* slot);
   void send_put_request(const QString& url, const QByteArray& data, const char* slot);

   bool read_number_fron_reply(QNetworkReply *reply, long& number);

  static const QString kAppUsageURL;
  static const QString kNGUsageURL;

  QNetworkAccessManager _nam;
  bool _app_usage_reported;
  bool _ng_usage_reported;


  bool _update_is_starting;

};

}
