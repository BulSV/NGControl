#ifdef DEBUG
#include <QDebug>
#endif

#include <QApplication>
#include "PlotterDialog.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

#if defined (Q_OS_UNIX)
    app.setWindowIcon(QIcon(":/Resources/NGControl.png"));
#endif
    PlotterDialog plotterDialog(QString::fromUtf8("NG Control"));
    plotterDialog.showMaximized();

    return app.exec();
}

