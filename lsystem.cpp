#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "lsystem.h"

/**
 * @brief LSystem::LSystem
 * @param fileName
 */
LSystem::LSystem(QString fileName)
{
    this->importJSON(fileName);
    this->iterate();
}

/**
 * @brief LSystem::getActionFromSymbol
 * @param symbol
 * @return
 */
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

/**
 * @brief LSystem::getActionNameFromAction
 * @param action
 * @return
 */
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

/**
 * @brief LSystem::importJSON
 * @param fileName
 * @return
 */
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

/**
 * @brief LSystem::setRandomGenerator
 */
void LSystem::setRandomGenerator(){
    distributionAngle                 = std::normal_distribution<float> (angle,angleAlea);
    distributionBranchRadius          = std::normal_distribution<float> (branchRadius,branchRadiusAlea);
    distributionBranchRadiusReduction = std::normal_distribution<float> (branchRadiusReduction,branchRadiusReductionAlea);
    distributionBranchLength          = std::normal_distribution<float> (branchLength,branchLengthAlea);
}

/**
 * @brief LSystem::exportJSON
 * @param fileName
 * @return
 */
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

/**
 * @brief LSystem::iterate
 */
void LSystem::iterate()
{
    //qDebug() << "NBR ITERATIONS A EFFECTUER:" << this->getIterations();
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
        //qDebug() << "ITERATION " << i+1 << " = " << result;
    }
    this->result = result;
}

/**
 * @brief LSystem::getAxiom
 * @return
 */
QString LSystem::getAxiom() const
{
    return axiom;
}

/**
 * @brief LSystem::setAxiom
 * @param value
 */
void LSystem::setAxiom(const QString &value)
{
    axiom = value;
}

/**
 * @brief LSystem::getRulesFrom
 * @return
 */
QStringList LSystem::getRulesFrom() const
{
    return rulesFrom;
}

/**
 * @brief LSystem::setRulesFrom
 * @param value
 */
void LSystem::setRulesFrom(const QStringList &value)
{
    rulesFrom = value;
}

/**
 * @brief LSystem::getRulesTo
 * @return
 */
QStringList LSystem::getRulesTo() const
{
    return rulesTo;
}

/**
 * @brief LSystem::setRulesTo
 * @param value
 */
void LSystem::setRulesTo(const QStringList &value)
{
    rulesTo = value;
}

/**
 * @brief LSystem::getAngle
 * @return
 */
float LSystem::getAngle() const
{
    return angle;
}

/**
 * @brief LSystem::getAngleRandom
 * @return
 */
float LSystem::getAngleRandom()
{
    return distributionAngle(R_generator);
}

/**
 * @brief LSystem::setAngle
 * @param value
 */
void LSystem::setAngle(float value)
{
    angle = value;
}

/**
 * @brief LSystem::getIterations
 * @return
 */
int LSystem::getIterations() const
{
    return iterations;
}

/**
 * @brief LSystem::setIterations
 * @param value
 */
void LSystem::setIterations(int value)
{
    iterations = value;
}

/**
 * @brief LSystem::getBranchRadius
 * @return
 */
float LSystem::getBranchRadius() const
{
    return branchRadius;
}

/**
 * @brief LSystem::getBranchRadiusRandom
 * @return
 */
float LSystem::getBranchRadiusRandom()
{
    return distributionBranchRadius(R_generator);
}

/**
 * @brief LSystem::setBranchRadius
 * @param value
 */
void LSystem::setBranchRadius(float value)
{
    branchRadius = value;
}

/**
 * @brief LSystem::getBranchRadiusReduction
 * @return
 */
float LSystem::getBranchRadiusReduction() const
{
    return branchRadiusReduction;
}

/**
 * @brief LSystem::getBranchRadiusReductionRandom
 * @return
 */
float LSystem::getBranchRadiusReductionRandom()
{
    return distributionBranchRadiusReduction(R_generator);
}

/**
 * @brief LSystem::setBranchRadiusReduction
 * @param value
 */
void LSystem::setBranchRadiusReduction(float value)
{
    branchRadiusReduction = value;
}

/**
 * @brief LSystem::getBranchLength
 * @return
 */
float LSystem::getBranchLength() const
{
    return branchLength;
}

/**
 * @brief LSystem::getBranchLengthRandom
 * @return
 */
float LSystem::getBranchLengthRandom()
{
    return distributionBranchLength(R_generator);
}

/**
 * @brief LSystem::setBranchLength
 * @param value
 */
void LSystem::setBranchLength(float value)
{
    branchLength = value;
}

/**
 * @brief LSystem::getResult
 * @return
 */
QString LSystem::getResult() const
{
    return result;
}

/**
 * @brief LSystem::setResult
 * @param value
 */
void LSystem::setResult(const QString &value)
{
    result = value;
}

/**
 * @brief LSystem::getBranchLengthAlea
 * @return
 */
float LSystem::getBranchLengthAlea() const
{
    return branchLengthAlea;
}

/**
 * @brief LSystem::setBranchLengthAlea
 * @param value
 */
void LSystem::setBranchLengthAlea(float value)
{
    branchLengthAlea = value;
}

/**
 * @brief LSystem::getBranchRadiusReductionAlea
 * @return
 */
float LSystem::getBranchRadiusReductionAlea() const
{
    return branchRadiusReductionAlea;
}

/**
 * @brief LSystem::setBranchRadiusReductionAlea
 * @param value
 */
void LSystem::setBranchRadiusReductionAlea(float value)
{
    branchRadiusReductionAlea = value;
}

/**
 * @brief LSystem::getBranchRadiusAlea
 * @return
 */
float LSystem::getBranchRadiusAlea() const
{
    return branchRadiusAlea;
}

/**
 * @brief LSystem::setBranchRadiusAlea
 * @param value
 */
void LSystem::setBranchRadiusAlea(float value)
{
    branchRadiusAlea = value;
}

/**
 * @brief LSystem::getAngleAlea
 * @return
 */
float LSystem::getAngleAlea() const
{
    return angleAlea;
}

/**
 * @brief LSystem::setAngleAlea
 * @param value
 */
void LSystem::setAngleAlea(float value)
{
    angleAlea = value;
}
