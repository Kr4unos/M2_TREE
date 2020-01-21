#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
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

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_openAction_triggered
 */
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

/**
 * @brief MainWindow::on_exportJSONAction_triggered
 */
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

/**
 * @brief MainWindow::on_exportOBJAction_triggered
 */
void MainWindow::on_exportOBJAction_triggered()
{
    ui->statusBar->showMessage("Exportation en OBJ...");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Exportation en OBJ..."), "C://", tr("OBJ Files (*.obj)"));
    if(fileName.isEmpty()) {
        QMessageBox::critical(this, tr("Erreur"), QString("Vous devez spécifier un nom de fichier!"));
        return;
    }

    QFile objFile(fileName);
    if(objFile.open(QIODevice::ReadWrite)){
        QTextStream stream(&objFile);
        QVector3D pos;

        uint nbCy= ui->glarea->cy.getNbObject();
        uint nbLeaf= ui->glarea->leaf.getNbObject();
        int id=1;


        int cysubdivision=ui->glarea->cy.subdivision;
        float cyradius1=ui->glarea->cy.radius1;
        float cyradius2=ui->glarea->cy.radius2;
        float cyheight=ui->glarea->cy.height;
        for (uint i =0 ;i < nbCy;i++) {
            QMatrix4x4 matrice=ui->glarea->cy.getMatrice(i);
            float size=ui->glarea->cy.getSize(i);

            for(double i=0;i<cysubdivision;i++){
                pos.setX(size*cyradius1*cos(2*M_PI*(i/cysubdivision)));
                pos.setY(0.0f);
                pos.setZ(size*cyradius1*sin(2*M_PI*(i/cysubdivision)));

                pos=matrice*pos;
                stream << "v " << pos.x() << " " << pos.y() << " " << pos.z() << endl ;
                id++;

                pos.setX(size*cyradius2*cos(2*M_PI*(i/cysubdivision)));
                pos.setY(cyheight);
                pos.setZ(size*cyradius2*sin(2*M_PI*(i/cysubdivision)));

                pos=matrice*pos;
                stream << "v " << pos.x() << " " << pos.y() << " " << pos.z() << endl ;
                id++;
            }
        }

        int idStartLeafs=id;
        float leafheight=ui->glarea->leaf.height;
        float leafwidth=ui->glarea->leaf.width;
        for (uint i =0 ;i < nbLeaf;i++) {
            QMatrix4x4 matrice=ui->glarea->leaf.getMatrice(i);

            pos.setX(-leafwidth/2);
            pos.setY(0.0f);
            pos.setZ(0.0f);

            pos=matrice*pos;
            stream << "v " << pos.x() << " " << pos.y() << " " << pos.z() << endl ;
            id++;

            pos.setX(-leafwidth/2);
            pos.setY(leafheight);
            pos.setZ(0.0f);

            pos=matrice*pos;
            stream << "v " << pos.x() << " " << pos.y() << " " << pos.z() << endl ;
            id++;

            pos.setX(leafwidth/2);
            pos.setY(leafheight);
            pos.setZ(0.0f);

            pos=matrice*pos;
            stream << "v " << pos.x() << " " << pos.y() << " " << pos.z() << endl ;
            id++;

            pos.setX(leafwidth/2);
            pos.setY(0.0f);
            pos.setZ(0.0f);

            pos=matrice*pos;
            stream << "v " << pos.x() << " " << pos.y() << " " << pos.z() << endl ;
            id++;
        }

        id=1;
        for (uint i =0 ;i < nbCy;i++) {
            int idStartCy=id;
            for(double i=0;i<cysubdivision-1;i++){

                stream << "f " << id << " " << id+1 << " " << id+3 << " " << id+2 << endl ;
                id+=2;
            }
            stream << "f " << id << " " << id+1 << " " << idStartCy+1 << " " << idStartCy << endl ;
            id+=2;
        }

        id=idStartLeafs;
        for (uint i =0 ;i < nbLeaf;i++) {

            stream << "f " << id++ << " " << id++ << " " << id++ << " " << id++ << endl ;
        }
    }
    ui->statusBar->showMessage("Exportation en .OBJ terminée!");
}

/**
 * @brief MainWindow::on_quitAction_triggered
 */
void MainWindow::on_quitAction_triggered()
{
    QApplication::quit();
}

/**
 * @brief MainWindow::on_settingsAction_triggered
 */
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

/**
 * @brief MainWindow::settingsModified
 */
void MainWindow::settingsModified()
{
    currentLSystem->iterate();
    emit parseAndGenerate(currentLSystem);
}
