#include "zerodialog.h"
#include <QDebug>

#include <languages/languages.h>

ZeroDialog::ZeroDialog(QWidget* parent) : QDialog(parent) {
    verticalLayout = new QVBoxLayout();
    horizontalLayout = new QHBoxLayout();

    explanation = new QLabel(this);
    std::string zeroQuestionString = Languages::getFormattedStringByID(Languages::StringResource::zeroQuestion);
    explanation->setText(zeroQuestionString.c_str());
    verticalLayout->addWidget(explanation);
    verticalLayout->setAlignment(explanation, Qt::AlignHCenter);

    automaticButton = new QPushButton(this);
    std::string automaticString = Languages::getFormattedStringByID(Languages::StringResource::automatic);
    automaticButton->setText(automaticString.c_str());
    horizontalLayout->addWidget(automaticButton);

    downButton = new QPushButton(this);
    std::string downString = Languages::getFormattedStringByID(Languages::StringResource::down);
    downButton->setText(downString.c_str());
    horizontalLayout->addWidget(downButton);

    upButton = new QPushButton(this);
    std::string upString = Languages::getFormattedStringByID(Languages::StringResource::up);
    upButton->setText(upString.c_str());
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
