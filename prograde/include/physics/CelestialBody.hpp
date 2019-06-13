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

#ifndef CELESTIALBODY_HPP
#define CELESTIALBODY_HPP

#include <QJsonArray>
#include <QJsonObject>
#include <cmath>
#include <vector>

#include "CSVOrbit.hpp"
#include "Color.hpp"
#include "Orbit.hpp"
#include "math/MathUtils.hpp"

class CelestialBody
{
  public:
	enum class Type
	{
		GENERIC,
		TERRESTRIAL,
		GAZGIANT,
	};

	struct Parameters
	{
		Type type = Type::GENERIC;
		double mass;
		double radius;
		Vector3 oblateness = Vector3(1.0, 1.0, 1.0);
		Color color;
		double atmosphere = 0.0;
		double innerRing  = 0.0;
		double outerRing  = 0.0;

		/* ROTATION */
		double siderealTimeAtEpoch = 0.0; // angle between FP of Aries and Prime
		                                  // Meridian in radians
		double siderealRotationPeriod = DBL_MAX; // in seconds

		double northPoleRightAsc    = 0.0;                // in rad
		double northPoleDeclination = constant::pi / 2.0; // in rad
	};

	CelestialBody(QJsonObject const& json, double influentBodyMass);
	CelestialBody(QJsonObject const& json, CelestialBody const& parent);

	CelestialBody(double influentBodyMass, Orbit::Parameters orbitalParams,
	              Parameters physicalParams);
	CelestialBody(CelestialBody const& parent,
	              Orbit::Parameters const& orbitalParams,
	              Parameters physicalParams);
	CelestialBody(double influentBodyMass, std::string const& ownName,
	              Parameters physicalParams);
	CelestialBody(CelestialBody const* parent, std::string const& ownName,
	              Parameters physicalParams);
	CelestialBody(CelestialBody const& copiedBody) = default;
	CelestialBody const* getParent() const;
	std::vector<CelestialBody*> const& getChildren() const;
	std::vector<CelestialBody*> getAllDescendants() const;
	CelestialBody* createChild(QJsonObject const& json);
	CelestialBody* createChild(Orbit::Parameters const& orbitalParams,
	                           Parameters const& physicalParams);
	CelestialBody* createChild(std::string const& childName,
	                           Parameters const& physicalParams);
	std::string const& getName() { return name; };
	Orbit const* getOrbit() const;
	Orbit* getOrbit();
	Parameters getParameters() const;
	Vector3 getRelativePositionAtUT(UniversalTime uT) const;
	Vector3 getAbsolutePositionAtUT(UniversalTime uT) const;
	Vector3 getAbsoluteVelocityAtUT(UniversalTime uT) const;
	CoordinateSystem getAttachedCoordinateSystemAtUT(UniversalTime uT) const;
	double getPrimeMeridianSiderealTimeAtUT(UniversalTime uT) const;
	virtual ~CelestialBody();

	// Will try to get more significant digits than the awful
	// to.absolute - from.absolute
	static Vector3 getRelativePositionAtUt(CelestialBody const* from,
	                                       CelestialBody const* to,
	                                       UniversalTime uT);
	QJsonObject getJSONRepresentation() const;

  private:
	std::string name;
	CelestialBody const* parent;
	std::vector<CelestialBody*> children;
	Orbit* orbit;
	Parameters parameters;

	static std::string typeToStr(Type type);
	static Type strToType(std::string const& str);
	static QJsonObject vector3ToJSON(Vector3 const& v);
	static Vector3 jsonToVector3(QJsonObject const& obj,
	                             Vector3 const& defaultValue = Vector3());
	static QJsonObject colorToJSON(Color const& c);
	static Color jsonToColor(QJsonObject const& obj,
	                         Color const& defaultValue = Color(255, 0, 0, 0));
};

#endif // CELESTIALBODY_HPP
