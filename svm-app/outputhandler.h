#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H
#include <QTextEdit>
#include <QScrollBar>
#include <thread>
#include <unistd.h>

#include <iostream>

class OutputHandler
{
public:
    OutputHandler(QTextEdit* output_textEdit);
    ~OutputHandler();
    void startReaderThread();
    void printToConsole(bool);
private:
    void handleOutput();
private:
    std::thread* _reader_thead;
    int _saved_stdout;
    int _stdout_pipe[2];
    QTextEdit* _output_textEdit;
};

#endif // OUTPUTHANDLER_H
