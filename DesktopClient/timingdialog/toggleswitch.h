#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H

#include <QCheckBox>
#include <timing.h>

class ToggleSwitch : public QCheckBox {
    Q_OBJECT
public:
    ToggleSwitch(QWidget*);
};

#endif // TOGGLESWITCH_H
