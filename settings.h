#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_settings.h"
#include "lsystem.h"

/**
 * @brief The Settings class linked to the settings window
 */
class Settings : public QDialog, private Ui::Settings
{
    Q_OBJECT

public:
    /**
     * @brief Settings
     * @param parent
     * @param lsystem
     */
    explicit Settings(QWidget *parent = 0, LSystem* lsystem = nullptr);

private:
    /**
     * @brief lsystem
     */
    LSystem* lsystem;

private slots:
    /**
     * @brief on_buttonBox_accepted
     */
    void on_buttonBox_accepted();

signals:
    /**
     * @brief LSystemChanged activate when we modify our lsystem in the settings window
     */
    void LSystemChanged();
};

#endif // SETTINGS_H
