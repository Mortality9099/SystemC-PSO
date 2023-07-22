import socket

best_value: int
best_changed_glob: bool
moves_since_best_changed = 0

# class Node:
#     def __init__(self, index, x, y):
#         self.index = index
#         self.x = x
#         self.y = y


# class Position:
#     def __init__(self):
#         self.nodes = []



server_socket = socket.socket()

host = socket.gethostname()
print(host)

port = 12344
server_socket.bind((host, port))

# Become a server socket and listen for connections
server_socket.listen(1)

print("Waiting for connection ...")
client_socket, addr = server_socket.accept()
print(f"Got a connection from {addr}")
ok = client_socket.recv(1024).decode()
if ok != 'ok':
    exit()

print("Message received!")

data = client_socket.recv(1024).decode()
best_value, moves_since_best_changed, best_changed_glob = data.split(', ') # best_value, moves_since_best_changed, best_changed_glob
best_value = float(best_value)
moves_since_best_changed = int(moves_since_best_changed)
best_changed_glob = best_changed_glob == 'true'

client_socket.send('ok'.encode())

while moves_since_best_changed < 4:
        
    data = client_socket.recv(1024).decode()
    # Split the data into its components
    best_value, moves_since_best_changed, best_changed_glob = data.split(', ') # best_value, moves_since_best_changed, best_changed_glob
    best_value = float(best_value)
    moves_since_best_changed = int(moves_since_best_changed)
    best_changed_glob = best_changed_glob == 'true'
    
    if best_changed_glob == False:
        moves_since_best_changed = moves_since_best_changed + 1
    else:
        moves_since_best_changed = 0
    
    print(f"Best Values so far: {best_value}")

    client_socket.send(('ok ' + str(moves_since_best_changed)).encode())

data = client_socket.recv(1024).decode()
client_socket.send('end'.encode()) 
print("End message sent!")


