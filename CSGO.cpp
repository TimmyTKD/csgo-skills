#include <iostream>
#include <Windows.h>
#include "memory.h"
#include <thread>
#include <stdlib.h>
#include <fstream>
#include <map>
#include <type_traits>
#include <limits>
#include <iomanip>
#include <filesystem>
#include <string.h>
#include <math.h>
#include "vector.h"
#include "offsets.h"

using namespace std;

char* exePath = NULL;

DWORD ProcessIdFromName(LPCTSTR ProcessName) // non-conflicting function name
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) { // must call this first
		do {
			if (!lstrcmpi(pt.szExeFile, ProcessName)) {
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap); // close handle on failure
	return 0;
}

HWND FindTopWindow(DWORD pid)
{
	std::pair<HWND, DWORD> params = { 0, pid };

	// Enumerate the windows using a lambda to process each window
	BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
		{
			auto pParams = (std::pair<HWND, DWORD>*)(lParam);

			DWORD processId;
			if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second)
			{
				// Stop enumerating
				SetLastError(-1);
				pParams->first = hwnd;
				return FALSE;
			}

			// Continue enumerating
			return TRUE;
		}, (LPARAM)&params);

	if (!bResult && GetLastError() == -1 && params.first)
	{
		return params.first;
	}

	return 0;
}

int vecDist2D(Vector3 a, Vector3 b, bool doAbs = true) {
	if (doAbs) {
		return abs(sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
	}
	else {
		float mult = 1;
		if ((a.x - b.x + a.y - b.y) < 0) {
			mult = -1;
		}
		return mult * sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
	}
}


int vecDist(Vector3 a, Vector3 b, bool doAbs = true) {
	if (doAbs) {
		return abs(sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2)));
	}
	else {
		float mult = 1;
		if ((a.x - b.x + a.y - b.y + a.z - b.z) < 0) {
			mult = -1;
		}
		return mult * sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
	}
}


float PI() { return std::atan(1) * 4; }

