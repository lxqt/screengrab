#ifndef UPLOADER_MEDIACRUSH_WIDGET_H
#define UPLOADER_MEDIACRUSH_WIDGET_H

#include <QWidget>

namespace Ui {
class Uploader_MediaCrush_Widget;
}

class Uploader_MediaCrush_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Uploader_MediaCrush_Widget(QWidget *parent = 0);
    ~Uploader_MediaCrush_Widget();

private:
    Ui::Uploader_MediaCrush_Widget *ui;
};

#endif // UPLOADER_MEDIACRUSH_WIDGET_H
