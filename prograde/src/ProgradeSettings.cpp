/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@epfl.ch>

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

#include "ProgradeSettings.hpp"

ProgradeSettings::ProgradeSettings(QWidget* parent)
    : SettingsWidget(parent)
{
	insertGroup("simulation", tr("Simulation"), 0);
	addDateTimeSetting("starttime", QDateTime().currentDateTimeUtc(),
	                   tr("Start time (UTC)"));

	addDirPathSetting(
	    "planetsystemdir",
	    QFileInfo(QSettings().fileName()).absoluteDir().absolutePath()
	        + "/systems/",
	    tr("Planetary System Root Directory"));

	insertGroup("quality", tr("Quality"), 1);
	addUIntSetting("texmaxsize", 16, tr("Textures max size (x512)"), 1, 32);

	setCurrentIndex(0);
}
