import serial


def send_serial(current_address, current_channel):
    # get 'current' from listening
    listen_address = int(input('address [1-6]: \n'))
    listen_channel = int(input('address [0-125]: \n'))

    if listen_address != current_address or listen_channel != current_channel:
        while True:
            x = y = b'0'

            if current_address != listen_address:
                current_address += 1
                x = b'1'
            if current_channel != listen_channel:
                current_channel += 1
                y = b'1'

            serialArduino.write(x)
            serialArduino.write(y)

            if listen_address == current_address and listen_channel == current_channel:
                break


RECEIVER_PORT = '/dev/cu.usbserial-14210'  # '/dev/ttyXXXX' : definition du port d ecoute (remplacer 'X' par le bon nom)
BROADBAND = 115200  # 9600 : vitesse de communication

if __name__ == '__main__':
    serialArduino = serial.Serial(RECEIVER_PORT, BROADBAND)
