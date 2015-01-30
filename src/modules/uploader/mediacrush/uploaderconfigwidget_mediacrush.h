#ifndef UPLOADERCONFIGWIDGET_MEDIACRUSH_H
#define UPLOADERCONFIGWIDGET_MEDIACRUSH_H

#include <QWidget>

namespace Ui {
class UploaderConfigWidget_MediaCrush;
}

class UploaderConfigWidget_MediaCrush : public QWidget
{
    Q_OBJECT

public:
    explicit UploaderConfigWidget_MediaCrush(QWidget *parent = 0);
    ~UploaderConfigWidget_MediaCrush();

public Q_SLOTS:
    void saveSettings();

private:
    Ui::UploaderConfigWidget_MediaCrush *ui;
};

#endif // UPLOADERCONFIGWIDGET_MEDIACRUSH_H
