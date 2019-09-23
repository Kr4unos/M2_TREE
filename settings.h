#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_settings.h"
#include "lsystem.h"

class Settings : public QDialog, private Ui::Settings
{
    Q_OBJECT

    public:

        explicit Settings(QWidget *parent = 0, LSystem* lsystem = nullptr);

    private:
        LSystem* lsystem;

    private slots:
        void on_buttonBox_accepted();
};

#endif // SETTINGS_H
