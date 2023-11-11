#include "mainWindow.h"

namespace Gui
{
	bool MainWindow::Init()
	{
		if (!glfwInit())
			return false;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		this->_mainWindow = glfwCreateWindow(820, 700, "Online Chat", nullptr, nullptr);
		if (this->_mainWindow == nullptr)
			return false;

		glfwMakeContextCurrent(this->_mainWindow);
		glfwSwapInterval(false); // Disable vsync

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(this->_mainWindow, true);
		ImGui_ImplOpenGL3_Init(this->_glslVersion);

		this->_windowStatus = WindowStatusCode::kWIndowInited;

		return true;
	}

	void MainWindow::Draw()
	{
		if (this->_windowStatus != WindowStatusCode::kWIndowInited)
			return;

		while (!glfwWindowShouldClose(this->_mainWindow))
		{
			MainWindow::NewFrame();
			MainWindow::GenerateControls();
			MainWindow::Render();
		}
	}

	void MainWindow::NewFrame()
	{
		if (this->_windowStatus != WindowStatusCode::kWIndowInited)
			return;

		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void MainWindow::GenerateControls() 
	{
		if (this->_windowStatus != WindowStatusCode::kWIndowInited)
			return;

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());				//enable docking 

		ImGui::Begin(" ");
	

		ImGui::InputText("gfd", &this->_inputBuffer);

		if(ImGui::Button("hello") && this->_inputBuffer != "")
		{
			Network::Client::GetInstance().Send(2, this->_inputBuffer.c_str(), this->_inputBuffer.size());
			this->_inputBuffer = "";
		}



		ImGui::End();

		ImGui::Begin("  ");
		ImGui::End();
		
	}

	void MainWindow::Render()
	{
		if (this->_windowStatus != WindowStatusCode::kWIndowInited)
			return;

		ImGui::Render();
		glfwGetFramebufferSize(this->_mainWindow, &this->_currentDisplayWidth, &this->_currentDisplayHeight);
		glViewport(0, 0, this->_currentDisplayWidth, this->_currentDisplayHeight);

		glClearColor(this->_windowBackgroundColor.x * this->_windowBackgroundColor.w,		//red
			this->_windowBackgroundColor.y * this->_windowBackgroundColor.w,				//green	
			this->_windowBackgroundColor.z * this->_windowBackgroundColor.w,				//blue
			this->_windowBackgroundColor.w);												//alpha

		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(this->_mainWindow);
	}

	void MainWindow::Cleanup()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(this->_mainWindow);
		glfwTerminate();

		Network::Client::GetInstance().~Client();

		this->_windowStatus = WindowStatusCode::kWindowDeleted;
	}

	MainWindow::~MainWindow()
	{
		if (this->_windowStatus != WindowStatusCode::kWindowDeleted)
			MainWindow::Cleanup();
	}

} // !namespace Gui