/***************************************************************************
 *   Copyright (C) 2009 - 2012 by Artem 'DOOMer' Galichkin                        *
 *   doomer3d@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "uploader.h"
#include "core/core.h"

#include <QtCore/QDir>
#include <QtCore/QUuid>
#include <QtCore/QRegExp>

#include <QDebug>

Uploader::Uploader(QObject *parent) :
    QObject(parent)
{
    qDebug() << "creating base uploader";
    _strBoundary = "uploadbound";
    _net = new QNetworkAccessManager(this);
	serverReply = 0;
	initUploadedStrList();
}

Uploader::~Uploader()
{
    qDebug() << " base uploader killed";
}

/*!
 * Create boundary string
 * \param cleared -detect for strt boyndary lin in httpRequest
 */
QByteArray Uploader::boundary(bool cleared)
{
    QByteArray retBoundary = _strBoundary;
    
    if (cleared == false)
    {
        retBoundary.append("\r\n");
        retBoundary.prepend("--");
    }
    
    return retBoundary;
}

/*!
 *  Generate filename for upload file
 */
QString Uploader::createFilename(QString& format)
{
    QString tmpFileName = QUuid::createUuid().toString();
    int size = tmpFileName.size() - 2;
    tmpFileName = tmpFileName.mid(1, size).left(8);
    
    tmpFileName = QDir::tempPath() + QDir::separator() + "screenshot-" + tmpFileName + "." + format;  
    
    return tmpFileName;
}

void Uploader::replyProgress(qint64 bytesSent, qint64 bytesTotal)
{
    Q_EMIT uploadProgress(bytesSent, bytesTotal);
}
    
/*!
 *  Get suser selected params from uploader widget (not from config file)
 */
void Uploader::getUserSettings(const QVariantMap& settings)
{
	_userSettings = settings;	
}
    
/*!
 *  Start uploadingin base class 
 */
void Uploader::startUploading()
{
	connect(_net, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	serverReply = _net->post(_request, imageData);	

	connect(serverReply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(replyProgress(qint64,qint64))); 
}

QMap< QByteArray, ResultString_t > Uploader::parsedLinks()
{
	return _uploadedStrings;
}


/*!
 * 	Return url for upload image
 */
QUrl Uploader::apiUrl()
{
    return QUrl();
}

/*!
 * 	Prepare image datafor uploading
 */
void Uploader::createData(bool inBase64)
{
    Core *core = Core::instance();
    _formatString = core->conf->getSaveFormat();
    _uploadFilename = createFilename(_formatString);
    core->writeScreen(_uploadFilename, _formatString , true);
    
	if (inBase64 == false)
	{
		imageData = core->getScreen();
	}
	else
	{
		imageData = core->getScreen().toBase64();
	}
}

/*!
 *  Create request for send to server.
 *  this method called from subclasses.
 */
void Uploader::createRequest(const QByteArray& requestData, const QUrl url)
{
    _request.setUrl(url);
}

/*!
 *  Parsing server reply and get map with server returned links
 *  \param keytags List of tags for parsing
 *  \param result String wuth server reply
 */
QMap<QByteArray, QByteArray> Uploader::parseResultStrings(const QVector< QByteArray >& keytags, const QByteArray& result)
{
	QMap<QByteArray, QByteArray> replyMap;
	
	QRegExp re;      
	QRegExp re2;
	
	int inStart = 0;
    int outStart = 0;
	int len = 0;
	
        // parsing xml 
    for (int i = 0; i < keytags.count(); ++i)
	{
        // set patterns  for full lenght item
        re.setPattern("<"+keytags[i]+">"); // open tag
        re2.setPattern("</"+keytags[i]+">"); //close tag
            
        // get start pos and lenght ite in xml
        inStart = re.indexIn(result); // ops open tag start
        outStart = re2.indexIn(result); // pos close tag start
        len = outStart - inStart + re2.matchedLength(); // length of full string
            
            // extract item and replase spec html sumbols
        QByteArray extractedText = result.mid(inStart, len);
        extractedText = extractedText.replace("&quot;","'");
        extractedText = extractedText.replace("&lt;","<");
        extractedText = extractedText.replace("&gt;",">");
        extractedText = extractedText.replace("<"+keytags[i]+">","");
        extractedText = extractedText.replace("</"+keytags[i]+">","");
            
			// to map
		replyMap.insert(keytags[i], extractedText);
			
// 		keytags[i] = extractedText;
	}
	
	return replyMap;
}

void Uploader::initUploadedStrList()
{
	qDebug() << "initialize final strings list";
	ResultString_t strPair = qMakePair(QByteArray(), tr("Direct link"));
	this->_uploadedStrings.insert(UL_DIRECT_LINK, strPair);
	
	strPair = qMakePair(QByteArray(), tr("HTML code"));
	this->_uploadedStrings.insert(UL_HTML_CODE ,strPair);
	
	strPair = qMakePair(QByteArray(), tr("BB code"));
	this->_uploadedStrings.insert(UL_BB_CODE, strPair);
	
	strPair = qMakePair(QByteArray(), tr("HTML code with thumb image"));
	this->_uploadedStrings.insert(UL_HTML_CODE_THUMB ,strPair);
	
	strPair = qMakePair(QByteArray("bb_code_thumb"), tr("BB code with thumb image"));
	this->_uploadedStrings.insert(UL_BB_CODE_THUMB, strPair);
}
