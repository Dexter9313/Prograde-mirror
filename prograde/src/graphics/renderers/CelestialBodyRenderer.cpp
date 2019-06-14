/*
    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "../../../include/graphics/renderers/CelestialBodyRenderer.hpp"

CelestialBodyRenderer::CelestialBodyRenderer(CelestialBody* drawnBody,
                                             Star const* star,
                                             double declinationTilt)
    : drawnBody(drawnBody)
    , centralBodyRadius(star->getParameters().radius)
    , declinationTilt(declinationTilt)
    , boundingSphere(drawnBody->getParameters().radius)
    , lightcolor(Utils::toQt(star->getColor()))
    , pointShader(GLHandler::newShader("colored"))
    , pointMesh(GLHandler::newMesh())
    , unloadedShader(
          GLHandler::newShader("planet/planet", "planet/uniformplanet"))
    , unloadedMesh(Primitives::newUnitSphere(unloadedShader, 50, 50))
{
	model = QMatrix4x4();
	model.translate(Utils::toQt(this->drawnBody->getAbsolutePositionAtUT(0)));

	// ROTATION

	Vector3 x(1.0, 0.0, 0.0), y(0.0, 1.0, 0.0);
	x.rotateAlongZ(drawnBody->getParameters().northPoleRightAsc);
	y.rotateAlongZ(drawnBody->getParameters().northPoleRightAsc);
	Vector3 planetLocalZ // north pole
	    = Matrix4x4(drawnBody->getParameters().northPoleDeclination, -1.0 * y)
	      * x;

	planetLocalZ.rotateAlongX(-1.f * declinationTilt);
	planetLocalZ = planetLocalZ.getUnitForm();

	Vector3 planetLocalY(
	    crossProduct(planetLocalZ, Vector3(1.0, 0.0, 0.0)).getUnitForm());
	Vector3 planetLocalX(
	    crossProduct(planetLocalY, planetLocalZ).getUnitForm());

	baseRotation = QMatrix4x4(
	    planetLocalX[0], planetLocalY[0], planetLocalZ[0], 0.f, planetLocalX[1],
	    planetLocalY[1], planetLocalZ[1], 0.f, planetLocalX[2], planetLocalY[2],
	    planetLocalZ[2], 0.f, 0.f, 0.f, 0.f, 1.f);

	// POINT
	Color color(drawnBody->getParameters().color);
	color.r *= lightcolor.redF() / 4;
	color.g *= lightcolor.greenF() / 4;
	color.b *= lightcolor.blueF() / 4;
	GLHandler::setVertices(
	    pointMesh,
	    {0.f, 0.f, 0.f, color.r / 255.f, color.g / 255.f, color.b / 255.f, 1.f},
	    pointShader, {{"position", 3}, {"color", 4}});

	// UNLOADED
	GLHandler::setShaderParam(unloadedShader, "color",
	                          Utils::toQt(drawnBody->getParameters().color));
	GLHandler::setShaderParam(
	    unloadedShader, "oblateness",
	    Utils::toQt(drawnBody->getParameters().oblateness));
}

void CelestialBodyRenderer::updateMesh(UniversalTime uT, Camera const& camera)
{
	Vector3 camRelPos = camera.getRelativePositionTo(drawnBody, uT);

	double camDist(camRelPos.length());
	double scale(centerPosition / camDist);

	QVector3D position(Utils::toQt(scale * camRelPos));

	double radiusScale(drawnBody->getParameters().radius * scale);

	// custom models have km units, not radius units
	if(customModel)
	{
		radiusScale *= 1000.0 / drawnBody->getParameters().radius;
	}

	if(drawnBody->getParameters().outerRing == 0.f)
	{
		culled = camera.shouldBeCulled(position, boundingSphere * scale);
	}
	else
	{
		// take rings into account !
		double cullingRadius(drawnBody->getParameters().outerRing * scale);
		culled = camera.shouldBeCulled(position, cullingRadius);
	}

	apparentAngle = 2.0 * atan(drawnBody->getParameters().radius / camDist);
	// apparentAngle = 0.003f;

	model = QMatrix4x4();
	model.translate(Utils::toQt(scale * camRelPos));
	model.scale(radiusScale);
	if(customModel
	   && (apparentAngle < 0.005 || (planet != nullptr && !planet->isValid())))
	{
		model.scale(drawnBody->getParameters().radius / 1000.0);
	}

	if(apparentAngle < 0.005)
	{
		unloadPlanet();
		if(apparentAngle < 0.0026)
		{
			return;
		}
	}
	else
	{
		loadPlanet();
	}
	if(culled)
	{
		return;
	}

	Vector3 centralBodyCenter(-1.0 * camera.getAbsolutePosition());

	lightpos = Utils::toQt(centralBodyCenter - camRelPos);

	lightradius = centralBodyRadius;
	if(customModel)
	{
		lightpos /= drawnBody->getParameters().radius / 1000.f;
		lightradius /= drawnBody->getParameters().radius / 1000.f;
	}
	else
	{
		lightpos /= drawnBody->getParameters().radius;
		lightradius /= drawnBody->getParameters().radius;
	}

	unsigned int i(0);

	if(drawnBody->getParent() != nullptr)
	{
		Vector3 parentRelPos(-1.0 * drawnBody->getRelativePositionAtUT(uT));
		float parentRad(drawnBody->getParent()->getParameters().radius);

		// in shader, drawnbody radius == 1.0
		if(customModel)
		{
			parentRelPos /= drawnBody->getParameters().radius / 1000.f;
			parentRad /= drawnBody->getParameters().radius / 1000.f;
		}
		else
		{
			parentRelPos /= drawnBody->getParameters().radius;
			parentRad /= drawnBody->getParameters().radius;
		}

		neighborsPosRadius[i] = QVector4D(Utils::toQt(parentRelPos), parentRad);
		neighborsOblateness[i] = QVector3D(
		    Utils::toQt(drawnBody->getParent()->getParameters().oblateness));
		++i;
	}
	std::vector<CelestialBody*> const& children(drawnBody->getChildren());
	for(auto child : children)
	{
		if(i >= 5)
		{
			break;
		}
		Vector3 childRelPos(
		    CelestialBody::getRelativePositionAtUt(drawnBody, child, uT));
		float childRad(child->getParameters().radius);

		// in shader, drawnbody radius == 1.0
		if(customModel)
		{
			childRelPos /= drawnBody->getParameters().radius / 1000.f;
			childRad /= drawnBody->getParameters().radius / 1000.f;
		}
		else
		{
			childRelPos /= drawnBody->getParameters().radius;
			childRad /= drawnBody->getParameters().radius;
		}

		neighborsPosRadius[i] = QVector4D(Utils::toQt(childRelPos), childRad);
		neighborsOblateness[i]
		    = QVector3D(Utils::toQt(child->getParameters().oblateness));
		++i;
	}
	for(; i < 5; ++i)
	{
		neighborsPosRadius[i]  = QVector4D(0.f, 0.f, 0.f, 0.f);
		neighborsOblateness[i] = QVector3D(1.f, 1.f, 1.f);
	}

	// custom models have (1, 0, 0) at planetographic origin
	// non custom have (-1, 0, 0) at planetographic origin
	float siderealTime = drawnBody->getPrimeMeridianSiderealTimeAtUT(uT)
	                     + (customModel ? 0.0 : constant::pi);

	QMatrix4x4 sideralRotation;
	sideralRotation.rotate(siderealTime * 180.f / constant::pi,
	                       QVector3D(0.f, 0.f, 1.f));

	properRotation = baseRotation * sideralRotation;

	if(planet != nullptr)
	{
		planet->updateTextureLoading();
		float modelRadius(planet->updateModelLoading());
		customModel = (modelRadius > 0.f);
		if(customModel)
		{
			boundingSphere = modelRadius * 1000.f;
		}
	}
}

void CelestialBodyRenderer::render()
{
	if(culled)
	{
		return;
	}

	if(apparentAngle < 0.0026)
	{
		GLHandler::setUpRender(pointShader, model);
		GLHandler::render(pointMesh);
		return;
	}

	if(apparentAngle < 0.005 || planet == nullptr || !planet->isValid())
	{
		GLHandler::setShaderParam(unloadedShader, "lightpos", lightpos);
		GLHandler::setShaderParam(unloadedShader, "lightradius", lightradius);
		GLHandler::setShaderParam(unloadedShader, "lightcolor", lightcolor);
		GLHandler::setShaderParam(unloadedShader, "neighborsPosRadius", 5,
		                          &(neighborsPosRadius[0]));
		GLHandler::setShaderParam(unloadedShader, "neighborsOblateness", 5,
		                          &(neighborsOblateness[0]));
		GLHandler::setShaderParam(unloadedShader, "properRotation",
		                          properRotation);
		GLHandler::setUpRender(unloadedShader, model);
		GLHandler::render(unloadedMesh);
		return;
	}

	planet->render(model, lightpos, lightradius, lightcolor, neighborsPosRadius,
	               neighborsOblateness, properRotation, customModel);
}

CelestialBodyRenderer::~CelestialBodyRenderer()
{
	GLHandler::deleteMesh(pointMesh);
	GLHandler::deleteShader(pointShader);
	GLHandler::deleteMesh(unloadedMesh);
	GLHandler::deleteShader(unloadedShader);
	unloadPlanet(true);
}

void CelestialBodyRenderer::loadPlanet()
{
	if(planet != nullptr)
	{
		return;
	}

	QString name(drawnBody->getName().c_str());

	planet
	    = new Planet(1.f, Utils::toQt(drawnBody->getParameters().oblateness));

	QString diffuse(""), normal("");
	QString str(QSettings().value("simulation/planetsystemdir").toString()
	            + "/images/" + name + "/diffuse.jpg");
	if(QFileInfo(str).exists())
	{
		diffuse = str;
	}
	str = QSettings().value("simulation/planetsystemdir").toString()
	      + "/images/" + name + "/diffuse.png";
	if(QFileInfo(str).exists())
	{
		diffuse = str;
	}
	if(diffuse != "")
	{
		str = QSettings().value("simulation/planetsystemdir").toString()
		      + "/images/" + name + "/normal.jpg";
		if(QFileInfo(str).exists())
		{
			normal = str;
		}
		str = QSettings().value("simulation/planetsystemdir").toString()
		      + "/images/" + name + "/normal.png";
		if(QFileInfo(str).exists())
		{
			normal = str;
		}
	}

	if(normal != "")
	{
		planet->initFromTex(diffuse, normal,
		                    drawnBody->getParameters().atmosphere);
	}
	else if(diffuse != "")
	{
		planet->initFromTex(diffuse, drawnBody->getParameters().atmosphere);
	}
	else if(drawnBody->getParameters().type == CelestialBody::Type::GAZGIANT)
	{
		planet->initGazGiant(Utils::toQt(drawnBody->getParameters().color));
	}
	else
	{
		planet->initTerrestrial(Utils::toQt(drawnBody->getParameters().color));
	}

	if(QFileInfo(QSettings().value("simulation/planetsystemdir").toString()
	             + "/models/" + name + ".ply")
	       .exists())
	{
		planet->updateModel(
		    QSettings().value("simulation/planetsystemdir").toString()
		    + "/models/" + name + ".ply");
	}

	// RINGS

	float outerRing(drawnBody->getParameters().outerRing);
	if(outerRing != 0.f)
	{
		float innerRing(drawnBody->getParameters().innerRing);
		float radius(drawnBody->getParameters().radius);
		QString rings;
		str = QSettings().value("simulation/planetsystemdir").toString()
		      + "/images/" + name + "/rings.jpg";
		if(QFileInfo(str).exists())
		{
			rings = str;
		}
		str = QSettings().value("simulation/planetsystemdir").toString()
		      + "/images/" + name + "/rings.png";
		if(QFileInfo(str).exists())
		{
			rings = str;
		}

		planet->initRings(innerRing / radius, outerRing / radius, rings);
	}
}

void CelestialBodyRenderer::unloadPlanet(bool waitIfPlanetNotLoaded)
{
	if(planet == nullptr)
	{
		return;
	}

	if(planet->isLoading() && !waitIfPlanetNotLoaded)
	{
		planet->updateTextureLoading();
		planet->updateModelLoading();
		return;
	}

	delete planet;
	planet = nullptr;
}
