#pragma once

class EditorUI;

class CImGuiMgr
	: public singleton<CImGuiMgr>
{
	SINGLE(CImGuiMgr)
private:
	map<string, EditorUI*>	m_mapUI;
	HANDLE					m_hNotify;

public:
	EditorUI* FindUI(const string& _ID);

public:
	int Init();
	void Progress();

private:
	void CreateEditorUI();
	void ObserveContent();

private:
	void Tick();
	void Render();
};

