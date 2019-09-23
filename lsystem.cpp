#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "lsystem.h"

LSystem::LSystem(QString fileName)
{
    this->importJSON(fileName);
    this->iterate();
}

LSystem::Action LSystem::getActionFromSymbol(char symbol)
{
    switch(symbol){
        case 'F':
            return DRAW_FORWARD;
        case '-':
            return TURN_LEFT;
        case '+':
            return TURN_RIGHT;
        case '[':
            return PUSH_BACK;
        case ']':
            return POP_BACK;
        default:
            return NO_ACTION;
    }
}

bool LSystem::importJSON(QString fileName)
{
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "Failed to open " << fileName;
        return false;
    }

    QTextStream file_text(&file);
    QString json_string;
    json_string = file_text.readAll();
    file.close();
    QByteArray json_bytes = json_string.toLocal8Bit();
    auto json_doc = QJsonDocument::fromJson(json_bytes);

    if(json_doc.isNull()){
        qDebug() << "Failed to create JSON doc.";
        return false;
    }

    if(!json_doc.isObject()){
        qDebug() << "JSON is not an object.";
        return false;
    }

    QJsonObject json_obj = json_doc.object();

    if(json_obj.isEmpty()){
        qDebug()<<"JSON object is empty.";
        return false;
    }

    QVariantMap json_map = json_obj.toVariantMap();

    this->setAngle(json_map["angle"].toInt());
    this->setAxiom(json_map["axiom"].toString());
    this->setIterations(json_map["iterations"].toInt());
    this->setLength(json_map["length"].toInt());
    this->setVariables(json_map["variables"].toStringList());

    QVariantMap rules_map = json_map["rules"].toMap();

    QStringList key_list = rules_map.keys();
    QStringList value_list;

    for(int i=0; i < key_list.count(); ++i){
        QString key = key_list.at(i);
        QString value = rules_map[key.toLocal8Bit()].toString();
        value_list.append(value);
    }

    this->setRulesFrom(key_list);
    this->setRulesTo(value_list);

    return true;
}
bool LSystem::exportJSON(QString fileName)
{

    QJsonObject settings_obj;
    settings_obj["angle"] = this->angle;
    settings_obj["axiom"] = this->axiom;

    QJsonObject rules_obj;
    for(int i = 0; i < rulesFrom.count(); i++){
        rules_obj[rulesFrom.at(i)] = rulesTo.at(i);
    }

    settings_obj.insert("rules", rules_obj);
    settings_obj["iterations"] = this->iterations;
    settings_obj["length"] = this->length;

    QJsonArray variables;
    for(int i = 0; i < this->variables.count(); i++){
        variables << this->variables.at(i);
    }
    settings_obj.insert("variables", variables);

    QJsonDocument json_doc(settings_obj);
    QString json_string = json_doc.toJson();

    QFile save_file(fileName);
    if(!save_file.open(QIODevice::WriteOnly)){
        qDebug() << "Failed to open save file";
        return false;
    }
    save_file.write(json_string.toLocal8Bit());
    save_file.close();
    return true;
}

void LSystem::iterate()
{
    QString result(this->getAxiom());
    qDebug() << "ITERATION 0 = " << result;
    // On fait le nombre d'itérations demandées
    for(int i = 0; i < this->getIterations(); ++i){
        // On parcours chaque caractère de la string
        for(int j = 0; j < result.size(); ++j){
            QString currentChar = result.at(j);
            // Si le caractère courant n'est pas une variable, on saute directement au suivant
            if(!this->getVariables().contains(currentChar)) continue;
            bool found = false;
            // Tant que l'on ne trouve pas la règle de transition
            while(!found) {
                // On parcours la liste des règles
                for(int k = 0; k < this->getRulesFrom().size(); ++k){
                    if(this->getRulesFrom().at(k) == currentChar) {
                        result.replace(j, currentChar.size(), this->getRulesTo().at(k));
                        j += this->getRulesTo().at(k).size()-1;
                        found = true;
                        break;
                    }
                }
                // Si on a pas trouvé la règle, on rajoute la caractère d'après et on recommence
                if(!found && j+1 < result.size()) currentChar += result.at(++j);
            }
        }
        qDebug() << "ITERATION " << i+1 << " = " << result;
    }
    this->result = result;
}


int LSystem::getAngle() const
{
    return angle;
}
void LSystem::setAngle(int value)
{
    angle = value;
}

QString LSystem::getAxiom() const
{
    return axiom;
}
void LSystem::setAxiom(const QString &value)
{
    axiom = value;
}

int LSystem::getIterations() const
{
    return iterations;
}
void LSystem::setIterations(int value)
{
    iterations = value;
}

int LSystem::getLength() const
{
    return length;
}
void LSystem::setLength(int value)
{
    length = value;
}

QStringList LSystem::getRulesFrom() const
{
    return rulesFrom;
}
void LSystem::setRulesFrom(const QStringList &value)
{
    rulesFrom = value;
}

QStringList LSystem::getRulesTo() const
{
    return rulesTo;
}
void LSystem::setRulesTo(const QStringList &value)
{
    rulesTo = value;
}

QStringList LSystem::getVariables() const
{
    return variables;
}
void LSystem::setVariables(const QStringList &value)
{
    variables = value;
}

QString LSystem::getResult() const
{
    return result;
}
void LSystem::setResult(const QString &value)
{
    result = value;
}
