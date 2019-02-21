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

#ifndef ORBIT_HPP
#define ORBIT_HPP

#include <cmath>
#include <iostream>

#include "math/CoordinateSystem.hpp"
#include "math/EccentricAnomalySolver.hpp"
#include "math/constants.hpp"

#include "UniversalTime.hpp"

class Orbit
{
  public:
	struct Parameters
	{
		float inclination;
		float ascendingNodeLongitude;
		float periapsisArgument;
		float eccentricity;
		float semiMajorAxis; // or altitude of periapsis if parabolic orbit
		float meanAnomalyAtEpoch;
	};

	struct MassiveBodyMass
	{
		explicit MassiveBodyMass(double value) : value(value) {};
		double value;
	};

	struct Period
	{
		explicit Period(double value) : value(value) {};
		double value;
	};

  public:
	Orbit() = delete;
	Orbit(MassiveBodyMass const& massiveBodyMass, Parameters parameters);
	Orbit(Period const& period, Parameters parameters);
	Orbit(Orbit const& copiedOrbit) = default;
	double getMassiveBodyMass() const;
	Parameters getParameters() const;
	double getPeriod() const;
	double getMeanAnomalyAtUT(UniversalTime uT) const;
	double getEccentricAnomalyAtUT(UniversalTime uT) const;
	double getTrueAnomalyAtUT(UniversalTime uT) const;
	double getMassiveBodyDistanceAtUT(UniversalTime uT) const;
	Vector3 getPositionAtUT(UniversalTime uT) const;
	//Vector3 getVelocityAtUT(UniversalTime uT) const;
	CoordinateSystem getRelativeCoordinateSystemAtUT(UniversalTime uT) const;
	std::ostream& displayAsText(std::ostream& stream) const;

  private:
	double massiveBodyMass;
	Parameters parameters;

	//period is precomputed as it is often used and doesn't change
	double period;
};

std::ostream& operator<<(std::ostream& stream, Orbit const& orbit);

#endif // ORBIT_HPP
