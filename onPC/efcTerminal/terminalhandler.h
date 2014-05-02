#ifndef TERMINALHANDLER_H
#define TERMINALHANDLER_H


#include <QObject>
#include <iostream>
#include <QString>
#include <QTextStream>

class TerminalInHandler : public QObject
{
    Q_OBJECT
public:
    TerminalInHandler();
    virtual ~TerminalInHandler();
signals:
    void NewCmd(const QString&);
public slots:
    void Execute();
protected:
    QTextStream * _in;
};
#endif // TERMINALHANDLER_H
