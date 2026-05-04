#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("SurveyForm");
    app.setApplicationDisplayName("Форма опитування");
    app.setOrganizationName("PIK Lab3");

    QLocale::setDefault(QLocale(QLocale::Ukrainian, QLocale::Ukraine));

    MainWindow window;
    window.show();

    return app.exec();
}
