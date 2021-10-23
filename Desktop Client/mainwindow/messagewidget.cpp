#include "messagewidget.h"
#include "languages/languages.h"

MessageWidget::MessageWidget(int ID, QWidget *parent) : QWidget(parent) {
    messageBox = new QHBoxLayout();

    idLabel = new QLabel(QString::number(ID + 1) + ".", this);
    //idLabel->setFixedSize(40,30);
    idLabel->setFixedWidth(40);
    messageBox->addWidget(idLabel);

    eventLabel = new QLabel(this);
    std::string eventLabelString = Languages::getFormattedStringByID(Languages::StringResource::eventLabel);
    eventLabel->setText(eventLabelString.c_str());
    messageBox->addWidget(eventLabel);

    dateLabel = new QLabel(this);
    std::string dateLabelString = Languages::getFormattedStringByID(Languages::StringResource::dateLabel);
    dateLabel->setText(dateLabelString.c_str());
    messageBox->addWidget(dateLabel);

    setLayout(messageBox);
}

void MessageWidget::updateEvent(QString event) {
    eventLabel->setText(event);
}


void MessageWidget::updateDate(QString date) {
    dateLabel->setText(date);
}
