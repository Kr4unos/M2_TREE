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
        case 'f':
            return DRAW_BRANCH;
        case 'L':
        case 'l':
            return DRAW_LEAF;
        case '-':
            return ROTATE_LEFT_X;
        case '+':
            return ROTATE_RIGHT_X;
        case '^':
            return ROTATE_UP_Y;
        case 'v':
            return ROTATE_DOWN_Y;
        case '<':
            return TWIST_LEFT_Z;
        case '>':
            return TWIST_RIGHT_Z;
        case '[':
            return PUSH_BACK;
        case ']':
            return POP_BACK;
        default:
            return NO_ACTION;
    }
}

QString LSystem::getActionNameFromAction(LSystem::Action action)
{
    switch(action){
        case DRAW_BRANCH:
            return "DRAW_BRANCH";
        case DRAW_LEAF:
            return "DRAW_LEAF";
        case ROTATE_LEFT_X:
            return "ROTATE_LEFT_X";
        case ROTATE_RIGHT_X:
            return "ROTATE_RIGHT_X";
        case ROTATE_UP_Y:
            return "ROTATE_UP_Y";
        case ROTATE_DOWN_Y:
            return "ROTATE_DOWN_Y";
        case TWIST_LEFT_Z:
            return "TWIST_LEFT_Z";
        case TWIST_RIGHT_Z:
            return "TWIST_RIGHT_Z";
        case PUSH_BACK:
            return "PUSH_BACK";
        case POP_BACK:
            return "POP_BACK";
        default:
            return "NO_ACTION";
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

    this->setAngle(json_map["angle"].toFloat());
    this->setAngleAlea(json_map["angle_alea"].toFloat());
    this->setAxiom(json_map["axiom"].toString());
    this->setIterations(json_map["iterations"].toInt());
    this->setBranchRadius(json_map["branch_radius"].toFloat());
    this->setBranchRadiusAlea(json_map["branch_radius_alea"].toFloat());
    this->setBranchRadiusReduction(json_map["branch_radius_reduction"].toFloat());
    this->setBranchRadiusReductionAlea(json_map["branch_radius_reduction_alea"].toFloat());
    this->setBranchLength(json_map["branch_length"].toFloat());
    this->setBranchLengthAlea(json_map["branch_length_alea"].toFloat());

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

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    R_generator= std::default_random_engine(seed);
    setRandomGenerator();

    return true;
}

void LSystem::setRandomGenerator(){
    distributionAngle                 = std::normal_distribution<float> (angle,angleAlea);
    distributionBranchRadius          = std::normal_distribution<float> (branchRadius,branchRadiusAlea);
    distributionBranchRadiusReduction = std::normal_distribution<float> (branchRadiusReduction,branchRadiusReductionAlea);
    distributionBranchLength          = std::normal_distribution<float> (branchLength,branchLengthAlea);
}

bool LSystem::exportJSON(QString fileName)
{

    QJsonObject settings_obj;
    settings_obj["angle"] = this->angle;
    settings_obj["angle_alea"] = this->angleAlea;
    settings_obj["axiom"] = this->axiom;

    QJsonObject rules_obj;
    for(int i = 0; i < rulesFrom.count(); i++){
        rules_obj[rulesFrom.at(i)] = rulesTo.at(i);
    }

    settings_obj.insert("rules", rules_obj);
    settings_obj["iterations"] = this->iterations;
    settings_obj["branch_radius"] = this->branchRadius;
    settings_obj["branch_radius_alea"] = this->branchRadiusAlea;
    settings_obj["branch_radius_reduction"] = this->branchRadiusReduction;
    settings_obj["branch_radius_reduction_alea"] = this->branchRadiusReductionAlea;
    settings_obj["branch_length"] = this->branchLength;
    settings_obj["branch_length_alea"] = this->branchLengthAlea;

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
    qDebug() << "NBR ITERATIONS A EFFECTUER:" << this->getIterations();
    QString result(this->getAxiom());
    // On fait le nombre d'itérations demandées
    for(int i = 0; i < this->getIterations(); ++i){
        // On parcours chaque caractère de la string
        for(int j = 0; j < result.size(); ++j){
            QString currentChar = result.at(j);
            // Si le caractère courant n'est pas une variable, on saute directement au suivant
            if(!this->getRulesFrom().contains(currentChar)) continue;
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

QString LSystem::getAxiom() const
{
    return axiom;
}

void LSystem::setAxiom(const QString &value)
{
    axiom = value;
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

float LSystem::getAngle() const
{
    return angle;
}

float LSystem::getAngleRandom()
{
    return distributionAngle(R_generator);
}

void LSystem::setAngle(float value)
{
    angle = value;
}

int LSystem::getIterations() const
{
    return iterations;
}

void LSystem::setIterations(int value)
{
    iterations = value;
}

float LSystem::getBranchRadius() const
{
    return branchRadius;
}

float LSystem::getBranchRadiusRandom()
{
    return distributionBranchRadius(R_generator);
}

void LSystem::setBranchRadius(float value)
{
    branchRadius = value;
}

float LSystem::getBranchRadiusReduction() const
{
    return branchRadiusReduction;
}

float LSystem::getBranchRadiusReductionRandom()
{
    return distributionBranchRadiusReduction(R_generator);
}

void LSystem::setBranchRadiusReduction(float value)
{
    branchRadiusReduction = value;
}

float LSystem::getBranchLength() const
{
    return branchLength;
}

float LSystem::getBranchLengthRandom()
{
    return distributionBranchLength(R_generator);
}

void LSystem::setBranchLength(float value)
{
    branchLength = value;
}

QString LSystem::getResult() const
{
    return result;
}

void LSystem::setResult(const QString &value)
{
    result = value;
}

float LSystem::getBranchLengthAlea() const
{
    return branchLengthAlea;
}

void LSystem::setBranchLengthAlea(float value)
{
    branchLengthAlea = value;
}

float LSystem::getBranchRadiusReductionAlea() const
{
    return branchRadiusReductionAlea;
}

void LSystem::setBranchRadiusReductionAlea(float value)
{
    branchRadiusReductionAlea = value;
}

float LSystem::getBranchRadiusAlea() const
{
    return branchRadiusAlea;
}

void LSystem::setBranchRadiusAlea(float value)
{
    branchRadiusAlea = value;
}

float LSystem::getAngleAlea() const
{
    return angleAlea;
}

void LSystem::setAngleAlea(float value)
{
    angleAlea = value;
}
