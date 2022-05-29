#ifndef ZERODIALOG_H
#define ZERODIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "requests/zeropostrequest.h"

class ZeroDialog : public QDialog {
    Q_OBJECT
public:
    ZeroDialog(QWidget* parent);

private:
    QLabel* explanation;
    QVBoxLayout* verticalLayout;
    QHBoxLayout* horizontalLayout;

    QPushButton* automaticButton;
    QPushButton* downButton;
    QPushButton* upButton;
signals:
    void zeroPostRequestStart(Zero);
private slots:
    void automaticPushed();
    void upPushed();
    void downPushed();
};

#endif // ZERODIALOG_H
