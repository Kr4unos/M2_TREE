#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("TREE - Tree Random and Epic Elaboration");
    this->setWindowIcon(QIcon(":/icons/logo.png"));
    // Connect open event
    connect(this, SIGNAL(parseAndGenerate(LSystem*)), ui->glarea, SLOT(parseAndGenerate(LSystem*)));

    // Remove unused toolbar
    this->removeToolBar(ui->mainToolBar);

    // Apply icons on menus
    ui->openAction->setIcon(icon_open);
    ui->exportJSONAction->setIcon(icon_export_json);
    ui->exportOBJAction->setIcon(icon_export_obj);
    ui->quitAction->setIcon(icon_quit);
    ui->settingsAction->setIcon(icon_settings);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openAction_triggered()
{
    ui->statusBar->showMessage("Sélection du fichier de règles...");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Ouvrir le fichier de configuration..."), "C://", tr("JSON Files (*.json)"));
    if(fileName.isEmpty()) return;
    currentLSystem = new LSystem(fileName);
    if(settings != nullptr && settings->isVisible()) settings->close();
    settings = nullptr;
    emit parseAndGenerate(currentLSystem);
    QFileInfo fileInfo(fileName);
    ui->statusBar->showMessage("Fichier " + fileInfo.baseName() + " ouvert avec succès!");
}
void MainWindow::on_exportJSONAction_triggered()
{
    if(currentLSystem == nullptr) {
        QMessageBox::warning(this, "Erreur!", "Vous n'avez rien à exporter!");
        return;
    }
    ui->statusBar->showMessage("Exportation en JSON...");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Exportation en JSON..."), "C://", tr("JSON Files (*.json)"));
    if(fileName.isEmpty()) {
        QMessageBox::critical(this, tr("Erreur"), QString("Vous devez spécifier un nom de fichier!"));
        return;
    }
    currentLSystem->exportJSON(fileName);
    ui->statusBar->showMessage("Exportation en .JSON terminée!");
}
void MainWindow::on_exportOBJAction_triggered()
{
    ui->statusBar->showMessage("Exportation en OBJ...");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Exportation en OBJ..."), "C://", tr("OBJ Files (*.obj)"));
    if(fileName.isEmpty()) {
        QMessageBox::critical(this, tr("Erreur"), QString("Vous devez spécifier un nom de fichier!"));
        return;
    }
    fileName += ".obj";
    //TODO: Exportation en OBJ
    ui->statusBar->showMessage("Exportation en .OBJ terminée!");
}
void MainWindow::on_quitAction_triggered()
{
    QApplication::quit();
}
void MainWindow::on_settingsAction_triggered()
{
    if(settings == nullptr){
        if(currentLSystem == nullptr) {
            QMessageBox::warning(this, "Erreur!", "Aucun système chargé!");
            return;
        }
        settings = new Settings(this, currentLSystem);
        settings->show();
    }
    if(!settings->isVisible()) settings->show();
}

void MainWindow::settingsModified()
{
    currentLSystem->iterate();
    emit parseAndGenerate(currentLSystem);
}
