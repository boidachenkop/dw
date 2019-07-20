#include "outputhandler.h"

OutputHandler::OutputHandler(QTextEdit* output_textEdit):
    _output_textEdit(output_textEdit)
{
    _saved_stdout = dup(STDOUT_FILENO);
    pipe(_stdout_pipe);
    dup2(_stdout_pipe[1], STDOUT_FILENO);
    close(_stdout_pipe[1]);
}



OutputHandler::~OutputHandler()
{
    _reader_thead->join();
    dup2(_saved_stdout, STDOUT_FILENO);
}

void OutputHandler::handleOutput(){
    int bytes_read;
    char buf[1024];
    int i=0;
    while((bytes_read = read(_stdout_pipe[0], buf, sizeof(buf)))){
        _output_textEdit->append(QString::fromLatin1(buf, bytes_read));
    }
}


void OutputHandler::startReaderThread()
{
    _reader_thead = new std::thread(&OutputHandler::handleOutput, this);
}

void OutputHandler::printToConsole(bool console)
{
    if(console){
        dup2(_stdout_pipe[1], STDOUT_FILENO);
    }else{
        dup2(_saved_stdout, STDOUT_FILENO);
    }
}
