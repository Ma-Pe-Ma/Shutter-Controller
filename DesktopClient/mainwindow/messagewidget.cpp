#include "messagewidget.h"

MessageWidget::MessageWidget(int ID, QWidget *parent) : QWidget(parent) {
    messageBox = new QHBoxLayout();

    idLabel = new QLabel(QString::number(ID + 1) + ".", this);
    //idLabel->setFixedSize(40,30);
    idLabel->setFixedWidth(40);
    messageBox->addWidget(idLabel);

    eventLabel = new QLabel(this);
    eventLabel->setText(tr("eventLabel"));
    messageBox->addWidget(eventLabel);

    dateLabel = new QLabel(this);
    dateLabel->setText(tr("dateLabel"));
    messageBox->addWidget(dateLabel);

    setLayout(messageBox);
}

void MessageWidget::updateEvent(QString event) {
    eventLabel->setText(event);
}


void MessageWidget::updateDate(QString date) {
    dateLabel->setText(date);
}
