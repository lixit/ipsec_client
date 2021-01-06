#include "runguard.h"
#include "login.h"
#include <QApplication>
#include <QTranslator>

QTranslator *translator = new QTranslator;
QTranslator *qtbaseTranslator = new QTranslator;

int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(systray);

    // only allow a single instance
    RunGuard guard( "some_random_key" );

    //Enables high-DPI scaling in Qt
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    //QApplication::setQuitOnLastWindowClosed(false);

    QString locale = QLocale::system().name();

    //load system default language
    translator->load(QString("vpn_cpp_") + locale, ":/translations");
    qApp->installTranslator(translator);

    //load qtbase translation
    qtbaseTranslator->load(QString("qtbase_") + locale, ":/translations");
    qApp->installTranslator(qtbaseTranslator);

    LoginWindow *loginWin = new LoginWindow();

    if ( !guard.tryToRun() ) {
        //w.isRunning();
        return 0;
    }

    loginWin->show();
    loginWin->raise();
    loginWin->activateWindow();


    return app.exec();
}
