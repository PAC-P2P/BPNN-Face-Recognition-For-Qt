#include "camera.h"

#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Camera camera;
    camera.show();

    return app.exec();
};
