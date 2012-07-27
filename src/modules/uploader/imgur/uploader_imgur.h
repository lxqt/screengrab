

#ifndef UPLOADER_IMGUR_H
#define UPLOADER_IMGUR_H

#include <uploader.h>

#include <QtCore/QUrl>

namespace UploadImgUr {
	enum Error {
		ErrorFile,
		ErrorNetwork,
		ErrorCredits,
		ErrorUpload,
		ErrorCancel,
	};
};

class Uploader_ImgUr : public Uploader
{
	Q_OBJECT
public:
    explicit Uploader_ImgUr(QObject* parent = 0);
    virtual ~Uploader_ImgUr();
    
    virtual void startUploading();
    
protected:
    virtual QUrl apiUrl();
    virtual void createData();
	
protected Q_SLOTS:
    virtual void replyFinished(QNetworkReply* reply);
};

#endif // UPLOADER_IMGUR_H
 
