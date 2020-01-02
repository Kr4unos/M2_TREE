#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>
#include <QIcon>
#include <QMessageBox>
#include <vector>

#include "lsystem.h"
#include "settings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


signals:
    void parseAndGenerate(LSystem* lsystem);

private slots:
    void on_openAction_triggered();
    void on_exportJSONAction_triggered();
    void on_exportOBJAction_triggered();
    void on_quitAction_triggered();
    void on_settingsAction_triggered();
    void settingsModified();

private:

    LSystem* currentLSystem = nullptr;
    Settings* settings = nullptr;

    const QIcon icon_open = QIcon(":/icons/open.png");
    const QIcon icon_export_json = QIcon(":/icons/export_json.png");
    const QIcon icon_export_obj = QIcon(":/icons/export_obj.png");
    const QIcon icon_quit = QIcon(":/icons/quit.png");
    const QIcon icon_settings = QIcon(":/icons/settings.png");

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
