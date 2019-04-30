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
		{
			bodyTracked = 0;
		}
		cam->target = bodies[bodyTracked];
	}
	else if(e->key() == Qt::Key_R)
	{
		float timeCoeff(clock.getTimeCoeff());
		if(timeCoeff >= 10)
		{
			timeCoeff /= 10;
		}
		clock.setTimeCoeff(timeCoeff);
	}
	else if(e->key() == Qt::Key_T)
	{
		float timeCoeff(clock.getTimeCoeff());
		if(timeCoeff <= 100000)
		{
			timeCoeff *= 10;
		}
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
		cam->distance /= 1.2f; // + (0.2f * e->angleDelta().y() / 1000.f);
	}
	else
	{
		cam->distance *= 1.2f; // - (0.2f * e->angleDelta().y() / 1000.f);
	}
	AbstractMainWin::wheelEvent(e);
}

void MainWin::initScene()
{
	stars.initFromFile(23.4392811 * constant::pi / 180.f);

	debugText = new Text3D(1920, 1080);
	debugText->setColor(QColor(255, 0, 0));

	auto cam = new Camera(&vrHandler);
	cam->setPerspectiveProj(70.0f, static_cast<float>(width())
	                                   / static_cast<float>(height()));
	cam->target    = orbitalSystem->getAllCelestialBodiesPointers()[0];
	systemRenderer = new OrbitalSystemRenderer(orbitalSystem);
	setCamera(cam);
}

void MainWin::updateScene(BasicCamera& camera)
{
	auto& cam = dynamic_cast<Camera&>(camera);

	clock.update();
	/*if(!clock.drawableFrame())
	{
	    return;
	}*/

	cam.updateUT(clock.getCurrentUt());
	systemRenderer->updateMesh(clock.getCurrentUt(), cam.getAbsolutePosition());

	std::stringstream stream;
	stream.precision(2);
	// stream << clock.getCurrentFPS() << " FPS" << std::endl;
	stream << 1.f / frameTiming << " FPS" << std::endl;
	stream << "Targeting : "
	       << orbitalSystem->getAllCelestialBodiesNames()[bodyTracked]
	       << std::endl;
	stream.precision(10);
	stream << "Distance : "
	       << (cam.getAbsolutePosition()
	           - orbitalSystem->getAllCelestialBodiesPointers()[bodyTracked]
	                 ->getAbsolutePositionAtUT(clock.getCurrentUt()))
	                  .length()
	              - orbitalSystem->getAllCelestialBodiesPointers()[bodyTracked]
	                    ->getParameters()
	                    .radius
	       << std::endl;
	stream.precision(4);
	stream << "UT = " << timeToStr(clock.getCurrentUt()) << std::endl;
	stream.precision(300);
	stream << " Raw UT = " << floor(clock.getCurrentUt() * 10) / 10
	       << std::endl;
	stream.precision(8);
	stream << "x" << clock.getTimeCoeff();

	debugText->getModel() = cam.screenToWorldTransform();
	debugText->getModel().scale(1.98f, 3.5f);

	timeSinceTextUpdate += frameTiming;
	if(timeSinceTextUpdate > 1.0)
	{
		debugText->setText(stream.str().c_str());
		timeSinceTextUpdate = 0.f;
	}
}

void MainWin::renderScene(BasicCamera const& camera)
{
	stars.render();
	systemRenderer->render(camera);
	debugText->render();
}

MainWin::~MainWin()
{
	delete systemRenderer;
	delete orbitalSystem;
}

#include <cmath>
std::string MainWin::timeToStr(UniversalTime uT)
{
	UniversalTime uT2 = floor(uT);
	auto time(uT2.convert_to<int64_t>());
	unsigned int sec, min, hour, day, month(0), year(1999);
	sec = time % 60;
	time -= sec;
	min = (time / 60) % 60;
	time -= min * 60;
	hour = (12 + (time / 3600)) % 24;
	time -= hour * 3600;
	day = time / (24 * 3600);

	int daytmp(day);

	while(daytmp >= 0)
	{
		year++;
		daytmp -= 365;

		if(daytmp < 0)
		{
			daytmp += 365;
			break;
		}

		year++;
		daytmp -= 365;

		if(daytmp < 0)
		{
			daytmp += 365;
			break;
		}

		year++;
		daytmp -= 365;

		if(daytmp < 0)
		{
			daytmp += 365;
			break;
		}

		year++;
		daytmp -= 366;

		if(daytmp < 0)
		{
			daytmp += 366;
			break;
		}
	}

	day = daytmp;

	computeDayMonth(&day, &month, (year % 4) == 0);

	std::stringstream stream;
	stream << day << " "
	       << " " << month << " " << year << " " << hour << ":" << min << ":"
	       << sec;
	return stream.str();
}

void MainWin::computeDayMonth(unsigned int* day, unsigned int* month,
                              bool bissextile)
{
	if(*day < 31)
	{
		*day += 1;
		*month = 1;
		return;
	}

	*day -= 31;

	if((*day < 28 && !bissextile) || (*day < 29 && bissextile))
	{
		*day += 1;
		*month = 2;
		return;
	}

	*day -= bissextile ? 29 : 28;

	if(*day < 31)
	{
		*day += 1;
		*month = 3;
		return;
	}

	*day -= 31;

	if(*day < 30)
	{
		*day += 1;
		*month = 4;
		return;
	}

	*day -= 30;

	if(*day < 31)
	{
		*day += 1;
		*month = 5;
		return;
	}

	*day -= 31;

	if(*day < 30)
	{
		*day += 1;
		*month = 6;
		return;
	}

	*day -= 30;

	if(*day < 31)
	{
		*day += 1;
		*month = 7;
		return;
	}

	*day -= 31;

	if(*day < 31)
	{
		*day += 1;
		*month = 8;
		return;
	}

	*day -= 31;

	if(*day < 30)
	{
		*day += 1;
		*month = 9;
		return;
	}

	*day -= 30;

	if(*day < 31)
	{
		*day += 1;
		*month = 10;
		return;
	}

	*day -= 31;

	if(*day < 30)
	{
		*day += 1;
		*month = 11;
		return;
	}

	*day -= 30;

	if(*day < 31)
	{
		*day += 1;
		*month = 12;
		return;
	}
}
