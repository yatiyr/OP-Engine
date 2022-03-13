import os, subprocess
import PyPackagesCheck

# get python modules that we need
# for installation 
PyPackagesCheck.Run()

import CheckVulkan

os.chdir('../')

if (not CheckVulkan.Check_VK_SDK()):
    print ("Vulkan SDK not installed")

print("Run premake")
subprocess.call(["ConfigureScripts/premake5.exe", "vs2022"])