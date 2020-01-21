#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <string>
#include <QFile>
#include <chrono>
#include <random>

/**
 * @brief The LSystem class used to import, export, parse and generate languages
 */
class LSystem
{

public:

    /**
     * @brief LSystem
     * @param fileName
     */
    LSystem(QString fileName);

    /**
     * @brief The Action enum used to get all the possible operations in our language
     */
    enum Action {
        DRAW_BRANCH = 0,
        DRAW_LEAF = 1,
        ROTATE_LEFT_X = 2,
        ROTATE_RIGHT_X = 3,
        ROTATE_UP_Y = 4,
        ROTATE_DOWN_Y = 5,
        TWIST_LEFT_Z = 6,
        TWIST_RIGHT_Z = 7,
        PUSH_BACK = 8,
        POP_BACK = 9,
        NO_ACTION = 10
    };

    /**
     * @brief getActionFromSymbol
     * @param symbol
     * @return the Action linked to the symbol
     */
    Action getActionFromSymbol(char symbol);

    /**
     * @brief getActionNameFromAction
     * @param action
     * @return the symbol linked to the Action
     */
    QString getActionNameFromAction(Action action);

    /**
     * @brief importJSON
     * @param fileName
     * @return if successfully imported
     */
    bool importJSON(QString fileName);

    /**
     * @brief exportJSON
     * @param fileName
     * @return if successfully exported
     */
    bool exportJSON(QString fileName);

    /**
     * @brief iterate function used to iterate from our axiom to generate our tree
     */
    void iterate();

    /**
     * @brief getAxiom
     * @return
     */
    QString getAxiom() const;

    /**
     * @brief setAxiom
     * @param value
     */
    void setAxiom(const QString &value);

    /**
     * @brief getRulesFrom
     * @return
     */
    QStringList getRulesFrom() const;

    /**
     * @brief setRulesFrom
     * @param value
     */
    void setRulesFrom(const QStringList &value);

    /**
     * @brief getRulesTo
     * @return
     */
    QStringList getRulesTo() const;

    /**
     * @brief setRulesTo
     * @param value
     */
    void setRulesTo(const QStringList &value);

    /**
     * @brief getAngle
     * @return
     */
    float getAngle() const;

    /**
     * @brief setAngle
     * @param value
     */
    void setAngle(float value);

    /**
     * @brief getIterations
     * @return
     */
    int getIterations() const;

    /**
     * @brief setIterations
     * @param value
     */
    void setIterations(int value);

    /**
     * @brief getBranchRadius
     * @return
     */
    float getBranchRadius() const;

    /**
     * @brief setBranchRadius
     * @param value
     */
    void setBranchRadius(float value);

    /**
     * @brief getBranchRadiusReduction
     * @return
     */
    float getBranchRadiusReduction() const;

    /**
     * @brief setBranchRadiusReduction
     * @param value
     */
    void setBranchRadiusReduction(float value);

    /**
     * @brief getBranchLength
     * @return
     */
    float getBranchLength() const;

    /**
     * @brief setBranchLength
     * @param value
     */
    void setBranchLength(float value);

    /**
     * @brief getResult
     * @return
     */
    QString getResult() const;

    /**
     * @brief setResult
     * @param value
     */
    void setResult(const QString &value);

    /**
     * @brief getBranchLengthAlea
     * @return
     */
    float getBranchLengthAlea() const;

    /**
     * @brief setBranchLengthAlea
     * @param value
     */
    void setBranchLengthAlea(float value);

    /**
     * @brief getBranchRadiusReductionAlea
     * @return
     */
    float getBranchRadiusReductionAlea() const;

    /**
     * @brief setBranchRadiusReductionAlea
     * @param value
     */
    void setBranchRadiusReductionAlea(float value);

    /**
     * @brief getBranchRadiusAlea
     * @return
     */
    float getBranchRadiusAlea() const;

    /**
     * @brief setBranchRadiusAlea
     * @param value
     */
    void setBranchRadiusAlea(float value);

    /**
     * @brief getAngleAlea
     * @return
     */
    float getAngleAlea() const;

    /**
     * @brief setAngleAlea
     * @param value
     */
    void setAngleAlea(float value);

    /**
     * @brief getAngleRandom
     * @return
     */
    float getAngleRandom();

    /**
     * @brief getBranchLengthRandom
     * @return
     */
    float getBranchLengthRandom();

    /**
     * @brief getBranchRadiusRandom
     * @return
     */
    float getBranchRadiusRandom();

    /**
     * @brief getBranchRadiusReductionRandom
     * @return
     */
    float getBranchRadiusReductionRandom();

    /**
     * @brief setRandomGenerator
     */
    void setRandomGenerator();

private:

    /**
     * @brief axiom
     */
    QString axiom;

    /**
     * @brief rulesFrom (left side of rules => to find)
     */
    QStringList rulesFrom;

    /**
     * @brief rulesTo (right side of rules => replace by)
     */
    QStringList rulesTo;

    /**
     * @brief iterations
     */
    int iterations = 0;

    /**
     * @brief angle
     */
    float angle = 0.0;

    /**
     * @brief angleAlea
     */
    float angleAlea = 0.0;

    /**
     * @brief branchRadius
     */
    float branchRadius = 0.0;

    /**
     * @brief branchRadiusAlea
     */
    float branchRadiusAlea = 0.0;

    /**
     * @brief branchRadiusReduction
     */
    float branchRadiusReduction = 0.0;

    /**
     * @brief branchRadiusReductionAlea
     */
    float branchRadiusReductionAlea = 0.0;

    /**
     * @brief branchLength
     */
    float branchLength = 0.0;

    /**
     * @brief branchLengthAlea
     */
    float branchLengthAlea = 0.0;


    QString result;

    std::default_random_engine R_generator;
    std::normal_distribution<float> distributionAngle;
    std::normal_distribution<float> distributionBranchRadius;
    std::normal_distribution<float> distributionBranchRadiusReduction;
    std::normal_distribution<float> distributionBranchLength;
};

#endif // LSYSTEM_H
