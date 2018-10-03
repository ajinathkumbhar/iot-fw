# MQTT client-server test code

MQTT stands for MQ Telemetry Transport. It is a publish/subscribe, extremely simple and 
lightweight messaging protocol, designed for constrained devices and low-bandwidth, 
high-latency or unreliable networks. The design principles are to minimise network bandwidth 
and device resource requirements whilst also attempting to ensure reliability and 
some degree of assurance of delivery. These principles also turn out to make the protocol ideal 
of the emerging “machine-to-machine” (M2M) or “Internet of Things” world of connected devices, 
and for mobile applications where bandwidth and battery power are at a premium.

                               +--------+
                               |MQtt    |
                               |client  |
                               |        |
                               +---+----+
                                   | Publish
                                   v
    +-------+    Publish     +--------------+                +-------+
    |MQtt   |                |              | Subscribe      |MQtt   |
    |client +--------------> |              | +----------->  |client |
    +-------+                |  MQtt Broker |                +-------+
                             |    on cloud  |
                             |              |
    +-------+    Publish     |              |                +-------+
    |MQtt   +--------------> |              | Subscribe      |MQtt   |
    |client |                |              |  +-----------> |client |
    +-------+                +--------------+                +-------+
                               | Subscribe | Subscribe
                               v           v
                          +-------+       +--------+
                          |MQtt   |       |MQtt    |
                          |client |       |client  |
                          +-------+       +-------
# Prerequisites:
1. MQTT broker on cloud ( Use Mosquitto Broker )
2. MQTT clients on end devices ( Use python paho-mqtt or MQTT.js 

# Install MQTT broker (Mosquitto)
Eclipse Mosquitto is an open source (EPL/EDL licensed) message broker that implements 
the MQTT protocol versions 3.1 and 3.1.1. Mosquitto is lightweight and is suitable for 
use on all devices from low power single board computers to full servers.
```sh
$ sudo apt-get update
$ sudo apt-get install mosquitto mosquitto-clients
```

Test Mosquitto:
Open termial and subscribe for topic 
```sh
$ mosquitto_sub -t "test"
```
Open another terminal and publish message on subscribed topic 
```sh
$ mosquitto_pub -m "Message from client XYZ" -t "test"
```
Secure with a Password

[mosquitto_passwd install](https://mosquitto.org/blog/2013/01/mosquitto-debian-repository/)

```sh
$ sudo mosquitto_passwd -c /etc/mosquitto/passwd dave
Password: password
```

Create a configuration file for Mosquitto pointing to the password file we have just created.
```sh
$ sudo nano /etc/mosquitto/conf.d/default.conf
```
This will open an empty file. Paste the following into it.

```sh
allow_anonymous false
password_file /etc/mosquitto/passwd
```
Save and exit the text editor with "Ctrl+O", "Enter" and "Ctrl+X". Now restart Mosquitto server and test our changes.
```sh
$ sudo service mosquitto restart
```
In the subscribe client window, press "Ctrl+C" to exit the subscribe client and restart it with following command.
```sh
$ mosquitto_sub -t "test" -u "dave" -P "password"
```
Now publish a message with the username and password.
```sh
$ mosquitto_pub -t "test" -m "message from mosquitto_pub client" -u "dave" -P "password"
```
# Install python module paho-mqtt 
```sh
$ pip install paho-mqtt --user
```

### These are reference links used
1. [MQTT](https://mqtt.org/)
2. [paho-mqtt](https://pypi.org/project/paho-mqtt/)
3. [Mosquitto](https://www.vultr.com/docs/how-to-install-mosquitto-mqtt-broker-server-on-ubuntu-16-04)

