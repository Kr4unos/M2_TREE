#include "settings.h"

#include <QDebug>

Settings::Settings(QWidget *parent, LSystem* lsystem) :
    QDialog(parent)
{
    setupUi(this);
    this->setWindowTitle("Paramètres");

    this->lsystem = lsystem;

    connect(this, SIGNAL(LSystemChanged()), parent, SLOT(settingsModified()));

    this->angle_input->setValue(lsystem->getAngle());
    this->axiom_input->setText(lsystem->getAxiom());
    this->iterations_input->setValue(lsystem->getIterations());
    this->length_input->setValue(lsystem->getBranchLength());

    QString rules_txt("");
    for(int i = 0; i < lsystem->getRulesFrom().count(); ++i){
        rules_txt += lsystem->getRulesFrom().at(i) + " = " + lsystem->getRulesTo().at(i) + "\n";
    }

    this->rules_input->setPlainText(rules_txt);
}

void Settings::on_buttonBox_accepted()
{
    this->lsystem->setAngle(this->angle_input->value());
    this->lsystem->setAxiom(this->axiom_input->text());
    this->lsystem->setIterations(this->iterations_input->value());
    this->lsystem->setBranchLength(this->length_input->value());

    QStringList rules_lines = this->rules_input->toPlainText().split(QRegExp("\n|\r\n|\r"));
    rules_lines.removeAll(QString(""));

    QStringList rulesFrom;
    QStringList rulesTo;

    for(int i = 0; i < rules_lines.count(); i++){
        QString line = rules_lines.at(i);
        line.replace(" ", "");
        std::string line_string = line.toStdString();

        rulesFrom.append(QString::fromStdString(line_string.substr(0, line_string.find("="))));
        rulesTo.append(QString::fromStdString(line_string.substr(line_string.find("=") + 1)));
    }

    this->lsystem->setRulesFrom(rulesFrom);
    this->lsystem->setRulesTo(rulesTo);

    emit LSystemChanged();
}
