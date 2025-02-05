#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H
#include <QObject>
#include <thread>
#include <unistd.h>

#include <iostream>
#include <string.h>

class OutputHandler : public QObject
{
    Q_OBJECT
public:
    OutputHandler();
    ~OutputHandler();
    void startReaderThread();
    void setCmdOutput(bool cmd_out);
signals:
    void updateOutput(QString);
private:
    void handleOutput();
private:
    QString _text;
    std::thread* _reader_thead;
    int _saved_stdout;
    int _stdout_pipe[2];

    bool _cmd_out{false};
};

#endif // OUTPUTHANDLER_H
