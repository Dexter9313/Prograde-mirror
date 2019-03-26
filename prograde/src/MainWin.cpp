#include "MainWin.hpp"

MainWin::MainWin()
    : AbstractMainWin()
{
}

void MainWin::keyPressEvent(QKeyEvent* e)
{
	auto cam(dynamic_cast<Camera*>(&getCamera()));
	if(e->key() == Qt::Key_Tab)
	{
		bodyTracked++;
		std::vector<CelestialBody*> bodies(
		    orbitalSystem->getAllCelestialBodiesPointers());
		if(bodyTracked == bodies.size())
			bodyTracked = 0;
		cam->target = bodies[bodyTracked];
	}
	else if(e->key() == Qt::Key_R)
	{
		float timeCoeff(clock.getTimeCoeff());
		if(timeCoeff >= 10)
			timeCoeff /= 10;
		clock.setTimeCoeff(timeCoeff);
	}
	else if(e->key() == Qt::Key_T)
	{
		float timeCoeff(clock.getTimeCoeff());
		if(timeCoeff <= 100000)
			timeCoeff *= 10;
		clock.setTimeCoeff(timeCoeff);
	}

	AbstractMainWin::keyPressEvent(e);
}

void MainWin::mousePressEvent(QMouseEvent* e)
{
	if(e->button() != Qt::MouseButton::RightButton)
	{
		return;
	}

	lastCursorPos = QCursor::pos();
	QCursor::setPos(width() / 2, height() / 2);
	QCursor c(cursor());
	c.setShape(Qt::CursorShape::BlankCursor);
	setCursor(c);
	trackballEnabled = true;
}

void MainWin::mouseReleaseEvent(QMouseEvent* e)
{
	if(e->button() != Qt::MouseButton::RightButton)
	{
		return;
	}

	trackballEnabled = false;
	QCursor c(cursor());
	c.setShape(Qt::CursorShape::ArrowCursor);
	setCursor(c);
	QCursor::setPos(lastCursorPos);
}

void MainWin::mouseMoveEvent(QMouseEvent* e)
{
	if(!trackballEnabled)
	{
		return;
	}
	auto cam(dynamic_cast<Camera*>(&getCamera()));
	float dx = (static_cast<float>(width()) / 2 - e->globalX()) / width();
	float dy = (static_cast<float>(height()) / 2 - e->globalY()) / height();
	cam->angleAroundZ += dx * 3.14f / 3.f;
	cam->angleAboveXY += dy * 3.14f / 3.f;
	QCursor::setPos(width() / 2, height() / 2);
}

void MainWin::wheelEvent(QWheelEvent* e)
{
	auto cam(dynamic_cast<Camera*>(&getCamera()));
	if(e->angleDelta().y() > 0.f)
	{
		cam->distance /= 1.2f;// + (0.2f * e->angleDelta().y() / 1000.f);
	}
	else
	{
		cam->distance *= 1.2f;// - (0.2f * e->angleDelta().y() / 1000.f);
	}
	AbstractMainWin::wheelEvent(e);
}

void MainWin::initScene()
{
	stars.initFromFile(23.4392811 * constant::pi / 180.f);

	auto cam = new Camera(&vrHandler);
	cam->setPerspectiveProj(70.0f, static_cast<float>(width())
	                                   / static_cast<float>(height()));
	cam->target = orbitalSystem->getAllCelestialBodiesPointers()[0];
	systemRenderer = new OrbitalSystemRenderer(orbitalSystem);
	setCamera(cam);
}

void MainWin::updateScene(BasicCamera& camera)
{
	auto& cam = dynamic_cast<Camera&>(camera);

	clock.update();

	cam.updateUT(clock.getCurrentUt());
	systemRenderer->updateMesh(clock.getCurrentUt(), cam.getAbsolutePosition());
}

void MainWin::renderScene(BasicCamera const& camera)
{
	Q_UNUSED(camera);
	stars.render();
	systemRenderer->render();
}

MainWin::~MainWin()
{
	delete systemRenderer;
	delete orbitalSystem;
}
