#include "exteditconfigwidget.h"
#include "ui_exteditconfigwidget.h"

ExtEditConfigWidget::ExtEditConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExtEditConfigWidget)
{
    ui->setupUi(this);
}

ExtEditConfigWidget::~ExtEditConfigWidget()
{
    delete ui;
}

void ExtEditConfigWidget::changeEvent(QEvent *e)
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
