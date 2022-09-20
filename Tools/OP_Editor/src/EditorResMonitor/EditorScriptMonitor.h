#pragma once

#include <Utils/FileMonitor.h>
#include <ScriptManager/ScriptManager.h>

namespace OP
{
	class EditorScriptMonitor
	{
	public:
		EditorScriptMonitor() : m_FM(std::string("assets/scripts/bin/"), std::chrono::milliseconds(1000))
		{
			m_Thread = std::thread([&]()-> void
			{
				m_FM.StartWatching([&](std::string path, FileStatus status) -> void
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

		void Terminate()
		{
			m_FM.Stop();
		}

		bool IsUpdated()
		{
			return m_Updated;
		}

		FileMonitor m_FM;
		bool m_Updated = false;
		std::thread m_Thread;
		bool m_Work = true;
		
	};
}
