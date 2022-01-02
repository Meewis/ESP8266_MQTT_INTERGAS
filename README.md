# ESP8266_MQTT_INTERGAS
Intergas boiler (universal) communication gateway

This project started after I got inspired by the topic about communication to a Intergas HRE boiler in a forum at www.circuitsonline.net (https://www.circuitsonline.net/forum/view/80667). I also took a ESP8266 to commicatie to the boiler, but made the MQTT communication transparent. 

The ESP8266 is subcribed to at MQTT broker and waits for any data string. When data is received by a MQTT message this is forwarden directly and uncondional to the Serial output. Secundairy the ESP8266 is waiting for any data on its Serial input. When one byte of x bytes is received an automatic timeout mechanisme is triggered so that when no serial data is comming after 1 seconde, the whole serial message wil be converted to a ascii hex csv string and that string is finaly problisched by MQTT to the MQTT broker.

In the CircuitOnline forum the setup was to transmit a serial string of 3 chars "S?/n" as a reqest to get 32 status bytes as a reply from the Intergas boiler. With my setup of the program it is free to send any string to the Intergas boiler. This wil be handy because not yet all possebilibies of the Intergas communication are explored jet. 

[2-1-2022; a new command is discovered: REV]

Intergas supplies its own communication softwaretool and it's called IDS. Out of this softwaretool is know that the following informatie could be pulled out of the boiler with the command "S?/n":
1) 'status' 
't1',
't2',
't3',
't4',
't5',
't6',
'temp_set',
'fanspeed_set',
'fanspeed',
'fan_pwm',
'opentherm',
'roomtherm',
'tap_switch',
'gp_switch',
'pump',
'dwk',
'gasvalve',
'io_signal',
'spark',
'io_curr',
'displ_code',
'ch_pressure',
'rf_rth_bound',
'rf_rth_communication',
'rf_rth_battery_info',
'rf_rth_battery_ok'

2) 'extra status'
code:
'bc_tapflow',
'pump_pwm',
'room_override_zone1',
'room_set_zone1',
'room_temp_zone1',
'room_override_zone2',
'room_set_zone2',
'room_temp_zone2',
'outside_temp',
'OT_master_member_id',
'OT_therm_prod_version',
'OT_therm_prod_type'

3) version
'Hardware release',
'Software release',
'ROM test 1 checksum',
'ROM test 2 checksum'

4) burner control unit
'Interrupt_time',
'Interrupt_load (%)',
'Main_load (%)',
'Net fequency (Hz)',
'Voltage ref. (V)',
'Checksum1',
'Checksum2'

5) operating hours
'Line power connected (hours)',
'Line power disconnected (number of times)',
'CH function(hours)',
'DHW function(hours)',
'Burnerstarts (number of times)',
'Ignition failed (number of times)',
'Flame lost (number of times)',
'Reset (number of times)',
'Gasmeter_ch',
'Gasmeter_dhw',
'watermeter',
'burnerstarts_dhw'

6) Error codes

7) Parameters
'HEATER_ON',
'COMFORT_MODE',
'CH_SET_MAX',
'DHW_SET',
'ECO_DAYS',
'COMFORT_SET',
'DHW_AT_NIGHT',
'CH_AT_NIGHT',
'Parameter 1',
'Parameter 2',
'Parameter 3',
'Parameter 4',
'Parameter 5',
'Parameter 6',
'Parameter 7',
'Parameter 8',
'Parameter 9',
'Parameter A',
'Parameter b',
'Parameter C',
'Parameter c',
'Parameter d',
'Parameter E',
'Parameter E.',
'Parameter F',
'Parameter H',
'Parameter n',
'Parameter o',
'Parameter P',
'Parameter r',
'Parameter F.'

8) Extra parameters
'id_dongle0',
'id_dongle1',
'id_dongle2',
'id_dongle3',
'id_lan0',
'id_lan1',
'id_lan2',
'id_lan3'

10) Second info datablock
'Node nr',
'Cloud id0',
'Cloud id1',
'Cloud id2',
'Rf cloud nr',
'rssi_lower',
'rssi_upper',
'rssi_wait',
'attention_period',
'attention_number',
'ramses_thermostat_idH',
'ramses_thermostat_idM',
'ramses_thermostat_idL',
'ramses_boiler_idH',
'ramses_boiler_idM',
'ramses_boiler_idL',
'Productioncode'

So with s serial string of 3 chars "S?/n" the reply delivers data for part 1, 2 and parlty(?) 6.
Adjusting this string to "P?/n" might reply data for part 7; but that has to be tested.

I use Node-Red to generate every 3 seconds a serial string (S?/n) and publish that to the MQTT broker. The ESP8266 will pick up that message and send thee 3 bytes to the boiler. The boiler replies (in this case) with 32 bytes. After convertion by the ESP8266, this array of data is published to the MQTT broker. Node-Red will pick up this array of data. Because its already formatted in a string as hex values, this is easely processed in Node-Red to buffer from where the data can selected by indexing.
