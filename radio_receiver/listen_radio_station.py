
import time
import serial
import datetime


class RadioPacket:
	def __init__(self, *args):
		self.timestamp = datetime.datetime.now().strftime("%d %B %Y %H:%M:%S.%f")
		self.radio_id = args[0] if len(args) > 0 and not None else None
		self.radio_channel = args[1] if len(args) > 1 and not None else None
		self.transmitter_id = args[2] if len(args) > 2 and not None else None
		self.pX = args[3] if len(args) > 3 and not None else None
		self.pY = args[4] if len(args) > 4 and not None else None
		self.dX = args[5] if len(args) > 5 and not None else None
		self.dY = args[6] if len(args) > 6 and not None else None

	def __repr__(self):
		return {
			'timestamp': self.timestamp,
			'radio_id': self.radio_id,
			'radio_channel': self.radio_channel,
			'transmitter_id': self.transmitter_id,
			'pX': self.pX,
			'pY': self.pY,
			'dX': self.dX,
			'dY': self.dY
		}

	def __str__(self):
		return self.__repr__().__str__()


RECEIVER_PORT = '/dev/cu.usbserial-14210'  # '/dev/ttyXXXX' : definition du port d ecoute (remplacer 'X' par le bon nom)
BROADBAND = 115200  # 9600 : vitesse de communication

if __name__ == '__main__':
	serialArduino = serial.Serial(RECEIVER_PORT, BROADBAND)

	def listen():
		data = serialArduino.readline().replace(b'\n', b'')

		try:
			if b'\xff' not in data:
				tmp = data.decode('utf-8').replace('\r', '').split('|')
				radio_arguments = tmp[0].split('-')
				if tmp[1] != 'None':
					radio_arguments += tmp[1].split('-')

				radio_packet = RadioPacket(*radio_arguments)
				# todo : insert action here
				print(radio_packet)
			else:
				print('WHITE_NOISE...')
		except UnicodeDecodeError as e:
			print('WHITE_NOISE... Something went wrong.')

	while True:
		listen()
		time.sleep(1)
