
Developed using ESP-IDF v5.3.4, this project implements a Wi-Fi control system on the ESP32-S3. The device establishes a network connection and acts as an HTTP Client to send motion control commands to a target host.

Key Features
Command Dispatch: Supports sending control commands including ok, left, right, and stop.

Communication Protocol: Utilizes HTTP GET requests for lightweight and reliable control.

Status Monitoring: Real-time monitoring of HTTP status codes to ensure command delivery.

本项目基于 ESP-IDF v5.3.4 开发，实现了一个基于 ESP32-S3 的 Wi-Fi 控制系统。设备通过 SoftAP 模式或 Station 模式建立网络连接，并作为 HTTP 客户端 向目标设备发送动作控制指令。

核心功能
指令下发：支持发送 ok / left / right / stop 等控制命令。

通信协议：采用 HTTP GET 请求协议。

状态监控：实时监控服务器返回的 HTTP 状态码，确保指令准确执行。
