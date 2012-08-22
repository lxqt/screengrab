#ifndef UPLOADER_IMGSHACK_WIDGET_H
#define UPLOADER_IMGSHACK_WIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QVariant>

namespace Ui {
class Uploader_ImgShack_Widget;
}

class Uploader_ImgShack_Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Uploader_ImgShack_Widget(QWidget *parent = 0);
    ~Uploader_ImgShack_Widget();

public Q_SLOTS:
	QVariantMap settingsMap() const;
    
protected:
    void changeEvent(QEvent *e);
    
private:
    Ui::Uploader_ImgShack_Widget *ui;
};

#endif // UPLOADER_IMGSHACK_WIDGET_H
