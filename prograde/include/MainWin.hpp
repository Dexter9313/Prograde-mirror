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
	class State : public AbstractState
	{
	  public:
		State()                   = default;
		State(State const& other) = default;
		State(State&& other)      = default;
		virtual void readFromDataStream(QDataStream& stream) override
		{
			toneMappingState.readFromDataStream(stream);
			camState.readFromDataStream(stream);
			double dut;
			stream >> dut;
			ut = dut;
			stream >> renderLabels;
			stream >> renderOrbits;
		};
		virtual void writeInDataStream(QDataStream& stream) override
		{
			toneMappingState.writeInDataStream(stream);
			camState.writeInDataStream(stream);
			double dut(ut);
			stream << dut;
			stream << renderLabels;
			stream << renderOrbits;
		};

		ToneMappingModel::State toneMappingState;
		OrbitalSystemCamera::State camState;
		UniversalTime ut;
		float renderLabels;
		float renderOrbits;
	};

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
	    QString const& id, GLShaderProgram const& shader,
	    GLFramebufferObject const& currentTarget) const override;

	virtual std::vector<
	    std::pair<GLTexture const*, GLComputeShader::DataAccessMode>>
	    getPostProcessingUniformTextures(
	        QString const& id, GLShaderProgram const& shader,
	        GLFramebufferObject const& currentTarget) const override;

	virtual AbstractState* constructNewState() const override
	{
		return new MainWin::State;
	};
	virtual void readState(AbstractState const& s) override
	{
		auto const& state = dynamic_cast<State const&>(s);
		toneMappingModel->readState(state.toneMappingState);

		auto& cam(renderer.getCamera<OrbitalSystemCamera&>("planet"));
		cam.readState(state.camState);
		clock.setCurrentUt(state.ut);
		CelestialBodyRenderer::renderLabels = state.renderLabels;
		CelestialBodyRenderer::renderOrbits = state.renderOrbits;
	};
	virtual void writeState(AbstractState& s) const override
	{
		auto& state = dynamic_cast<State&>(s);
		toneMappingModel->writeState(state.toneMappingState);

		auto const& cam(
		    renderer.getCamera<OrbitalSystemCamera const&>("planet"));
		cam.writeState(state.camState);
		state.ut           = clock.getCurrentUt();
		state.renderLabels = CelestialBodyRenderer::renderLabels;
		state.renderOrbits = CelestialBodyRenderer::renderOrbits;
	};

  private:
	StarryBackground* stars;

	QPoint lastCursorPos;

	unsigned int bodyTracked = 0;

	Text3D* debugText;
	float timeSinceTextUpdate = FLT_MAX;

	// render controllers as points used to measure distances
	// useful for spacecrafts scale checking
	GLShaderProgram* shader;
	GLMesh* point;
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
