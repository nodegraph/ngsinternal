#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/computes/baseprocess.h>
#include <guicomponents/comms/messagetypes.h>

#include <QtCore/QProcess>
#include <QtCore/QObject>

namespace ngs {


// This class communicates with the nodejs process.
class GUICOMPUTES_EXPORT DownloadVideoProcess : public BaseProcess {
Q_OBJECT
 public:
  explicit DownloadVideoProcess();
  virtual ~DownloadVideoProcess();
  int get_id() const;

  void set_url(const QString& url);
  void set_dir(const QString& dir);
  void set_max_width(int w);
  void set_max_height(int h);
  void set_max_filesize(int s);

  virtual void start();
  virtual const QString& get_filename() const;

  signals:
  void started(const QString& filename);
  void progress(const  QString& progress);
  void finished();

 protected:

 protected slots:
  virtual void on_state_changed(QProcess::ProcessState);
  virtual void on_stderr();
  virtual void on_stdout();

 protected:
  static int next_id;
  const int _id;

  bool _started;

  QString _url;
  QString _dir;
  int _max_width; // in pixels
  int _max_height; // in pixels
  int _max_filesize; // in mega bytes

  QString _filename; // This is sent to us over stdout from the process.
  static const char* _already_downloaded_pattern;
  static const char* _progress_pattern;
  static const char* _filename_pattern;
  const QRegExp _already_downloaded_regex;
  const QRegExp _progress_regex; // Extract progress info from stdout.
  const QRegExp _filename_regex; // Extract filename info from stdout.
};

}

