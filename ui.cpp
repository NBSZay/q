#include "globals.hh"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx9.h"
#include "headers/gui.h"
#include "headers/imguipp.h"
#include "headers/offsets.h"
#include "ui/ui.hh"
#include "ui/keyauth.hpp"
#include "world2screen.h"

void ui::render()
{   
	if (!login_submitted && !globals.username[0] && !globals.password[0]) {
		char* env;
		size_t len;
		_dupenv_s(&env, &len, "USERNAME");
		std::string username = env ? env : "User";
		free(env);
		std::string filename = "C:\\Users\\" + username + "\\Desktop\\login_info.txt";
		std::ifstream infile(filename);
		if (infile.is_open()) {
			std::string line;
			if (std::getline(infile, line)) {
				size_t pos = line.find(',');
				if (pos != std::string::npos) {
					std::string saved_username = line.substr(0, pos);
					std::string saved_password = line.substr(pos + 1);
					strncpy_s(globals.username, IM_ARRAYSIZE(globals.username), saved_username.c_str(), _TRUNCATE);
					strncpy_s(globals.password, IM_ARRAYSIZE(globals.password), saved_password.c_str(), _TRUNCATE);
				}
			}
			infile.close();
		}
	}

	welcomeMessageShown = false;

	if (!globals.active) return;

	switch (globals.tab)
	{
		break;

	case 0:
	{
		ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(300, 300));
		ImGui::SetNextWindowBgAlpha(1.0f);

		ImGui::Begin(window_title, &globals.active, window_flags);
		{
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
			{
				if (ImGui::BeginTabItem("Login"))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

					ImGui::InputText("Username", globals.username, IM_ARRAYSIZE(globals.username));
					ImGui::InputText("Password", globals.password, IM_ARRAYSIZE(globals.password), ImGuiInputTextFlags_Password);

					static bool remember_me = false;
					ImGui::Checkbox("Remember me", &remember_me);

					ImGui::PopStyleColor(1);

					if (ImGui::Button("Login") || (ImGui::IsKeyPressedMap(ImGuiKey_Enter) && !ImGui::IsAnyItemActive()))
					{
						login_submitted = true;
						if (remember_me)
						{
							char* env;
							size_t len;
							_dupenv_s(&env, &len, "USERNAME");
							std::string username = env ? env : "User";
							free(env);
							std::string filename = "C:\\Users\\" + username + "\\Desktop\\login_info.txt";
							std::ofstream outfile(filename);
							outfile << globals.username << "," << globals.password;
							outfile.close();
						}
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Login, NULL, NULL, NULL);
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Register"))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

					ImGui::InputText("Username", globals.username, IM_ARRAYSIZE(globals.username));
					ImGui::InputText("Password", globals.password, IM_ARRAYSIZE(globals.password), ImGuiInputTextFlags_Password);
					ImGui::InputText("Key", globals.key, IM_ARRAYSIZE(globals.key));

					ImGui::PopStyleColor(1);

					if (ImGui::Button("Register")) {
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Register, NULL, NULL, NULL);
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Upgrade"))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

					ImGui::InputText("Username", globals.username, IM_ARRAYSIZE(globals.username));
					ImGui::InputText("Key", globals.key, IM_ARRAYSIZE(globals.key));

					ImGui::PopStyleColor(1);

					if (ImGui::Button("Upgrade")) {
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Upgrade, NULL, NULL, NULL);
					}
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}
	break;

	case 1:
	{
		if (KeyAuthApp.data.success)
		{
			static ImVec4 inactive = imguipp::to_vec4(101, 1, 11, 255);
			static ImVec4 active = imguipp::to_vec4(231, 1, 39, 255);
			static ImVec4 button_hover_color = imguipp::to_vec4(102, 0, 10, 255);
			static ImVec4 separator_color = imguipp::to_vec4(0, 0, 0, 255);

			ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(window_size.x, window_size.y));
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGui::Begin("OneDeepMenu", &globals.active, window_flags);
			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.59f, 0.0f, 0.10f, 1.0f)); // set background color to light gray
				ImGui::BeginChild("LeftPane", ImVec2(200, 0), false);
				ImGui::PopStyleColor();

				ImVec4 button_color = showCheatMenu ? inactive : active;

				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_hover_color);
				ImGui::PushStyleColor(ImGuiCol_Button, button_color);
				if (ImGui::Button("Zombies", ImVec2(230 - 20, 39)))
				{
					showCheatMenu = !showCheatMenu;
				}

				ImGui::PopStyleColor(2);

				ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 39);
				imguipp::center_text_ex(globals.username, 200, 1, false);

				ImGui::EndChild();

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Separator, separator_color);
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
				ImGui::PopStyleColor(1);

				if (showCheatMenu)
				{
					ImGui::SameLine();

					ImGui::BeginChild("RightPane", ImVec2(600, 0), false);

					static HWND hWnd = FindWindow(nullptr, "Call of Duty®: Black Ops II - Zombies");
					static HDC hdc = GetDC(hWnd);
					static DWORD pid;

					GetWindowThreadProcessId(hWnd, &pid);

					HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);

					static ImVec4 button_hover_color = imguipp::to_vec4(101, 1, 11, 255);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_hover_color);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 50);
					if (ImGui::Button("Toggle ESP"))
					{
						showESP = !showESP;
					}

					if (showESP)
					{
						// Get the local player's view matrix
						float viewMatrix[16];
						ReadProcessMemory(hProcess, (LPVOID)matrixAddr, &viewMatrix, sizeof(viewMatrix), nullptr);

						// Get the number of zombies in the game
						static int numZombies;
						static bool numZombiesInitialized = false;
						if (!numZombiesInitialized)
						{
							ReadProcessMemory(hProcess, (LPVOID)zombies, &numZombies, sizeof(numZombies), nullptr);
							numZombiesInitialized = true;
						}

						// Read the memory for all zombies
						float zombieData[20][4]; // array to store zombie data (x, y, z, health)
						for (int i = 0; i < 20; i++)
						{
							DWORD_PTR zombieAddr = baseAddr + next * i;
							ReadProcessMemory(hProcess, (LPVOID)(zombieAddr + xOffs), &zombieData[i][0], sizeof(float), nullptr);
							ReadProcessMemory(hProcess, (LPVOID)(zombieAddr + yOffs), &zombieData[i][1], sizeof(float), nullptr);
							ReadProcessMemory(hProcess, (LPVOID)(zombieAddr + zOffs), &zombieData[i][2], sizeof(float), nullptr);
							ReadProcessMemory(hProcess, (LPVOID)(zombieAddr + health), &zombieData[i][3], sizeof(float), nullptr);
						}

						// Loop through each zombie in the array
						for (int i = 0; i < 20; i++)
						{
							// Convert the zombie's world coordinates to screen coordinates
							POINT screenPos;
							if (!world2screen(zombieData[i], screenPos, viewMatrix, windowWidth, windowHeight))
							{
								continue; // Zombie is behind us or not visible
							}

							// Draw a rectangle around the zombie
							if (zombieData[i][3] > 0)
							{
								// Create a blue pen with a solid line style and a width of 1 pixel
								HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));

								// Select the pen into the device context
								HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

								// Set the brush to a NULL brush to make the rectangle transparent
								HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
								HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

								// Draw the rectangle with the selected pen and brush
								RECT rect = { screenPos.x - 30, screenPos.y - 50, screenPos.x + 10, screenPos.y + 100 };
								Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

								// Select the old pen and brush back into the device context
								SelectObject(hdc, hOldPen);
								SelectObject(hdc, hOldBrush);

								// Delete the blue pen object
								DeleteObject(hPen);
							}
						}
						// Release the device context
						ReleaseDC(hWnd, hdc);
					}

					if (ImGui::Button("Toggle God Mode"))
					{
						GodMode = !GodMode;
						if (GodMode)
						{
							SIZE_T bytesWritten;
							WriteProcessMemory(hProcess, Health, patchBytes, sizeof(patchBytes), &bytesWritten);
							if (bytesWritten != sizeof(patchBytes))
							{
							}
						}
						else
						{
							SIZE_T bytesWritten;
							WriteProcessMemory(hProcess, Health, originalBytes, sizeof(originalBytes), &bytesWritten);
							if (bytesWritten != sizeof(originalBytes))
							{
							}
						}
					}

					if (ImGui::Button("Toggle Unlimited Ammo"))
					{
						UnlimitedAmmo = !UnlimitedAmmo;
						if (UnlimitedAmmo)
						{
							SIZE_T bytesWritten;
							WriteProcessMemory(hProcess, Ammo, patchBytes2, sizeof(patchBytes2), &bytesWritten);
							if (bytesWritten != sizeof(patchBytes2))
							{
							}
						}
						else
						{
							SIZE_T bytesWritten;
							WriteProcessMemory(hProcess, Ammo, originalBytes2, sizeof(originalBytes2), &bytesWritten);
							if (bytesWritten != sizeof(originalBytes2))
							{
							}
						}
					}

					if (ImGui::Button("Toggle Noclip"))
					{
						Noclip = !Noclip;
						if (Noclip)
						{
							SIZE_T bytesWritten;
							WriteProcessMemory(hProcess, Fly, patchBytes3, sizeof(patchBytes3), &bytesWritten);
							if (bytesWritten != sizeof(patchBytes3))
							{
							}
						}
						else
						{
							SIZE_T bytesWritten;
							WriteProcessMemory(hProcess, Fly, originalBytes3, sizeof(originalBytes3), &bytesWritten);
							if (bytesWritten != sizeof(originalBytes3))
							{
							}
						}
					}
					ImGui::PopStyleColor(1);

					if (ImGui::Combo("Jump Height", &selected_height, jump_height, height))
					{
						const float height_id = height_ids[selected_height];

						WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(jumpheight), &height_id, sizeof(int), nullptr);
					}

					if (ImGui::Combo("Select Weapon", &selected_weapon, weapon_names, num_weapons))
					{
						const int weapon_id = weapon_ids[selected_weapon];

						WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(weaponchanger), &weapon_id, sizeof(int), nullptr);
					}

					if (ImGui::Combo("Select Money", &selected_amount, money_amount, num_money))
					{
						const int money_id = money_ids[selected_amount];
						WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(moneychanger), &money_id, sizeof(int), nullptr);
					}
					ImGui::EndChild();
				}
				else
				{
					if (!showCheatMenu && !welcomeMessageShown)
					{
						ImGui::SetCursorPos({ (ImGui::GetWindowSize().x - ImGui::CalcTextSize("Welcome to The Zombies menu!").x) * 0.65f, ImGui::GetWindowSize().y / 2 - 10 });
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Welcome to The Zombies menu!");
						ImGui::SetCursorPos({ (ImGui::GetWindowSize().x - ImGui::CalcTextSize("Click the Zombies button to get started.").x) * 0.66f, ImGui::GetWindowSize().y / 2 + 10 });
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Click the Zombies button to get started.");
						welcomeMessageShown = true;
					}
				}
			}
			ImGui::End();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		}
		break;
	}
	}
}

