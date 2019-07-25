#include "outputhandler.h"

OutputHandler::OutputHandler()
{
    _saved_stdout = dup(STDOUT_FILENO);
    pipe(_stdout_pipe);
    dup2(_stdout_pipe[1], STDOUT_FILENO);
//    dup2(STDOUT_FILENO, STDERR_FILENO); // merge stderr and stdout
    close(_stdout_pipe[1]);
}

OutputHandler::~OutputHandler()
{
    _reader_thead->join();
    dup2(_saved_stdout, STDOUT_FILENO); //cancel stdout redirection
}

void OutputHandler::handleOutput(){
    int bytes_read;
    char buf[1024];
    while((bytes_read = (int)read(_stdout_pipe[0], buf, sizeof(buf)))){
        emit updateOutput(QString::fromLatin1(buf, bytes_read));
        if(_cmd_out){
            write(_saved_stdout, buf, bytes_read);
        }
    }
}

void OutputHandler::startReaderThread()
{
    _reader_thead = new std::thread(&OutputHandler::handleOutput, this);
}

void OutputHandler::setCmdOutput(bool cmd_out){
    _cmd_out = cmd_out ? true : false;
}

