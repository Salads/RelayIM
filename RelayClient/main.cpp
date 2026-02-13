#include "RelayClient.h"
#include <QtWidgets/QApplication>
#include <QPalette>
#include <QColor>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set light mode stylesheet globally
    app.setStyle("Fusion");
    QPalette lightPalette;
    lightPalette.setColor(QPalette::Window, QColor(255, 255, 255));           // White background
    lightPalette.setColor(QPalette::WindowText, QColor(0, 0, 0));             // Black text
    lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));             // White input areas
    lightPalette.setColor(QPalette::AlternateBase, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Text, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Button, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Link, QColor(0, 0, 255));
    app.setPalette(lightPalette);

    RelayClient window;
    window.show();
    return app.exec();
}
