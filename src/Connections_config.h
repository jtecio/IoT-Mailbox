// Wifi settings
const char* ssid = "Preach-IoT";
const char* password = "krewkrew";

// Set your Static IP address
IPAddress local_IP(10, 10, 50, 42);
// Set your Gateway IP address
IPAddress gateway(10, 10, 50, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(10, 10, 50, 1);

// Device name

const char* devicename = "Mailbox";

// mqtt settings
const char* mqtt_server = "10.10.10.50";
const char* mqtt_username = "IoT";
const char* mqtt_password = "krewkrew";
const char* mqtt_command_topic = "iot/mailbox/command";
const char* mqtt_debug_topic = "iot/mailbox/debug";
String mqtt_status_topic = "iot/mailbox/status";
const char* mqtt_location_topic = "iot/mailbox/location";
const char* mqtt_preferences_topic = "iot/mailbox/preferences";
const char* mqtt_prefstatus_topic = "iot/mailbox/prefstatus";
const char* mqtt_lwt_topic = "iot/mailbox/lwt";
const char* mqtt_boot_topic = "iot/mailbox/boot";


// Define the debug serial port
#define DEBUG_PORT Serial