#include "MainWin.hpp"

MainWin::MainWin()
    : AbstractMainWin()
{
}

void MainWin::initScene()
{
	stars.initFromFile(23.4392811 * constant::pi / 180.f);
}

void MainWin::updateScene(BasicCamera& camera)
{
	camera.setView(QVector3D(0, 0, 0), QVector3D(-1, 0, 0), QVector3D(0, 0, 1));
}

void MainWin::renderScene(BasicCamera const& camera)
{
	Q_UNUSED(camera);
	stars.render();
}

MainWin::~MainWin() {}
