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
			PlanetRenderer::currentSystemDir = planetsystemdir;
			CSVOrbit::currentSystemDir       = planetsystemdir;

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
	auto spacecrafts = orbitalSystem->getAllSpacecraftsNames();

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

	std::cout << "Spacecrafts : " << spacecrafts.size() << std::endl;
	for(auto name : spacecrafts)
	{
		std::cout << name << "("
		          << (*orbitalSystem)[name]->getParent()->getName() << ")"
		          << std::endl;
	}
	std::cout << std::endl;
}

void MainWin::selectOrbitable(QTreeWidgetItem* item, int column)
{
	auto cam(dynamic_cast<OrbitalSystemCamera*>(&renderer.getCamera("planet")));

	QString name(item->text(column));
	if(name.contains('('))
	{
		unsigned int pos(name.lastIndexOf('('));
		name = name.left(pos - 1);
	}
	auto orbitable = (*orbitalSystem)[name.toLatin1().data()];

	if(orbitable->getOrbit() != nullptr
	   && !orbitable->getOrbit()->isInRange(clock.getCurrentUt()))
	{
		if(orbitable->getOrbitableType() == Orbitable::Type::SPACECRAFT)
		{
			item->setTextColor(0, QColor("red"));
		}
		return;
	}
	if(orbitable->getOrbitableType() == Orbitable::Type::SPACECRAFT)
	{
		item->setTextColor(0, QColor("green"));
	}

	cam->target = dynamic_cast<CelestialBody const*>(orbitable);

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

void MainWin::actionEvent(BaseInputManager::Action a, bool pressed)
{
	if(pressed)
	{
		auto cam(
		    dynamic_cast<OrbitalSystemCamera*>(&renderer.getCamera("planet")));
		if(a.id == "nextcelestialbody")
		{
			std::vector<CelestialBody*> bodies(
			    orbitalSystem->getAllCelestialBodiesPointers());
			bodyTracked++;
			if(bodyTracked == bodies.size())
			{
				bodyTracked = 0;
			}
			cam->target = bodies[bodyTracked];

			if(cam->target->getOrbit() != nullptr
			   && !cam->target->getOrbit()->isInRange(clock.getCurrentUt()))
			{
				actionEvent({"nextcelestialbody", ""}, true);
			}

			if(cam->relativePosition.length()
			   >= cam->target->getSphereOfInfluenceRadius())
			{
				cam->relativePosition
				    = cam->relativePosition.getUnitForm()
				      * cam->target->getSphereOfInfluenceRadius() / 2.0;
			}

			if(cam->relativePosition.length()
			   <= cam->target->getCelestialBodyParameters().radius)
			{
				cam->relativePosition
				    = cam->relativePosition.getUnitForm()
				      * cam->target->getCelestialBodyParameters().radius * 2.0;
			}
		}
		else if(a.id == "prevcelestialbody")
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

			if(cam->target->getOrbit() != nullptr
			   && !cam->target->getOrbit()->isInRange(clock.getCurrentUt()))
			{
				actionEvent({"prevcelestialbody", ""}, true);
			}

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
		else if(a.id == "timecoeffdown")
		{
			float tc(clock.getTimeCoeff());
			if(tc > 1.f && !clock.getLockedRealTime())
			{
				clock.setTimeCoeff(tc / 10.f);
				debugText->setText(
				    ("Time coeff. : "
				     + std::to_string(static_cast<int>(tc / 10.f)) + "x")
				        .c_str());
				timeSinceTextUpdate = 0.f;
			}
		}
		else if(a.id == "timecoeffup")
		{
			float tc(clock.getTimeCoeff());
			if(tc < 10000000.f && !clock.getLockedRealTime())
			{
				clock.setTimeCoeff(tc * 10.f);
				debugText->setText(
				    ("Time coeff. : "
				     + std::to_string(static_cast<int>(tc * 10.f)) + "x")
				        .c_str());
				timeSinceTextUpdate = 0.f;
			}
		}
		else if(a.id == "resetvrpos")
		{
			if(vrHandler)
			{
				vrHandler.resetPos();
			}
		}
		else if(a.id == "togglelabels")
		{
			CelestialBodyRenderer::renderLabels
			    = !CelestialBodyRenderer::renderLabels;
		}
		else if(a.id == "toggleorbits")
		{
			CelestialBodyRenderer::renderOrbits
			    = !CelestialBodyRenderer::renderOrbits;
		}
		else if(a.id == "togglelock")
		{
			cam->toggleLockedOnRotation(clock.getCurrentUt());
		}
		else if(a.id == "autoexposure")
		{
			autoexposure = !autoexposure;
			if(autoexposure)
			{
				cam->dynamicrange      = 1e4f;
				cam->autoexposurecoeff = 1.f;
			}
		}
		else if(a.id == "exposureup")
		{
			if(autoexposure)
			{
				cam->autoexposurecoeff *= 1.5f;
			}
			else
			{
				cam->exposure *= 1.5f;
			}
		}
		else if(a.id == "exposuredown")
		{
			if(autoexposure)
			{
				cam->autoexposurecoeff /= 1.5f;
			}
			else
			{
				cam->exposure /= 1.5f;
			}
		}
		else if(a.id == "dynamicrangeup")
		{
			if(cam->dynamicrange < 1e37)
			{
				cam->dynamicrange *= 10.f;
				if(autoexposure)
				{
					cam->autoexposurecoeff *= 10.f;
				}
				else
				{
					cam->exposure *= 10.f;
				}
			}
		}
		else if(a.id == "dynamicrangedown")
		{
			if(cam->dynamicrange > 10.f)
			{
				cam->dynamicrange /= 10.f;
				if(autoexposure)
				{
					cam->autoexposurecoeff /= 10.f;
				}
				else
				{
					cam->exposure /= 10.f;
				}
			}
		}
		// CONTROLS
		else if(a.id == "centercam")
		{
			Vector3 unitRelPos(cam->relativePosition.getUnitForm());
			float yaw(atan2(unitRelPos[1], unitRelPos[0]));
			float pitch(cam->pitch = -1.0 * asin(unitRelPos[2]));
			cam->yaw   = yaw;
			cam->pitch = pitch;
		}
		else if(a.id == "forward")
		{
			negativeVelocity.setZ(-1);
		}
		else if(a.id == "left")
		{
			negativeVelocity.setX(-1);
		}
		else if(a.id == "backward")
		{
			positiveVelocity.setZ(1);
		}
		else if(a.id == "right")
		{
			positiveVelocity.setX(1);
		}
	}
	else
	{
		// CONTROLS
		if(a.id == "forward")
		{
			negativeVelocity.setZ(0);
		}
		else if(a.id == "left")
		{
			negativeVelocity.setX(0);
		}
		else if(a.id == "backward")
		{
			positiveVelocity.setZ(0);
		}
		else if(a.id == "right")
		{
			positiveVelocity.setX(0);
		}
	}
	AbstractMainWin::actionEvent(a, pressed);

	// reload after AbstractMainWin toggled VR
	if(pressed && a.id == "togglevr")
	{
		reloadBloomTargets();
	}
}

bool MainWin::event(QEvent* e)
{
	if(e->type() == QEvent::Type::Close)
	{
		menuBar->close();
		dialog->close();
	}
	return AbstractMainWin::event(e);
}

void MainWin::mousePressEvent(QMouseEvent* e)
{
	if(e->button() == Qt::MouseButton::LeftButton)
	{
		rotateViewEnabled = true;
		QCursor::setPos(width() / 2, height() / 2);
	}
	else if(e->button() == Qt::MouseButton::RightButton)
	{
		trackballEnabled = true;
		QCursor::setPos(width() / 2, height() / 2);
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
	lastCursorPos = e->pos();

	if(!rotateViewEnabled && !trackballEnabled)
	{
		return;
	}

	auto cam(dynamic_cast<OrbitalSystemCamera*>(&renderer.getCamera("planet")));
	float dx = (static_cast<float>(width()) / 2 - e->globalX()) / width();
	float dy = (static_cast<float>(height()) / 2 - e->globalY()) / height();

	// means both mouse buttons are clicked
	if(rotateViewEnabled && trackballEnabled)
	{
		double alt(cam->getAltitude());
		alt *= 1.f - dy;
		cam->setAltitude(alt);
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
	auto cam(dynamic_cast<OrbitalSystemCamera*>(&renderer.getCamera("planet")));
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
						          renderer.getCamera("planet")
						              .seatedTrackedSpaceToWorldTransform()
						          * left->getPosition())
						          / CelestialBodyRenderer::overridenScale
						      + cam->relativePosition;
					}
					else if(e.side == Side::RIGHT && right != nullptr)
					{
						rightGripPressed = true;
						initControllerRelPos
						    = Utils::fromQt(
						          renderer.getCamera("planet")
						              .seatedTrackedSpaceToWorldTransform()
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
						    = renderer.getCamera("planet")
						          .seatedTrackedSpaceToWorldTransform()
						      * controllersMidPoint;
						scaleCenter
						    = Utils::fromQt(controllersMidPoint)
						          / CelestialBodyRenderer::overridenScale
						      + cam->relativePosition;
					}
					break;
				}
				case VRHandler::Button::TRIGGER:
				{
					CelestialBodyRenderer::renderLabels
					    = !CelestialBodyRenderer::renderLabels;
					CelestialBodyRenderer::renderOrbits
					    = !CelestialBodyRenderer::renderOrbits;
					break;
				}
				case VRHandler::Button::TOUCHPAD:
				{
					Controller const* ctrl(vrHandler.getController(e.side));
					if(ctrl != nullptr)
					{
						QVector2D padCoords(ctrl->getPadCoords());
						if(fabsf(padCoords[0])
						   < fabsf(padCoords[1])) // UP OR DOWN
						{
							float tc(clock.getTimeCoeff());
							if(padCoords[1] < 0.0f) // DOWN
							{
								if(tc > 1.f && !clock.getLockedRealTime())
								{
									clock.setTimeCoeff(tc / 10.f);
									debugText->setText(
									    ("Time coeff. : "
									     + std::to_string(
									           static_cast<int>(tc / 10.f))
									     + "x")
									        .c_str());
									timeSinceTextUpdate = 0.f;
								}
							}
							else // UP
							{
								if(tc < 10000000.f
								   && !clock.getLockedRealTime())
								{
									clock.setTimeCoeff(tc * 10.f);
									debugText->setText(
									    ("Time coeff. : "
									     + std::to_string(
									           static_cast<int>(tc * 10.f))
									     + "x")
									        .c_str());
									timeSinceTextUpdate = 0.f;
								}
							}
						}
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
							          renderer.getCamera("planet")
							              .seatedTrackedSpaceToWorldTransform()
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
							          renderer.getCamera("planet")
							              .seatedTrackedSpaceToWorldTransform()
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

	debugText = new Text3D(textWidth, textHeight,
	                       GLHandler::newShader("billboard", "label"));
	debugText->setColor(QColor(255, 0, 0));

	auto cam = new OrbitalSystemCamera(&vrHandler);
	cam->setPerspectiveProj(70.0f, static_cast<float>(width())
	                                   / static_cast<float>(height()));
	cam->target           = orbitalSystem->getAllCelestialBodiesPointers()[0];
	cam->relativePosition = Vector3(
	    cam->target->getCelestialBodyParameters().radius * 2.0, 0.0, 0.0);
	systemRenderer = new OrbitalSystemRenderer(orbitalSystem);

	renderer.removeSceneRenderPath("default");
	renderer.appendSceneRenderPath("planet", Renderer::RenderPath(cam));

	renderer.appendPostProcessingShader("exposure", "exposure");
	renderer.appendPostProcessingShader("bloom", "bloom");

	CelestialBodyRenderer::overridenScale = 1.0;

	// we will draw them ourselves
	renderer.pathIdRenderingControllers = "";

	menuBar = new QMenuBar();
	menuBar->setWindowFlags(Qt::X11BypassWindowManagerHint
	                        | Qt::MSWindowsFixedSizeDialogHint
	                        | Qt::FramelessWindowHint);

	auto file(menuBar->addMenu(tr("File")));
	file->addAction(tr("Close"), this, [this]() { this->close(); });

	auto tools(menuBar->addMenu("Tools"));
	tools->addAction("Orbitables List", this,
	                 [this]() { this->dialog->show(); });

	menuBar->show();

	dialog = new QDialog;
	dialog->setFixedSize(250, 600);
	dialog->setWindowTitle("Orbitables List");

	auto layout = new QVBoxLayout(dialog);
	tree        = new QTreeWidget(dialog);
	tree->setColumnCount(1);
	tree->setHeaderLabel(QString());
	connect(tree, &QTreeWidget::itemActivated, this, &MainWin::selectOrbitable);
	layout->addWidget(tree);
	constructItems(orbitalSystem->getRootOrbitable());

	// DBG
	shader = GLHandler::newShader("default");
	point  = GLHandler::newMesh();
	GLHandler::setVertices(point, {0.f, 0.f, 0.f}, shader, {{"position", 3}});

	// BLOOM
	if(!vrHandler)
	{
		bloomTargets[0]
		    = GLHandler::newRenderTarget(width(), height(), GL_RGBA32F);
		bloomTargets[1]
		    = GLHandler::newRenderTarget(width(), height(), GL_RGBA32F);
	}
	else
	{
		QSize size(vrHandler.getEyeRenderTargetSize());
		bloomTargets[0] = GLHandler::newRenderTarget(size.width(),
		                                             size.height(), GL_RGBA32F);
		bloomTargets[1] = GLHandler::newRenderTarget(size.width(),
		                                             size.height(), GL_RGBA32F);
	}
}

void MainWin::reloadBloomTargets()
{
	GLHandler::deleteRenderTarget(bloomTargets[0]);
	GLHandler::deleteRenderTarget(bloomTargets[1]);
	if(!vrHandler)
	{
		bloomTargets[0]
		    = GLHandler::newRenderTarget(width(), height(), GL_RGBA32F);
		bloomTargets[1]
		    = GLHandler::newRenderTarget(width(), height(), GL_RGBA32F);
	}
	else
	{
		QSize size(vrHandler.getEyeRenderTargetSize());
		bloomTargets[0] = GLHandler::newRenderTarget(size.width(),
		                                             size.height(), GL_RGBA32F);
		bloomTargets[1] = GLHandler::newRenderTarget(size.width(),
		                                             size.height(), GL_RGBA32F);
	}
}

void MainWin::updateScene(BasicCamera& camera, QString const& /*pathId*/)
{
	if(lastCursorPos.y() < menuBar->height() + 10 && lastCursorPos.y() >= 0
	   && lastCursorPos.x() >= 0 && lastCursorPos.x() < width())
	{
		menuBar->move(position());
		menuBar->show();
	}
	else if(menuBar->activeAction() == nullptr && menuBar->isVisible())
	{
		menuBar->hide();
	}

	auto& cam = dynamic_cast<OrbitalSystemCamera&>(camera);
	cam.setWindowSize(size());

	clock.update();
	/*if(!clock.drawableFrame())
	{
	    return;
	}*/

	cam.updateUT(clock.getCurrentUt());

	if(!vrHandler && cam.getAltitude() < 0.0)
	{
		cam.setAltitude(1.0);
	}

	if(!vrHandler)
	{
		CelestialBodyRenderer::overridenScale = 1.0;
	}

	if(autoexposure)
	{
		cam.autoUpdateExposure(renderer.getLastFrameAverageLuminance(),
		                       frameTiming);
	}

	Controller const* left(vrHandler.getController(Side::LEFT));
	Controller const* right(vrHandler.getController(Side::RIGHT));

	// single grip = translation
	if(leftGripPressed != rightGripPressed)
	{
		Vector3 controllerRelPos;
		if(leftGripPressed && left != nullptr)
		{
			controllerRelPos
			    = Utils::fromQt(cam.seatedTrackedSpaceToWorldTransform()
			                    * left->getPosition())
			          / CelestialBodyRenderer::overridenScale
			      + cam.relativePosition;
		}
		else if(rightGripPressed && right != nullptr)
		{
			controllerRelPos
			    = Utils::fromQt(cam.seatedTrackedSpaceToWorldTransform()
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

	orbitalSystem->update(clock.getCurrentUt());
	systemRenderer->updateMesh(clock.getCurrentUt(), cam);

	std::stringstream stream;
	stream.precision(3);
	// stream << clock.getCurrentFPS() << " FPS" << std::endl;
	stream << round(1.f / frameTiming) << " FPS" << std::endl;
	stream << "Targeting : " << cam.target->getName() << std::endl;
	stream.precision(10);
	stream << "Altitude : " << lengthPrettyPrint(cam.getAltitude()).first << " "
	       << lengthPrettyPrint(cam.getAltitude()).second << std::endl;
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

	stream << "Exposure " << (autoexposure ? "(auto)" : "") << ": "
	       << cam.exposure << std::endl;

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

void MainWin::renderScene(BasicCamera const& camera, QString const& /*pathId*/)
{
	auto& cam(renderer.getCamera<OrbitalSystemCamera>("planet"));

	GLHandler::setPointSize(1);
	stars.render(cam.getPixelSolidAngle());
	systemRenderer->render(camera);
	renderer.renderVRControls();
	// dbgRenderVRControls();
	systemRenderer->renderTransparent(camera);
	if(!vrHandler)
	{
		GLHandler::setShaderParam(debugText->getShader(), "exposure",
		                          cam.exposure);
		GLHandler::setShaderParam(debugText->getShader(), "dynamicrange",
		                          cam.dynamicrange);
		debugText->render();
	}
}

void MainWin::applyPostProcShaderParams(
    QString const& id, GLHandler::ShaderProgram shader,
    GLHandler::RenderTarget const& currentTarget) const
{
	AbstractMainWin::applyPostProcShaderParams(id, shader, currentTarget);
	if(id == "exposure")
	{
		auto& cam(renderer.getCamera<OrbitalSystemCamera>("planet"));
		GLHandler::setShaderParam(shader, "exposure", cam.exposure);
		GLHandler::setShaderParam(shader, "dynamicrange", cam.dynamicrange);
	}
	if(id == "bloom")
	{
		GLHandler::setShaderParam(shader, "highlumtex", 1);
	}
}

std::vector<GLHandler::Texture> MainWin::getPostProcessingUniformTextures(
    QString const& id, GLHandler::ShaderProgram shader,
    GLHandler::RenderTarget const& currentTarget) const
{
	auto abstractResult(AbstractMainWin::getPostProcessingUniformTextures(
	    id, shader, currentTarget));
	if(!abstractResult.empty())
	{
		return abstractResult;
	}
	if(id == "bloom")
	{
		if(bloom)
		{
			// high luminosity pass
			GLHandler::ShaderProgram hlshader(
			    GLHandler::newShader("postprocess", "highlumpass"));
			GLHandler::postProcess(hlshader, currentTarget, bloomTargets[0]);
			GLHandler::deleteShader(hlshader);

			// blurring
			GLHandler::ShaderProgram blurshader(
			    GLHandler::newShader("postprocess", "blur"));
			for(unsigned int i = 0; i < 6; i++)
			{
				GLHandler::setShaderParam(blurshader, "horizontal",
				                          static_cast<float>(i % 2));
				GLHandler::postProcess(blurshader, bloomTargets[i % 2],
				                       bloomTargets[(i + 1) % 2]);
			}
			GLHandler::deleteShader(blurshader);

			return {GLHandler::getColorAttachmentTexture(bloomTargets[0])};
		}
		GLHandler::beginRendering(bloomTargets[0]);
		return {GLHandler::getColorAttachmentTexture(bloomTargets[0])};
	}
	return {};
}

void MainWin::dbgRenderVRControls()
{
	if(!vrHandler)
	{
		return;
	}

	Controller const* left(vrHandler.getController(Side::LEFT));
	Controller const* right(vrHandler.getController(Side::RIGHT));

	QVector3D leftPos, rightPos;
	if(left != nullptr)
	{
		leftPos = left->getPosition();
		GLHandler::setShaderParam(shader, "color", QColor("red"));
		QMatrix4x4 model;
		model.translate(left->getPosition());
		GLHandler::setUpRender(shader, model,
		                       GLHandler::GeometricSpace::SEATEDTRACKED);
		GLHandler::render(point);
	}
	if(right != nullptr)
	{
		rightPos = right->getPosition();
		GLHandler::setShaderParam(shader, "color", QColor("green"));
		QMatrix4x4 model;
		model.translate(right->getPosition());
		GLHandler::setUpRender(shader, model,
		                       GLHandler::GeometricSpace::SEATEDTRACKED);
		GLHandler::render(point);
	}

	if(left != nullptr && right != nullptr)
	{
		debugText->setText(QString::number((leftPos - rightPos).length()));
	}
}

void MainWin::rescale(double newScale, Vector3 const& scaleCenter)
{
	auto& cam(dynamic_cast<OrbitalSystemCamera&>(renderer.getCamera("planet")));
	Vector3 diff(cam.relativePosition - scaleCenter);
	diff /= newScale / CelestialBodyRenderer::overridenScale;
	cam.relativePosition                  = scaleCenter + diff;
	CelestialBodyRenderer::overridenScale = newScale;
}

QTreeWidgetItem* MainWin::constructItems(Orbitable const& orbitable,
                                         QTreeWidgetItem* parent)
{
	QTreeWidgetItem* item;
	if(parent == nullptr)
	{
		item = new QTreeWidgetItem(tree);
	}
	else
	{
		item = new QTreeWidgetItem(parent);
	}

	if(orbitable.getOrbitableType() != Orbitable::Type::SPACECRAFT)
	{
		item->setText(0, QString::fromStdString(orbitable.getName()));
	}
	else
	{
		auto obt(orbitable.getOrbit());
		if(obt != nullptr)
		{
			QString beginDate(
			    SimulationTime::utToDateTime(obt->getRange().first)
			        .date()
			        .toString(Qt::ISODate));
			QString endDate(SimulationTime::utToDateTime(obt->getRange().second)
			                    .date()
			                    .toString(Qt::ISODate));
			if(obt->isInRange(clock.getCurrentUt()))
			{
				item->setText(0, QString::fromStdString(orbitable.getName())
				                     + "\n(" + beginDate + "\n->" + endDate
				                     + ")");
				item->setTextColor(0, QColor("green"));
			}
			else
			{
				item->setText(0, QString::fromStdString(orbitable.getName())
				                     + "\n(" + beginDate + "\n->" + endDate
				                     + ")");
				item->setTextColor(0, QColor("red"));
			}
		}
		else
		{
			item->setText(0, QString::fromStdString(orbitable.getName()));
			std::cout << std::string("Spacecraft \"") + orbitable.getName()
			                 + "\" doesn't have an orbit"
			          << std::endl;
		}
	}

	for(auto child : orbitable.getChildren())
	{
		item->addChild(constructItems(*child, item));
	}

	return item;
}

MainWin::~MainWin()
{
	GLHandler::deleteRenderTarget(bloomTargets[0]);
	GLHandler::deleteRenderTarget(bloomTargets[1]);

	delete systemRenderer;
	delete orbitalSystem;

	delete menuBar;
	delete dialog;

	GLHandler::deleteMesh(point);
	GLHandler::deleteShader(shader);
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
