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

#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include "BaseInputManager.hpp"

class InputManager : public BaseInputManager
{
  public:
	InputManager()
	{
		addAction(Qt::Key_Tab,
		          {"nextcelestialbody", tr("Next Celestial Body")});
		addAction(Qt::Key_Backtab,
		          {"prevcelestialbody", tr("Previous Celestial Body")});
		addAction(Qt::Key_X,
		          {"togglelock", "Toggle Lock on Celestial Body Rotation"});
		addAction(Qt::Key_Home, {"resetvrpos", "Reset VR origin"});
		addAction(Qt::Key_L, {"togglelabels", "Toggle Labels"});
		addAction(Qt::Key_O, {"toggleorbits", "Toggle Orbits"});
		addAction(Qt::Key_R, {"timecoeffdown", "Lower Time Coeff"});
		addAction(Qt::Key_T, {"timecoeffup", "Raise Time Coeff"});
		addAction(Qt::Key_C, {"centercam", "Center Camera On Target"});
		addAction(Qt::Key_W, {"forward", "Move Forward"});
		addAction(Qt::Key_A, {"left", "Move Left"});
		addAction(Qt::Key_S, {"backward", "Move Backward"});
		addAction(Qt::Key_D, {"right", "Move Right"});
	};
};

#endif // INPUTMANAGER_HPP
