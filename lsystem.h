#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <string>
#include <QFile>
#include <chrono>
#include <random>

class LSystem
{
    public:
        LSystem(QString fileName);

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

        Action getActionFromSymbol(char symbol);
        QString getActionNameFromAction(Action action);
        bool importJSON(QString fileName);
        bool exportJSON(QString fileName);
        void iterate();

        QString getAxiom() const;
        void setAxiom(const QString &value);

        QStringList getRulesFrom() const;
        void setRulesFrom(const QStringList &value);

        QStringList getRulesTo() const;
        void setRulesTo(const QStringList &value);

        float getAngle() const;
        void setAngle(float value);

        int getIterations() const;
        void setIterations(int value);

        float getBranchRadius() const;
        void setBranchRadius(float value);

        float getBranchRadiusReduction() const;
        void setBranchRadiusReduction(float value);

        float getBranchLength() const;
        void setBranchLength(float value);

        QString getResult() const;
        void setResult(const QString &value);

        float getBranchLengthAlea() const;
        void setBranchLengthAlea(float value);

        float getBranchRadiusReductionAlea() const;
        void setBranchRadiusReductionAlea(float value);

        float getBranchRadiusAlea() const;
        void setBranchRadiusAlea(float value);

        float getAngleAlea() const;
        void setAngleAlea(float value);


        float getAngleRandom();
        float getBranchLengthRandom();
        float getBranchRadiusRandom();
        float getBranchRadiusReductionRandom();

private:

        QString axiom;
        QStringList rulesFrom;
        QStringList rulesTo;
        int iterations = 0;
        float angle = 0.0;
        float angleAlea = 0.0;
        float branchRadius = 0.0;
        float branchRadiusAlea = 0.0;
        float branchRadiusReduction = 0.0;
        float branchRadiusReductionAlea = 0.0;
        float branchLength = 0.0;
        float branchLengthAlea = 0.0;


        QString result;

        std::default_random_engine R_generator;
        std::normal_distribution<float> distributionAngle;
        std::normal_distribution<float> distributionBranchRadius;
        std::normal_distribution<float> distributionBranchRadiusReduction;
        std::normal_distribution<float> distributionBranchLength;
};

#endif // LSYSTEM_H
