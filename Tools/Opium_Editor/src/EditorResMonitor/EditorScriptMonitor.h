#pragma once

#include <Utils/FileMonitor.h>
#include <ScriptManager/ScriptManager.h>

namespace OP
{
	class EditorScriptMonitor
	{
	public:
		EditorScriptMonitor()
		{
			m_Thread = std::thread([&]()-> void
			{
				FileMonitor fM(std::string("assets/scripts/bin/"), std::chrono::milliseconds(1000));
				fM.StartWatching([&](std::string path, FileStatus status) -> void
					{
						if (m_Work)
						{
							if (status == FileStatus::modified)
								m_Updated = true;
								//ScriptManager::ReloadAssembly();
						}
					});
			});
		}

		void Stop()
		{
			m_Work = false;
		}

		void Start()
		{
			m_Work = true;
		}
		
		void SetUpdated(bool newUpdated)
		{
			m_Updated = newUpdated;
		}

		bool IsUpdated()
		{
			return m_Updated;
		}

		bool m_Updated = false;
		std::thread m_Thread;
		bool m_Work = true;
		
	};
}
