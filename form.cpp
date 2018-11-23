#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

void Form::setPic(QImage &image){
    ui->image->clear();
    ui->image->setPixmap(QPixmap::fromImage(image));
}


Form::~Form()
{
    delete ui;
}
