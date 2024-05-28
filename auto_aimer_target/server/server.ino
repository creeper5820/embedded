#include <Arduino.h>
#include <Ticker.h>
#include <WebServer.h>
#include <WiFi.h>

namespace global {
// constexpr char* ssid = "Alliance_SentryDebug";
// constexpr char* password = "sentryno1";

// constexpr char* ssid = "AllianceTeam2.4G";
// constexpr char* password = "rm-alliance.icu";
constexpr char* ssid = "auto-aimer-target";
constexpr char* password = "rm-alliance.icu";

constexpr char* host_name = "esp32_auto_aimer_target";

constexpr auto rx_pin = 16;
constexpr auto tx_pin = 17;
constexpr auto led_pin = 22;

int move = 0;
int rotate = 0;

WebServer server(80);
String html = "<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0,maximum-scale=1.0,minimum-scale=1.0,user-scalable=no\"> <title>AutoAimer</title> </head> <body> <div class=\"main\"> <p class=\"title\">你的新一代自瞄移动靶</p> <div class=\"card\"> <form class=\"slider_item\"> <div class=\"label\"> <label>MOVE</label> </div> <input type=\"range\" name=\"move_range\" min=\"0\" max=\"100\" value=\"0\" class=\"slider\" id=\"move\" oninput=\"this.form.move_number.value=this.value\" onchange=\"sync_data('move', this.value)\"> <input type=\"text\" name=\"move_number\" pattern=\"[0-9]*\" size=\"3\" value=\"0\" class=\"number\" onchange=\"this.form.move_range.value=this.value\"> </form> <form class=\"slider_item\"> <div class=\"label\"> <label>ROTATE</label> </div> <input type=\"range\" name=\"rotate_range\" min=\"0\" max=\"100\" value=\"0\" class=\"slider\" id=\"rotate\" oninput=\"this.form.rotate_number.value=this.value\" onchange=\"sync_data('rotate', this.value)\"> <input type=\"text\" name=\"rotate_number\" pattern=\"[0-9]*\" size=\"3\" value=\"0\" class=\"number\" onchange=\"this.form.rotate_range.value=this.value\"> </form> </div> </div> </body> <style> .main { font-family: Verdana, Geneva, Tahoma, sans-serif; position: fixed; top: 30%; left: 50%; -webkit-transform: translate(-50%, -50%); transform: translate(-50%, -50%); align-items: center; text-align: center; } .title { text-align: center; color: #9e4747; font-size: 200%; font-family: monospace; font-weight: bold; } .card { background-color: rgb(255, 255, 255); box-shadow: 0px 0px 25px -8px #b88b8b; outline: solid 5px rgb(240, 238, 238); display: flex; flex-direction: column; justify-content: center; align-items: center; width: 300px; height: auto; padding-top: 10px; padding-bottom: 10px; padding-left: 20px; padding-right: 20px; border-radius: 30px; } .slider_item { display: flex; align-items: center; justify-content: left; padding: 20px; } .label { text-align: center; color: #ffffff; font-weight: 1000; background-color: #9e4747; border-radius: 10px; padding-top: 10px; padding-bottom: 10px; padding-left: 8px; padding-right: 8px; margin: 5px; width: 70px; } .slider { -webkit-appearance: none; appearance: none; width: 150px; height: 25px; background: #e6d9d9; outline: none; opacity: 0.7; -webkit-transition: .2s; transition: opacity .2s; } .slider::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 30px; height: 30px; background: #9e4747; cursor: pointer; border-radius: 15%; } .number { color: #ffffff; width: 35px; margin: 5px; padding: 5px; text-align: center; font-weight: bolder; font-size: large; border: none; background-color: #9e4747; border-radius: 15%; } </style> <script> function sync_data(name, value) { var sync = new XMLHttpRequest(); sync.open(\"GET\", \"/slider?name=\" + name + \"&value=\" + value, true); sync.send(); } </script> </html>";

Ticker ticker;

void index_handler()
{
    server.send(200, "text/html", html);
}

void sync_handler()
{
    String name = server.arg("name");
    String value = server.arg("value");

    if (name.equals("move")) {
        move = value.toInt();
    } else if (name.equals("rotate")) {
        rotate = value.toInt();
    } else {
        server.send(501, "text", "Bad get");
    }

    server.send(200, "text", "Sync done");

    Serial.println("Sync arrived: " + name + "=" + value);

    digitalWrite(led_pin, LOW);
    delay(100);
    digitalWrite(led_pin, HIGH);
}

void sync_with_stm32()
{
    uint8_t send[4] = { 233, move, rotate, 233 };
    Serial2.write(send, 4);
}
}

void setup()
{
    pinMode(global::led_pin, OUTPUT);
    digitalWrite(global::led_pin, HIGH);

    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, global::rx_pin, global::tx_pin);

    // Connect to wifi as client
    //
    // WiFi.setHostname(global::host_name);
    // WiFi.begin(global::ssid, global::password);
    //
    // while (WiFi.status() != WL_CONNECTED) {
    //     digitalWrite(global::led_pin, HIGH);
    //     delay(400);
    //     digitalWrite(global::led_pin, LOW);
    //     delay(100);

    //     Serial.println(WiFi.status());
    //     Serial.println("Connecting to WiFi...");
    // }
    // Serial.println(WiFi.status());
    // Serial.println("Connected.");
    // Serial.println(WiFi.localIP().toString());

    // Work as wifi server
    //
    WiFi.softAPConfig(
        IPAddress(192, 168, 233, 233),
        IPAddress(192, 168, 233, 0),
        IPAddress(255, 255, 255, 0));
    WiFi.softAP(global::ssid, global::password);

    Serial.println(WiFi.softAPIP());

    digitalWrite(global::led_pin, LOW);

    global::server.on("/", global::index_handler);
    global::server.on("/slider", global::sync_handler);
    global::server.begin();

    global::ticker.attach(0.01, global::sync_with_stm32);
}

void loop()
{
    global::server.handleClient();
    delay(2);
}
