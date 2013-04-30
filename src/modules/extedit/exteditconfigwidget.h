#ifndef EXTEDITCONFIGWIDGET_H
#define EXTEDITCONFIGWIDGET_H

#include <QtGui/QWidget>

namespace Ui {
class ExtEditConfigWidget;
}

class ExtEditConfigWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ExtEditConfigWidget(QWidget *parent = 0);
    ~ExtEditConfigWidget();
    
protected:
    void changeEvent(QEvent *e);
    
private:
    Ui::ExtEditConfigWidget *ui;
};

#endif // EXTEDITCONFIGWIDGET_H
