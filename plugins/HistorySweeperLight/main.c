/*
Miranda IM History Sweeper Light plugin
Copyright (C) 2002-2003  Sergey V. Gershovich
Copyright (C) 2006-2009  Boris Krasnovskiy
Copyright (C) 2010, 2011 tico-tico

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "historysweeperlight.h"
#include "version.h"

HINSTANCE hInst;
PLUGINLINK *pluginLink; 

struct MM_INTERFACE mmi;

static HANDLE hHooks[5];
int hLangpack;

static PLUGININFOEX pluginInfoEx =
{ 
	// about plugin
	sizeof(PLUGININFOEX),
	"History Sweeper Light",
	__VERSION_DWORD,
	"This plugin can sweep system history, history from all contacts or only from chosen; also it may sweep history older than certain time; and may do it at Miranda IM startup/shutdown.",
	"Sergey V. Gershovich a.k.a. Jazzy$, Boris Krasnovskiy, tico-tico",
	"",
	"� 2002-2003 Sergey V. Gershovich a.k.a. Jazzy$, 2006-2009 Boris Krasnovskiy, 2010, 2011 tico-tico",
	"",
	UNICODE_AWARE,
	0,
	MIID_HISTORYSWEEPERLIGHT
};

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	hInst=hinstDLL;
	return TRUE;
}

int OnIconPressed(WPARAM wParam, LPARAM lParam) 
{
	StatusIconClickData *sicd = (StatusIconClickData *)lParam;

	if ( !(sicd->flags & MBCF_RIGHTBUTTON) && !lstrcmpA(sicd->szModule, ModuleName) 
		&& DBGetContactSettingByte(NULL, ModuleName, "ChangeInMW", 0) )
	{
		int nh = sicd->dwId; HANDLE hContact = (HANDLE)wParam; StatusIconData sid = {0};

		sid.cbSize = sizeof(sid);
		sid.szModule = ModuleName;
		sid.dwId = nh;
		sid.flags = MBF_HIDDEN;
		CallService(MS_MSG_MODIFYICON, (WPARAM)hContact, (LPARAM)&sid);	
		
		nh = (nh + 1) % 4;
		DBWriteContactSettingByte((HANDLE)wParam, ModuleName, "SweepHistory", (BYTE)nh);

		sid.dwId = nh;
		sid.flags = 0;
		CallService(MS_MSG_MODIFYICON, (WPARAM)hContact, (LPARAM)&sid);
	}
	return 0;
}


int OnModulesLoaded(WPARAM wParam, LPARAM lParam) 
{
	StatusIconData sid = {0};
	int i, sweep = DBGetContactSettingByte(NULL, ModuleName, "SweepHistory", 0);
	HANDLE hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);

	sid.cbSize = sizeof(sid);
	sid.szModule = ModuleName;

	sid.hIcon = LoadIconEx("actG");
	if (sweep == 0)
		sid.szTooltip = Translate("Keep all events");
	else if (sweep == 1)
	{
		sid.szTooltip = Translate(time_stamp_strings[DBGetContactSettingByte(NULL, ModuleName, "StartupShutdownOlder", 0)]);
	}
	else if (sweep == 2)
	{
		sid.szTooltip = Translate(keep_strings[DBGetContactSettingByte(NULL, ModuleName, "StartupShutdownKeep", 0)]);
	}
	else if (sweep == 3)
	{
		sid.szTooltip = Translate("Delete all events");
	}
	sid.flags = MBF_HIDDEN;
	CallService(MS_MSG_ADDICON, 0, (LPARAM)&sid);

	sid.dwId = 1;
	sid.hIcon = LoadIconEx("act1");
	sid.szTooltip = Translate(time_stamp_strings[DBGetContactSettingByte(NULL, ModuleName, "StartupShutdownOlder", 0)]);
	sid.flags = MBF_HIDDEN;
	CallService(MS_MSG_ADDICON, 0, (LPARAM)&sid);

	sid.dwId = 2;
	sid.hIcon = LoadIconEx("act2");
	sid.szTooltip = Translate(keep_strings[DBGetContactSettingByte(NULL, ModuleName, "StartupShutdownKeep", 0)]);
	sid.flags = MBF_HIDDEN;
	CallService(MS_MSG_ADDICON, 0, (LPARAM)&sid);

	sid.dwId = 3;
	sid.hIcon = LoadIconEx("actDel");
	sid.szTooltip = Translate("Delete all events");
	sid.flags = MBF_HIDDEN;
	CallService(MS_MSG_ADDICON, 0, (LPARAM)&sid);
	
	// for new contacts
	while ( hContact != NULL )
	{
		ZeroMemory(&sid,sizeof(sid));

		sweep = DBGetContactSettingByte(hContact, ModuleName, "SweepHistory", 0);

		sid.cbSize = sizeof(sid);
		sid.szModule = ModuleName;
		
		for(i = 0; i < 4; i++)
		{
			sid.dwId = i;
			sid.flags = (sweep == i) ? 0 : MBF_HIDDEN;
			CallService(MS_MSG_MODIFYICON, (WPARAM)hContact, (LPARAM)&sid);
		}

		hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
	}

	hHooks[2] = HookEvent(ME_MSG_WINDOWEVENT, OnWindowEvent);
	hHooks[3] = HookEvent(ME_MSG_ICONPRESSED, OnIconPressed);

	return 0;
}

__declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	return &pluginInfoEx;
}

static const MUUID interfaces[] = {MIID_HISTORYSWEEPERLIGHT, MIID_LAST};
__declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;

	mir_getMMI(&mmi);
	mir_getLP(&pluginInfoEx);

	hHooks[0] = HookEvent(ME_SYSTEM_MODULESLOADED, OnModulesLoaded);
	hHooks[1] = HookEvent(ME_OPT_INITIALISE, HSOptInitialise);
	
	InitIcons();

	return 0;
}

int __declspec(dllexport) Unload(void)
{ 
	int i;

	ShutdownAction();

	for (i = 0; i < SIZEOF(hHooks); i++)
	{
		if (hHooks[i])
			UnhookEvent(hHooks[i]);
	}

	return 0;
}