void ui::init(LPDIRECT3DDEVICE9 device) {
	dev = device;

	ImVec4 red = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_TitleBg] = black;
	style.Colors[ImGuiCol_TitleBgCollapsed] = black;
	style.Colors[ImGuiCol_TitleBgActive] = red;

	style.Colors[ImGuiCol_Text] = black;
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

	style.Colors[ImGuiCol_FrameBg] = red;
	style.Colors[ImGuiCol_FrameBgHovered] = red;
	style.Colors[ImGuiCol_FrameBgActive] = red;

	style.Colors[ImGuiCol_Border] = red;
	style.Colors[ImGuiCol_BorderShadow] = black;

	style.WindowTitleAlign = ImVec2(0.5, 0.5);
	style.FramePadding = ImVec2(8, 4);

	style.Colors[ImGuiCol_WindowBg] = black;

	style.Colors[ImGuiCol_Tab] = white;
	style.Colors[ImGuiCol_TabHovered] = red;
	style.Colors[ImGuiCol_TabActive] = red;
	style.Colors[ImGuiCol_TabUnfocused] = black;
	style.Colors[ImGuiCol_TabUnfocusedActive] = red;

	style.Colors[ImGuiCol_Button] = red;
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);

	style.Colors[ImGuiCol_Header] = red;
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);

	if (window_pos.x == 0) {
		RECT screen_rect{};
		GetWindowRect(GetDesktopWindow(), &screen_rect);
		screen_res = ImVec2(float(screen_rect.right), float(screen_rect.bottom));
		window_pos = (screen_res - window_size) * 0.5f;
	}
}