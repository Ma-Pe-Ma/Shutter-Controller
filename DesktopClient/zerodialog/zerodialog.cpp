#include "zerodialog.h"
#include <QDebug>

ZeroDialog::ZeroDialog(QWidget* parent) : QDialog(parent) {
    verticalLayout = new QVBoxLayout();
    horizontalLayout = new QHBoxLayout();

    explanation = new QLabel(this);
    explanation->setText(tr("zeroQuestion"));
    verticalLayout->addWidget(explanation);
    verticalLayout->setAlignment(explanation, Qt::AlignHCenter);

    automaticButton = new QPushButton(this);
    automaticButton->setText(tr("automatic"));
    horizontalLayout->addWidget(automaticButton);

    downButton = new QPushButton(this);
    downButton->setText(tr("down"));
    horizontalLayout->addWidget(downButton);

    upButton = new QPushButton(this);
    upButton->setText(tr("up"));
    horizontalLayout->addWidget(upButton);

    verticalLayout->addItem(horizontalLayout);

    setLayout(verticalLayout);

    connect(automaticButton, &QPushButton::clicked, this, &ZeroDialog::automaticPushed);
    connect(upButton, &QPushButton::clicked, this, &ZeroDialog::upPushed);
    connect(downButton, &QPushButton::clicked, this, &ZeroDialog::downPushed);
}

void ZeroDialog::automaticPushed() {
    done(0);
    emit zeroPostRequestStart(automatic);
}

void ZeroDialog::upPushed() {
    done(1);
    emit zeroPostRequestStart(up);
}

void ZeroDialog::downPushed() {
    done(2);
    emit zeroPostRequestStart(down);
}
