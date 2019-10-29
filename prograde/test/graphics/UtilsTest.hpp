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

#ifndef UTILSTEST_H
#define UTILSTEST_H

#include "graphics/Utils.hpp"
#include <QtTest>

class UtilsTest : public QObject
{
	Q_OBJECT
  private slots:
	void Matrix4x4Conversion()
	{
		std::array<std::array<double, 4>, 4> data = {{{1.0, 2.0, 3.0, 4.0},
		                                              {5.0, 6.0, 7.0, 8.0},
		                                              {9.0, 10.0, 11.0, 12.0},
		                                              {0.0, 0.0, 0.0, 1.0}}};

		Matrix4x4 matrix(data);

		QVERIFY(matrix == Utils::fromQt(Utils::toQt(matrix)));
	};

	void Matrix4x4ConversionThenOperation()
	{
		std::array<std::array<double, 4>, 4> data = {{{1.0, 2.0, 3.0, 4.0},
		                                              {5.0, 6.0, 7.0, 8.0},
		                                              {9.0, 10.0, 11.0, 12.0},
		                                              {0.0, 0.0, 0.0, 1.0}}};

		Matrix4x4 matrix(data);
		QMatrix4x4 mQt(Utils::toQt(matrix));

		Vector3 vector(.5, .33333, .25);
		QVector3D vQt(Utils::toQt(vector));

		QVERIFY((matrix * vector - Utils::fromQt(mQt * vQt)).length() < 1.0e-6);
	};
};

#endif // UTILSTEST_H
