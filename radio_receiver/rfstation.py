
import time
import serial


if __name__ == '__main__':
	default_address = 1
	default_channel = 100

	# '/dev/ttyXXXX' : definition du port d ecoute (remplacer 'X' par le bon nom)
	# 9600 : vitesse de communication
	serialArduino = serial.Serial('/dev/cu.usbserial-1410', 115200)

	def listen():
		data = serialArduino.readline().replace(b'\n', b'')
		if b'\xff' not in data:
			print(data.decode('utf-8'))
		else:
			print('noise..')

	listen_address = int(input('address [1-6]: \n'))
	listen_channel = int(input('address [0-125]: \n'))

	if listen_address != default_address or listen_channel != default_channel:
		while True:
			x = y = b'0'

			if default_address != listen_address:
				default_address += 1
				x = b'1'
			if default_channel != listen_channel:
				default_channel += 1
				y = b'1'

			serialArduino.write(x)
			serialArduino.write(y)
			listen()

			if listen_address == default_address and listen_channel == default_channel:
				break

	while True:
		listen()
		time.sleep(1)
