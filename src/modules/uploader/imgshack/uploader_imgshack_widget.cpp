#include "uploader_imgshack_widget.h"
#include "ui_uploader_imgshack_widget.h"

#include <QDebug>

Uploader_ImgShack_Widget::Uploader_ImgShack_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Uploader_ImgShack_Widget)
{
    ui->setupUi(this);
	
	QString warningTitle = tr("Warning!");
	QString warningText =  tr("Resize makes on servers imageshack.us");
	ui->labResizeWarning->setText("<font color='red'><b>" + warningTitle + "</b></font><br />" + warningText);	
}

Uploader_ImgShack_Widget::~Uploader_ImgShack_Widget()
{
    delete ui;
}

void Uploader_ImgShack_Widget::changeEvent(QEvent *e)
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

QVariantMap Uploader_ImgShack_Widget::settingsMap() const
{
	QVariantMap map;

	map.insert("resize", ui->cbxResize->currentIndex() - 1);
	map.insert("anonimous", ui->checkAnonimusUpload->isChecked());
	
	return map;
}
