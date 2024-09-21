#include "filedownloader.h"

FileDownloader::FileDownloader(QUrl Url, QObject *parent) :
    QObject(parent)
{
    connect(
                &m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
                this, SLOT (onFinished(QNetworkReply*))
                );
    // connect( &m_WebCtrl, SIGNAL(urlChanged (const QUrl&)), this, SLOT(onRedirected(const QUrl&)));

    m_WebCtrl.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    //    QNetworkRequest request(Url);
    //    RequestGet(Url);
}

FileDownloader::~FileDownloader() { }

void FileDownloader::RequestGet(QUrl url) {
    storedUrl = url;
    request.setUrl(url);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration()); // Set default ssl config

    reply_ = m_WebCtrl.head(request);


//    cookieJar = m_WebCtrl.cookieJar();
    if (!cookieJar) {
//        cookieJar = new QNetworkCookieJar(m_WebCtrl);
//        m_WebCtrl.setCookieJar(cookieJar);
    }
}

void FileDownloader::RequestPost(QUrl url) {
    storedUrl = url;
    request.setUrl(url);
    m_WebCtrl.post(request, QByteArray("some data"));
}

void FileDownloader::onRedirected(const QUrl& redirectedUrl) {
    qDebug() << "URL redirected to:" << redirectedUrl.toString();
}

void FileDownloader::onFinished(QNetworkReply* reply) {

    reply = reply_;
    QList<QByteArray> headerList = reply->rawHeaderList();

    foreach(QByteArray head, headerList)
    {
        qDebug() << head << ":" << reply->rawHeader(head);
    }


    QNetworkReply::NetworkError error = reply->error();
    //    if (error == QNetworkReply::NoError)
    {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        switch (statusCode) {
        case 301:
        case 302:
        case 303:
        case 307:
        {
            QString url = reply->header(QNetworkRequest::LocationHeader).toString();
            QUrl qurl(url);


            QString url_ = reply->header(QNetworkRequest::LocationHeader).toString();
            QUrl newUrl = QUrl::fromUserInput(url_);


            QUrl redirectedUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (redirectedUrl.isValid()) {
                qDebug() << "URL redirected to:" << redirectedUrl.toString();

                // Send a new request to the redirected URL
                RequestGet(redirectedUrl);
            } else if (redirectedUrl.isRelative()) {
                redirectedUrl = storedUrl.resolved(redirectedUrl);
                RequestGet(redirectedUrl);
            } else {
                qDebug() << "Error: Redirect target URL not valid";
            }
        }
            break;

        case 200:
        {
            QByteArray data = reply->readAll();
            qDebug() << "Response from original URL:" << data;
            emit downloaded();
        }
            break;
        }
    }



    //emit a signal
    reply->deleteLater();

}

QByteArray FileDownloader::downloadedData() const {
    return m_DownloadedData;
}
