#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <string>
#include <QFile>

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

private:

        QString axiom;
        QStringList rulesFrom;
        QStringList rulesTo;
        float angle = 0.0;
        int iterations = 0;
        float branchRadius = 0.0;
        float branchRadiusReduction = 0.0;
        float branchLength = 0.0;

        QString result;
};

#endif // LSYSTEM_H
