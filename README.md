# 🤖 Telegram Bot for IoT Device Automation

Control your ESP32 (or other IoT devices) directly from **Telegram** using **MQTT** and **Pipedream**.  
This project connects your bot, MQTT broker, and ESP32 to create a simple remote automation system.

---

## 🏗️ Architecture

**Components:**
- **Telegram Bot** → Receives commands from the user  
- **Pipedream Workflow** → Processes commands and publishes MQTT messages  
- **HiveMQ MQTT Broker** → Cloud broker to pass messages between bot and ESP32  
- **ESP32** → Subscribes to MQTT and controls connected devices (LEDs, relays, sensors, etc.)  

📌 Suggested diagram location: `docs/architecture.png` (draw with [draw.io](https://draw.io))

---

## ⚡ Setup Guide

### Prerequisites
- Telegram account with a bot created via **BotFather**  
- **HiveMQ Cloud** account (for MQTT broker)  
- **Pipedream** account (for workflow automation)  
- ESP32 development board  
- **Arduino IDE** with WiFi + PubSubClient libraries installed  

---

### 1. Create a Telegram Bot
1. Open Telegram and search for **BotFather**.  
2. Use `/newbot` to create a bot and get the **bot token**.  
3. Save this token safely, you’ll use it in Pipedream.

---

### 2. HiveMQ Cloud Setup
1. Sign up at [HiveMQ Cloud](https://www.hivemq.com/mqtt-cloud-broker/).  
2. Create a free cluster and note:
   - **Host**
   - **Username**
   - **Password**

---

### 3. Pipedream Workflow
1. Create a new workflow with **HTTP/Webhook trigger**.  
2. Set Telegram webhook to forward messages:  

https://api.telegram.org/bot<YOUR_BOT_TOKEN>/setWebhook?url=<PIPEDREAM_WEBHOOK_URL>

bash
Copy code

3. Add these steps:

#### Step 1: Parse Telegram Command
```javascript
export default defineComponent({
async run({ steps, $ }) {
 const incomingText = steps.trigger.event.body.message.text;
 const chatId = steps.trigger.event.body.message.chat.id;
 const command = incomingText.toLowerCase();

 let response = {
   chatId,
   responseMessage: null,
   mqttMessage: null
 };

 if (command === "/start") {
   response.responseMessage = `🤖 Welcome!\n\nCommands:\n• /led on\n• /led off\n• /help`;
 } else if (command === "/help") {
   response.responseMessage = `💡 Commands:\n/led on\n/led off\n/help`;
 } else if (command === "/led on") {
   response.responseMessage = "✅ Turning LED ON.";
   response.mqttMessage = "ON";
 } else if (command === "/led off") {
   response.responseMessage = "✅ Turning LED OFF.";
   response.mqttMessage = "OFF";
 } else {
   response.responseMessage = "❌ Unknown command. Try /help.";
 }

 return response;
},
});
Step 2: Publish MQTT
Condition:
steps.parse_telegram_command.$return_value.mqttMessage != null

Topic: myhome/led

Message:
{{steps.parse_telegram_command.$return_value.mqttMessage}}

Step 3: Send Telegram Reply
javascript
Copy code
import { axios } from "@pipedream/platform"

export default defineComponent({
  async run({ steps, $ }) {
    const res = steps.parse_telegram_command.$return_value;
    if (!res || !res.responseMessage) return;

    const token = "YOUR_BOT_TOKEN";
    const url = `https://api.telegram.org/bot${token}/sendMessage`;

    return await axios($, {
      method: "POST",
      url,
      data: {
        chat_id: res.chatId,
        text: res.responseMessage,
        parse_mode: "Markdown"
      },
    });
  },
});
4. ESP32 Setup
Open Arduino IDE.

Install libraries:

WiFi.h

PubSubClient.h

Upload the sketch below (update WiFi + MQTT credentials).

cpp
Copy code
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
const char* mqtt_server = "YOUR_HIVEMQ_HOST";
const char* mqtt_user = "YOUR_HIVEMQ_USERNAME";
const char* mqtt_pass = "YOUR_HIVEMQ_PASSWORD";

WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin = 2;

void callback(char* topic, byte* message, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }
  if (msg == "ON") digitalWrite(ledPin, HIGH);
  else if (msg == "OFF") digitalWrite(ledPin, LOW);
}

void setup() {
  pinMode(ledPin, OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while (!client.connected()) {
    client.connect("ESP32Client", mqtt_user, mqtt_pass);
  }
  client.subscribe("myhome/led");
}

void loop() {
  client.loop();
}
🚀 Usage
Commands to send your bot:

/start → Welcome message

/help → Command list

/led on → Turn LED ON

/led off → Turn LED OFF

🔧 Extend the Project
Add more LEDs or relays

Control AC appliances

Read sensors (temperature, humidity)

Create full smart home automation

🐛 Troubleshooting
Check ESP32 serial monitor for WiFi/MQTT connection errors

Verify HiveMQ credentials

Ensure Telegram webhook is set correctly

📜 License
MIT License – free for personal and commercial use.
