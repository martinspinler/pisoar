#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QCheckBox>

class Settings : public QWidget
{
    Q_OBJECT

public:
    QCheckBox   *fl_show;
public:
    explicit Settings(QWidget *parent = 0);

signals:

public slots:
};

#endif // SETTINGS_H
