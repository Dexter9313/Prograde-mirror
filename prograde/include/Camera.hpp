/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

#include "BasicCamera.hpp"
#include "physics/CelestialBody.hpp"

class Camera : public BasicCamera
{
	Q_OBJECT
  public:
	Camera(VRHandler const* vrHandler);
	void updateUT(UniversalTime uT);
	Vector3 getAbsolutePosition() const { return absolutePosition; };

	float angleAroundZ = 0.f;
	float angleAboveXY = 0.f;
	float distance     = 100000.f;

	CelestialBody* target;

  public slots:
	QVector3D getPosition() const { return position; };
	QVector3D getLookDirection() const { return lookDirection; };
	QVector3D getUp() const { return up; };

	float getAngleAroundZ() const { return angleAroundZ; };
	void setAngleAroundZ(float angleAroundZ)
	{
		this->angleAroundZ = angleAroundZ;
	};
	float getAngleAboveXY() const { return angleAboveXY; };
	void setAngleAboveXY(float angleAboveXY)
	{
		this->angleAboveXY = angleAboveXY;
	};
	float getDistance() const { return distance; };
	void setDistance(float distance) { this->distance = distance; };

  private:
	QVector3D position      = {1.f, 0.f, 0.f};
	QVector3D lookDirection = {-1.f, 0.f, 0.f};
	QVector3D up            = {0.f, 0.f, 1.f};

	void updateView();

	Vector3 absolutePosition;
};

#endif // CAMERA_H
