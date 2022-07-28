# Project: IPK - HTTP resolver doménových jmen
# Author: Michal Cibák - xcibak00

import sys
import socket

# ------------------- #
# PORT argument check #
# ------------------- #

if len(sys.argv) < 2:
    sys.exit("PORT argument missing!")
elif len(sys.argv) > 2:
    sys.exit("Too many arguments!")
else:
    try:
        the_PORT = int(sys.argv[1])
    except ValueError:
        sys.exit("PORT argument is not a number!")

    if the_PORT not in range(1, 65536):
        sys.exit("Invalid PORT number!")

# --------- #
# MAIN code #
# --------- #

def is_valid_IP(str):
    if str.count('.') == 3:
        try:
            socket.inet_aton(str)
            return True
        except:
            return False
    else:
        return False

# // creation
the_SOCKET = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# // binding
the_SOCKET.bind(('127.0.0.1', the_PORT))
# // listening
the_SOCKET.listen()
print('It lives! :)')

while True:
    # // connecting
    the_CONNECTION, the_ADDRESS = the_SOCKET.accept()
    print('Connected to:', the_ADDRESS)
    # // reading and sending
    while True:
        # // receive message
        the_MESSAGE = the_CONNECTION.recv(2048)
        if not the_MESSAGE:
            print('No data received.')
            break
        print('Message received.')
        # print('Raw received data:')
        # print(the_MESSAGE)
        the_MESSAGE = the_MESSAGE.decode()
        #print('Decoded received data:\n' + the_MESSAGE)

        # // message processing
        the_MESSAGE = the_MESSAGE.splitlines() # // splits the header into lines
        the_REQUEST = the_MESSAGE[0].split()
        the_ANSWER = ''
        if the_REQUEST[0] == 'GET':
            the_RESPONSE = 'HTTP/1.1 200 OK\r\n'
            if len(the_REQUEST) == 3: # // GET, url, version
                if the_REQUEST[1].startswith('/resolve?'): # // proper start of URL
                    the_GET_PARAMETERS = the_REQUEST[1][9:len(the_REQUEST[1])].split('&')
                    # print(the_GET_PARAMETERS)
                    if len(the_GET_PARAMETERS) == 2:
                        if the_GET_PARAMETERS[0].startswith('name=') and the_GET_PARAMETERS[1].startswith('type='):
                            the_NAME = the_GET_PARAMETERS[0][5:len(the_GET_PARAMETERS[0])]
                            the_TYPE = the_GET_PARAMETERS[1][5:len(the_GET_PARAMETERS[1])]
                            if the_TYPE == 'A': # // it wants to find IP
                                if not is_valid_IP(the_NAME): # // from something that is not an IP, thus it can be considered a hostname
                                    try:
                                        the_IP_ADDRESS = socket.gethostbyname(the_NAME)
                                        the_ANSWER = the_NAME + ':' + the_TYPE + '=' + the_IP_ADDRESS + '\r\n'
                                    except:
                                        the_RESPONSE = 'HTTP/1.1 404 Not Found\r\n'
                                else: # // from an IP
                                    the_RESPONSE = 'HTTP/1.1 400 Bad Request\r\n'
                            elif the_TYPE == 'PTR': # // it wants to find hostname
                                if is_valid_IP(the_NAME): # // from an IP
                                    try:
                                        the_HOSTNAME = socket.gethostbyaddr(the_NAME)
                                        the_ANSWER = the_NAME + ':' + the_TYPE + '=' + the_HOSTNAME[0] + '\r\n'
                                    except:
                                        the_RESPONSE = 'HTTP/1.1 404 Not Found\r\n'
                                else: # // from something that is not an IP
                                    the_RESPONSE = 'HTTP/1.1 400 Bad Request\r\n'
                            else:
                                the_RESPONSE = 'HTTP/1.1 400 Bad Request\r\n'
                        else:
                            the_RESPONSE = 'HTTP/1.1 400 Bad Request\r\n'
                    else:
                        the_RESPONSE = 'HTTP/1.1 400 Bad Request\r\n'
                else:
                    the_RESPONSE = 'HTTP/1.1 400 Bad Request\r\n'
            else: # // there is something missing or something extra (how though? that shouldn't happen)
                the_RESPONSE = 'HTTP/1.1 500 Internal Server Error\r\n'
        elif the_REQUEST[0] == 'POST':
            the_RESPONSE = 'HTTP/1.1 404 Not Found\r\n'
            if len(the_REQUEST) == 3: # // POST, url, version
                if the_REQUEST[1] == '/dns-query': # // proper URL
                    the_BODY_INDEX = 0
                    for the_BODY_INDEX in range(0, len(the_MESSAGE)):
                        if len(the_MESSAGE[the_BODY_INDEX]) == 0:
                            the_BODY_INDEX += 1
                            break
                    if the_BODY_INDEX < len(the_MESSAGE):
                        the_BAD_REQUEST = False
                        for the_REQUEST_INDEX in range(the_BODY_INDEX, len(the_MESSAGE)):
                            the_POST_PARAMETERS = the_MESSAGE[the_REQUEST_INDEX].split(':')
                            # print(the_POST_PARAMETERS)
                            if len(the_POST_PARAMETERS) == 2:
                                the_NAME = the_POST_PARAMETERS[0]
                                the_TYPE = the_POST_PARAMETERS[1]
                                if the_TYPE == 'A': # // it wants to find IP
                                    if not is_valid_IP(the_NAME): # // from something that is not an IP, thus it can be considered a hostname
                                        try:
                                            the_IP_ADDRESS = socket.gethostbyname(the_NAME)
                                            the_RESPONSE = 'HTTP/1.1 200 OK\r\n'
                                            the_ANSWER += the_NAME + ':' + the_TYPE + '=' + the_IP_ADDRESS + '\r\n'
                                        except:
                                            pass
                                    else: # // from an IP
                                        the_BAD_REQUEST = True
                                elif the_TYPE == 'PTR': # // it wants to find hostname
                                    if is_valid_IP(the_NAME): # // from an IP
                                        try:
                                            the_HOSTNAME = socket.gethostbyaddr(the_NAME)
                                            the_RESPONSE = 'HTTP/1.1 200 OK\r\n'
                                            the_ANSWER += the_NAME + ':' + the_TYPE + '=' + the_HOSTNAME[0] + '\r\n'
                                        except:
                                            pass
                                    else: # // from something that is not an IP
                                        the_BAD_REQUEST = True
                                else:
                                    the_BAD_REQUEST = True
                            else: # // more than 2 parameters or 1 parameter or empty line
                                the_BAD_REQUEST = True
                        if the_BAD_REQUEST == True and the_RESPONSE != 'HTTP/1.1 200 OK\r\n':
                            the_RESPONSE = 'HTTP/1.1 400 Bad Request\r\n'
                    else:
                        the_RESPONSE = 'HTTP/1.1 400 Bad Request\r\n'
                else:
                    the_RESPONSE = 'HTTP/1.1 400 Bad Request\r\n'
            else: # // there is something missing or something extra (how though? that shouldn't happen)
                the_RESPONSE = 'HTTP/1.1 500 Internal Server Error\r\n'
        else: # // unknown method
            the_RESPONSE = 'HTTP/1.1 405 Method Not Allowed\r\n'

        # // send message
        the_RESPONSE += 'Connection: close\r\n\r\n'
        the_RESPONSE += the_ANSWER
        the_RESPONSE = the_RESPONSE.encode()
        the_CONNECTION.sendall(the_RESPONSE)
        print('Response sent.')
        break
    # // closing connection
    the_CONNECTION.close()
    print('Connection closed.')

the_SOCKET.close()
print('It livesn\'t. :(')
