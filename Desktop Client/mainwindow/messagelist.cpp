#include "messagelist.h"
#include <QDebug>

MessageList::MessageList(QWidget *parent) : QListWidget(parent) {
    labels = new QLabel*[MESSAGE_NR];
    listItems = new QListWidgetItem*[MESSAGE_NR];
    messageWidgets = new MessageWidget*[MESSAGE_NR];

    for (int i = 0; i < MESSAGE_NR; i++) {
        //labels[i] = new QLabel(this);
        messageWidgets[i] = new MessageWidget(i, this);
        listItems[i] = new QListWidgetItem(this);
        //labels[i]->setText(Messages::getProcessedMessage(i));
        addItem(listItems[i]);
        setItemWidget(listItems[i], messageWidgets[i]);
        messageWidgets[i]->setFixedHeight(30);
        listItems[i]->setSizeHint(QSize(50,30));
        messageWidgets[i]->setFocusPolicy(Qt::NoFocus);
    }

    viewport()->setAutoFillBackground( false );

    setSelectionMode(QAbstractItemView::NoSelection);
    setFocusPolicy(Qt::NoFocus);

    //QListWidget::item:hover, QListWidget::item:disabled:hover, QListWidget::item:hover:!active,  {background: transparent;}
    //setStyleSheet("QListWidget::item:hover {background-color:transparent;}");
    //setStyleSheet("QListWidget::item:select {background-color:transparent;}");
    //setStyleSheet("QListWidget {background:transparent;}");

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    int listHeight = sizeHintForRow(0) * count() + 2 * frameWidth();
    setFixedSize(500, listHeight);

    updateList();
}

void MessageList::updateList() {
    for (int i = 0; i < MESSAGE_NR; i++) {
        QString message = Messages::getProcessedMessage(i);
        QString date = Messages::getDate(i);

        messageWidgets[i]->updateEvent(message);
        messageWidgets[i]->updateDate(date);
    }
}
