 


#include "uploader_imgur.h"

#include <QDebug>

Uploader_ImgUr::Uploader_ImgUr(QObject* parent): Uploader(parent)
{
    qDebug() << " create img Ur uploader";	
}

Uploader_ImgUr::~Uploader_ImgUr()
{
    qDebug() << " kill img Ur uploader";
}

/*!
 * 	Start upload process
 */
void Uploader_ImgUr::startUploading()
{	
    createData();
	createRequest(imageData, apiUrl());
	
	this->_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	
    Uploader::startUploading();
}

/*!
 * 	Return url for upload image
 */
QUrl Uploader_ImgUr::apiUrl()
{
    return QUrl("http://api.imgur.com/2/upload");
}

/*!
 * 	Prepare image datafor uploading
 */
void Uploader_ImgUr::createData()
{
    qDebug() << "create data in imagUR";
	bool toBase64 = true;
    Uploader::createData(toBase64);
	
	// create data for upload 
    QByteArray uploadData;
	
	uploadData.append(QString("key=").toUtf8());
	uploadData.append(QUrl::toPercentEncoding("6920a141451d125b3e1357ce0e432409"));
	uploadData.append(QString("&image=").toUtf8());
	uploadData.append(QUrl::toPercentEncoding(this->imageData));
	
	this->imageData = uploadData;	
}

/*!
 * 	Process server reply data
 */
void Uploader_ImgUr::replyFinished(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray replyXmalText = reply->readAll();

        //  creating list of element names
		QVector<QByteArray> listXmlNodes;
        listXmlNodes << "original" << "imgur_page" << "large_thumbnail" << "small_square";
        
		QMap<QByteArray, QByteArray> replyXmlMap = parseResultStrings(listXmlNodes, replyXmalText);
		qDebug() << "original ";
		qDebug() << replyXmlMap["original"];
		
		this->_uploadedStrings[UL_DIRECT_LINK].first = replyXmlMap["original"];
		this->_uploadedStrings[UL_HTML_CODE].first = "<img src=\"" + replyXmlMap["original"] + "\" />";
		this->_uploadedStrings[UL_BB_CODE].first = "[img]" + replyXmlMap["original"] +"[/img]";
		this->_uploadedStrings[UL_HTML_CODE_THUMB].first = "<a href=\"" + replyXmlMap["original"] + "\"><img src=\"" + replyXmlMap["small_square"] + "\" /></a>";
		this->_uploadedStrings[UL_BB_CODE_THUMB].first = "[url=" + replyXmlMap["original"] + "][img]"+ replyXmlMap["original"] +"[/img][/url]";
		
		Q_EMIT uploadDone();
	}
	else
	{
		Q_EMIT uploadFail(reply->errorString().toAscii());
	}
	
	reply->deleteLater();
}
