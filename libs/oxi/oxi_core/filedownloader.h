#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject
{
 Q_OBJECT
 public:
  explicit FileDownloader(QUrl imageUrl, QObject *parent = 0);
  virtual ~FileDownloader();
  QByteArray downloadedData() const;
  void RequestGet(QUrl url);
  void RequestPost(QUrl url);

 signals:
  void downloaded();

 private slots:
  void onRedirected(const QUrl&);
  void onFinished(QNetworkReply* pReply);
  private:
  QNetworkAccessManager m_WebCtrl;
  QNetworkRequest request;
  QByteArray m_DownloadedData;
  QNetworkCookieJar *cookieJar;
  QNetworkReply * reply_;
  QUrl storedUrl;
};

#endif // FILEDOWNLOADER_H
