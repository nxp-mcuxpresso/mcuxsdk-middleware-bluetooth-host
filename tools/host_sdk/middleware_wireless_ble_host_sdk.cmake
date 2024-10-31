# Add set(CONFIG_USE_middleware_wireless_ble_host_sdk true) in config.cmake to use this component

include_guard(GLOBAL)
message("${CMAKE_CURRENT_LIST_FILE} component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/demo/FsciBootloader.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/demo/GetKinetisDevices.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/physical/PhysicalDevice.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/protocol/Framer.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/sys/EventManager.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/sys/MessageQueue.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/sys/RawFrame.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/sys/hsdkEvent.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/sys/hsdkFile.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/sys/hsdkLock.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/sys/hsdkLogger.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/sys/hsdkSemaphore.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/sys/hsdkThread.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/sys/utils.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/physical/PCAP/PCAPDevice.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/physical/SPI/SPIConfiguration.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/physical/SPI/SPIDevice.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/physical/UART/UARTConfiguration.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/physical/UART/UARTDevice.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/physical/UART/UARTDiscovery.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/protocol/FSCI/FSCIFrame.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk/protocol/FSCI/FSCIFramer.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk-c/demo/HeartRateSensor.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk-c/src/cmd_ble.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk-c/src/evt_ble.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk-c/src/evt_printer_ble.c
  ${CMAKE_CURRENT_LIST_DIR}/hsdk-c/src/unload_ble.c
)

