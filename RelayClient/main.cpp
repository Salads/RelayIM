#include "RelayClient.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    RelayClient window;
    window.show();
    return app.exec();
}
