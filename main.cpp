#include "widget.h"
#include <QApplication>
#include "exception_handler.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //设置dump文件收集
    SetUnhandledExceptionFilter(HS_UnhandledExceptionFilter);

    Widget w;
    w.show();

    return a.exec();
}
