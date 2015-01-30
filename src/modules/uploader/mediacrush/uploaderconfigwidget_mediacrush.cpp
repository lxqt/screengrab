#include "uploaderconfigwidget_mediacrush.h"
#include "ui_uploaderconfigwidget_mediacrush.h"

#include "uploaderconfig.h"

#include <QDebug>

UploaderConfigWidget_MediaCrush::UploaderConfigWidget_MediaCrush(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UploaderConfigWidget_MediaCrush)
{
    ui->setupUi(this);

    // load settings
    UploaderConfig config;

    QVariantMap loadedValues;
    loadedValues.insert(KEY_MCSH_URL, "");

    loadedValues = config.loadSettings("mediacru.sh", loadedValues);
    ui->editUrl->setText(loadedValues[KEY_MCSH_URL].toString());

}

UploaderConfigWidget_MediaCrush::~UploaderConfigWidget_MediaCrush()
{
    delete ui;
}

void UploaderConfigWidget_MediaCrush::saveSettings()
{
    UploaderConfig config;

    QVariantMap savingValues;
    savingValues.insert(KEY_MCSH_URL, ui->editUrl->text());

    config.saveSettings("mediacru.sh", savingValues);
}
