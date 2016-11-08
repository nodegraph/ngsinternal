#include <qmqtt/qmqtt.h>
#include <QtCore/QCoreApplication>
#include <Qtcore/QTimer>

#include <iostream>

namespace ngs {


//const QHostAddress EXAMPLE_HOST("52.29.193.216"); // = "broker.hivemq.com";

const QHostAddress EXAMPLE_HOST = QHostAddress::LocalHost;
const quint16 EXAMPLE_PORT = 1883;
const QString EXAMPLE_TOPIC = "testing_mqtt/testing";
const QString USERNAME = "tester";
const QString PASSWORD = "booya";
const bool USE_AUTHENTICATION = false;

class Publisher : public QMQTT::Client {
Q_OBJECT
 public:
  explicit Publisher(const QHostAddress& host = EXAMPLE_HOST, const quint16 port = EXAMPLE_PORT, QObject* parent = NULL)
      : QMQTT::Client(host, port, parent),
        _number(0) {
    if (USE_AUTHENTICATION) {
      setUsername(USERNAME);
      setPassword(PASSWORD);
    }
    connect(this, &Publisher::connected, this, &Publisher::onConnected);
    connect(&_timer, &QTimer::timeout, this, &Publisher::onTimeout);
    connect(this, &Publisher::disconnected, this, &Publisher::onDisconnected);
  }
  virtual ~Publisher() {
  }

  QTimer _timer;
  quint16 _number;

 public slots:
  void onConnected() {
    std::cerr << "publisher connected\n";
    subscribe(EXAMPLE_TOPIC, 0);
    _timer.start(1000);
  }

  void onTimeout() {
    std::cerr << "publisher pulse work\n";
    QMQTT::Message message(_number, EXAMPLE_TOPIC, QString("Number is %1").arg(_number).toUtf8());
    publish(message);
    _number++;
    if (_number >= 10) {
      _timer.stop();
      disconnectFromHost();
    }
  }

  void onDisconnected() {
    std::cerr << "publisher disconnected\n";
    QTimer::singleShot(0, qApp, &QCoreApplication::quit);
  }
};

class Subscriber : public QMQTT::Client {
Q_OBJECT
 public:
  explicit Subscriber(const QHostAddress& host = EXAMPLE_HOST, const quint16 port = EXAMPLE_PORT, QObject* parent = NULL)
      : QMQTT::Client(host, port, parent) {
    if (USE_AUTHENTICATION) {
      setUsername(USERNAME);
      setPassword(PASSWORD);
    }
    connect(this, &Subscriber::connected, this, &Subscriber::onConnected);
    connect(this, &Subscriber::subscribed, this, &Subscriber::onSubscribed);
    connect(this, &Subscriber::received, this, &Subscriber::onReceived);
  }
  virtual ~Subscriber() {
  }

 public slots:
  void onConnected() {
    std::cerr << "subscriber connected" << "\n";
    subscribe(EXAMPLE_TOPIC, 0);
  }

  void onSubscribed(const QString& topic) {
    std::cerr << "subscriber subscribed to topic: " << topic.toStdString() << "\n";
  }

  void onReceived(const QMQTT::Message& message) {
    std::cerr << "publisher received: \"" << QString::fromUtf8(message.payload()).toStdString() << "\"" << "\n";
  }
};

}
