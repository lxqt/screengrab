#include "uploader_mediacrush_widget.h"
#include "ui_uploader_mediacrush_widget.h"

Uploader_MediaCrush_Widget::Uploader_MediaCrush_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Uploader_MediaCrush_Widget)
{
    ui->setupUi(this);
}

Uploader_MediaCrush_Widget::~Uploader_MediaCrush_Widget()
{
    delete ui;
}
