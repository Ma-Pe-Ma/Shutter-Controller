#ifndef SETDIALOG_H
#define SETDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>

class SetDialog : public QDialog {
    Q_OBJECT
private:
    QLabel* explanation;
    QLabel* value;
    QSlider* slider;
    QDialogButtonBox* dialogBox;
    QPushButton* okButton;

public:
    SetDialog(QWidget*);

signals:
    void setValue(int);
    void setPostRequstStart(float);

public slots:
    void updateProgressValue(int);
    void okPushed();
    void showEvent(QShowEvent*) override;
};

#endif // SETDIALOG_H
