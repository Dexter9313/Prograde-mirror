#include "MainWin.hpp"

MainWin::MainWin()
    : AbstractMainWin()
{
	QString planetsystemdir(
	    QSettings().value("simulation/planetsystemdir").toString());

	srand(time(nullptr));
	unsigned int tries(
	    QSettings().value("simulation/randomsystem").toBool() ? 3 : 1);

	while(tries > 0)
	{
		tries--;
		QFile jsonFile;
		if(QSettings().value("simulation/randomsystem").toBool())
		{
			QStringList nameFilter;
			nameFilter << "*.json";

			QStringList files;
			QDirIterator it(planetsystemdir, QStringList() << "*.json",
			                QDir::Files, QDirIterator::Subdirectories);
			while(it.hasNext())
			{
				files << it.next();
			}

			jsonFile.setFileName(files[rand() % files.size()]);
		}
		else
		{
			jsonFile.setFileName(planetsystemdir + "/definition.json");
		}

		if(jsonFile.exists())
		{
			jsonFile.open(QIODevice::ReadOnly);
			QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
			QString name(QFileInfo(jsonFile).dir().dirName());
			orbitalSystem
			    = new OrbitalSystem(name.toStdString(), jsonDoc.object());
			if(!orbitalSystem->isValid())
			{
				std::cerr << orbitalSystem->getName() << " is invalid... ";
				delete orbitalSystem;
				if(tries > 0)
				{
					std::cerr << "Trying another one..." << std::endl;
				}
				else
				{
					std::cerr << "All tries done. Shuting down..." << std::endl;
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				tries = 0;
			}
		}
		else
		{
			QMessageBox::critical(
			    nullptr, tr("Invalid data directory"),
			    tr("The planetary system root directory doesn't "
			       "contain any definition.json file."));
			exit(EXIT_FAILURE);
		}
	}

	auto barycenters = orbitalSystem->getAllBinariesNames();
	auto stars       = orbitalSystem->getAllStarsNames();
	auto fcPlanets   = orbitalSystem->getAllFirstClassPlanetsNames();
	auto satellites  = orbitalSystem->getAllSatellitePlanetsNames();

	std::cout << "-=-=- SYSTEM " << orbitalSystem->getName() << " -=-=-"
	          << std::endl;
	std::cout << "Barycenters : " << barycenters.size() << std::endl;
	for(auto name : barycenters)
	{
		std::cout << name << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Stars : " << stars.size() << std::endl;
	for(auto name : stars)
	{
		std::cout << name << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Main Planets : " << fcPlanets.size() << std::endl;
	for(auto name : fcPlanets)
	{
		std::cout << name << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Satellites : " << satellites.size() << std::endl;
	for(auto name : satellites)
	{
		std::cout << name << "("
		          << (*orbitalSystem)[name]->getParent()->getName() << ")"
		          << std::endl;
	}
	std::cout << std::endl;
}

void MainWin::keyPressEvent(QKeyEvent* e)
{
	auto cam(dynamic_cast<OrbitalSystemCamera*>(&getCamera()));
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

		if(cam->relativePosition.length()
		   >= cam->target->getSphereOfInfluenceRadius())
		{
			cam->relativePosition = cam->relativePosition.getUnitForm()
			                        * cam->target->getSphereOfInfluenceRadius()
			                        / 2.0;
		}

		if(cam->relativePosition.length()
		   <= cam->target->getCelestialBodyParameters().radius)
		{
			cam->relativePosition
			    = cam->relativePosition.getUnitForm()
			      * cam->target->getCelestialBodyParameters().radius * 2.0;
		}
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

		if(cam->relativePosition.length()
		   >= cam->target->getSphereOfInfluenceRadius())
		{
			cam->relativePosition = Vector3(
			    cam->target->getSphereOfInfluenceRadius() / 2.0, 0.0, 0.0);
		}

		if(cam->relativePosition.length()
		   <= cam->target->getCelestialBodyParameters().radius)
		{
			cam->relativePosition
			    = cam->relativePosition.getUnitForm()
			      * cam->target->getCelestialBodyParameters().radius * 2.0;
		}
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
	// CONTROLS
	else if(e->key() == Qt::Key_C)
	{
		Vector3 unitRelPos(cam->relativePosition.getUnitForm());
		cam->yaw   = atan2(unitRelPos[1], unitRelPos[0]);
		cam->pitch = -1.0 * asin(unitRelPos[2]);
	}
	else if(e->key() == Qt::Key_W || e->key() == Qt::Key_Up)
	{
		negativeVelocity.setZ(-1);
	}
	else if(e->key() == Qt::Key_A || e->key() == Qt::Key_Left)
	{
		negativeVelocity.setX(-1);
	}
	else if(e->key() == Qt::Key_S || e->key() == Qt::Key_Down)
	{
		positiveVelocity.setZ(1);
	}
	else if(e->key() == Qt::Key_D || e->key() == Qt::Key_Right)
	{
		positiveVelocity.setX(1);
	}

	AbstractMainWin::keyPressEvent(e);
}

void MainWin::keyReleaseEvent(QKeyEvent* e)
{
	// CONTROLS
	if(e->key() == Qt::Key_W || e->key() == Qt::Key_Up)
	{
		negativeVelocity.setZ(0);
	}
	else if(e->key() == Qt::Key_A || e->key() == Qt::Key_Left)
	{
		negativeVelocity.setX(0);
	}
	else if(e->key() == Qt::Key_S || e->key() == Qt::Key_Down)
	{
		positiveVelocity.setZ(0);
	}
	else if(e->key() == Qt::Key_D || e->key() == Qt::Key_Right)
	{
		positiveVelocity.setX(0);
	}

	AbstractMainWin::keyReleaseEvent(e);
}

void MainWin::mousePressEvent(QMouseEvent* e)
{
	if(e->button() == Qt::MouseButton::LeftButton)
	{
		rotateViewEnabled = true;
	}
	else if(e->button() == Qt::MouseButton::RightButton)
	{
		trackballEnabled = true;
	}
	if(rotateViewEnabled && trackballEnabled)
	{
		OrbitalSystemRenderer::autoCameraTarget = false;
	}
}

void MainWin::mouseReleaseEvent(QMouseEvent* e)
{
	if(e->button() == Qt::MouseButton::LeftButton)
	{
		rotateViewEnabled = false;
	}
	else if(e->button() == Qt::MouseButton::RightButton)
	{
		trackballEnabled = false;
	}
	OrbitalSystemRenderer::autoCameraTarget = true;
}

void MainWin::mouseMoveEvent(QMouseEvent* e)
{
	if(!rotateViewEnabled && !trackballEnabled)
	{
		return;
	}

	auto cam(dynamic_cast<OrbitalSystemCamera*>(&getCamera()));
	float dx = (static_cast<float>(width()) / 2 - e->globalX()) / width();
	float dy = (static_cast<float>(height()) / 2 - e->globalY()) / height();

	// means both mouse buttons are clicked
	if(rotateViewEnabled && trackballEnabled)
	{
		double radius(cam->target->getCelestialBodyParameters().radius);
		double alt(cam->relativePosition.length() - radius);
		alt *= 1.f - dy;
		cam->relativePosition
		    = cam->relativePosition.getUnitForm() * (radius + alt);
	}
	else if(rotateViewEnabled || trackballEnabled)
	{
		double dYaw(dx * constant::pi / 3.0), dPitch(dy * constant::pi / 3.0);
		if(cam->pitch + dPitch > M_PI_2 - 0.20)
		{
			dPitch = M_PI_2 - 0.20 - cam->pitch;
		}
		if(cam->pitch + dPitch < -1.f * M_PI_2 + 0.2)
		{
			dPitch = -1.f * M_PI_2 + 0.2 - cam->pitch;
		}
		cam->yaw += dYaw;
		cam->pitch += dPitch;
		if(trackballEnabled)
		{
			cam->relativePosition.rotateAlongZ(dYaw);
			double posPitch(asin(cam->relativePosition.getUnitForm()[2]));
			if(dPitch - posPitch < -1.0 * M_PI_2 + 0.01)
			{
				dPitch = -1.0 * M_PI_2 + 0.01 + posPitch;
			}
			else if(dPitch - posPitch > M_PI_2 - 0.01)
			{
				dPitch = M_PI_2 - 0.01 + posPitch;
			}

			Vector3 axis(
			    crossProduct(Vector3(0.0, 0.0, 1.0), cam->relativePosition));
			Matrix4x4 rot(dPitch, axis);
			cam->relativePosition = rot * cam->relativePosition;
		}
	}
	QCursor::setPos(width() / 2, height() / 2);
}

void MainWin::wheelEvent(QWheelEvent* e)
{
	velMag *= 1.f + e->angleDelta().y() / 1000.f;

	AbstractMainWin::wheelEvent(e);
}

void MainWin::vrEvent(VRHandler::Event const& e)
{
	auto cam(dynamic_cast<OrbitalSystemCamera*>(&getCamera()));
	switch(e.type)
	{
		case VRHandler::EventType::BUTTON_PRESSED:
			switch(e.button)
			{
				case VRHandler::Button::GRIP:
				{
					OrbitalSystemRenderer::autoCameraTarget = false;
					Controller const* left(vrHandler.getController(Side::LEFT));
					Controller const* right(
					    vrHandler.getController(Side::RIGHT));
					if(e.side == Side::LEFT && left != nullptr)
					{
						leftGripPressed = true;
						initControllerRelPos
						    = Utils::fromQt(
						          getCamera().trackedSpaceToWorldTransform()
						          * left->getPosition())
						          / CelestialBodyRenderer::overridenScale
						      + cam->relativePosition;
					}
					else if(e.side == Side::RIGHT && right != nullptr)
					{
						rightGripPressed = true;
						initControllerRelPos
						    = Utils::fromQt(
						          getCamera().trackedSpaceToWorldTransform()
						          * right->getPosition())
						          / CelestialBodyRenderer::overridenScale
						      + cam->relativePosition;
					}
					else
					{
						break;
					}
					if(leftGripPressed && rightGripPressed && left != nullptr
					   && right != nullptr)
					{
						initControllersDistance
						    = left->getPosition().distanceToPoint(
						        right->getPosition());
						initScale = CelestialBodyRenderer::overridenScale;

						QVector3D controllersMidPoint;
						controllersMidPoint
						    = left->getPosition() + right->getPosition();
						controllersMidPoint /= 2.f;

						controllersMidPoint
						    = getCamera().trackedSpaceToWorldTransform()
						      * controllersMidPoint;
						scaleCenter
						    = Utils::fromQt(controllersMidPoint)
						          / CelestialBodyRenderer::overridenScale
						      + cam->relativePosition;
					}
					break;
				}
				default:
					break;
			}
			break;
		case VRHandler::EventType::BUTTON_UNPRESSED:
			switch(e.button)
			{
				case VRHandler::Button::GRIP:
				{
					Controller const* left(vrHandler.getController(Side::LEFT));
					Controller const* right(
					    vrHandler.getController(Side::RIGHT));
					if(e.side == Side::LEFT)
					{
						leftGripPressed = false;
						if(right != nullptr && rightGripPressed)
						{
							initControllerRelPos
							    = Utils::fromQt(
							          getCamera().trackedSpaceToWorldTransform()
							          * right->getPosition())
							          / CelestialBodyRenderer::overridenScale
							      + cam->relativePosition;
						}
						else
						{
							OrbitalSystemRenderer::autoCameraTarget = true;
						}
					}
					else if(e.side == Side::RIGHT)
					{
						rightGripPressed = false;
						if(left != nullptr && leftGripPressed)
						{
							initControllerRelPos
							    = Utils::fromQt(
							          getCamera().trackedSpaceToWorldTransform()
							          * left->getPosition())
							          / CelestialBodyRenderer::overridenScale
							      + cam->relativePosition;
						}
						else
						{
							OrbitalSystemRenderer::autoCameraTarget = true;
						}
					}
					break;
				}
				default:
					break;
			}
			break;
		default:
			break;
	}

	AbstractMainWin::vrEvent(e);
}

void MainWin::initScene()
{
	QCursor c(cursor());
	c.setShape(Qt::CursorShape::BlankCursor);
	setCursor(c);

	clock.setTargetFPS(0.f);
	stars.initFromFile(23.4392811 * constant::pi / 180.f);

	debugText = new Text3D(textWidth, textHeight);
	debugText->setColor(QColor(255, 0, 0));

	auto cam = new OrbitalSystemCamera(&vrHandler);
	cam->setPerspectiveProj(70.0f, static_cast<float>(width())
	                                   / static_cast<float>(height()));
	cam->target           = orbitalSystem->getAllCelestialBodiesPointers()[0];
	cam->relativePosition = Vector3(
	    cam->target->getCelestialBodyParameters().radius * 2.0, 0.0, 0.0);
	systemRenderer = new OrbitalSystemRenderer(orbitalSystem);
	setCamera(cam);

	CelestialBodyRenderer::overridenScale = 1.0;
}

void MainWin::updateScene(BasicCamera& camera)
{
	auto& cam = dynamic_cast<OrbitalSystemCamera&>(camera);

	clock.update();
	/*if(!clock.drawableFrame())
	{
	    return;
	}*/

	cam.updateUT(clock.getCurrentUt());

	if(!vrHandler)
	{
		CelestialBodyRenderer::overridenScale = 1.0;
	}

	Controller const* left(vrHandler.getController(Side::LEFT));
	Controller const* right(vrHandler.getController(Side::RIGHT));

	// single grip = translation
	if(leftGripPressed != rightGripPressed)
	{
		Vector3 controllerRelPos;
		if(leftGripPressed && left != nullptr)
		{
			controllerRelPos = Utils::fromQt(cam.trackedSpaceToWorldTransform()
			                                 * left->getPosition())
			                       / CelestialBodyRenderer::overridenScale
			                   + cam.relativePosition;
		}
		else if(rightGripPressed && right != nullptr)
		{
			controllerRelPos = Utils::fromQt(cam.trackedSpaceToWorldTransform()
			                                 * right->getPosition())
			                       / CelestialBodyRenderer::overridenScale
			                   + cam.relativePosition;
		}
		cam.relativePosition -= controllerRelPos - initControllerRelPos;
	}
	// double grip = scale
	if(leftGripPressed && rightGripPressed && left != nullptr
	   && right != nullptr)
	{
		rescale(initScale
		            * left->getPosition().distanceToPoint(right->getPosition())
		            / initControllersDistance,
		        scaleCenter);
	}

	// apply keyboard controls
	for(unsigned int i(0); i < 3; ++i)
	{
		cam.relativePosition[i]
		    += frameTiming
		       * (cam.getView().inverted()
		          * (negativeVelocity + positiveVelocity))[i]
		       * velMag;
	}

	systemRenderer->updateMesh(clock.getCurrentUt(), cam);

	std::stringstream stream;
	stream.precision(3);
	// stream << clock.getCurrentFPS() << " FPS" << std::endl;
	stream << round(1.f / frameTiming) << " FPS" << std::endl;
	stream << "Targeting : " << cam.target->getName() << std::endl;
	stream.precision(10);
	stream
	    << "Altitude : "
	    << lengthPrettyPrint(cam.relativePosition.length()
	                         - cam.target->getCelestialBodyParameters().radius)
	           .first
	    << " "
	    << lengthPrettyPrint(cam.relativePosition.length()
	                         - cam.target->getCelestialBodyParameters().radius)
	           .second
	    << std::endl;
	stream.precision(4);
	stream << "UT = " << SimulationTime::UTToStr(clock.getCurrentUt())
	       << std::endl;
	stream.precision(12);
	stream << "Raw UT = " << floor(clock.getCurrentUt() * 10) / 10 << std::endl;
	stream.precision(8);
	stream << "x" << clock.getTimeCoeff()
	       << (clock.getLockedRealTime() ? " (locked)" : "") << std::endl;
	stream << "Velocity : " << lengthPrettyPrint(velMag).first << " "
	       << lengthPrettyPrint(velMag).second << "/s" << std::endl;
	if(vrHandler)
	{
		stream.precision(4);
		stream << "Scale : 1 real meter = "
		       << lengthPrettyPrint(1.0 / CelestialBodyRenderer::overridenScale)
		              .first
		       << " "
		       << lengthPrettyPrint(1.0 / CelestialBodyRenderer::overridenScale)
		              .second
		       << std::endl;
	}
	/*
	// GPU memory usage
	stream << std::endl << std::endl;
	stream << "Render Targets count : " << GLHandler::renderTargetCount() <<
	std::endl; stream << "Shaders count : " << GLHandler::shaderCount() <<
	std::endl; stream << "Meshs count : " << GLHandler::meshCount() <<
	std::endl; stream << "Textures count : " << GLHandler::texCount() <<
	std::endl; stream << "PBOs count : " << GLHandler::PBOCount() <<
	std::endl;
	*/

	if(vrHandler)
	{
		debugText->getModel() = cam.hmdSpaceToWorldTransform();
		debugText->getModel().translate(QVector3D(0.0f, -0.15f, -0.4f));
		debugText->getModel().scale(
		    1.5 * static_cast<float>(textWidth) / width(),
		    1.5 * static_cast<float>(textHeight) / height());
	}
	else
	{
		debugText->getModel() = cam.screenToWorldTransform();
		debugText->getModel().translate(QVector3D(-0.88f, 0.88f, 0.f));
		debugText->getModel().scale(2 * static_cast<float>(textWidth) / width(),
		                            2 * static_cast<float>(textWidth)
		                                / height());
	}

	timeSinceTextUpdate += frameTiming;
	if(timeSinceTextUpdate > 0.2)
	{
		debugText->setText(stream.str().c_str());
		timeSinceTextUpdate = 0.f;
	}
}

void MainWin::renderScene(BasicCamera const& camera)
{
	GLHandler::setPointSize(1);
	stars.render();
	systemRenderer->render(camera);
	debugText->render();
}

void MainWin::rescale(double newScale, Vector3 const& scaleCenter)
{
	auto& cam(dynamic_cast<OrbitalSystemCamera&>(getCamera()));
	Vector3 diff(cam.relativePosition - scaleCenter);
	diff /= newScale / CelestialBodyRenderer::overridenScale;
	cam.relativePosition                  = scaleCenter + diff;
	CelestialBodyRenderer::overridenScale = newScale;
}

MainWin::~MainWin()
{
	delete systemRenderer;
	delete orbitalSystem;
}

std::pair<double, std::string> MainWin::lengthPrettyPrint(double length)
{
	// m
	if(fabs(length) < 500.0)
	{
		return {length, "m"};
	}
	// km
	length /= 1000.0;
	if(fabs(length) < 1495979.0) // 0.01AU
	{
		return {length, "km"};
	}
	// AU
	length /= 149597871.0;
	if(length < 50000.0)
	{
		return {length, "AU"};
	}
	// ly
	length /= 63241.1;
	return {length, "ly"};
}
