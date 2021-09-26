#ifndef MESSAGELIST_H
#define MESSAGELIST_H

#include <QListWidget>
#include <QObject>
#include <QLabel>

#include "messages.h"
#include "messagewidget.h"

class MessageList : public QListWidget {
    Q_OBJECT
private:
    QLabel** labels;
    MessageWidget** messageWidgets;
    QListWidgetItem** listItems;

public:
    explicit MessageList(QWidget *parent = nullptr);
    void updateList();
};

#endif // MESSAGELIST_H
