# Telegram IoT Device Automation

Control IoT devices through Telegram using **MQTT** and **Pipedream**.  
This project enables remote control of ESP32-based devices from anywhere in the world using simple Telegram commands.

---

## 🏗️ Architecture Overview

**Flow:**  
User → Telegram Bot → Pipedream Workflow → HiveMQ Broker → ESP32 Device → Physical Output  

1. User sends command to Telegram bot (e.g., `/led on`)  
2. Telegram forwards the message to Pipedream via webhook  
3. Pipedream processes the command and publishes to HiveMQ  
4. HiveMQ broker distributes the message to subscribed devices  
5. ESP32 receives the MQTT message and controls connected devices  
6. Confirmation is sent back to the user via Telegram  

---

## ⚡ Quick Start

### 1. Create a Telegram Bot
- Open Telegram and search for **@BotFather**  
- Send `/newbot` command and follow instructions  
- Save the **API token** provided  
- Note your bot's **username**  

### 2. Set Up HiveMQ Cloud Broker
- Sign up at [HiveMQ Cloud](https://www.hivemq.com/mqtt-cloud-broker/)  
- Create a new cluster (free plan)  
- Note your **cluster credentials**  

### 3. Configure Pipedream Workflow
- Sign up at [Pipedream](https://pipedream.com/)  
- Create a new workflow with **HTTP/Webhook trigger**  
- Set up the workflow steps as detailed in `/docs/pipedream_setup.md`  

### 4. Flash ESP32 Device
- Use the code from `/src/esp32/telegram_iot_control.ino`  
- Replace placeholder values with your **credentials**  
- Upload to your ESP32 device  

---

## 🚀 Usage

Send these commands to your Telegram bot:  
- `/start` → Show welcome message  
- `/help` → Show help information  
- `/led on` → Turn on the LED  
- `/led off` → Turn off the LED  

---

## 🔧 Extending the Project

This architecture can be extended to control:  
- Multiple LEDs or lights  
- Relays for appliances  
- Sensors (temperature, humidity)  
- Motors and actuators  
- Home automation systems  

---

## 📋 Prerequisites

- Telegram account  
- ESP32 development board  
- HiveMQ Cloud account (free tier)  
- Pipedream account (free tier)  
- Arduino IDE  

---

## 📝 Documentation

Detailed setup guides are available in the `/docs/` folder:  
- `telegram_setup.md` → Creating and configuring your Telegram bot  
- `hivemq_setup.md` → Setting up MQTT broker  
- `pipedream_setup.md` → Configuring the automation workflow  
- `esp32_setup.md` → Flashing and configuring the ESP32  

---

## 🐛 Troubleshooting

Common issues and solutions:  
- **ESP32 won't connect to WiFi** → Check credentials and network  
- **MQTT connection fails** → Verify broker credentials  
- **Telegram not responding** → Check webhook configuration  

---

## 📜 License

MIT License – feel free to use this project for personal or commercial purposes.