void LeftClick()
{
	INPUT Input = { 0 };
	// left down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}

void RightClick()
{
	INPUT Input = { 0 };
	// left down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}

constexpr const int RandomizeDelay(const auto& itemDefinition)
{
	switch (itemDefinition)
	{
	case 1: return 280 + (rand() % 300); // deagle
	case 4: return 10 + (rand() % 20); // glock
	case 7: return 80 + (rand() % 100); // ak
	case 9: return 390 + (rand() % 400); // awp
	case 61: return 10 + (rand() % 20); // usp
	default: return 70 + (rand() % 90); // other
	}
}

constexpr const int GetWeaponPaint(const short& itemDefinition)
{
	switch (itemDefinition)
	{
	case 1: return 231; // deagle
	case 4: return 38; // glock
	case 7: return 456; // ak
	case 9: return 640; // awp
	case 61: return 60; // usp
	default: return 0;
	}
}


string defaultOptions = "[1]ON [0]OFF";

class Setting {
public: string Name;
public: string Value = "ON";
public: string Options = defaultOptions;

public: Setting(string name, string value) {
	Name = name;
	Value = value;
}

public: Setting(string name, string value, string options) {
	Name = name;
	Value = value;
	Options = options;
}
};

Setting Settings[] = {
	Setting("TriggerBot", "ON"),
	Setting("TriggerDelay", "70", "Type In Miliseconds"),
	Setting("GunId", "ON"),
	Setting("TriggerSnap", "ON"),
	Setting("AimCheck", "ON"),
	Setting("BHOP", "ON"),
	Setting("NoFlash", "ON"),
	Setting("NoRecoil", "ON"),
	Setting("Esp", "ON"),
	Setting("SelectiveTrigger", "ON"),
	Setting("SnapDelay", "75", "Type In Miliseconds"),
	Setting("Aimbot", "ON"),
	Setting("AimbotDelay", "0.5", "Type In Miliseconds"),
	Setting("AutoStrafe", "ON"),
	Setting("AimSmoothness", "4", "Type Amount (>0)"),
	Setting("AimbotFOV", "10", "Type FOV (>0)"),
	Setting("WallCheck", "ON"),
	Setting("Chams", "ON"),
	Setting("ChamsBright", "600", "Type in amount")
};

Setting GetSetting(string settingName) {
	Setting obj = Setting("NULL", "NULL");
	bool found = false;

	for (size_t i = 0; i < sizeof(Settings); i++)
	{
		if (Settings[i].Name == settingName) {
			obj = Settings[i];
			found = true;
			break;
		}
	}

	if (found == false) {
		cout << "\n\n\nError\n\n\n";
	}

	return obj;
}

void CmdPrint()
{
	if (GetSetting("TriggerDelay").Value == "10") {
		cout << "a";
	}
	else {
		cout << "b";
	}
	ifstream infile;
	infile.open("Settings.txt");

	for (size_t i = 0; i < size(Settings); i++)
	{
		string a;
		infile >> a;
		Settings[i].Value = a;
	}

	infile.close();

	int answer = NULL;
	int answer2 = NULL;

	while (true)
	{
		system("CLS");

		string output = "[-1] Restart Client\n[0] Save Config\n";

		for (size_t i = 0; i < size(Settings); i++)
		{
			output += "[" + to_string(i + 1) + "] " + Settings[i].Name + ": " + Settings[i].Value + "\n";
		}

		cout << output;

		cout << "\n";

		cin >> answer;

		cout << "\n";

		if (answer <= size(Settings) && answer > 0 && floor(answer) == answer) {
			cout << Settings[answer - 1].Options << "\n";
			cin >> answer2;

			if (Settings[answer - 1].Options == defaultOptions) {
				cout << "a";
				if (answer2 == 1)
				{
					Settings[answer - 1].Value = "ON";
					continue;
				}
				else if (answer2 == 0)
				{
					Settings[answer - 1].Value = "OFF";
					continue;
				}
			}
			else {
				Settings[answer - 1].Value = to_string(answer2);
				continue;
			}
		}
		else if (answer == 0) {
			fstream Config;
			Config.open("Settings.txt", ios::out); // write
			if (Config.is_open())
			{
				for (size_t i = 0; i < size(Settings); i++)
				{
					Config << Settings[i].Value << endl;
				}
			}


			Config.open("Settings.txt", ios::in);

			cout << "\n\n Saved settings to Settings.txt";

			Sleep(900);

			continue;
		}
		else if (answer == -1) {
			std::ostringstream path;

			path << '\"' << exePath << '\"';

			cout << "\n\n-------------------\nRestarting Client...\n-------------------\n\n";

			Sleep(1000);

			ShellExecuteA(NULL, "open", path.str().c_str(), NULL, NULL, SW_NORMAL);

			Sleep(100);

			ExitProcess(0);
		}
	}
}

void Bhop()
{
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");

	while (true)
	{
		if (GetSetting("BHOP").Value == "ON")
		{
			auto startTime = std::chrono::system_clock::now();
			while (GetAsyncKeyState(VK_SPACE))
			{
				Sleep(0.1f);
				auto currentTime = std::chrono::system_clock::now();

				if (std::chrono::duration<double>(currentTime - startTime).count() > 4)
				{
					Sleep(10);
				}

				const auto& localPlayer = memory.Read<uintptr_t>(client + offsets::dwLocalPlayer);
				const auto onGround = memory.Read<bool>(localPlayer + offsets::m_fFlags);

				if (onGround & (1 << 0))
				{
					memory.Write<BYTE>(client + offsets::dwForceJump, 6);
				}
			}
		}
		Sleep(10);
	}
}

struct Vector2
{
	float x = { }, y = { };
};

void TriggerBot()
{
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");
	bool Trigger = false;

	//cout << hex << "client.dll -> 0x" << client << dec << endl;

	while (true)
	{
		const auto& localPlayer = memory.Read<uintptr_t>(client + offsets::dwLocalPlayer);
		const auto& localTeam = memory.Read<int32_t>(localPlayer + offsets::m_iTeamNum);

		const auto& localHealth = memory.Read<int32_t>(localPlayer + offsets::m_iHealth);

		const auto& crosshairId = memory.Read<int32_t>(localPlayer + offsets::m_iCrosshairId);
		const auto& player = memory.Read<uintptr_t>(client + offsets::dwEntityList + (crosshairId - 1) * 0x10);
		const auto& playerTeam = memory.Read<int32_t>(player + offsets::m_iTeamNum);

		const auto& clientstate = memory.Read<uintptr_t>(engine + offsets::dwClientState);
		Vector2 rot = memory.Read<Vector2>(clientstate + offsets::dwClientState_ViewAngles);

		uintptr_t iCurWeaponAdress = memory.Read< std::uintptr_t >(localPlayer + offsets::m_hActiveWeapon) & 0xFFF;
		uintptr_t m_iBase = memory.Read< std::uintptr_t >(client + offsets::dwEntityList + (iCurWeaponAdress - 1) * 0x10);

		short id = memory.Read< short >(m_iBase + offsets::m_iItemDefinitionIndex);

		Sleep(2);

		if (!crosshairId || crosshairId > 64)
		{
			Trigger = false;
			continue;
		}

		if (localTeam == playerTeam)
			continue;

		if (!Trigger)
		{
			Trigger = true;
			Sleep(stoi(GetSetting("TriggerDelay").Value));
		}

		if (GetSetting("TriggerBot").Value == "OFF")
			continue;

		if (!localHealth)
			continue;

		const auto& zoomLevel = memory.Read<uintptr_t>(m_iBase + offsets::m_zoomLevel);

		if (GetSetting("SelectiveTrigger").Value == "ON")
		{
			if (id == 9 || id == 40)
			{
				if (GetSetting("AimCheck").Value == "ON" && zoomLevel == 0)
				{
					RightClick();
					Sleep(80);
					LeftClick();
					Sleep(RandomizeDelay(id));
					continue;
				}
				else
				{
					LeftClick();
					Sleep(RandomizeDelay(id));
					continue;
				}
			}
			continue;
		}

		if (GetSetting("AimCheck").Value == "ON")
		{
			if (zoomLevel == 1 && id == 9)
			{
				RightClick();
				Sleep(80);
				LeftClick();
			}
		}


		if (GetSetting("TriggerSnap").Value == "OFF")
		{
			LeftClick();
		}

		if (GetSetting("TriggerSnap").Value == "ON")
		{
			Sleep(stoi(GetSetting("SnapDelay").Value));
			Vector2 newrot = memory.Read<Vector2>(clientstate + offsets::dwClientState_ViewAngles);
			memory.Write<Vector2>(clientstate + offsets::dwClientState_ViewAngles, rot);
			//hasLastPlayer = true;
			//lastPlayer = rot;
			LeftClick();

			for (size_t i = 0; i < 500; i += 1)
			{
				Sleep(0);
				memory.Write<Vector2>(clientstate + offsets::dwClientState_ViewAngles, rot);
			}

			Sleep(1);
			memory.Write<Vector2>(clientstate + offsets::dwClientState_ViewAngles, newrot);
		}

		if (GetSetting("GunId").Value == "ON")
		{
			Sleep(RandomizeDelay(id));
		}
		else
		{
			Sleep(1);
		}

	}
}

void AntiFlash()
{
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");

	while (true)
	{
		Sleep(500);
		const auto& localPlayer = memory.Read<uintptr_t>(client + offsets::dwLocalPlayer);
		auto flashDur = memory.Read<uintptr_t>(localPlayer + offsets::m_flFlashDuration);

		if (GetSetting("NoFlash").Value == "ON")
		{
			if (flashDur > 0)
			{
				memory.Write<int>(localPlayer + offsets::m_flFlashDuration, 0);
			}
		}
	}
}

void AntiRecoil()
{
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");

	auto oldPunch = Vector2{ };

	while (true)
	{
		Sleep(50);
		if (GetSetting("NoRecoil").Value == "OFF")
			continue;

		const auto& localPlayer = memory.Read<uintptr_t>(client + offsets::dwLocalPlayer);
		const auto& ShotsFired = memory.Read<int32_t>(localPlayer + offsets::m_iShotsFired);

		uintptr_t iCurWeaponAdress = memory.Read< std::uintptr_t >(localPlayer + offsets::m_hActiveWeapon) & 0xFFF;
		uintptr_t m_iBase = memory.Read< std::uintptr_t >(client + offsets::dwEntityList + (iCurWeaponAdress - 1) * 0x10);

		short id = memory.Read< short >(m_iBase + offsets::m_iItemDefinitionIndex);

		if (ShotsFired > 1)
		{
			const auto& clientState = memory.Read<uintptr_t>(engine + offsets::dwClientState);
			const auto& viewAngles = memory.Read<Vector2>(clientState + offsets::dwClientState_ViewAngles);

			const auto& aimPunch = memory.Read<Vector2>(localPlayer + offsets::m_aimPunchAngle);

			auto newAngles = Vector2
			{
				viewAngles.x + oldPunch.x - aimPunch.x * 2.f,
				viewAngles.y + oldPunch.y - aimPunch.y * 2.f,
			};

			if (newAngles.x > 89.f)
				newAngles.x = 89.f;

			if (newAngles.x < -89.f)
				newAngles.x = -89.f;

			while (newAngles.y > 180.f)
				newAngles.y -= 360.f;

			while (newAngles.y < -180.f)
				newAngles.y += 360.f;

			memory.Write<Vector2>(clientState + offsets::dwClientState_ViewAngles, newAngles);

			oldPunch.x = aimPunch.x * 2.f;
			oldPunch.y = aimPunch.y * 2.f;

		}
		else
		{
			oldPunch.x = oldPunch.y = 0.f;
		}
	}
}

DWORD getClosest() {
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");

	int dist = 1000000000;

	DWORD closest = NULL;

	for (int i = 1; i < 63; i++)
	{
		const auto& entity = memory.Read<DWORD>(client + offsets::dwEntityList + i * 0x10);
		const auto& localPlayer = memory.Read<uintptr_t>(client + offsets::dwLocalPlayer);
		const auto& health = memory.Read<int32_t>(entity + offsets::m_iHealth);
		const auto& dormant = memory.Read<bool>(entity + offsets::m_bDormant);

		if (!entity) {
			continue;
		}

		if (dormant) {
			continue;
		}

		if (!health) {
			continue;
		}
		if (memory.Read<uintptr_t>(entity + offsets::m_iTeamNum) == memory.Read<uintptr_t>(localPlayer + offsets::m_iTeamNum)) {
			continue;
		}

		if (GetSetting("WallCheck").Value == "ON")
		{
			if (!memory.Read<bool>(entity + offsets::m_bSpottedByMask)) {
				continue;
			}
		}

		if (memory.Read<int32_t>(entity + offsets::m_lifeState) != 0) {
			continue;
		}

		Vector3 pos = memory.Read<Vector3>(entity + offsets::m_vecOrigin);

		Vector3 lPos = memory.Read<Vector3>(localPlayer + offsets::m_vecOrigin);
		const auto& clientState = memory.Read<uintptr_t>(engine + offsets::dwClientState);
		Vector2 lRot = memory.Read<Vector2>(clientState + offsets::dwClientState_ViewAngles);

		if (lRot.y < 0) {
			lRot = Vector2(lRot.x, lRot.y + 360);
		}

		if (lRot.x < 0) {
			lRot = Vector2(lRot.x + 360, lRot.y);
		}

		Vector3 lPos2 = Vector3(lPos.x + cos(lRot.y * PI() / 180) * vecDist(lPos, pos), lPos.y + sin(lRot.y * PI() / 180) * vecDist(lPos, pos), lPos.z - sin(lRot.x * PI() / 180) * vecDist(lPos, pos));

		if (vecDist(lPos2, pos) < dist && entity) {
			closest = entity;
			dist = vecDist(lPos2, pos);
		}

		//memory.Write<Vector3>(localPlayer + 0x114, Vector3(500, 500, 500));
	}
	return closest;
}

void GlowEsp()
{
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");

	const auto& glowManager = memory.Read<uintptr_t>(client + offsets::dwGlowObjectManager);

	uintptr_t localPlayer = NULL;
	int32_t LTeam = NULL;

	DWORD entity = NULL;
	int glowIndex = NULL;
	int32_t entTeam = NULL;

	while (true) {
		Sleep(0.7);

		if (GetSetting("Esp").Value != "ON")
			continue;

		localPlayer = memory.Read<uintptr_t>(client + offsets::dwLocalPlayer);
		LTeam = memory.Read<int32_t>(localPlayer + offsets::m_iTeamNum);

		for (int i = 1; i < 32; i++)
		{
			entity = memory.Read<DWORD>(client + offsets::dwEntityList + i * 0x10);

			glowIndex = memory.Read<int>(entity + offsets::m_iGlowIndex);
			entTeam = memory.Read<int32_t>(entity + offsets::m_iTeamNum);

			if (entTeam != LTeam)
			{
				if (entTeam == 2) {
					memory.Write<float>(glowManager + (glowIndex * 0x38) + 0x8, 0.8f); // r
					memory.Write<float>(glowManager + (glowIndex * 0x38) + 0xC, 0.2f); // g
					memory.Write<float>(glowManager + (glowIndex * 0x38) + 0x10, 0.2f); // b
					memory.Write<float>(glowManager + (glowIndex * 0x38) + 0x14, 0.8f); // a

					memory.Write<bool>(glowManager + (glowIndex * 0x38) + 0x28, true);
					memory.Write<bool>(glowManager + (glowIndex * 0x38) + 0x29, false);
				}
				if (entTeam == 3)
				{
					memory.Write<float>(glowManager + (glowIndex * 0x38) + 0x8, 0.4f); // r
					memory.Write<float>(glowManager + (glowIndex * 0x38) + 0xC, 0.4f); // g
					memory.Write<float>(glowManager + (glowIndex * 0x38) + 0x10, 0.5f); // b
					memory.Write<float>(glowManager + (glowIndex * 0x38) + 0x14, 0.8f); // a

					memory.Write<bool>(glowManager + (glowIndex * 0x38) + 0x28, true);
					memory.Write<bool>(glowManager + (glowIndex * 0x38) + 0x29, false);
				}

				if (memory.Read<bool>(entity + offsets::m_bSpotted)) {
					memory.Write<float>(glowManager + (glowIndex * 0x38) + 0x8, 1.f); // r
					memory.Write<float>(glowManager + (glowIndex * 0x38) + 0xC, 0.4f); // g
					memory.Write<float>(glowManager + (glowIndex * 0x38) + 0x10, 1.f); // b
					memory.Write<float>(glowManager + (glowIndex * 0x38) + 0x14, 0.7f); // a

					memory.Write<bool>(glowManager + (glowIndex * 0x38) + 0x28, true);
					memory.Write<bool>(glowManager + (glowIndex * 0x38) + 0x29, false);
				}
			}
		}
	}
}

class BoneMatrix
{
public:
	BYTE pad3[12];
	float x;
	BYTE pad1[12];
	float y;
	BYTE pad2[12];
	float z;
};

void Aimbot() {
	const auto memory = Memory{ "csgo.exe" };
	const auto engine = memory.GetModuleAddress("engine.dll");
	const auto client = memory.GetModuleAddress("client.dll");

	while (true)
	{
		Sleep(12.5f);

		DWORD closest = getClosest();

		const auto& localPlayer = memory.Read<uintptr_t>(client + offsets::dwLocalPlayer);

		int KeyBind = VK_LBUTTON;
		int KeyBind2 = VK_LMENU;

		uintptr_t iCurWeaponAdress = memory.Read< std::uintptr_t >(localPlayer + offsets::m_hActiveWeapon) & 0xFFF;
		uintptr_t m_iBase = memory.Read< std::uintptr_t >(client + offsets::dwEntityList + (iCurWeaponAdress - 1) * 0x10);

		short id = memory.Read< short >(m_iBase + offsets::m_iItemDefinitionIndex);

		if (id == 44 || id == 45 || id == 48 || id == 68 || id == 83 || id == 41 || id == 42 || id == 59 || id == 80 || id == 505 || id == 506 || id == 507 || id == 508 || id == 509 || id == 512 || id == 514 || id == 515 || id == 516 || id == 519 || id == 520 || id == 522 || id == 523)
			continue;

		const auto& crosshairId = memory.Read<int32_t>(localPlayer + offsets::m_iCrosshairId);

		bool stop = false;

		if ((GetKeyState(KeyBind) & 0x8000) == 0 || (GetKeyState(KeyBind2) & 0x8000)) {
			stop = false;
		}

		const int breakChecks = 4;

		int dir = 1;
		float degs[breakChecks]{};
		int flips = 0;
		int lockedDir = 0;

		Vector2 lRot = Vector2(NULL, NULL);
		bool setlRot = false;

		int index = 0;

		if (stop == false) {
			while (((GetKeyState(KeyBind) & 0x8000) != 0 || (GetKeyState(KeyBind2) & 0x8000) != 0) && closest != NULL)
			{
				stop = true;
				Sleep(5);

				index++;

				bool cancel = false;

				auto dwboneMatrix = memory.Read<DWORD>(closest + offsets::m_dwBoneMatrix);
				BoneMatrix boneMatrix = memory.Read<BoneMatrix>(dwboneMatrix + 0x30 * 8);

				Vector3 pos = Vector3(boneMatrix.x, boneMatrix.y, boneMatrix.z - 64);

				//Vector3 pos = memory.Read<Vector3>(closest + offsets::m_vecOrigin);

				//cout << "\n" << boneMatrix.x << "," << boneMatrix.y << "," << boneMatrix.z << "|" << pos.x << "," << pos.y << "," << pos.z;

				auto dwboneMatrixP = memory.Read<DWORD>(localPlayer + offsets::m_dwBoneMatrix);
				BoneMatrix boneMatrixP = memory.Read<BoneMatrix>(dwboneMatrixP + 0x30 * 8);
				Vector3 lPos = Vector3(boneMatrixP.x, boneMatrixP.y, boneMatrixP.z - 64);
				//Vector3 lPos = memory.Read<Vector3>(localPlayer + offsets::m_vecOrigin);

				const auto& clientState = memory.Read<uintptr_t>(engine + offsets::dwClientState);
				if (setlRot == false) {
					setlRot = true;
					lRot = memory.Read<Vector2>(clientState + offsets::dwClientState_ViewAngles);
				}

				if (lRot.y < 0) {
					lRot = Vector2(lRot.x, lRot.y + 360);
				}

				if (lRot.x < 0) {
					lRot = Vector2(lRot.x + 360, lRot.y);
				}

				float dist = vecDist(lPos, pos);
				float distXY = vecDist2D(lPos, pos);
				Vector3 mousePos = Vector3(lPos.x + cos(lRot.y * PI() / 180) * dist, lPos.y + sin(lRot.y * PI() / 180) * dist, lPos.z - sin(lRot.x * PI() / 180) * dist + 4.f);

				float mouseDist = vecDist2D(mousePos, pos, false);

				float angleRad = 2 * asin(mouseDist / (2 * distXY));
				float angleDeg = angleRad * 180 / PI();

				// ------------ Break checking ------------------ \\

				if (lRot.y < 41 || lRot.y > 227) {
					angleDeg *= -1;
				}

				//float cDir = angleDeg < 0 ? -1 : 1;
				//if (cDir != dir) {
				//	dir = cDir;
				//	flips++;

				//	if (flips > 1 && flips < breakChecks + 2) {
				//		degs[flips - 2] = angleDeg;
				//		//cout << "\n" << (flips - 2) << ": " << angleDeg;
				//		cancel = true;
				//	}

				//	if (flips > breakChecks) {
				//		float biggestDeg = 0;
				//		for (size_t i = 0; i < breakChecks; i++)
				//		{
				//			if (abs(degs[i]) > biggestDeg) {
				//				biggestDeg = abs(degs[i]);
				//				lockedDir = (degs[i] < 0 ? -1 : 1);
				//			}
				//		}
				//	}
				//}

				//if (flips > breakChecks) {
				//	angleDeg = abs(angleDeg) * lockedDir;
				//}

				//---------------------------------------------- \\

				float mouseDistY = vecDist(Vector3(pos.x, pos.y, mousePos.z), pos, false);

				float angleRadY = 2 * asin(mouseDistY / (2 * vecDist(lPos, Vector3(pos.x, pos.y, mousePos.z))));
				float angleDegY = angleRadY * 180 / PI();

				float clampMax = 60;

				if (index == 1) {
					clampMax = 1;
				}

				if (index == 2) {
					clampMax = 5;
				}

				if (index == 3) {
					clampMax = 25;
				}

				float xAngle = abs(angleDegY / stof(GetSetting("AimSmoothness").Value)) < 0.1 / stof(GetSetting("AimSmoothness").Value) ? angleDegY : angleDegY / stof(GetSetting("AimSmoothness").Value);
				float yAngle = abs(angleDeg / stof(GetSetting("AimSmoothness").Value)) < 0.25 / stof(GetSetting("AimSmoothness").Value) ? angleDeg : angleDeg < 0 ? clamp(angleDeg / stof(GetSetting("AimSmoothness").Value), -clampMax, -0.001f) : clamp(angleDeg / stof(GetSetting("AimSmoothness").Value), 0.001f, clampMax);

				if (abs(xAngle) < 0.02 && abs(yAngle) < 0.02)
					continue;

				//cout << "\n"  << lRot.y << " | " << angleDeg;

				lRot = Vector2(lRot.x + xAngle, lRot.y + yAngle);


				if (lRot.y > 180) {
					lRot = Vector2(lRot.x, lRot.y - 360);
				}

				if (lRot.x > 180) {
					lRot = Vector2(lRot.x - 360, lRot.y);
				}

				/*const auto& entity2 = memory.Read<DWORD>(client + offsets::dwEntityList + 5 * 0x10);
				memory.Write<Vector3>(entity2 + offsets::m_vecOrigin, mousePos);*/

				if (isnan(lRot.x) || isnan(lRot.y)) {
					lRot = memory.Read<Vector2>(clientState + offsets::dwClientState_ViewAngles);
					break;
				}

				if (!isnan(stof(GetSetting("AimbotFOV").Value))) {
					if (abs(angleDeg) >= stof(GetSetting("AimbotFOV").Value) || abs(angleDegY) >= stof(GetSetting("AimbotFOV").Value))
						break;
				}

				if (closest && memory.Read<int32_t>(closest + offsets::m_lifeState) == 0) {
					if (cancel == false) {
						memory.Write<Vector2>(clientState + offsets::dwClientState_ViewAngles, lRot);
					}
				}
				else {
					Sleep(stoi(GetSetting("AimbotDelay").Value));
					stop = false;
					break;
				}
			}
		}
	}
}


void AutoStrafe()
{
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");
	bool Activated = false;

	while (true)
	{
		if (GetSetting("AutoStrafe").Value != "ON")
			continue;
		Sleep(1);
		const auto& localPlayer = memory.Read<uintptr_t>(client + offsets::dwLocalPlayer);
		const auto& clientState = memory.Read<uintptr_t>(engine + offsets::dwClientState);
		const auto& viewAngles = memory.Read<Vector2>(clientState + offsets::dwClientState_ViewAngles);

		if (GetSetting("BHOP").Value == "ON")
		{
			const auto& localPlayer = memory.Read<uintptr_t>(client + offsets::dwLocalPlayer);
			const auto onGround = memory.Read<bool>(localPlayer + offsets::m_fFlags);

			Sleep(10);

			const auto& newAngle = memory.Read<Vector2>(clientState + offsets::dwClientState_ViewAngles);

			// LEFT STRAFE
			if (newAngle.y > viewAngles.y && GetAsyncKeyState(VK_SPACE))
			{
				SHORT key;
				UINT mappedkey;
				INPUT input = { 0 };
				key = VkKeyScan('d');
				mappedkey = MapVirtualKey(LOBYTE(key), 0);
				input.type = INPUT_KEYBOARD;
				input.ki.dwFlags = KEYEVENTF_SCANCODE;
				input.ki.wScan = mappedkey;
				input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
				SendInput(1, &input, sizeof(input));

				key;
				mappedkey;
				input = { 0 };
				key = VkKeyScan('a');
				mappedkey = MapVirtualKey(LOBYTE(key), 0);
				input.type = INPUT_KEYBOARD;
				input.ki.dwFlags = KEYEVENTF_SCANCODE;
				input.ki.wScan = mappedkey;
				SendInput(1, &input, sizeof(input));
				Activated = true;
			}


			// LEFT STRAFE
			if (newAngle.y < viewAngles.y && GetAsyncKeyState(VK_SPACE))
			{

				SHORT key;
				UINT mappedkey;
				INPUT input = { 0 };
				key = VkKeyScan('a');
				mappedkey = MapVirtualKey(LOBYTE(key), 0);
				input.type = INPUT_KEYBOARD;
				input.ki.dwFlags = KEYEVENTF_SCANCODE;
				input.ki.wScan = mappedkey;
				input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
				SendInput(1, &input, sizeof(input));

				key;
				mappedkey;
				input = { 0 };
				key = VkKeyScan('d');
				mappedkey = MapVirtualKey(LOBYTE(key), 0);
				input.type = INPUT_KEYBOARD;
				input.ki.dwFlags = KEYEVENTF_SCANCODE;
				input.ki.wScan = mappedkey;
				SendInput(1, &input, sizeof(input));
				Activated = true;
			}

			// Kill if on ground
			if (onGround && (GetAsyncKeyState(0x41) || GetAsyncKeyState(0x44)) && Activated && !GetAsyncKeyState(VK_SPACE))
			{
				SHORT key;
				UINT mappedkey;
				INPUT input = { 0 };
				key = VkKeyScan('a');
				mappedkey = MapVirtualKey(LOBYTE(key), 0);
				input.type = INPUT_KEYBOARD;

				input.ki.dwFlags = KEYEVENTF_SCANCODE;
				input.ki.wScan = mappedkey;
				input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
				SendInput(1, &input, sizeof(input));


				input = { 0 };
				key = VkKeyScan('d');
				mappedkey = MapVirtualKey(LOBYTE(key), 0);
				input.type = INPUT_KEYBOARD;
				input.ki.dwFlags = KEYEVENTF_SCANCODE;
				input.ki.wScan = mappedkey;
				input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
				SendInput(1, &input, sizeof(input));

				Activated = false;
			}
			Sleep(20);
		}
	}
}

void Keybinds()
{
	HWND hWnd = GetConsoleWindow();
	HWND csgo = FindTopWindow(ProcessIdFromName("csgo.exe"));

	while (true)
	{
		Sleep(500);
		if (GetAsyncKeyState(VK_RMENU))
		{
			if (IsIconic(hWnd) || GetForegroundWindow() != hWnd) {
				ShowWindow(hWnd, SW_NORMAL);
				SetForegroundWindow(hWnd);
				SetFocus(hWnd);
				SetActiveWindow(hWnd);
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
			}
			else {
				ShowWindow(csgo, SW_NORMAL);
				SetForegroundWindow(csgo);
				SetFocus(csgo);
				SetActiveWindow(csgo);
				SetWindowPos(csgo, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				SetWindowPos(csgo, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
			}
		}
	}
}

struct Color
{
	uint8_t r{ }, g{ }, b{ };
};

void Chams()
{
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");

	const auto Team = Color{ 255, 50, 0 };
	const auto Back = Color{ 255, 255, 255 };

	while (true)
	{
		Sleep(50);
		if (GetSetting("Chams").Value == "OFF")
		{
			for (int i = 1; i <= 32; ++i)
			{
				const auto entity = memory.Read<uintptr_t>(client + offsets::dwEntityList + i * 0x10);
				const auto entTeam = memory.Read<int32_t>(entity + offsets::m_iTeamNum);
				const auto localPlayer = memory.Read<uintptr_t>(client + offsets::dwLocalPlayer);
				const auto LTeam = memory.Read<int32_t>(localPlayer + offsets::m_iTeamNum);

				memory.Write<Color>(entity + offsets::m_clrRender, Back);

				float brightness1 = 0;
				const auto _this = static_cast<uintptr_t>(engine + offsets::model_ambient_min - 0x2c);
				memory.Write<int32_t>(engine + offsets::model_ambient_min, *reinterpret_cast<uintptr_t*>(&brightness1) ^ _this);
			}
			continue;
		}

		for (int i = 1; i <= 64; ++i)
		{
			const auto entity = memory.Read<uintptr_t>(client + offsets::dwEntityList + i * 0x10);
			const auto entTeam = memory.Read<int32_t>(entity + offsets::m_iTeamNum);
			const auto localPlayer = memory.Read<uintptr_t>(client + offsets::dwLocalPlayer);
			const auto LTeam = memory.Read<int32_t>(localPlayer + offsets::m_iTeamNum);

			if (LTeam != entTeam)
			{
				memory.Write<Color>(entity + offsets::m_clrRender, Team);
			}
			else
			{
				memory.Write<Color>(entity + offsets::m_clrRender, Back);
			}
			float brightness = stof(GetSetting("ChamsBright").Value);
			const auto _this = static_cast<uintptr_t>(engine + offsets::model_ambient_min - 0x2c);
			memory.Write<int32_t>(engine + offsets::model_ambient_min, *reinterpret_cast<uintptr_t*>(&brightness) ^ _this);
		}

	}
}

int main(int argc, char* argv[])
{
	exePath = argv[0];

	thread t1(CmdPrint);
	thread t2(Bhop);
	thread t3(TriggerBot);
	thread t4(AntiFlash);
	thread t5(AntiRecoil);
	thread t6(GlowEsp);
	thread t7(Aimbot);
	thread t8(AutoStrafe);
	thread t9(Keybinds);
	thread t10(Chams);
	t1.join();
}
