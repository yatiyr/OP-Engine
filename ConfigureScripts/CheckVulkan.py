import os
import subprocess
import sys
from pathlib import Path

from Download import FileDownload, YesNoQuestion

from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile

# Get vulkan sdk from environment variables
# if it exists we don't need to install vulkan
VK_SDK = os.environ.get('VULKAN_SDK') 


VK_SDK_INSTALLER_URL = 'https://sdk.lunarg.com/sdk/download/1.3.204.1/windows/vulkan_sdk.exe'
OP_VULKAN_VERSION    = '1.3.204.1'
VK_SDK_EXE_PATH      = 'Opium/external/VulkanSDK/VulkanSDK.exe'

def Install_VK_SDK():
    print('Downloading {} to {}'.format(VK_SDK_INSTALLER_URL, VK_SDK_EXE_PATH))
    FileDownload(VK_SDK_INSTALLER_URL, VK_SDK_EXE_PATH)
    print("Finished!")
    print("Now Vulkan SDK installer will start...")
    os.startfile(os.path.abspath(VK_SDK_EXE_PATH))
    print("Re-run this script after installation")

def Install_VK_Prompt():
    print("Would you like to install the Vulkan SDK")
    install = YesNoQuestion()
    if (install):
        Install_VK_SDK()
        quit()


def Check_VK_SDK():
    if (VK_SDK is None):
        print("Could not find a Vulkan SDK in environment variables!")
        Install_VK_Prompt()
        return False
    elif (OP_VULKAN_VERSION not in VK_SDK):
        print(f"Located Vulkan SDK at {VK_SDK}")
        print(f"Version of Vulkan SDK does not match! (Opium requires {OP_VULKAN_VERSION})")
        Install_VK_Prompt()
        return False

    print(f"Correct Vulkan SDK is located at {VK_SDK}")
    return True