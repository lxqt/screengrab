#include "uploader_imgur_widget.h"
#include "ui_uploader_imgur_widget.h"

Uploader_ImgUr_Widget::Uploader_ImgUr_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Uploader_ImgUr_Widget)
{
    ui->setupUi(this);
}

Uploader_ImgUr_Widget::~Uploader_ImgUr_Widget()
{
    delete ui;
}

void Uploader_ImgUr_Widget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QVariantMap Uploader_ImgUr_Widget::settingsMap() const
{
	QVariantMap map;
	
	return map;
}
