import socket
import time

import threading
import select
import json

from threading import Thread

class Port:
  BROADCAST_PORT = 2899
  RECEIVE_PORT = 2843


class FindRemoteIP:
    def __init__(self):
        self.json_road_limit = None
        self.active = 0
        self.remote_addr = None
        self.last_exception = None

        self.lock = threading.Lock()

        broadcast = Thread(target=self.udp_server_find_remote_ip, args=[])
        broadcast.daemon = True
        broadcast.start()


    def udp_server_find_remote_ip(self):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server_address = ('0.0.0.0', Port.BROADCAST_PORT)  # 서버 주소와 포트 번호
        server_socket.bind( server_address )
  
        print('UDP Server is listening on {}:{}'.format(*server_address))
        while True:
            data, self.remote_addr = server_socket.recvfrom(1024)
            print('Received message from {}: {}'.format( self.remote_addr, data.decode()))
            server_socket.sendto( 'echo'.encode(), self.remote_addr )
  

    def udp_recv(self, sock):
        ret = False
        try:
            ready = select.select([sock], [], [], 1.)
            ret = bool(ready[0])
            if ret:
                data, self.remote_addr = sock.recvfrom(2048)
                print(f"udp_recv={data}")                
                json_obj = json.loads(data.decode())
                print(f"json={json_obj}")  
 
                if 'cmd' in json_obj:
                    try:
                        #os.system(json_obj['cmd'])
                        ret = False
                    except:
                        pass

                if 'echo' in json_obj:
                    try:
                        echo = json.dumps(json_obj["echo"])
                        sock.sendto(echo.encode(), self.remote_addr ) # (self.remote_addr[0], Port.BROADCAST_PORT))
                        ret = False
                    except:
                        pass


        except:
            try:
                self.lock.acquire()
                self.json_road_limit = None
            finally:
                self.lock.release()

        return ret


def main():
    server = FindRemoteIP() 
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        try:
            sock.bind(('0.0.0.0', Port.RECEIVE_PORT))
            sock.setblocking(False)
            while True:
                server.udp_recv(sock)

        except Exception as e:
            print(f"An error occurred: {e}")
            server.last_exception = e


if __name__ == "__main__":
  main()

