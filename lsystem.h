#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <string>
#include <QFile>

class LSystem
{
    public:
        LSystem(QString fileName);

        enum Action { DRAW_FORWARD, TURN_LEFT, TURN_RIGHT, PUSH_BACK, POP_BACK, NO_ACTION };

        Action getActionFromSymbol(char symbol);
        bool importJSON(QString fileName);
        bool exportJSON(QString fileName);
        void iterate();

        int getAngle() const;
        void setAngle(int value);

        QString getAxiom() const;
        void setAxiom(const QString &value);

        int getIterations() const;
        void setIterations(int value);

        int getLength() const;
        void setLength(int value);

        QStringList getRulesFrom() const;
        void setRulesFrom(const QStringList &value);

        QStringList getRulesTo() const;
        void setRulesTo(const QStringList &value);

        QStringList getVariables() const;
        void setVariables(const QStringList &value);

        QString getResult() const;
        void setResult(const QString &value);

    private:
        int angle = 0;
        QString axiom;
        QStringList rulesFrom;
        QStringList rulesTo;
        QStringList variables;
        int iterations = 0;
        int length = 0;

        QString result;
};

#endif // LSYSTEM_H
