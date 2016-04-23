# source this script
# It start the adb server as root.
# This apparently make the nexus defice get detected properly.
# Otherwise your device will be reported as something like: ???????????

sudo /home/raindrop/devandroid/local/android-sdk-linux/platform-tools/adb kill-server
sudo /home/raindrop/devandroid/local/android-sdk-linux/platform-tools/adb start-server
sudo /home/raindrop/devandroid/local/android-sdk-linux/platform-tools/adb devices
