#  Telegram Bot for IoT Device Automation

Control your ESP32 (or other IoT devices) directly from Telegram using MQTT and Pipedream.  
This project connects your bot, MQTT broker, and ESP32 to create a simple remote automation system.

---

##  Architecture

**Components:**

- **Telegram Bot** → Receives commands from the user  
- **Pipedream Workflow** → Processes commands and publishes MQTT messages  
- **HiveMQ MQTT Broker** → Cloud broker to pass messages between bot and ESP32  
- **ESP32** → Subscribes to MQTT and controls connected devices (LEDs, relays, sensors, etc.)

---

##  Setup Guide

### Prerequisites
- Telegram account with a bot created via BotFather  
- HiveMQ Cloud account (for MQTT broker)  
- Pipedream account (for workflow automation)  
- ESP32 development board  
- Arduino IDE with WiFi + PubSubClient libraries installed  

---

### 1. Create a Telegram Bot
1. Open Telegram and search for **BotFather**  
2. Use `/newbot` to create a bot and get the bot token  
3. Save this token safely, you'll use it in Pipedream  

---

### 2. HiveMQ Cloud Setup
1. Sign up at [HiveMQ Cloud](https://www.hivemq.com/mqtt-cloud-broker/)  
2. Create a free cluster and note:
   - Host  
   - Username  
   - Password  

---

### 3. Pipedream Workflow
1. Create a new workflow with **HTTP/Webhook trigger**  
2. Set Telegram webhook to forward messages:
```
https://api.telegram.org/bot<YOUR_BOT_TOKEN>/setWebhook?url=<PIPEDREAM_WEBHOOK_URL>
```

3. Add a **Node.js step** named `process_command` with this code:

```javascript
import mqtt from 'mqtt';

export default defineComponent({
  async run({ steps, $ }) {
    const incomingText = steps.trigger.event.body.message.text;
    const chatId = steps.trigger.event.body.message.chat.id;
    const command = incomingText.toLowerCase();

    let response = {
      chatId: chatId,
      responseMessage: null,
      mqttMessage: null
    };

    if (command === "/start") {
      response.responseMessage = "🤖 Welcome to your ESP32 Controller Bot!\n\nUse commands:\n/led on - Turn LED ON\n/led off - Turn LED OFF\n/help - Show help";
    } else if (command === "/help") {
      response.responseMessage = "💡 Help Menu\n\nCommands:\n/led on - Turn LED ON\n/led off - Turn LED OFF\n/help - Show this message";
    } else if (command === "/led on") {
      response.responseMessage = "✅ Turning LED ON.";
      response.mqttMessage = "ON";
    } else if (command === "/led off") {
      response.responseMessage = "✅ Turning LED OFF.";
      response.mqttMessage = "OFF";
    } else if (command === "/led") {
      response.responseMessage = "Please specify on or off. Example: /led on";
    } else {
      response.responseMessage = "❌ Unknown command. Type /help for options.";
    }

    // Send MQTT message if needed
    if (response.mqttMessage) {
      const config = {
        protocol: 'mqtts',
        host: 'YOUR_HIVEMQ_HOST', // Replace with your HiveMQ host
        port: 8883,
        username: 'YOUR_HIVEMQ_USERNAME', // Replace with your HiveMQ username
        password: 'YOUR_HIVEMQ_PASSWORD', // Replace with your HiveMQ password
        rejectUnauthorized: false
      };

      try {
        const client = mqtt.connect(config);
        await new Promise((resolve, reject) => {
          client.on('connect', () => {
            client.publish('myhome/led', response.mqttMessage, { qos: 1 }, (err) => {
              client.end();
              if (err) reject(err);
              else resolve();
            });
          });
          client.on('error', reject);
        });
        console.log('MQTT message sent successfully');
      } catch (error) {
        console.error('MQTT error:', error);
        response.responseMessage = "❌ Error connecting to device. Please try again.";
      }
    }

    return response;
  },
});
```
Add another Node.js step named send_telegram_reply with this code:

```
import { axios } from "@pipedream/platform"

export default defineComponent({
  async run({ steps, $ }) {
    const commandResult = steps.process_command.$return_value;

    if (!commandResult || !commandResult.responseMessage) {
      return { status: "skipped", reason: "No response message" };
    }

    const token = "YOUR_BOT_TOKEN"; // REPLACE WITH YOUR TOKEN
    const chatId = commandResult.chatId;
    const textToSend = commandResult.responseMessage;

    const url = `https://api.telegram.org/bot${token}/sendMessage`;

    return await axios($, {
      method: "POST",
      url: url,
      data: {
        chat_id: chatId,
        text: textToSend,
        parse_mode: "Markdown"
      },
    })
  },
});
```
Usage
Commands to send your bot:

/start → Welcome message

/help → Command list

/led on → Turn LED ON

/led off → Turn LED OFF

 Extend the Project
Add more LEDs or relays

Control AC appliances

Read sensors (temperature, humidity)

Create full smart home automation

 Troubleshooting
Check Pipedream workflow logs for errors

Verify HiveMQ credentials

Ensure Telegram webhook is set correctly

Confirm MQTT topic matches between Pipedream and ESP32

 License
MIT License – free for personal and commercial use.
