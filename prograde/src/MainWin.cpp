#include "MainWin.hpp"

MainWin::MainWin()
    : AbstractMainWin()
{
	QFile jsonFile(QSettings().value("simulation/planetsystemdir").toString()
	               + "/definition.json");
	if(jsonFile.exists())
	{
		jsonFile.open(QIODevice::ReadOnly);
		QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
		orbitalSystem         = new OrbitalSystem(jsonDoc.object());
	}
	else
	{
		QMessageBox::critical(nullptr, tr("Invalid data directory"),
		                      tr("The planetary system root directory doesn't "
		                         "contain any definition.json file."));
		exit(EXIT_FAILURE);
	}
}

void MainWin::keyPressEvent(QKeyEvent* e)
{
	auto cam(dynamic_cast<Camera*>(&getCamera()));
	if(e->key() == Qt::Key_Tab)
	{
		std::vector<CelestialBody*> bodies(
		    orbitalSystem->getAllCelestialBodiesPointers());
		bodyTracked++;
		if(bodyTracked == bodies.size())
		{
			bodyTracked = 0;
		}
		cam->target = bodies[bodyTracked];
	}
	else if(e->key() == Qt::Key_Backtab)
	{
		std::vector<CelestialBody*> bodies(
		    orbitalSystem->getAllCelestialBodiesPointers());
		if(bodyTracked > 0)
		{
			bodyTracked--;
		}
		else
		{
			bodyTracked = bodies.size() - 1;
		}
		cam->target = bodies[bodyTracked];
	}
	else if(e->key() == Qt::Key_R)
	{
		float timeCoeff(clock.getTimeCoeff());
		if(timeCoeff >= 10.f)
		{
			timeCoeff /= 10.f;
		}
		clock.setTimeCoeff(timeCoeff);
	}
	else if(e->key() == Qt::Key_T)
	{
		float timeCoeff(clock.getTimeCoeff());
		if(timeCoeff <= 100000.f)
		{
			timeCoeff *= 10.f;
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

	debugText = new Text3D(textWidth, textHeight);
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
	systemRenderer->updateMesh(clock.getCurrentUt(), cam);

	std::stringstream stream;
	stream.precision(3);
	// stream << clock.getCurrentFPS() << " FPS" << std::endl;
	stream << round(1.f / frameTiming) << " FPS" << std::endl;
	stream << "Targeting : "
	       << orbitalSystem->getAllCelestialBodiesNames()[bodyTracked]
	       << std::endl;
	stream.precision(10);
	stream << "Distance : " << cam.distance << std::endl;
	stream.precision(4);
	stream << "UT = " << timeToStr(clock.getCurrentUt()) << std::endl;
	stream.precision(12);
	stream << "Raw UT = " << floor(clock.getCurrentUt() * 10) / 10 << std::endl;
	stream.precision(8);
	stream << "x" << clock.getTimeCoeff();
	/*
	// GPU memory usage
	stream << std::endl << std::endl;
	stream << "Render Targets count : " << GLHandler::renderTargetCount() <<
	std::endl; stream << "Shaders count : " << GLHandler::shaderCount() <<
	std::endl; stream << "Meshs count : " << GLHandler::meshCount() <<
	std::endl; stream << "Textures count : " << GLHandler::texCount() <<
	std::endl; stream << "PBOs count : " << GLHandler::PBOCount() << std::endl;
	*/

	debugText->getModel() = cam.screenToWorldTransform();
	debugText->getModel().translate(QVector3D(-0.88f, 0.9f, 0.f));
	debugText->getModel().scale(2 * static_cast<float>(textWidth) / width(),
	                            2 * static_cast<float>(textWidth) / height());

	timeSinceTextUpdate += frameTiming;
	if(timeSinceTextUpdate > 0.2)
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

UniversalTime MainWin::loadStartUT()
{
	QDateTime startDateTime(
	    QSettings().value("simulation/starttime").value<QDateTime>());

	UniversalTime result(startDateTime.date().toJulianDay() - 2451545.0);
	result *= 24 * 3600;
	QTime t(startDateTime.toLocalTime().time());
	result += t.hour() * 3600.0;
	result += t.minute() * 60.0;
	result += t.second();

	return result;
}

std::string MainWin::timeToStr(UniversalTime uT)
{
	uT /= 24.0 * 3600.0;
	auto j2000d(static_cast<int64_t>(uT));
	if(uT < 0)
	{
		--j2000d;
	}
	uT -= j2000d;

	UniversalTime timeOfDayInMSecs(uT * 24.0 * 3600.0 * 1000.0);

	QDateTime dt(
	    QDate::fromJulianDay(j2000d + 2451545),
	    QTime::fromMSecsSinceStartOfDay(static_cast<int>(timeOfDayInMSecs)),
	    Qt::UTC);

	QString seconds
	    = QString("%1").arg(dt.toUTC().time().second(), 2, 10, QChar('0'));

	return (dt.toUTC().toString(Qt::SystemLocaleShortDate) + ":" + seconds)
	    .toStdString();
}
