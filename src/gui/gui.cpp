#include "gui.h"

namespace Gui
{
	bool MainWindow::Init()
	{
		if (!glfwInit())
			return false;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		this->_window = glfwCreateWindow(1280, 720, "Online Chat", nullptr, nullptr);
		if (this->_window == nullptr)
			return false;

		glfwMakeContextCurrent(this->_window);
		glfwSwapInterval(false); // Disable vsync

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		this->_io = ImGui::GetIO();
		this->_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		this->_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(this->_window, true);
		ImGui_ImplOpenGL3_Init(this->_glslVersion);

		this->_windowStatus = kWIndowInited;

		return true;
	}

	void MainWindow::Draw()
	{
		if (this->_windowStatus != kWIndowInited)
			return;

		while (!glfwWindowShouldClose(this->_window))
		{
			MainWindow::NewFrame();
			MainWindow::GenerateControls();
			MainWindow::Render();
		}
	}

	void MainWindow::NewFrame()
	{
		if (this->_windowStatus != kWIndowInited)
			return;

		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void MainWindow::GenerateControls() 
	{
		if (this->_windowStatus != kWIndowInited)
			return;

		//rendering Gui
		
	}

	void MainWindow::Render()
	{
		if (this->_windowStatus != kWIndowInited)
			return;

		ImGui::Render();
		glfwGetFramebufferSize(this->_window, &this->_currentDisplayWidth, &this->_currentDisplayHeight);
		glViewport(0, 0, this->_currentDisplayWidth, this->_currentDisplayHeight);

		glClearColor(this->_windowColorBackground.x * this->_windowColorBackground.w,		//red
			this->_windowColorBackground.y * this->_windowColorBackground.w,				//green	
			this->_windowColorBackground.z * this->_windowColorBackground.w,				//blue
			this->_windowColorBackground.w);												//alpha

		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(this->_window);
	}

	void MainWindow::Cleanup()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(this->_window);
		glfwTerminate();

		this->_windowStatus = kWindowDeleted;
	}

	MainWindow::~MainWindow()
	{
		if (this->_windowStatus != kWindowDeleted)
			MainWindow::Cleanup();
	}

} // !namespace Gui