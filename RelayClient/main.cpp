#include <QtWidgets/QApplication>
#include <QPalette>
#include <QColor>
#include <QFile>

#include "windows/RelayClient/RelayClient.h"
#include "Logging.h"
#include "Util.h"

int main(int argc, char *argv[])
{
    Log::Initialize("client.log");
    //Log::Initialize("client-" + GetLocalTimestamp() + ".log");
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    // Set light mode stylesheet globally
    //QPalette lightPalette;
    //lightPalette.setColor(QPalette::Window, QColor(255, 255, 255));           // White background
    //lightPalette.setColor(QPalette::WindowText, QColor(0, 0, 0));             // Black text
    //lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));             // White input areas
    //lightPalette.setColor(QPalette::AlternateBase, QColor(240, 240, 240));
    //lightPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    //lightPalette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
    //lightPalette.setColor(QPalette::Text, QColor(0, 0, 0));
    //lightPalette.setColor(QPalette::Button, QColor(240, 240, 240));
    //lightPalette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
    //lightPalette.setColor(QPalette::Link, QColor(0, 0, 255));
    //app.setPalette(lightPalette);

    QFile file("styles.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    app.setStyleSheet(styleSheet);

    RelayClient window;
    window.show();
    window.TryConnect();

    int execResult = app.exec(); 
    Log::Destroy();

    return execResult;
}
