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
	else if(e->key() == Qt::Key_Up)
	{
		CelestialBodyRenderer::overridenScale *= 1.2;
	}
	else if(e->key() == Qt::Key_Down)
	{
		CelestialBodyRenderer::overridenScale /= 1.2;
	}
	// CONTROLS
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
	/*if(e->button() != Qt::MouseButton::RightButton)
	{
	    return;
	}

	lastCursorPos = QCursor::pos();
	QCursor::setPos(width() / 2, height() / 2);
	QCursor c(cursor());
	c.setShape(Qt::CursorShape::BlankCursor);
	setCursor(c);
	trackballEnabled = true;*/
}

void MainWin::mouseReleaseEvent(QMouseEvent* e)
{
	/*if(e->button() != Qt::MouseButton::RightButton)
	{
	    return;
	}

	trackballEnabled = false;
	QCursor c(cursor());
	c.setShape(Qt::CursorShape::ArrowCursor);
	setCursor(c);
	QCursor::setPos(lastCursorPos);*/
}

/*
void MainWin::mouseMoveEvent(QMouseEvent* e)
{
    if(!trackballEnabled)
    {
        return;
    }
    auto cam(dynamic_cast<OrbitalSystemCamera*>(&getCamera()));
    float dx = (static_cast<float>(width()) / 2 - e->globalX()) / width();
    float dy = (static_cast<float>(height()) / 2 - e->globalY()) / height();
    cam->angleAroundZ += dx * 3.14f / 3.f;
    cam->angleAboveXY += dy * 3.14f / 3.f;
    QCursor::setPos(width() / 2, height() / 2);
}
*/
void MainWin::mouseMoveEvent(QMouseEvent* e)
{
	if(!isActive() || vrHandler)
	{
		return;
	}
	auto cam(dynamic_cast<OrbitalSystemCamera*>(&getCamera()));
	float dx = (static_cast<float>(width()) / 2 - e->globalX()) / width();
	float dy = (static_cast<float>(height()) / 2 - e->globalY()) / height();
	cam->yaw += dx * 3.14f / 3.f;
	cam->pitch += dy * 3.14f / 3.f;
	QCursor::setPos(width() / 2, height() / 2);
}

void MainWin::wheelEvent(QWheelEvent* e)
{
	/*auto cam(dynamic_cast<OrbitalSystemCamera*>(&getCamera()));
	if(e->angleDelta().y() > 0.f)
	{
	    cam->distance /= 1.2f; // + (0.2f * e->angleDelta().y() / 1000.f);
	}
	else
	{
	    cam->distance *= 1.2f; // - (0.2f * e->angleDelta().y() / 1000.f);
	}*/
	CelestialBodyRenderer::overridenScale *= 1.f + e->angleDelta().y() / 1000.f;

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
					Controller const* left(vrHandler.getController(Side::LEFT));
					Controller const* right(
					    vrHandler.getController(Side::RIGHT));
					if(e.side == Side::LEFT && left != nullptr)
					{
						leftGripPressed = true;
						initControllerPosInCube
						    = Utils::fromQt(
						          getCamera().trackedSpaceToWorldTransform()
						          * left->getPosition())
						          / CelestialBodyRenderer::overridenScale
						      - cam->relativePosition;
					}
					else if(e.side == Side::RIGHT && right != nullptr)
					{
						rightGripPressed = true;
						initControllerPosInCube
						    = Utils::fromQt(
						          getCamera().trackedSpaceToWorldTransform()
						          * right->getPosition())
						          / CelestialBodyRenderer::overridenScale
						      - cam->relativePosition;
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
						scaleCenter = Utils::fromQt(controllersMidPoint);

						QVector3D controllersMidPointInCube;
						controllersMidPointInCube
						    = (controllersMidPoint
						       - Utils::toQt(cam->relativePosition))
						      / CelestialBodyRenderer::overridenScale;
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
							initControllerPosInCube
							    = Utils::fromQt(
							          getCamera().trackedSpaceToWorldTransform()
							          * right->getPosition())
							      - cam->relativePosition;
						}
					}
					else if(e.side == Side::RIGHT)
					{
						rightGripPressed = false;
						if(left != nullptr && leftGripPressed)
						{
							initControllerPosInCube
							    = Utils::fromQt(
							          getCamera().trackedSpaceToWorldTransform()
							          * left->getPosition())
							      - cam->relativePosition;
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
	cam->target    = orbitalSystem->getAllCelestialBodiesPointers()[0];
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

	Controller const* left(vrHandler.getController(Side::LEFT));
	Controller const* right(vrHandler.getController(Side::RIGHT));

	// single grip = translation
	if(leftGripPressed != rightGripPressed)
	{
		Vector3 controllerPosInCube;
		if(leftGripPressed && left != nullptr)
		{
			controllerPosInCube
			    = Utils::fromQt(getCamera().trackedSpaceToWorldTransform()
			                    * left->getPosition())
			          / CelestialBodyRenderer::overridenScale
			      - cam.relativePosition;
		}
		else if(rightGripPressed && right != nullptr)
		{
			controllerPosInCube
			    = Utils::fromQt(getCamera().trackedSpaceToWorldTransform()
			                    * right->getPosition())
			          / CelestialBodyRenderer::overridenScale
			      - cam.relativePosition;
		}
		cam.relativePosition -= controllerPosInCube - initControllerPosInCube;
	}
	// double grip = scale
	if(leftGripPressed && rightGripPressed && left != nullptr
	   && right != nullptr)
	{
		rescaleCube(
		    initScale
		        * left->getPosition().distanceToPoint(right->getPosition())
		        / initControllersDistance,
		    scaleCenter);
	}

	// apply keyboard controls
	if(!vrHandler)
	{
		for(unsigned int i(0); i < 3; ++i)
		{
			cam.relativePosition[i]
			    += frameTiming
			       * (cam.getView().inverted()
			          * (negativeVelocity + positiveVelocity))[i]
			       / CelestialBodyRenderer::overridenScale;
		}
	}

	systemRenderer->updateMesh(clock.getCurrentUt(), cam);

	std::stringstream stream;
	stream.precision(3);
	// stream << clock.getCurrentFPS() << " FPS" << std::endl;
	stream << round(1.f / frameTiming) << " FPS" << std::endl;
	stream << "Targeting : " << cam.target->getName() << std::endl;
	stream.precision(10);
	stream << "Distance : " << cam.relativePosition.length() << std::endl;
	stream.precision(4);
	stream << "UT = " << SimulationTime::UTToStr(clock.getCurrentUt())
	       << std::endl;
	stream.precision(12);
	stream << "Raw UT = " << floor(clock.getCurrentUt() * 10) / 10 << std::endl;
	stream.precision(8);
	stream << "x" << clock.getTimeCoeff();
	if(clock.getLockedRealTime())
	{
		stream << " (locked)";
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
	GLHandler::setPointSize(1);
	stars.render();
	systemRenderer->render(camera);
	debugText->render();
}

void MainWin::rescaleCube(double newScale, Vector3 const& scaleCenter)
{
	auto& cam(dynamic_cast<OrbitalSystemCamera&>(getCamera()));
	Vector3 scaleCenterPosInCube;
	scaleCenterPosInCube = scaleCenter - cam.relativePosition;
	scaleCenterPosInCube *= newScale / CelestialBodyRenderer::overridenScale;
	// cam.relativePosition                  = scaleCenter -
	// scaleCenterPosInCube;
	CelestialBodyRenderer::overridenScale = newScale;
}

MainWin::~MainWin()
{
	delete systemRenderer;
	delete orbitalSystem;
}
