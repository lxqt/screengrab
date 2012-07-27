

#ifndef UPLOADER_IMGSHACK_H
#define UPLOADER_IMGSHACK_H

#include <uploader.h>

#include <QtCore/QUrl>

class Uploader_ImgShack : public Uploader
{
	Q_OBJECT
public:
    explicit Uploader_ImgShack(QObject* parent = 0);
    virtual ~Uploader_ImgShack();
    
    virtual void startUploading();
    
protected:
    virtual QUrl apiUrl();
//     virtual QByteArray createData();
    void createData();

protected Q_SLOTS:
    virtual void replyFinished(QNetworkReply* reply);
};

#endif // UPLOADER_IMGSHACK_H
