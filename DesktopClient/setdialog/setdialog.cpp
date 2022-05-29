#include "setdialog.h"
#include "timing.h"
#include <QDebug>
#include <QPushButton>

SetDialog::SetDialog(QWidget* parent) : QDialog(parent) {
    QVBoxLayout* verticalLayout = new QVBoxLayout;

    float progessStartValue = Timing::getCurrentValue();

    explanation = new QLabel(tr("setShutter"), this);
    verticalLayout->addWidget(explanation);
    verticalLayout->setAlignment(explanation, Qt::AlignHCenter);

    value = new QLabel(this);
    value->setText(QString::number(progessStartValue) + "%");
    verticalLayout->addWidget(value);
    verticalLayout->setAlignment(value, Qt::AlignHCenter);

    slider = new QSlider(this);
    slider->setValue(50);
    slider->setMinimum(0);
    slider->setMaximum(100);
    slider->setValue((int) progessStartValue);
    slider->setOrientation(Qt::Horizontal);
    verticalLayout->addWidget(slider);

    okButton = new QPushButton("OK", this);
    verticalLayout->addWidget(okButton);
    verticalLayout->setAlignment(okButton, Qt::AlignRight);

    //dialogBox = new QDialogButtonBox(this);
    //verticalLayout->addWidget(dialogBox);
    //dialogBox->addButton(new ("BUCK", this));

    setLayout(verticalLayout);

    connect(slider, &QSlider::valueChanged, this, &SetDialog::updateProgressValue);
    connect(okButton, &QPushButton::clicked, this, &SetDialog::okPushed);
}

void SetDialog::updateProgressValue(int newValue) {
    value->setText(QString::number(newValue)+"%");
}

void SetDialog::okPushed() {
    done(0);
    emit setPostRequstStart(slider->value());
}


void SetDialog::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);

    int valueInt = Timing::getCurrentValue();

    value->setText(QString::number(valueInt)+"%");
    slider->setValue(valueInt);
}
