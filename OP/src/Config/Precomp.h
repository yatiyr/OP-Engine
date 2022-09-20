#pragma once

#include <random>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <utility>
#include <functional>


#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>

#include <filesystem>
#include <fstream>

#include <Op/Logging.h>

#include <Profiling/Instrumentor.h>

#ifdef OP_PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include <csignal>
#endif