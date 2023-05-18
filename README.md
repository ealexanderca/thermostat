## WiFi Thermostat
The current version of this thermostat pulls the temperature from a google calendar and controls an electric heater through a relay.

### Components
- [NodeMCU](https://www.digikey.ca/en/products/detail/adafruit/2821/1528-1530-ND/5775536)
- [Relay](https://www.digikey.ca/en/products/detail/dfrobot/DFR0251/6588596)
- [DHT11](https://www.digikey.ca/en/products/detail/adafruit-industries-llc/386/5356713?s=N4IgTCBcDaICIAkAqBGFIC6BfIA)
- [e-Paper Display](https://www.waveshare.com/product/1.54inch-e-paper-module.htm)

### Calendar Setup and Use
To setup the temperature schedule all you need to do is create a google calendar with titles equal to the target temperature. If you to overide the temperature for a day you can create an all day event. to disable the temperature control set the values to -999 and it will comepletely disable the device.
<center><figure><img src=https://raw.githubusercontent.com/ealexanderca/thermostat/main/extras/GSconfig.png alt='missing'></figure></center>

### Google Script Setup
To setup the google script go to [script.google.com](https://script.google.com/) and sign in with your google account. Create a new project and copy the contents of [googleScript.gs](https://raw.githubusercontent.com/ealexanderca/thermostat/main/googleScript.gs) into it altering the calendar name to match what you chose above. Deploy the project selecting web app and ensuring that its set to run as me and anyone can use it as shown below.
<center><figure><img src=https://raw.githubusercontent.com/ealexanderca/thermostat/main/extras/Calendar.png alt='missing'></figure></center>
Once the script has been deployed copy the script ID for later.

### NodeMCU Code
To program the NodeMCU download and copy the files linked in the [Requirements.txt](https://raw.githubusercontent.com/ealexanderca/thermostat/main/Requirements.txt) into your arduino libraries file. Download and configure the [thermoWeb.ino](https://raw.githubusercontent.com/ealexanderca/thermostat/main/thermoWeb/thermoWeb.ino) script entering your WiFi information and the script ID from earlier. Once those edits have been made the NodeMCU can be flashed and is ready to be deployed.

### Pin Out
The basic pin assignment to hook up all the components is shown in the table below.

| NodeMCU | 1.54inch e-Paper Module | DHT11 | Relay  | Heater |
|---------|-------------------------|-------|--------|--------|
| D1      | BUSY                    |       |        |        |
| D2      | RST                     |       |        |        |
| D3      | DC                      |       |        |        |
| D4      | CS                      |       |        |        |
| D5      | CLK                     |       |        |        |
| D6      |                         | DATA  |        |        |
| D7      | DIN                     |       |        |        |
| D8      |                         |       | SIGNAL |        |
| 3V3     | VCC                     | VDD   | VCC    |        |
| GND     | GND                     | GND   | GND    |        |
|         |                         |       | NC     | LIVE   |
|         |                         |       | COM    | HEATER |
