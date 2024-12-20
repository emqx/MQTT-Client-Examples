import network
import time

def connect():
	ssid = 'NAME OF YOUR WIFI NETWORK'
	password = 'PASSWORD OF YOUR WIFI NETWORK'
	wlan = network.WLAN(network.STA_IF)
	wlan.active(True)
	wlan.connect(ssid, password)
	while wlan.isconnected() == False:
		print('Waiting for connection...')
		time.sleep(1)
	print('Connected on {ip}'.format(ip = wlan.ifconfig()[0]))
