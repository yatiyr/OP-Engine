#pragma once

#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>


enum class FileStatus {created, modified, deleted};


class FileMonitor
{
public:
	std::string watchPath;

	// Check duration
	std::chrono::duration<int, std::milli> interval;

	FileMonitor(std::string wP, std::chrono::duration<int, std::milli> i) :
		watchPath( wP ), interval( i )
	{
		for (auto& f : std::filesystem::recursive_directory_iterator(watchPath))
		{
			m_Paths[f.path().string()] = std::filesystem::last_write_time(f);
		}
	}


	void StartWatching(const std::function<void (std::string, FileStatus)>& action)
	{
		while (m_Running)
		{
			// Sleep running thread for interval
			std::this_thread::sleep_for(interval);

			auto it = m_Paths.begin();
			while (it != m_Paths.end())
			{
				if (!std::filesystem::exists(it->first))
				{
					action(it->first, FileStatus::deleted);
					it = m_Paths.erase(it);
				}
				else
				{
					it++;
				}
			}

			try
			{
				for (auto& file : std::filesystem::recursive_directory_iterator(watchPath))
				{
					auto current_file_last_write_time = std::filesystem::last_write_time(file);

					if (!contains(file.path().string()))
					{
						m_Paths[file.path().string()] = current_file_last_write_time;
						action(file.path().string(), FileStatus::created);
					}

					else
					{
						if (m_Paths[file.path().string()] != current_file_last_write_time)
						{
							m_Paths[file.path().string()] = current_file_last_write_time;
							action(file.path().string(), FileStatus::modified);
						}
					}
				}
			}
			catch (const std::exception&)
			{

			}
		}
	}

	void Stop()
	{
		m_Running = false;
	}

private:
	std::unordered_map<std::string, std::filesystem::file_time_type> m_Paths;
	bool m_Running = true;

	bool contains(const std::string& key)
	{
		auto el = m_Paths.find(key);
		return el != m_Paths.end();
	}
};