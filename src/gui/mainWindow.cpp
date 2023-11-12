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

		ImGui::Begin("##main window");

		//available chats
		static uint32_t chatSelected = 0;
		{
			ImGui::BeginChild("##available chats", ImVec2(300, 0));
			for (int i = 0; i < 100; i++)
			{
				char label[128];
				sprintf(label, "User %d", i);
				if (ImGui::Selectable(label, chatSelected == i))
					chatSelected = i;
			}
			ImGui::EndChild();
		}
		ImGui::SameLine();

		//chat zone
		{
			//input text
			static bool isEnterPressed = false;
			static bool reclaimFocus = false;
			ImGui::SetCursorPos(ImVec2(310, 670));
			ImGui::PushItemWidth(450);
			if (ImGui::InputText("##input", &(this->_inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				isEnterPressed = true;
				reclaimFocus = true;
			}
			ImGui::PopItemWidth();

			//button
			static bool isButtonPressed = false;
			ImGui::SameLine();
			ImGui::SetCursorPosY(670);
			if (ImGui::Button("send"))
			{
				isButtonPressed = true;
				reclaimFocus = true;
			}

			// auto-focus on window apparition
			ImGui::SetItemDefaultFocus();
			if (reclaimFocus)
			{
				ImGui::SetKeyboardFocusHere(-1); // auto focus input text
				reclaimFocus = false;
			}

			if ((isEnterPressed || isButtonPressed) && this->_inputBuffer != "")
			{
				Network::Client::GetInstance().Send(chatSelected, this->_inputBuffer.c_str(), this->_inputBuffer.size());
				Buffer::MessageBuffer::GetInstance().PushFront(Buffer::MessageType::kSend, this->_inputBuffer.c_str());
				isEnterPressed = false;
				isButtonPressed = false;

				this->_inputBuffer = "";
			}

			//client zone
			ImGui::SetCursorPos(ImVec2(310, 60));
			ImGui::BeginChild("##chat zone", ImVec2(510, 600));
			for (auto& item : Buffer::MessageBuffer::GetInstance())
			{
				if (item._messageType == Buffer::MessageType::kReceived)
				{
					ImGui::TextWrapped(item._data);
				}
				else if (item._messageType == Buffer::MessageType::kSend)
				{
					ImGui::SetCursorPosX(300);
					ImGui::TextWrapped(item._data);
				}
			}
			ImGui::EndChild();
		}
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