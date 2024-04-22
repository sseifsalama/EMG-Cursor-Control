import serial.tools.list_ports
import pyautogui as pg
import time

x = 1000
y = 1000
ports = serial.tools.list_ports.comports()
serialInst = serial.Serial()
val = 3 # Change this value to the port number you want to connect to
serialInst.baudrate = 9600
serialInst.port = "COM3"
serialInst.open()

while True:
	if serialInst.in_waiting:
		packet = serialInst.readline()
		packet = packet.decode('utf').rstrip('\n')
		xs, ys, z = float(packet.split(',')[0]), float(packet.split(',')[1]), float(packet.split(',')[2])
		print(pg.position())
		if xs > 60:
			x -= 100
		elif xs > 30:
			x -= 50
		elif xs < -60:
			x += 100
		elif xs < -30:
			x += 50
		if ys > 60:
			y += 100
		elif ys > 30:
			y += 50
		elif ys < -60:
			y -= 100
		elif ys < -30:
			y -= 50   
		pg.moveTo(x, y, duration = 0.01)
		if z == 1:
			pg.click()

		if z == 2:
			pg.hotkey('win', 'h')

		if x > 2800:
			x -= 20
		elif x <= 0:
			x += 20
		if y > 1580:
			y -= 20
		elif y <= 0:
			y += 20