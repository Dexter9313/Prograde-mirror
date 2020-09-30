#ifndef MAINWIN_H
#define MAINWIN_H

#include <QDateTime>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "AbstractMainWin.hpp"
#include "InputManager.hpp"

#include "Text3D.hpp"
#include "graphics/OrbitalSystemCamera.hpp"
#include "graphics/renderers/OrbitalSystemRenderer.hpp"
#include "graphics/renderers/StarryBackground.hpp"
#include "physics/OrbitalSystem.hpp"
#include "physics/SimulationTime.hpp"

// TEMP
#include "physics/default-orbitalsystems/KerbalSystem.hpp"
#include "physics/default-orbitalsystems/SolarSystem.hpp"

class MainWin : public AbstractMainWin
{
	Q_OBJECT
  public:
	MainWin();
	~MainWin();

  public slots:
	void selectOrbitable(QTreeWidgetItem* item, int column);

  protected:
	virtual void actionEvent(BaseInputManager::Action a, bool pressed) override;
	virtual bool event(QEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void wheelEvent(QWheelEvent* e) override;
	virtual void vrEvent(VRHandler::Event const& e) override;

	// declare drawn resources
	virtual void initScene() override;

	// update physics/controls/meshes, etc...
	// prepare for rendering
	virtual void updateScene(BasicCamera& camera,
	                         QString const& pathId) override;

	// render user scene on camera
	// (no controllers or hands)
	virtual void renderScene(BasicCamera const& camera,
	                         QString const& pathId) override;

	virtual void applyPostProcShaderParams(
	    QString const& id, GLHandler::ShaderProgram shader,
	    GLHandler::RenderTarget const& currentTarget) const override;

	virtual std::vector<GLHandler::Texture> getPostProcessingUniformTextures(
	    QString const& id, GLHandler::ShaderProgram shader,
	    GLHandler::RenderTarget const& currentTarget) const override;

  private:
	StarryBackground stars;

	QPoint lastCursorPos;

	unsigned int bodyTracked = 0;

	Text3D* debugText;
	float timeSinceTextUpdate = FLT_MAX;

	// render controllers as points used to measure distances
	// useful for spacecrafts scale checking
	GLHandler::ShaderProgram shader;
	GLHandler::Mesh point;
	void dbgRenderVRControls();
	void rescale(double newScale, Vector3 const& scaleCenter);

	OrbitalSystem* orbitalSystem;
	OrbitalSystemRenderer* systemRenderer;

	// SimulationTime clock = SimulationTime(0);

	// SimulationTime clock = SimulationTime(496666800);

	// SimulationTime clock = SimulationTime(610595100); // MOON + MARS
	// ALIGNMENT

	// SimulationTime clock = SimulationTime(1748424); // "PERFECT" FULL MOON

	// SimulationTime clock = SimulationTime(675343080); // LUNAR ECLIPSE

	// SimulationTime clock = SimulationTime(615401700); // SOLAR ECLIPSE IN
	// SOUTH AMERICA

	// SimulationTime clock = SimulationTime(6852600); // March equinox

	SimulationTime clock = SimulationTime(
	    QSettings().value("simulation/starttime").value<QDateTime>());

	// scaling/translation controls variables
	bool leftGripPressed  = false;
	bool rightGripPressed = false;
	Vector3 initControllerRelPos;
	float initControllersDistance;
	Vector3 scaleCenter;
	double initScale;

	// keyboard+mouse controls variables
	QVector3D positiveVelocity;
	QVector3D negativeVelocity;
	double velMag          = 1.0;
	bool rotateViewEnabled = false;
	bool trackballEnabled  = false;

	// TEMP
	const int textWidth  = 225;
	const int textHeight = 145;

	// returns value and unit
	static std::pair<double, std::string>
	    lengthPrettyPrint(double length /* in meters */);

	// UI
	QMenuBar* menuBar = nullptr;

	QDialog* dialog   = nullptr;
	QTreeWidget* tree = nullptr;
	QTreeWidgetItem* constructItems(Orbitable const& orbitable,
	                                QTreeWidgetItem* parent = nullptr);
};

#endif // MAINWIN_H
