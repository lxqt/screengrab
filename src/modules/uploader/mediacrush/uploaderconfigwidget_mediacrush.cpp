#include "uploaderconfigwidget_mediacrush.h"
#include "ui_uploaderconfigwidget_mediacrush.h"

UploaderConfigWidget_MediaCrush::UploaderConfigWidget_MediaCrush(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UploaderConfigWidget_MediaCrush)
{
    ui->setupUi(this);
}

UploaderConfigWidget_MediaCrush::~UploaderConfigWidget_MediaCrush()
{
    delete ui;
}
