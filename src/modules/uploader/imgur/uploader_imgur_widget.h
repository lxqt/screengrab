#ifndef UPLOADER_IMGUR_WIDGET_H
#define UPLOADER_IMGUR_WIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QVariant>

namespace Ui {
class Uploader_ImgUr_Widget;
}

class Uploader_ImgUr_Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Uploader_ImgUr_Widget(QWidget *parent = 0);
    ~Uploader_ImgUr_Widget();

public Q_SLOTS:
	QVariantMap settingsMap() const;
    
protected:
    void changeEvent(QEvent *e);
    
private:
    Ui::Uploader_ImgUr_Widget *ui;
};

#endif // UPLOADER_IMGUR_WIDGET_H
