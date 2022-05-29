#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

class MessageWidget : public QWidget {
    Q_OBJECT

private:
    QHBoxLayout* messageBox;
    QLabel* idLabel;
    QLabel* eventLabel;
    QLabel* dateLabel;

public:
    explicit MessageWidget(int ID, QWidget *parent = nullptr);

    void updateEvent(QString event);
    void updateDate(QString event);
signals:

};

#endif // MESSAGEWIDGET_H
